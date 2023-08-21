/*----------------------------------------------------------------------------*/
/*
 * \file SubVolume.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/12/12
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/SubVolume.h"
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
const char* SubVolume::typeNameMeshSubVolume = "MeshSubVolume";
/*----------------------------------------------------------------------------*/
SubVolume::SubVolume(Internal::Context& ctx,
		Utils::Property* prop,
		Utils::DisplayProperties* disp,
		uint gmds_id)
: Volume(ctx, prop, disp)
, m_gmds_id (gmds_id)
{
	//std::cout<<"SubVolume::SubVolume("<<id<<") de nom "<<getName()<<std::endl;
}
/*----------------------------------------------------------------------------*/
SubVolume::~SubVolume()
{
}
/*----------------------------------------------------------------------------*/
SubVolume::
SubVolume(const SubVolume& v)
: Volume(getContext(),0,0)
, m_gmds_id (0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
SubVolume & SubVolume::
operator = (const SubVolume& cl)
{
    if (&cl != this){
        MGX_NOT_YET_IMPLEMENTED("Opérateur de copie");
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
void SubVolume::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
	// on doit faire avec la liste des mailles
	if (Utils::DisplayRepresentation::DISPLAY_MESH != dr.getDisplayType ( ))
		throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

	MeshDisplayRepresentation*  mdr =
	        dynamic_cast<MeshDisplayRepresentation*>(&dr);
	CHECK_NULL_PTR_ERROR (mdr)

	const bool	skin	= mdr->skinDisplayed ( );
	if (3 != getDim ( ))
	    throw TkUtil::Exception (TkUtil::UTF8String ("SubVolume::getRepresentation : dimension différente de 3.", TkUtil::Charset::UTF_8));

	Mesh::MeshItf*				meshItf		= getMeshManager ( ).getMesh ( );
	Mesh::MeshImplementation*   meshImpl	=
	        dynamic_cast<Mesh::MeshImplementation*> (meshItf);
	if (0 == meshImpl)
	{
	    INTERNAL_ERROR (exc, "Le maillage n'est pas de type MeshImplementation ou absence de maillage.", "SubVolume::getRepresentation")
		        throw exc;
	}   // if (0 == meshImpl)

	bool	solid	= dr.hasRepresentation(Utils::DisplayRepresentation::SOLID);
	// Vue pleine :
	if (true == solid)
	{
	    if (1 == mdr->getDecimationStep ( ))
	    {
	        gmds::IGMesh&	gmdsMesh	= meshImpl->getGMDSMesh (m_gmds_id);
	        if (false == skin)
	        {	// Mailles internes comprises
	            // Constitution d'un vecteur avec les noeuds de la surface
	            // et une map pour l'indirection entre gmds::Node vers indice
	            // local
	            std::vector<gmds::Node>	nodes;
	            std::map<gmds::TCellID, int>  node2id;
	            // nombres de références sur les ids dans les polyèdres :
	            uint	nbRefIds	= 0;

	            gmds::Variable<short>* mark = gmdsMesh.newVariable<short>(gmds::GMDS_NODE,"markSubVol");
	            for (std::vector<gmds::Region>::const_iterator
	                    iter_p = m_poly.begin(); iter_p != m_poly.end();
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
	                    } // if (!gmdsMesh.isMarked(*iter_n,done))
	                }   // for (std::vector<gmds::Node*>::const_iterator ...
	                nbRefIds += nds.size();
	            } // for (std::vector<gmds::Region*>::const_iterator iter_p =

	            // on démarque les noeuds
	            gmdsMesh.deleteVariable(gmds::GMDS_NODE,mark);

	            std::vector<Utils::Math::Point>& points	= mdr->getPoints ( );
	            points.clear();
	            for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
	                    itn != nodes.end ( ); itn++)
	                points.push_back (Utils::Math::Point (
	                        (*itn).X ( ), (*itn).Y ( ), (*itn).Z ( )));
	            std::vector<size_t>*	cells   = new std::vector<size_t> ( );
	            const size_t	polyedreNum  = m_poly.size ( );
	            for (size_t id = 0; id < polyedreNum; id++)
	            {
	                gmds::Region		 poly	= m_poly [id];
	                std::vector<gmds::TCellID>  ndsIDs	 = poly.getAllIDs<gmds::Node> ( );
	                const size_t		count   = ndsIDs.size ( );
	                cells->push_back (count);
	                for (size_t j = 0; j < count; j++)
	                    cells->push_back (node2id [ndsIDs [j]]);
	            }   // for (size_t id = 0; id < polyedreNum; id++)
	            mdr->setCells (cells, skin);
//	            std::cout<<"Représentation avec mailles internes au nombre de "<<cells->size()<<std::endl;
	        }	// if (false == skin)
	        else
	        {	// On prend la peau du maillage

	            // NB: on ne dispose pas des polygones pour les mailles d'un sous-volume


	            gmds::IGMesh& gmdsMesh = meshImpl->getGMDSMesh (m_gmds_id);

	            // on cumule les noeuds, en évitant les doublons
	            std::vector<gmds::Node>	nodes;
	            std::vector<gmds::Face>	polygones;
	            std::map<gmds::TCellID, int>	node2id;
	            // nombres de références sur les ids dans les polygones :
	            uint			nbRefIds	= 0;
//                const int done = gmdsMesh.getNewMark();
//	            std::map<gmds::Face*, uint> marque_faces;
//
//	            // on marque les polygones suivant le nombre fois qu'ils sont dans le sous-volume
//	            for (std::vector<gmds::Region*>::const_iterator
//	                    iter_p = m_poly.begin(); iter_p != m_poly.end();
//	                    ++iter_p){
//	                gmds::Region*       poly   = *iter_p;
//	                //std::cout<<"polyèdre "<<poly->getID()<<" avec "<<poly->getFaces().size()<<" polygones"<<std::endl;
//	                std::vector<gmds::Face*> faces = poly->getFaces();
//	                for (std::vector<gmds::Face*>::const_iterator
//	                        iter_f = faces.begin(); iter_f != faces.end();
//	                        ++iter_f){
//	                    marque_faces[*iter_f] += 1;
//	                }
//	            }


//	            for (std::vector<gmds::Region*>::const_iterator
//	                    iter_p = m_poly.begin(); iter_p != m_poly.end();
//	                    ++iter_p){
//	                gmds::Region*       poly   = *iter_p;
//	                std::vector<gmds::Face*> faces = poly->getFaces();
//	                for (std::vector<gmds::Face*>::const_iterator
//	                        iter_f = faces.begin(); iter_f != faces.end();
//	                        ++iter_f){
//	                    if (marque_faces[*iter_f] == 1){
//
//
//	                        std::vector<Node*>	nds	= (*iter_f)->getNodes();
//	                        polygones.push_back(*iter_f);
//	                        for (std::vector<gmds::Node*>::const_iterator
//	                                iter_n = nds.begin();
//	                                iter_n != nds.end(); ++iter_n){
//	                            if (!gmdsMesh.isMarked(*iter_n,done)){
//	                                node2id[*iter_n]	= nodes.size();
//	                                nodes.push_back(*iter_n);
//	                                gmdsMesh.mark(*iter_n, done);
//	                            }	// if (!gmdsMesh.isMarked(*iter_n,done))
//	                        }	// for (std::vector<gmds::Node*>:: ...
//
//	                        nbRefIds	+= nds.size();
//	                    }	// if (marque_faces[*iter_f] == 1)
//	                }
//	            } // end for iter_p

//	            // on démarque les noeuds
//	            for (std::vector<gmds::Node*>::iterator
//	                    iter = nodes.begin(); iter != nodes.end();
//	                    ++iter)
//	                gmdsMesh.unmark(*iter, done);
//	            gmdsMesh.freeMark(done);

	            std::vector<Utils::Math::Point>& points	= mdr->getPoints ( );
	            points.clear();
	            for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
	                    itn != nodes.end ( ); itn++)
	                points.push_back (Utils::Math::Point (
	                        (*itn).X ( ), (*itn).Y ( ), (*itn).Z ( )));
//	            std::vector<size_t>*	cells   = new std::vector<size_t> ( );
//	            const size_t	polygonNum  = polygones.size ( );
//	            for (size_t id = 0; id < polygonNum; id++)
//	            {
//	                gmds::Face*			poly	= polygones [id];
//	                std::vector<Node*>  nds	 	= poly->getNodes ( );
//	                const size_t		count   = nds.size ( );
//	                cells->push_back (count);
//	                for (size_t j = 0; j < count; j++)
//	                    cells->push_back (node2id [nds [j]]);
//	            }   // for (size_t id = 0; id < polygonNum; id++)
//	            mdr->setCells (cells, skin);
//	            std::cout<<"Représentation avec mailles de peau au nombre de "<<cells->size()<<std::endl;
	        }	// else if (false == skin)
	    }	// if (1 == mdr->getDecimationStep ( ))
	    else
	    {
	        throw TkUtil::Exception (TkUtil::UTF8String ("SubVolume::getRepresentation : cas avec décimation non pris en charge.", TkUtil::Charset::UTF_8));
	    }	// else if (1 == mdr->getDecimationStep ( ))
	}	// if (true == solid)
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* SubVolume::getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            Volume::getDescription (alsoComputed));

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
bool SubVolume::isA(std::string& name)
{
    MGX_NOT_YET_IMPLEMENTED("Il n'est pas prévu de faire un tel test");
//    return (name.compare(0,strlen(typeNameMeshSubVolume),typeNameMeshSubVolume) == 0);
    return false;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const SubVolume & cl)
{
    o << cl.getName() << " (uniqueId " << cl.getUniqueId() << ", Name "<<cl.getName()<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
void SubVolume::getGMDSRegions(std::vector<gmds::Region>& ARegions) const
{
	ARegions = m_poly;
}
/*----------------------------------------------------------------------------*/
void SubVolume::addRegion(gmds::Region& reg)
{
	m_poly.push_back(reg);
}
/*----------------------------------------------------------------------------*/
void SubVolume::addRegion(std::vector<gmds::Region> reg)
{
	m_poly.insert(m_poly.end(), reg.begin(), reg.end());
}
/*----------------------------------------------------------------------------*/
void SubVolume::clear()
{
	m_poly.clear();
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
