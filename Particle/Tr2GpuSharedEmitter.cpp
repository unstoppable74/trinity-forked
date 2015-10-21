////////////////////////////////////////////////////////////
//
//    Created:   October 2015
//    Copyright: CCP 2015
//

#include "StdAfx.h"
#include "Tr2GpuSharedEmitter.h"


Tr2GpuSharedEmitter::Tr2GpuSharedEmitter( IRoot* lockObj )
	:m_rate( 0 ),
	m_id( 0 ),
	m_paramsHash( 0 ),
	m_previousTime( -1 ),
	m_carryOver( 0 ),
	m_emissionDensity( 0.f ),
	m_inheritVelocity( 1.f ),
	m_position( 0.f, 0.f, 0.f ),
	m_direction( 0.f, 1.f, 0.f ),
	m_prevPosition( 0.f, 0.f, 0.f )
{
	memset( &m_emitter, 0, sizeof( m_emitter ) );
	memset( &m_params, 0, sizeof( m_params ) );
	m_params.turbulenceFrequency = 1;
}


bool Tr2GpuSharedEmitter::Initialize()
{
	UpdateHash();
	GenerateID();
	return true;
}

bool Tr2GpuSharedEmitter::OnModified( Be::Var* )
{
	UpdateHash();
	GenerateID();
	return true;
}

void Tr2GpuSharedEmitter::UpdateHash()
{
	m_paramsHash = CcpHashFNV1( &m_params, sizeof( m_params ) );
}

void Tr2GpuSharedEmitter::GenerateID()
{
	m_id = m_paramsHash & ~( 1 << ( sizeof( uintptr_t ) - 1 ) );
}

void Tr2GpuSharedEmitter::Update( Be::Time time, Tr2GpuParticleSystem& system, const Matrix& parentTransform, const Vector3& originShift )
{
	const bool firstUpdate = m_previousTime == Be::Time( -1 );
	const float dt = firstUpdate ? 0 : TimeAsFloat( time - m_previousTime );
	m_previousTime = time;

	m_emitter.position = XMVector3TransformCoord( m_position, parentTransform );

	float total = m_carryOver + dt * m_rate;
	if( m_emissionDensity > 0 && !firstUpdate )
	{
		Vector3 move = m_emitter.position - m_prevPosition;
		total += XMVectorGetX( XMVector3Length( move ) ) * m_emissionDensity;
	}
	m_emitter.count = int( total );
	m_carryOver = total - std::floor( total );

	if( m_emitter.count )
	{
		Vector3 velocity = Vector3( 0.f, 0.f, 0.f );
		if( firstUpdate )
		{
			m_emitter.positionPrevious = m_emitter.position;
		}
		else
		{
			velocity = ( m_emitter.position - m_prevPosition - originShift ) / dt;
			m_emitter.positionPrevious = m_prevPosition - originShift;
		}

		m_emitter.velocity = velocity * m_inheritVelocity;
		m_emitter.velocityPrevious = m_prevVelocity * m_inheritVelocity;
		m_prevVelocity = velocity;
		m_prevPosition = m_emitter.position;

		m_emitter.directionPrevious = m_emitter.direction;
		m_emitter.direction = XMVector3TransformNormal( m_direction, parentTransform );
		system.Emit( m_emitter, m_id, m_paramsHash, m_params );
	}
}
