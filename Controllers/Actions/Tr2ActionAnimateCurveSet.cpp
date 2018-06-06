////////////////////////////////////////////////////////////
//
//    Created:   May 2018
//    Copyright: CCP 2018
//

#include "StdAfx.h"
#include "Tr2ActionAnimateCurveSet.h"
#include "Controllers/Tr2Controller.h"
#include "Controllers/Tr2ControllerFloatVariable.h"
#include "Curves/TriCurveSet.h"
#include "Tr2ExpressionTermInfo.h"


namespace
{
	float s_stateTime = 0;

	float StateTime()
	{
		return s_stateTime;
	}

	void ModifyParser( mu::Parser& parser )
	{
		parser.DefineFun( "StateTime", StateTime );
	}
}


Tr2ActionAnimateCurveSet::Tr2ActionAnimateCurveSet( IRoot* )
	:m_controller( nullptr ),
	m_value( "StateTime()" ),
	m_startTime( 0 )
{
}

void Tr2ActionAnimateCurveSet::Link( Tr2Controller& controller )
{
	m_controller = &controller;
	std::unordered_map<std::string, IRoot*> roots;
	controller.GetBindingPathRoots( roots );
	m_evaluator.SetExpr( m_value.c_str(), controller, ModifyParser );
}

void Tr2ActionAnimateCurveSet::Unlink()
{
	m_controller = nullptr;
	m_evaluator.Clear();
}

void Tr2ActionAnimateCurveSet::Start( Tr2Controller& controller )
{
	if( !m_curveSet )
	{
		return;
	}
	m_startTime = BeOS->GetCurrentFrameTime();
	controller.RegisterUpdateable( *this );
}

void Tr2ActionAnimateCurveSet::Stop( Tr2Controller& controller )
{
	controller.UnRegisterUpdateable( *this );
}

void Tr2ActionAnimateCurveSet::Update( Be::Time realTime, Be::Time simTime )
{
	if( !m_curveSet )
	{
		return;
	}
	s_stateTime = TimeAsFloat( simTime - m_startTime );
	auto value = m_evaluator.Eval();
	if( value.first )
	{
		m_curveSet->ApplyTime( value.second );
	}
}

bool Tr2ActionAnimateCurveSet::OnModified( Be::Var* value )
{
	if( !m_controller )
	{
		return true;
	}
	if( IsMatch( value, m_value ) )
	{
		m_evaluator.SetExpr( m_value.c_str(), *m_controller, ModifyParser );
	}
	return true;
}

bool Tr2ActionAnimateCurveSet::IsExpressionValid() const
{
	return m_evaluator.IsExpressionValid();
}

bool Tr2ActionAnimateCurveSet::IsAttrExpressionValid( const char* ) const
{
	return IsExpressionValid();
}

std::vector<Tr2ExpressionTermInfoPtr> Tr2ActionAnimateCurveSet::GetExpressionTermInfo() const
{
	std::vector<Tr2ExpressionTermInfoPtr> result;
	m_evaluator.GetExpressionTermInfo( result );

	if( m_controller )
	{
		auto& variables = m_controller->GetVariables();
		for( auto it = begin( variables ); it != end( variables ); ++it )
		{
			result.push_back( Tr2ExpressionTermInfo::Variable( "Variables", ( *it )->GetName().c_str(), "controller variable" ) );
		}
	}
	return result;
}