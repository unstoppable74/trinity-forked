////////////////////////////////////////////////////////////
//
//    Created:   April 2014
//    Copyright: CCP 2014
//
#include "StdAfx.h"
#include "EveCustomMask.h"

BLUE_DEFINE( EveCustomMask );
const Be::ClassInfo* EveCustomMask::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveCustomMask, "" )
        MAP_INTERFACE( EveCustomMask )

		MAP_ATTRIBUTE( "position", m_position, "data\n", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "scaling", m_scaling, "data\n", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "rotation", m_rotation, "data\n", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "isMirrored", m_isMirrored, "data\n", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "isForMaskMap", m_isForMaskMap, "data\n", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "isForSubmaskMap", m_isForSubmaskMap, "data\n", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "isAdditive", m_isAdditive, "data\n", Be::READWRITE | Be::PERSIST )

	EXPOSURE_END()
}
