/**
 * \file		StructuredMesh.cpp
 * \author		Charles PIGNEROL
 * \date		23/11/2018
 */

#include "Utils/Common.h"
#include "Structured/StructuredMesh.h"

#include <TkUtil/Exception.h>

#include <cstdint>
#include <string.h>

using namespace TkUtil;
using namespace std;

namespace Mgx3D
{

namespace Structured
{


StructuredMesh::StructuredMesh (const string& type, int64_t ni, int64_t nj, int64_t nk, bool allocate)
	: _type (type), _ni (ni), _nj (nj), _nk (nk), _cellNum (ni * nj * nk),
	  _mixedCellNum (0), _pureCellNum (0),
	  _abs (0), _ord (0), _elev (0), _pureIds (0), _mixedIds (0),
	  _densities (0), _densityMin (0.), _densityMax (0.)
{
	if (true == allocate)
	{
		_abs	= new double [_ni + 1];
		memset (_abs, 0, (_ni + 1) * sizeof (double));
		_ord	= new double [_nj + 1];
		memset (_ord, 0, (_nj + 1) * sizeof (double));
		_elev	= new double [_nk + 1];
		memset (_elev, 0, (_nk + 1) * sizeof (double));
	}	// if (true == allocate)
}	// StructuredMesh::StructuredMesh


StructuredMesh::StructuredMesh (const StructuredMesh&)
{
	MGX_FORBIDDEN ("StructuredMesh copy constructor is not allowed.");
}	// StructuredMesh::StructuredMesh


StructuredMesh& StructuredMesh::operator = (const StructuredMesh&)
{
	MGX_FORBIDDEN ("StructuredMesh assignment operator is not allowed.");
	return *this;
}	// StructuredMesh::operator =


StructuredMesh::~StructuredMesh ( )
{
	_ni					= _nj	= _nk	= 0;
	_cellNum				= 0;
	_mixedCellNum				= 0;
	_pureCellNum				= 0;
	delete [] _abs;		_abs		= 0;
	delete [] _ord;		_ord		= 0;
	delete [] _elev;	_elev		= 0;	
	delete [] _pureIds;	_pureIds	= 0;
	delete [] _mixedIds;	_mixedIds	= 0;
	delete [] _densities;	_densities	= 0;
}	// StructuredMesh::~StructuredMesh


void StructuredMesh::setPureCellNum (int64_t num)
{
	if (num < 0)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Affectation au maillage de type " << type ( ) << " d'un nombre négatif (" << num << ") de mailles pures.";
		throw Exception (error);
	}	// if (num < 0)

	_pureCellNum	= num;
	_mixedCellNum	= _cellNum - num;
	_pureIds	= new int64_t [_pureCellNum];
	memset (_pureIds, 0, _pureCellNum * sizeof (int64_t));
	_mixedIds	= new int64_t [_mixedCellNum];
	memset (_mixedIds, 0, _mixedCellNum * sizeof (int64_t));
}	// StructuredMesh::setPureCellNum


void StructuredMesh::setDensityRange (double min, double max)
{
	if ((min > max) || (min < 0.))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Domaine de densités invalide : [" << min << "-" << max << "].";
		throw Exception (message);
	}	// if ((min > max) || (min < 0.))

	_densityMin	= min;
	_densityMax	= max;
}	// StructuredMesh::setDensityRange


void StructuredMesh::setCellDensities (double* densities)
{
	if (densities != _densities)
		delete [] _densities;
	_densities	= densities; 
}	// StructuredMesh::setCellDensities


}	// namespace Structured

}	// namespace Mgx3D

