////////////////////////////////////////////////////////////
//
//    Created:   February 2013
//    Copyright: CCP 2013
//

#include "StdAfx.h"
#include "Tr2GpuBufferAlias.h"

using namespace Tr2RenderContextEnum;

// --------------------------------------------------------------------------------------
// Description:
//   Tr2GpuBufferAlias default constructor
// --------------------------------------------------------------------------------------
Tr2GpuBufferAlias::Tr2GpuBufferAlias( IRoot* )
	:m_format( PIXEL_FORMAT_UNKNOWN ),
	m_index( 0 )
{
}

// --------------------------------------------------------------------------------------
// Description:
//   Tr2GpuBufferAlias destructor
// --------------------------------------------------------------------------------------
Tr2GpuBufferAlias::~Tr2GpuBufferAlias()
{
}

// --------------------------------------------------------------------------------------
// Description:
//   Blue-exposed initializer. 
// --------------------------------------------------------------------------------------
ALResult Tr2GpuBufferAlias::py__init__( 
	Be::Optional<ITr2GpuBuffer*> source, 
	unsigned index, 
	Tr2RenderContextEnum::PixelFormat format )
{
	if( source.IsAssigned() )
	{
		return Create( source, index, format );
	}
	return S_OK;
}

// --------------------------------------------------------------------------------------
// Description:
//   Implements IInitialize interface. Construct AL buffer from read parameters.
// Return Value:
//   true always
// --------------------------------------------------------------------------------------
bool Tr2GpuBufferAlias::Initialize()
{
	CreateBuffer();
	return true;
}

// --------------------------------------------------------------------------------------
// Description:
//   Implements INotify interface. Re-creates AL buffer when of the object parameters
//   changes.
// Arguments:
//   value - The Blue-exposed parameter that changed
// Return Value:
//   true always
// --------------------------------------------------------------------------------------
bool Tr2GpuBufferAlias::OnModified( Be::Var* value )
{
	CreateBuffer();
	return true;
}

// --------------------------------------------------------------------------------------
// Description:
//   Assigns new values to buffer parameters and construct AL buffer.
// Arguments:
//   source - Original GPU buffer
//   index - Buffer index for multi-buffer
//   format - New element type
// Return Value:
//   true If AL buffer is successfully created
//   false Otherwise
// --------------------------------------------------------------------------------------
ALResult Tr2GpuBufferAlias::Create( ITr2GpuBuffer* source, unsigned index, Tr2RenderContextEnum::PixelFormat format )
{
	m_source = source;
	m_index = index;
	m_format = format;
	return CreateBuffer();
}

// --------------------------------------------------------------------------------------
// Description:
//   Implements ITr2GpuBuffer interface. Returns AL buffer.
// Arguments:
//   index - Buffer index (unused)
// Return Value:
//   Pointer to AL buffer
// --------------------------------------------------------------------------------------
Tr2GpuBufferAL* Tr2GpuBufferAlias::GetGpuBuffer( unsigned index )
{
	return &m_buffer;
}

// --------------------------------------------------------------------------------------
// Description:
//   Check if the object contains a valid AL buffer.
// Return Value:
//   true If the object contains a valid AL buffer
//   false Otherwise
// --------------------------------------------------------------------------------------
bool Tr2GpuBufferAlias::IsValid() const
{
	return m_buffer.IsValid();
}

// --------------------------------------------------------------------------------------
// Description:
//   Returns number of elements in the buffer.
// Return Value:
//   Number of elements in the buffer.
// --------------------------------------------------------------------------------------
uint32_t Tr2GpuBufferAlias::GetCount() const
{
	return m_buffer.GetNumElements();
}

// --------------------------------------------------------------------------------------
// Description:
//   Re-creates AL buffer.
// Return Value:
//   true If AL buffer is successfully created
//   false Otherwise
// --------------------------------------------------------------------------------------
ALResult Tr2GpuBufferAlias::CreateBuffer()
{
	USE_MAIN_THREAD_RENDER_CONTEXT();
	Tr2GpuBufferAL* source = m_source->GetGpuBuffer( 0 );
	if( !source )
	{
		return E_INVALIDARG;
	}
	return m_buffer.CreateAlias( *source, m_format, renderContext );
}

void Tr2GpuBufferAlias::ReleaseResources( TriStorage s )
{
	if( m_buffer.IsValid() && ( ( s & m_buffer.GetMemoryClass() ) != 0 ) )
	{
		m_buffer.Destroy();
	}
}

bool Tr2GpuBufferAlias::OnPrepareResources()
{
	if( !m_buffer.IsValid() && m_format != PIXEL_FORMAT_UNKNOWN )
	{
		return SUCCEEDED( CreateBuffer() );
	}
	return true;
}