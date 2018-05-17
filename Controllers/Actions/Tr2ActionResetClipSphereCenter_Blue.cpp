#include "StdAfx.h"
#include "Tr2ActionResetClipSphereCenter.h"


BLUE_DEFINE( Tr2ActionResetClipSphereCenter );

const Be::ClassInfo* Tr2ActionResetClipSphereCenter::ExposeToBlue()
{
	EXPOSURE_BEGIN( Tr2ActionResetClipSphereCenter, "" )
		MAP_INTERFACE( Tr2ActionResetClipSphereCenter )
		MAP_INTERFACE( ITr2ControllerAction )
	EXPOSURE_END()
}
