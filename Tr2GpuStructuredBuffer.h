////////////////////////////////////////////////////////////
//
//    Created:   February 2013
//    Copyright: CCP 2013
//

#pragma once
#ifndef Tr2GpuStructuredBuffer_H
#define Tr2GpuStructuredBuffer_H


#include "include/ITr2GpuBuffer.h"

// --------------------------------------------------------------------------------------
// Description:
//   A Blue-exposed wrapper around Tr2GpuBufferAL class for structured buffers, i.e. 
//   buffers containing structures as elements. The creation parameters for the class are 
//   persisted, but the contents of the buffer is not.
// See Also:
//   Tr2GpuBufferAL
// --------------------------------------------------------------------------------------
BLUE_CLASS( Tr2GpuStructuredBuffer ): 
	public ITr2GpuBuffer,
	public INotify,
	public IInitialize,
	public Tr2DeviceResource
{
public:
	EXPOSE_TO_BLUE();

	using IInitialize::Lock;
	using IInitialize::Unlock;

	enum CreationFlag
	{
		// Can the buffer be locked with write-only access
		CPU_WRITABLE	= 1,
		// Is the buffer used for GPU write access
		GPU_WRITABLE	= 2,
		// Buffer with append/consume GPU access
		APPEND_BUFFER	= 4,
		// Add a counter to the buffer
		COUNTER			= 8,
	};

	typedef uint32_t CreationFlags;

	Tr2GpuStructuredBuffer( IRoot* = 0 );
	~Tr2GpuStructuredBuffer();

	ALResult py__init__( uint32_t count, uint32_t stride, CreationFlags creationFlags );

	bool Initialize();

	bool OnModified( Be::Var* value );

	Tr2GpuBufferAL* GetGpuBuffer( unsigned index );

	ALResult Create( uint32_t count, uint32_t stride, CreationFlags creationFlags );
	bool IsValid() const;
	uint32_t GetCount() const;

	operator Tr2GpuBufferAL&() { return m_buffer; }
	operator const Tr2GpuBufferAL&() const { return m_buffer; }

	virtual void ReleaseResources( TriStorage s );
private:
	virtual bool OnPrepareResources();
	ALResult CreateBuffer();

	// AL buffer
	Tr2GpuBufferAL	m_buffer;

	// Number of elements in the buffer
	uint32_t m_count;
	// Size of one element in bytes
	uint32_t m_stride;
	CreationFlags m_creationFlags;
};

TYPEDEF_BLUECLASS( Tr2GpuStructuredBuffer );

#endif