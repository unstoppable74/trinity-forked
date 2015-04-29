////////////////////////////////////////////////////////////
//
//    Created:   February 2015
//    Copyright: CCP 2015
//

#include "StdAfx.h"
#include "EveCloud.h"
#include "TriFrustum.h"
#include "TriRenderBatch.h"
#include "include/ITriFunction.h"
#include "Utilities/ViewDistanceInfo.h"
#include "Utilities/BoundingSphere.h"
#include "Utilities/BoundingBox.h"
#include "Tr2PerObjectData.h"
#include "EveCloudEditableVolume.h"
#include "../EveUpdateContext.h"

using namespace Tr2RenderContextEnum;

namespace
{

class EveCloudPerObjectData : public Tr2PerObjectData
{
public:
	virtual void SetPerObjectDataToDevice( Tr2ConstantBufferAL** buffers, unsigned constantTypeMask, Tr2RenderContext& renderContext ) const
	{
		static const unsigned mask = 
			( 1 << VERTEX_SHADER )					|
			SHADER_TYPE_EXISTS( COMPUTE_SHADER )	|
			SHADER_TYPE_EXISTS( GEOMETRY_SHADER )	|
			SHADER_TYPE_EXISTS( HULL_SHADER )		|
			SHADER_TYPE_EXISTS( DOMAIN_SHADER)		;
		FillAndSetConstants(	
			*buffers[Tr2RenderContextEnum::CBUFFER_FFE],
			&m_data, sizeof( m_data ),
			mask,
			Tr2Renderer::GetPerObjectVSStartRegister(),
			renderContext );
	}

	struct Data
	{
		Matrix m_world;
		Matrix m_worldView;
		Matrix m_worldViewInv;
		Matrix m_projectionInv;
		Vector4 m_nearPlaneLocal;
		Vector3 m_eyePosLocal;
		float m_screenDepth;
		Vector4 m_screenSize;
	} m_data;
};

void GetProjectedCubeBounds( AxisAlignedBoundingBox& box, const Matrix& worldViewProj )
{
	Vector4 corners[8];

	Vector3 min = Vector3( -0.5f, -0.5f, -0.5f );
	Vector3 max = Vector3( 0.5f, 0.5f, 0.5f );

	corners[0] = Vector4( min, 1.f );
	corners[1] = Vector4( min.x, min.y, max.z, 1.f );
	corners[2] = Vector4( max.x, min.y, min.z, 1.f );
	corners[3] = Vector4( max.x, min.y, max.z, 1.f );
	corners[4] = Vector4( max, 1.f );
	corners[5] = Vector4( max.x, max.y, min.z, 1.f );
	corners[6] = Vector4( min.x, max.y, max.z, 1.f );
	corners[7] = Vector4( min.x, max.y, min.z, 1.f );

	D3DXVec4TransformArray( corners, sizeof( Vector4 ), corners, sizeof( Vector4 ), &worldViewProj, 8 );

	for( int i = 0; i < 8; ++i )
	{
		if( corners[i].w < 0 )
		{
			corners[i].x = corners[i].x < 0 ? -1.f : 1.f;
			corners[i].y = corners[i].y < 0 ? -1.f : 1.f;
		}
		else
		{
			corners[i] /= corners[i].w;
			corners[i].x = std::min( std::max( corners[i].x, -1.f ), 1.f );
			corners[i].y = std::min( std::max( corners[i].y, -1.f ), 1.f );
		}
	}

	box = AxisAlignedBoundingBox( Vector3( &corners[0].x ), Vector3( &corners[0].x ) );
	for( int i = 1; i < 8; ++i )
	{
		box.IncludePoint( Vector3( &corners[i].x ) );
	}
}

}



EveCloud::EveCloud( IRoot* lockobj )
	:m_worldTransform( Tr2Renderer::GetIdentityTransform() ),
	m_scaling( 1.0f, 1.0f, 1.0f ),
	m_display( true ),
	m_declaration( Tr2EffectStateManager::UNINITIALIZED_DECLARATION ),
	m_preTesselationLevel( 32 )
{
	PrepareResources();
}

EveCloud::~EveCloud()
{
}

bool EveCloud::Initialize()
{
	ReleaseResources( TRISTORAGE_ALL );
	PrepareResources();
	return true;
}

bool EveCloud::OnModified( Be::Var* value )
{
	if( IsMatch( value, m_preTesselationLevel ) )
	{
		ReleaseResources( TRISTORAGE_ALL );
		PrepareResources();
	}
	return true;
}

