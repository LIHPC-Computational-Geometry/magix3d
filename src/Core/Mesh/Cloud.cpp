/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Mesh/Cloud.h"
#include "Mesh/MeshDisplayRepresentation.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Common.h"
#include "Utils/Bounds.h"
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <memory>           // unique_ptr
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Node.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
const char* Cloud::typeNameMeshCloud = "MeshCloud";
/*----------------------------------------------------------------------------*/
Cloud::Cloud(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp)
: MeshEntity(ctx, prop, disp)
, m_topo_property(new Topo::MeshCloudTopoProperty())
, m_save_topo_property(0)
{
}
/*----------------------------------------------------------------------------*/
Cloud::~Cloud()
{
	delete m_topo_property;
}
/*----------------------------------------------------------------------------*/
Cloud::
Cloud(const Cloud& v)
: MeshEntity(getContext(),0,0)
, m_topo_property(0)
, m_save_topo_property(0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Cloud & Cloud::
operator = (const Cloud& cl)
{
    if (&cl != this){
        MGX_NOT_YET_IMPLEMENTED("Opérateur de copie");
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
void Cloud::getBounds (double bounds[6]) const
{
	Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
	Mesh::MeshImplementation*	meshImpl	=
							dynamic_cast<Mesh::MeshImplementation*> (meshItf);
	if (0 == meshImpl)
	{
		INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "Cloud::getBounds")
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
void Cloud::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire comme cela ...");
    // cf VTKGMDSEntityRepresentation
}
/*----------------------------------------------------------------------------*/
bool Cloud::isA(std::string& name)
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire un tel test");
//    return (name.compare(0,strlen(typeNameMeshCloud),typeNameMeshCloud) == 0);
    return false;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Cloud & cl)
{
    o << cl.getName() << " (uniqueId " << cl.getUniqueId() << ", Name "<<cl.getName()<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
void Cloud::addCoEdge(Topo::CoEdge* ed)
{
    m_topo_property->getCoEdgeContainer().push_back(ed);
}
/*----------------------------------------------------------------------------*/
void Cloud::removeCoEdge(Topo::CoEdge* ed)
{
    Utils::remove(ed, m_topo_property->getCoEdgeContainer());
}
/*----------------------------------------------------------------------------*/
const std::vector<Topo::CoEdge* >& Cloud::getCoEdges() const
{
    Utils::checkIfDestroyed(m_topo_property->getCoEdgeContainer());
    return m_topo_property->getCoEdgeContainer();
}
/*----------------------------------------------------------------------------*/
void Cloud::addVertex(Topo::Vertex* vtx)
{
    m_topo_property->getVertexContainer().push_back(vtx);
}
/*----------------------------------------------------------------------------*/
void Cloud::removeVertex(Topo::Vertex* vtx)
{
    Utils::remove(vtx, m_topo_property->getVertexContainer());
}
/*----------------------------------------------------------------------------*/
const std::vector<Topo::Vertex* >& Cloud::getVertices() const
{
    Utils::checkIfDestroyed(m_topo_property->getVertexContainer());
    return m_topo_property->getVertexContainer();
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Cloud::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            MeshEntity::getDescription (alsoComputed));

    // le maillage vu depuis les arêtes et celui stocké dans GMDS
    Utils::SerializedRepresentation  meshProprietes ("Propriétés du maillage", "");

    auto coedges = getCoEdges();
    auto vertices = getVertices();

    uint nbNodes = 0;
    for (uint i=0; i<coedges.size(); i++){
        Topo::CoEdge* ce = coedges[i];
        nbNodes += ce->nodes().size();
    }
    for (uint i=0; i<vertices.size(); i++){
        nbNodes += 1;
    }
    meshProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Nombre de noeuds des arêtes ou sommets", (long)nbNodes));

    description->addPropertiesSet(meshProprietes);

    // les liens vers les sommets
    if (!vertices.empty()){
        Utils::SerializedRepresentation sommets ("Sommets topologiques",
                TkUtil::NumericConversions::toStr((short)vertices.size()));
        for (uint i=0; i<vertices.size(); i++){
            Topo::Vertex* vtx = vertices[i];
            sommets.addProperty (
                    Utils::SerializedRepresentation::Property (
                    		vtx->getName ( ), *vtx));
        }

        description->addPropertiesSet (sommets);
    }

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
void Cloud::getGMDSNodes(std::vector<gmds::Node >& ANodes) const
{
    ANodes.clear();

    Mesh::MeshItf*              meshItf     = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl    =
                                dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::Mesh& gmdsMesh = meshImpl->getGMDSMesh();

    // ajout d'un filtre pour éviter de mettre 2 fois (ou plus) un même noeud
    std::map<gmds::TCellID, uint> filtre;

    for(Topo::Vertex* vtx : getVertices()) {
        gmds::TCellID node  = vtx->getNode();

        if (filtre[node] == 0){
        	ANodes.push_back(gmdsMesh.get<gmds::Node>(node));
        	filtre[node] = 1;
        }
    }

    for(Topo::CoEdge* coedge : getCoEdges()) {
        std::vector<gmds::TCellID> nodes  = coedge->nodes();

        for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
            if (filtre[nodes[iNode]] == 0){
                ANodes.push_back(gmdsMesh.get<gmds::Node>(nodes[iNode]));
                filtre[nodes[iNode]] = 1;
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void Cloud::
saveMeshCloudTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool new_entity = icmd->addMeshInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (!new_entity && m_save_topo_property == 0)
            m_save_topo_property = m_topo_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
Topo::MeshCloudTopoProperty* Cloud::
setProperty(Topo::MeshCloudTopoProperty* prop)
{
    Topo::MeshCloudTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Cloud::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_topo_property) {
        ccm->addCloudInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
