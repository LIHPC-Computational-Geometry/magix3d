/**
 * \file        GraphicalEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        24/11/2011
 */

#include "Utils/GraphicalEntityRepresentation.h"

#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace Utils
{


// ===========================================================================
//                      LA CLASSE GraphicalEntityRepresentation
// ===========================================================================


const unsigned long	GraphicalEntityRepresentation::CLOUDS		= 1UL;
const unsigned long	GraphicalEntityRepresentation::CURVES		= CLOUDS << 1UL;
const unsigned long	GraphicalEntityRepresentation::ISOCURVES	= CURVES << 1UL;
const unsigned long	GraphicalEntityRepresentation::SURFACES		= ISOCURVES << 1UL;
const unsigned long	GraphicalEntityRepresentation::VOLUMES		= SURFACES << 1UL;
const unsigned long	GraphicalEntityRepresentation::NAMES		= VOLUMES << 1UL;
const unsigned long	GraphicalEntityRepresentation::MESH_SHAPE	= NAMES << 1UL;
const unsigned long	GraphicalEntityRepresentation::ASSOCIATIONS	= MESH_SHAPE << 1UL;
const unsigned long	GraphicalEntityRepresentation::TRIHEDRON	= ASSOCIATIONS << 1UL;
const unsigned long	GraphicalEntityRepresentation::DISCRETISATIONTYPE	= TRIHEDRON << 1UL;
const unsigned long	GraphicalEntityRepresentation::NBMESHINGEDGE	= DISCRETISATIONTYPE << 1UL;
const unsigned long	GraphicalEntityRepresentation::NODES_VALUES	= NBMESHINGEDGE << 1UL;
const unsigned long	GraphicalEntityRepresentation::CELLS_VALUES	= NODES_VALUES << 1UL;

GraphicalEntityRepresentation::GraphicalEntityRepresentation (Entity& entity)
	: Mgx3D::Utils::DisplayProperties::GraphicalRepresentation ( ),
	  _entity (&entity)
{
}	// GraphicalEntityRepresentation::GraphicalEntityRepresentation


GraphicalEntityRepresentation::GraphicalEntityRepresentation (const GraphicalEntityRepresentation&)
	: Mgx3D::Utils::DisplayProperties::GraphicalRepresentation ( ),
	  _entity (0)
{
	MGX_FORBIDDEN ("GraphicalEntityRepresentation copy constructor is not allowed.");
}	// GraphicalEntityRepresentation::GraphicalEntityRepresentation


GraphicalEntityRepresentation& GraphicalEntityRepresentation::operator = (const GraphicalEntityRepresentation& er)
{
	MGX_FORBIDDEN ("GraphicalEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
		Mgx3D::Utils::DisplayProperties::GraphicalRepresentation::operator = (er);
	}	// if (&er != this)

	return *this;
}	// GraphicalEntityRepresentation::operator =


GraphicalEntityRepresentation::~GraphicalEntityRepresentation ( )
{
}	// GraphicalEntityRepresentation::~GraphicalEntityRepresentation


unsigned long GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::objectType entitiesType)
{
	switch (entitiesType)
	{
		case Entity::GeomVertex		:
		case Entity::TopoVertex		:
		case Entity::MeshCloud		:
			return CLOUDS;
		break;
		case Entity::GeomCurve		:
		case Entity::GeomVolume		:
		case Entity::TopoCoEdge		:
		case Entity::TopoEdge		:
		case Entity::TopoCoFace		:
		case Entity::TopoFace		:
		case Entity::TopoBlock		:
		case Entity::MeshLine		:
			return CURVES;
        	case Entity::GeomSurface    :
            		return CURVES | ISOCURVES;
		case Entity::MeshSurface	:
		case Entity::MeshSubSurface	:
		case Entity::MeshVolume		:
			return CURVES | SURFACES;
        	case Entity::MeshSubVolume     :
            		return CURVES | VOLUMES;
        	case Entity::SysCoord :
        		return TRIHEDRON;
		case Entity::StructuredMesh	:
			return VOLUMES | CELLS_VALUES;
	}	// switch (entitiesType)

	UTF8String	message (Charset::UTF_8);
	message << "Erreur dans GraphicalEntityRepresentation::getDefaultRepresentationMask."
	        << "Type d'entités non supporté : "
	        << Entity::objectTypeToObjectTypeName (entitiesType) << ".";
	throw Exception (message);
}	// GraphicalEntityRepresentation::getDefaultRepresentationMask


