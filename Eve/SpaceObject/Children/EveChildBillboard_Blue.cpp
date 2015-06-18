////////////////////////////////////////////////////////////
//
//    Created:   2015
//    Copyright: CCP 2015
//
#include "StdAfx.h"
#include "EveChildBillboard.h"

BLUE_DEFINE( EveChildBillboard );

const Be::ClassInfo* EveChildBillboard::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveChildBillboard, "" )
        MAP_INTERFACE( EveChildBillboard )
		MAP_INTERFACE( IEveSpaceObjectChild )

		MAP_ATTRIBUTE( "name", m_name, "", Be::READWRITE | Be::PERSIST	)
		MAP_ATTRIBUTE( "display", m_display, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "translation", m_translation, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "scaling", m_scaling, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "mesh", m_mesh, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "transform", m_worldTransform, "", Be::READ )

    EXPOSURE_END()
}