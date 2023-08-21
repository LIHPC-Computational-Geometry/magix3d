/*----------------------------------------------------------------------------*/
/** \file Edge.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 19/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string.h>
/*----------------------------------------------------------------------------*/
#include "Topo/TopoHelper.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/CommandEditTopo.h"

#include "Mesh/CommandCreateMesh.h"
#include <Mesh/MeshItf.h>

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/SerializedRepresentation.h"

#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/Timer.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IGMesh.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
const char* Edge::typeNameTopoEdge = "TopoEdge";
/*----------------------------------------------------------------------------*/
//#define _DEBUG_TIMER
/*----------------------------------------------------------------------------*/
Edge::
Edge(Internal::Context& ctx,
        CoEdge* ce)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoEdge),
        ctx.newDisplayProperties(Utils::Entity::TopoEdge))
, m_topo_property (new EdgeTopoProperty())
, m_save_topo_property (0)
, m_mesh_property (new EdgeMeshingProperty())
, m_save_mesh_property (0)
{
    // association remontante
    ce->addEdge(this);

    const std::vector<Vertex* > & vertices = ce->getVertices();
    m_topo_property->getVertexContainer().add(vertices);

    m_topo_property->getCoEdgeContainer().add(ce);
	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
Edge::
Edge(Internal::Context& ctx,
        Topo::Vertex* v1,
        Topo::Vertex* v2,
        std::vector<CoEdge* >& coedges)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoEdge),
        ctx.newDisplayProperties(Utils::Entity::TopoEdge))
