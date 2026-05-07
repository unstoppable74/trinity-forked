#include "StdAfx.h"
#include "EveBoosterSetItem.h"

BLUE_DEFINE( EveBoosterSetItem );

const Be::ClassInfo* EveBoosterSetItem::ExposeToBlue()
{
	EXPOSURE_BEGIN( EveBoosterSetItem, "" )
		MAP_INTERFACE( EveBoosterSetItem )
		MAP_ATTRIBUTE( "transform",     transform,     "Local transform of this booster exhaust point", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "functionality", functionality, "Booster behaviour flags",                        Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "atlasIndex0",   atlasIndex0,   "Shape map index (primary)",                      Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "atlasIndex1",   atlasIndex1,   "Shape map index (secondary)",                    Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "hasTrail",      hasTrail,      "Whether this booster contributes to trails",     Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "lightScale",    lightScale,    "Scaling factor for dynamic light radius",        Be::READWRITE | Be::PERSIST )
	EXPOSURE_END()
}

