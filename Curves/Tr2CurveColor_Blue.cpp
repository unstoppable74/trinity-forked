////////////////////////////////////////////////////////////
//
//    Created:   May 2017
//    Copyright: CCP 2017
//

#include "StdAfx.h"
#include "Tr2CurveColor.h"

BLUE_DEFINE( Tr2CurveColor );

const Be::ClassInfo* Tr2CurveColor::ExposeToBlue()
{
	EXPOSURE_BEGIN( Tr2CurveColor, ":jessica-icon: tree/tricolor.png" )
		MAP_INTERFACE( Tr2CurveColor )
		MAP_INTERFACE( ITriFunction )
		MAP_INTERFACE( ITriCurveLength )

		MAP_ATTRIBUTE(
			"name",
			m_name,
			"",
			Be::READWRITE | Be::PERSIST )

		MAP_ATTRIBUTE(
			"srgbOutput",
			m_srgbOutput,
			"Convert resulting color value to sRGB",
			Be::READWRITE | Be::PERSIST )

		MAP_ATTRIBUTE(
			"r",
			m_r,
			"Red component curve",
			Be::READ | Be::PERSIST )
		MAP_ATTRIBUTE(
			"g",
			m_g,
			"Green component curve",
			Be::READ | Be::PERSIST )
		MAP_ATTRIBUTE(
			"b",
			m_b,
			"Blue component curve",
			Be::READ | Be::PERSIST )
		MAP_ATTRIBUTE(
			"a",
			m_a,
			"Alpha component curve",
			Be::READ | Be::PERSIST )

		MAP_ATTRIBUTE(
			"currentValue",
			m_currentValue,
			"Curve value after the last update",
			Be::READ )

		MAP_METHOD_AND_WRAP(
			"GetValueAt",
			GetValue,
			"Returns curve value at specified time\n"
			":param time: input time"
		)

		EXPOSURE_END()
}