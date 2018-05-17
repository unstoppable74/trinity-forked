#include "StdAfx.h"
#include "Tr2ActionResetClipSphereCenter.h"
#include "Controllers/Tr2Controller.h"
#include "Eve/SpaceObject/EveMobile.h"


void Tr2ActionResetClipSphereCenter::Start( Tr2Controller& controller )
{
	if( EveMobilePtr owner = BlueCastPtr( controller.GetOwner() ) )
	{
		owner->ResetClipSphereCenter();
	}
}