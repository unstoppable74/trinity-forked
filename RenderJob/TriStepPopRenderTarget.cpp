#include "StdAfx.h"
#include "TriStepPopRenderTarget.h"
#include "Tr2Renderer.h"


TriStepPopRenderTarget::TriStepPopRenderTarget( IRoot* lockobj )
{
}

TriStepPopRenderTarget::~TriStepPopRenderTarget(void)
{
}

TriStepResult TriStepPopRenderTarget::Execute( Be::Time realTime, Be::Time simTime, Tr2RenderContext& renderContext )
{
	renderContext.m_esm.PopRenderTarget();
	return RS_OK;
}