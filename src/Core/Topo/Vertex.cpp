/*----------------------------------------------------------------------------*/
/** \file Vertex.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <string>
#include <string.h>
/*----------------------------------------------------------------------------*/
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/CommandEditTopo.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"
#include "Topo/BlockMeshingPropertyTransfinite.h"
#include "Mesh/CommandCreateMesh.h"
#include <Mesh/MeshItf.h>
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/SerializedRepresentation.h"
#include "Internal/InfoCommand.h"
#include "Geom/GeomEntity.h"
#include "Group/Group0D.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/NumericConversions.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
const char* Vertex::typeNameTopoVertex = "TopoVertex";
/*----------------------------------------------------------------------------*/
Vertex::
Vertex(Internal::Context& ctx)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoVertex),
        ctx.newDisplayProperties(Utils::Entity::TopoVertex))
, m_geom_property(new VertexGeomProperty())
, m_save_geom_property(0)
, m_topo_property(new VertexTopoProperty())
, m_save_topo_property(0)
, m_mesh_data(new VertexMeshingData())
, m_save_mesh_data(0)
{
	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
Vertex::
Vertex(Internal::Context& ctx,
        const Utils::Math::Point& pt)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoVertex),
        ctx.newDisplayProperties(Utils::Entity::TopoVertex))
