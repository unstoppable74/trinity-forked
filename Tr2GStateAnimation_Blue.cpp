#include "StdAfx.h"
#include "Tr2GStateAnimation.h"
#include "Resources/TriGeometryRes.h"
#include "Resources/TriGrannyRes.h"
#include "Resources/Tr2GrannyStateRes.h"

BLUE_DEFINE( Tr2GStateAnimation );

const Be::ClassInfo* Tr2GStateAnimation::ExposeToBlue()
{
    EXPOSURE_BEGIN( Tr2GStateAnimation, "" )
        MAP_INTERFACE( Tr2GStateAnimation )
		MAP_INTERFACE( IInitialize )
		MAP_INTERFACE( ITr2AnimationUpdater )

		MAP_PROPERTY( "resPath", GetResPath, SetResPath, "The resource path to the Granny file containing the animations to be played." )
		MAP_ATTRIBUTE( "resPath_", m_resPath, "", Be::PERSISTONLY )
		MAP_PROPERTY("gStateResPath", GetGStateResPath, SetGStateResPath, "The resource path to a base layer GState file containing animations to be played.")
		MAP_ATTRIBUTE("gStateResPath_", m_gStateResPath, "", Be::PERSISTONLY)
		MAP_ATTRIBUTE( "grannyRes", m_grannyRes, "", Be::READ )
		MAP_PROPERTY( "model", GetModel, SetModel, "" )
		MAP_ATTRIBUTE( "model_", m_model, "", Be::PERSISTONLY )

		MAP_ATTRIBUTE
		( 
			"debugRenderSkeleton", m_debugRenderSkeleton, 
			"If set, and a debug renderer is set, then the skeleton is rendered with lines connecting\n"
			"the joints of the skeleton.",
			Be::READWRITE
		)

		MAP_ATTRIBUTE
		( 
			"debugRenderJointNames", m_debugRenderJointNames, 
			"If set, and a debug renderer is set, then the names of the joints in the skeleton\n"
			"are displayed at their projected locations.",
			Be::READWRITE
		)

		MAP_ATTRIBUTE
		( 
			"animationEnabled", m_animationEnabled, 
			"Enable/disable animation update",
			Be::READWRITE
		)

		MAP_METHOD_AND_WRAP
		(
			"EndAnimation",
			EndAnimation,
			"EndAnimation()\n\n"
			"Stops currently playing animation at the end of the current loop iteration."
		)
		MAP_METHOD_AND_WRAP
		(
			"ClearAnimations",
			ClearAnimations,
			"ClearAnimations()\n\n"
			"Abruptly ends all animations."
		)

		MAP_ATTRIBUTE( "boneOffset", m_boneOffset, "Per-bone post animation offsets.", Be::READ )

		MAP_ATTRIBUTE
		(
			"eventListener",
			m_eventListener,
			"An event listener that's triggered by granny text track events.",
			Be::READWRITE
		)

		MAP_METHOD_AND_WRAP
		(
			"TogglePauseAnimations",
			TogglePauseAnimations,
			"TogglePauseAnimations( pause )\n\n"
			"Pauses/unpauses animation playback."
			":param pause: boolean True for pause, boolean False to unpause"
		)

		MAP_METHOD_AND_WRAP
		(
			"LoadAnimResPath",
			LoadAnimResPath,
			"LoadAnimResPath( path )\n\n"
			"Preloads animation .gr2 resource file."
			":param string: resource path for animation file"
		)

		MAP_METHOD_AND_WRAP( "GetAnimationNames", GetAnimationNames, "Returns all animation names" )
	EXPOSURE_END()
}
