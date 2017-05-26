////////////////////////////////////////////////////////////
//
//    Created:   May 2017
//    Copyright: CCP 2017
//

#pragma once

#include "Include/ITriFunction.h"
#include "Include/ITriCurveLength.h"
#include "Tr2CurveScalar.h"


BLUE_CLASS( Tr2CurveColor ) :
	public ITriCurveLength,
	public ITriFunction
{
public:
	Tr2CurveColor( IRoot* lockobj = nullptr );

	EXPOSE_TO_BLUE();

	virtual void UpdateValue( double time );

	virtual float Length();

	Color GetValue( double time ) const;
private:
	std::string m_name;

	PTr2CurveScalar m_r;
	PTr2CurveScalar m_g;
	PTr2CurveScalar m_b;
	PTr2CurveScalar m_a;

	Color m_currentValue;

	bool m_srgbOutput;
};

TYPEDEF_BLUECLASS( Tr2CurveColor );