, m_geom_property(new VertexGeomProperty(pt))
, m_save_geom_property(0)
, m_topo_property(new VertexTopoProperty())
, m_save_topo_property(0)
, m_mesh_data(new VertexMeshingData())
, m_save_mesh_data(0)
{
	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
Vertex* Vertex::
clone()
{
    Vertex* new_vertex = new Vertex(getContext(), getCoord());
    new_vertex->setGeomAssociation(getGeomAssociation());
    return new_vertex;
}
/*----------------------------------------------------------------------------*/
Vertex::
Vertex(const Vertex& v)
: TopoEntity(v.getContext(), 0, 0)
, m_geom_property(new VertexGeomProperty())
, m_save_geom_property(0)
, m_topo_property(new VertexTopoProperty())
, m_save_topo_property(0)
, m_mesh_data(new VertexMeshingData())
, m_save_mesh_data(0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Vertex & Vertex::
operator = (const Vertex& v)
{
    if (&v != this){
        m_geom_property = v.m_geom_property->clone();
        m_save_geom_property = 0;
        m_topo_property = v.m_topo_property->clone();
        m_save_topo_property = 0;
        m_mesh_data = v.m_mesh_data->clone();
        m_save_mesh_data = 0;
    }
    return *this;
}
/*----------------------------------------------------------------------------*/
Vertex::
~Vertex()
{
#ifdef _DEBUG
    if (m_topo_property == 0 || m_geom_property == 0 || m_mesh_data == 0)
    	std::cerr<<"Serait-on passé deux fois dans le destructeur ? pour "<<getName()<<std::endl;
#endif

    delete m_geom_property;
    delete m_topo_property;
    delete m_mesh_data;

#ifdef _DEBUG
    m_topo_property = 0;
    m_geom_property = 0;
    m_mesh_data = 0;

    if (m_save_geom_property != 0)
    	std::cerr<<"La sauvegarde du VertexGeomProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_topo_property != 0)
    	std::cerr<<"La sauvegarde du VertexTopoProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_data)
    	std::cerr<<"La sauvegarde du VertexMeshingData a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Vertex::
setCoord(const Utils::Math::Point & pt, bool acceptMeshed)
{
#ifdef _DEBUG2
    std::cout<<"setCoord à "<<pt<<" pour le sommet "<<getName()<<std::endl;
#endif

    if ((not acceptMeshed) && isMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
         message << "On ne peut pas déplacer un sommet ("<<getName()<<") sans avoir détruit le maillage.";
         throw TkUtil::Exception (message);
     }

    m_geom_property->setCoord(pt);

    updateCoEdgeModificationTime();
}
/*----------------------------------------------------------------------------*/
void Vertex::
merge(Vertex* ve, Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
    std::cout<<"merge("<<ve->getName()<<") pour sommet "<<getName()<<std::endl;
    std::cout<<*this<<std::endl;
    std::cout<<*ve<<std::endl;
#endif
    if (ve == this)
        return;

    // distance entre les 2 sommets
    double dist2 = getCoord().length2(ve->getCoord());

    if (dist2 > Utils::Math::MgxNumeric::mgxGeomDoubleEpsilonSquare){
    	// si le sommet bouge suffisament, il faut éviter les méthodes de maillage autres que transfini
    	std::vector<CoFace* > cofaces;
    	ve->getCoFaces(cofaces);
    	for (uint i=0; i<cofaces.size(); i++){
    		if (cofaces[i]->getMeshLaw() < CoFaceMeshingProperty::transfinite){
#ifdef _DEBUG2
    			std::cout<<"modification méthode de maillage pour "<<cofaces[i]->getName()<<std::endl;
#endif

    		    CoFaceMeshingProperty* prop = new FaceMeshingPropertyTransfinite();
    		    cofaces[i]->switchCoFaceMeshingProperty(icmd, prop);
    		    delete prop;
    		}
    	}

    	std::vector<Block* > blocs;
    	ve->getBlocks(blocs);
    	for (uint i=0; i<blocs.size(); i++){
    		if (blocs[i]->getMeshLaw() < BlockMeshingProperty::transfinite){
#ifdef _DEBUG2
    			std::cout<<"modification méthode de maillage pour "<<blocs[i]->getName()<<std::endl;
#endif

    		    BlockMeshingProperty* prop = new BlockMeshingPropertyTransfinite();
    		    blocs[i]->switchBlockMeshingProperty(icmd, prop);
    		    delete prop;

    		}
    	}

    	// les arêtes voisines sont déplacées donc avec une association invalide
        std::vector<CoEdge* > coedges;
        ve->getCoEdges(coedges);
        for (uint i=0; i<coedges.size(); i++){
        	CoEdge* coedge = coedges[i];
        	if (coedge->isDestroyed()){
        		if (icmd)
        			icmd->addTopoInfoEntity(coedge, Internal::InfoCommand::DISPMODIFIED);
        		coedge->saveTopoProperty();
        		coedge->setGeomAssociation(0);
        	}
        }

        // idem avec les cofaces
        for (uint i=0; i<cofaces.size(); i++){
        	CoFace* coface = cofaces[i];
        	if (icmd)
        		icmd->addTopoInfoEntity(coface, Internal::InfoCommand::DISPMODIFIED);
        	coface->saveTopoProperty();
        	coface->setGeomAssociation(0);
        }

   } // end if (dist2 > Utils::Math::MgxNumeric::mgxGeomDoubleEpsilonSquare)

    // on remplace le sommet dans les arêtes associées à ve
    // On travaille sur une copie car il y a une mise à jour en même temps de m_coedges
    std::vector<CoEdge* > coedges;
    ve->getCoEdges(coedges);

    for (std::vector<CoEdge* >::iterator iter=coedges.begin();
            iter != coedges.end(); ++iter)
        (*iter)->replace(ve, this, true, false, icmd);

    // récupère l'association côté ve si celle de this est détruite, ou si la dimension de la geom est inférieure
    if (getGeomAssociation() && ve->getGeomAssociation()
    		&& ((getGeomAssociation()->isDestroyed() && !ve->getGeomAssociation()->isDestroyed())
    				|| (getGeomAssociation()->getDim() > ve->getGeomAssociation()->getDim()))){
    	saveTopoProperty();
    	setGeomAssociation(ve->getGeomAssociation());
    }

    ve->setDestroyed(true);
    if (icmd)
        icmd->addTopoInfoEntity(ve,Internal::InfoCommand::DELETED);
}
/*----------------------------------------------------------------------------*/
void Vertex::
setDestroyed(bool b)
{
	if (b && !isFinished() && isMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "On ne peut pas détruire un sommet sans avoir détruit le maillage.\n"
                << "Ce que l'on tente de faire pour le sommet "<<getName();
        throw TkUtil::Exception (message);
    }

	if (isDestroyed() == b)
		return;

	Utils::Container<Group::Group0D>& groups = getGroupsContainer();
	if (b)
		for (uint i=0; i<groups.size(); i++){
			Group::Group0D* gr = groups.get(i);
			gr->remove(this);
		}
	else
		for (uint i=0; i<groups.size(); i++){
			Group::Group0D* gr = groups.get(i);
			gr->add(this);
		}

    TopoEntity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
void Vertex::
free(Internal::InfoCommand* icmd)
{
    saveVertexTopoProperty(icmd);

    // on retire la relation avec la géométrie
    if (icmd && getGeomAssociation()
             && icmd->getTopoInfoEntity()[this] != Internal::InfoCommand::NONE){
        saveTopoProperty();
        setGeomAssociation(0);
    }

    // on met au courant la commande de la destruction
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);

    setDestroyed(true);

//    // on supprime les relations des arêtes vers ce sommet
//    for (uint i=0; i<getNbCoEdges(); i++) {
//        getCoEdge(i)->saveCoEdgeTopoProperty(icmd);
//        getCoEdge(i)->removeVertex(this, false);
//    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
void Vertex::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_TOPO)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

    if (isDestroyed() && (true == checkDestroyed)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Interdit d'afficher une entité détruite ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }

    TopoDisplayRepresentation* tdr =
            dynamic_cast<Topo::TopoDisplayRepresentation*>(&dr);
    CHECK_NULL_PTR_ERROR(tdr);

    std::vector<Utils::Math::Point>& points = tdr->getPoints();

    points.clear();
    points.push_back(getCoord());

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWASSOCIATION) && getGeomAssociation()){
        // on ajoute une flêche entre l'arête et la géométrie
        //std::cout<<"  SHOWASSOCIATION -> vers "<<getGeomAssociation()->getName()<<std::endl;
        Utils::Math::Point pt2;
        getGeomAssociation()->project(getCoord(), pt2);
        std::vector<Utils::Math::Point>& vecteur = tdr->getVector();
        vecteur.push_back(getCoord());
        vecteur.push_back(pt2);
    }
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Vertex::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            TopoEntity::getDescription (alsoComputed));

    // les propriétés spécifiques à ce type d'entité

    Utils::SerializedRepresentation  topoProprietes ("Propriétés topologiques", "");

#ifdef _DEBUG
    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("En cours d'édition", isEdited()));
