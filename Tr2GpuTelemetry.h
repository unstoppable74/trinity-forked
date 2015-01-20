#pragma once

void Tr2GpuTelemetryDeviceCreated();
void Tr2GpuTelemetryDeviceReset();
void Tr2GpuTelemetryTick();
void Tr2GpuTelemetryEnterZone( const char* name );
void Tr2GpuTelemetryLeaveZone();

extern bool g_gpuTelemetryEnabled;

#if CCP_TELEMETRY_ENABLED && ( TRINITY_PLATFORM == TRINITY_DIRECTX9 || TRINITY_PLATFORM == TRINITY_DIRECTX11 )

class Tr2GpuTelemetryZone
{
public:
	Tr2GpuTelemetryZone( const char* name )
	{
		Tr2GpuTelemetryEnterZone( name );
	}
	~Tr2GpuTelemetryZone()
	{
		Tr2GpuTelemetryLeaveZone();
	}
};

#define CCP_STATS_GPU_ZONE( name ) Tr2GpuTelemetryZone gpuZone_##_COUNTER_( name )

#else

#define CCP_STATS_GPU_ZONE( name )

#endif