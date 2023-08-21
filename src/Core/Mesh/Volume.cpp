/*----------------------------------------------------------------------------*/
/*
 * \file Volume.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/Volume.h"
#include "Mesh/MeshDisplayRepresentation.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Common.h"
#include "Utils/Bounds.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/Block.h"
#include "Utils/SerializedRepresentation.h"
#include "Internal/InfoCommand.h"
#include "Group/Group3D.h"

#include <TkUtil/NumericConversions.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <memory>           // unique_ptr
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
const char* Volume::typeNameMeshVolume = "MeshVolume";
/*----------------------------------------------------------------------------*/
Volume::Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp)
: MeshEntity(ctx, prop, disp)
, m_topo_property(new Topo::MeshVolumeTopoProperty())
, m_save_topo_property(0)
{
}
/*----------------------------------------------------------------------------*/
Volume::~Volume()
{
	delete m_topo_property;

#ifdef _DEBUG
    if (m_save_topo_property)
        std::cerr <<"La sauvegarde du MeshVolumeTopoProperty a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
Volume::
Volume(const Volume& v)
: MeshEntity(getContext(),0,0)
, m_topo_property(0)
, m_save_topo_property(0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Volume & Volume::
operator = (const Volume& cl)
{
    if (&cl != this){
        MGX_NOT_YET_IMPLEMENTED("Opérateur de copie");
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
void Volume::getBounds (double bounds[6]) const
{
	//std::cout<<"Volume::getBounds pour "<<getName()<<(m_bounds_to_be_calculate?" à recalculer":" à réutiliser")<<std::endl;

	if (m_bounds_to_be_calculate){

		Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
		Mesh::MeshImplementation*	meshImpl	=
				dynamic_cast<Mesh::MeshImplementation*> (meshItf);
		if (0 == meshImpl)
		{
			INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "Volume::getBounds")
		}
		// Récupération du groupe GMDS
		std::vector<gmds::Region>   polyedres;
		getGMDSRegions(polyedres);

		if (polyedres.empty()) {
			MeshEntity::getBounds (bounds);
			return;
		}
		std::vector<gmds::Region>::const_iterator iter_p = polyedres.begin();

		std::vector<gmds::Node> nds = (*iter_p).getAll<gmds::Node>();
		if (nds.empty()) {
			MeshEntity::getBounds (bounds);
			return;
		}

		std::vector<gmds::Node>::const_iterator iter_n = nds.begin();

		// utilisation de l'objet Bounds que l'on initialise avec le premier noeud du premier polyedre
		Utils::Bounds bnds((*iter_n).X(), (*iter_n).Y(), (*iter_n).Z());

		for (;iter_p != polyedres.end(); ++iter_p) {

			std::vector<gmds::Node> nds = (*iter_p).getAll<gmds::Node>();
			iter_n = nds.begin();
			for (;iter_n != nds.end(); ++iter_n) {
				bnds.add((*iter_n).X(), (*iter_n).Y(), (*iter_n).Z());
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
void Volume::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
	//std::cout<<"Mesh::Volume::getRepresentation... pour "<<getName()<<std::endl;
	// Note CP :
	// Implémentation au 29/05/12 : la classe VTKGMDSEntityRepresentation
	// est une spécialisation (optimisation) pour l'affichage des entités de
	// maillage : on passe directement du maillage GDMS (choix stable et
	// pérenne) aux objets VTK impliqués dans l'affichage, sans passer par
	// une structure générale (générique ?).
	// Avantages :
	// - gain en mémoire : pas de structure intermédiaire chargée de porter
	// points, mailles, ...
	// - gain en temps : pas de conversion GMDS -> structure générale puis
	// structure générale -> VTK, on fait directement GMDS -> VTK.
	// Inconvénients :
	// - L'optimisation n'est pas utilisable en mode client/serveur, les
	// données VTK n'étant pas (directement) sérialisable.
	// - L'extraction des arêtes (vue filaire) est faite par VTK, et est donc à
	// réimplémenter pour le cas général.
	// Cas général, passe partout, notamment en mode client/serveur.
	// Cas particulier, sur station de travail, optimisé GMDS/VTK : voir
	// QtVtkGraphicalWidget::createRepresentation et
	// VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation
	if (Utils::DisplayRepresentation::DISPLAY_MESH != dr.getDisplayType ( ))
		throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

	MeshDisplayRepresentation*  mdr =
	        dynamic_cast<MeshDisplayRepresentation*>(&dr);
	CHECK_NULL_PTR_ERROR (mdr)
	// CP : Code from VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation
	const bool	skin	= mdr->skinDisplayed ( );
	//std::cout<<"  skin "<<skin<<std::endl;
	if (3 != getDim ( ))
	    throw TkUtil::Exception (TkUtil::UTF8String ("Volume::getRepresentation : dimension différente de 3.", TkUtil::Charset::UTF_8));

	Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
	Mesh::MeshImplementation*   meshImpl	=
	        dynamic_cast<Mesh::MeshImplementation*> (meshItf);
	if (0 == meshImpl)
	{
	    INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "Volume::getRepresentation")
		        throw exc;
	}   // if (0 == meshImpl)

	bool	solid	= dr.hasRepresentation(Utils::DisplayRepresentation::SOLID);
	//std::cout<<"  solid "<<skin<<std::endl;
	// Vue pleine :
	if (true == solid)
	{
	    if (1 == mdr->getDecimationStep ( ))
	    {
	        // on passe par GMDS pour récupérer les noeuds et les mailles
	        gmds::IGMesh& gmdsMesh = meshImpl->getGMDSMesh();
	        if (false == skin)
	        {	// Mailles pleines
	            // Récupération du groupe GMDS
	            std::vector<gmds::Region>   polyedres;
	            getGMDSRegions(polyedres);

	            // Constitution d'un vecteur avec les noeuds de la surface
	            // et une map pour l'indirection entre gmds::Node vers indice
	            // local
	            std::vector<gmds::Node>	nodes;
	            std::map<gmds::TCellID, int>    node2id;
	            // nombres de références sur les ids dans les polyèdres :
	            uint	nbRefIds	= 0;

	            gmds::Variable<short>* mark = gmdsMesh.newVariable<short>(gmds::GMDS_NODE,"markVol2");
	            for (std::vector<gmds::Region>::const_iterator
	                    iter_p = polyedres.begin(); iter_p != polyedres.end();
	                    ++iter_p)
	            {
	                std::vector<gmds::Node> nds = (*iter_p).getAll<gmds::Node>();

	                for (std::vector<gmds::Node>::const_iterator
	                        iter_n = nds.begin(); iter_n != nds.end(); ++iter_n)
	                {
	                    gmds::Node current = *iter_n;
	                    if((*mark)[current.getID()]==0)
	                    {
	                        node2id[current.getID()] = nodes.size();
	                        nodes.push_back(current);
	                        (*mark)[current.getID()]=1;
	                    }   // if (!gmdsMesh.isMarked(*iter_n,done))

	                }   // for (std::vector<gmds::Node*>::const_iterator ...
	                nbRefIds += nds.size();
	            } // for (std::vector<gmds::Region*>::const_iterator iter_p =

	            // on démarque les noeuds
	            gmdsMesh.deleteVariable(gmds::GMDS_NODE,mark);
	            std::vector<Utils::Math::Point>& points	= mdr->getPoints ( );
	            points.clear();
	            for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
	                    itn != nodes.end ( ); itn++)
	                points.push_back (
	                        Utils::Math::Point((*itn).X(),(*itn).Y(),(*itn).Z()));
	            std::vector<size_t>*	cells   = new std::vector<size_t> ( );
	            const size_t	polyedreNum  = polyedres.size ( );
	            for (size_t id = 0; id < polyedreNum; id++)
	            {
	                gmds::Region		 poly	= polyedres [id];
	                std::vector<gmds::TCellID>  ndsIDs	 = poly.getAllIDs<gmds::Node> ( );
	                const size_t		count   = ndsIDs.size ( );
	                cells->push_back (count);
	                for (size_t j = 0; j < count; j++)
	                    cells->push_back (node2id [ndsIDs [j]]);
	            }   // for (size_t id = 0; id < polyedreNum; id++)
	            mdr->setCells (cells, skin);
	        }	// if (false == skin)
	        else
	        {	// On prend la peau du maillage
	            // on passe par GMDS pour récupérer les noeuds et les mailles :
	            gmds::IGMesh&	gmdsMesh	= meshImpl->getGMDSMesh ( );
	            // la liste des Topo::Block qui ont contribués :
	            std::vector<Topo::Block* >	blocs;
	            getBlocks (blocs);

	            // la liste des faces externes au groupe de blocs
	            // on utilise une map et on marque les faces à chaque fois
	            // qu'elles sont vus
	            std::map<Topo::CoFace*, int>			marque_faces;
	            std::vector<Topo::Face* >	faces;
	            for (std::vector<Topo::Block* >::iterator
	                    iter1 = blocs.begin();  iter1 != blocs.end(); ++iter1)
	            {
	                (*iter1)->getFaces(faces);
	                std::vector<Topo::CoFace* >	cofaces;
	                for (std::vector<Topo::Face* >::iterator
	                        iter2 = faces.begin(); iter2 != faces.end();++iter2)
	                {
	                    (*iter2)->getCoFaces(cofaces);
	                    for (std::vector<Topo::CoFace* >::iterator
	                            iter3 = cofaces.begin(); iter3 != cofaces.end();
	                            ++iter3)
	                    {
	                        marque_faces[*iter3] += 1;
	                    }	// for (std::vector<Topo::CoFace*> ...
	                }	// for (std::vector<Topo::Face* > ...
                }   // for (std::vector<Topo::Block* > ...

	            // on cumule les noeuds, en évitant les doublons
	            std::vector<gmds::Node>	nodes;
	            std::vector<gmds::Face>	polygones;
	            std::map<gmds::TCellID, int>	node2id;
	            // nombres de références sur les ids dans les polygones :
	            uint			nbRefIds	= 0;
	            gmds::Variable<short>* mark = gmdsMesh.newVariable<short>(gmds::GMDS_NODE,"markVol1");
	            for (std::map<Topo::CoFace*, int>::iterator
	                    iter = marque_faces.begin();
	                    iter != marque_faces.end(); ++iter)
	            {
	                // les faces en relation avec un seul bloc (on évite les
	                // faces internes)
	                if (iter->second == 1)
	                {
	                    Topo::CoFace*	face	= iter->first;
	                    // les polygones de la Topo::CoFace :
	                    std::vector<gmds::TCellID>& loc_polygones	= face->faces();
	                    for (std::vector<gmds::TCellID>::const_iterator
	                            iter_p = loc_polygones.begin();
	                            iter_p != loc_polygones.end(); ++iter_p)
	                    {
	                        gmds::Face current_face = gmdsMesh.get<gmds::Face>(*iter_p);
	                        std::vector<gmds::Node>	nds	= current_face.getAll<gmds::Node>();
	                        polygones.push_back(current_face);
	                        for (std::vector<gmds::Node>::const_iterator
	                                iter_n = nds.begin();
	                                iter_n != nds.end(); ++iter_n)
	                        {
	                            gmds::Node current = *iter_n;
	                            if((*mark)[current.getID()]==0)
	                            {
	                                node2id[current.getID()] = nodes.size();
	                                nodes.push_back(current);
	                                (*mark)[current.getID()]=1;
	                            }   // if (!gmdsMesh.isMarked(*iter_n,done))
	                        }	// for (std::vector<gmds::Node*>:: ...

	                        nbRefIds	+= nds.size();
	                    }	// for (std::vector<gmds::Face*>::const_iterator
	                }	// if (iter->second == 1)
	            }   // for (std::map<Topo::CoFace*, int>::iterator

	            // on démarque les noeuds
	            gmdsMesh.deleteVariable(gmds::GMDS_NODE,mark);
	            std::vector<Utils::Math::Point>& points	= mdr->getPoints ( );
	            points.clear();
	            for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
	                    itn != nodes.end ( ); itn++)
	                points.push_back (Utils::Math::Point (
	                        (*itn).X ( ),
	                        (*itn).Y ( ),
	                        (*itn).Z ( )));
	            std::vector<size_t>*	cells   = new std::vector<size_t> ( );
	            const size_t	polygonNum  = polygones.size ( );
	            for (size_t id = 0; id < polygonNum; id++)
	            {
	                gmds::Face			poly	= polygones [id];
	                std::vector<gmds::TCellID>  nds	 	= poly.getIDs<gmds::Node> ( );
	                const size_t		count   = nds.size ( );
	                cells->push_back (count);
	                for (size_t j = 0; j < count; j++)
	                    cells->push_back (node2id [nds [j]]);
	            }   // for (size_t id = 0; id < polygonNum; id++)
	            mdr->setCells (cells, skin);
	        }	// else if (false == skin)
	    }	// if (1 == mdr->getDecimationStep ( ))
	    else
	    {
	        throw TkUtil::Exception (TkUtil::UTF8String ("Volume::getRepresentation : cas avec décimation non pris en charge.", TkUtil::Charset::UTF_8));
	    }	// else if (1 == mdr->getDecimationStep ( ))
	}	// if (true == solid)
}
/*----------------------------------------------------------------------------*/
bool Volume::isA(std::string& name)
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire un tel test");
//    return (name.compare(0,strlen(typeNameMeshVolume),typeNameMeshVolume) == 0);
    return false;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Volume & cl)
{
    o << cl.getName() << " (uniqueId " << cl.getUniqueId() << ", Name "<<cl.getName()<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
void Volume::addBlock(Topo::Block* b)
{
    m_topo_property->getBlockContainer().add(b);
}
/*----------------------------------------------------------------------------*/
void Volume::removeBlock(Topo::Block* b)
{
    m_topo_property->getBlockContainer().remove(b, true);
}
/*----------------------------------------------------------------------------*/
void Volume::getBlocks(std::vector<Topo::Block* >& blocks) const
{
    m_topo_property->getBlockContainer().checkIfDestroyed();
    m_topo_property->getBlockContainer().get(blocks);
}
/*----------------------------------------------------------------------------*/
void Volume::getGMDSRegions(std::vector<gmds::Region >& ARegions) const
{
    ARegions.clear();

    std::vector<Topo::Block* > blocks;
    getBlocks(blocks);

    Mesh::MeshItf*              meshItf     = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl    =
                                dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::IGMesh&  gmdsMesh    = meshImpl->getGMDSMesh ( );
    for(unsigned int iBlock=0; iBlock<blocks.size(); iBlock++) {
        std::vector<gmds::TCellID> regions  = blocks[iBlock]->regions();

        for(unsigned int iRegion=0; iRegion<regions.size(); iRegion++) {
            ARegions.push_back(gmdsMesh.get<gmds::Region>(regions[iRegion]));
        }
    }
}
/*----------------------------------------------------------------------------*/
void Volume::getGMDSNodes(std::vector<gmds::Node>& ANodes) const
{
	ANodes.clear();

	// utilisation d'un filtre pour ne pas référencer plusieurs fois un même noeud
	std::map<gmds::TCellID, uint> filtre_nodes;

    std::vector<Topo::Block* > blocks;
    getBlocks(blocks);

    Mesh::MeshItf*              meshItf  = getMeshManager ( ).getMesh ( );
    Mesh::MeshImplementation*   meshImpl = dynamic_cast<Mesh::MeshImplementation*> (meshItf);
    CHECK_NULL_PTR_ERROR(meshImpl);
    gmds::IGMesh&  gmdsMesh = meshImpl->getGMDSMesh();

    for(unsigned int iBlock=0; iBlock<blocks.size(); iBlock++) {
    	std::vector<gmds::TCellID> nodes  = blocks[iBlock]->nodes();

    	for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
    		if (filtre_nodes[nodes[iNode]] == 0){
    			ANodes.push_back(gmdsMesh.get<gmds::Node>(nodes[iNode]));
    			filtre_nodes[nodes[iNode]] = 1;
    		}
    	}
    }
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Volume::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            MeshEntity::getDescription (alsoComputed));

    std::vector<Topo::Block* > blocks;
    getBlocks(blocks);

    if (!blocks.empty()){
    	// le maillage vu depuis les blocs
    	Utils::SerializedRepresentation  meshProprietes ("Propriétés du maillage", "");


    	uint nbMailles = 0;
    	for (uint i=0; i<blocks.size(); i++){
    		Topo::Block* bl = blocks[i];
    		nbMailles += bl->regions().size();
    	}
    	meshProprietes.addProperty (
    			Utils::SerializedRepresentation::Property ("Nombre de mailles des blocs", (long)nbMailles));

    	description->addPropertiesSet(meshProprietes);

    	// les liens vers les blocs
    	Utils::SerializedRepresentation blocks_sr ("Blocs topologiques",
    			TkUtil::NumericConversions::toStr((short)blocks.size()));
    	for (uint i=0; i<blocks.size(); i++){
    		Topo::Block* bl = blocks[i];
    		blocks_sr.addProperty (
    				Utils::SerializedRepresentation::Property (
    						bl->getName ( ), *bl));
    	}

    	description->addPropertiesSet (blocks_sr);
    } // end if (!blocks.empty())


    // relation vers le groupe associé
    Group::Group3D* gr = 0;
    MeshManager* mm = dynamic_cast<MeshManager*>(&getMeshManager());
    if (mm)
    	gr = mm->getLocalContext().getLocalGroupManager().getGroup3D(getName(),false);
    if (gr){
        Utils::SerializedRepresentation  groupeDescription (
                        "Groupe 3D associé", "");
    	groupeDescription.addProperty (
    			Utils::SerializedRepresentation::Property (gr->getName(), *gr));
    	description->addPropertiesSet(groupeDescription);
    }


    return description.release ( );
}
/*----------------------------------------------------------------------------*/
void Volume::
saveMeshVolumeTopoProperty(Internal::InfoCommand* icmd)
{
//    std::cout<<"saveMeshVolumeTopoProperty demandé ..."<<std::endl;
    if (icmd) {
        bool change = icmd->addMeshInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (change && m_save_topo_property == 0){
//            std::cout<<"  clone du MeshVolumeTopoProperty avec "
//                    <<m_topo_property->getBlockContainer().size()<<" blocs"<<std::endl;
            m_save_topo_property = m_topo_property->clone();
        }
    }
    m_bounds_to_be_calculate = true;
}
/*----------------------------------------------------------------------------*/
Topo::MeshVolumeTopoProperty* Volume::
setProperty(Topo::MeshVolumeTopoProperty* prop)
{
//    std::cout<<"Mesh::Volume::setProperty, on passe de "
//            <<m_topo_property->getBlockContainer().size()
//            <<" à "<<prop->getBlockContainer().size()<<" blocs"<<std::endl;
    Topo::MeshVolumeTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    m_bounds_to_be_calculate = true;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Volume::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    //std::cout<<"Volume::saveInternals appelé"<<std::endl;
    if (m_save_topo_property) {
        ccm->addVolumeInfoTopoProperty(this, m_save_topo_property);
//        std::cout<<"  -> sauvegarde m_save_topo_property avec "
//                <<m_save_topo_property->getBlockContainer().size()<<" blocs"<<std::endl;
        m_save_topo_property = 0;
    }
    m_bounds_to_be_calculate = true;
}
/*----------------------------------------------------------------------------*/
bool Volume::
isStructured()
{
    std::vector<Topo::Block* > blocs;
    getBlocks(blocs);
    for(unsigned int i=0; i<blocs.size(); i++)
    	if (!blocs[i]->isStructured())
    		return false;

    return true;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
