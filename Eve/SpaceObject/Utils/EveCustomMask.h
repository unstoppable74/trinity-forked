////////////////////////////////////////////////////////////
//
//    Created:   April 2014
//    Copyright: CCP 2014
//
#pragma once
#ifndef EveCustomMask_H
#define EveCustomMask_H


// forwards

// --------------------------------------------------------------------------------
// Description:
//   This class holds data about custom masks for spaceobjects
// SeeAlso:
//   EveSpaceObject2
// --------------------------------------------------------------------------------
BLUE_CLASS( EveCustomMask ) :
	public IRoot
{
public:
	EXPOSE_TO_BLUE();

	EveCustomMask( IRoot* lockobj = NULL );
	~EveCustomMask();

	// access
	void GetDebugDrawMatrix( Matrix* matrix, float objectRadius ) const;
	void GetInvCustomMaskTransform( Matrix* matrix ) const;
	void GetExtendedData( Vector4* data ) const;


private:
	/////////////////////////////////////////////////////////////////////////////////////
	// mask projection data
	Vector3 m_position;
	Vector3 m_scaling;
	Quaternion m_rotation;

	// options
	bool m_isMirrored;
	bool m_isForMaskMap;
	bool m_isForSubmaskMap;
	bool m_isAdditive;
};

TYPEDEF_BLUECLASS( EveCustomMask );

#endif // EveCustomMask_H
