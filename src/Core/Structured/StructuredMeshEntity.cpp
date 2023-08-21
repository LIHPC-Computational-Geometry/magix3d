/**
 * \file		StructuredMeshEntity.cpp
 * \author		Charles PIGNEROL
 * \date		04/12/2018
 */


#include "Structured/StructuredMeshEntity.h"
#include "Utils/Common.h"

#include <TkUtil/MemoryError.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;



namespace Mgx3D
{

namespace Structured
{

StructuredMeshEntity::StructuredMeshEntity (Context& context, Property* prop, DisplayProperties* disp, Structured::/*??*/StructuredMesh& mesh, const vector<Material*>& materials)
	: InternalEntity (context, prop, disp), _mesh (&mesh), _materials (materials)
{
}	// StructuredMeshEntity::StructuredMeshEntity


StructuredMeshEntity::StructuredMeshEntity (const StructuredMeshEntity& sme)
	: InternalEntity (sme), _mesh (0), _materials ( )
{
	MGX_FORBIDDEN ("StructuredMeshEntity copy constructor is not allowed.")
}	// StructuredMeshEntity::StructuredMeshEntity


StructuredMeshEntity& StructuredMeshEntity::operator = (const StructuredMeshEntity&)
{
	MGX_FORBIDDEN ("StructuredMeshEntity assignment operator is not allowed.")
	return *this;
}	// StructuredMeshEntity::operator =


StructuredMeshEntity::~StructuredMeshEntity ( )
{
	delete _mesh;	_mesh	= 0;
	for (vector<Material*>::iterator itm = _materials.begin ( ); _materials.end ( ) != itm; itm++)
		delete *itm;
	_materials.clear ( );
}	// StructuredMeshEntity::~StructuredMeshEntity


string StructuredMeshEntity::getTypeName ( ) const
{
	return getMesh ( ).type ( );
}	// StructuredMeshEntity::getTypeName


Entity::objectType StructuredMeshEntity::getType ( ) const
{
	return Entity::StructuredMesh;
}	// StructuredMeshEntity::getType


const StructuredMesh& StructuredMeshEntity::getMesh ( ) const
{
	CHECK_NULL_PTR_ERROR (_mesh)
	return *_mesh;
}	// StructuredMeshEntity::getMesh


StructuredMesh& StructuredMeshEntity::getMesh ( )
{
	CHECK_NULL_PTR_ERROR (_mesh)
	return *_mesh;
}	// StructuredMeshEntity::getMesh


const StructuredMesh* StructuredMeshEntity::operator -> ( ) const
{
	CHECK_NULL_PTR_ERROR (_mesh)
	return _mesh;
}	// StructuredMeshEntity::operator ->


StructuredMesh* StructuredMeshEntity::operator -> ( )
{
	CHECK_NULL_PTR_ERROR (_mesh)
	return _mesh;
}	// StructuredMeshEntity::operator ->


}	// namespace Structured

}	// namespace Mgx3D


