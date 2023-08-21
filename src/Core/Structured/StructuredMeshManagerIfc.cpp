/*
 * \file StructuredMeshManagerIfc.cpp
 *
 * \author	Charles Pignerol
 *
 * \date	03/12/2018
 */

#include "Structured/StructuredMeshManagerIfc.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace Structured
{


StructuredMeshManagerIfc::StructuredMeshManagerIfc (const std::string& name, Mgx3D::Internal::ContextIfc* c)
	: CommandCreator (name, c)
{
}	// StructuredMeshManagerIfc::StructuredMeshManagerIfc


StructuredMeshManagerIfc::StructuredMeshManagerIfc (const StructuredMeshManagerIfc& mm)
	: CommandCreator (mm)
{
	MGX_FORBIDDEN ("StructuredMeshManagerIfc copy constructor is not allowed.")
}	// StructuredMeshManagerIfc::StructuredMeshManagerIfc


StructuredMeshManagerIfc& StructuredMeshManagerIfc::operator = (const StructuredMeshManagerIfc& mm)
{
	MGX_FORBIDDEN ("StructuredMeshManagerIfc assignment operator is not allowed.")
	return *this;
}	// StructuredMeshManagerIfc::operator =


StructuredMeshManagerIfc::~StructuredMeshManagerIfc ( )
{
}	// StructuredMeshManagerIfc::~StructuredMeshManagerIfc


Mgx3D::Internal::M3DCommandResultIfc* StructuredMeshManagerIfc::loadHIC (const string& path, const string& type, size_t procNum)
{
    throw Exception ("StructuredMeshManagerIfc::loadHIC should be overloaded.");
}	// StructuredMeshManagerIfc::loadHIC


Mgx3D::Internal::M3DCommandResultIfc* StructuredMeshManagerIfc::releaseMesh ( )
{
    throw Exception ("StructuredMeshManagerIfc::releaseMesh should be overloaded.");
}	// StructuredMeshManagerIfc::releaseMesh


void StructuredMeshManagerIfc::setMeshEntity (size_t procNum, StructuredMeshEntity*& mesh)
{
	throw Exception ("StructuredMeshManagerIfc::setMeshEntity should be overloaded.");
}	// StructuredMeshManagerIfc::setMeshEntity


void StructuredMeshManagerIfc::releaseMeshEntities ( )
{
	throw Exception ("StructuredMeshManagerIfc::releaseMeshEntities should be overloaded.");
}	// StructuredMeshManagerIfc::releaseMeshEntities


const StructuredMeshEntity& StructuredMeshManagerIfc::getMeshEntity (size_t procNum) const
{
	throw Exception ("StructuredMeshManagerIfc::getMeshEntity should be overloaded.");
}	// StructuredMeshManagerIfc::getMeshEntity


StructuredMeshEntity& StructuredMeshManagerIfc::getMeshEntity (size_t procNum)
{
	throw Exception ("StructuredMeshManagerIfc::getMeshEntity should be overloaded.");
}	// StructuredMeshManagerIfc::getMeshEntity


const StructuredMesh& StructuredMeshManagerIfc::getMesh (size_t procNum) const
{
	throw Exception ("StructuredMeshManagerIfc::getMesh should be overloaded.");
}	// StructuredMeshManagerIfc::getMesh


StructuredMesh& StructuredMeshManagerIfc::getMesh (size_t procNum)
{
	throw Exception ("StructuredMeshManagerIfc::getMesh should be overloaded.");
}	// StructuredMeshManagerIfc::getMesh


vector<StructuredMeshEntity*> StructuredMeshManagerIfc::getMeshEntities ( ) const
{
	throw Exception ("StructuredMeshManagerIfc::getMeshEntities should be overloaded.");
}	// StructuredMeshManagerIfc::getMeshEntities


}	// namespace Structured

}	// namespace Mgx3D

