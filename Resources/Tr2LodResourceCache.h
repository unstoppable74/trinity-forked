////////////////////////////////////////////////////////////////////////////////
//
// Created:		September 2018
// Copyright:	CCP 2018
//

#pragma once
#ifndef Tr2LodResourceCache_h
#define Tr2LodResourceCache_h

#include "Tr2LodResource.h"

template <typename T>
class Tr2LodResourceCache
{
public:
	Tr2LodResourceCache() :
		m_rawResource( nullptr ),
		m_typedResource( nullptr )
	{
	}

	~Tr2LodResourceCache() 
	{
	}

	T* GetResource( Tr2LodResource* resource )
	{
		if( resource )
		{
			IBlueResource *data = resource->GetResource();
			if( data != m_rawResource )
			{
				m_rawResource = data;
				m_typedResource = dynamic_cast<T*>( data );
			}

			return m_typedResource;
		}
		else
		{
			m_rawResource = nullptr;
			m_typedResource = nullptr;
		}

		return nullptr;
	}
private:
	IBlueResource* m_rawResource;
	T* m_typedResource;
};

#endif