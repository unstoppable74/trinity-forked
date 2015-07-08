////////////////////////////////////////////////////////////
//
//    Created:   October 2014
//    Copyright: CCP 2014
//

#pragma once
#ifndef Tr2LightManager_H
#define Tr2LightManager_H

#include "Tr2DeviceResource.h"
#include "Tr2Variable.h"
#include "TriFrustum.h"

BLUE_DECLARE( Tr2Effect );
BLUE_DECLARE( Tr2GpuBuffer );
BLUE_DECLARE( Tr2GpuStructuredBuffer );


// --------------------------------------------------------------------------------------
// Description:
//   Helper class to store a growable array of objects. A super-limited version of 
//   std::vector, but it allows appending new objects in parallel. All other operations
//   are not thread safe.
// --------------------------------------------------------------------------------------
template<typename T>
class Tr2AddSafeGrowableBuffer
{
public:
	Tr2AddSafeGrowableBuffer( uint32_t initialCount, const char* allocationName );
	void Clear();
	bool Add( const T& element, const char* allocationName );
	uint32_t GetCount() const;
	const T* GetData() const;
private:
	CcpMallocBuffer m_data;
	CcpAtomic<uint32_t> m_count;
	CcpAtomic<uint32_t> m_capacity;
	CcpAtomic<uint32_t> m_dataUsed;
};


// --------------------------------------------------------------------------------------
// Description:
//   Manages light sources for tiled/clustered lighting. An object of this class does not
//   store lights permanently. Rather a user adds lights every frame and then the object
//   updates GPU buffers needed for tiled lighting. Normally the per-frame operations
//   for the light manager would look like:
//     lightManager.Clear();
//     lightManager.SetFrustum( frustum );
//     for each in scene.lights
//       lightManager.AddPointLight( each ); // this can be done in parallel
//     lightManager.UpdateLists();
//     renderYourStuff();
//  Light manager GPU buffers are accessed through variable store with names 
//  "LightBuffer" and "LightIndexBuffer".
// --------------------------------------------------------------------------------------
class Tr2LightManager: public Tr2DeviceResource
{
public:
	Tr2LightManager( const char* effectPath );
	~Tr2LightManager();

	void Clear();
	void SetFrustum( const TriFrustum& frustum );
	void AddPointLight( const Vector3& position, float radius, const Color& color );
	ALResult UpdateLists( Tr2RenderContext& renderContext );

	virtual void ReleaseResources( TriStorage s );

	static Tr2LightManager* GetOrCreateInstance( const char* effectPath );
	static Tr2LightManager* GetInstance();
	static void DeleteInstance();
private:
	struct PerLightData
	{
		Vector3 position;
		float radius;
		Vector3 color;
		float _padding;
	};

	Tr2LightManager( const Tr2LightManager& );
	Tr2LightManager& operator=( const Tr2LightManager& );

	virtual bool OnPrepareResources();

	ALResult DoUpdateLists( Tr2RenderContext& renderContext );
	ALResult ClearLightIndices( Tr2RenderContext& renderContext );
	ALResult UpdateLightBuffer( Tr2RenderContext& renderContext );

	Tr2AddSafeGrowableBuffer<PerLightData> m_lightData;
	Tr2GpuStructuredBufferPtr m_lightBuffer;
	Tr2GpuStructuredBufferPtr m_indexBuffer;
	Tr2ConstantBufferAL m_perFrameData;
	Tr2EffectPtr m_effect;
	Tr2Variable m_lightBufferVariable;
	Tr2Variable m_indexBufferVariable;
	TriFrustum m_frustum;
};



template<typename T>
Tr2AddSafeGrowableBuffer<T>::Tr2AddSafeGrowableBuffer( uint32_t initialCount, const char* allocationName )
	:m_capacity( initialCount )
{
	m_data.resize( allocationName, m_capacity * sizeof( T ) );
}

template<typename T>
void Tr2AddSafeGrowableBuffer<T>::Clear()
{
	m_count = 0;
}

template<typename T>
bool Tr2AddSafeGrowableBuffer<T>::Add( const T& element, const char* allocationName )
{
	uint32_t index = m_count++;
	while( true )
	{
		if( index == m_capacity )
		{
			while( m_dataUsed )
			{
			}
			m_data.resize( allocationName, m_capacity * 2 * sizeof( T ) );
			m_capacity = m_capacity * 2;
		}
		else if( index < m_capacity )
		{
			break;
		}
	}

	m_dataUsed = 1;
	if( !m_data.get() )
	{
		m_dataUsed = 0;
		return false;
	}
	reinterpret_cast<T*>( m_data.get() )[index] = element;
	m_dataUsed = 0;
	return true;
}

template<typename T>
uint32_t Tr2AddSafeGrowableBuffer<T>::GetCount() const
{
	return m_count;
}

template<typename T>
const T* Tr2AddSafeGrowableBuffer<T>::GetData() const
{
	return reinterpret_cast<const T*>( m_data.get() );
}

#endif