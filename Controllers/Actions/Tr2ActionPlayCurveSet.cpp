////////////////////////////////////////////////////////////
//
//    Created:   March 2018
//    Copyright: CCP 2018
//

#include "StdAfx.h"
#include "Tr2ActionPlayCurveSet.h"
#include "Controllers/Tr2Controller.h"
#include "ITr2CurveSetOwner.h"
#include "Curves/TriCurveSet.h"

#include "Eve/SpaceObject/EveSpaceObject2.h"
#include "Eve/SpaceObject/Children/IEveSpaceObjectChild.h"
#include "Eve/EveEffectRoot2.h"


Tr2ActionPlayCurveSet::Tr2ActionPlayCurveSet( IRoot* )
{
}

void Tr2ActionPlayCurveSet::Start( Tr2Controller& controller )
{
	if( auto owner = dynamic_cast<ITr2CurveSetOwner*>( controller.GetOwner() ) )
	{
		owner->PlayCurveSet( m_curveSetName, m_rangeName );
	}
}

void Tr2ActionPlayCurveSet::Stop( Tr2Controller& controller )
{
	if( auto owner = dynamic_cast<ITr2CurveSetOwner*>( controller.GetOwner() ) )
	{
		owner->StopCurveSet( m_curveSetName );
	}
}
