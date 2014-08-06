////////////////////////////////////////////////////////////
//
//    Created:   August 2014
//    Copyright: CCP 2014
//
#pragma once
#ifndef StringUtils_H
#define StringUtils_H

// --------------------------------------------------------------------------------
// Description:
//   Inserts insertStr before the last instance of beforeSubstr in baseString.
// --------------------------------------------------------------------------------
inline bool InsertStringStub( std::string& baseString, const char* beforeSubstr, const char* insertStr )
{
	size_t index = baseString.rfind( beforeSubstr );
	if( index == std::string::npos )
	{
		return false;
	}

	baseString.insert( index, insertStr );
	return true;
}

#endif // StringUtils_H