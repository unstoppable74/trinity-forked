////////////////////////////////////////////////////////////
//
//    Created:   February 2015
//    Copyright: CCP 2015
//

#include "StdAfx.h"
#include "EveCloudEditableVolume.h"

#include "Resources/TriTextureRes.h"
#include "Tr2HostBitmap.h"
#include "Tr2Renderer.h"


EveCloudVolumeBall::EveCloudVolumeBall( IRoot* lockobj )
{
	m_ballData.m_position = Vector3( 0.f, 0.f, 0.f );
	m_ballData.m_radius = 0.f;
	m_ballData.m_selfIllumination = Color( 0.f, 0.f, 0.f, 0.f );
	m_ballData.m_opacity = 0.f;
	m_ballData.m_falloff = 1.f;
}

EveCloudVolumeBall::~EveCloudVolumeBall()
{
}

bool EveCloudVolumeBall::OnModified( Be::Var* value )
{
	if( !!m_owner )
	{
		m_owner->OnVolumeModified();
	}
	return true;
}


EveCloudEditableVolume::EveCloudEditableVolume( IRoot* lockobj )
	:PARENTLOCK( m_balls ),
	m_width( 64 ),
	m_height( 64 ),
	m_depth( 64 ),
	m_thread( 0 ),
	m_renderDebugInfo( false )
{
	m_texture.CreateInstance();
	m_bitmap.CreateInstance();

	m_balls.SetNotify( this );
}

EveCloudEditableVolume::~EveCloudEditableVolume()
{
}

bool EveCloudEditableVolume::OnModified( Be::Var* value )
{
	OnVolumeModified();
	return true;
}

void EveCloudEditableVolume::Update()
{
	if( m_currentParams.status == DataReady )
	{
		m_thread = 0;
		m_bitmap->CreateVolume( m_currentParams.width, m_currentParams.height, m_currentParams.depth, 1, Tr2RenderContextEnum::PIXEL_FORMAT_B8G8R8A8_UNORM );
		memcpy( m_bitmap->GetRawData(), m_currentParams.pixels.get(), m_bitmap->GetRawDataSize() );
		m_texture->CreateFromHostBitmap( m_bitmap );
	}
}

void EveCloudEditableVolume::OnListModified(
	long event,
	ssize_t key,
	ssize_t key2,
	IRoot* value,
	const struct IList* theList )
{
	switch( event & BELIST_EVENTMASK )
	{
	case BELIST_INSERTED:
		if( event & BELIST_LOADING )
		{
			return;
		}
		else
		{
			EveCloudVolumeBallPtr sphere = BlueCastPtr( value );
			if( sphere )
			{
				sphere->m_owner = this;
			}
			OnVolumeModified();
		}
		break;
	case BELIST_REMOVED:
		if(event & BELIST_UNLOADING)
		{
			return;
		}
		else
		{
			EveCloudVolumeBallPtr sphere = BlueCastPtr( value );
			if( sphere )
			{
				sphere->m_owner = nullptr;
			}
			OnVolumeModified();
		}
		break;
	case BELIST_LOADFINISHED:
		for( auto it = m_balls.begin(); it != m_balls.end(); ++it )
		{
			( *it )->m_owner = this;
		}
		break;
	case BELIST_UNLOADSTART:
		for( auto it = m_balls.begin(); it != m_balls.end(); ++it )
		{
			( *it )->m_owner = nullptr;
		}
		break;
	}
}

void EveCloudEditableVolume::OnVolumeModified()
{
	if( m_thread )
	{
		m_currentParams.status = StopRequested;
		uint32_t result;
		CcpJoinThread( m_thread, &result );
	}

	m_currentParams.balls.resize( m_balls.size() );
	for( size_t i = 0; i < m_balls.size(); ++i )
	{
		m_currentParams.balls[i] = m_balls[i]->m_ballData;
	}
	m_currentParams.width = m_width;
	m_currentParams.height = m_height;
	m_currentParams.depth = m_depth;
	m_currentParams.pixels = std::unique_ptr<uint8_t[]>( new uint8_t[m_width * m_height * m_depth * 4] );
	m_currentParams.status = Working;

	m_thread = CcpCreateThread( &ThreadProc, &m_currentParams, CCP_THREAD_PRIORITY_NORMAL );
}

