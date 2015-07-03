#include "StdAfx.h"
#include "Tr2PointLight.h"

BLUE_DEFINE( Tr2PointLight );

const Be::ClassInfo* Tr2PointLight::ExposeToBlue()
{
    EXPOSURE_BEGIN( Tr2PointLight, "" )
        MAP_INTERFACE( Tr2PointLight )

		MAP_ATTRIBUTE( "name", m_name, "Name so artists dont get confused", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "position", m_position, "Light position", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "radius", m_radius, "Light radius", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "color", m_color, "Light color (in linear space)", Be::READWRITE | Be::PERSIST )
    EXPOSURE_END()
}
