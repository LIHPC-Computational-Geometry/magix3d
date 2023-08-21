/*----------------------------------------------------------------------------*/
/*
 * \file Line.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/08/16
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/Line.h"
#include "Mesh/MeshDisplayRepresentation.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Common.h"
#include "Utils/Bounds.h"
#include "Utils/SerializedRepresentation.h"
#include "Internal/InfoCommand.h"

#include "TkUtil/Exception.h"
#include "TkUtil/InternalError.h"
#include <TkUtil/NumericConversions.h>
#include <TkUtil/MemoryError.h>
#include <memory>           // unique_ptr

#include "GMDS/IG/Node.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
const char* Line::typeNameMeshLine = "MeshLine";
/*----------------------------------------------------------------------------*/
Line::Line(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp)
: MeshEntity(ctx, prop, disp)
, m_topo_property(new Topo::MeshLineTopoProperty())
, m_save_topo_property(0)
{
}
/*----------------------------------------------------------------------------*/
Line::~Line()
{
	delete m_topo_property;
}
/*----------------------------------------------------------------------------*/
Line::
Line(const Line& v)
: MeshEntity(getContext(),0,0)
, m_topo_property(0)
, m_save_topo_property(0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Line & Line::
operator = (const Line& cl)
{
    if (&cl != this){
        MGX_NOT_YET_IMPLEMENTED("Opérateur de copie");
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
void Line::getBounds (double bounds[6]) const
{
	Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
	Mesh::MeshImplementation*	meshImpl	=
							dynamic_cast<Mesh::MeshImplementation*> (meshItf);
	if (0 == meshImpl)
	{
		INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "Line::getBounds")
	}

    // Récupération du groupe GMDS
    std::vector<gmds::Node>   cells;
    getGMDSNodes(cells);

	std::vector<gmds::Node>::iterator ic = cells.begin();

	// utilisation de l'objet Bounds
	Utils::Bounds bnds((*ic).X(), (*ic).Y(), (*ic).Z());
	ic++;

	for (; ic != cells.end(); ++ic){
	    bnds.add((*ic).X(), (*ic).Y(), (*ic).Z());
	}

	bnds.get(bounds);
}
/*----------------------------------------------------------------------------*/
void Line::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire comme cela ...");
    // cf VTKGMDSEntityRepresentation
}
/*----------------------------------------------------------------------------*/
bool Line::isA(std::string& name)
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire un tel test");
//    return (name.compare(0,strlen(typeNameMeshLine),typeNameMeshLine) == 0);
    return false;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Line & cl)
{
    o << cl.getName() << " (uniqueId " << cl.getUniqueId() << ", Name "<<cl.getName()<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
void Line::addCoEdge(Topo::CoEdge* ed)
{
    m_topo_property->getCoEdgeContainer().add(ed);
}
/*----------------------------------------------------------------------------*/
void Line::removeCoEdge(Topo::CoEdge* ed)
{
    m_topo_property->getCoEdgeContainer().remove(ed, true);
}
/*----------------------------------------------------------------------------*/
void Line::getCoEdges(std::vector<Topo::CoEdge* >& edges) const
{
    m_topo_property->getCoEdgeContainer().checkIfDestroyed();
    m_topo_property->getCoEdgeContainer().get(edges);
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Line::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            MeshEntity::getDescription (alsoComputed));

    // le maillage vu depuis les arêtes et celui stocké dans GMDS
    Utils::SerializedRepresentation  meshProprietes ("Propriétés du maillage", "");

    std::vector<Topo::CoEdge* > coedges;
    getCoEdges(coedges);

    uint nbEdges = 0;
    for (uint i=0; i<coedges.size(); i++){
        Topo::CoEdge* ce = coedges[i];
        nbEdges += ce->edges().size();
    }
    meshProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Nombre de bras des arêtes", (long)nbEdges));

    description->addPropertiesSet(meshProprietes);

    // les liens vers les arêtes
    if (!coedges.empty()){
        Utils::SerializedRepresentation aretes ("Arêtes topologiques",
                TkUtil::NumericConversions::toStr((short)coedges.size()));
        for (uint i=0; i<coedges.size(); i++){
            Topo::CoEdge* ce = coedges[i];
            aretes.addProperty (
                    Utils::SerializedRepresentation::Property (
                            ce->getName ( ), *ce));
        }

        description->addPropertiesSet (aretes);
    }

    return description.release ( );

}
/*----------------------------------------------------------------------------*/
void Line::getGMDSEdges(std::vector<gmds::Edge >& AEdges) const
{
    AEdges.clear();

    std::vector<Topo::CoEdge* > coEdges;
    getCoEdges(coEdges);

    Mesh::MeshItf*              meshItf     = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl    =
                                dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::IGMesh& gmdsMesh = meshImpl->getGMDSMesh();

     for(unsigned int iCoEdge=0; iCoEdge<coEdges.size(); iCoEdge++) {
        std::vector<gmds::TCellID> edges  = coEdges[iCoEdge]->edges();

        for(unsigned int iEdge=0; iEdge<edges.size(); iEdge++) {
        	AEdges.push_back(gmdsMesh.get<gmds::Edge>(edges[iEdge]));
        }
    }
}
/*----------------------------------------------------------------------------*/
void Line::getGMDSNodes(std::vector<gmds::Node >& ANodes) const
{
	std::map<gmds::TCellID, uint> filtre;
	std::vector<gmds::Edge> AEdges;

    Mesh::MeshItf*              meshItf     = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl    =
                                dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::IGMesh& gmdsMesh = meshImpl->getGMDSMesh();

    for(std::vector<gmds::Edge>::const_iterator iter=AEdges.begin();
			iter!=AEdges.end();++iter) {
		std::vector<gmds::TCellID> nodes  = (iter)->getAllIDs<gmds::Node>();
		for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
			if (filtre[nodes[iNode]] == 0){
				ANodes.push_back(gmdsMesh.get<gmds::Node>(nodes[iNode]));
				filtre[nodes[iNode]] = 1;
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
void Line::
saveMeshLineTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool new_entity = icmd->addMeshInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (!new_entity && m_save_topo_property == 0)
            m_save_topo_property = m_topo_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
Topo::MeshLineTopoProperty* Line::
setProperty(Topo::MeshLineTopoProperty* prop)
{
    Topo::MeshLineTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Line::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_topo_property) {
        ccm->addLineInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
