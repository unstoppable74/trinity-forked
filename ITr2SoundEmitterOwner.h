////////////////////////////////////////////////////////////
//
//    Created:   October 2018
//    Copyright: CCP 2018
//

#pragma once


BLUE_DECLARE_INTERFACE( ITr2SoundEmitter );


BLUE_INTERFACE( ITr2SoundEmitterOwner ): public IRoot
{
	virtual ITr2SoundEmitter* FindSoundEmitter( const char* name ) = 0;
};
