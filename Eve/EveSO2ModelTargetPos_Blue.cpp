#include "StdAfx.h"
#include "EveSO2ModelTargetPos.h"

BLUE_DEFINE( EveSO2ModelTargetPos );

const Be::ClassInfo* EveSO2ModelTargetPos::ExposeToBlue()
{

	EXPOSURE_BEGIN(EveSO2ModelTargetPos,"A function that returns a target locator of an EveSpaceObject2 model")
		MAP_INTERFACE( ITriVectorFunction )

		MAP_ATTRIBUTE
		(  
			 "name",
			 m_name,
			 "na", 
			 Be::READWRITE | Be::PERSIST 
		 )

		 MAP_ATTRIBUTE
		 ( 
			 "target",
			 m_targetObject,
			 "na",
			 Be::READWRITE
		 )

		 MAP_ATTRIBUTE
		 ( 
			 "parent",
			 m_parentObject,
			 "na", 
			 Be::READWRITE
		 )

		 MAP_ATTRIBUTE
		 (  
			 "value",
			 m_value,
			 "na",
			 Be::READWRITE | Be::PERSIST
		 )

	EXPOSURE_END()

}