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

template<typename TCellType, unsigned char Dim>
Mgx3DQualifSerie<TCellType, Dim>::Mgx3DQualifSerie (
	const std::vector<TCellType>& v, const std::string& name,
	const std::string& fileName, Mgx3D::Mesh::MeshEntity* me)
	: GQualif::GMDSQualifSerie<TCellType, Dim> (v, name, fileName), _meshEntity (me)
{
}	// Mgx3DQualifSerie::Mgx3DQualifSerie

template<typename TCellType, unsigned char Dim>
Mgx3DQualifSerie<TCellType, Dim>::Mgx3DQualifSerie (const Mgx3DQualifSerie&)
	: GQualif::GMDSQualifSerie<TCellType, Dim> (
			*new std::vector<TCellType>(),
			"Invalid serie name", "Invalid file name")
{
	MGX_FORBIDDEN ("Mgx3DQualifSerie copy constructor is not allowed.")
}	// Mgx3DQualifSerie::Mgx3DQualifSerie

template<typename TCellType, unsigned char Dim>
Mgx3DQualifSerie<TCellType, Dim>& Mgx3DQualifSerie<TCellType, Dim>::operator = (const Mgx3DQualifSerie<TCellType, Dim>&)
{
	MGX_FORBIDDEN ("Mgx3DQualifSerie assignment operator is not allowed.")
	return *this;
}	// Mgx3DQualifSerie::operator =

template<typename TCellType, unsigned char Dim>
Mgx3DQualifSerie<TCellType, Dim>::~Mgx3DQualifSerie ( )
{
	_meshEntity	= 0;
}	// Mgx3DQualifSerie::~Mgx3DQualifSerie

template<typename TCellType, unsigned char Dim>
void Mgx3DQualifSerie<TCellType, Dim>::getGMDSCellsIndexes (std::vector<gmds::TCellID>& indexes, size_t clas) const
{
	const std::vector<size_t>&	idx		= this->getClassesCellsIndexes ( )[clas];
	const size_t				size	= idx.size ( );
	indexes.resize (size);
	for (size_t i = 0; i < size; i++)
		indexes [i]	= idx [i];
}	// Mgx3DQualifSerie::getGMDSCellsIndexes 

template<typename TCellType, unsigned char Dim>
MeshEntity* Mgx3DQualifSerie<TCellType, Dim>::getMeshEntity ( )
{
	return _meshEntity;
}	// Mgx3DQualifSerie::getMeshEntity

template<typename TCellType, unsigned char Dim>
const MeshEntity* Mgx3DQualifSerie<TCellType, Dim>::getMeshEntity ( ) const
{
	return _meshEntity;
}	// Mgx3DQualifSerie::getMeshEntity

template class Mgx3DQualifSerie<gmds::Face, 2>;  // Explicit instantiation
template class Mgx3DQualifSerie<gmds::Region, 3>;  // Explicit instantiation

}	// namespace Mesh

}	// namespace Mgx3D
