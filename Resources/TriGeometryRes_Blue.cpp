#include "StdAfx.h"
#include "TriGeometryRes.h"
#include "Tr2Mesh.h"
#include "Utilities/BoundingSphere.h"

BLUE_DEFINE( TriGeometryRes );

static Be::VarChooser TriGeometryCollisionResultFlagsChooser[] =
{
	{
		"ANY",     
		BeCast( COLLISION_RESULT_ANY ),     
		"Collision function will return the first intersection it finds"
	},
	{
		"CLOSEST",     
		BeCast( COLLISION_RESULT_CLOSEST ),     
		"Collision function will return the closest intersection to ray origin"
	},
	{ 0 },
};

BLUE_REGISTER_ENUM_EX( "TriGeometryCollisionResultFlags", 
					  TriGeometryCollisionResultFlags, 
					  TriGeometryCollisionResultFlagsChooser, 
					  ENUM_REG_ENUM_OBJECT_ON_MODULE );

static Be::VarChooser TriGeometryCollisionCullingFlagsChooser[] =
{
	{
		"CCW",     
		BeCast( COLLISION_CULL_CCW ),     
		"CCW culling"
	},
	{
		"CW",     
		BeCast( COLLISION_CULL_CW ),     
		"CW culling"
	},
	{
		"NONE",     
		BeCast( COLLISION_CULL_NONE ),     
		"None culling"
	},
	{ 0 },
};

BLUE_REGISTER_ENUM_EX( "TriGeometryCollisionCullingFlags", 
					  TriGeometryCollisionCullingFlags, 
					  TriGeometryCollisionCullingFlagsChooser, 
					  ENUM_REG_ENUM_OBJECT_ON_MODULE );

IBlueResource* CreateStaticGeometryResource( const wchar_t* name )
{
	TriGeometryResPtr p;
	p.CreateInstance();
	p->SetIsDynamic( false );
	p->m_name = CW2A( name );
	return p.Detach();
}

IBlueResource* CreateDynamicGeometryResource( const wchar_t* name )
{
	TriGeometryResPtr p;
	p.CreateInstance();
	p->SetIsDynamic( true );
	p->m_name = CW2A( name );
	return p.Detach();
}

BLUE_REGISTER_RESOURCE_EXTENSION( L"gr2", CreateStaticGeometryResource );
BLUE_REGISTER_RESOURCE_EXTENSION( L"gr2dyn", CreateDynamicGeometryResource );


const Be::ClassInfo* TriGeometryRes::ExposeToBlue()
{
    EXPOSURE_BEGIN( TriGeometryRes, "" )
        MAP_INTERFACE( TriGeometryRes )
		MAP_INTERFACE( IBlueResource )
		MAP_INTERFACE( ICacheable )
		MAP_INTERFACE( ITr2InstanceData )
		MAP_INTERFACE( ITr2GpuBuffer )
		MAP_ICACHEABLE_METHODS()

		MAP_ATTRIBUTE( "isDynamicGeometry", m_isDynamicGeometry, "is for dynamic geometry (cpu skinning)", Be::READ )
		MAP_PROPERTY_READONLY
		(
			"modelCount",
			GetModelCount,
			"Gets the count of models in this geometry resource"
		)
		MAP_PROPERTY_READONLY
		(
			"meshCount",
			GetMeshCount,
			"Gets the count of meshes in this geometry resource"
		)
		MAP_PROPERTY_READONLY
		(
			"animationCount",
			GetAnimationCount,
			"Gets the count of animations in this geometry resource"
		)
		MAP_ATTRIBUTE( "name", m_name, "Name for debugging/logging", Be::READWRITE )

		MAP_METHOD_AND_WRAP( 
			"GetMeshSurfaceArea", 
			GetMeshSurfaceArea,
			"Gets the surface area for a particular meshID"
			"\n:param meshID: the mesh to calculate the surface area for")
		
		MAP_METHOD_AND_WRAP
		(
			"GetModelCount", 
			GetModelCount, 
			"Gets the count of models"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetModelName", 
			GetModelName, 
			"Gets the name of the indexed model\n"
			":param idx: model index"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetMeshCount", 
			GetMeshCount, 
			"Gets the count of meshes"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetMeshName", 
			GetMeshName, 
			"Gets the name of the indexed mesh\n"
			":param idx: mesh index"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetMeshAreaCount",
			GetMeshAreaCount, 
			"Gets the count of areas within the indexed mesh\n"
			":param idx: mesh index"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetMeshAreaName", 
			GetMeshAreaName, 
			"Gets the name of the indexed area within the indexed mesh\n"
			":param meshIdx: mesh index\n"
			":param areaIdx: area index\n"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetAreaBoundingBox", 
			GetAreaBoundingBoxFromScript, 
			"Get the bounding box of the specified area within the mesh\n"
			":param meshIdx: mesh index\n"
			":param areaIdx: area index\n"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetBoundingBox", 
			GetBoundingBoxFromScript, 
			"Get the bounding box of the specified mesh\n"
			":param meshIdx: mesh index\n"
		)
		MAP_METHOD_AND_WRAP(
			"GetBoundingSphere", 
			GetBoundingSphereFromScript,
			"Get the bounding sphere of the specified area\n"
			":param meshIdx: mesh index\n"
		)
		MAP_METHOD_AND_WRAP
		(
			"CalculateBoundingBoxFromTransform", 
			CalculateBoundingBoxFromTransform, 
			"Get the mesh's bounding box with all points transformed using transform.\n"
			":param meshIdx: mesh index\n"
			":param transform: transform matrix\n"
		)
		MAP_METHOD_AND_WRAP
		(
			"RecalculateBoundingSphere",
			RecalculateBoundingSphere,
			"Rebuild bounding sphere info for these meshes"
		)

		MAP_METHOD_AND_WRAP
		( 
			"Reload", 
			Reload, 
			"Forces a reload from disk"
		)
		
		MAP_METHOD_AND_WRAP
		(
			"GetRayAreaIntersection", 
			GetRayAreaIntersectionFromScript, 
			"Perform ray - area geometry intersection test"
			"\nReturns (position, uv) tuple with intersection position and texture UV coordinates "
			"or None if no intersection is found"
			"\n:param origin: Ray origin (3-tuple)"
			"\n:param direction: Ray direction (3-tuple)"
			"\n:param meshIndex: Mesh index"
			"\n:param reaIndex: Area index"
			"\n:param resultFlags: Result flags (member of TriGeometryCollisionResultFlags), closest collision by default"
			"\n:param cullingFlags: Culling flags (member of TriGeometryCollisionCullingFlags), no culling by default"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetIntersectionPointAndNormal", 
			GetIntersectionPointAndNormalFromScript,
			"( pos, dir ) ->( near, far )\nGet the near intersection points and the normal between a ray and the geometry.\n"
			":param pos: ray origin\n"
			":param direction: ray direction\n"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetIntersectionPointNormalBone", 
			GetIntersectionPointNormalBoneFromScript,
			"( pos, dir ) ->( near, far )\nGet the near intersection points and the normal between a ray and the geometry.\n"
			":param pos: ray origin\n"
			":param direction: ray direction\n"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetAreaIntersectionPointNormalBone", 
			GetAreaIntersectionPointNormalBoneFromScript,
			"( pos, dir ) ->( near, far )\nGet the near intersection points and the normal between a ray and the geometry.\n"
			":param pos: ray origin\n"
			":param direction: ray direction\n"
			":param areaIx: the mesh area index\n"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetClosestVertex", 
			GetClosestVertex,
			"( pos ) ->( dist, pos )\nGet the closest vertex of this model to the given point.\n"
			":param pos: position"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetMeshVertexElements",
			GetMeshVertexElements,
			"Returns a list of (usage, usage index) tuples for vertex layout of the specified mesh.\n"
			":param idx: mesh index"
		)
    EXPOSURE_CHAINTO( BlueAsyncRes )
}