#endif

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Est maillé", isMeshed()));


    description->addPropertiesSet (topoProprietes);


    // les propriétés géométriques

    std::vector<double> coords = m_geom_property->getCoord().getCoords();
    Mgx3D::Utils::SerializedRepresentation::Property    coordsProp (
                                                        "Coordonnées", coords);
    Mgx3D::Utils::SerializedRepresentation  vertexDescription (
                                            "Propriété géométrique", coordsProp.getValue ( ));
    vertexDescription.addProperty (coordsProp);
    description->setSummary (coordsProp.getValue ( ));
    description->addPropertiesSet (vertexDescription);


    // les relations vers les autres types d'entités topologiques
    // on cache l'existance des Edge et Face

    std::vector<CoEdge* > ce;
    getCoEdges(ce);

    Utils::SerializedRepresentation  topoRelation ("Relations topologiques", "");

    Utils::SerializedRepresentation  coedges ("Arêtes topologiques",
            TkUtil::NumericConversions::toStr(ce.size()));
    for (std::vector<Topo::CoEdge*>::iterator iter = ce.begin( ); ce.end( )!=iter; iter++)
        coedges.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (coedges);

    description->addPropertiesSet (topoRelation);

    std::vector<Group::Group0D*> grp;
    getGroups(grp);
    if (!grp.empty()){
    	Utils::SerializedRepresentation  groupe ("Relation vers les groupes",
    			TkUtil::NumericConversions::toStr(grp.size()));
    	for (std::vector<Group::Group0D*>::iterator iter = grp.begin( ); iter!=grp.end( ); ++iter)
    		groupe.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));

    	description->addPropertiesSet (groupe);

    } // end if (!grp.empty())

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
std::string Vertex::
getSummary ( ) const
{
    std::vector<double> coords = m_geom_property->getCoord().getCoords();
    Mgx3D::Utils::SerializedRepresentation::Property    coordsProp (
                                                        "Coordonnées", coords);
    return coordsProp.getValue ( );
}
/*----------------------------------------------------------------------------*/
void Vertex::
saveVertexGeomProperty(Internal::InfoCommand* icmd, bool propagate)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (change && m_save_geom_property == 0){
            m_save_geom_property = m_geom_property->clone();

            // les blocs, faces, arêtes ont leur aspect qui change
            if (propagate){

                std::vector<CoEdge* > loc_coedges;
                getCoEdges(loc_coedges);

                for (std::vector<CoEdge* >::iterator iter1=loc_coedges.begin();
                        iter1 != loc_coedges.end(); ++iter1){
                    std::vector<Edge* > local_edges;
                    (*iter1)->getEdges(local_edges);
                    icmd->addTopoInfoEntity(*iter1, Internal::InfoCommand::DISPMODIFIED);

                    for (std::vector<Edge* >::iterator iter2 = local_edges.begin();
                            iter2 != local_edges.end(); ++iter2){
                        std::vector<CoFace* > loc_cofaces;
                        (*iter2)->getCoFaces(loc_cofaces);
                        icmd->addTopoInfoEntity(*iter2, Internal::InfoCommand::DISPMODIFIED);

                        for (std::vector<CoFace* >::iterator iter3 = loc_cofaces.begin();
                                iter3 != loc_cofaces.end(); ++iter3){
                            std::vector<Face* > loc_faces;
                            (*iter3)->getFaces(loc_faces);
                            icmd->addTopoInfoEntity(*iter3, Internal::InfoCommand::DISPMODIFIED);

                            for (std::vector<Face* >::iterator iter4 = loc_faces.begin();
                                    iter4 != loc_faces.end(); ++iter4){
                                std::vector<Block* > loc_bl;
                                (*iter4)->getBlocks(loc_bl);
                                icmd->addTopoInfoEntity(*iter4, Internal::InfoCommand::DISPMODIFIED);

                                for (std::vector<Block* >::iterator iter5 = loc_bl.begin();
                                        iter5 != loc_bl.end(); ++iter5){
                                    icmd->addTopoInfoEntity(*iter5, Internal::InfoCommand::DISPMODIFIED);
                                } // end for iter5
                            } // end for iter4
                        } // end for iter3
                    } // end for iter2
                } // end for iter1
            } // end if (propagate)
        } // if (change ...
    } // end if (icmd)
}
/*----------------------------------------------------------------------------*/
void Vertex::
saveVertexTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
#ifdef _DEBUG2
    	std::cout<<"Vertex::saveVertexTopoProperty pour "<<getName()<<", valeur initiale: "
    			<<Internal::InfoCommand::type2String(icmd->getTopoInfoEntity()[this])<<std::endl;