unsigned long GraphicalEntityRepresentation::getVisibleTypeByEntityMask (Entity::objectType entitiesType)
{
    switch (entitiesType)
    {
        case Entity::GeomVertex     :
            return CLOUDS | NAMES;
        case Entity::GeomCurve      :
            return CURVES | NAMES;
        case Entity::GeomSurface    :
        case Entity::GeomVolume     :
            return CURVES | ISOCURVES | SURFACES | NAMES;

        case Entity::TopoVertex     :
            return CLOUDS | NAMES | ASSOCIATIONS;
        case Entity::TopoCoEdge     :
            return CLOUDS | CURVES | NAMES | ASSOCIATIONS | MESH_SHAPE | DISCRETISATIONTYPE | NBMESHINGEDGE;
        case Entity::TopoEdge       :
            return CURVES | NAMES;
        case Entity::TopoCoFace     :
            return CURVES | SURFACES | NAMES | ASSOCIATIONS | DISCRETISATIONTYPE;
        case Entity::TopoFace       :
            return CURVES | SURFACES | NAMES;
        case Entity::TopoBlock      :
            return CURVES | SURFACES | NAMES | DISCRETISATIONTYPE;

        case Entity::MeshCloud      :
            return CLOUDS | NAMES | NODES_VALUES;
        case Entity::MeshLine      :
            return CLOUDS | CURVES | NAMES ;
        case Entity::MeshSurface    :
        case Entity::MeshSubSurface :
            return CLOUDS | CURVES | SURFACES | NAMES | NODES_VALUES | CELLS_VALUES;
        case Entity::MeshVolume     :
        case Entity::MeshSubVolume  :
            return CLOUDS | CURVES | SURFACES | VOLUMES | NAMES | NODES_VALUES | CELLS_VALUES;

	case Entity::StructuredMesh	:
		return VOLUMES | NODES_VALUES | CELLS_VALUES;	// A affiner

        case Entity::Group0D        :
            return CLOUDS | NAMES;
        case Entity::Group1D        :
            return CLOUDS | CURVES | NAMES;
        case Entity::Group2D        :
            return CLOUDS | CURVES | SURFACES | NAMES;
        case Entity::Group3D        :
            return CLOUDS | CURVES | SURFACES | VOLUMES | NAMES;

        case Entity::SysCoord :
        	return TRIHEDRON;

    }   // switch (entitiesType)

    UTF8String   message (Charset::UTF_8);
    message << "Erreur dans GraphicalEntityRepresentation::getTypeVisibleByEntityMask."
            << "Type d'entités non supporté : "
            << Entity::objectTypeToObjectTypeName (entitiesType) << ".";
    throw Exception (message);
}   // GraphicalEntityRepresentation::getTypeVisibleByEntityMask


bool GraphicalEntityRepresentation::displayTextualRepresentation (unsigned long mask)
{
	if ((0 != (NAMES & mask)) || (0 != (NBMESHINGEDGE & mask)))
		return true;

	return false;
}	// GraphicalEntityRepresentation::displayTextualRepresentation


string GraphicalEntityRepresentation::getRepresentationText (unsigned long mask) const
{
	string	text;

	if (0 != getEntity ( ))
	{
		if (0 != (NAMES & mask))
			text	= getEntity ( )->getName ( );
	}	// if (0 != getEntity ( ))

	return text;
}	// GraphicalEntityRepresentation::getRepresentationText


