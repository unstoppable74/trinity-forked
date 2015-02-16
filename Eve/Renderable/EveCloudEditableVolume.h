////////////////////////////////////////////////////////////
//
//    Created:   February 2015
//    Copyright: CCP 2015
//

#pragma once
#ifndef EveCloudVolumeBall_H
#define EveCloudVolumeBall_H

BLUE_DECLARE( TriTextureRes );
BLUE_DECLARE( Tr2HostBitmap );
BLUE_DECLARE( EveCloudEditableVolume );

// --------------------------------------------------------------------------------
// Description:
//   A ball for EveCloudEditableVolume objects.
// --------------------------------------------------------------------------------
BLUE_CLASS( EveCloudVolumeBall ): public INotify
{
public:
	EXPOSE_TO_BLUE();

	EveCloudVolumeBall(IRoot* lockobj = NULL);
	~EveCloudVolumeBall();

	//////////////////////////////////////////////////////////////////////////
	// INotify
	virtual bool OnModified( Be::Var* value );

	struct BallData
	{
		Vector3 m_position;
		float m_radius;
		Color m_selfIllumination;
		float m_opacity;
		float m_falloff;
	} m_ballData;

	BlueWeakRef<EveCloudEditableVolume> m_owner;
};

TYPEDEF_BLUECLASS( EveCloudVolumeBall );
BLUE_DECLARE_VECTOR( EveCloudVolumeBall );


// --------------------------------------------------------------------------------
// Description:
//   An "editor" for volume textures used by EveCloud. Maintains a list of balls
//   (EveCloudVolumeBall) that are rasterized into a volume texture.
// --------------------------------------------------------------------------------
BLUE_CLASS( EveCloudEditableVolume ): 
	public INotify,
	public IListNotify
{
public:
	EXPOSE_TO_BLUE();

	EveCloudEditableVolume(IRoot* lockobj = NULL);
	~EveCloudEditableVolume();

	//////////////////////////////////////////////////////////////////////////
	// INotify
	virtual bool OnModified( Be::Var* value );

	//////////////////////////////////////////////////////////////////////////
	// IListNotify
	virtual void OnListModified(
		long event,
		ssize_t key,
		ssize_t key2,
		IRoot* value,
		const struct IList* theList );

	void OnVolumeModified();

	void Update();
	void RenderDebugInfo( const Matrix& world, Tr2RenderContext& renderContext );

private:
	enum Status
	{
		Working,
		StopRequested,
		DataReady,
		Aborted,
	};
	struct RasterizeParams
	{
		CcpAtomic<uint32_t> status;
		std::vector<EveCloudVolumeBall::BallData> balls;
		uint32_t width;
		uint32_t height;
		uint32_t depth;
		std::unique_ptr<uint8_t[]> pixels;
	};
	static uint32_t ThreadProc( void* context );
	static void RasterizeBalls( RasterizeParams& params );
	static void RasterizeBall( const EveCloudVolumeBall::BallData& ball, const RasterizeParams& params, float* pixels );

	RasterizeParams m_currentParams;
	CcpThreadHandle_t m_thread;

	TriTextureResPtr m_texture;
	Tr2HostBitmapPtr m_bitmap;
	PEveCloudVolumeBallVector m_balls;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_depth;
	bool m_renderDebugInfo;
};

TYPEDEF_BLUECLASS( EveCloudEditableVolume );
BLUE_DECLARE_VECTOR( EveCloudEditableVolume );

#endif