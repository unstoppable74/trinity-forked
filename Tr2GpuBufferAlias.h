////////////////////////////////////////////////////////////
//
//    Created:   February 2013
//    Copyright: CCP 2013
//

#pragma once
#ifndef Tr2GpuBufferAlias_H
#define Tr2GpuBufferAlias_H

#include "include/ITr2GpuBuffer.h"

// --------------------------------------------------------------------------------------
// Description:
//   A Blue-exposed wrapper around Tr2GpuBufferAL class for creating typed aliases to
//   existing buffers.
// See Also:
//   Tr2GpuBufferAL
// --------------------------------------------------------------------------------------
BLUE_CLASS( Tr2GpuBufferAlias ): 
	public ITr2GpuBuffer,
	public INotify,
	public IInitialize,
	public Tr2DeviceResource
{
public:
	EXPOSE_TO_BLUE();

	using IInitialize::Lock;
	using IInitialize::Unlock;

    Tr2GpuBufferAlias( IRoot* = 0 );
	~Tr2GpuBufferAlias();

	ALResult py__init__( 
		Be::Optional<ITr2GpuBuffer*> source, 
		unsigned index, 
		Tr2RenderContextEnum::PixelFormat format );

	bool Initialize();

	bool OnModified( Be::Var* value );

	Tr2GpuBufferAL* GetGpuBuffer( unsigned index );

	ALResult Create( ITr2GpuBuffer* source, unsigned index, Tr2RenderContextEnum::PixelFormat format );
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
	// Buffer index (for multi-buffers)
	unsigned m_index;
	// Source GPU buffer
	ITr2GpuBufferPtr m_source;
	// New element type
	Tr2RenderContextEnum::PixelFormat m_format;
};

TYPEDEF_BLUECLASS( Tr2GpuBufferAlias );

#endif