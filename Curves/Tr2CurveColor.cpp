////////////////////////////////////////////////////////////
//
//    Created:   May 2017
//    Copyright: CCP 2017
//

#include "StdAfx.h"
#include "Tr2CurveColor.h"
#include "Tr2CurveScalar.h"
#include "TriUtil.h"


// --------------------------------------------------------------------------------
Tr2CurveColor::Tr2CurveColor( IRoot* lockobj )
	:PARENTLOCK( m_r ),
	PARENTLOCK( m_g ),
	PARENTLOCK( m_b ),
	PARENTLOCK( m_a ),
	m_currentValue( 0, 0, 0, 1 ),
	m_srgbOutput( false )
{
}

// --------------------------------------------------------------------------------
void Tr2CurveColor::UpdateValue( double time )
{
	m_currentValue.r = m_r.Update( time );
	m_currentValue.g = m_g.Update( time );
	m_currentValue.b = m_b.Update( time );
	m_currentValue.a = m_a.Update( time );
	if( m_a.IsEmpty() )
	{
		m_currentValue.a = 1;
	}
	if( m_srgbOutput )
	{
		m_currentValue = TriLinearToGamma( m_currentValue );
	}
}

// --------------------------------------------------------------------------------
float Tr2CurveColor::Length()
{
	return std::max( std::max( std::max( m_r.Length(), m_g.Length() ), m_b.Length() ), m_a.Length() );
}

// --------------------------------------------------------------------------------
Color Tr2CurveColor::GetValue( double time ) const
{
	Color color( m_r.GetValue( time ), m_g.GetValue( time ), m_b.GetValue( time ), m_a.IsEmpty() ? 1.0f : m_a.GetValue( time ) );
	if( m_srgbOutput )
	{
		color = TriLinearToGamma( color );
	}
	return color;
}
