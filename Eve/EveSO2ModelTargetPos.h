////////////////////////////////////////////////////////////
//
//    Created:   March 2015
//    Copyright: CCP 2015
//

#ifndef _EVESPACEOBJECTMODELTARGETPOSITION_H_
#define _EVESPACEOBJECTMODELTARGETPOSITION_H_

#include "include/ITriFunction.h"
#include "include/ITriTargetable.h"
#include "include/TriVector.h"

class EveSO2ModelTargetPos:
	public ITriVectorFunction
{

public:

	EXPOSE_TO_BLUE();

	EveSO2ModelTargetPos(IRoot* lockobj = NULL);
	~EveSO2ModelTargetPos();

	void UpdateValue( double time ) { Vector3 v; Update( &v, time ); }

	/////////////////////////////////////////////////////////////////////////////////////
	// ITriVectorFunction
	/////////////////////////////////////////////////////////////////////////////////////

	Vector3* Update( Vector3* in, Be::Time time );
	Vector3* Update( Vector3* in, double time );
	Vector3* GetValueAt( Vector3* in, Be::Time time );
	Vector3* GetValueAt( Vector3* in, double time );
	Vector3* GetValueDotAt( Vector3* in, Be::Time time );
	Vector3* GetValueDotAt( Vector3* in, double time );
	Vector3* GetValueDoubleDotAt( Vector3* in, Be::Time time );
	Vector3* GetValueDoubleDotAt( Vector3* in, double time );
	Vector3d* InterpolatedPosition(Vector3d* out, Be::Time time);

private:
	std::wstring  m_name;
	ITriVectorFunctionPtr m_parentObject;
	ITriTargetablePtr m_targetObject;
	Vector3 m_value;
	int m_index;
};
TYPEDEF_BLUECLASS(EveSO2ModelTargetPos);

#endif