Math::Point GraphicalEntityRepresentation::getRepresentationTextPosition (unsigned long mask) const
{
	Math::Point	center (0., 0., 0.);

	try
	{
		if (0 != getEntity ( ))
		{
			double	bounds [6]	= { 0., 0., 0., 0., 0., 0. };
			getEntity ( )->getBounds (bounds);
			center.setX ((bounds [0] + bounds [1]) / 2.);
			center.setY ((bounds [2] + bounds [3]) / 2.);
			center.setZ ((bounds [4] + bounds [5]) / 2.);
		}	// if (0 != getEntity ( ))
	}
	catch (...)
	{
	}

	return center;
}	// GraphicalEntityRepresentation::getRepresentationTextPosition


void GraphicalEntityRepresentation::updateRepresentation (unsigned long mask, bool force)
{
	GraphicalRepresentation::updateRepresentation (mask, force);
}	// GraphicalEntityRepresentation::updateRepresentation


string GraphicalEntityRepresentation::rep3DMaskToString (unsigned long mask)
{
	if ((unsigned long)-1 == mask)
		return "Indéterminé";

	UTF8String	rep3d (Charset::UTF_8);
    if (0 != (mask & GraphicalEntityRepresentation::CLOUDS))
        rep3d << "Pts";
    if (0 != (mask & GraphicalEntityRepresentation::CURVES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Fil"; 
    }   // if (0 != (mask & GraphicalEntityRepresentation::CURVES))
    if (0 != (mask & GraphicalEntityRepresentation::ISOCURVES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "FilIso"; 
    }   // if (0 != (mask & GraphicalEntityRepresentation::ISOCURVES))
    if (0 != (mask & GraphicalEntityRepresentation::SURFACES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Fac"; 
    }   // if (0 != (mask & GraphicalEntityRepresentation::SURFACES))
    if (0 != (mask & GraphicalEntityRepresentation::VOLUMES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Vol";
    }   // if (0 != (mask & GraphicalEntityRepresentation::VOLUMES))
    if (0 != (mask & GraphicalEntityRepresentation::NAMES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Nom";
    }   // if (0 != (mask & GraphicalEntityRepresentation::NAMES))
    if (0 != (mask & GraphicalEntityRepresentation::MESH_SHAPE))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "MeshShape";
    }   // if (0 != (mask & GraphicalEntityRepresentation::MESH_SHAPE))
    if (0 != (mask & GraphicalEntityRepresentation::ASSOCIATIONS))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Ass";
    }   // if (0 != (mask & GraphicalEntityRepresentation::ASSOCIATIONS))
    if (0 != (mask & GraphicalEntityRepresentation::TRIHEDRON))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Tri";
    }   // if (0 != (mask & GraphicalEntityRepresentation::TRIHEDRON))
    if (0 != (mask & GraphicalEntityRepresentation::DISCRETISATIONTYPE))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Disc";
    }   // if (0 != (mask & GraphicalEntityRepresentation::DISCRETISATIONTYPE))
    if (0 != (mask & GraphicalEntityRepresentation::NBMESHINGEDGE))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "Nb";
    }   // if (0 != (mask & GraphicalEntityRepresentation::NBMESHINGEDGE))
    if (0 != (mask & GraphicalEntityRepresentation::NODES_VALUES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "NodesVals";
    }   // if (0 != (mask & GraphicalEntityRepresentation::NODES_VALUES))
    if (0 != (mask & GraphicalEntityRepresentation::CELLS_VALUES))
    {
        if (0 != rep3d.length ( ))
            rep3d << " | ";
        rep3d << "CellsVals";
    }   // if (0 != (mask & GraphicalEntityRepresentation::CELLS_VALUES))

	return rep3d.iso ( );
}	// GraphicalEntityRepresentation::rep3DMaskToString



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace Utils

}	// namespace Mgx3D
