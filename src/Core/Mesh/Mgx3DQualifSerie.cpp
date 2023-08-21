/*
 * \file		Mgx3DQualifSerie.t.h
 * \author		Charles PIGNEROL
 * \date		02/08/2013
 */
#include "Mesh/Mgx3DQualifSerie.h"

#include "Utils/Common.h"

#include <assert.h>


namespace Mgx3D
{

namespace Mesh
{

Mgx3DQualifSerie::Mgx3DQualifSerie (
	gmds::IGMesh::surface& s, const std::string& name,
	const std::string& fileName, Mgx3D::Mesh::MeshEntity* me)
	: GQualif::GMDSQualifSerie (s, name, fileName), _meshEntity (me)
{
}	// Mgx3DQualifSerie::Mgx3DQualifSerie


Mgx3DQualifSerie::Mgx3DQualifSerie (
	 gmds::IGMesh::volume& v, const std::string& name,
	const std::string& fileName, Mgx3D::Mesh::MeshEntity* me)
	: GQualif::GMDSQualifSerie (v, name, fileName), _meshEntity (me)
{
}	// Mgx3DQualifSerie::Mgx3DQualifSerie


Mgx3DQualifSerie::Mgx3DQualifSerie (const Mgx3DQualifSerie&)
	: GQualif::GMDSQualifSerie (
			*new gmds::IGMesh::surface (0, ""),
			"Invalid serie name", "Invalid file name")
{
	MGX_FORBIDDEN ("Mgx3DQualifSerie copy constructor is not allowed.")
}	// Mgx3DQualifSerie::Mgx3DQualifSerie


Mgx3DQualifSerie& Mgx3DQualifSerie::operator = (const Mgx3DQualifSerie&)
{
	MGX_FORBIDDEN ("Mgx3DQualifSerie assignment operator is not allowed.")
	return *this;
}	// Mgx3DQualifSerie::operator =


Mgx3DQualifSerie::~Mgx3DQualifSerie ( )
{
	_meshEntity	= 0;
}	// Mgx3DQualifSerie::~Mgx3DQualifSerie


void Mgx3DQualifSerie::getGMDSCellsIndexes (
						std::vector<gmds::TCellID>& indexes, size_t clas) const
{
	const std::vector<size_t>&	idx		= getClassesCellsIndexes ( )[clas];
	const size_t				size	= idx.size ( );
	indexes.resize (size);
	for (size_t i = 0; i < size; i++)
		indexes [i]	= idx [i];
}	// Mgx3DQualifSerie::getGMDSCellsIndexes 


MeshEntity* Mgx3DQualifSerie::getMeshEntity ( )
{
	return _meshEntity;
}	// Mgx3DQualifSerie::getMeshEntity


const MeshEntity* Mgx3DQualifSerie::getMeshEntity ( ) const
{
	return _meshEntity;
}	// Mgx3DQualifSerie::getMeshEntity


}	// namespace Mesh

}	// namespace Mgx3D
