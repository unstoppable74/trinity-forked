////////////////////////////////////////////////////////////
//
//    Created:   October 2018
//    Copyright: CCP 2018
//

#include "StdAfx.h"
#include "Tr2ActionPlaySound.h"


BLUE_DEFINE_INTERFACE( ITr2SoundEmitter );

BLUE_DEFINE( Tr2ActionPlaySound );


const Be::ClassInfo* Tr2ActionPlaySound::ExposeToBlue()
{
	EXPOSURE_BEGIN( Tr2ActionPlaySound, "" )
		MAP_INTERFACE( Tr2ActionPlaySound )
		MAP_INTERFACE( ITr2ControllerAction )
		MAP_ATTRIBUTE( "emitter", m_emitterName, "Emitter name", Be::READWRITE | Be::PERSIST );
		MAP_ATTRIBUTE( "event", m_soundEvent, "Sound event name", Be::READWRITE | Be::PERSIST );
	EXPOSURE_END()
}