, m_topo_property(new EdgeTopoProperty())
, m_save_topo_property (0)
, m_mesh_property (new EdgeMeshingProperty())
, m_save_mesh_property (0)
{
    m_topo_property->getVertexContainer().add(v1);
    m_topo_property->getVertexContainer().add(v2);

    m_topo_property->getCoEdgeContainer().add(coedges);

    // association remontante
    for (uint i=0; i<getNbCoEdges(); i++)
        getCoEdge(i)->addEdge(this);
	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
Edge* Edge::
clone()
{
    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);

    Edge* new_edge = new Edge(getContext(), getVertex(0), getVertex(1), coedges);

    delete new_edge->m_mesh_property;
    new_edge->m_mesh_property = m_mesh_property->clone();

    if (m_save_topo_property)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, m_save_topo_property non nul lors du Edge::clone()", TkUtil::Charset::UTF_8));
    if (m_save_mesh_property)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, m_save_mesh_property non nul lors du Edge::clone()", TkUtil::Charset::UTF_8));

    return new_edge;
}
/*----------------------------------------------------------------------------*/
Edge::
Edge(const Edge& e)
: TopoEntity(e.getContext(), 0, 0)
, m_topo_property(0)
, m_save_topo_property (0)
, m_mesh_property (0)
, m_save_mesh_property (0)
{
    MGX_FORBIDDEN("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Edge::
~Edge()
{
#ifdef _DEBUG
    if (m_topo_property == 0)
    	std::cerr<<"Serait-on passé deux fois dans le destructeur ? pour "<<getName()<<std::endl;
#endif

    delete m_topo_property;
    delete m_mesh_property;

#ifdef _DEBUG
    m_topo_property = 0;
    m_mesh_property = 0;

    if (m_save_topo_property)
    	std::cerr<<"La sauvegarde du EdgeTopoProperty a été oubliée pour "<<getName()<<std::endl;
    if (m_save_mesh_property)
    	std::cerr<<"La sauvegarde du EdgeMeshingProperty a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Edge::sortCoEdges()
{
    const std::vector<CoEdge* > & coedges = getCoEdges();
    std::vector<CoEdge* > sorted_coedges;

    if (!TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(1),
            coedges, sorted_coedges)){
        std::cerr<< "Erreur, Edge::sortCoEdges() pour : "<<*this;
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec Edge::sortCoEdges()", TkUtil::Charset::UTF_8));
    }

    m_topo_property->getCoEdgeContainer().clear();
    m_topo_property->getCoEdgeContainer().add(sorted_coedges);
}
/*----------------------------------------------------------------------------*/
void Edge::replace(Vertex* v1, Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd)
{
//    // on ne fait rien en cas d'arête en cours de destruction
//    if (isDestroyed())
//        return;

    if (!find(v1)){
        // si le sommet est interne à l'arête, tout va bien,
        // il n'y a rien à faire pour l'arête ni pour la face,
        // sinon erreur ...

        std::vector<CoEdge* > coedges;
        getCoEdges(coedges);

        for (std::vector<CoEdge* >::iterator iter = coedges.begin();
                iter != coedges.end(); ++iter)
            if ((*iter)->find(v1))
                return;
    }

    // transmet aux entités de niveau supérieur
    // avant de modifier les arêtes, des fois que les arêtes dégénèrent
    if (propagate_up)
    	for (uint i=0; i<getNbCoFaces(); i++)
    		getCoFace(i)->replace(v1, v2, propagate_up, propagate_down, icmd);
    if (propagate_down)
    	for (uint i=0; i<getNbCoEdges(); i++)
    		getCoEdge(i)->replace(v1, v2, propagate_up, propagate_down, icmd);


    bool found = false;
    for (uint i=0; i<getNbVertices(); i++)
        if (v1 == getVertex(i)){
            found = true;

            saveEdgeTopoProperty(icmd);
            m_topo_property->getVertexContainer().set(i,v2);
        }

    if (propagate_up && !found)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (sommet non trouvé), avec Edge::replace", TkUtil::Charset::UTF_8));

    if (getNbVertices() == 2 && getVertex(0) == getVertex(1))
        free(icmd);
}
/*----------------------------------------------------------------------------*/
void Edge::replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd)
{
    // on ne fait rien en cas d'arête en cours de destruction
    if (isDestroyed())
        return;

    for (uint i=0; i<getNbCoEdges(); i++)
        if (e1 == getCoEdge(i)){

        	if (e2->getNbMeshingEdges()<e1->getNbMeshingEdges()){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        		message << "Remplace l'arête "
        				<< e1->getName()<<" par "<< e2->getName()
        				<<" problématique car les discrétisations sont différentes ("
        		 <<(long)e1->getNbMeshingEdges()<<" > "
        		 <<(long)e2->getNbMeshingEdges()<<")";
        		throw TkUtil::Exception (message);
        	}

        	// dans le cas où les discrétisations ont un ratio entre avant et après on en tient compte
        	uint ratio = e2->getNbMeshingEdges() / e1->getNbMeshingEdges();
        	uint reste = e2->getNbMeshingEdges()-ratio*e1->getNbMeshingEdges();
        	if (reste){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        		message << "Remplace l'arête "
        				<< e1->getName()<<" par "<< e2->getName()
						<<" problématique car les discrétisations sont différentes ("
						<<(long)e1->getNbMeshingEdges()<<" < "
						<<(long)e2->getNbMeshingEdges()<<")";
        		throw TkUtil::Exception (message);
        	}
        	if (ratio > 1){
        		saveEdgeMeshingProperty(icmd);
        		setRatio(e2, getRatio(e1)*ratio);
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message<<"Attribution d'un ratio entre les arêtes "<<e1->getName()<<" et "<<e2->getName();
            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
        	}

            saveEdgeTopoProperty(icmd);
            m_topo_property->getCoEdgeContainer().set(i, e2);

            e1->saveCoEdgeTopoProperty(icmd);
            e2->saveCoEdgeTopoProperty(icmd);
            e1->removeEdge(this);
            e2->addEdge(this);
        }
}
/*----------------------------------------------------------------------------*/
void Edge::replace(CoEdge* e1, std::vector<CoEdge*>& coedges, Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
	std::cout<<"Edge::replace pour "<<*this;
	std::cout<<" l'arête "<<e1->getName()<<std::endl;
	std::cout<<" par";
	for (uint i=0; i<coedges.size(); i++)
		std::cout<<" "<<coedges[i]->getName();
	std::cout<<std::endl;
	if (isDestroyed())
		std::cout<<"on ne fait rien car l'arête est en cours de destruction"<<std::endl;
#endif

    // on ne fait rien en cas d'arête en cours de destruction
    if (isDestroyed())
        return;

    saveEdgeTopoProperty(icmd);

    std::vector<CoEdge*> new_edges;
    std::vector<CoEdge*> old_edges;
    getCoEdges(old_edges);
    std::vector<CoEdge*>::iterator iter = old_edges.begin();
    while (e1 != *iter){
    	new_edges.push_back(*iter);
    	++iter;
    	if (iter == old_edges.end()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message<<"Erreur interne, l'arête "<<e1->getName()<<" n'a pas été trouvé dans "<<getName();
    		throw TkUtil::Exception (message);
    	}

    }

    new_edges.insert(new_edges.end(), coedges.begin(), coedges.end());

    ++iter;
    while (iter != old_edges.end()){
    	new_edges.push_back(*iter);
    	++iter;
    }

    e1->removeEdge(this);
    for (uint i=0; i<coedges.size(); i++){
    	coedges[i]->saveCoEdgeTopoProperty(icmd);
    	coedges[i]->addEdge(this);
    }
    m_topo_property->getCoEdgeContainer().clear();
    m_topo_property->getCoEdgeContainer().add(new_edges);
#ifdef _DEBUG2
	std::cout<<"new_edges:";
	for (uint i=0; i<new_edges.size(); i++)
		std::cout<<" "<<new_edges[i]->getName();
	std::cout<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
void Edge::merge(Edge* ed, Internal::InfoCommand* icmd, bool updateOtherEdges)
{
    if (ed == this)
        return;

//    std::cout<<"Edge::merge() entre this: "<<*this<<std::endl;
//    std::cout<<" AVEC : "<<*ed<<std::endl;

    // précondition: avoir les mêmes discrétisations
    if (getNbMeshingEdges() != ed->getNbMeshingEdges()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Fusion entre les arêtes "
                << getName()
                <<" et "
                <<ed->getName()
                <<" impossible car les discrétisations sont différentes (";
        message <<(long)getNbMeshingEdges()
                <<" != ";
        message <<(long)ed->getNbMeshingEdges()
                <<")";
        throw TkUtil::Exception (message);
    }

    // on remplace ed dans les faces associées à ed
    // On travaille sur une copie car il y a une mise à jour en même temps des faces communes
    std::vector<CoFace* > cofaces;
    ed->getCoFaces(cofaces);

    for (std::vector<CoFace* >::iterator iter=cofaces.begin();
            iter != cofaces.end(); ++iter)
        (*iter)->replace(ed, this, icmd);

    // les arêtes qui disparaissent
    std::vector<CoEdge* > old_coedges;
    ed->getCoEdges(old_coedges);

    // changement des coedges de ed par celle de this dans les edges qui références
    if (updateOtherEdges){
    	if (old_coedges.size() == 1){
    		std::vector<CoEdge* > new_coedges;
    		getCoEdges(new_coedges);

    		std::vector<Edge* > edges;
    		old_coedges[0]->getEdges(edges);
    		for (uint i=0; i<edges.size(); i++)
    			if (edges[i] != this)
    				edges[i]->replace(old_coedges[0], new_coedges, icmd);
    	}
    	else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message << "Erreur interne, la fusion de 2 Edge ne peut se faire avec la deuxième ayant autre chose qu'une unique arête ("
    				<<old_coedges.size()<<" actuellement)";
    		throw TkUtil::Exception (message);
    	}
    } // updateOtherEdges

    // il faut libérer les CoEdges de l'arête qui disparait (ed)
    for (std::vector<CoEdge* >::iterator iter = old_coedges.begin();
            iter != old_coedges.end(); ++iter){
//        std::cout<<"libération de "<<(*iter)->getName()<<std::endl;
        (*iter)->free(icmd);
    }

    ed->free(icmd);
}
/*----------------------------------------------------------------------------*/
void Edge::
collapse(Internal::InfoCommand* icmd)
{
    //std::cout<<"Edge::collapse pour : "<<*this<<std::endl;

    if (getNbVertices() != 2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Edge::collapse avec autre chose que 2 sommets", TkUtil::Charset::UTF_8));
    if (getVertex(0) == getVertex(1))
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Edge::collapse avec 2 sommets identiques", TkUtil::Charset::UTF_8));

    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);

    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
        (*iter)->collapse(icmd);
    }

    free(icmd);
}
/*----------------------------------------------------------------------------*/
void Edge::
free(Internal::InfoCommand* icmd)
{
    saveEdgeTopoProperty(icmd);

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

    // on supprime l'arête des relations des arêtes communes vers les arêtes
    for (uint i=0; i<getNbCoEdges(); i++) {
        getCoEdge(i)->saveCoEdgeTopoProperty(icmd);
        getCoEdge(i)->removeEdge(this, false);
    }

    // idem entre faces communes et cette arête
    for (uint i=0; i<getNbCoFaces(); i++) {
        getCoFace(i)->saveCoFaceTopoProperty(icmd);
        getCoFace(i)->removeEdge(this, false);
    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT
void Edge::
split(std::vector<Vertex*> new_vtx, std::vector<Edge*>& new_edges, Internal::InfoCommand* icmd)
{
//    std::cout<<"Edge::split("<<...->getName()<<") avec comme arête : "<<*this<<std::endl;

    // si l'un des sommets pour la coupe est à une extrémité mais qu'il il a un autre sommet qui coupe l'arête,
    // alors on se contante de couper avec le sommet

    // nombre de sommets qui coupent l'arête
    uint nb_splitingVtx = new_vtx.size();

    for (uint i=0; i<new_vtx.size(); i++)
        if (new_vtx[i] == getVertex(0) || new_vtx[i] == getVertex(1))
            nb_splitingVtx --;

    if (nb_splitingVtx == 0)
        new_edges.push_back(this);
    else if (nb_splitingVtx == 1){
        if (new_vtx.size() == 1 || (new_vtx.size() == 2
                && (new_vtx[1] == getVertex(0) || new_vtx[1] == getVertex(1))))
            split(new_vtx[0], new_edges, icmd);
        else
            split(new_vtx[1], new_edges, icmd);
    }
    else if (nb_splitingVtx == 2){
        split(new_vtx[0], new_vtx[1], new_edges, icmd);
    }
}
/*----------------------------------------------------------------------------*/
void Edge::
split(Vertex* new_vtx, std::vector<Edge*>& new_edges, Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"Edge::split("<<new_vtx->getName()<<") avec comme arête : "<<*this<<std::endl;
#endif
#ifdef _DEBUG_TIMER
   TkUtil::Timer timer(true);
#endif
    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);

    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        (*iter)->saveCoEdgeTopoProperty(icmd);

    // recherche des 2 groupes d'arêtes communes
    // on part d'une extrémité de l'arête
    std::vector<CoEdge* > coedges1;
    std::vector<CoEdge* > coedges2;
    TopoHelper::getCoEdgesBetweenVertices(getVertex(0), new_vtx, coedges, coedges1);
    TopoHelper::getCoEdgesBetweenVertices(getVertex(1), new_vtx, coedges, coedges2);

    // construction des 2 nouvelles arêtes
    Edge* edge1 = new Topo::Edge(getContext(), getVertex(0), new_vtx, coedges1);
    Edge* edge2 = new Topo::Edge(getContext(), new_vtx, getVertex(1), coedges2);
    edge1->sortCoEdges();
    edge2->sortCoEdges();
    if (icmd){
        icmd->addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
    }
    new_edges.push_back(edge1);
    new_edges.push_back(edge2);

    // reporte les ratios pour la semi-conformité
    for (uint i=0; i<coedges1.size(); i++)
    	edge1->setRatio(coedges1[i],getRatio(coedges1[i]));
    for (uint i=0; i<coedges2.size(); i++)
    	edge2->setRatio(coedges2[i],getRatio(coedges2[i]));

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"split Edge en  "<<timer.strDuration()<<std::endl;
#endif
#ifdef _DEBUG_SPLIT
    std::cout<<"  donne les 2 arêtes:\n";
    std::cout<<*edge1;
    std::cout<<*edge2;
#endif
}
/*----------------------------------------------------------------------------*/
void Edge::
split(Vertex* new_vtx1, Vertex* new_vtx2, std::vector<Edge*>& new_edges, Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"Edge::split("<<new_vtx1->getName()<<", "<<new_vtx2->getName()<<") avec comme arête : "<<*this<<std::endl;
#endif
#ifdef _DEBUG_TIMER
   TkUtil::Timer timer(true);
#endif

    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);

    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        (*iter)->saveCoEdgeTopoProperty(icmd);

    // recherche des 3 groupes d'arêtes communes
    // on part d'une extrémité de l'arête
    // comme on ne sait pas lequel des 2 sommets est le plus près du premier sommet de l'arête
    // on fait les 2 recherches
    std::vector<CoEdge* > coedges1_1;
    std::vector<CoEdge* > coedges1_2;
    TopoHelper::getCoEdgesBetweenVertices(getVertex(0), new_vtx1, coedges, coedges1_1);
    TopoHelper::getCoEdgesBetweenVertices(getVertex(0), new_vtx2, coedges, coedges1_2);
    bool newVtxOrdonnes = (coedges1_1.size() < coedges1_2.size());

    std::vector<CoEdge* > coedges1;
    std::vector<CoEdge* > coedges2;
    std::vector<CoEdge* > coedges3;
    if (newVtxOrdonnes){
        coedges1 = coedges1_1;
    }
    else {
        coedges1 = coedges1_2;
        Vertex* vtxTmp = new_vtx1;
        new_vtx1 = new_vtx2;
        new_vtx2 = vtxTmp;
    }
    TopoHelper::getCoEdgesBetweenVertices(new_vtx1, new_vtx2, coedges, coedges2);
    TopoHelper::getCoEdgesBetweenVertices(new_vtx2, getVertex(1), coedges, coedges3);

    // construction des 3 nouvelles arêtes
    Edge* edge1 = new Topo::Edge(getContext(), getVertex(0), new_vtx1, coedges1);
    Edge* edge2 = new Topo::Edge(getContext(), new_vtx1, new_vtx2, coedges2);
    Edge* edge3 = new Topo::Edge(getContext(), new_vtx2, getVertex(1), coedges3);
    edge1->sortCoEdges();
    edge2->sortCoEdges();
    edge3->sortCoEdges();
    if (icmd){
        icmd->addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
    }
    new_edges.push_back(edge1);
    new_edges.push_back(edge2);
    new_edges.push_back(edge3);

    // reporte les ratios pour la semi-conformité
    for (uint i=0; i<coedges1.size(); i++)
    	edge1->setRatio(coedges1[i],getRatio(coedges1[i]));
    for (uint i=0; i<coedges2.size(); i++)
    	edge2->setRatio(coedges2[i],getRatio(coedges2[i]));
    for (uint i=0; i<coedges3.size(); i++)
    	edge3->setRatio(coedges3[i],getRatio(coedges3[i]));


#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"split Edge en  "<<timer.strDuration()<<std::endl;
#endif
#ifdef _DEBUG_SPLIT
    std::cout<<"  donne les 3 arêtes:\n";
    std::cout<<*edge1;
    std::cout<<*edge2;
    std::cout<<*edge3;
#endif
}
/*----------------------------------------------------------------------------*/
Vertex* Edge::
getOppositeVertex(Vertex* v)
{
    if (v == getVertex(0)){
        return getVertex(1);
    }
    else if (v == getVertex(1)){
        return getVertex(0);
    }
    else {
        std::cerr<<"Edge::getOppositeVertex("<<v->getName()<<")"<<std::endl;
        std::cerr<<" dans l'arête : "<<*this;
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (sommet non trouvé), avec Edge::getOppositeVertex", TkUtil::Charset::UTF_8));
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
void Edge::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indices = dr.getCurveDiscretization();
    points.clear();
    indices.clear();

    // représentation pour une CoEdge
    Topo::TopoDisplayRepresentation tdr(Utils::DisplayRepresentation::WIRE);

    // on cumule les représentations des CoEdges
    for (uint i=0; i<getNbCoEdges(); i++){
       getCoEdge(i)->getRepresentation(tdr, checkDestroyed);

       std::vector<Utils::Math::Point>& coedge_points = tdr.getPoints();
       std::vector<size_t>& coedge_indices = tdr.getCurveDiscretization();
       uint dec = points.size();
       points.insert(points.end(), coedge_points.begin(), coedge_points.end());
       for (uint j=0; j<coedge_indices.size(); j++)
           indices.push_back(coedge_indices[j] + dec);
    }
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Edge::
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
            Utils::SerializedRepresentation::Property ("Nombre de bras", (long)getNbMeshingEdges()));

    std::vector<CoEdge* > ce;
    getCoEdges(ce);

    for (std::vector<Topo::CoEdge*>::iterator iter = ce.begin( ); ce.end( )!=iter; iter++)
    	if (getRatio(*iter) > 1)
    		topoProprietes.addProperty (
    				Utils::SerializedRepresentation::Property (
    						std::string("ratio ") + (*iter)->getName ( ),  (long)getRatio(*iter)));

    description->addPropertiesSet (topoProprietes);


    // les relations vers les autres types d'entités topologiques

    Utils::SerializedRepresentation  topoRelation ("Relations topologiques", "");

    std::vector<Topo::Vertex* > vtx;
    getVertices(vtx);

    Utils::SerializedRepresentation  vertices ("Sommets topologiques",
            TkUtil::NumericConversions::toStr(vtx.size()));
    for (std::vector<Topo::Vertex*>::iterator iter = vtx.begin( ); vtx.end( )!=iter; iter++)
        vertices.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (vertices);


   Utils::SerializedRepresentation  coedges ("Arêtes communes topologiques",
           TkUtil::NumericConversions::toStr(ce.size()));
   for (std::vector<Topo::CoEdge*>::iterator iter = ce.begin( ); ce.end( )!=iter; iter++)
       coedges.addProperty (
               Utils::SerializedRepresentation::Property (
                       (*iter)->getName ( ),  *(*iter)));
   topoRelation.addPropertiesSet (coedges);


   std::vector<CoFace* > fa;
   getCoFaces(fa);

   Utils::SerializedRepresentation  cofaces ("Faces communes topologiques",
           TkUtil::NumericConversions::toStr(fa.size()));
   for (std::vector<Topo::CoFace*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
       cofaces.addProperty (
               Utils::SerializedRepresentation::Property (
                       (*iter)->getName ( ),  *(*iter)));
   topoRelation.addPropertiesSet (cofaces);


   description->addPropertiesSet (topoRelation);


   return description.release ( );
}
/*----------------------------------------------------------------------------*/
void Edge::
saveEdgeTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (change && m_save_topo_property == 0)
            m_save_topo_property = m_topo_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
EdgeTopoProperty* Edge::
setProperty(EdgeTopoProperty* prop)
{
    EdgeTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Edge::
saveEdgeMeshingProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_property == 0)
            m_save_mesh_property = m_mesh_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
EdgeMeshingProperty* Edge::
setProperty(EdgeMeshingProperty* prop)
{
    EdgeMeshingProperty* tmp = m_mesh_property;
    m_mesh_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Edge::
saveInternals(CommandEditTopo* cet)
{
    TopoEntity::saveInternals (cet);

    if (m_save_mesh_property) {
        cet->addEdgeInfoMeshingProperty(this, m_save_mesh_property);
        m_save_mesh_property = 0;
    }
    if (m_save_topo_property) {
        cet->addEdgeInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
void Edge::
check() const
{
    if (isDestroyed()){
        std::cerr<<"Edge::check() pour l'arête "<<getName()<<std::endl;
        throw TkUtil::Exception (TkUtil::UTF8String ("On utilise une arête détruite", TkUtil::Charset::UTF_8));
    }

    for (uint i=0; i<getNbCoEdges(); i++)
       getCoEdge(i)->check();

    for (uint i=0; i<getNbCoFaces(); i++)
        if (getCoFace(i)->isDestroyed()){
            std::cerr<<"Edge::check() pour l'arête "<<getName()<<std::endl;
            throw TkUtil::Exception (TkUtil::UTF8String ("Une arête pointe sur une face commune détruite", TkUtil::Charset::UTF_8));
        }

}
/*----------------------------------------------------------------------------*/
bool Edge::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Edge & e)
{
    o << "  " << e.getName() << " (uniqueId "<<e.getUniqueId()<<")"
      << (e.isDestroyed()?" (DETRUITE)":"")
      << (e.isEdited()?" (EN COURS D'EDITION)":"")
      << " discrétisée en "<<(short)(e.getNbMeshingEdges())
      << " avec comme arêtes communes:\n";
    const std::vector<Topo::CoEdge* > & edges = e.getCoEdges();
    for (uint i=0; i<edges.size();i++)
        o <<" "<<edges[i]->getName();
//      o <<*edges[i];
    o << "\n";

    o << "  arête avec comme sommets:\n";
    const std::vector<Topo::Vertex* > & vertices = e.getVertices();
    for (uint i=0; i<vertices.size();i++)
        o <<"    "<<*vertices[i]<<"\n";

    const std::vector<Topo::CoFace* > & cofaces = e.getCoFaces();
    if (cofaces.empty()){
        o << "    cette arête ne pointe sur aucune face commune.";
    }
    else {
        o << "    cette arête pointe sur les faces communes :";
        for (uint i=0; i<cofaces.size();i++){
            o << "  " << cofaces[i]->getName();
            if (cofaces[i]->isDestroyed())
                o << "  [DETRUITE] ";
        }
    }
    o << "\n";

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const Edge & e)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << e;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo Edge::getInfos() const
{
	Topo::TopoInfo infos;
	infos.name = getName();
	infos.dimension = getDim();
	getVertices(infos.incident_vertices);
	getCoEdges(infos.incident_coedges);
	getCoFaces(infos.incident_cofaces);
	infos.geom_entity = getGeomAssociation();
	return infos;
}
/*----------------------------------------------------------------------------*/
void Edge::
getNodes(Vertex* v1, Vertex* v2, std::vector<gmds::Node>& vectNd)
{
#ifdef _DEBUG2
	UTF8String	message (TkUtil::Charset::UTF_8);
    message << "Edge::getNodes( \""
            << v1->getName() << "\", "
            << v2->getName() << "\") avec l'arête : "
            << *this;
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
#endif

    vectNd.clear();

    gmds::IGMesh& gmds_mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();

   // le premier noeud
    vectNd.push_back(gmds_mesh.get<gmds::Node>(v1->getNode()));

    // utilise les CoEdge pour obtenir les noeuds internes de l'arête
    // on évite le premier noeud de la CoEdge pour éviter les doublons

    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);
    Vertex* v_dep = v1;
    Vertex* v_opp = 0;


    if (v1 == getVertex(0) && v2 == getVertex(1))
        for (std::vector<CoEdge* >::iterator iter = coedges.begin();
                iter != coedges.end(); ++iter){

            int ratio = getRatio(*iter);

            v_opp = (*iter)->getOppositeVertex(v_dep);

            std::vector<gmds::Node> nodes; // les noeuds de la CoEdge

            (*iter)->getNodes(v_dep, v_opp, nodes);
            std::vector<gmds::Node>::iterator iter2 = nodes.begin();

            uint nb_bras = (*iter)->getNbMeshingEdges() / ratio;

            for (uint j=0; j<nb_bras; j++){
                for (int i=0; i<ratio; i++)
                    ++iter2;
                vectNd.push_back(*iter2);
            }

            v_dep = v_opp;
        }
    else if (v1 == getVertex(1) && v2 == getVertex(0))
        for (std::vector<CoEdge* >::reverse_iterator iter = coedges.rbegin();
                iter != coedges.rend(); ++iter){

            int ratio = getRatio(*iter);

            v_opp = (*iter)->getOppositeVertex(v_dep);

            std::vector<gmds::Node> nodes; // les noeuds de la CoEdge

            (*iter)->getNodes(v_dep, v_opp, nodes);
            std::vector<gmds::Node>::iterator iter2 = nodes.begin();

            uint nb_bras = (*iter)->getNbMeshingEdges() / ratio;

            for (uint j=0; j<nb_bras; j++){
                for (int i=0; i<ratio; i++)
                    ++iter2;
                vectNd.push_back(*iter2);
            }

            v_dep = v_opp;
        }
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne: Edge::getNodes ne trouve pas le sens de parcours", TkUtil::Charset::UTF_8));

    if (v2 != v_opp)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne: Edge::getNodes a échouée, on ne trouve pas le dernier sommet", TkUtil::Charset::UTF_8));


//        TkUtil::UTF8String   message (Charset::UTF_8);
//        message << "Edge::getNodes( \""
//                << v1->getName() << "\", "
//                << v2->getName() << "\") avec l'arête "
//                << getName() << " qui a comme sommets:";
//        for (uint i=0; i<getNbVertices();i++)
//            message <<"  "<<*getVertex(i);
//        message << ", a échoué";
//        throw TkUtil::Exception (message);
//    }

//		TkUtil::UTF8String	message (Charset::UTF_8);
//    message << "Edge::getNodes( \""
//            << v1->getName() << "\", "
//            << v2->getName() << "\") avec l'arête "
//            << getName()<<" donne un vecteur de "<<vectNd.size()<<" noeuds";
//    std::cout<<message<<std::endl;
//    std::cout<<"m_mesh_data->nodes().size() = "<<m_mesh_data->nodes().size()<<std::endl;
}
/*----------------------------------------------------------------------------*/
void Edge::getPoints(std::vector<Utils::Math::Point> &points) const
{
	points.clear();
//	std::cout<<"Edge::getPoints pour "<<getName()<<std::endl;

    // utilise les CoEdge pour obtenir les points internes de l'arête

    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);
    Vertex* v_dep = getVertex(0);
    Vertex* v_opp = 0;

	for (std::vector<CoEdge* >::iterator iter = coedges.begin();
			iter != coedges.end(); ++iter){

		int ratio = getRatio(*iter);

		v_opp = (*iter)->getOppositeVertex(v_dep);

		std::vector<Utils::Math::Point> coedge_points; // les points de la CoEdge
		(*iter)->getPoints(coedge_points);

//		std::cout<<" => CoEdge::getPoints pour "<<(*iter)->getName()
//				<<", v_dep : "<<v_dep->getName()<<", v_opp : "<<v_opp->getName()<<std::endl;

		// si sens inverse / edge, on permute les points
		if ((*iter)->getVertex(0) != v_dep){
//			std::cout<<"  inversion du sens ..."<<std::endl;
			uint nb = coedge_points.size();
			for (uint i=0; i<nb/2; i++){
				Utils::Math::Point pt = coedge_points[i];
				coedge_points[i] = coedge_points[nb-i-1];
				coedge_points[nb-i-1] = pt;
			}
		}

		std::vector<Utils::Math::Point>::iterator iter2 = coedge_points.begin();

		uint nb_bras = (*iter)->getNbMeshingEdges() / ratio;

		if (v_dep == getVertex(0))
			points.push_back(*iter2);

		for (uint j=0; j<nb_bras; j++){
			for (int i=0; i<ratio; i++)
				++iter2;
			points.push_back(*iter2);
		}

		v_dep = v_opp;
	}

    if (getVertex(1) != v_opp)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne: Edge::getPoints a échouée, on ne trouve pas le dernier sommet", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
uint Edge::
getNbVertices() const
{
#ifdef _DEBUG
    if (isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Edge::getNbVertices() pour arête "<<getName()<<" détruite !";
        log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
    }
#endif
    return m_topo_property->getVertexContainer().getNb();
}
/*----------------------------------------------------------------------------*/
void Edge::getAllVertices(std::vector<Vertex* >& vertices, const bool unique) const
{
    vertices.clear();
    std::list<Topo::Vertex*> l_v;
    for(uint l=0; l < getNbCoEdges(); l++){
        CoEdge* coedge = getCoEdge(l);

        const std::vector<Vertex* > & local_vertices = coedge->getVertices();

        l_v.insert(l_v.end(), local_vertices.begin(), local_vertices.end());
    }

    l_v.sort(Utils::Entity::compareEntity);
    l_v.unique();

    vertices.insert(vertices.end(),l_v.begin(),l_v.end());
}
/*----------------------------------------------------------------------------*/
uint Edge::
getNbMeshingEdges() const
{
    const std::vector<CoEdge* > & coedges = getCoEdges();

    uint nb = 0;
    for (std::vector<CoEdge* >::const_iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        nb += (*iter)->getNbMeshingEdges() / m_mesh_property->getRatio(*iter);

    return nb;
}
/*----------------------------------------------------------------------------*/
uint Edge::
getNbNodes() const
{
    return getNbMeshingEdges() + 1;
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty* Edge::
getMeshingProperty()
{
    if (getNbCoEdges() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne: Il n'est pas prévu d'utiliser Edge::getMeshingProperty en dehors du cas avec une seule CoEdge", TkUtil::Charset::UTF_8));

    return getCoEdge(0)->getMeshingProperty();
}
/*----------------------------------------------------------------------------*/
const CoEdgeMeshingProperty* Edge::
getMeshingProperty() const
{
    if (getNbCoEdges() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne: Il n'est pas prévu d'utiliser Edge::getMeshingProperty en dehors du cas avec une seule CoEdge", TkUtil::Charset::UTF_8));

    return getCoEdge(0)->getMeshingProperty();
}
/*----------------------------------------------------------------------------*/
void Edge::setRatio(uint ratio)
{
	for (uint i=0; i<getNbCoEdges(); i++)
		m_mesh_property->setRatio(getCoEdge(i), ratio);
}
/*----------------------------------------------------------------------------*/
void Edge::
setGeomAssociation(Geom::GeomEntity* ge)
{
    // ce sont les arêtes communes qui portent les projections
    for (uint i=0; i<getNbCoEdges(); i++)
        getCoEdge(i)->setGeomAssociation(ge);
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT
void Edge::
computeCorrespondingNbMeshingEdges(Vertex* v_dep, const CoEdge* coedge, uint nbBras_coedge,
        uint& nbBras_edge, bool& sens_coedge)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"computeCorrespondingNbMeshingEdges("<<v_dep->getName()<<", "<<coedge->getName()<<", "<<nbBras_coedge<<")\n";
    std::cout<<" this : "<<*this<<std::endl;
#endif

   // sommes nous dans le même sens que l'arête ?
    bool sens;
    if (v_dep == getVertex(0))
        sens = true;
    else if (v_dep == getVertex(1))
        sens = false;
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::computeCorrespondingNbMeshingEdges ne trouve pas le sommet", TkUtil::Charset::UTF_8));
    }

    // on suppose que les CoEdges sont ordonnées (d'un sommet à un autre)
    int ind_coedge = m_topo_property->getCoEdgeContainer().getIndex(coedge);
    int ind_dep = (sens?0:getNbCoEdges()-1);
    int ind_inc = (sens?1:-1);
    Vertex* v_ar;
    nbBras_edge = 0;

    bool termine = false;
    int i = ind_dep;
    do {
#ifdef _DEBUG_SPLIT
        std::cout<<"i = "<<i<<", nbBras_edge "<<nbBras_edge<<", v_dep = "<<v_dep->getName()<<std::endl;
#endif
        if (v_dep == getCoEdge(i)->getVertex(0)){
            sens_coedge = true;
            v_ar = getCoEdge(i)->getVertex(1);
        } else if (v_dep == getCoEdge(i)->getVertex(1)){
            sens_coedge = false;
            v_ar = getCoEdge(i)->getVertex(0);
        } else {
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::computeCorrespondingNbMeshingEdges ne trouve pas le sommet lors du parcours", TkUtil::Charset::UTF_8));
        }

        if (i!=ind_coedge)
            nbBras_edge += getCoEdge(i)->getNbMeshingEdges()/getRatio(getCoEdge(i));
        else if (sens_coedge)
            nbBras_edge += nbBras_coedge/getRatio(getCoEdge(i));
        else
            nbBras_edge += (getCoEdge(i)->getNbMeshingEdges()-nbBras_coedge)/getRatio(getCoEdge(i));


        if (i==ind_coedge)
            termine = true;

        i+=ind_inc;
        v_dep = v_ar;
    }
    while (!termine);

#ifdef _DEBUG_SPLIT
    std::cout<<" retourne nbBras_edge = "<<nbBras_edge
    		<<", sens_coedge = "<<sens_coedge
    		<<", sens (/edge) = "<<sens
    		<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Edge::
computeCorrespondingCoEdgeAndNbMeshingEdges(Vertex* v_dep, uint nbBras_edge, bool coedge_after_cut,
        CoEdge*& coedge, uint& nbBras_coedge, bool& sens_coedge)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"computeCorrespondingCoEdgeAndNbMeshingEdges("<<v_dep->getName()<<", "<<nbBras_edge<<(coedge_after_cut?", vrai":", faux")<<")\n";
    std::cout<<" this : "<<*this<<std::endl;
#endif
#ifdef _DEBUG_TIMER
   TkUtil::Timer timer(true);
#endif
    nbBras_coedge = 0;
    sens_coedge = true;
    coedge = 0;

    // sommes nous dans le même sens que l'arête ?
    bool sens;
    if (v_dep == getVertex(0))
        sens = true;
    else if (v_dep == getVertex(1))
        sens = false;
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::computeCorrespondingCoEdgeAndNbMeshingEdges ne trouve pas le sommet", TkUtil::Charset::UTF_8));
    }

    int ind_dep = (sens?0:getNbCoEdges()-1);
    int ind_inc = (sens?1:-1);
    Vertex* v_ar;

    bool termine = false;
    int i = ind_dep;

    do {
    	if (i<0 || i>getNbCoEdges()-1){
    		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::computeCorrespondingCoEdgeAndNbMeshingEdges indice i en dehors des bornes...", TkUtil::Charset::UTF_8));
    	}
#ifdef _DEBUG_SPLIT
        std::cout<<"i = "<<i<<", nbBras_edge "<<nbBras_edge<<", v_dep = "<<v_dep->getName()<<std::endl;
#endif
        if (v_dep == getCoEdge(i)->getVertex(0)){
            sens_coedge = true;
            v_ar = getCoEdge(i)->getVertex(1);
        } else if (v_dep == getCoEdge(i)->getVertex(1)){
            sens_coedge = false;
            v_ar = getCoEdge(i)->getVertex(0);
        } else {
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::computeCorrespondingCoEdgeAndNbMeshingEdges ne trouve pas le sommet lors du parcours", TkUtil::Charset::UTF_8));
        }

        if (getCoEdge(i)->getNbMeshingEdges()/getRatio(getCoEdge(i)) < nbBras_edge
        		|| (coedge_after_cut && getCoEdge(i)->getNbMeshingEdges()/getRatio(getCoEdge(i)) == nbBras_edge)){
        	//std::cout<<"nbBras_edge : "<<nbBras_edge<<" -= getNbMeshingEdges: "<<getCoEdge(i)->getNbMeshingEdges()<<" / ratio : "<<getRatio(getCoEdge(i))<<std::endl;
            nbBras_edge -= getCoEdge(i)->getNbMeshingEdges()/getRatio(getCoEdge(i));
            if (nbBras_edge == 0){
            	coedge = getCoEdge(i);
            	nbBras_coedge = getCoEdge(i)->getNbMeshingEdges();
            }
        }
        else {
            if (sens_coedge){
            	//std::cout<<"nbBras_edge = "<<nbBras_edge<<" * ratio : "<<getRatio(getCoEdge(i))<<std::endl;
                nbBras_coedge = nbBras_edge*getRatio(getCoEdge(i));
            }
            else {
            	//std::cout<<"getNbMeshingEdges: "<<getCoEdge(i)->getNbMeshingEdges()<<" - nbBras_edge = "<<nbBras_edge<<" * ratio : "<<getRatio(getCoEdge(i))<<std::endl;
                nbBras_coedge = getCoEdge(i)->getNbMeshingEdges()-nbBras_edge*getRatio(getCoEdge(i));
            }
            coedge = getCoEdge(i);
            termine = true;
        }

        i+=ind_inc;
        v_dep = v_ar;

        if (i<0 || i>getNbCoEdges()-1){
        	termine = true;
        }

    }
    while (!termine);

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"computeCorrespondingCoEdgeAndNbMeshingEdges en  "<<timer.strDuration()<<std::endl;
#endif
#ifdef _DEBUG_SPLIT
    std::cout<<" retourne coedge = "<<coedge->getName()
    		<<", nbBras_coedge = "<<nbBras_coedge
    		<<", sens (/edge) = "<<sens
    		<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
bool Edge::
isSameSense(Vertex* vtx, CoEdge* coedge)
{
    if (vtx == getVertex(0)){
        uint i=0;
        do {
            if (vtx == coedge->getVertex(0))
                return true;
            else if (vtx == coedge->getVertex(1))
                return false;
            // on passe au sommet suivant
            vtx = getCoEdge(i)->getOppositeVertex(vtx);
            i++;
        }
        while (i<getNbCoEdges());
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::isSameSense ne trouve pas le sommet dans l'arête", TkUtil::Charset::UTF_8));

    } else if (vtx == getVertex(1)){
        uint i=0;
        do {
            if (vtx == coedge->getVertex(0))
                return true;
            else if (vtx == coedge->getVertex(1))
                return false;
            // on passe au sommet suivant
            vtx = getCoEdge(getNbCoEdges()-i-1)->getOppositeVertex(vtx);
            i++;
        }
        while (i<getNbCoEdges());
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::isSameSense ne trouve pas le sommet dans l'arête", TkUtil::Charset::UTF_8));

    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::isSameSense ne trouve pas le sommet de départ", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool Edge::
isEdited() const
{
    return TopoEntity::isEdited()
    || m_save_topo_property != 0
    || m_save_mesh_property != 0;
}
/*----------------------------------------------------------------------------*/
CoEdge* Edge::getCoEdge(Vertex* vtx1, Vertex* vtx2)
{
    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);

    uint nb = 0;
    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
     CoEdge* coedge = *iter;
        if ((vtx1 == coedge->getVertex(0) && vtx2 == coedge->getVertex(1))
                || (vtx2 == coedge->getVertex(0) && vtx1 == coedge->getVertex(1)))
            return coedge;
    }

    std::cout<<"Edge::getCoEdge("<<vtx1->getName()<<","<<vtx2->getName()<<") dans l'arête "<<*this<<std::endl;

    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Edge::getCoEdge ne trouve pas l'arête commune à partir des 2 sommets", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
