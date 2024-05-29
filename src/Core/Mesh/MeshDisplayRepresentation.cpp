/*
 * \file MeshDisplayRepresentation
 *
 *  \author Charles Pignerol
 *
 *  \date 22/05/2012
 */
/*----------------------------------------------------------------------------*/

#include "Mesh/MeshDisplayRepresentation.h"
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace Mesh
{


MeshDisplayRepresentation::MeshDisplayRepresentation (
					DisplayRepresentation::type t, unsigned long decimationStep)
	: DisplayRepresentation (t, DisplayRepresentation::DISPLAY_MESH),
	  _cells (new std::vector<size_t> ( )), _skin (false),
	  _decimationStep (decimationStep)
{
//	std::cout<<"MeshDisplayRepresentation avec decimationStep = "<<decimationStep<<std::endl;
}	// MeshDisplayRepresentation::MeshDisplayRepresentation


MeshDisplayRepresentation::MeshDisplayRepresentation (
										const MeshDisplayRepresentation& mdr)
	: DisplayRepresentation (mdr),
	  _cells (new std::vector<size_t> ( )),
	  _skin (mdr._skin),
	  _decimationStep (mdr._decimationStep)
{
	if (0 != mdr._cells)
	{
		delete _cells;
		_cells	= new vector<size_t> (*(mdr._cells));
	}	// if (0 != mdr._cells)
}	// MeshDisplayRepresentation::MeshDisplayRepresentation


MeshDisplayRepresentation& MeshDisplayRepresentation::operator = (
										const MeshDisplayRepresentation& mdr)
{
	if (&mdr != this)
	{
		DisplayRepresentation::operator = (mdr);
		delete _cells;
		_cells				= new vector<size_t> (*(mdr._cells));
		_skin				= mdr._skin;
		_decimationStep		= mdr._decimationStep;
	}	// if (&mdr != this)

	return *this;
}	// MeshDisplayRepresentation::operator =


MeshDisplayRepresentation::~MeshDisplayRepresentation ( )
{
	delete _cells;			_cells	= 0;
}	// MeshDisplayRepresentation::~MeshDisplayRepresentation


void MeshDisplayRepresentation::setCells (std::vector<size_t>* cells, bool skin)
{
	_skin	= skin;
	if (cells != _cells)
	{
		delete _cells;
		_cells	= cells;
	}	// if (cells != _cells)
}	// MeshDisplayRepresentation::setCells


const std::vector<size_t>& MeshDisplayRepresentation::getCells ( ) const
{
	CHECK_NULL_PTR_ERROR (_cells)
	return *_cells;
}	// MeshDisplayRepresentation::getCells


void MeshDisplayRepresentation::setPoints2nodesID (std::map<int,int> points2nodesID)
{
	_points2nodesID = points2nodesID;
}	// MeshDisplayRepresentation::setPoints2nodesID


std::map<int,int> MeshDisplayRepresentation::getPoints2nodesID () const
{
	return _points2nodesID;
}	// MeshDisplayRepresentation::getPoints2nodesID


void MeshDisplayRepresentation::setSkinDisplayed (bool display)
{
	_skin	= display;
}	// MeshDisplayRepresentation::setSkinDisplayed


bool MeshDisplayRepresentation::skinDisplayed ( ) const
{
	return _skin;
}	// MeshDisplayRepresentation::skinDisplayed


void MeshDisplayRepresentation::setDecimationStep (unsigned long decimationStep)
{
	_decimationStep	= decimationStep;
}	// MeshDisplayRepresentation::setDecimationStep


unsigned long MeshDisplayRepresentation::getDecimationStep ( ) const
{
	return _decimationStep;
}	// MeshDisplayRepresentation::getDecimationStep


}	// namespace Mesh

}	// namespace Mgx3D
