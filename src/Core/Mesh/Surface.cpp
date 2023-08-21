/*----------------------------------------------------------------------------*/
/*
 * \file Surface.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/Surface.h"
#include "Mesh/MeshDisplayRepresentation.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Common.h"
#include "Utils/Bounds.h"
#include "Topo/CoFace.h"
#include "Utils/SerializedRepresentation.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <memory>           // unique_ptr
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
const char* Surface::typeNameMeshSurface = "MeshSurface";
/*----------------------------------------------------------------------------*/
Surface::Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp)
: MeshEntity(ctx, prop, disp)
, m_topo_property(new Topo::MeshSurfaceTopoProperty())
, m_save_topo_property(0)
{
}
/*----------------------------------------------------------------------------*/
Surface::~Surface()
{
	delete m_topo_property;
}
/*----------------------------------------------------------------------------*/
Surface::
Surface(const Surface& v)
: MeshEntity(getContext(),0,0)
, m_topo_property(0)
, m_save_topo_property(0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Surface & Surface::
operator = (const Surface& cl)
{
    if (&cl != this){
        MGX_NOT_YET_IMPLEMENTED("Opérateur de copie");
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
void Surface::getBounds (double bounds[6]) const
{
	//std::cout<<"Surface::getBounds pour "<<getName()<<(m_bounds_to_be_calculate?" à recalculer":" à réutiliser")<<std::endl;

	if (m_bounds_to_be_calculate){
		Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
		Mesh::MeshImplementation*	meshImpl	=
				dynamic_cast<Mesh::MeshImplementation*> (meshItf);
		if (0 == meshImpl)
		{
			INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "Surface::getBounds")
		}
		// Récupération du groupe GMDS
		std::vector<gmds::Face> polygones;
		getGMDSFaces (polygones);

		if (polygones.empty()) {
			MeshEntity::getBounds (bounds);
			return;
		}
		std::vector<gmds::Face>::const_iterator iter_p = polygones.begin();

		std::vector<gmds::Node> nds = (*iter_p).getAll<gmds::Node>();
		if (nds.empty()) {
			MeshEntity::getBounds (bounds);
			return;
		}

		std::vector<gmds::Node>::const_iterator iter_n = nds.begin();

		// utilisation de l'objet Bounds que l'on initialise avec le premier noeud du premier polygone
		gmds::Node current_node = *iter_n;
		Utils::Bounds bnds(current_node.X(), current_node.Y(), current_node.Z());


		for (;iter_p != polygones.end(); ++iter_p) {

			std::vector<gmds::Node> nds = (*iter_p).getAll<gmds::Node>();
			iter_n = nds.begin();
			for (;iter_n != nds.end(); ++iter_n) {
				current_node = *iter_n;
				bnds.add(current_node.X(),
						current_node.Y(),
						current_node.Z());
			}
		}

		bnds.get(bounds);
		// mémorisation
		m_bounds_to_be_calculate = false;
		for (uint i=0; i<6; i++)
			m_bounds[i] = bounds[i];
	}
	else {
		// réutilisation
		for (uint i=0; i<6; i++)
			bounds[i] = m_bounds[i];
	}
}
/*----------------------------------------------------------------------------*/
void Surface::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
	// Note CP :
	// Implémentation au 29/05/12 : la classe VTKGMDSEntityRepresentation
	// est une spécialisation (optimisation) pour l'affichage des entités de
	// maillage : on passe directement du maillage GMDS (choix stable et
	// pérenne) aux objets VTK impliqués dans l'affichage, sans passer par
	// une structure générale (générique ?).
	// Avantages :
	// - gain en mémoire : par de structure intermédiaire chargée de porter
	// points, mailles, ...
	// - gain en temps : pas de conversion GMDS -> structure générale puis
	// structure générale -> VTK, on fait directement GMDS -> VTK.
	// Inconvénients :
	// - L'optimisation n'est pas utilisable en mode client/serveur, les
	// données VTK n'étant pas (directement) sérialisable.
	// - L'extraction des arêtes (vue filaire) est faite par VTK, et est donc à
	// réimplémenter pour le cas général.

//    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire comme cela ...");
	// Cas général, passe partout, notamment en mode client/serveur.
	// Cas particulier, sur station de travail, optimisé GMDS/VTK : voir
	// QtVtkGraphicalWidget::createRepresentation et
	// VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation
	if (Utils::DisplayRepresentation::DISPLAY_MESH != dr.getDisplayType ( ))
		throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

	MeshDisplayRepresentation*	mdr	=
						dynamic_cast<MeshDisplayRepresentation*>(&dr);
	CHECK_NULL_PTR_ERROR (mdr)
	// CP : Code from VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation
	Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
	Mesh::MeshImplementation*	meshImpl	=
						dynamic_cast<Mesh::MeshImplementation*> (meshItf);
	if (0 == meshImpl)
	{
		INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "Surface::getRepresentation")
		throw exc;
	}	// if (0 == meshImpl)

	// Vue solide ? Oui si demandée ou vue filaire demandée (à charge dans ce
	// second cas à l'appelant d'extraire les arêtes).
	bool	solid	=
		(true == dr.hasRepresentation (Utils::DisplayRepresentation::SOLID)) ||
		(true == dr.hasRepresentation (Utils::DisplayRepresentation::WIRE)) ?
		true : false;

	// Vue pleine :
	if (true == solid)
	{
		if (1 == mdr->getDecimationStep ( ))
		{
			// on passe par GMDS pour récupérer les noeuds et les mailles
			gmds::IGMesh&	gmdsMesh	=	meshImpl->getGMDSMesh ( );
			// Récupération du groupe GMDS
		    std::vector<gmds::Face>   polygones;
		    getGMDSFaces(polygones);

			// Constitution d'un vecteur avec les noeuds de la surface
			// et une map pour l'indirection entre gmds::Node vers indice local
			std::vector<gmds::Node>	nodes;
			std::map<gmds::TCellID, int>	node2id;
			std::map<int,int> id2node;
			getGMDSNodes(nodes);

			for(int iNode=0; iNode<nodes.size(); iNode++) {
				node2id[nodes[iNode].getID()] = iNode;
				id2node[iNode] = nodes[iNode].getID();
			}

			std::vector<Utils::Math::Point>& points	= mdr->getPoints ( );
			points.clear();
			for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
			     itn != nodes.end ( ); itn++)
				points.push_back (Utils::Math::Point (
											(*itn).X(),
											(*itn).Y(),
											(*itn).Z()));
			mdr->setPoints2nodesID(id2node);
			std::vector<size_t>*	cells	= new std::vector<size_t> ( );
			const size_t	polygonNum	= polygones.size ( );
			for (size_t id = 0; id < polygonNum; id++)
			{
				gmds::Face			poly	= polygones [id];
				std::vector<gmds::TCellID>	nds		= poly.getIDs<gmds::Node>( );
				const size_t		count	= nds.size ( );
				cells->push_back (count);
				for (size_t j = 0; j < count; j++)
					cells->push_back (node2id [nds [j]]);
			}	// for (size_t id = 0; id < polygonNum; id++)
			mdr->setCells (cells, false);
		}	// if (1 == mdr->getDecimationStep ( ))
		else	// On décime
		{
    MGX_NOT_YET_IMPLEMENTED("Surface::getRepresentation : décimation non implémentée");
		}	// else if (1 == mdr->getDecimationStep ( ))
	}	// if (true == solid)
	else
	{
    MGX_NOT_YET_IMPLEMENTED("Surface::getRepresentation : cas dim != 2 non implémenté");
	}	// else if (true == solid)
    // cf VTKGMDSEntityRepresentation
}
/*----------------------------------------------------------------------------*/
bool Surface::isA(std::string& name)
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire un tel test");
    //return (name.compare(0,strlen(typeNameMeshSurface),typeNameMeshSurface) == 0);
    return false;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Surface & cl)
{
    o << cl.getName() << " (uniqueId " << cl.getUniqueId() << ", Name "<<cl.getName()<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
void Surface::addCoFace(Topo::CoFace* f)
{
    m_topo_property->getCoFaceContainer().add(f);
}
/*----------------------------------------------------------------------------*/
void Surface::removeCoFace(Topo::CoFace* f)
{
    m_topo_property->getCoFaceContainer().remove(f, true);
}
/*----------------------------------------------------------------------------*/
void Surface::getCoFaces(std::vector<Topo::CoFace* >& faces) const
{
    m_topo_property->getCoFaceContainer().checkIfDestroyed();
    m_topo_property->getCoFaceContainer().get(faces);
}
/*----------------------------------------------------------------------------*/
void Surface::getGMDSFaces(std::vector<gmds::Face >& AFaces) const
{
    AFaces.clear();

    std::vector<Topo::CoFace* > coFaces;
    getCoFaces(coFaces);

    Mesh::MeshItf*              meshItf  = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl = dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::IGMesh&  gmdsMesh = meshImpl->getGMDSMesh();

    for(unsigned int iCoFace=0; iCoFace<coFaces.size(); iCoFace++) {
        std::vector<gmds::TCellID> faces  = coFaces[iCoFace]->faces();

        for(unsigned int iFace=0; iFace<faces.size(); iFace++) {
            AFaces.push_back(gmdsMesh.get<gmds::Face>(faces[iFace]));
        }
    }
}
/*----------------------------------------------------------------------------*/
void Surface::getGMDSNodes(std::vector<gmds::Node>& ANodes) const
{
	ANodes.clear();

	// utilisation d'un filtre pour ne pas référencer plusieurs fois un même noeud
	std::map<gmds::TCellID, uint> filtre_nodes;

    std::vector<Topo::CoFace* > coFaces;
    getCoFaces(coFaces);

    Mesh::MeshItf*              meshItf  = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl = dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::IGMesh&  gmdsMesh = meshImpl->getGMDSMesh();

    for(unsigned int iCoFace=0; iCoFace<coFaces.size(); iCoFace++) {
    	std::vector<gmds::TCellID> nodes  = coFaces[iCoFace]->nodes();

    	for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
    		if (filtre_nodes[nodes[iNode]] == 0){
    			ANodes.push_back(gmdsMesh.get<gmds::Node>(nodes[iNode]));
    			filtre_nodes[nodes[iNode]] = 1;
    		}
    	}
    }
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Surface::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            MeshEntity::getDescription (alsoComputed));

    std::vector<Topo::CoFace* > faces;
    getCoFaces(faces);

    // le maillage vu depuis les cofaces
    Utils::SerializedRepresentation  meshProprietes ("Propriétés du maillage", "");

    uint nbMailles = 0;
    for (uint i=0; i<faces.size(); i++){
        Topo::CoFace* cf = faces[i];
        nbMailles += cf->faces().size();
    }
    meshProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Nombre de mailles des Faces topologiques", (long)nbMailles));

    description->addPropertiesSet(meshProprietes);

    // les liens vers les faces
    if (!faces.empty()){
        Utils::SerializedRepresentation surfaces ("Surfaces topologiques",
                TkUtil::NumericConversions::toStr((short)faces.size()));

        for (uint i=0; i<faces.size(); i++){
            Topo::CoFace* fa = faces[i];
            surfaces.addProperty (
                    Utils::SerializedRepresentation::Property (
                            fa->getName ( ), *fa));
        }

        description->addPropertiesSet (surfaces);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
void Surface::
saveMeshSurfaceTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool new_entity = icmd->addMeshInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (!new_entity && m_save_topo_property == 0)
            m_save_topo_property = m_topo_property->clone();
    }
    m_bounds_to_be_calculate = true;
}
/*----------------------------------------------------------------------------*/
Topo::MeshSurfaceTopoProperty* Surface::
setProperty(Topo::MeshSurfaceTopoProperty* prop)
{
    Topo::MeshSurfaceTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    m_bounds_to_be_calculate = true;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Surface::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_topo_property) {
        ccm->addSurfaceInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
    m_bounds_to_be_calculate = true;
}
/*----------------------------------------------------------------------------*/
bool Surface::
isStructured()
{
    std::vector<Topo::CoFace* > coFaces;
    getCoFaces(coFaces);
    for(unsigned int iCoFace=0; iCoFace<coFaces.size(); iCoFace++)
    	if (!coFaces[iCoFace]->isStructured())
    		return false;

    return true;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
