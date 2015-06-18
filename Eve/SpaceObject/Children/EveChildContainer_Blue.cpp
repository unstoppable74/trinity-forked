////////////////////////////////////////////////////////////
//
//    Created:   2015
//    Copyright: CCP 2015
//
#include "StdAfx.h"
#include "EveChildContainer.h"

BLUE_DEFINE( EveChildContainer );

const Be::ClassInfo* EveChildContainer::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveChildContainer, "" )
        MAP_INTERFACE( EveChildContainer )
		MAP_INTERFACE( IEveSpaceObjectChild )

		MAP_ATTRIBUTE( "name", m_name, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "objects", m_objects, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "curveSets", m_curveSets, "", Be::READWRITE | Be::PERSIST )
    EXPOSURE_END()
}