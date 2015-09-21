////////////////////////////////////////////////////////////
//
//    Created:   September 2015
//    Copyright: CCP 2015
//

#include "StdAfx.h"
#include "EveChildLink.h"

BLUE_DEFINE( EveChildLink );

const Be::ClassInfo* EveChildLink::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveChildLink, "" )
        MAP_INTERFACE( EveChildLink )
        MAP_INTERFACE( EveChildMesh )

		MAP_ATTRIBUTE( "currentDirection", m_currentDirection, "Current normalized direction to link source in worldspace", Be::READ )
		MAP_ATTRIBUTE( "currentDistance", m_currentDistance, "Current distance to link source", Be::READ )
		MAP_ATTRIBUTE( "target", m_target, "Link source destiny ball", Be::READWRITE )
		MAP_ATTRIBUTE( "linkStrength", m_linkStrength, "Normalized value indicating link strength", Be::READ )
		MAP_ATTRIBUTE( "linkBarrier", m_linkBarrier, "Absolute barrier radius", Be::READWRITE )
		MAP_ATTRIBUTE( "linkBarrierZone", m_linkBarrierZone, "Fuzzy radius offset for barrier", Be::READWRITE )
		MAP_ATTRIBUTE( "linkStrengthCurves", m_linkStrengthCurves, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "linkStrengthBindings", m_linkStrengthBindings, "", Be::READWRITE | Be::PERSIST )

    EXPOSURE_CHAINTO( EveChildMesh )
}