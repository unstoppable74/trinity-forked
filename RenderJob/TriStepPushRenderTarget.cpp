#include "StdAfx.h"
#include "TriStepPushRenderTarget.h"
#include "Tr2Renderer.h"


TriStepPushRenderTarget::TriStepPushRenderTarget( IRoot* lockobj )
{
}

TriStepResult TriStepPushRenderTarget::Execute( Be::Time realTime, Be::Time simTime, Tr2RenderContext& renderContext )
{
	if( m_renderTarget )
	{
		renderContext.m_esm.PushRenderTarget( *m_renderTarget );
	}
	else
	{
		renderContext.m_esm.PushRenderTarget();
	}
	return RS_OK;
}

void TriStepPushRenderTarget::py__init__( Tr2RenderTarget* rt )
{
	m_renderTarget = rt;
}