#endif
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_topo_property == 0){
        	if (isMeshed()){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        		message << "On ne peut pas modifier un sommet alors qu'il est déjà maillé.";
        		throw TkUtil::Exception (message);
        	}
        	else
        		m_save_topo_property = m_topo_property->clone();
        }
    }
}
/*----------------------------------------------------------------------------*/
void Vertex::
saveVertexMeshingData(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_data == 0)
            m_save_mesh_data = m_mesh_data->clone();
    }
}
/*----------------------------------------------------------------------------*/
VertexGeomProperty* Vertex::
setProperty(VertexGeomProperty* prop)
{
#ifdef _DEBUG2
	std::cout<<"Vertex::setProperty pour "<<getName()<<std::endl;
#endif
    updateCoEdgeModificationTime();
    VertexGeomProperty* tmp = m_geom_property;
    m_geom_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
VertexTopoProperty* Vertex::
setProperty(VertexTopoProperty* prop)
{
    VertexTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
VertexMeshingData* Vertex::
setProperty(VertexMeshingData* prop)
{
    VertexMeshingData* tmp = m_mesh_data;
    m_mesh_data = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Vertex::
saveInternals(CommandEditTopo* cet)
{
    TopoEntity::saveInternals (cet);

    if (m_save_geom_property) {
        cet->addVertexInfoGeomProperty(this, m_save_geom_property);
        m_save_geom_property = 0;
    }
    if (m_save_topo_property) {
        cet->addVertexInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
void Vertex::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_mesh_data) {
        ccm->addVertexInfoMeshingData(this, m_save_mesh_data);
        m_save_mesh_data = 0;
    }
}
/*----------------------------------------------------------------------------*/
void Vertex::
check() const
{
    if (isDestroyed()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    	messErr << "On utilise un sommet détruit : "<<getName();
    	throw TkUtil::Exception(messErr);
    }
    if (getGeomAssociation() && getGeomAssociation()->isDestroyed()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur avec le sommet "<<getName()
                <<", il pointe sur "<<getGeomAssociation()->getName()
                <<" qui est détruit[e] !";
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
bool Vertex::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Vertex & v)
{
    o << v.getName() << " (uniqueId "<<v.getUniqueId()<<")"
        << (v.isDestroyed()?" (DETRUIT) ":"")
        << (v.isEdited()?" (EN COURS D'EDITION) ":"");

    if (v.getGeomAssociation()){
        o << ", projeté sur " << v.getGeomAssociation()->getName()
        <<(v.getGeomAssociation()->isDestroyed()?" (DETRUITE)":"");

        std::vector<std::string> gn;
        v.getGeomAssociation()->getGroupsName(gn);
        if (!gn.empty()) {
            o << " (groupes:";
            for (size_t i=0; i<gn.size(); i++)
                o << " "<<gn[i];
            o << ")";
        }
        else {
            o << " (sans groupe)";
        }
    }
    else
        o << ", sans projection";

    o << v.getCoord();
    std::vector<Topo::CoEdge* > coedges;
    v.getCoEdges(coedges);
    o << " relié à "<<(short int)v.getNbCoEdges()<<" arêtes communes : ";
    for (uint j=0; j<coedges.size(); j++){
        o << " " << coedges[j]->getName();
        if (coedges[j]->isDestroyed())
            o << "  [DETRUITE] ";
    }

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const Vertex & v)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << v;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo Vertex::getInfos() const
{
	Topo::TopoInfo infos;
	infos.name = getName();
	infos.dimension = getDim();
	getCoEdges(infos.incident_coedges);
	getEdges(infos.incident_edges);
	getCoFaces(infos.incident_cofaces);
	getBlocks(infos.incident_blocks);
	infos.geom_entity = getGeomAssociation();
	return infos;
}
/*----------------------------------------------------------------------------*/
gmds::TCellID Vertex::
getNode()
{
    if (!m_mesh_data->isMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Vertex::getNode() ne peut se faire avec le sommet "<<
                getName()<<" car le sommet n'est pas maillé";
        throw TkUtil::Exception (message);
    }
    return m_mesh_data->node();
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Vertex::getCoord() const
{
	Utils::Math::Point res;
	if (isMeshed()){
		gmds::IGMesh& gmds_mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();
		gmds::math::Point pt = gmds_mesh.getPoint(m_mesh_data->node());
		res.setX(pt.X());
		res.setY(pt.Y());
		res.setZ(pt.Z());
#ifdef _DEBUG2
		std::cout<<"getCoord() pour "<<getName()<<" , TCellID = "<<m_mesh_data->node()<<" : "<<res<<std::endl;
#endif
	}
	else {
		res = m_geom_property->getCoord();
#ifdef _DEBUG2
		std::cout<<"getCoord() pour "<<getName()<<" : "<<res<<std::endl;
#endif
	}
	return res;
}
/*----------------------------------------------------------------------------*/
void Vertex::findNearlyVertices(const std::vector<Vertex* >& vertices_A,
        const std::vector<Vertex* >& vertices_B,
        std::map<Topo::Vertex*, Topo::Vertex*>& corr_vertex_A_B)
{
    // algo simpliste, non adapté à des cas avec beaucoup de sommets
    for (std::vector<Vertex* >::const_iterator iterA = vertices_A.begin();
            iterA != vertices_A.end(); ++iterA){
        bool found = false;
        double bestLength = 0.0;
        Utils::Math::Point ptA = (*iterA)->getCoord();
        for (std::vector<Vertex* >::const_iterator iterB = vertices_B.begin();
                iterB != vertices_B.end(); ++iterB){
            Utils::Math::Point ptB = (*iterB)->getCoord();
            double length = ptA.length(ptB);

            if (!found || length < bestLength){
                found = true;
                corr_vertex_A_B[(*iterA)] = (*iterB);
                bestLength = length;
            }
        } // for iterB
    } // for iterA
}
/*----------------------------------------------------------------------------*/
void Vertex::
getEdges(std::vector<Edge* >& edges) const
{
    edges.clear();
    std::list<Topo::Edge*> l_e;
    for(uint i=0; i <getNbCoEdges();i++) {
        std::vector<Edge* > local_edges;
        getCoEdge(i)->getEdges(local_edges);
        l_e.insert(l_e.end(), local_edges.begin(), local_edges.end());
    }
    l_e.sort(Utils::Entity::compareEntity);
    l_e.unique();

    edges.insert(edges.end(),l_e.begin(),l_e.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::
getCoFaces(std::vector<CoFace* >& cofaces) const
{
    std::list<Topo::CoFace*> l_f;

    std::vector<Edge* > edges;
    getEdges(edges);

    for (std::vector<Edge* >::iterator iter1 = edges.begin();
            iter1 != edges.end(); ++iter1){
        std::vector<CoFace* > loc_cofaces;
        (*iter1)->getCoFaces(loc_cofaces);

        l_f.insert(l_f.end(), loc_cofaces.begin(), loc_cofaces.end());
    }

    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();

    cofaces.insert(cofaces.end(),l_f.begin(),l_f.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::
getBlocks(std::vector<Block* >& blocks) const
{
#ifdef _DEBUG2
	std::cout<<"getBlocks() pour "<<getName();
#endif
    std::list<Topo::Block*> l_b;

    for(uint i=0; i <getNbCoEdges();i++) {
        std::vector<Edge* > local_edges;
        getCoEdge(i)->getEdges(local_edges);

        for (std::vector<Edge* >::iterator iter1 = local_edges.begin();
                iter1 != local_edges.end(); ++iter1){
            std::vector<CoFace* > loc_cofaces;
            (*iter1)->getCoFaces(loc_cofaces);

            for (std::vector<CoFace* >::iterator iter2 = loc_cofaces.begin();
                    iter2 != loc_cofaces.end(); ++iter2){
                std::vector<Face* > loc_faces;
                (*iter2)->getFaces(loc_faces);

                for (std::vector<Face* >::iterator iter3 = loc_faces.begin();
                        iter3 != loc_faces.end(); ++iter3){
                    std::vector<Block* > loc_bl;
                    (*iter3)->getBlocks(loc_bl);

                    l_b.insert(l_b.end(), loc_bl.begin(), loc_bl.end());
                }
            }
        }
    }


    l_b.sort(Utils::Entity::compareEntity);
    l_b.unique();

    blocks.insert(blocks.end(),l_b.begin(),l_b.end());
#ifdef _DEBUG2
	std::cout<<"  => trouve "<<blocks.size()<<" blocs"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
bool Vertex::
isEdited() const
{
    return TopoEntity::isEdited()
    || m_save_geom_property != 0
    || m_save_topo_property != 0
    || m_save_mesh_data != 0;
}
/*----------------------------------------------------------------------------*/
void Vertex::getAllVertices(std::vector<Vertex* >& vertices, const bool unique) const
{
    vertices.clear();
    Vertex* vtx = (Vertex*)this;
    vertices.push_back(vtx);
}
/*----------------------------------------------------------------------------*/
void Vertex::getGroupsName (std::vector<std::string>& gn, bool byGeom, bool byTopo) const
{
	if (byGeom)
		TopoEntity::getGroupsName(gn, byGeom, byTopo);

	if (byTopo)
		for (uint i = 0; i<m_topo_property->getGroupsContainer().size(); ++i)
			gn.push_back(m_topo_property->getGroupsContainer().get(i)->getName());
}
/*----------------------------------------------------------------------------*/
void Vertex::updateCoEdgeModificationTime()
{
    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);

    for (std::vector<CoEdge* >::iterator iter=coedges.begin();
            iter != coedges.end(); ++iter)
    	(*iter)->getMeshingProperty()->updateModificationTime();
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
