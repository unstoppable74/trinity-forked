#pragma once
#ifndef ITr2Interior_H
#define ITr2Interior_H

#include "include/ITr2BoundingBox.h"
#include "ITr2Renderable.h"
#include "Tr2PerObjectData.h"
#include "Resources/TriTextureRes.h"
#include "Tr2AtlasTexture.h"

struct WodStencilBatchParams;
class TriFrustum;
class Tr2InteriorCell;
class Tr2InteriorLightSet;
struct Tr2InteriorPerObjectLightData;
struct Tr2PerFrameVSData;
struct Tr2PerFrameShadowPSData;
struct ITr2InteriorLight;
class Tr2RenderContext;
struct AxisAlignedBoundingBox;

enum CullResult
{
	CULLRES_OK = 1,
	CULLRES_NOTREADY,
	CULLRES_FAILED,
};

BLUE_DECLARE( Tr2ApexScene );

BLUE_INTERFACE( ITr2InteriorCullable ) : public IRoot
{
	virtual bool IsInFrustum( const TriFrustum& frustum, Matrix& objectToWorld ) const = 0;
};

BLUE_INTERFACE( ITr2Interior ) : public ITr2InteriorCullable
{
	// Per-object data for instanced lighting
	virtual Tr2PerObjectData* GetPerObjectDataWithPerInstanceLighting( 
		ITriRenderBatchAccumulator* accumulator,
		Tr2InteriorLightSet* lightSet, 
		const Matrix& objectToWorldMatrix, 
		const Matrix& mirrorToWorldMatrix 
	) = 0;
};
BLUE_DECLARE_IVECTOR( ITr2Interior );

BLUE_INTERFACE( ITr2InteriorDynamic ) : public ITr2Interior
{
	virtual bool GetWorldBoundingBox( Vector3& min, Vector3& max ) const = 0;
	virtual bool IsBoundingBoxReady( void ) const = 0;

	// Spherical harmonics update
	virtual void PrePhysicsUpdate( Be::Time time ) = 0;
	virtual void PostPhysicsUpdate( Be::Time time, Tr2ApexScene* apexScene ) = 0;

	// Scene add/remove
	virtual bool AddToScene( Tr2ApexScene* apexScene ) = 0;
	virtual void RemoveFromScene( void ) = 0;

	virtual bool TestCellIntersectionAndAdd( Tr2InteriorCell* cell ) = 0;
	virtual bool IsDirty( void ) const = 0;
	// Set the dirty flag
	virtual void SetDirtyFlag( bool isDirty ) = 0;
	virtual bool IsShadowCaster( void ) const = 0;

	// Some 'dynamic' placeables are actually placed by level designers and do not move or change
	// We allow these to be treated as statics from the point of view of shadows etc
	virtual bool IsStatic( void ) const { return false; }

	// LOD
	virtual void SetLOD( const TriFrustum* frustum ) = 0;
};
BLUE_DECLARE_IVECTOR( ITr2InteriorDynamic );

class TriTextureRes;

BLUE_DECLARE( Tr2AtlasTexture );

// -------------------------------------------------------------
// Description:
//   ITr2InteriorLight represents light source for interior 
//   scene.
// -------------------------------------------------------------
BLUE_INTERFACE( ITr2InteriorLight ) : public ITr2InteriorCullable
{

	// Helper structure for determining shadow caster importance
	// To Do: Rename this guy & maybe unify it with the LightInstance structure
	struct LightSourceItem
	{
		// Pointer to the light source
		ITr2InteriorLight* lightSource;
		// Light importance
		float importance;
		// Shadow map index
		unsigned int shadowMapIndex;

		// Compare light importance for descending sort
		bool operator<( const LightSourceItem& other ) const
		{
			// for equal importance, sort by lightSource.  this way moving the
			// camera around a pair of lights keeps them in the same order.
			// Which is nice for expensive fx that do something with light0 only.
			if( importance > other.importance )
			{
				return true;
			}
			if( importance < other.importance )
			{
				return false;
			}
			return (size_t)lightSource < (size_t)other.lightSource;
		}
	};

	// -------------------------------------------------------------
	// Description:
	//   Returns axis aligned bounding box for light source in 
	//   world space.
	// -------------------------------------------------------------
	virtual const AxisAlignedBoundingBox& GetBoundingBox() const = 0;

	// -------------------------------------------------------------
	// Description:
	//   Returns if the light is dirty (moved/changed).
	// Return Value:
	//   true If light is dirty
	//   false Otherwise
	// -------------------------------------------------------------
	virtual bool IsDirty( void ) const = 0;

	// -------------------------------------------------------------
	// Description:
	//   Set the dirty flag
	// Arguments:
	//   isDirty - If light is dirty (has changed)
	// -------------------------------------------------------------
	virtual void SetDirtyFlag( bool isDirty ) = 0;

	// -------------------------------------------------------------
	// Description:
	//   Perform cell intersection test and add light to the cell.
	// Arguments:
	//   cell - Cell to add light to
	// -------------------------------------------------------------
	virtual bool TestCellIntersectionAndAdd( Tr2InteriorCell* cell ) = 0;

	// -------------------------------------------------------------
	// Description:
	//   Called whenever light is added to interior scene.
	// -------------------------------------------------------------
	virtual void AddToScene( void ) = 0;

	// -------------------------------------------------------------
	// Description:
	//   Called whenever light is removed from interior scene.
	// -------------------------------------------------------------
	virtual void RemoveFromScene( void ) = 0;

	// -------------------------------------------------------------
	// Description:
	//   Copy the light parameters into the per-object data.
	// Arguments:
	//   lightData - Per-object light data
	//   mirrorToWorldMatrix - Mirror to world space transform.
	// -------------------------------------------------------------
	virtual void PopulateLightData( Tr2InteriorPerObjectLightData* lightData, 
									const Matrix &mirrorToWorldMatrix ) const = 0;

	// -------------------------------------------------------------
	// Description:
	//   Per-frame update method.
	// Arguments:
	//   time - Current system time.
	// -------------------------------------------------------------
	virtual void Update( Be::Time time ) = 0;
};

BLUE_DECLARE_IVECTOR( ITr2InteriorLight );

class Tr2PerObjectDataPSBuffer;

#endif // ITr2Interior_H
