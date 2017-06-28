#include "StdAfx.h"

#include "Tr2InteriorLightSource.h"
#include "Tr2InteriorConstantBufferFormats.h"
#include "TriDebugResourceHelper.h"

#include "Utilities/BoundingSphere.h"
#include "Tr2AtlasTexture.h"
#include "Tr2InteriorCell.h"
#include "Tr2InteriorLightGeometryRenderBatch.h"
#include "Tr2KelvinColor.h"
#include "Shader/Tr2ShaderMaterial.h"
#include "Tr2ConstGeometry.h"
#include "Curves/TriCurveSet.h"
#include "Include/TriMath.h"
#include "Shader/Tr2Effect.h"
#include "TriViewport.h"

CCP_STATS_DECLARE( wodIntLightsAlive, "Trinity/Tr2IntLightsAlive", false, CST_COUNTER_LOW, 
				  "Count of Tr2InteriorLightSources alive" );

BLUE_DEFINE_INTERFACE( ITr2InteriorLight );

using namespace Tr2RenderContextEnum;

// --------------------------------------------------------------------------------------
// Description:
//   Tr2InteriorLightSource default constructor
// --------------------------------------------------------------------------------------
Tr2InteriorLightSource::Tr2InteriorLightSource( IRoot* lockobj ) :
	m_name(),
	m_position( 0.f, 0.f, 0.f ),
	m_radius( 1.f ),
	m_color( 1.f, 1.f, 1.f, 1.f ),
	m_falloff( 1.f ),
	m_specularIntensity( 1.f ),
	m_coneAlphaOuter( 180.f ),
	m_coneAlphaInner( 180.f ),
	m_coneDirection( 0.f, -1.f, 0.f ),
	m_primaryLighting( true ),
	m_affectTransparentObjects( true ),
	m_importanceScale( 1.0f ),
	m_importanceBias( 0.0f ),
	m_isDirty( true ),
	PARENTLOCK( m_curveSets )
{
	CCP_STATS_INC( wodIntLightsAlive );
	RebuildVolume();

	D3DXMatrixIdentity( &m_unitToWorldTransform );
	m_worldBoundingBox = AxisAlignedBoundingBox( Vector3( -1.f, -1.f, -1.f ), Vector3( 1.f, 1.f, 1.f ) );

	m_kelvinColor.CreateInstance();
	m_useKelvinColor = false;

	PrepareResources();
}

// --------------------------------------------------------------------------------------
// Description:
//   Tr2InteriorLightSource destructor.
// --------------------------------------------------------------------------------------
Tr2InteriorLightSource::~Tr2InteriorLightSource()
{
	CCP_STATS_DEC( wodIntLightsAlive );
}

// --------------------------------------------------------------------------------------
// Description:
//   Inherited from IInitialize interface.
// Return Value:
//   true always
// --------------------------------------------------------------------------------------
bool Tr2InteriorLightSource::Initialize()
{
	RebuildVolume();
	
	PrepareResources();

    return true;
}

