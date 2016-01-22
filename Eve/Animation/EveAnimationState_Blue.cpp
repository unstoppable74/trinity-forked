#include "StdAfx.h"
#include "EveAnimationState.h"
#include "Eve/SpaceObject/EveSpaceObject2.h"

const Be::VarChooser EveAnimationProgressChooser[] =
{
	// Name						Value								Docstring
	{ "EVE_ANIM_DONE",			BeCast( EVE_ANIM_DONE ),			"Animation state done" }, 
	{ "EVE_ANIM_INACTIVE",		BeCast( EVE_ANIM_INACTIVE ),		"Animation state inactive(no actions taken yet)" }, 
	{ "EVE_ANIM_FINALIZING",	BeCast( EVE_ANIM_FINALIZING ),		"Animation is finishing and waiting to be replaced" }, 
	{ "EVE_ANIM_RUNNING",		BeCast( EVE_ANIM_RUNNING ),			"Animation state active, i.e. running it's main animations" }, 
	{0}
};

BLUE_REGISTER_ENUM( "EVE_ANIMATION_STATE_PROGRESS", EveAnimationStateProgress, EveAnimationProgressChooser );

BLUE_DEFINE( EveAnimationState );

const Be::ClassInfo* EveAnimationState::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveAnimationState, "" )
        MAP_INTERFACE( EveAnimationState )
        MAP_INTERFACE( IRoot )

		MAP_ATTRIBUTE( "name", m_name, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE_WITH_CHOOSER( "progress", m_progress, "", Be::READ, EveAnimationProgressChooser )
		MAP_ATTRIBUTE( "secondsLeft", m_secondsRemaining, "", Be::READ )

		MAP_ATTRIBUTE( "animation", m_animation, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "curves", m_curves, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "overlayPath", m_overlayPath, "Path to the overlay effect that is applied to the owner for the duration of the state", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "commands", m_commands, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "initCurves", m_initCurves, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "initCommands", m_initCommands, "", Be::READWRITE | Be::PERSIST )
		
		MAP_ATTRIBUTE( "transitions", m_transitions, "", Be::READWRITE | Be::PERSIST )
    EXPOSURE_END()
}

