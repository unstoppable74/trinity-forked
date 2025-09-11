////////////////////////////////////////////////////////////
//
//    Created:   August 2025
//    Copyright: CCP 2025
//
#pragma once

#ifndef EveAudioObject_h
#define EveAudioObject_h

#include "include/ITriFunction.h"
#include "IWorldPosition.h"
#include "EveTransform.h"
#include "IEveSpaceObject2.h"
#include "Include/ITr2DebugRenderer2.h"
#include <ITr2AudEmitter.h>

BLUE_DECLARE_INTERFACE( ITr2AudEmitter );
BLUE_DECLARE_IVECTOR( ITr2AudEmitter );

BLUE_DECLARE( EveAudioObject );

/**
 * @brief Standalone audio object for 3D positioning without Trinity asset dependency.
 * 
 * EveAudioObject is a stripped-down version of EveEffectRoot2 that focuses purely on
 * audio positioning. It implements the minimal required interfaces (IWorldPosition, 
 * IEveSpaceObject2) and provides audio emitter creation capability.
 * 
 */

BLUE_CLASS( EveAudioObject ):
	public IWorldPosition,
	public IEveSpaceObject2,
	public IInitialize,
	public ITr2DebugRenderable,
	public INotify

{
public:
    EXPOSE_TO_BLUE();

	EveAudioObject( IRoot* lockobj = NULL );
	
	bool Initialize();
	
	// INotify
	virtual bool OnModified( Be::Var* val );

	// IEveSpaceObject2
	void UpdateSyncronous( const EveUpdateContext& updateContext );
	void UpdateAsyncronous( const EveUpdateContext& updateContext );
	void UpdateVisibility( const EveUpdateContext& updateContext, const Matrix& parentTransform );
	void GetRenderables( std::vector<ITr2Renderable*>& renderables, Tr2ImpostorManager* impostors );
	bool GetBoundingSphere( Vector4& sphere, BoundingSphereQuery query=EVE_BOUNDS_NORMAL ) const;
	void GetPerObjectStructs( EveSpaceObjectVSData& vsData, EveSpaceObjectPSData& psData ) const;
	void UpdateModelCenterWorldPosition( Vector3 &position, Be::Time t );
	void GetModelCenterWorldPosition( Vector3 &position ) const;
	bool GetLocalBoundingBox( Vector3 &min, Vector3 &max );
	void GetLocalToWorldTransform( Matrix &transform ) const;

	// IWorldPosition
	virtual Vector3 GetWorldPosition();
	virtual Quaternion GetWorldRotation();
	
	// ITr2DebugRenderable
	virtual void GetDebugOptions( Tr2DebugRendererOptions& options );
	virtual void RenderDebugInfo( ITr2DebugRenderer2& renderer );
	
	virtual void Update( const Matrix& worldTransform );

	/**
	 * @brief Gets the audio emitter for this object.
	 * @return Smart pointer to the emitter, or null if not initialized
	 */
	ITr2AudEmitterPtr GetAudioEmitter() const { return m_audioEmitter; }
	
	/**
	 * @brief Sets the name of this object's audio emitter.
	 * @param name Name for the emitter
	 */
	void SetEmitterName( const std::string& name );
	
	/**
	 * @brief Sets the world position of this audio object.
	 * @param position New position in world coordinates
	 */
	void SetPosition( const Vector3& position );
	
	/**
	 * @brief Sets the world rotation of this audio object.
	 * @param rotation New rotation as quaternion
	 */
	void SetRotation( const Quaternion& rotation );
	
	/**
	 * @brief Plays an audio event on this object's emitter.
	 * @param eventName Wwise event name to trigger
	 * @return Playing ID from Wwise, or 0 if failed
	 */
	unsigned int PlayAudioEvent( const std::wstring& eventName );
	
	/**
	 * @brief Sets mute state for this object's emitter.
	 * @param mute True to mute, false to unmute
	 */
	void SetMute( bool mute );
	
	/**
	 * @brief Gets current mute state.
	 * @return True if muted, false otherwise
	 */
	bool GetMute() const { return m_mute; }

private:
	void UpdateWorldTransform( Be::Time time );

protected:
	Vector3 m_scaling;
	Quaternion m_rotation;  
	Vector3 m_translation;
	
	ITriVectorFunctionPtr m_ballPosition;
	ITriQuaternionFunctionPtr m_ballRotation;
	
	Matrix m_worldTransform;
	Matrix m_lastUpdateMatrix;
	
	ITr2AudEmitterPtr m_audioEmitter;
	bool m_mute;

	std::string m_name;
	bool m_display;
	std::wstring m_audioEvent;
};

TYPEDEF_BLUECLASS( EveAudioObject );

#endif // EveAudioObject_h