////////////////////////////////////////////////////////////
//
//    Created:   January 2015
//    Copyright: CCP 2015
//

#pragma once
#ifndef Tr2PointLight_H
#define Tr2PointLight_H

BLUE_CLASS( Tr2PointLight ): public IRoot
{
public:
	EXPOSE_TO_BLUE();

	Tr2PointLight( IRoot* lockobj = nullptr );

	Vector3 m_position;
	float m_radius;
	Color m_color;
	std::string m_name;
};

TYPEDEF_BLUECLASS( Tr2PointLight );

#endif