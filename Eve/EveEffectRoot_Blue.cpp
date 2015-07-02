#include "StdAfx.h"
#include "EveEffectRoot.h"

BLUE_DEFINE( EveEffectRoot );

const Be::ClassInfo* EveEffectRoot::ExposeToBlue()
{
    EXPOSURE_BEGIN( EveEffectRoot, "" )
		MAP_INTERFACE( IEveSpaceObject2 )
		MAP_INTERFACE( IInitialize )

		MAP_ATTRIBUTE
		(
			"name",
			m_name,
			"",
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		(
			"display",
			m_display,
			"",
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		(
			"dynamicLOD",
			m_dynamicLODSelection,
			"",
			Be::READWRITE | Be::PERSIST
		)
		
		MAP_ATTRIBUTE
		(
			"lodLevel",
			m_lodLevel,
			"",
			Be::READ
		)
		
		MAP_ATTRIBUTE
		(
			"estimatedSize",
			m_estimatedSize,
			"",
			Be::READ
		)

		MAP_ATTRIBUTE
		( 
			"highDetail",    
			m_highDetail,
			"Proxy for a mesh used for rendering in high detail.", 
			Be::READWRITE | Be::PERSIST
		)
		
		MAP_ATTRIBUTE
		( 
			"mediumDetail",    
			m_mediumDetail,
			"Proxy for a mesh used for rendering in medium detail.", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"lowDetail",    
			m_lowDetail,
			"Proxy for a mesh used for rendering in low detail.", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		(
			"active",
			m_effectObject,
			"",
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		(    
			"translationCurve",
			m_ballPosition,
			"Vector function slot for attaching a destiny ball to set the position of an object",
			Be::READWRITE | Be::PERSIST
		)
		MAP_ATTRIBUTE
		(    
			"rotationCurve",
			m_ballRotation,
			"Quaternion function slot for attaching a destiny ball to set the rotation of an object",
			Be::READWRITE | Be::PERSIST
		)

#if BLUE_WITH_PYTHON
		// expose bounding sphere as two variables: center pos and radius
		MAPFLOATARRAYSIZE( "boundingSphereCenter", m_boundingSphere, BlueDefaultIID, "The center of the minimum bounding sphere of the effect in local coordinates", Be::READWRITE | Be::PERSIST, 3 )
#endif

		MAP_ATTRIBUTE( "boundingSphereRadius", m_boundingSphere.w, "The radius of the minimum bounding sphere of the effect", Be::READWRITE | Be::PERSIST )

		MAP_ATTRIBUTE( "scaling", m_scaling, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "rotation", m_rotation, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "translation", m_translation, "", Be::READWRITE | Be::PERSIST )
		MAP_ATTRIBUTE( "duration", m_effectDuration, "", Be::READWRITE | Be::PERSIST )
		
		MAP_ATTRIBUTE
		( 
			"observers", 
			m_observers, 
			"Observers for pushing data between modules every frame. Currently used to push locator data out to the audio2 module.",
			Be::READWRITE | Be::PERSIST
		)

		MAP_METHOD_AND_WRAP( "GetBoundingSphereRadius", GetBoundingSphereRadius, "Returns the bounding sphere radius." )
		MAP_METHOD_AND_WRAP( "Start", Start, "" )
		MAP_METHOD_AND_WRAP( "Stop", Stop, "" )

		MAP_ATTRIBUTE
		(
			"loadedCallback",
			m_loadedEventListener,
			"An event listener that's triggered when assigning a lod for the first time",
			Be::READWRITE
		)

		MAP_ATTRIBUTE( "lights", m_lights, "List of dynamic lights", Be::READ | Be::PERSIST );

    EXPOSURE_END();
}
