#include "StdAfx.h"
#include "Tr2TextureReference.h"

Tr2TextureReference::Tr2TextureReference( IRoot* )
{
}

Tr2TextureAL* Tr2TextureReference::GetTexture()
{
	return &m_texture;
}



Tr2TransientTextureReference::Tr2TransientTextureReference( IRoot* )
	:m_texture( nullptr )
{
}

Tr2TextureAL* Tr2TransientTextureReference::GetTexture()
{
	return m_texture;
}

void Tr2TransientTextureReference::SetTexture( Tr2TextureAL* texture )
{
	m_texture = texture;
}