uint32_t EveCloudEditableVolume::ThreadProc( void* context )
{
	RasterizeBalls( *static_cast<RasterizeParams*>( context ) );
	return 0;
}

void EveCloudEditableVolume::RasterizeBalls( RasterizeParams& params )
{
	std::unique_ptr<float[]> pixels( new float[params.width * params.height * params.depth * 4] );
	memset( pixels.get(), 0, sizeof( float ) * ( params.width * params.height * params.depth * 4 ) );
	for( auto it = params.balls.begin(); it != params.balls.end(); ++it )
	{
		RasterizeBall( *it, params, pixels.get() );
		if( params.status != Working)
		{
			params.status = Aborted;
			return;
		}
	}
	const float* srcPixel = pixels.get();
	uint8_t* destPixel = params.pixels.get();
	for( uint32_t i = 0; i < params.width * params.height * params.depth * 4; ++i )
	{
		destPixel[i] = uint8_t( std::min( std::max( TriLinearToGamma( srcPixel[i] ) * 255.f, 0.f ), 255.f ) );
	}
	params.status = DataReady;
}

void EveCloudEditableVolume::RasterizeBall( const EveCloudVolumeBall::BallData& ball, const RasterizeParams& params, float* pixels )
{
	auto selfIllumination = TriGammaToLinear( ball.m_selfIllumination );
	int minX = std::max( int( ( ball.m_position.x + 0.5 - ball.m_radius ) * params.width ), 0 );
	int minY = std::max( int( ( ball.m_position.y + 0.5 - ball.m_radius ) * params.height ), 0 );
	int minZ = std::max( int( ( ball.m_position.z + 0.5 - ball.m_radius ) * params.depth ), 0 );

	int maxX = std::min( int( ( ball.m_position.x + 0.5 + ball.m_radius ) * params.width ), int( params.width ) - 1 );
	int maxY = std::min( int( ( ball.m_position.y + 0.5 + ball.m_radius ) * params.height ), int( params.height ) - 1 );
	int maxZ = std::min( int( ( ball.m_position.z + 0.5 + ball.m_radius ) * params.depth ), int( params.depth ) - 1 );

	for( int k = minZ; k <= maxZ; ++k )
	{
		float z = float( k ) / params.depth - 0.5f;
		float dz = z - ball.m_position.z;
		dz *= dz;
		for( int j = minY; j <= maxY; ++j )
		{
			if( params.status == StopRequested )
			{
				return;
			}
			float y = float( j ) / params.height - 0.5f;
			float dy = y - ball.m_position.y;
			dy *= dy;
			for( int i = minX; i <= maxX; ++i )
			{
				float x = float( i ) / params.width - 0.5f;
				float dx = x - ball.m_position.x;
				dx *= dx;

				float distance = std::min( 1.f,  sqrtf( dx + dy + dz ) / ball.m_radius );
				float alpha = pow( 1.f - distance, ball.m_falloff );

				int offset = ( i + j * params.width + k * params.width * params.height ) * 4;

				float destR = pixels[offset + 2];
				float destG = pixels[offset + 1];
				float destB = pixels[offset + 0];

				pixels[offset] = destB * ( 1.f - alpha ) + selfIllumination.b * alpha;
				pixels[offset + 1] = destG * ( 1.f - alpha ) + selfIllumination.g * alpha;
				pixels[offset + 2] = destR * ( 1.f - alpha ) + selfIllumination.r * alpha;
				pixels[offset + 3] += alpha * ball.m_opacity;
			}
		}
	}
}

void EveCloudEditableVolume::RenderDebugInfo( const Matrix& world, Tr2RenderContext& renderContext )
{
	if( !m_renderDebugInfo )
	{
		return;
	}
	Matrix scale;
	D3DXMatrixScaling( &scale, 0.5f, 0.5f, 0.5f );
	Tr2Renderer::DrawOrientedBox( scale * world, 0xffff0000 );

	for( auto it = m_balls.begin(); it != m_balls.end(); ++it )
	{
		Vector3 position;
		D3DXVec3TransformCoord( &position, &( *it )->m_ballData.m_position, &world );
		Tr2Renderer::DrawSphere( position, 
			( *it )->m_ballData.m_radius * world._11, 
			10, 
			( uint32_t( ( *it )->m_ballData.m_opacity * 255 ) << 24 ) | ( *it )->m_ballData.m_selfIllumination );
	}
}