void SaveMeshDataToGrannyFile(	Tr2Mesh* mesh,
								char const* resPath,
								char const* rawPath )
{
	TriGeometryRes* geometryRes = mesh->GetGeometryResource();
	TriGeometryResMeshData* meshData = geometryRes->GetMeshData( mesh->GetMeshIndex() );
	TriGeometryRes::SaveMeshToGrannyFile( meshData, rawPath );
	mesh->DeferGeometryLoad( true );
	mesh->SetMeshResPath( resPath );
	mesh->DeferGeometryLoad( false );
}

#if BLUE_WITH_PYTHON
static PyObject* PySaveMeshDataToGrannyFile( PyObject* self, PyObject* args )
{
	PyObject* pyMesh = NULL;
	char const* resPath = NULL;
	char const* rawPath = NULL;

	if( PyArg_ParseTuple( args, "Oss", &pyMesh, &resPath, &rawPath ) )
	{
		if( Tr2Mesh* mesh = BluePythonCast<Tr2Mesh*>( pyMesh ) )
		{
			SaveMeshDataToGrannyFile( mesh, resPath, rawPath );
			return PyBool_FromLong( true );
		}
	}

	return PyBool_FromLong( false );
}

MAP_FUNCTION( "SaveMeshDataToGrannyFile",
              PySaveMeshDataToGrannyFile,
              "Saves mesh data to a Granny file.\n"
			  ":param mesh: mesh\n"
			  ":type mesh: Tr2Mesh\n"
			  ":param resPath: res path\n"
			  ":type resPath: str\n"
			  ":param rawPath: raw path\n"
			  ":type rawPath: str\n"
			  ":rtype: bool"
			  );

#endif

namespace
{
	std::pair<Vector3, float> BoundingSphereFromPointsScript( const std::vector<Vector3>& points )
	{
		Vector4 sphere( 0, 0, 0, 0 );
		if( points.empty() )
		{
			::BoundingSphereFromPoints( sphere, nullptr, 0 );
		}
		else
		{
			std::vector<const Vector3*> pointers;
			pointers.reserve( points.size() );
			auto ptr = &points[0];
			for( size_t i = 0; i < points.size(); ++i )
			{
				pointers.push_back( ptr++ );
			}
			::BoundingSphereFromPoints( sphere, &pointers[0], points.size() );
		}
		return std::make_pair( Vector3( sphere.x, sphere.y, sphere.z ), sphere.w );
	}
}

MAP_FUNCTION_AND_WRAP(
	"BoundingSphereFromPoints",
	BoundingSphereFromPointsScript,
	"Generates bounding sphere around a given points.\n"
	":param points: list of 3D points in space" );