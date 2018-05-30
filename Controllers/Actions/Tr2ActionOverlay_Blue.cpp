////////////////////////////////////////////////////////////
//
//    Created:   March 2018
//    Copyright: CCP 2018
//

#include "StdAfx.h"
#include "Tr2ActionOverlay.h"


BLUE_DEFINE( Tr2ActionOverlay );

const Be::ClassInfo* Tr2ActionOverlay::ExposeToBlue()
{
	EXPOSURE_BEGIN( Tr2ActionOverlay, "" )
		MAP_INTERFACE( Tr2ActionOverlay )
		MAP_INTERFACE( ITr2ControllerAction )

		MAP_ATTRIBUTE( 
			"path", 
			m_path, 
			"Overlay .red file path\n"
			":jessica-widget: filepath\n"
			":jessica-file-filter : redfile",
			Be::READWRITE | Be::PERSIST )
	EXPOSURE_END()
}
