/*----------------------------------------------------------------------------*/
/*
 * \file SubSurface.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/12/12
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/SubSurface.h"
#include "Mesh/MeshDisplayRepresentation.h"
#include "Mesh/MeshImplementation.h"
#include "Utils/Common.h"
#include "Utils/SerializedRepresentation.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <memory>           // unique_ptr
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
const char* SubSurface::typeNameMeshSubSurface = "MeshSubSurface";
/*----------------------------------------------------------------------------*/
SubSurface::SubSurface(Internal::Context& ctx,
		Utils::Property* prop,
		Utils::DisplayProperties* disp,
		uint gmds_id)
: Surface(ctx, prop, disp)
, m_gmds_id (gmds_id)
{
}
/*----------------------------------------------------------------------------*/
SubSurface::~SubSurface()
{
}
/*----------------------------------------------------------------------------*/
SubSurface::
SubSurface(const SubSurface& v)
: Surface(getContext(),0,0)
, m_gmds_id (0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
SubSurface & SubSurface::
operator = (const SubSurface& cl)
{
    if (&cl != this){
        MGX_NOT_YET_IMPLEMENTED("Opérateur de copie");
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* SubSurface::getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            Surface::getDescription (alsoComputed));

	// le maillage
	Utils::SerializedRepresentation  meshProprietes ("Propriétés du sous maillage", "");

	meshProprietes.addProperty (
			Utils::SerializedRepresentation::Property ("Id sur maillage GMDS", (long)m_gmds_id));

	meshProprietes.addProperty (
			Utils::SerializedRepresentation::Property ("Nombre de mailles", (long)m_poly.size()));

	description->addPropertiesSet(meshProprietes);

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
bool SubSurface::isA(std::string& name)
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire un tel test");
//    return (name.compare(0,strlen(typeNameMeshSubSurface),typeNameMeshSubSurface) == 0);
    return false;
}
/*----------------------------------------------------------------------------*/
void SubSurface::addFace(gmds::Face face)
{
	m_poly.push_back(face);
}
/*----------------------------------------------------------------------------*/
void SubSurface::addFace(std::vector<gmds::Face> faces)
{
	m_poly.insert(m_poly.end(), faces.begin(), faces.end());
}
/*----------------------------------------------------------------------------*/
void SubSurface::getGMDSFaces(std::vector<gmds::Face >& faces) const
{
	faces = m_poly;
}
/*----------------------------------------------------------------------------*/
void SubSurface::getGMDSNodes(std::vector<gmds::Node >& nodes) const
{
	nodes.clear();

	// utilisation d'un filtre pour ne pas référencer plusieurs fois un même noeud
	std::map<gmds::Node, uint> filtre_nodes;

	for(uint i=0; i<m_poly.size(); i++){
		std::vector<gmds::Node> cell_nodes = m_poly[i].getAll<gmds::Node>();

		for(unsigned int iNode=0; iNode<cell_nodes.size(); iNode++) {
			if (filtre_nodes[cell_nodes[iNode]] == 0){
				nodes.push_back(cell_nodes[iNode]);
				filtre_nodes[cell_nodes[iNode]] = 1;
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
void SubSurface::clear()
{
	m_poly.clear();
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const SubSurface & cl)
{
    o << cl.getName() << " (uniqueId " << cl.getUniqueId() << ", Name "<<cl.getName()<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
