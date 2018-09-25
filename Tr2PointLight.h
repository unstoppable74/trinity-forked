////////////////////////////////////////////////////////////
//
//    Created:   January 2015
//    Copyright: CCP 2015
//

#pragma once
#ifndef Tr2PointLight_H
#define Tr2PointLight_H

class Tr2LightManager;

BLUE_CLASS( Tr2PointLight ): public IRoot
{
public:
	EXPOSE_TO_BLUE();

	Tr2PointLight( IRoot* lockobj = nullptr );
	void AddLight( Tr2LightManager& lightManager, CXMMATRIX transform, float scale );
	void GetLight( Vector3& position, float& radius, Color& color );

	Vector3 m_position;
	float m_radius;
	float m_innerRadius;
	Color m_color;
	float m_brightness;
	float m_noiseAmplitude;
	float m_noiseFrequency;
	uint32_t m_noiseOctaves;
	Be::Time m_startTime;
	std::string m_name;
};

TYPEDEF_BLUECLASS( Tr2PointLight );

#endif