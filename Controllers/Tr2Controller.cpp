////////////////////////////////////////////////////////////
//
//    Created:   March 2018
//    Copyright: CCP 2018
//

#include "StdAfx.h"
#include "Tr2Controller.h"
#include "Tr2StateMachine.h"
#include "Tr2ControllerFloatVariable.h"
#include "Tr2ControllerEventHandler.h"
#include "Include/ITr2Updateable.h"
#include "blue/Include/ScopedBlockTrap.h"


Tr2Controller::Tr2Controller( IRoot* lockobj )
	:PARENTLOCK( m_stateMachines ),
	PARENTLOCK( m_variables ),
	PARENTLOCK( m_eventHandlers ),
	m_updateables( "Tr2Controller::m_updateables" ),
	m_owner( nullptr ),
	m_isActive( false ),
	m_isShared( false )
{
	m_stateMachines.SetNotify( this );
	m_variables.SetNotify( this );
	m_eventHandlers.SetNotify( this );
}

void Tr2Controller::OnListModified( long event, ssize_t key, ssize_t key2, IRoot* value, const IList* list )
{
	if( list == &m_stateMachines )
	{
		switch( event & BELIST_EVENTMASK )
		{
		case BELIST_INSERTED:
			if( m_owner )
			{
				if( Tr2StateMachinePtr stateMachine = BlueCastPtr( value ) )
				{
					stateMachine->Link( *this );
					if( m_isActive )
					{
						stateMachine->Start();
					}
				}
			}
			break;
		case BELIST_REMOVED:
			if( Tr2StateMachinePtr stateMachine = BlueCastPtr( value ) )
			{
				if( m_isActive )
				{
					stateMachine->Stop();
				}
				stateMachine->Unlink();
			}
			break;
		default:
			break;
		}
	}
	else if( list == &m_eventHandlers )
	{
		switch( event & BELIST_EVENTMASK )
		{
		case BELIST_INSERTED:
			if( m_owner )
			{
				if( Tr2ControllerEventHandlerPtr handler = BlueCastPtr( value ) )
				{
					handler->Link( *this );
				}
			}
			break;
		case BELIST_REMOVED:
			if( Tr2ControllerEventHandlerPtr handler = BlueCastPtr( value ) )
			{
				handler->Unlink();
			}
			break;
		default:
			break;
		}
	}
	else if( list == &m_variables )
	{
		switch( event & BELIST_EVENTMASK )
		{
		case BELIST_INSERTED:
		case BELIST_REMOVED:
		{
			if( auto owner = m_owner )
			{
				Unlink();
				Link( *owner );
			}
			break;
		}
		default:
			break;
		}
	}
}

void Tr2Controller::Link( IRoot& owner )
{
	CCP_STATS_ZONE( __FUNCTION__ );

	Unlink();

	m_owner = &owner;
	for( auto it = begin( m_stateMachines ); it != end( m_stateMachines ); ++it )
	{
		( *it )->Link( *this );
	}
	for( auto it = begin( m_eventHandlers ); it != end( m_eventHandlers ); ++it )
	{
		( *it )->Link( *this );
	}
}

void Tr2Controller::Unlink()
{
	if( !m_owner )
	{
		return;
	}

	CCP_STATS_ZONE( __FUNCTION__ );

	Stop();
	m_owner = nullptr;
	for( auto it = begin( m_stateMachines ); it != end( m_stateMachines ); ++it )
	{
		( *it )->Unlink();
	}
	for( auto it = begin( m_eventHandlers ); it != end( m_eventHandlers ); ++it )
	{
		( *it )->Unlink();
	}
}

bool Tr2Controller::IsLinked() const
{
	return m_owner != nullptr;
}

void Tr2Controller::Start()
{
	ScopedBlockTrap blockTrap;
	
	if( m_isActive )
	{
		Stop();
	}
	for( auto it = begin( m_stateMachines ); it != end( m_stateMachines ); ++it )
	{
		( *it )->Start();
	}
	m_isActive = true;
}

void Tr2Controller::Stop()
{
	if( !m_isActive )
	{
		return;
	}
	for( auto it = begin( m_stateMachines ); it != end( m_stateMachines ); ++it )
	{
		( *it )->Stop();
	}
	m_isActive = false;
}

void Tr2Controller::Update()
{
	if( !m_isActive )
	{
		return;
	}
	for( auto it = begin( m_stateMachines ); it != end( m_stateMachines ); ++it )
	{
		( *it )->Update();
	}
	for( auto it = begin( m_updateables ); it != end( m_updateables ); ++it )
	{
		( *it )->Update( BeOS->GetActualTime(), BeOS->GetCurrentFrameTime() );
	}
}

void Tr2Controller::SetVariable( const char* name, float value )
{
	if( auto var = GetVariableByName( name ) )
	{
		*var->GetPointerForParser() = value;
	}
}

void Tr2Controller::HandleEvent( const char* eventName )
{
	if( !m_isActive )
	{
		return;
	}
	for( auto it = begin( m_eventHandlers ); it != end( m_eventHandlers ); ++it )
	{
		auto handler = *it;
		if( strcmp( eventName, handler->GetName() ) == 0 )
		{
			handler->Execute( *this );
		}
	}
}

IRoot* Tr2Controller::GetOwner() const
{
	return m_owner;
}

Tr2ControllerFloatVariable* Tr2Controller::GetVariableByName( const char* name ) const
{
	for( auto it = begin( m_variables ); it != end( m_variables ); ++it )
	{
		if( ( *it )->GetName() == name )
		{
			return *it;
		}
	}
	return nullptr;
}

const PTr2ControllerFloatVariableVector& Tr2Controller::GetVariables() const
{
	return m_variables;
}

void Tr2Controller::GetBindingPathRoots( std::unordered_map<std::string, IRoot*>& variables ) const
{
	if( m_owner )
	{
		variables["Owner"] = m_owner;
	}
	for( auto it = begin( m_variables ); it != end( m_variables ); ++it )
	{
		variables[( *it )->GetName()] = *it;
	}
}

void Tr2Controller::RegisterUpdateable( ITr2Updateable& updateable )
{
	m_updateables.insert( &updateable );
}

void Tr2Controller::UnRegisterUpdateable( ITr2Updateable& updateable )
{
	m_updateables.erase( &updateable );
}

void Tr2Controller::Callback( BlueSharedString callbackName )
{
	if( !m_isActive || m_callbacks.empty() )
	{
		return;
	}

	for( auto callbackpair = begin( m_callbacks ); callbackpair != end( m_callbacks ); ++callbackpair )
	{
		auto pair = *callbackpair;
		if( pair.first == callbackName )
		{
			pair.second.CallVoid().ReportException();
		}
	}
}

void Tr2Controller::RegisterCallback( BlueSharedString callbackName, BlueScriptCallback callback )
{
	m_callbacks.push_back(std::pair<BlueSharedString, BlueScriptCallback>( callbackName, callback) );
}

void Tr2Controller::ClearCallbacks()
{
	m_callbacks.clear();
}