// --------------------------------------------------------------------------------------
// Description:
//   Inherited from INotify interface.  Allows the light source to respond to parameter 
//   changes generated in Python.  If the light position changes, the regions of influence 
//   are updated with the new transform matrix & the light is flagged as 'dirty', forcing 
//   a new round of light-cell intersection tests on the next scene Update. The light is 
//   flagged as dirty.
// Arguments:
//   value - The Blue-exposed parameter that changed
// Return Value:
//   true always
// --------------------------------------------------------------------------------------
bool Tr2InteriorLightSource::OnModified( Be::Var* value )
{
	// Update the regions of influce if the position changes
	if( IsMatch( value, m_position ) )
	{
		m_worldBoundingBox = AxisAlignedBoundingBox( m_position - Vector3( m_radius, m_radius, m_radius ), m_position + Vector3( m_radius, m_radius, m_radius ) );

		// Mark the dirty flag
		m_isDirty = true;
	}
	else if( IsMatch( value, m_radius )			||
			 IsMatch( value, m_coneAlphaOuter ) ||
			 IsMatch( value, m_coneDirection ) )
	{
		m_worldBoundingBox = AxisAlignedBoundingBox( m_position - Vector3( m_radius, m_radius, m_radius ), m_position + Vector3( m_radius, m_radius, m_radius ) );

		// Rebuild the bounding volume
		RebuildVolume();

		// Mark the dirty flag
		m_isDirty = true;

		PrepareResources();
	}
	else if( IsMatch( value, m_primaryLighting ) )
	{
		PrepareResources();
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Description:
//   Implements IBlueAsyncResNotifyTarget interface. Re-binds shaders whenever projected
//   texture changes.
// Arguments:
//   p - Resource that changed (projected texture)
// --------------------------------------------------------------------------------------
void Tr2InteriorLightSource::RebuildCachedData( BlueAsyncRes* p )
{
}

// --------------------------------------------------------------------------------------
// Description:
//   Inherited from ITriDeviceResource interface. Invalidates light's geometry vertex 
//   declaration.
// --------------------------------------------------------------------------------------
void Tr2InteriorLightSource::ReleaseResources( TriStorage s )
{
}

// --------------------------------------------------------------------------------------
// Description:
//   Inherited from ITriDeviceResource interface. Re-creates light's geometry vertex 
//   declaration.
// --------------------------------------------------------------------------------------
bool Tr2InteriorLightSource::OnPrepareResources()
{
	return true;
}

// --------------------------------------------------------------------------------------
// Description:
//   Copies the lighting parameters into the per-object data.
// Arguments:
//   lightData - The per-object light data to populate
// --------------------------------------------------------------------------------------
void Tr2InteriorLightSource::PopulateLightData( Tr2InteriorPerObjectLightData* lightData, 
											   const Matrix& mirrorToWorldMatrix ) const
{
	Vector3 direction( XMVector3TransformNormal( m_coneDirection, mirrorToWorldMatrix ) );

	XMVECTOR det;
	XMMATRIX invTransMirrorMat = XMMatrixInverse( &det, XMMatrixTranspose( mirrorToWorldMatrix ) );
	Vector3 position( XMVector3TransformCoord( m_position, invTransMirrorMat ) );

	// just put this in struct
	lightData->position = position;
	lightData->radius = std::max( m_radius, 0.0f ); // when radius<0 the light is treated as box light in forward rendering
	if( m_useKelvinColor )
	{
		Color k = m_kelvinColor->AsRGB();
		lightData->color = Vector3( k.r, k.g, k.b );
	}
	else
	{
		lightData->color = Vector3( m_color.r, m_color.g, m_color.b );
	}

	lightData->color = TriGammaToLinear( lightData->color );

	lightData->pointLightFalloff = m_falloff;
	lightData->shadow0Influence = 0.f;
	lightData->shadow1Influence = 0.f;

	// Spot light values (always populate them, they are basicly pointlights with 
	// some non-default values...).  First apply some limits
	float innerAngle = m_coneAlphaInner;
	float outerAngle = m_coneAlphaOuter;
	if( innerAngle + 1.f > outerAngle )
	{
		innerAngle = outerAngle - 1.f;
	}
	// if this is not a spotlight, force full 360degrees sphere
	if( !IsSpotLight() )
	{
		outerAngle = innerAngle = 360.f;
	}

	lightData->coneCosAlphaOuter = cosf( XMConvertToRadians( outerAngle ) );
	lightData->coneCosAlphaInner = cosf( XMConvertToRadians( innerAngle ) );
	lightData->spotDirection = XMVector3Normalize( direction );
}

// --------------------------------------------------------------------------------------
// Description:
//   Gets the world-space axis aligned bounding box for the light
// --------------------------------------------------------------------------------------
const AxisAlignedBoundingBox& Tr2InteriorLightSource::GetBoundingBox() const
{
	return m_worldBoundingBox;
}

// --------------------------------------------------------------------------------------
// Description:
//   Tests the light's bounding volume against the bounding box of a cell.  If the light
//   intersects the cell, a ROI is created and the light is added to the cell.  If the
//   light does not intersect, the ROI (if it exists is destroyed) and the light is
//   removed from the cell.  The exact behavior of this function depends on the light
//   intersection type.  If the light has a non-empty explicit cell list, then the light
//   is not tested against the bounding box.
// Arguments:
//   cell - The cell to test against
// --------------------------------------------------------------------------------------
bool Tr2InteriorLightSource::TestCellIntersectionAndAdd( Tr2InteriorCell* cell )
{
	// Bail out if the cell is invalid
	if( cell == NULL )
	{
		// No cell, return no intersection
		return false;
	}

	bool intersects = cell->IsUnbounded();

	if( !intersects )
	{
		if( !IsSpotLight() )
		{
			intersects = cell->IntersectsSphere( m_position, m_radius );
		}
		else
		{
			intersects = cell->IntersectsOBB( m_collisionCenter + m_position, m_collisionExtents, m_collisionOrientation );
		}
	}

	if( intersects )
	{
		cell->AddLight( this );
	}
	else
	{
		// Remove the light from the cell
		cell->RemoveLight( this );
	}

	// Return the result of the intersection test
	return intersects;
}

// --------------------------------------------------------------------------------------
// Description:
//   Gets the importance of the light, given the current view position.  The view
//   importance is simply the ratio of the light radius and the distance to the viewer.
// Arguments:
//   viewerPos - The position of the viewpoint
// Return Value:
//   The view importance
// --------------------------------------------------------------------------------------
float Tr2InteriorLightSource::GetCurrentViewImportance( const Vector3& viewerPos ) const
{
	// importance is based on:
	// 1. dist from camera
	Vector3 dist = viewerPos - m_position;
	float distToViewer = D3DXVec3LengthSq( &dist );

	// put together the result
	float res = m_radius * m_radius / distToViewer;

	return res * m_importanceScale + m_importanceBias;
}

void Tr2InteriorLightSource::AddToScene( void )
{
	// Set the dirty flag
	m_isDirty = true;
}


void Tr2InteriorLightSource::RemoveFromScene( void )
{
}

// -------------------------------------------------------------
// Description:
//   Calculates the fraction of camera field of view occupied by
//   a set of points.
// Arguments:
//   vertices - Array of points
//   world - Local to world space transformation
// Return Value:
//   Fraction of camera field of view occupied by points
// -------------------------------------------------------------
template<unsigned int count>
float GetScreenSize( const Vector4* vertices, const Matrix& world )
{
	if( Tr2Renderer::GetFieldOfView() <= 0.0f )
	{
		return 0.0f;
	}

	Vector3 directions[count];
	D3DXVec3TransformCoordArray( directions, sizeof( Vector3 ), reinterpret_cast<const Vector3*>( vertices ), sizeof( Vector4 ), &world, count );
	for( unsigned int i = 0; i < count; ++i )
	{
		directions[i] -= Tr2Renderer::GetViewPosition();
		D3DXVec3Normalize( directions + i, directions + i );
	}
	float cosMaxAngle = 1.f;
	for( unsigned int i = 0; i < count; ++i )
	{
		for( unsigned int j = i + 1; j < count; ++j )
		{
			float cosAngle = D3DXVec3Dot( directions + i, directions + j );
			if( cosAngle < cosMaxAngle )
			{
				cosMaxAngle = cosAngle;
			}
		}
	}
	cosMaxAngle = min( max( cosMaxAngle, -1.f ), 1.f );
	return acos( cosMaxAngle ) / Tr2Renderer::GetFieldOfView();
}

// -------------------------------------------------------------
// Description:
//   Per-frame update method. Updates curve sets.
// Arguments:
//   time - Current system time.
// -------------------------------------------------------------
void Tr2InteriorLightSource::Update( Be::Time time )
{
	for( auto it = m_curveSets.cbegin(); it != m_curveSets.cend(); ++it )
	{
		( *it )->Update( TimeAsDouble( time ) );
	}
}

void Tr2InteriorLightSource::RebuildVolume( void )
{
	if( IsSpotLight() )
	{
		// direct:
		float size = sinf( m_coneAlphaOuter / 180.f * XM_PI ) * m_radius;

		Matrix obbRotMatrix, obbScaleMatrix, obbTransMatrix;
		TriMatrixArcFromForward( &obbRotMatrix, &m_coneDirection );
		D3DXMatrixTranslation( &obbTransMatrix, 0.f, 0.f, -0.5f * m_radius );
		D3DXMatrixScaling( &obbScaleMatrix, size, size, 0.5f * m_radius );

		// Now build some vectors & quats for our own collision detection routines
		// Since the center is in world coordinates, we must pre-multiply the direction of the light
		Vector3 centerOffset;
		D3DXVec3Normalize( &centerOffset, &m_coneDirection );
		centerOffset *= 0.5f * m_radius;
		m_collisionCenter = centerOffset;
		m_collisionExtents = Vector3( size, size, 0.5f * m_radius );

		D3DXQuaternionRotationMatrix( &m_collisionOrientation, &obbRotMatrix );
		D3DXQuaternionNormalize( &m_collisionOrientation, &m_collisionOrientation );
	}
}

bool Tr2InteriorLightSource::IsInFrustum( const TriFrustum& frustum, Matrix& objectToWorld ) const
{
	if( !m_primaryLighting )
	{
		return false;
	}
	D3DXMatrixTranslation( &objectToWorld, m_position.x, m_position.y, m_position.z );
	return frustum.IsBoxVisible( m_worldBoundingBox.m_min, m_worldBoundingBox.m_max );
}

