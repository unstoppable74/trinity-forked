#include "StdAfx.h"
#include "SeekTarget.h"

BLUE_DEFINE( SeekTarget );

const Be::ClassInfo* SeekTarget::ExposeToBlue()
{
	EXPOSURE_BEGIN( SeekTarget, "" )
		MAP_INTERFACE( SeekTarget )
		MAP_INTERFACE( IBehavior )

		MAP_ATTRIBUTE( "locatorSet", m_locatorSets, "Set of Blue structure lists of locators identified by a name", Be::READ | Be::PERSIST )
		MAP_ATTRIBUTE( "behaviorWeight", m_behaviorWeight, ":jessica-group: SeekTarget", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "arrivedRadius", m_arrivedRadius, ":jessica-group: SeekTarget", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "slowDownRadius", m_slowDownRadius, ":jessica-group: SeekTarget", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "fxBehavior", m_fxBehavior, ":jessica-group: SeekTarget", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "target", m_target, ":jessica-group: SeekTarget", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "onFirstDroneArrivedCallback", m_onFirstDroneArrivedCallback, "A callback that is called when the first drone arrives", Be::READ )
		MAP_ATTRIBUTE( "exit", m_exit, ":jessica-group: SeekTarget", Be::READWRITE )

		MAP_METHOD_AND_WRAP(
			"SetTarget",
			SetTarget,
			"Assigns target.\n"
			":param transforms: target" )

		MAP_METHOD_AND_WRAP(
			"SetExit",
			SetExit,
			"Set exit value.\n"
			":param transforms: target" )

		MAP_METHOD_AND_WRAP(
			"SetBehaviorWeight",
			SetBehaviorWeight,
			"Set behavior weight.\n"
			":param transforms: target" )

		MAP_METHOD_AND_WRAP(
			"ResetBehavior",
			ResetBehavior,
			"Reset seek target and play fx behavior for when players repair ship again.\n"
			":param transforms: target" )

		MAP_METHOD_AND_WRAP(
			"SplitBoundingBox",
			SplitBoundingBox,
			"Splits bounding box into sub boxes if ship is large enough.\n"
			":param transforms: target" )

		EXPOSURE_END()
}