void EveCloud::UpdateSyncronous( EveUpdateContext& updateContext )
{
	if( m_volume )
	{
		m_volume->Update( updateContext.GetTime() );
	}

	Quaternion rotation( 0.0f, 0.0f, 0.0f, 1.0f );
	Vector3 translation( 0.0f, 0.0f, 0.0f );

	if( m_ballPosition )
	{
		m_ballPosition->Update( &translation, updateContext.GetTime() );
	}
	if( m_ballRotation )
	{
		m_ballRotation->Update( &rotation, updateContext.GetTime() );
	}

	D3DXMatrixTransformation( &m_worldTransform, 0, 0, &m_scaling, 0, &rotation, &translation );
}

void EveCloud::UpdateAsyncronous( EveUpdateContext& updateContext )
{
	Vector3 min = Vector3( -0.5f, -0.5f, -0.5f );
	Vector3 max = Vector3( 0.5f, 0.5f, 0.5f );
	BoundingSphereFromBox( m_boundingSphere, min, max, &m_worldTransform );
}

void EveCloud::GetRenderables( const TriFrustum& frustum, std::vector<ITr2Renderable*>& renderables, const Matrix& parentTransform )
{
	if( !m_display || !frustum.IsSphereVisible( &m_boundingSphere ) )
	{
		return;
	}
	renderables.push_back( this );
}

bool EveCloud::GetBoundingSphere( Vector4& sphere, BoundingSphereQuery query ) const
{
	sphere = m_boundingSphere;
	return true;
}

void EveCloud::UpdateViewDistanceInfo( const TriFrustum& frustum, ViewDistanceInfo& viewDistance ) const
{
	if( !m_display )
	{
		return;
	}

	viewDistance.UpdateClipPlanes( m_boundingSphere, frustum );
}

void EveCloud::RenderDebugInfo( Tr2RenderContext& renderContext )
{
	if( m_display && m_volume )
	{
		m_volume->RenderDebugInfo( m_worldTransform, renderContext );
	}
}

void EveCloud::GetModelCenterWorldPosition( Vector3 &position, Be::Time t )
{
	position = m_worldTransform.GetTranslation();
}

void EveCloud::GetCurrentModelCenterWorldPosition( Vector3 &position ) 
{
	position = m_worldTransform.GetTranslation();
}

bool EveCloud::GetLocalBoundingBox( Vector3 &min, Vector3 &max ) 
{ 
	min = Vector3( -0.5f, -0.5f, -0.5f );
	max = Vector3( 0.5f, 0.5f, 0.5f );
	return true; 
}

void EveCloud::GetLocalToWorldTransform( Matrix &transform ) 
{ 
	transform = m_worldTransform;
}

bool EveCloud::HasTransparentBatches()
{
	return true;
}

float EveCloud::GetSortValue()
{
	Vector3 d = Tr2Renderer::GetViewPosition() - m_worldTransform.GetTranslation();
	float distance = D3DXVec3Length( &d );
	return distance;
}

void EveCloud::GetBatches( ITriRenderBatchAccumulator* batches, TriBatchType batchType, const Tr2PerObjectData* perObjectData )
{
	if( batchType != TRIBATCHTYPE_TRANSPARENT )
	{
		return;
	}
	TriForwardingBatch* batch = batches->Allocate<TriForwardingBatch>();
	if( batch )
	{
		batch->SetPerObjectData( perObjectData );
		batch->SetShaderMaterial( m_effect );
		batch->SetGeometryProvider( this );
		batch->SetRenderingMode( Tr2EffectStateManager::RM_ALPHA );
		batches->Commit( batch );
	}
}

void EveCloud::ReleaseResources( TriStorage s )
{
	m_declaration = Tr2EffectStateManager::UNINITIALIZED_DECLARATION;
	if( m_vertexBuffer.IsValid() && m_vertexBuffer.GetMemoryClass() & s )
	{
		m_vertexBuffer.Destroy();
	}
	if( m_indexBuffer.IsValid() && m_indexBuffer.GetMemoryClass() & s )
	{
		m_indexBuffer.Destroy();
	}
}

