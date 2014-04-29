////////////////////////////////////////////////////////////
//
//    Created:   April 2014
//    Copyright: CCP 2014
//
#include "StdAfx.h"

#include "EveCustomMask.h"

// --------------------------------------------------------------------------------
// Description:
//   Initialize data members
// --------------------------------------------------------------------------------
EveCustomMask::EveCustomMask( IRoot* lockobj ) :
	m_position( 0.f, 0.f, 0.f ),
	m_scaling( 1.f, 1.f, 1.f ),
	m_rotation( 0.f, 0.f, 0.f, 1.f ),
	m_isMirrored( false ),
	m_isForMaskMap( false ),
	m_isForSubmaskMap( false ),
	m_isAdditive( false )
{
}

// --------------------------------------------------------------------------------
// Description:
//   Destruction!
// --------------------------------------------------------------------------------
EveCustomMask::~EveCustomMask()
{
}

// --------------------------------------------------------------------------------
// Description:
//   Return the matrix used to render the debug boudning box if the projection
// --------------------------------------------------------------------------------
void EveCustomMask::GetDebugDrawMatrix( Matrix* matrix, float objectRadius ) const
{
	// scaling includes size!
	Vector3 finalScale( 0.1f * objectRadius, m_scaling.y * objectRadius, m_scaling.z * objectRadius );

	// build matrix
	D3DXMatrixTransformation( matrix, NULL, NULL, &finalScale, NULL, &m_rotation, &m_position );
}

// --------------------------------------------------------------------------------
// Description:
//   Return the inv matrix used put worldpos into projection space
// --------------------------------------------------------------------------------
void EveCustomMask::GetInvCustomMaskTransform( Matrix* matrix ) const
{
	Matrix customMaskTransform, invCustomMaskTransform;
	D3DXMatrixTransformation( &customMaskTransform, NULL, NULL, &m_scaling, NULL, &m_rotation, &m_position );
	D3DXMatrixInverse( &invCustomMaskTransform, NULL, &customMaskTransform );
	D3DXMatrixTranspose( matrix, &invCustomMaskTransform );
}

// --------------------------------------------------------------------------------
// Description:
//   Mirrors the mask at left/right, etc. all in one vector
// --------------------------------------------------------------------------------
void EveCustomMask::GetExtendedData( Vector4* data ) const
{
	// some additional features
	*data = Vector4(
		m_isMirrored ? 1.f : 0.f,
		m_isForMaskMap ? 1.f : 0.f,
		m_isForSubmaskMap ? 1.f : 0.f,
		m_isAdditive ? 1.f : 0.f );
}



