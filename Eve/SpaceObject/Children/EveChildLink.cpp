////////////////////////////////////////////////////////////
//
//    Created:   September 2015
//    Copyright: CCP 2015
//

#include "StdAfx.h"
#include "EveChildLink.h"

#include "include/TriMath.h"
#include "TriValueBinding.h"
#include "Eve/EveUpdateContext.h"

// --------------------------------------------------------------------------------
// Description:
//   Initialize data members
// --------------------------------------------------------------------------------
EveChildLink::EveChildLink( IRoot* lockobj ) :
	EveChildMesh( lockobj ),
	PARENTLOCK( m_linkStrengthCurves ),
	PARENTLOCK( m_linkStrengthBindings ),
	m_currentDirection( 0.f, 0.f, 1.f ),
	m_currentDistance( 0.f ),
	m_linkStrength( 0.f ),
	m_linkBarrier( 0.f ),
	m_linkBarrierZone( 1.f )
{
}

// --------------------------------------------------------------------------------
// Description:
//   Byebye
// --------------------------------------------------------------------------------
EveChildLink::~EveChildLink()
{
}

// --------------------------------------------------------------------------------
// Description:
//   Synchronous updates happen here
// --------------------------------------------------------------------------------
void EveChildLink::UpdateSyncronous( EveUpdateContext& updateContext, IEveSpaceObject2* spaceObjectParent, IEveSpaceObjectChild* childParent )
{
	EveChildMesh::UpdateSyncronous( updateContext, spaceObjectParent, childParent );

	// if we have a target, we can calc the direction
	if( m_target )
	{
		// target comes from attached ball
		Vector3 tgtPos;
		m_target->GetValueAt( &tgtPos, updateContext.GetTime() );
		// source is from parent
		Vector3 srcPos;

		if ( spaceObjectParent )
		{
			spaceObjectParent->GetCurrentModelCenterWorldPosition( srcPos );
		}
		else
		{
			return;
		}

		// and that gives the direcion and current distance
		m_currentDirection = tgtPos - srcPos;
		m_currentDistance = D3DXVec3Length( &m_currentDirection );

		D3DXVec3Normalize( &m_currentDirection, &m_currentDirection );
	}
}

// --------------------------------------------------------------------------------
// Description:
//   Asynchronous updates happen here
// --------------------------------------------------------------------------------
void EveChildLink::UpdateAsyncronous( EveUpdateContext& updateContext, IEveSpaceObject2* spaceObjectParent, IEveSpaceObjectChild* childParent )
{
	// update the special link curves
	for( ITriFunctionVector::const_iterator it = m_linkStrengthCurves.begin(); it != m_linkStrengthCurves.end(); ++it )
	{
		(*it)->UpdateValue( m_linkStrength );
	}

	for( ITr2ValueBindingVector::const_iterator it = m_linkStrengthBindings.begin(); it != m_linkStrengthBindings.end(); ++it )
	{
		(*it)->CopyValue();
	}

	// get parent worldmatrix
	if( spaceObjectParent )
	{
		spaceObjectParent->GetLocalToWorldTransform( m_worldTransform );
	}
	else if ( childParent )
	{
		childParent->GetLocalToWorldTransform( m_worldTransform );
	}
	else
	{
		return;
	}

	// link rotation comes from direction
	Vector3 linkMeshDir( 0.f, 1.f, 0.f );
	Matrix linkRotationMat;
	TriMatrixRotationArc( &linkRotationMat, &linkMeshDir, &m_currentDirection );

	// link strength comes from distance vs. barrier
	m_linkStrength = Clamp( ( m_linkBarrierZone - m_currentDistance + m_linkBarrier ) / ( 2.f * m_linkBarrierZone ), 0.f, 1.f );

	// need inverse rotation-only from worldmatrix
	Matrix invRotationWorldMat;
	TriMatrixRemoveTranslation( &invRotationWorldMat, &m_worldTransform );
	D3DXMatrixInverse( &invRotationWorldMat, nullptr, &invRotationWorldMat );

	// combine inverse to link matrix, so we can do the intersection calculation in axis-aligned space
	D3DXMatrixMultiply( &linkRotationMat, &linkRotationMat, &invRotationWorldMat );

	// update perobject data buffers
	m_perObjectDataVs.InvalidateBufferData();
	m_perObjectDataPs.InvalidateBufferData();
	
	if( spaceObjectParent )
	{
		spaceObjectParent->GetPerObjectStructs( m_vsData, m_psData );
	}
	m_vsData.worldTransformLast = linkRotationMat;
	D3DXMatrixTranspose( &m_vsData.worldTransform, &m_worldTransform );
	D3DXMatrixTranspose( &m_vsData.worldTransformLast, &linkRotationMat );
}

// --------------------------------------------------------------------------------
// Description:
//   Returns the Local to World transformation matrix
// --------------------------------------------------------------------------------
void EveChildLink::GetLocalToWorldTransform( Matrix& transform ) const
{
	transform = m_worldTransform;
}