bool EveCloud::OnPrepareResources()
{
	USE_MAIN_THREAD_RENDER_CONTEXT();

	const uint32_t dimension = m_preTesselationLevel;

	if( !m_vertexBuffer.IsValid() )
	{
		std::vector<Vector2> data( ( dimension + 1 ) * ( dimension + 1 ) );
		for( uint32_t j = 0; j <= dimension; ++j )
		{
			for( uint32_t i = 0; i <= dimension; ++i )
			{
				data[i + j * ( dimension + 1 )] = Vector2( ( float( i ) - 0.5f + 0.5f * ( j % 2 ) ) / ( dimension - 0.5f ) * 2 - 1, ( float( j ) / dimension * 2 - 1 ) );
			}
		}
		CR_RETURN_VAL( m_vertexBuffer.Create( sizeof( Vector2 ) * uint32_t( data.size() ), USAGE_IMMUTABLE, &data.front(), renderContext ), false );
	}
	if( !m_indexBuffer.IsValid() )
	{
		std::vector<uint16_t> data( dimension * dimension * 6 );
		uint32_t index = 0;
		for( uint32_t j = 0; j < dimension; ++j )
		{
			for( uint32_t i = 0; i < dimension; ++i )
			{
				if( j % 2 )
				{
					data[index++] = i + j * ( dimension + 1 );
					data[index++] = i + 1 + j * ( dimension + 1 );
					data[index++] = i + 1 + ( j + 1 ) * ( dimension + 1 );
					data[index++] = i + 1 + ( j + 1 ) * ( dimension + 1 );
					data[index++] = i + ( j + 1 ) * ( dimension + 1 );
					data[index++] = i + j * ( dimension + 1 );
				}
				else
				{
					data[index++] = i + j * ( dimension + 1 );
					data[index++] = i + 1 + j * ( dimension + 1 );
					data[index++] = i + ( j + 1 ) * ( dimension + 1 );
					data[index++] = i + ( j + 1 ) * ( dimension + 1 );
					data[index++] = i + 1 + j * ( dimension + 1 );
					data[index++] = i + 1 + ( j + 1 ) * ( dimension + 1 );
				}
			}
		}
		CR_RETURN_VAL( m_indexBuffer.Create( uint32_t( data.size() ), USAGE_IMMUTABLE, IB_16BIT, &data.front(), renderContext ), false );
	}
	if( m_declaration == Tr2EffectStateManager::UNINITIALIZED_DECLARATION )
	{
		static Tr2VertexDefinition s_decl;
		if( s_decl.empty() )
		{
			s_decl.Add( s_decl.FLOAT32_2, s_decl.POSITION );
		}

		m_declaration = renderContext.m_esm.GetVertexDeclarationHandle( s_decl );
	}
	return true;
}

Tr2PerObjectData* EveCloud::GetPerObjectData( ITriRenderBatchAccumulator* accumulator )
{
	EveCloudPerObjectData* data = accumulator->Allocate<EveCloudPerObjectData>();

	if( !data )
	{
		return NULL;
	}

	D3DXMatrixTranspose( &data->m_data.m_world, &m_worldTransform );

	Matrix worldView = m_worldTransform * Tr2Renderer::GetViewTransform();
	D3DXMatrixTranspose( &data->m_data.m_worldView, &worldView );

	Matrix worldViewProjection = m_worldTransform * Tr2Renderer::GetViewTransform() * Tr2Renderer::GetProjectionTransform();

	Matrix worldViewTransposed;
	D3DXMatrixTranspose( &worldViewTransposed, &worldView );
	Vector4 nearPlane( 0.f, 0.f, -1.f, -Tr2Renderer::GetFrontClip() );
	D3DXVec4Transform( &data->m_data.m_nearPlaneLocal, &nearPlane, &worldViewTransposed );

	Matrix worldViewInv;
	D3DXMatrixInverse( &worldViewInv, nullptr, &worldView );
	D3DXMatrixTranspose( &data->m_data.m_worldViewInv, &worldViewInv );
	Vector3 zero( 0.f, 0.f, 0.f );
	D3DXVec3TransformCoord( &data->m_data.m_eyePosLocal, &zero, &worldViewInv );

	Matrix projectionInv;
	D3DXMatrixInverse( &projectionInv, nullptr, &Tr2Renderer::GetProjectionTransform() );
	D3DXMatrixTranspose( &data->m_data.m_projectionInv, &projectionInv );


	Vector3 center;
	D3DXVec3TransformCoord( &center, &zero, &worldViewProjection );
	data->m_data.m_screenDepth = center.z;


	AxisAlignedBoundingBox box;
	GetProjectedCubeBounds( box, worldViewProjection );

	data->m_data.m_screenSize.x = box.m_min.x;
	data->m_data.m_screenSize.y = box.m_min.y;
	data->m_data.m_screenSize.z = box.m_max.x;
	data->m_data.m_screenSize.w = box.m_max.y;

	return data;
}

void EveCloud::SubmitGeometry( Tr2RenderContext& renderContext )
{
	renderContext.m_esm.ApplyVertexDeclaration( m_declaration );
	renderContext.m_esm.ApplyIndexBuffer( m_indexBuffer );
	renderContext.m_esm.ApplyStreamSource( 0, m_vertexBuffer, 0, sizeof( Vector2 ) );
	renderContext.SetTopology( TOP_TRIANGLES );
	renderContext.DrawIndexedPrimitive( m_vertexBuffer.GetTotalSizeInBytes() / sizeof( Vector2 ), 0, m_indexBuffer.GetNumIndices() / 3 );
}