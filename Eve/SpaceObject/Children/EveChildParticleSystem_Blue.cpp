#include "StdAfx.h"
#include "EveChildParticleSystem.h"

BLUE_DEFINE( EveChildParticleSystem );

const Be::ClassInfo* EveChildParticleSystem::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveChildParticleSystem, "" )
        MAP_INTERFACE( EveChildParticleSystem )
		MAP_INTERFACE( IEveSpaceObjectChild )

		MAP_ATTRIBUTE( "name", m_name, "", Be::READWRITE | Be::PERSIST	)
		MAP_ATTRIBUTE( "display", m_display, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "rotation", m_rotation, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "translation", m_translation, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "scaling", m_scaling, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "mesh", m_mesh, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "particleEmitters", m_particleEmitters, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "particleSystems", m_particleSystems, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "transform", m_worldTransform, "", Be::READ )

    EXPOSURE_END()
}