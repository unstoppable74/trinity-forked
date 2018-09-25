////////////////////////////////////////////////////////////
//
//    Created:   January 2015
//    Copyright: CCP 2015
//

#include "StdAfx.h"
#include "Tr2PointLight.h"
#include "Tr2LightManager.h"
#include "Include/TriMath.h"

Tr2PointLight::Tr2PointLight( IRoot* lockobj )
	:m_position( 0.f, 0.f, 0.f ),
	m_radius( 0.f ),
	m_color( 0.f, 0.f, 0.f, 1.f ),
	m_brightness( 1.f ),
	m_noiseAmplitude( 0.f ),
	m_noiseFrequency( 1.f ),
	m_noiseOctaves( 1 ),
	m_innerRadius(0.f)
{
	m_startTime = BeOS->GetCurrentFrameTime();
}

void Tr2PointLight::AddLight( Tr2LightManager& lightManager, CXMMATRIX transform, float scale )
{
	float brightness = m_brightness;
	if( m_noiseAmplitude != 0.f )
	{
		float noise = float( PerlinNoise1D( TimeAsDouble( BeOS->GetCurrentFrameTime() - m_startTime ) * m_noiseFrequency, 2.f, 2.f, m_noiseOctaves ) );
		brightness *= ( ( noise + 1.0f ) / 2.0f ) * m_noiseAmplitude;
	}
	lightManager.AddPointLight( 
		Vector3( XMVector3TransformCoord( m_position, transform ) ), 
		m_radius * scale, 
		m_color * brightness, m_innerRadius );
}

void Tr2PointLight::GetLight( Vector3& position, float& radius, Color& color )
{
	position = m_position;
	radius = m_radius;
	float brightness = m_brightness;
	if( m_noiseAmplitude != 0.f )
	{
		float noise = float( PerlinNoise1D( TimeAsDouble( BeOS->GetCurrentFrameTime() ) * m_noiseFrequency, 2.f, 2.f, m_noiseOctaves ) );
		brightness *= ( ( noise + 1.0f ) / 2.0f ) * m_noiseAmplitude;
	}
	color = m_color * brightness;
}