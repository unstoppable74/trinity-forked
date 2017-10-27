#include "StdAfx.h"
#include "Tr2TextureReference.h"

BLUE_DEFINE( Tr2TextureReference );


const Be::ClassInfo* Tr2TextureReference::ExposeToBlue()
{
    EXPOSURE_BEGIN( Tr2TextureReference, "" )
        MAP_INTERFACE( Tr2TextureReference )
        MAP_INTERFACE( ITr2TextureProvider )
    EXPOSURE_END()
}



BLUE_DEFINE( Tr2TransientTextureReference );


const Be::ClassInfo* Tr2TransientTextureReference::ExposeToBlue()
{
	EXPOSURE_BEGIN( Tr2TransientTextureReference, "" )
		MAP_INTERFACE( Tr2TransientTextureReference )
		MAP_INTERFACE( ITr2TextureProvider )
	EXPOSURE_END()
}
