////////////////////////////////////////////////////////////
//
//    Created:   May 2018
//    Copyright: CCP 2018
//

#pragma once

#include "ITr2ControllerAction.h"


BLUE_CLASS( Tr2ActionResetClipSphereCenter ) : public ITr2ControllerAction
{
public:
	EXPOSE_TO_BLUE();

	virtual void Start( Tr2Controller& controller );
};

TYPEDEF_BLUECLASS( Tr2ActionResetClipSphereCenter );
