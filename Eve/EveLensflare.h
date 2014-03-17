////////////////////////////////////////////////////////////
//
//    Created:   June 2010
//    Copyright: CCP 2010
//
#pragma once
#ifndef EveLensflare_H
#define EveLensflare_H

#include "ITr2Renderable.h"
#include "Tr2Variable.h"

// forwards
class TriFrustum;
BLUE_DECLARE( EveOccluder );
BLUE_DECLARE_VECTOR( EveOccluder );
BLUE_DECLARE( EveTransform );
BLUE_DECLARE_VECTOR( EveTransform );
BLUE_DECLARE( TriVariable );
BLUE_DECLARE( TriCurveSet );
BLUE_DECLARE_VECTOR( TriCurveSet );
BLUE_DECLARE_INTERFACE( ITriVectorFunction );
BLUE_DECLARE_INTERFACE( ITriFunction );
BLUE_DECLARE_IVECTOR( ITriFunction );
BLUE_DECLARE_INTERFACE( ITr2ValueBinding );
BLUE_DECLARE_IVECTOR( ITr2ValueBinding );
BLUE_DECLARE( Tr2Mesh );

// --------------------------------------------------------------------------------
// Description:
//   EveLensflare is for the sun's lensflares. A lensflare is
//   made of several 2D billboards (aka flares) on top of each
//   other and occluders to determine visibility.
//   Flares are just standard EveTransform with a special
//   transformation
// SeeAlso:
//   EveSpaceScene, EveOccluder, Tr2TransformModifier
// --------------------------------------------------------------------------------
class EveLensflare :
	public ITr2Renderable
{
public:
	EXPOSE_TO_BLUE();

	using IRoot::Lock;
	using IRoot::Unlock;

	EveLensflare(IRoot* lockobj = NULL);
	~EveLensflare();

	// timing
	void Update( Be::Time time );

	// prepare some data necessary for rendering
	void PrepareRender( const TriFrustum& frustum );

	// rendering
	void GetRenderables( const TriFrustum& frustum, std::vector<ITr2Renderable*>& renderables );

	// do the foreground occlusion rendering/querying
	void RunOcclusionQueries( Tr2RenderContext& renderContext, const TriFrustum& frustum );
	// do the background occlusion rendering/querying
	void RunBackgroundOcclusionQueries( Tr2RenderContext& renderContext, const TriFrustum& frustum );


	virtual void GetBatches( ITriRenderBatchAccumulator* batches, TriBatchType batchType, const Tr2PerObjectData* perObjectData );
    virtual bool HasTransparentBatches();
    virtual float GetSortValue(); 
	virtual Tr2PerObjectData* GetPerObjectData( ITriRenderBatchAccumulator* accumulator );

private:
	// name
	std::string m_name;
	// toggle display
	bool m_display;
	// toggle updates
	bool m_update;
	// toggle visibility queries
	bool m_doOcclusionQueries;

	// factor for pushing the sun away from the camera position
	float m_cameraFactor;
	// this transform rotates the 2d billboard geomtry into the correct orientation
	Matrix m_transform;

	// position
	Vector3 m_position;
	Vector3 m_direction;
	float m_sunSize;

	PITriFunctionVector m_distanceToEdgeCurves;
	PITriFunctionVector m_distanceToCenterCurves;
	PITriFunctionVector m_radialAngleCurves;
	PITriFunctionVector m_xDistanceToCenter;
	PITriFunctionVector m_yDistanceToCenter;

	PITr2ValueBindingVector m_bindings;

	// position function
	ITriVectorFunctionPtr m_translationCurve;

	// the actual vector of flares
	PEveTransformVector m_flares;

	// shader variable-store handle for direction of flare/sun
	Tr2Variable m_directionVar;
	// shader variable-store handle for overall lensflare intensity
	Tr2Variable m_occScaleVar;

	// foreground occluder modules for ships, stations, etc.
	PEveOccluderVector m_occluders;
	// background occluder modules for planets
	PEveOccluderVector m_backgroundOccluders;

	// the intensity based on occlusion results
	float m_occlusionIntensity;
    // the intensity based on background occlusion results
	float m_backgroundOcclusionIntensity;

	PTriCurveSetVector m_curveSets;

	Tr2MeshPtr m_mesh;
};

TYPEDEF_BLUECLASS( EveLensflare );
BLUE_DECLARE_VECTOR( EveLensflare );

#endif // EveLensflare_H
