/*----------------------------------------------------------------------------*/
/** \file CoFace.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13/02/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <cstdio>
#include <string.h>
/*----------------------------------------------------------------------------*/
#include "Topo/TopoHelper.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/Block.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/CommandEditTopo.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
#include "Topo/FaceMeshingPropertyOrthogonal.h"
#include "Topo/FaceMeshingPropertyRotational.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"
#include "Topo/FaceMeshingPropertyDelaunayGMSH.h"
#include "Topo/FaceMeshingPropertyMeshGems.h"

#include "Mesh/CommandCreateMesh.h"
#include "Mesh/MeshManagerIfc.h"
#include "Mesh/MeshItf.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/SerializedRepresentation.h"

#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Internal/InternalPreferences.h"

#include "Geom/GeomEntity.h"

#include "Group/Group2D.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
// #pragma "GCC" optimize "00" // désactivation du inline (avec gcc)
/*----------------------------------------------------------------------------*/
//#define _DEBUG2
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
const char* CoFace::typeNameTopoCoFace = "TopoCoFace";
/*----------------------------------------------------------------------------*/
CoFace::
CoFace(Internal::Context& ctx,
     std::vector<Edge* > &edges,
     bool isStructured)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoCoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoCoFace))
, m_topo_property(new CoFaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(new CoFaceMeshingData())
, m_save_mesh_data(0)
{
#ifdef _DEBUG2
    std::cout << "CoFace::CoFace(...) pour "<<getName()<<" avec "<<edges.size()<<" arêtes"<<std::endl;
#endif
    if (isStructured)
        m_mesh_property = new FaceMeshingPropertyTransfinite();
    else
        m_mesh_property = new FaceMeshingPropertyDelaunayGMSH();

    init(edges);
}
/*----------------------------------------------------------------------------*/
CoFace::
CoFace(Internal::Context& ctx,
     std::vector<Edge* > &edges,
     std::vector<Topo::Vertex* > &vertices,
     bool isStructured,
     bool hasHole)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoCoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoCoFace))
, m_topo_property(new CoFaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(new CoFaceMeshingData())
, m_save_mesh_data(0)
{
    if (isStructured)
        m_mesh_property = new FaceMeshingPropertyTransfinite();
    else
        m_mesh_property = new FaceMeshingPropertyDelaunayGMSH();

    init(edges, vertices);
    m_topo_property->setHoled(hasHole);
}
/*----------------------------------------------------------------------------*/
CoFace::
CoFace(Internal::Context& ctx,
        Edge* e1,
        Edge* e2,
        Edge* e3,
        Edge* e4)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoCoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoCoFace))
, m_topo_property(new CoFaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(new FaceMeshingPropertyTransfinite())
, m_save_mesh_property(0)
, m_mesh_data(new CoFaceMeshingData())
, m_save_mesh_data(0)
{
    std::vector<Edge* > v_e;
    v_e.push_back(e1);
    v_e.push_back(e2);
    v_e.push_back(e3);
    v_e.push_back(e4);
    init(v_e);
}
/*----------------------------------------------------------------------------*/
CoFace::
CoFace(Internal::Context& ctx,
        Edge* e1,
        Edge* e2,
        Edge* e3)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoCoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoCoFace))
, m_topo_property(new CoFaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(new FaceMeshingPropertyTransfinite())
, m_save_mesh_property(0)
, m_mesh_data(new CoFaceMeshingData())
, m_save_mesh_data(0)
{
    std::vector<Edge* > v_e;
    v_e.push_back(e1);
    v_e.push_back(e2);
    v_e.push_back(e3);
    init(v_e);
}
/*----------------------------------------------------------------------------*/
CoFace::
CoFace(Internal::Context& ctx, int ni, int nj,
    		CoFaceMeshingProperty::meshLaw ml,
    		CoFaceMeshingProperty::meshDirLaw md)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoCoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoCoFace))
, m_topo_property(new CoFaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(new CoFaceMeshingData())
, m_save_mesh_data(0)
{
    if (ml == CoFaceMeshingProperty::directional)
        m_mesh_property =new FaceMeshingPropertyDirectional(md);
    else
        m_mesh_property =new FaceMeshingPropertyTransfinite();


    // face avec les sommets équivalents à ceux d'une surface de taille 1
    std::vector<Vertex* > vertices;
    vertices.push_back(new Topo::Vertex(ctx, Utils::Math::Point(1,0,0)));
    vertices.push_back(new Topo::Vertex(ctx, Utils::Math::Point(0,0,0)));
    vertices.push_back(new Topo::Vertex(ctx, Utils::Math::Point(0,1,0)));
    vertices.push_back(new Topo::Vertex(ctx, Utils::Math::Point(1,1,0)));

    // les arêtes (communes)
    std::vector<CoEdge* > coedges;

    // une discretisation par défaut (découpage uniforme suivant le nombre de bras par défaut)
    // à moins qu'un nombre de bras ait été demandé
    EdgeMeshingPropertyUniform empI(ni?ni:ctx.getLocalTopoManager().getDefaultNbMeshingEdges());
    EdgeMeshingPropertyUniform empJ(nj?nj:ctx.getLocalTopoManager().getDefaultNbMeshingEdges());
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, vertices[0], vertices[1]));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, vertices[1], vertices[2]));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, vertices[2], vertices[3]));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, vertices[0], vertices[3]));

    // création des Edges qui utilisent les CoEdges
    std::vector<Edge* > edges;
    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
    	edges.push_back(new Edge(ctx, *iter));
    }

    init(edges, vertices);
}
/*----------------------------------------------------------------------------*/
void CoFace::
init(std::vector<Edge* > &edges,
        std::vector<Topo::Vertex* > &vertices)
{
#ifdef _DEBUG2
    std::cout << "CoFace::init(...) pour "<<getName()<<" avec comme arêtes:";
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        std::cout <<" "<<(*iter)->getName();
    std::cout <<std::endl;
    std::cout << "  et avec comme sommets:";
    for (std::vector<Topo::Vertex* >::iterator iter = vertices.begin();
            iter != vertices.end(); ++iter)
        std::cout <<" "<<(*iter)->getName();
    std::cout <<std::endl;
#endif
   // association remontante
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        (*iter)->addCoFace(this);

    // on copie les arêtes
    m_topo_property->getEdgeContainer().add(edges);

    // on copie les sommets
    m_topo_property->getVertexContainer().add(vertices);

	getContext ( ).newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
void CoFace::
init(std::vector<Edge* > &edges)
{
#ifdef _DEBUG2
    std::cout << "CoFace::init(...) pour "<<getName()<<" avec comme arêtes:";
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        std::cout <<" "<<(*iter)->getName();
        //std::cout <<(**iter);
    std::cout <<std::endl;
#endif

    // association remontante
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        (*iter)->addCoFace(this);

    // on copie les arêtes
    m_topo_property->getEdgeContainer().add(edges);

    // dans le cas non structuré, on s'arrange pour que les arêtes soient ordonnées
    // (forment un ou plusieurs cycles), pas seulement suivant l'id
    if (!isStructured())
        sortEdges();

    if (m_topo_property->hasHole()){
        // cas avec trou, on met juste les sommets dans l'ordre en évitant les doublons
        std::map<Topo::Vertex*, uint> filtre_sommets;

        for (uint i=0; i<getNbEdges(); i++){
            for (uint j=0; j<getEdge(i)->getNbVertices(); j++){
                Topo::Vertex* som = getEdge(i)->getVertex(j);
                if (filtre_sommets[som] == 0){
                    filtre_sommets[som] = 1;
                    m_topo_property->getVertexContainer().add(som);
                }
            }
        }

// ceci n'est pas toujours vrai ... [EB]
//        // on doit avoir autant de sommets que d'arêtes
//        if (getNbVertices() != getNbEdges()){
//            TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
//            messErr << "Erreur avec la face commune "<<getName()
//                    <<", on a "<<(short)getNbEdges()
//                    <<" arêtes, alors que l'on trouve "<<(short)getNbVertices()<<" sommets.";
//            throw TkUtil::Exception(messErr);
//        }

    }
    else { // cas sans trou

        // récupération des sommets ordonnés de manière équivalente aux arêtes
        // avec comme premiers sommets ceux de la première arête
        if (getNbEdges()==0)
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas d'arête), avec CoFace::CoFace",  TkUtil::Charset::UTF_8));
        else if (getNbEdges()==1){
            if (getEdge(0)->getVertex(0) != getEdge(0)->getVertex(1))
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (une unique arête avec des sommets différents), avec CoFace::CoFace",  TkUtil::Charset::UTF_8));
            else
                m_topo_property->getVertexContainer().add(getEdge(0)->getVertex(0));
        }
        else {
            // recherche du sens de la première arête et du sommet commun avec la dernière
            Edge* eN = getEdge(getNbEdges()-1);
            Edge* e1 = getEdge(0);

            bool sens_normal;
            if (e1->getVertex(0) == eN->getVertex(0) || e1->getVertex(0) == eN->getVertex(1))
                sens_normal = true;
            else if (e1->getVertex(1) == eN->getVertex(0) || e1->getVertex(1) == eN->getVertex(1))
                sens_normal = false;
            else{
                std::cerr << "CoFace::init(...) pour "<<getName()<<" avec comme arêtes:\n";
                for (std::vector<Edge* >::iterator iter = edges.begin();
                        iter != edges.end(); ++iter)
                    std::cerr <<(**iter);
                std::cerr <<std::endl;
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas de sommet commun entre 1ère et dernière arête), avec CoFace::CoFace",  TkUtil::Charset::UTF_8));
            }

            // sommet commun avec l'arête précédente
            Topo::Vertex* som_commun = (sens_normal?getEdge(0)->getVertex(0):getEdge(0)->getVertex(1));
            m_topo_property->getVertexContainer().add(som_commun);

            // on boucle sur les arêtes en cherchant le sens de l'arête en fonction du sommet commun
            for (uint i=0; i<getNbEdges()-1; i++){
                Edge* ei = getEdge(i);

                //std::cout<<"Arête : "<<ei->getName()<<" avec les sommets: "<<ei->getVertex(0)->getName()<<" et "<<ei->getVertex(1)->getName()<<std::endl;
                //std::cout<<"som_commun : "<<som_commun->getName()<<std::endl;

                if (ei->getVertex(0) == som_commun)
                    sens_normal = true;
                else if (ei->getVertex(1) == som_commun)
                    sens_normal = false;
                else {
                    std::cout << "CoFace::init(...) pour "<<getName()<<" avec comme arêtes:\n";
                    for (std::vector<Edge* >::iterator iter = edges.begin();
                            iter != edges.end(); ++iter)
                        std::cout <<(**iter);
                    std::cout <<std::endl;
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas de sommet commun avec ième arête), avec CoFace::CoFace",  TkUtil::Charset::UTF_8));
                }
                // sommet commun avec l'arête suivante
                som_commun = (sens_normal?ei->getVertex(1):ei->getVertex(0));
                m_topo_property->getVertexContainer().add(som_commun);
            }

            //        // suppression du dernier sommet (aussi le premier)
            //        m_vertices.pop_back();

        } // end else / if (getNbEdges()==0)
    }
//    std::cout<<"CoFace::CoFace() avec "<<edges.size()<<" arêtes => "<<getNbVertices()<<" sommets"<<std::endl;
//    std::cout<<"CoFace::init() OK\n";
	getContext ( ).newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
void CoFace::
sortEdges()
{
//    std::cout <<"CoFace::sortEdges() \n";
    if (getNbEdges()<=1)
        return;

    // liste des arêtes initiales (sous forme de liste pour optimiser les erase())
    std::list<Edge* > initial_edges;
    m_topo_property->getEdgeContainer().get(initial_edges);

    // arêtes ordonnées
    std::vector<Edge* > sorted_edges;

    // on met les Edges petit à petit en passant de l'un au suivant via un Vertex commun
    // il se peut que plusieurs Edges conviennent, on prend en premier ceux avec 2 Vertex identiques
    // un Edge peut apparaitre plusieurs fois dans la liste initiale (cas d'un cylindre par exemple)
    // donc on enlève de cette liste (initial_edges) au fur et à mesure pour remplir sorted_edges
    std::list<Edge* >::iterator prec_edge_it = initial_edges.begin();
//    std::cout << "CoFace::sortEdges(), Arête initiale:\n";
//    std::cout <<**prec_edge_it;

    Edge* prec_edge = *prec_edge_it;
    sorted_edges.push_back(prec_edge);
    initial_edges.erase(prec_edge_it);
    while (!initial_edges.empty()) {

        // nombre de Vertex communs entre suiv_edge_it et l'un des Vertex de prec_edge_it
        uint nb_common_vertex = 0;
        std::list<Edge* >::iterator suiv_edge_it =  initial_edges.end();

        for (std::list<Edge* >::iterator iter = initial_edges.begin();
                iter != initial_edges.end(); ++iter){
            for (uint i=0; i<prec_edge->getNbVertices(); i++){
                // un Vertex de l'Edge précédent
                Topo::Vertex* vert_prec = prec_edge->getVertex(i);
                uint nb_com_vtx_i = 0;
                for (uint j=0; j<(*iter)->getNbVertices(); j++){
                    Topo::Vertex* vert_suiv = (*iter)->getVertex(j);
                    if (vert_prec == vert_suiv)
                        nb_com_vtx_i++;
                }
                // on identifie l'Edge avec un max de Vertex en commun
                if (nb_com_vtx_i>nb_common_vertex){
                    nb_common_vertex = nb_com_vtx_i;
                    suiv_edge_it = iter;
                }
            }// end for i<prec_edge->getNbVertices()
        } // end for iter = initial_edges.begin()

        if (suiv_edge_it != initial_edges.end()){
//            std::cout << "Arête suivante:\n";
//            std::cout <<**suiv_edge_it;
            prec_edge_it = suiv_edge_it;
        }
        else {
            // on est dans le cas d'une surface trouée
            m_topo_property->setHoled(true);
            // on recherche le cycle suivant
            prec_edge_it = initial_edges.begin();
//            std::cout << "(Trou détecté) Arête suivante:\n";
//            std::cout <<**prec_edge_it;
        }
        prec_edge = *prec_edge_it;
        sorted_edges.push_back(prec_edge);
        initial_edges.erase(prec_edge_it);

    } // end while (!m_edges.empty())

    if (getNbEdges() != sorted_edges.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CoFace::sortEdges(), vecteurs de tailles différentes",  TkUtil::Charset::UTF_8));

    m_topo_property->getEdgeContainer().clear();
    m_topo_property->getEdgeContainer().add(sorted_edges);
}
/*----------------------------------------------------------------------------*/
CoFace::
CoFace(const CoFace& f)
: TopoEntity(f.getContext(), 0, 0)
, m_topo_property(0)
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(0)
, m_save_mesh_data(0)
{
    MGX_FORBIDDEN("Constructeur de copie pour CoFace");
}
/*----------------------------------------------------------------------------*/
CoFace::
~CoFace()
{
#ifdef _DEBUG
    if (m_topo_property == 0 || m_mesh_property == 0 || m_mesh_data == 0)
    	std::cerr<<"Serait-on passé deux fois dans le destructeur ? pour "<<getName()<<std::endl;
#endif

    delete m_topo_property;
    delete m_mesh_property;
    delete m_mesh_data;

#ifdef _DEBUG
    m_topo_property = 0;
    m_mesh_property = 0;
    m_mesh_data = 0;

    if (m_save_topo_property)
    	std::cerr<<"La sauvegarde du CoFaceTopoProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_property)
    	std::cerr<<"La sauvegarde du CoFaceMeshingProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_data)
    	std::cerr<<"La sauvegarde du CoFaceMeshingData a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CoFace::
replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd)
{
//    std::cout<<"CoFace::replace("<<v1->getName()<<", "<<v2->getName()<<")\n";
//    std::cout<<"CoFace initiale: "<<*this<<std::endl;

//    // on ne fait rien en cas de CoFace en cours de destruction
//    if (isDestroyed())
//        return;

    for (uint i=0; i<getNbVertices(); i++)
        if (v1 == getVertex(i)){
            saveCoFaceTopoProperty(icmd);

            // 2 cas de figure
            // soit v2 est déjà présent, donc la face va être dégénérée (v1 disparait)
            // soit v2 est nouveau, on remplace v1 par v2
            if (m_topo_property->getVertexContainer().find(v2)){
                if (isStructured()){
                    if (getNbVertices() == 4){
                        permuteToJmaxEdge(getEdge(v1,v2), icmd);
                        if (v1 == getVertex(0))
                            m_topo_property->getVertexContainer().set(0, v2);
                        m_topo_property->getVertexContainer().resize(3);
                        // suppression de l'arête
                        m_topo_property->getEdgeContainer().resize(3);
                    }
                    else {
                        m_topo_property->getEdgeContainer().remove(getEdge(v1,v2), true);
                        m_topo_property->getVertexContainer().remove(v1, true);
                    }
                }
                else {
                    permuteToLastEdge(getEdge(v1,v2), icmd);
                    if (v1 == getVertex(0))
                        m_topo_property->getVertexContainer().set(0, v2);
                    uint new_size = m_topo_property->getVertexContainer().getNb()-1;
                    m_topo_property->getVertexContainer().resize(new_size);
                    m_topo_property->getEdgeContainer().resize(new_size);
                }
            } else
                m_topo_property->getVertexContainer().set(i, v2);

        }// end if (v1 == getVertex(i))

    // transmet aux entités de niveau supérieur (les Faces)
    if (propagate_up)
    	for (uint j=0; j<getNbFaces(); j++)
    		getFace(j)->replace(v1, v2, propagate_up, propagate_down, icmd);
    if (propagate_down)
    	for (uint j=0; j<getNbEdges(); j++)
    		getEdge(j)->replace(v1, v2, propagate_up, propagate_down, icmd);

//    // une des arêtes disparait
//    if (getNbVertices() == 2){
//        getEdge(0)->merge(getEdge(1), icmd);
//    }

    // destruction de la face si elle est dégénérée en une arête ou moins
    if (getNbVertices()<3){
//        free(icmd);
        setDestroyed(true);
        if (icmd)
            icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);
    }

//    std::cout<<"CoFace finale: "<<*this<<std::endl;
}
/*----------------------------------------------------------------------------*/
void CoFace::
replace(Edge* e1, Edge* e2, Internal::InfoCommand* icmd)
{
    // on ne fait rien en cas de face commune en cours de destruction
    if (isDestroyed())
        return;

    for (uint i=0; i<getNbEdges(); i++)
        if (e1 == getEdge(i)){
            saveCoFaceTopoProperty(icmd);
            m_topo_property->getEdgeContainer().set(i, e2);

            e1->saveEdgeTopoProperty(icmd);
            e2->saveEdgeTopoProperty(icmd);
            e1->removeCoFace(this);
            e2->addCoFace(this);
        }

    // pas de transmition aux blocs, ils n'ont pas la connaissance des arêtes
}
/*----------------------------------------------------------------------------*/
void CoFace::
replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd)
{
    for (uint i=0; i<getNbEdges(); i++)
    	getEdge(i)->replace(e1, e2, icmd);
}
/*----------------------------------------------------------------------------*/
void CoFace::
merge(CoFace* coface, Internal::InfoCommand* icmd)
{
    if (coface == this)
        return;

#ifdef _DEBUG2
    std::cout<<"CoFace::merge("<<coface->getName()<<") pour "<<getName()<<std::endl;
#endif


    // on remplace coface dans les faces qui référencent cette CoFace
    // On travaille sur une copie car il y a une mise à jour en même temps de la liste des faces
    std::vector<Face* > faces;
    coface->getFaces(faces);
    for (std::vector<Face* >::iterator iter = faces.begin();
                iter != faces.end(); ++iter)
        (*iter)->replace(coface, this, icmd);

    // il faut libérer les arêtes de la face commune qui disparait
    std::vector<Edge* > edges;
    coface->getEdges(edges);

    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter){
        (*iter)->saveEdgeTopoProperty(icmd);
        (*iter)->removeCoFace(coface);

        if ((*iter)->getNbCoFaces() == 0){
            // cas où l'arête peut être supprimée
            (*iter)->free(icmd);
        }
    }

    // récupère l'association côté coface si celle de this est dédtruite
     if (getGeomAssociation() && getGeomAssociation()->isDestroyed() && coface->getGeomAssociation() && !coface->getGeomAssociation()->isDestroyed()){
     	saveTopoProperty();
     	setGeomAssociation(coface->getGeomAssociation());
     }

    coface->free(icmd);
}
/*----------------------------------------------------------------------------*/
void CoFace::
setDestroyed(bool b)
{
	if (b && !isFinished() && isMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "On ne peut pas détruire une face sans avoir détruit le maillage.\n"
                << "Ce que l'on tente de faire pour la face "<<getName();
        throw TkUtil::Exception (message);
    }

    // cas sans changement
    if (isDestroyed() == b)
        return;

    // on retire la relation depuis les groupes
     Utils::Container<Group::Group2D>& groups = getGroupsContainer();
#ifdef _DEBUG2
    std::cout<<"Les groupes:";
    for  (uint i=0; i<groups.size(); i++)
		 std::cout<<" "<<groups.get(i)->getName();
    std::cout<<std::endl;
#endif


     if (b)
    	 for (uint i=0; i<groups.size(); i++){
    		 Group::Group2D* gr = groups.get(i);
    		 gr->remove(this);
    	 }
     else
    	 for (uint i=0; i<groups.size(); i++){
    		 Group::Group2D* gr = groups.get(i);
    		 gr->add(this);
    	 }

    TopoEntity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
void CoFace::
free(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
    std::cout<<"CoFace::free pour "<<getName()<<std::endl;
#endif

    saveCoFaceTopoProperty(icmd);

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

    // on supprime la face commune des relations des faces vers les faces communes
    for (uint i=0; i<getNbFaces(); i++) {
        getFace(i)->saveFaceTopoProperty(icmd);
        getFace(i)->removeCoFace(this, false);
    }

    // idem entre arêtes et cette face commune
    for (uint i=0; i<getNbEdges(); i++) {
        getEdge(i)->saveEdgeTopoProperty(icmd);
        getEdge(i)->removeCoFace(this, false);
    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT
std::vector<Edge*> CoFace::
split(eDirOnCoFace dir, std::vector<Edge*>& edges1, std::vector<Edge*>& edges3,
        std::vector<Topo::Vertex*>& vtx1, std::vector<Topo::Vertex*>& vtx2,
        Internal::InfoCommand* icmd)
{

#ifdef _DEBUG_SPLIT
    std::cout<<"Découpage de la face commune : "<<*this; //<<getName() std::endl;
    std::cout<<"edges1 : ";
    for (uint i=0; i<edges1.size(); i++)
        std::cout<<" "<<(edges1[i]?edges1[i]->getName():"undef");
    std::cout<<std::endl;
//    for (uint i=0; i<edges1.size(); i++)
//    	if (edges1[i])
//    		std::cout<<*edges1[i];
    std::cout<<"edges3 : ";
    for (uint i=0; i<edges3.size(); i++)
        std::cout<<" "<<(edges3[i]?edges3[i]->getName():"undef");
    std::cout<<std::endl;
//    for (uint i=0; i<edges3.size(); i++)
//    	if (edges3[i])
//    		std::cout<<*edges3[i];
   std::cout<<"vtx1 : ";
    for (uint i=0; i<vtx1.size(); i++)
        std::cout<<" "<<(vtx1[i]?vtx1[i]->getName():"undef");
    std::cout<<std::endl;
    std::cout<<"vtx2 : ";
    for (uint i=0; i<vtx2.size(); i++)
        std::cout<<" "<<(vtx2[i]?vtx2[i]->getName():"undef");
    std::cout<<std::endl;
#endif

    std::vector<Edge*> newEdges;

    if (edges1.size() == 1 && edges3.size() == 1){
        newEdges.push_back(getEdge(vtx1[0], vtx2[0]));
    }
    else if (edges1.size() == 2 && (edges3.size() == 2 || edges3.empty())){
        if (vtx1.size() == 2){
            // il faut supprimer les sommets sur un bord de la face
            std::vector<Topo::Vertex*> vtx1bis;
            std::vector<Topo::Vertex*> vtx2bis;
            if (!find(vtx1[0])){ // on fait le test que sur un des sommets
                vtx1bis.push_back(vtx1[0]);
                vtx2bis.push_back(vtx2[0]);
            }
            if (!find(vtx1[1])){
                vtx1bis.push_back(vtx1[1]);
                vtx2bis.push_back(vtx2[1]);
            }

            newEdges = split2(dir, edges1, edges3, vtx1bis, vtx2bis, icmd);
        }
        else
            newEdges = split2(dir, edges1, edges3, vtx1, vtx2, icmd);
    }
    else if (edges1.size() == 3 && (edges3.size() == 3 || edges3.empty())){
        newEdges = split3(dir, edges1, edges3, vtx1, vtx2, icmd);
    }
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::split() ne peut se faire avec autre chose que 2 ou 3 couples d'arêtes",  TkUtil::Charset::UTF_8));

    return newEdges;
}
/*----------------------------------------------------------------------------*/
std::vector<Edge*> CoFace::
split2(eDirOnCoFace dir, std::vector<Edge*>& edges1, std::vector<Edge*>& edges3,
        std::vector<Topo::Vertex*> vtx1, std::vector<Topo::Vertex*> vtx2,
        Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"CoFace::split2("<<dir<<", ["<<edges1[0]->getName()<<","<<edges1[1]->getName()<<"]";
    if (edges3.empty())
        std::cout<<", edges3 vide";
    else
        std::cout<<", ["<<edges3[0]->getName()<<","<<edges3[1]->getName()<<"]";
    std::cout<<", [";
    for (uint i=0; i<vtx1.size(); i++)
        std::cout<<" "<<(vtx1[i]?vtx1[i]->getName():"undef");
    std::cout<<"], [";
    for (uint i=0; i<vtx2.size(); i++)
        std::cout<<" "<<(vtx2[i]?vtx2[i]->getName():"undef");
    std::cout<<"])"<<std::endl;
#endif

    //    std::cout<<"Découpage de la face commune en 2 : "<<getName()<<std::endl;//*this;
    if (vtx1.size() != 1 || vtx2.size() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::split2 devrait recevoir 2 groupes de 1 sommet",  TkUtil::Charset::UTF_8));

//    Edge* edge1 = getEdge(edges1[0]->getOppositeVertex(vtx1), edges1[1]->getOppositeVertex(vtx1));
//    Edge* edge3;
//    if (!edges3.empty())
//        edge3 = getEdge(edges3[0]->getOppositeVertex(vtx2), edges3[1]->getOppositeVertex(vtx2));

    Edge* edge0 = 0;
    Edge* edge2 = 0;
    if (dir == i_dir){
        edge0 = getEdge(0);
        edge2 = getEdge(2);
    } else {
        edge0 = getEdge(1);
        if (getNbEdges() == 4)
            edge2 = getEdge(3);
    }
#ifdef _DEBUG_SPLIT
    std::cout<<"edge0 : "<<edge0->getName()<<std::endl;
    std::cout<<"edge2 : "<<edge2->getName()<<std::endl;
#endif

    // récupération de la discrétisation d'une arête du bord
    CoEdgeMeshingProperty* cemp = 0;
    bool edge0_isOnAxe = (Utils::Math::MgxNumeric::isNearlyZero(edge0->getVertex(0)->getY())
    		&& Utils::Math::MgxNumeric::isNearlyZero(edge0->getVertex(1)->getY()));
    bool edge2_isOnAxe = (edge2 && Utils::Math::MgxNumeric::isNearlyZero(edge2->getVertex(0)->getY())
    		&& Utils::Math::MgxNumeric::isNearlyZero(edge2->getVertex(1)->getY()));

#ifdef _DEBUG_SPLIT
    std::cout<<"edge0_isOnAxe : "<<edge0_isOnAxe<<std::endl;
    std::cout<<"edge2_isOnAxe : "<<edge2_isOnAxe<<std::endl;
#endif

    if (edge0->getNbCoEdges() == 1
    		&& !(edge0->getCoEdge(0)->getMeshLaw() == CoEdgeMeshingProperty::interpolate // on évite l'interpolation vers l'axe
    				&& edge2_isOnAxe)){
    	CoEdge* coedge0 = edge0->getCoEdge(0);
#ifdef _DEBUG_SPLIT
    	std::cout<<" Utilisation de la discrétisation de "<<coedge0->getName()<<" (edge0)"<<std::endl;
#endif
        cemp = coedge0->getMeshingProperty()->clone();
        // inverse le sens si nécessaire
        if (edges1[0]->find(coedge0->getVertex(1))
                || edges1[1]->find(coedge0->getVertex(1)) ){
            cemp->setDirect(!coedge0->getMeshingProperty()->getDirect());
#ifdef _DEBUG_SPLIT
            std::cout<<"  avec inversion du sens"<<std::endl;
#endif
        }
    }
    else if (0 != edge2 && edge2->getNbCoEdges() == 1
    		&& !(edge2->getCoEdge(0)->getMeshLaw() == CoEdgeMeshingProperty::interpolate // on évite l'interpolation vers l'axe
    				&& edge0_isOnAxe)){
    	CoEdge* coedge2 = edge2->getCoEdge(0);
#ifdef _DEBUG_SPLIT
    	std::cout<<" Utilisation de la discrétisation de "<<coedge2->getName()<<" (edge2)"<<std::endl;
#endif
        cemp = coedge2->getMeshingProperty()->clone();
        if (edges1[0]->find(coedge2->getVertex(1))
                || edges1[1]->find(coedge2->getVertex(1)) ){
        	cemp->setDirect(!coedge2->getMeshingProperty()->getDirect());
#ifdef _DEBUG_SPLIT
        	std::cout<<"  avec inversion du sens"<<std::endl;
#endif
        }
    }
    else {
    	// nous allons construire une interpolation par rapport aux arêtes à une extrémité
    	// on évite de référencer les arêtes sur l'axe des X car elles peuvent disparaitre
    	// dans le cas d'une construction de blocs par révolution entière

    	Edge* edge = 0;
    	CHECK_NULL_PTR_ERROR(edge0);
    	CHECK_NULL_PTR_ERROR(edge2);
    	if (!edge0_isOnAxe)
    		edge = edge0;
    	else if (!edge2_isOnAxe)
    		edge = edge2;

    	if (edge) {
    		std::vector<std::string> coedgesName;
    		for (uint i=0; i<edge->getNbCoEdges(); i++)
    			coedgesName.push_back(edge->getCoEdge(i)->getName());
    		cemp = new EdgeMeshingPropertyInterpolate(edge->getNbMeshingEdges(), coedgesName);
    	}
    }

    // pour les cas où l'on n'ait rien réussi à faire
    if (cemp == 0)
    	cemp = new EdgeMeshingPropertyUniform(edge0->getNbMeshingEdges());

    vtx1[0]->saveVertexTopoProperty(icmd);
    vtx2[0]->saveVertexTopoProperty(icmd);

    MAJInterpolated(cemp);

#ifdef _DEBUG_SPLIT
    std::cout<<" cemp après MAJInterpolated getScriptCommandBegin => "<<cemp->getScriptCommandBegin()<<std::endl;
#endif

    // création de l'arête centrale (qui coupe la face commune)
    Topo::CoEdge* newCoedge = new Topo::CoEdge(getContext(), cemp, vtx1[0], vtx2[0]);
    delete cemp;

    // on reporte la projection sur la nouvelle arête commune
    newCoedge->setGeomAssociation(getGeomAssociation());

    Topo::Edge* newEdge = new Topo::Edge(getContext(), newCoedge);
    if (icmd){
        icmd->addTopoInfoEntity(newCoedge, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(newEdge, Internal::InfoCommand::CREATED);
    }

    // on vérifie si c'est la première ou la deuxième Edge qui est en relation avec edge0
    bool edges1_sens = edge0->find(edges1[0]->getVertex(0));
    bool edges3_sens = (edges3.empty()?true:edge0->find(edges3[0]->getVertex(0)));

    // création des 2 nouvelles faces communes
    Topo::CoFace* coface1 = 0;
    Topo::CoFace* coface2 = 0;

    // on sauvegarde l'état des arêtes avant création de la face commune
    edge0->saveEdgeTopoProperty(icmd);
    if (0 != edge2)
        edge2->saveEdgeTopoProperty(icmd);

    if (edges3.empty())
        // Face commune dégénérée
        coface1 = new CoFace(getContext(),
                edge0,
                edges1[edges1_sens?0:1],
                newEdge);
    else
        coface1 = new CoFace(getContext(),
                edge0,
                edges1[edges1_sens?0:1],
                newEdge,
                edges3[edges3_sens?0:1]);

    if (edges3.empty())
        coface2 = new CoFace(getContext(),
                newEdge,
                edges1[edges1_sens?1:0],
                edge2);
    else if (0 == edge2)
        coface2 = new CoFace(getContext(),
                edges1[edges1_sens?1:0],
                newEdge,
                edges3[edges3_sens?1:0]);
    else
        coface2 = new CoFace(getContext(),
                newEdge,
                edges1[edges1_sens?1:0],
                edge2,
                edges3[edges3_sens?1:0]);

    if (icmd){
        icmd->addTopoInfoEntity(coface1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(coface2, Internal::InfoCommand::CREATED);
    }

    // On reprend la même projection
    if (getGeomAssociation()){
        coface1->setGeomAssociation(getGeomAssociation());
        coface2->setGeomAssociation(getGeomAssociation());
    }

    Utils::Container<Group::Group2D>& groups = getGroupsContainer();
    for (uint i=0; i<groups.size(); i++){
    	Group::Group2D* gr = groups.get(i);
    	gr->add(coface1);
    	gr->add(coface2);
    	coface1->getGroupsContainer().add(gr);
    	coface2->getGroupsContainer().add(gr);
    }

    // recherche de la méthode de maillage qui semble la plus adaptée
    if (getMeshLaw() == CoFaceMeshingProperty::directional){
    	coface1->selectBasicMeshLaw(icmd);
    	coface2->selectBasicMeshLaw(icmd);
    }

    // on ajoute les 2 CoFaces aux faces
    for (uint i=0; i<getNbFaces(); i++) {
        getFace(i)->saveFaceTopoProperty(icmd);
        getFace(i)->addCoFace(coface1);
        getFace(i)->addCoFace(coface2);

        coface1->addFace(getFace(i));
        coface2->addFace(getFace(i));
    }

    // destruction de la face commune actuelle
    free(icmd);

    //        std::cout<<"coface1 en sortie du split: "<<*coface1;
    //        std::cout<<"coface2 en sortie du split: "<<*coface2;

    std::vector<Edge*> newEdges;
    newEdges.push_back(newEdge);

    // met à jour les interpolations se basant sur cette coface qui est découpée
    MAJInterpolated(coface1, newEdges);
    MAJInterpolated(coface2, newEdges);

    return newEdges;
}
/*----------------------------------------------------------------------------*/
std::vector<Edge*> CoFace::
split3(eDirOnCoFace dir, std::vector<Edge*>& edges1, std::vector<Edge*>& edges3,
        std::vector<Topo::Vertex*> vtx1, std::vector<Topo::Vertex*> vtx2,
        Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"CoFace::split3("<<dir<<", ["<<edges1[0]->getName()<<","<<edges1[1]->getName()<<","<<edges1[2]->getName()<<"]";
    if (edges3.empty())
        std::cout<<", edges3 vide";
    else
        std::cout<<", ["<<edges3[0]->getName()<<","<<edges3[1]->getName()<<","<<edges3[2]->getName()<<"]";
    std::cout<<", [";
    for (uint i=0; i<vtx1.size(); i++)
        std::cout<<" "<<(vtx1[i]?vtx1[i]->getName():"undef");
    std::cout<<"], [";
    for (uint i=0; i<vtx2.size(); i++)
        std::cout<<" "<<(vtx2[i]?vtx2[i]->getName():"undef");
    std::cout<<"])"<<std::endl;
#endif

    //    std::cout<<"Découpage de la face commune en 3 : "<<getName()<<std::endl;//*this;
    if (vtx1.size() != 2 || vtx2.size() != 2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::split3 devrait recevoir 2 groupes de 2 sommets",  TkUtil::Charset::UTF_8));

    // on met les sommets dans l'ordre pour éviter de croiser les arêtes
    if (getNbVertices()==4){

    	// l'indice de l'arête dans laquelle sont les sommets de vtx1
    	uint ind_edge_vtx1=5;
    	for (uint i=0; i<getNbEdges(); i++){
    		std::vector<Topo::Vertex*> all_vtx;
    		getEdge(i)->getAllVertices(all_vtx);
    		for (uint j=0; j<all_vtx.size(); j++)
    			if (all_vtx[j] == vtx1[0])
    				ind_edge_vtx1 = i;
    	}
    	if (ind_edge_vtx1 == 5)
    		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::split3, on ne trouve pas vtx1[0] dans l'une des arêtes",  TkUtil::Charset::UTF_8));
    	uint ind_edge_vtx2=5;
    	for (uint i=0; i<getNbEdges(); i++){
    		std::vector<Topo::Vertex*> all_vtx;
    		getEdge(i)->getAllVertices(all_vtx);
    		for (uint j=0; j<all_vtx.size(); j++)
    			if (all_vtx[j] == vtx2[0])
    				ind_edge_vtx2 = i;
    	}
    	if (ind_edge_vtx2 == 5)
    		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::split3, on ne trouve pas vtx2[0] dans l'une des arêtes",  TkUtil::Charset::UTF_8));

    	std::vector<Topo::CoEdge* > coedges_between;
    	std::vector<Topo::Vertex*> innerVertices;
    	TopoHelper::getCoEdgesBetweenVertices(getVertex(ind_edge_vtx1), getVertex((ind_edge_vtx1+1)%4),
    			getEdge(ind_edge_vtx1)->getCoEdges(), coedges_between);
    	TopoHelper::getInnerVertices(coedges_between, innerVertices);
#ifdef _DEBUG_SPLIT
    	std::cout<<" innerVertices pour ind_edge_vtx1 :"<<std::endl;
    	for (uint i=0; i<innerVertices.size(); i++)
    		std::cout<<"  "<<*innerVertices[i]<<std::endl;
    	std::cout<<std::endl;
#endif
    	bool vtx1_same_dir = (vtx1[0] == innerVertices[0]);
    	coedges_between.clear();
    	innerVertices.clear();
    	TopoHelper::getCoEdgesBetweenVertices(getVertex((ind_edge_vtx2+1)%4), getVertex(ind_edge_vtx2),
    			getEdge(ind_edge_vtx2)->getCoEdges(), coedges_between);
    	TopoHelper::getInnerVertices(coedges_between, innerVertices);
#ifdef _DEBUG_SPLIT
    	std::cout<<" innerVertices pour ind_edge_vtx2 :"<<std::endl;
    	for (uint i=0; i<innerVertices.size(); i++)
    		std::cout<<"  "<<*innerVertices[i]<<std::endl;
    	std::cout<<std::endl;
#endif
    	bool vtx2_same_dir = (vtx2[0] == innerVertices[0]);

    	if (vtx1_same_dir != vtx2_same_dir){
    		// on fait une permutation
    		Vertex* vtxTmp = vtx2[0];
    		vtx2[0] = vtx2[1];
    		vtx2[1] = vtxTmp;
    	}
    } // end if (getNbVertices()==4)


    Edge* edge0 = 0;
    Edge* edge2 = 0;
    if (dir == i_dir){
        edge0 = getEdge(0);
        edge2 = getEdge(2);
    } else {
        edge0 = getEdge(1);
        if (getNbEdges() == 4)
            edge2 = getEdge(3);
    }
    //std::cout<<"edge0 : "<<edge0->getName()<<std::endl;
    //std::cout<<"edge2 : "<<edge2->getName()<<std::endl;

    // récupération de la discrétisation d'une arête du bord
    CoEdgeMeshingProperty* cemp = 0;
    if (edge0->getNbCoEdges() == 1){
        cemp = edge0->getCoEdge(0)->getMeshingProperty()->clone();
        // inverse le sens si nécessaire
        if (edges1[0]->find(edge0->getVertex(1))
                || edges1[1]->find(edge0->getVertex(1)) )
            cemp->setDirect(!edge0->getMeshingProperty()->getDirect());
    }
    else if (0 != edge2 && edge2->getNbCoEdges() == 1){
        cemp = edge2->getCoEdge(0)->getMeshingProperty()->clone();
        if (edges1[0]->find(edge2->getVertex(1))
                || edges1[1]->find(edge2->getVertex(1)) )
            cemp->setDirect(!edge2->getMeshingProperty()->getDirect());
    }
    else
        cemp = new EdgeMeshingPropertyUniform(edge0->getNbMeshingEdges());

    vtx1[0]->saveVertexTopoProperty(icmd);
    vtx2[0]->saveVertexTopoProperty(icmd);
    vtx1[1]->saveVertexTopoProperty(icmd);
    vtx2[1]->saveVertexTopoProperty(icmd);

    MAJInterpolated(cemp);

    // création des arêtes centrales (qui coupe la face commune)
    Topo::CoEdge* newCoedge1 = new Topo::CoEdge(getContext(), cemp, vtx1[0], vtx2[0]);
    Topo::CoEdge* newCoedge2 = new Topo::CoEdge(getContext(), cemp, vtx1[1], vtx2[1]);
    delete cemp;

    // on reporte la projection sur les nouvelles arêtes communes
    newCoedge1->setGeomAssociation(getGeomAssociation());
    newCoedge2->setGeomAssociation(getGeomAssociation());

    Topo::Edge* newEdge1 = new Topo::Edge(getContext(), newCoedge1);
    Topo::Edge* newEdge2 = new Topo::Edge(getContext(), newCoedge2);
    if (icmd){
        icmd->addTopoInfoEntity(newCoedge1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(newCoedge2, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(newEdge1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(newEdge2, Internal::InfoCommand::CREATED);
    }

    // on vérifie si c'est la première ou la dernière Edge qui est en relation avec edge0
    bool edges1_sens = edge0->find(edges1[0]->getVertex(0));
    bool edges3_sens = (edges3.empty()?true:edge0->find(edges3[0]->getVertex(0)));

    // on regarde s'il faut permuter ou non l'ordre des newEdge1 et newEdge2
    bool newEdges_sens;
    if (edges1[edges1_sens?0:2]->find(vtx1[0]) || edges1[edges1_sens?0:2]->find(vtx2[0]))
        newEdges_sens = true;
    else if (edges1[edges1_sens?0:2]->find(vtx1[1]) || edges1[edges1_sens?0:2]->find(vtx2[1]))
        newEdges_sens = false;
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::split3 on ne trouve pas l'ordre pour les nouvelles arêtes",  TkUtil::Charset::UTF_8));

    // création des 3 nouvelles faces communes
    Topo::CoFace* coface1;
    Topo::CoFace* coface2;
    Topo::CoFace* coface3;

    // on sauvegarde l'état des arêtes avant création de la face commune
    edge0->saveEdgeTopoProperty(icmd);
    if (0 != edge2)
        edge2->saveEdgeTopoProperty(icmd);

    if (edges3.empty())
        // Face commune dégénérée
        coface1 = new CoFace(getContext(),
                edge0,
                edges1[edges1_sens?0:2],
                newEdges_sens?newEdge1:newEdge2);
    else
        coface1 = new CoFace(getContext(),
                edge0,
                edges1[edges1_sens?0:2],
                newEdges_sens?newEdge1:newEdge2,
                edges3[edges3_sens?0:2]);

    if (edges3.empty())
        // Face commune dégénérée
        coface2 = new CoFace(getContext(),
                newEdges_sens?newEdge1:newEdge2,
                edges1[1],
                newEdges_sens?newEdge2:newEdge1);
    else
        coface2 = new CoFace(getContext(),
                newEdges_sens?newEdge1:newEdge2,
                edges1[1],
                newEdges_sens?newEdge2:newEdge1,
                edges3[1]);

    if (edges3.empty())
        coface3 = new CoFace(getContext(),
                newEdges_sens?newEdge2:newEdge1,
                edges1[edges1_sens?2:0],
                edge2);
    else if (0 == edge2)
        coface3 = new CoFace(getContext(),
                edges1[edges1_sens?2:0],
                newEdges_sens?newEdge2:newEdge1,
                edges3[edges3_sens?2:0]);
    else
        coface3 = new CoFace(getContext(),
                newEdges_sens?newEdge2:newEdge1,
                edges1[edges1_sens?2:0],
                edge2,
                edges3[edges3_sens?2:0]);

    if (icmd){
        icmd->addTopoInfoEntity(coface1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(coface2, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(coface3, Internal::InfoCommand::CREATED);
    }


    // On reprend la même projection
    if (getGeomAssociation()){
        coface1->setGeomAssociation(getGeomAssociation());
        coface2->setGeomAssociation(getGeomAssociation());
        coface3->setGeomAssociation(getGeomAssociation());
    }

    Utils::Container<Group::Group2D>& groups = getGroupsContainer();
    for (uint i=0; i<groups.size(); i++){
    	Group::Group2D* gr = groups.get(i);
    	gr->add(coface1);
    	gr->add(coface2);
    	gr->add(coface3);
    	coface1->getGroupsContainer().add(gr);
    	coface2->getGroupsContainer().add(gr);
    	coface3->getGroupsContainer().add(gr);
    }

    // recherche de la méthode de maillage qui semble la plus adaptée
    if (getMeshLaw() == CoFaceMeshingProperty::directional){
    	coface1->selectBasicMeshLaw(icmd);
    	coface2->selectBasicMeshLaw(icmd);
    	coface3->selectBasicMeshLaw(icmd);
    }

    // on ajoute les 3 CoFaces aux faces
    for (uint i=0; i<getNbFaces(); i++) {
        getFace(i)->saveFaceTopoProperty(icmd);
        getFace(i)->addCoFace(coface1);
        getFace(i)->addCoFace(coface2);
        getFace(i)->addCoFace(coface3);

        coface1->addFace(getFace(i));
        coface2->addFace(getFace(i));
        coface3->addFace(getFace(i));
    }

    // destruction de la face commune actuelle
    free(icmd);

    //        std::cout<<"coface1 en sortie du split: "<<*coface1;
    //        std::cout<<"coface2 en sortie du split: "<<*coface2;
    //        std::cout<<"coface3 en sortie du split: "<<*coface3;


    std::vector<Edge*> newEdges;
    newEdges.push_back(newEdge1);
    newEdges.push_back(newEdge2);

    // met à jour les interpolations se basant sur cette coface qui est découpée
    MAJInterpolated(coface1, newEdges);
    MAJInterpolated(coface2, newEdges);
    MAJInterpolated(coface3, newEdges);

    return newEdges;

}
/*----------------------------------------------------------------------------*/
void CoFace::
splitOgrid(eDirOnCoFace dir,
        std::vector<Edge*>& edges0,
        std::vector<Edge*>& edges1,
        std::vector<Edge*>& edges2,
        std::vector<Topo::Vertex*>& vtx0,
        std::vector<Topo::Vertex*>& vtx1,
        std::vector<Topo::Vertex*>& vtx2,
        double& ratio,
        Internal::InfoCommand* icmd)
{
	if (edges1.size() == 1)
		throw TkUtil::Exception (TkUtil::UTF8String ("Découpage de face impossible pour cas dégénéré",  TkUtil::Charset::UTF_8));

	if (edges0.size() != 2 || edges2.size() != 2)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Découpage de face impossible pour cas dégénéré, pb avec les arêtes sur l'axe",  TkUtil::Charset::UTF_8));

#ifdef _DEBUG_SPLIT
    std::cout<<"CoFace::splitOgrid("<<dir
            <<", ["<<edges0[0]->getName()<<","<<edges0[1]->getName()<<"]"
            <<", ["<<edges1[0]->getName()<<","<<edges1[1]->getName();
    if (edges1.size() == 3)
        std::cout<<","<<edges1[2]->getName();
    std::cout <<"], ["<<edges2[0]->getName()<<","<<edges2[1]->getName()<<"]";
    std::cout<<", [";
    for (uint i=0; i<vtx0.size(); i++)
        std::cout<<" "<<(vtx0[i]?vtx0[i]->getName():"undef");
    std::cout<<"], [";
    for (uint i=0; i<vtx1.size(); i++)
        std::cout<<" "<<(vtx1[i]?vtx1[i]->getName():"undef");
    std::cout<<"], [";
    for (uint i=0; i<vtx2.size(); i++)
        std::cout<<" "<<(vtx2[i]?vtx2[i]->getName():"undef");
    std::cout<<"])"<<std::endl;
    std::cout<<*this;
    std::cout<<"edges0 disc en "<<edges0[0]->getNbMeshingEdges()<<" et "<<edges0[1]->getNbMeshingEdges()<<std::endl;
    std::cout<<"edges2 disc en "<<edges2[0]->getNbMeshingEdges()<<" et "<<edges2[1]->getNbMeshingEdges()<<std::endl;
    std::cout<<"edges1 disc en "<<edges1[0]->getNbMeshingEdges()<<" et "<<edges1[1]->getNbMeshingEdges()<<std::endl;
#endif

    if (edges1.size() == 2){
        // cas de la création de 3 faces en Ogrid

        // création du sommet au centre de la face (sommet du o-grid)
        Utils::Math::Point pt = getVertex(0)->getCoord()
                + (vtx1[0]->getCoord() - getVertex(0)->getCoord()) * ratio;

        Topo::Vertex* new_vertex = new Topo::Vertex(getContext(), pt);
#ifdef _DEBUG_SPLIT
        std::cout<<" création du sommet "<<new_vertex->getName()<<std::endl;
#endif
        if (icmd)
            icmd->addTopoInfoEntity(new_vertex, Internal::InfoCommand::CREATED);

        // repositionne les sommets en fonction du nouveau sommet pour avoir de "beaux" angles droits
        if (Utils::Math::MgxNumeric::isNearlyZero(vtx0[0]->getY())){
            Utils::Math::Point pt0 = vtx0[0]->getCoord();
            pt0.setX(pt.getX());
            vtx0[0]->setCoord(pt0);

            Utils::Math::Point pt2 = vtx2[0]->getCoord();
            pt2.setY(pt.getY());
            vtx2[0]->setCoord(pt2);
       } else {
            Utils::Math::Point pt0 = vtx0[0]->getCoord();
            pt0.setY(pt.getY());
            vtx0[0]->setCoord(pt0);

            Utils::Math::Point pt2 = vtx2[0]->getCoord();
            pt2.setX(pt.getX());
            vtx2[0]->setCoord(pt2);
        }

        // on vérifie si c'est la première ou la deuxième Edge qui est en relation avec le sommet à la dégénérescence
        bool edges0_sens = edges0[0]->find(getVertex(0));
        bool edges2_sens = edges2[0]->find(getVertex(0));
        // on vérifie si c'est la première ou la deuxième Edge qui est en relation avec edges0
        bool edges1_sens = edges1[0]->find(getVertex(1));

        // on ajuste les discrétisations
        // on conserve celles sur le côté opposé à la dégénérescence (le 1)
        // on garde celle qui est succeptible d'être utilisée par une face voisine
        {
            Edge* edge0 = edges0[edges0_sens?0:1];
            Edge* edge1 = edges1[edges1_sens?1:0];

            uint nbMeshingEdges0 = edge0->getNbMeshingEdges();
            uint nbMeshingEdges1 = edge1->getNbMeshingEdges();

            if (edge0->getNbCoEdges() != 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid on a edges0 composée de plusieurs arêtes communes",  TkUtil::Charset::UTF_8));
            CoEdge* coedge0 = edge0->getCoEdge(0);
#ifdef _DEBUG_SPLIT
            std::cout<<"coedge0 : "<<coedge0->getName()<<std::endl;
            std::cout<<"nbMeshingEdges0 = "<<nbMeshingEdges0<<std::endl;
            std::cout<<"nbMeshingEdges1 = "<<nbMeshingEdges1<<std::endl;
#endif
            if (nbMeshingEdges0 != nbMeshingEdges1)
                TopoHelper::copyMeshingProperty(edge1 ,coedge0);
        }
        {
            Edge* edge2 = edges2[edges2_sens?0:1];
            Edge* edge1 = edges1[edges1_sens?0:1];

            uint nbMeshingEdges2 = edge2->getNbMeshingEdges();
            uint nbMeshingEdges1 = edge1->getNbMeshingEdges();

            if (edge2->getNbCoEdges() != 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid on a edges2 composée de plusieurs arêtes communes",  TkUtil::Charset::UTF_8));
            CoEdge* coedge2 = edge2->getCoEdge(0);
#ifdef _DEBUG_SPLIT
            std::cout<<"coedge2 : "<<coedge2->getName()<<std::endl;
            std::cout<<"nbMeshingEdges2 = "<<nbMeshingEdges2<<std::endl;
            std::cout<<"nbMeshingEdges1 = "<<nbMeshingEdges1<<std::endl;
#endif

            if (nbMeshingEdges2 != nbMeshingEdges1)
                TopoHelper::copyMeshingProperty(edge1 ,coedge2);
        }
        {
            Edge* edge0 = edges0[edges0_sens?1:0];
            Edge* edge2 = edges2[edges2_sens?1:0];

            if (edge2->getNbCoEdges() != 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid on a edges2 composée de plusieurs arêtes communes",  TkUtil::Charset::UTF_8));
            if (edge0->getNbCoEdges() != 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid on a edges0 composée de plusieurs arêtes communes",  TkUtil::Charset::UTF_8));

            CoEdge* coedge0 = edge0->getCoEdge(0);
            CoEdge* coedge2 = edge2->getCoEdge(0);
#ifdef _DEBUG_SPLIT
            std::cout<<"coedge0 : "<<coedge0->getName()<<std::endl;
            std::cout<<"coedge2 : "<<coedge2->getName()<<std::endl;
            std::cout<<"coedge2->getNbMeshingEdges() = "<<coedge2->getNbMeshingEdges()<<std::endl;
            std::cout<<"coedge0->getNbMeshingEdges() = "<<coedge0->getNbMeshingEdges()<<std::endl;
#endif

            if (coedge2->getNbMeshingEdges() != coedge0->getNbMeshingEdges()){
                if (coedge2->getNbEdges() == 1){
                    CoEdgeMeshingProperty* cmp = coedge2->setProperty(coedge0->getMeshingProperty()->clone());
                    delete cmp;
                }
                else {
                    CoEdgeMeshingProperty* cmp = coedge0->setProperty(coedge2->getMeshingProperty()->clone());
                    delete cmp;
                }
            }
        }

        // Création des 3 coedges
        CoEdge* coedge1 = new Topo::CoEdge(getContext(),
                edges0[edges0_sens?1:0]->getCoEdge(0)->getMeshingProperty(),
                new_vertex, vtx1[0]);
        if (edges0[edges0_sens?1:0]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges0[edges0_sens?1:0] ,coedge1);
        Topo::Edge* newEdge1 = new Topo::Edge(getContext(), coedge1);

        CoEdge* coedge0 = new Topo::CoEdge(getContext(),
                edges2[edges2_sens?0:1]->getCoEdge(0)->getMeshingProperty(),
                new_vertex, vtx0[0]);
        if (edges2[edges2_sens?0:1]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges2[edges2_sens?0:1] ,coedge0);
        Topo::Edge* newEdge0 = new Topo::Edge(getContext(), coedge0);

        CoEdge* coedge2 = new Topo::CoEdge(getContext(),
                edges0[edges0_sens?0:1]->getCoEdge(0)->getMeshingProperty(),
                new_vertex, vtx2[0]);
        if (edges0[edges0_sens?0:1]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges0[edges0_sens?0:1] ,coedge2);
        Topo::Edge* newEdge2 = new Topo::Edge(getContext(), coedge2);

        // on ne préserve pas le découpage polaire au centre de l'ogrid
        coedge0->getMeshingProperty()->setPolarCut(false);
        coedge1->getMeshingProperty()->setPolarCut(false);
        coedge2->getMeshingProperty()->setPolarCut(false);
#ifdef _DEBUG_SPLIT
            std::cout<<"création des 3 coedges: "<<std::endl;
            std::cout<<"coedge0 : "<<coedge0<<std::endl;
            std::cout<<"coedge1 : "<<coedge1<<std::endl;
            std::cout<<"coedge2 : "<<coedge2<<std::endl;
#endif

        if (icmd){
            icmd->addTopoInfoEntity(coedge0, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coedge1, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coedge2, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge0, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge1, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge2, Internal::InfoCommand::CREATED);
        }

        // création des 3 nouvelles faces communes
        Topo::CoFace* coface0; // celle contre le bord 0
        Topo::CoFace* coface1; // celle au centre de l'ogrid, opposée au bord 1
        Topo::CoFace* coface2; // celle contre le bord 2

        coface1 = new CoFace(getContext(),
                edges0[edges0_sens?0:1],
                edges2[edges2_sens?0:1],
                newEdge2,
                newEdge0);

        coface0 = new CoFace(getContext(),
                edges0[edges0_sens?1:0],
                newEdge0,
                newEdge1,
                edges1[edges1_sens?0:1]);

        coface2 = new CoFace(getContext(),
                edges2[edges2_sens?1:0],
                newEdge2,
                newEdge1,
                edges1[edges1_sens?1:0]);

        if (icmd){
            icmd->addTopoInfoEntity(coface0, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coface1, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coface2, Internal::InfoCommand::CREATED);
        }

        // On reprend la même projection
        if (getGeomAssociation()){
            coface0->setGeomAssociation(getGeomAssociation());
            coface1->setGeomAssociation(getGeomAssociation());
            coface2->setGeomAssociation(getGeomAssociation());
            coedge0->setGeomAssociation(getGeomAssociation());
            coedge1->setGeomAssociation(getGeomAssociation());
            coedge2->setGeomAssociation(getGeomAssociation());
            new_vertex->setGeomAssociation(getGeomAssociation());
        }

        Utils::Container<Group::Group2D>& groups = getGroupsContainer();
        for (uint i=0; i<groups.size(); i++){
        	Group::Group2D* gr = groups.get(i);
        	gr->add(coface0);
        	gr->add(coface1);
        	gr->add(coface2);
        	coface0->getGroupsContainer().add(gr);
        	coface1->getGroupsContainer().add(gr);
        	coface2->getGroupsContainer().add(gr);
        }

        // recherche de la méthode de maillage qui semble la plus adaptée
        coface0->selectBasicMeshLaw(icmd);
        coface1->selectBasicMeshLaw(icmd);
        coface2->selectBasicMeshLaw(icmd);


    } else if (edges1.size() == 3){
        // cas de la création de 4 faces en Ogrid

        // création des 2 sommets au centre de la face (sommet du o-grid)
        Utils::Math::Point pt1 = getVertex(0)->getCoord()
                + (vtx1[0]->getCoord() - getVertex(0)->getCoord()) * ratio;
        Utils::Math::Point pt2 = getVertex(0)->getCoord()
                + (vtx1[1]->getCoord() - getVertex(0)->getCoord()) * ratio;

        std::vector<Topo::Vertex*> new_vtx;
        new_vtx.push_back(new Topo::Vertex(getContext(), pt1));
        new_vtx.push_back(new Topo::Vertex(getContext(), pt2));

        if (icmd){
            icmd->addTopoInfoEntity(new_vtx[0], Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(new_vtx[1], Internal::InfoCommand::CREATED);
        }

        // on vérifie si c'est la première ou la deuxième Edge qui est en relation avec le sommet à la dégénérescence
        bool edges0_sens = edges0[0]->find(getVertex(0));
        bool edges2_sens = edges2[0]->find(getVertex(0));
        // le sommet opposé au centre sur edges0
        Topo::Vertex* vtx_edges0_opp = edges0[edges0_sens?1:0]->getOppositeVertex(edges0[edges0_sens?0:1]->getOppositeVertex(getVertex(0)));
        // on vérifie si c'est la première (ou la troisième) Edge qui est en relation avec edges0
        bool edges1_sens = edges1[0]->find(vtx_edges0_opp);
        // vrai si vtx1[0] relié à l'arête elle même reliée au premier sommet
        bool vtx1_sens = edges1[edges1_sens?0:2]->find(vtx1[0]);

#ifdef _DEBUG_SPLIT
        std::cout<<"  edges0_sens = "<<(edges0_sens?"vrai":"faux")<<std::endl;
        std::cout<<"  edges2_sens = "<<(edges2_sens?"vrai":"faux")<<std::endl;
        std::cout<<"  edges1_sens = "<<(edges1_sens?"vrai":"faux")<<std::endl;
        std::cout<<"  vtx1_sens = "<<(vtx1_sens?"vrai":"faux")<<std::endl;

        // prérequis pour la suite
        if (!edges0[0]->find(vtx0[0]))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid vtx0 n'est pas dans edges0",  TkUtil::Charset::UTF_8));
        if (!edges2[0]->find(vtx2[0]))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid vtx2 n'est pas dans edges2",  TkUtil::Charset::UTF_8));
#endif

        // repositionne les sommets en fonction du nouveau sommet pour avoir de "beaux" angles droits
        {
            Utils::Math::Point pt0(new_vtx[vtx1_sens?0:1]->getX(),0,0);
            vtx0[0]->setCoord(pt0);

            Utils::Math::Point pt2(new_vtx[vtx1_sens?1:0]->getX(),0,0);
            vtx2[0]->setCoord(pt2);
       }

        // on ajuste les discrétisations
        // on conserve celles sur le côté opposé à la dégénérescence (le 1)
        {
            Edge* edge0 = edges0[edges0_sens?0:1];
            Edge* edge1 = edges1[1];
            Edge* edge2 = edges2[edges2_sens?0:1];

            uint nbMeshingEdges0 = edge0->getNbMeshingEdges();
            uint nbMeshingEdges2 = edge2->getNbMeshingEdges();
            uint nbMeshingEdges1 = edge1->getNbMeshingEdges();
            if (nbMeshingEdges1 != nbMeshingEdges0 + nbMeshingEdges2){
                if (edge0->getNbCoEdges() != 1 || edge2->getNbCoEdges() != 1)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::splitOgrid on a edges0 ou edges2 composée de plusieurs arêtes communes",  TkUtil::Charset::UTF_8));


                uint n1 = nbMeshingEdges1/2;
                uint n2 = nbMeshingEdges1 - n1;
                Topo::CoEdgeMeshingProperty* cmp1 = new Topo::EdgeMeshingPropertyUniform(n1);
                Topo::CoEdgeMeshingProperty* cmp2 = new Topo::EdgeMeshingPropertyUniform(n2);
                cmp1 = edge0->getCoEdge(0)->setProperty(cmp1);
                cmp2 = edge2->getCoEdge(0)->setProperty(cmp2);
                delete cmp1;
                delete cmp2;
            }
        }


        // Création des 5 coedges
        CoEdge* coedge1 = new Topo::CoEdge(getContext(),
                edges0[edges0_sens?1:0]->getCoEdge(0)->getMeshingProperty(),
                new_vtx[vtx1_sens?0:1], vtx1[vtx1_sens?0:1]);
        if (edges0[edges0_sens?1:0]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges0[edges0_sens?1:0] ,coedge1);
        Topo::Edge* newEdge1 = new Topo::Edge(getContext(), coedge1);

        CoEdge* coedge2 = new Topo::CoEdge(getContext(),
                edges0[edges0_sens?1:0]->getCoEdge(0)->getMeshingProperty(),
                new_vtx[vtx1_sens?1:0], vtx1[vtx1_sens?1:0]);
        if (edges0[edges0_sens?1:0]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges0[edges0_sens?1:0] ,coedge2);
        Topo::Edge* newEdge2 = new Topo::Edge(getContext(), coedge2);

        CoEdge* coedge3 = new Topo::CoEdge(getContext(),
                edges1[0]->getCoEdge(0)->getMeshingProperty(),
                new_vtx[vtx1_sens?0:1], vtx0[0]);
        if (edges1[0]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges1[0] ,coedge3);
        Topo::Edge* newEdge3 = new Topo::Edge(getContext(), coedge3);

        CoEdge* coedge4 = new Topo::CoEdge(getContext(),
                edges1[1]->getCoEdge(0)->getMeshingProperty(),
                new_vtx[0], new_vtx[1]);
        if (edges1[1]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges1[1] ,coedge4);
        Topo::Edge* newEdge4 = new Topo::Edge(getContext(), coedge4);

        CoEdge* coedge5 = new Topo::CoEdge(getContext(),
                edges1[0]->getCoEdge(0)->getMeshingProperty(),
                new_vtx[vtx1_sens?1:0], vtx2[0]);
        if (edges1[0]->getNbCoEdges() != 1)
            TopoHelper::copyMeshingProperty(edges1[0] ,coedge5);
        Topo::Edge* newEdge5 = new Topo::Edge(getContext(), coedge5);

        // l'arête de part et d'autre du sommet dégénéré
        std::vector<CoEdge* > coedges6;
        coedges6.push_back(edges0[edges0_sens?0:1]->getCoEdge(0));
        coedges6.push_back(edges2[edges2_sens?0:1]->getCoEdge(0));
        Topo::Edge* newEdge6 = new Topo::Edge(getContext(), vtx0[0], vtx2[0], coedges6);
        // destruction des 2 Edges inutilisées
        edges0[edges0_sens?0:1]->free(icmd);
        edges2[edges2_sens?0:1]->free(icmd);

        // on ne préserve pas le découpage polaire au centre de l'ogrid
        coedge1->getMeshingProperty()->setPolarCut(false);
        coedge2->getMeshingProperty()->setPolarCut(false);
        coedge3->getMeshingProperty()->setPolarCut(false);
        coedge4->getMeshingProperty()->setPolarCut(false);
        coedge5->getMeshingProperty()->setPolarCut(false);
#ifdef _DEBUG_SPLIT
            std::cout<<"création des 5 coedges: "<<std::endl;
            std::cout<<"coedge1 : "<<coedge1<<std::endl;
            std::cout<<"coedge2 : "<<coedge2<<std::endl;
            std::cout<<"coedge3 : "<<coedge3<<std::endl;
            std::cout<<"coedge4 : "<<coedge4<<std::endl;
            std::cout<<"coedge5 : "<<coedge5<<std::endl;
#endif

        if (icmd){
            icmd->addTopoInfoEntity(coedge1, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coedge2, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coedge3, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coedge4, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coedge5, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge1, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge2, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge3, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge4, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge5, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(newEdge6, Internal::InfoCommand::CREATED);
        }

        // création des 4 nouvelles faces communes
        Topo::CoFace* coface0; // celle au centre
        Topo::CoFace* coface1;
        Topo::CoFace* coface2;
        Topo::CoFace* coface3;

        coface1 = new CoFace(getContext(),
                edges0[edges0_sens?1:0],
                edges1[edges1_sens?0:2],
                newEdge1,
                newEdge3);

        coface2 = new CoFace(getContext(),
                edges1[1],
                newEdge1,
                newEdge4,
                newEdge2);

        coface3 = new CoFace(getContext(),
                edges2[edges2_sens?1:0],
                edges1[edges1_sens?2:0],
                newEdge2,
                newEdge5);

        coface0 = new CoFace(getContext(),
                newEdge6,
                newEdge5,
                newEdge4,
                newEdge3);


        if (icmd){
            icmd->addTopoInfoEntity(coface0, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coface1, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coface2, Internal::InfoCommand::CREATED);
            icmd->addTopoInfoEntity(coface3, Internal::InfoCommand::CREATED);
        }

        // On reprend la même projection
        if (getGeomAssociation()){
            coface0->setGeomAssociation(getGeomAssociation());
            coface1->setGeomAssociation(getGeomAssociation());
            coface2->setGeomAssociation(getGeomAssociation());
            coface3->setGeomAssociation(getGeomAssociation());
            coedge1->setGeomAssociation(getGeomAssociation());
            coedge2->setGeomAssociation(getGeomAssociation());
            coedge3->setGeomAssociation(getGeomAssociation());
            coedge4->setGeomAssociation(getGeomAssociation());
            coedge5->setGeomAssociation(getGeomAssociation());
            new_vtx[0]->setGeomAssociation(getGeomAssociation());
            new_vtx[1]->setGeomAssociation(getGeomAssociation());
        }

        Utils::Container<Group::Group2D>& groups = getGroupsContainer();
        for (uint i=0; i<groups.size(); i++){
        	Group::Group2D* gr = groups.get(i);
        	gr->add(coface0);
        	gr->add(coface1);
        	gr->add(coface2);
        	gr->add(coface3);
        	coface0->getGroupsContainer().add(gr);
        	coface1->getGroupsContainer().add(gr);
        	coface2->getGroupsContainer().add(gr);
        	coface3->getGroupsContainer().add(gr);
        }

        // recherche de la méthode de maillage qui semble la plus adaptée
        coface0->selectBasicMeshLaw(icmd);
        coface1->selectBasicMeshLaw(icmd);
        coface2->selectBasicMeshLaw(icmd);
        coface3->selectBasicMeshLaw(icmd);


    } // end else if (edges1.size() == 3)

    // destruction de la face commune actuelle
    free(icmd);
}
/*----------------------------------------------------------------------------*/
uint CoFace::getNbVertices() const
{
#ifdef _DEBUG
    if (isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "CoFace::getNbVertices() pour face "<<getName()<<" DETRUITE !!!";
        log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
    }
#endif
    return m_topo_property->getVertexContainer().getNb();
}
/*----------------------------------------------------------------------------*/
Edge* CoFace::
getEdge(Topo::Vertex* v1, Topo::Vertex* v2) const
{
    for(unsigned int i=0; i <getNbEdges();i++){
        if (getEdge(i)->getNbVertices() == 2){
            Topo::Vertex* s1 = getEdge(i)->getVertex(0);
            Topo::Vertex* s2 = getEdge(i)->getVertex(1);
            if ((s1 == v1 && s2 == v2) || (s1 == v2 && s2 == v1))
                return getEdge(i);
        }
    }

    // on a échoué
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "CoFace::getEdge("
            <<v1->getName()<<","
            <<v2->getName()<<")\n";
    message1 << "this :"<<*this;
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "CoFace::getEdge("<<v1->getName()<<", "<<v2->getName()<<") ne trouve pas l'arête pour ces 2 sommets dans "<<getName();
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
Edge* CoFace::
getEdgeContaining(const CoEdge* coedge) const
{
    for(unsigned int i=0; i <getNbEdges();i++)
        if (getEdge(i)->find(coedge))
            return getEdge(i);

    throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getEdgeContaining(coedge) ne trouve l'arête contenant cette arête commune",  TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Edge* CoFace::
getOppositeEdge(Edge* e) const
{
    if (!isStructured())
        return 0;

    if (getNbEdges() == 3){
    	if (e == getEdge(0))
    		return getEdge(2);
    	else if (e == getEdge(2))
    		return getEdge(1);
    	else
    		return 0;
    }
    else
    	for (uint i=0; i<4; i++) {
    		Edge* e1 = getEdge(i);
    		if (e1 == e)
    			return getEdge((i+2)%4);
    	}

    // on a échoué
    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFace::getOppositeEdge ne trouve pas l'arête",  TkUtil::Charset::UTF_8));
    return 0;
}
/*----------------------------------------------------------------------------*/
CoEdge* CoFace::
getOppositeCoEdge(CoEdge* coedge_dep, bool& inverse_sens) const
{
#ifdef _DEBUG2
	std::cout<<"CoFace::getOppositeCoEdge("<<coedge_dep->getName()<<") en cours pour "<<getName()<<std::endl;
#endif
	CoEdge* coedge_opp = 0;
	inverse_sens = true;

	if (!isStructured())
		return coedge_opp;

	uint cote_dep = getIndex(coedge_dep);

	// on sort si on fait face au côté dégénéré
	if (getNbEdges()==3 && cote_dep == 1)
		return coedge_opp;

	uint cote_opp = (cote_dep+2)%4;

	// récupération des sommets de la face avec le dernier sommet dupliqué pour le cas dégénéré
	std::vector<Vertex*> vertices;
	getVertices(vertices);
	if (vertices.size() == 3)
		vertices.push_back(vertices[0]);

	Edge* edge_dep = getEdge(cote_dep);
	Edge* edge_opp = getEdge(cote_opp);

	// recherche des arêtes triées côté départ
	Vertex* vtx1_dep = vertices[cote_dep];
	Vertex* vtx2_dep = vertices[(cote_dep+1)%4];
	//std::cout<<"vtx1_dep = "<<vtx1_dep->getName()<<std::endl;
	//std::cout<<"vtx2_dep = "<<vtx2_dep->getName()<<std::endl;
	std::vector<CoEdge*> coedges_dep;
	edge_dep->getCoEdges(coedges_dep);
	std::vector<CoEdge*> coedges_dep_sort;
	TopoHelper::getCoEdgesBetweenVertices(vtx1_dep, vtx2_dep, coedges_dep, coedges_dep_sort);

	// idem côté opposé
	Vertex* vtx1_opp = vertices[(cote_dep+3)%4];
	Vertex* vtx2_opp = vertices[(cote_dep+2)%4];
	//std::cout<<"vtx1_opp = "<<vtx1_opp->getName()<<std::endl;
	//std::cout<<"vtx2_opp = "<<vtx2_opp->getName()<<std::endl;
	std::vector<CoEdge*> coedges_opp;
	edge_opp->getCoEdges(coedges_opp);
	std::vector<CoEdge*> coedges_opp_sort;
	TopoHelper::getCoEdgesBetweenVertices(vtx1_opp, vtx2_opp, coedges_opp, coedges_opp_sort);

	// Recherche les indices des (indices des) noeuds extrémités de l'arête coedge_dep dans edge_dep
	uint ind_nd1_dep = 0;
	uint ind_nd2_dep = 0;
	uint i=0;
	Vertex* vtxI = vtx1_dep;
	for (i=0; i<coedges_dep_sort.size() && coedge_dep!=coedges_dep_sort[i]; i++){
		ind_nd1_dep += coedges_dep_sort[i]->getNbMeshingEdges();
		vtxI = coedges_dep_sort[i]->getOppositeVertex(vtxI);
	}
	if (coedge_dep!=coedges_dep_sort[i]){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Erreur interne dans CoFace::getOppositeCoEdge, on ne retrouve pas l'arête commune "
				<< coedge_dep->getName() << " dans l'arête "<<edge_dep->getName();
		throw TkUtil::Exception (message);
	}
	ind_nd2_dep = ind_nd1_dep + coedges_dep_sort[i]->getNbMeshingEdges();
	bool sens_dep = coedges_dep_sort[i]->getVertex(0) == vtxI;
	//std::cout<<"ind_nd1_dep = "<<ind_nd1_dep<<std::endl;
	//std::cout<<"ind_nd2_dep = "<<ind_nd2_dep<<std::endl;

	// Recherche de l'arête opposée
	uint ind_nd1_opp = 0;
	uint ind_nd2_opp = 0;
	vtxI = vtx1_opp;
	for (i=0; i<coedges_opp_sort.size(); i++){
		ind_nd2_opp = ind_nd1_opp+coedges_opp_sort[i]->getNbMeshingEdges();
		//std::cout<<"coedges_opp_sort["<<i<<"] = "<<coedges_opp_sort[i]->getName()<<std::endl;
		//std::cout<<"ind_nd1_opp = "<<ind_nd1_opp<<std::endl;
		//std::cout<<"ind_nd2_opp = "<<ind_nd2_opp<<std::endl;
		// cas où les arêtes correspondent
		if (ind_nd1_dep == ind_nd1_opp && ind_nd2_dep == ind_nd2_opp){
			coedge_opp = coedges_opp_sort[i];

			bool sens_opp = coedge_opp->getVertex(0) == vtxI;
			inverse_sens = (sens_dep == sens_opp);
		}

		// pour la prochaine arête
		ind_nd1_opp = ind_nd2_opp;
		vtxI = coedges_opp_sort[i]->getOppositeVertex(vtxI);
	}

#ifdef _DEBUG2
	std::cout<<"CoFace::getOppositeCoEdge("<<coedge_dep->getName()<<") => "
			<<(coedge_opp?coedge_opp->getName():"0")
			<<" inverse_sens = "<<(inverse_sens?"vrai":"faux")<<std::endl;
#endif

	return coedge_opp;
}
/*----------------------------------------------------------------------------*/
void CoFace::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_TOPO)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity",  TkUtil::Charset::UTF_8));

    if (isDestroyed() && (true == checkDestroyed)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Interdit d'afficher une entité détruite ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }

    TopoDisplayRepresentation* tdr =
            dynamic_cast<TopoDisplayRepresentation*>(&dr);
    CHECK_NULL_PTR_ERROR(tdr);
	double shrink = tdr->getShrink();

    std::vector<Utils::Math::Point>& points = tdr->getPoints();
    std::vector<size_t>& indicesFilaire = tdr->getCurveDiscretization();
    std::vector<size_t>& indicesSurf = tdr->getSurfaceDiscretization();

    points.clear();
    indicesFilaire.clear();
    indicesSurf.clear();

    // recherche du barycentre
    Utils::Math::Point barycentre = getBarycentre();

    // si la face à un unique bloc voisin, on tient compte du barycentre de ce dernier pour écarter la face du bord du volume
    // dans le but de distinguer les faces non fusionnées entre 2 volumes collés
    std::vector<Topo::Block* > blocks;
    getBlocks(blocks);
    if (blocks.size() == 1){
        Utils::Math::Point barycentre_bl = blocks[0]->getBarycentre();
        barycentre = barycentre*0.7 + barycentre_bl*0.3;
    }

    // barycentre de la représentation
    Utils::Math::Point repr_barycentre;

    // cas avec des trous
    if (m_topo_property->hasHole()){

        if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE)){
            indicesFilaire.clear();
            points.clear();
            // plutot que de rechercher les cycles, on duplique les sommets ...

            std::vector<Topo::Edge* > edges;
            getEdges(edges);

            for (std::vector<Topo::Edge* >::iterator iter = edges.begin();
                        iter != edges.end(); ++iter) {
                Topo::Edge* edge = *iter;
                if (edge->getNbVertices() == 2){
                    for (uint i=0; i<2; i++){
                        indicesFilaire.push_back(points.size());
                        points.push_back(barycentre + (edge->getVertex(i)->getCoord() -  barycentre) * shrink);
                        repr_barycentre += points.back();
                    }
                }
                else if (edge->getNbVertices() == 1){
                    points.push_back(barycentre + (edge->getVertex(0)->getCoord() -  barycentre) * shrink);
                    repr_barycentre += points.back();
                }
            } // end for iter = edges.begin()

            repr_barycentre /=(double)points.size();

        }
        else if (tdr->hasRepresentation(Utils::DisplayRepresentation::SOLID)) {
            indicesSurf.clear();
            points.clear();
            // TODO [EB] : Faire Edge::getRepresentation pour le cas avec trous (passer par un maillage minimaliste)
            MGX_NOT_YET_IMPLEMENTED("Faire Edge::getRepresentation avec des trous en mode solide")
        }

    }
    else {
        // cas sans trou:

        // modif des coordonnées des sommets de la représentation en fonction du shrink
        points.clear();
        for (uint i=0; i<getNbVertices(); i++){
            points.push_back(barycentre + (getVertex(i)->getCoord() - barycentre) * shrink);
            repr_barycentre += points.back();
        }
        if (points.size())
        	repr_barycentre /=(double)points.size();

        if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE)){
            indicesFilaire.clear();

            for (uint i=0; i<getNbVertices(); i++){
                indicesFilaire.push_back(i);
                indicesFilaire.push_back((i+1)%getNbVertices());
            }
        }
        else if (tdr->hasRepresentation(Utils::DisplayRepresentation::SOLID)) {
            if (getNbVertices() == 4){
                // représentation à l'aide de 2 triangles
                indicesSurf.push_back(0);
                indicesSurf.push_back(1);
                indicesSurf.push_back(2);

                indicesSurf.push_back(2);
                indicesSurf.push_back(3);
                indicesSurf.push_back(0);
            }
            else if (getNbVertices() == 3){
                // représentation à l'aide d'1 triangle
                indicesSurf.push_back(0);
                indicesSurf.push_back(1);
                indicesSurf.push_back(2);
            }
            else if (getNbVertices() < 3){
                // pas de représentation solide prévue
            }
            else {
                // TODO [EB] : Faire Edge::getRepresentation pour le cas non structuré (passer par un maillage minimaliste)
                MGX_NOT_YET_IMPLEMENTED("Faire Edge::getRepresentation pour le cas non structuré à plus de 4 côtés en mode solide")
            }
        }
    } // end else / if (m_topo_property->hasHole())

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWASSOCIATION) && getGeomAssociation()){
        // on ajoute une flêche entre le barycentre de la face et la géométrie
        //std::cout<<"  SHOWASSOCIATION pour "<<getName()<<" -> vers "<<getGeomAssociation()->getName()<<std::endl;
        //std::cout<<"    barycentre : "<<repr_barycentre<<std::endl;
        Utils::Math::Point pt2;
        getGeomAssociation()->project(repr_barycentre, pt2);
        //std::cout<<"    pt2 : "<<pt2<<std::endl;
        std::vector<Utils::Math::Point>& vecteur = tdr->getVector();
        vecteur.push_back(repr_barycentre);
        vecteur.push_back(pt2);
    }

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWDISCRETISATIONTYPE)){

    	std::vector<size_t>& indices = tdr->getCurveDiscretization();

    	// un point de départ pour la représentation
    	Utils::Math::Point orig;
    	if (points.size()==1)
    		orig = points[0];
    	else if (points.size()>1)
    		orig = points[1];

//    	std::cout<<"points de taille "<<points.size()<<":";
//    	for (uint i=0; i<points.size(); i++)
//    		std::cout<<" "<<points[i];
//    	std::cout<<std::endl;

    	// deux vecteurs pour orienter et placer dans un plan de l'espace la représentation
    	Utils::Math::Vector vect1;
    	Utils::Math::Vector vect2;
    	// un ratio pour normaliser la représentation (basé sur la plus petite longueur suivant les 2 vecteurs)
    	double ratio = 1.0;

    	// on se débarrasse des 3 ou 4 sommets extrémités (qui ont servi pour le barycentre ...)
    	points.clear();
    	indicesFilaire.clear();

    	// on va utiliser les points des discrétisations des 2 premières arêtes de la coface
    	// mais il peut n'y avoir qu'une arête dans la cas non structuré !

    	if (isStructured()){

    		uint lastId = (getNbVertices() == 3?0:3);
    		Edge* edge1=0;
    		Edge* edge2=0;
    		if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::directional){
    			FaceMeshingPropertyDirectional* mp = dynamic_cast<FaceMeshingPropertyDirectional*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			if (mp->getDir() == 1){
    				edge1 = getEdge(0);
    				edge2 = getEdge(1);
    			}
    			else {
    				edge1 = getEdge(1);
    				edge2 = getEdge(0);
    			}
    		}
    		else if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::orthogonal){
    			FaceMeshingPropertyOrthogonal* mp = dynamic_cast<FaceMeshingPropertyOrthogonal*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			if (mp->getDir() == 1){
    				edge1 = getEdge(0);
    				if (mp->getSide() == 1){
    					// cas où il faut prendre les arêtes de l'autre côté
    					edge2 = getEdge(getVertex(0), getVertex(lastId));
    				}
    				else
    					edge2 = getEdge(1);
    			}
    			else {
    				edge1 = getEdge(1);
    				if (mp->getSide() == 1){
    					// cas où il faut prendre les arêtes de l'autre côté
    					edge2 = getEdge(getVertex(2), getVertex(lastId));
    				}
    				else
    					edge2 = getEdge(0);
    			}
    		}
    		else if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::rotational){
    			FaceMeshingPropertyRotational* mp = dynamic_cast<FaceMeshingPropertyRotational*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			if (mp->getDir() == 1){
    				edge1 = getEdge(0);
    				edge2 = getEdge(1);
    			}
    			else {
    				edge1 = getEdge(1);
    				edge2 = getEdge(0);
    			}
    		}
    		else {
    			edge1 = getEdge(1);
    			edge2 = getEdge(0);
    		}

//    		std::cout<<"CoFace::getRepresentation pour "<<getName()<<" basé sur "
//    				<<edge1->getName()<<" - "<<edge2->getName()<<std::endl;


    		std::vector<Utils::Math::Point> points_edge1;
    		std::vector<Utils::Math::Point> points_edge2;
    		edge1->getPoints(points_edge1);
    		edge2->getPoints(points_edge2);
    		if (edge1->getVertex(0) == getVertex(1)
    				|| edge1->getVertex(1) == getVertex(lastId))
    			vect1 = Utils::Math::Vector(points_edge1[0], points_edge1[1]);
    		else if (edge1->getVertex(1) == getVertex(1)
    				|| edge1->getVertex(0) == getVertex(lastId))
    			vect1 = Utils::Math::Vector(points_edge1[points_edge1.size()-1],
    					points_edge1[points_edge1.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<edge1->getName();
    			throw TkUtil::Exception(messErr);
    		}

    		if (edge2->getVertex(0) == getVertex(1)
    				|| edge2->getVertex(1) == getVertex(lastId))
    			vect2 = Utils::Math::Vector(points_edge2[0], points_edge2[1]);
    		else if (edge2->getVertex(1) == getVertex(1)
    				|| edge2->getVertex(0) == getVertex(lastId))
    			vect2 = Utils::Math::Vector(points_edge2[points_edge2.size()-1],
    					points_edge2[points_edge2.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<edge2->getName();
    			throw TkUtil::Exception(messErr);
    		}

    		if (edge1->getVertex(1) == getVertex(lastId) || edge1->getVertex(0) == getVertex(lastId)){
    			vect2 *= -1;
    			if (edge1->getVertex(1) == getVertex(0) || edge1->getVertex(0) == getVertex(0))
    				orig = points[0];
    			else
    				orig = points[2];
    		}
    		else if (edge2->getVertex(1) == getVertex(lastId) || edge2->getVertex(0) == getVertex(lastId)){
    			vect1 *= -1;
    			if (edge2->getVertex(1) == getVertex(0) || edge2->getVertex(0) == getVertex(0))
    				orig = points[0];
    			else
    				orig = points[2];
    		}


    		double lg1 = 0;
    		double lg2 = 0;
    		for (uint i=0; i<points_edge1.size()-1; i++)
    			lg1 += (points_edge1[i+1]-points_edge1[i]).norme();
    		for (uint i=0; i<points_edge2.size()-1; i++)
    			lg2 += (points_edge2[i+1]-points_edge2[i]).norme();

    		if (lg1<lg2)
    			ratio = lg1;
    		else
    			ratio = lg2;
    		ratio /= 7.0;


    		// TODO [EB] rendre les vecteurs orthogonaux ?

    		// normalisation des 2 vecteurs
    		vect1 /= vect1.norme();
    		vect2 /= vect2.norme();
    		// pour la courbure
    		Utils::Math::Vector vect3 = vect1*vect2;
//    		std::cout<<" ratio : "<<ratio<<std::endl;
//    		std::cout<<" vect1 : "<<vect1<<std::endl;
//    		std::cout<<" vect2 : "<<vect2<<std::endl;

    		// décalage pour faire apparaitre la courbure, si nécessaire
    		double dec2 = 0.0;
    		double dec3 = 0.0;
    		if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::rotational){
    			FaceMeshingPropertyRotational* mp = dynamic_cast<FaceMeshingPropertyRotational*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			Utils::Math::Point axis1, axis2;
    			mp->getAxis(axis1, axis2);

    			// on cherche à minimiser la distance entre l'axe et l'un des points
    			// avec l'une des options pour la direction
    			Utils::Math::Point option1 = (vect1*0.5+vect2*(0.5+0.1)+vect3*(0.5))*ratio+orig;
    			Utils::Math::Point option2 = (vect1*0.5+vect2*(0.5-0.1)+vect3*(0.5))*ratio+orig;
    			Utils::Math::Point option3 = (vect1*0.5+vect2*(0.5)+vect3*(0.5+0.1))*ratio+orig;
    			Utils::Math::Point option4 = (vect1*0.5+vect2*(0.5)+vect3*(0.5-0.1))*ratio+orig;

    			uint best_option = 1;
    			double dist = (axis1-option1).norme2();

    			if ((axis1-option2).norme2()<dist){
    				dist = (axis1-option2).norme2();
    				best_option = 2;
    			}
    			if ((axis1-option3).norme2()<dist){
    				dist = (axis1-option3).norme2();
    				best_option = 3;
    			}
    			if ((axis1-option4).norme2()<dist){
    				dist = (axis1-option4).norme2();
    				best_option = 4;
    			}

    			if (best_option == 1)
    				dec2 = +0.1;
    			else if (best_option == 2)
    				dec2 = -0.1;
    			else if (best_option == 3)
    				dec3 = +0.1;
    			else if (best_option == 4)
    				dec3 = -0.1;
    		}

    		// on dessine dans le repère vect1 vect2
    		points.push_back(vect1*0.2+vect2*(0.4+dec2)+vect3*dec3);
    		points.push_back(vect1*0.4+vect2*0.4);
    		points.push_back(vect1*0.8+vect2*0.4);
    		points.push_back(vect1*1.0+vect2*(0.4+dec2)+vect3*dec3);
    		points.push_back(vect1*0.4+vect2*0.2);
    		points.push_back(vect1*0.8+vect2*0.2);
    		points.push_back(vect1*0.4+vect2*1.0);
    		points.push_back(vect1*0.8+vect2*1.0);
    		points.push_back(vect1*0.2+vect2*(0.8+dec2)+vect3*dec3);
    		points.push_back(vect1*0.4+vect2*0.8);
    		points.push_back(vect1*0.8+vect2*0.8);
    		points.push_back(vect1*1.0+vect2*(0.8+dec2)+vect3*dec3);
    		indices.push_back(0); indices.push_back(1);
    		indices.push_back(1); indices.push_back(2);
    		indices.push_back(2); indices.push_back(3);
    		indices.push_back(1); indices.push_back(4);
    		indices.push_back(2); indices.push_back(5);
    		indices.push_back(8); indices.push_back(9);
    		indices.push_back(9); indices.push_back(10);
    		indices.push_back(10); indices.push_back(11);
    		indices.push_back(1); indices.push_back(9);
    		indices.push_back(2); indices.push_back(10);
    		indices.push_back(6); indices.push_back(9);
    		indices.push_back(7); indices.push_back(10);

    		if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::directional
    				|| m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::orthogonal){
				points.push_back(vect1*1.1+vect2*0.6);
   				points.push_back(vect1*1.1+vect2*0.2);
   				points.push_back(vect1*1.3+vect2*0.4);
    		} else if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::rotational){
    			if (dec2>0.0){
    				points.push_back(vect1*1.2+vect2*0.5);
       				points.push_back(vect1*1.0+vect2*0.7);
       				points.push_back(vect1*1.2+vect2*0.7);
    			} else if (dec2<0.0) {
       				points.push_back(vect1*1.0+vect2*0.1);
       				points.push_back(vect1*1.2+vect2*0.3);
    				points.push_back(vect1*1.2+vect2*0.1);
    			} else {
    				points.push_back(vect1*1.1+vect2*0.6+vect3*dec3*2);
       				points.push_back(vect1*1.1+vect2*0.2+vect3*dec3*2);
       				points.push_back(vect1*1.3+vect2*0.4+vect3*dec3*4);
    			}
    		}
    		if (points.size() > 12){
    			indices.push_back(3); indices.push_back(14);
    			indices.push_back(13); indices.push_back(14);
    			indices.push_back(12); indices.push_back(14);
    		}
    		if (m_mesh_property->getMeshLaw() == CoFaceMeshingProperty::orthogonal){
    			uint id = points.size();
    			points.push_back(vect1*0.5+vect2*0.4);
    			points.push_back(vect1*0.5+vect2*0.5);
    			points.push_back(vect1*0.4+vect2*0.5);
    			indices.push_back(id); indices.push_back(id+1);
    			indices.push_back(id+2); indices.push_back(id+1);
    		}
    	} // end if (isStructured())
    	else {

    		// recherche de 2 vecteurs
    		std::vector<CoEdge* > coedges;
    		getCoEdges(coedges, false);
    		if (coedges.size() == 1){
    			CoEdge* coedge1 = coedges[0];
    			std::vector<Utils::Math::Point> points_coedge1;
    			coedge1->getPoints(points_coedge1);
    			vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			vect2 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    			    						points_coedge1[points_coedge1.size()-2]);

    			double lg1 = 0;
    			for (uint i=0; i<points_coedge1.size()-1; i++)
    				lg1 += (points_coedge1[i+1]-points_coedge1[i]).norme();
    			ratio =  lg1/7.0;

    		} else if (coedges.size() > 1){
    			CoEdge* coedge1 = coedges[0];
    			CoEdge* coedge2 = coedges[1];
    			std::vector<Utils::Math::Point> points_coedge1;
    			std::vector<Utils::Math::Point> points_coedge2;
    			coedge1->getPoints(points_coedge1);
    			coedge2->getPoints(points_coedge2);
    			if (coedge1->getVertex(0) == getVertex(1))
    				vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			else if (coedge1->getVertex(1) == getVertex(1))
    				vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    						points_coedge1[points_coedge1.size()-2]);
    			else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Erreur interne pour l'affichage de la discrétisation ("
    						<<getName()<<"), pb avec arête "<<coedge1->getName();
    				throw TkUtil::Exception(messErr);
    			}

        		if (coedge2->getVertex(0) == getVertex(1))
        			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
        		else if (coedge2->getVertex(1) == getVertex(1))
        			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
        					points_coedge2[points_coedge2.size()-2]);
        		else {
        			TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
        			messErr << "Erreur interne pour l'affichage de la discrétisation ("
        					<<getName()<<"), pb avec arête "<<coedge2->getName();
        			throw TkUtil::Exception(messErr);
        		}

        		double lg1 = 0;
        		double lg2 = 0;
        		for (uint i=0; i<points_coedge1.size()-1; i++)
        			lg1 += (points_coedge1[i+1]-points_coedge1[i]).norme();
        		for (uint i=0; i<points_coedge2.size()-1; i++)
        			lg2 += (points_coedge2[i+1]-points_coedge2[i]).norme();

        		if (lg1<lg2)
        			ratio = lg1;
        		else
        			ratio = lg2;
        		ratio /= 7.0;

    		} else {
    			TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pas assez d'arêtes";
    			throw TkUtil::Exception(messErr);
    		}

    		// normalisation des 2 vecteurs
    		vect1 /= vect1.norme();
    		vect2 /= vect2.norme();


    		points.push_back(vect1*0.3+vect2*0.3);
    		points.push_back(vect1*1.0+vect2*0.4);
    		points.push_back(vect1*0.5+vect2*1.0);

    		points.push_back(vect1*0.1+vect2*0.4);
    		points.push_back(vect1*0.2+vect2*0.1);
    		points.push_back(vect1*0.4+vect2*0.1);

    		points.push_back(vect1*0.9+vect2*0.2);
    		points.push_back(vect1*1.2+vect2*0.3);
    		points.push_back(vect1*1.2+vect2*0.6);

    		points.push_back(vect1*0.3+vect2*1.0);
    		points.push_back(vect1*0.5+vect2*1.2);
    		points.push_back(vect1*0.7+vect2*1.1);

			indices.push_back(0); indices.push_back(1);
			indices.push_back(2); indices.push_back(1);
			indices.push_back(0); indices.push_back(2);

			indices.push_back(0); indices.push_back(3);
			indices.push_back(0); indices.push_back(4);
			indices.push_back(0); indices.push_back(5);

			indices.push_back(1); indices.push_back(6);
			indices.push_back(1); indices.push_back(7);
			indices.push_back(1); indices.push_back(8);

			indices.push_back(2); indices.push_back(9);
			indices.push_back(2); indices.push_back(10);
			indices.push_back(2); indices.push_back(11);

    	} // end else if (isStructured())

        // homothétie pour se mettre à l'échelle
        for (uint i=0; i<points.size(); i++)
        	points[i] = points[i]*ratio;

        // translation
        for (uint i=0; i<points.size(); i++)
        	points[i] += orig;

    } // end if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWDISCRETISATIONTYPE))

}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* CoFace::
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
            Utils::SerializedRepresentation::Property ("Méthode de maillage", getMeshLawName()));

     /// ajoute la description des propriétés spécifiques à la méthode de discrétisation du bloc
     m_mesh_property->addProperties(topoProprietes);

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Méthode maillage structurée", isStructured()));

    if (isStructured()){
        uint nbI, nbJ;
        getNbMeshingEdges(nbI, nbJ);

        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de maille 1ère direction", (long)nbI));
        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de maille 2ème direction", (long)nbJ));
    }

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Est maillée", isMeshed()));

    if (isMeshed()){
        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de noeuds", (long)m_mesh_data->nodes().size()));

        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de polygones", (long)m_mesh_data->faces().size()));
    }


    description->addPropertiesSet (topoProprietes);


    // les relations vers les autres types d'entités topologiques
    // on cache l'existance des Edge et Face en mode release

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

    if (Internal::InternalPreferences::instance ( )._displayEdge.getValue ( )){
    	std::vector<Edge* > ed;
    	getEdges(ed);

    	Utils::SerializedRepresentation  edges ("Arêtes topologiques",
    			TkUtil::NumericConversions::toStr(ed.size()));
    	for (std::vector<Topo::Edge*>::iterator iter = ed.begin( ); ed.end( )!=iter; iter++)
    		edges.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (edges);
    }
    else {
    	std::vector<CoEdge* > ce;
    	getCoEdges(ce, false);

    	Utils::SerializedRepresentation  coedges ("Arêtes topologiques",
    			TkUtil::NumericConversions::toStr(ce.size()));
    	for (std::vector<Topo::CoEdge*>::iterator iter = ce.begin( ); ce.end( )!=iter; iter++)
    		coedges.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (coedges);
    }

   if (Internal::InternalPreferences::instance ( )._displayFace.getValue ( )){
	   std::vector<Face* > fa;
	   getFaces(fa);

	   if (!fa.empty()){
		   Utils::SerializedRepresentation  faces ("Faces topologiques",
				   TkUtil::NumericConversions::toStr(fa.size()));

		   for (std::vector<Topo::Face*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
			   faces.addProperty (
					   Utils::SerializedRepresentation::Property (
							   (*iter)->getName ( ),  *(*iter)));
		   topoRelation.addPropertiesSet (faces);
	   }
   }
   else {
	   std::vector<Block* > bl;
	   getBlocks(bl);

	   if (!bl.empty()){
		   Utils::SerializedRepresentation  blocks ("Blocs topologiques",
				   TkUtil::NumericConversions::toStr(bl.size()));
		   for (std::vector<Topo::Block*>::iterator iter = bl.begin( ); bl.end( )!=iter; iter++)
			   blocks.addProperty (
					   Utils::SerializedRepresentation::Property (
							   (*iter)->getName ( ),  *(*iter)));
		   topoRelation.addPropertiesSet (blocks);
	   }
   }

   description->addPropertiesSet (topoRelation);

   std::vector<Group::Group2D*> grp;
   getGroups(grp);
   if (!grp.empty()){
	   Utils::SerializedRepresentation  groupe ("Relation vers les groupes",
			   TkUtil::NumericConversions::toStr(grp.size()));
	   for (std::vector<Group::Group2D*>::iterator iter = grp.begin( ); iter!=grp.end( ); ++iter)
		   groupe.addProperty (
				   Utils::SerializedRepresentation::Property (
						   (*iter)->getName ( ),  *(*iter)));

	   description->addPropertiesSet (groupe);

   } // end if (!grp.empty())

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
bool CoFace::
isStructured() const
{
    return m_mesh_property->isStructured();
}
/*----------------------------------------------------------------------------*/
bool CoFace::structurable()
{
   if (!isStructured()) {
       if (m_topo_property->hasHole())
           return false;

       if ((getNbEdges() == 4)    // 4 côtés et discrétisations des arêtes opposées ==
               && (getEdge(0)->getNbMeshingEdges()
               == getEdge(2)->getNbMeshingEdges())
               && (getEdge(1)->getNbMeshingEdges()
               == getEdge(3)->getNbMeshingEdges()))
           return true;
       else if ((getNbEdges() == 3)    // 3 côtés et discrétisations des arêtes opposées ==
               && (getEdge(0)->getNbMeshingEdges()
                       == getEdge(2)->getNbMeshingEdges()))
           return true;
       else
           return false;
   }
   return true;
}
/*----------------------------------------------------------------------------*/
bool CoFace::unstructurable()
{
    // il faut que tous les blocs incidents soient non structurés
    for (uint j=0; j<getNbFaces(); j++){
        Face* face = getFace(j);
        for (uint i=0; i<face->getNbBlocks(); i++)
            if (face->getBlock(i)->isStructured())
                return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
void CoFace::structure(Internal::InfoCommand* icmd)
{
    if (!structurable())
        throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'une CoFace impossible",  TkUtil::Charset::UTF_8));

    // c'est déjà fait
    if (getMeshLaw() == CoFaceMeshingProperty::transfinite)
    	return;

    CoFaceMeshingProperty* prop = new FaceMeshingPropertyTransfinite();
    switchCoFaceMeshingProperty(icmd, prop);
    delete prop;
}
/*----------------------------------------------------------------------------*/
void CoFace::unstructure(Internal::InfoCommand* icmd)
{
    if (!unstructurable())
        throw TkUtil::Exception (TkUtil::UTF8String ("Destructuration d'une CoFace impossible",  TkUtil::Charset::UTF_8));
    CoFaceMeshingProperty* prop = new FaceMeshingPropertyDelaunayGMSH();
    switchCoFaceMeshingProperty(icmd, prop);
    delete prop;
}
/*----------------------------------------------------------------------------*/
uint CoFace::check() const
{
#ifdef _DEBUG2
	std::cout<<"CoFace::check() pour coface "<<*this<<std::endl;
#endif

    if (isDestroyed()){
    	TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
    	messErr << "On utilise une face détruite : "<<getName();
    	throw TkUtil::Exception(messErr);
    }

    if (isStructured()){
        // il faut qu'il y ait 3 ou 4 arêtes
        if (getNbEdges() != 3 && getNbEdges() != 4){
            TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
             messErr << "Erreur avec la face commune structurée "<<getName()
                     <<", il doit y avoir 3 ou 4 arêtes, alors qu'il y en a "<<(short)getNbEdges();
             throw TkUtil::Exception(messErr);
        }

        // controle des discrétisations des arêtes (pour les deux directions)
        for (uint i=0; i<2; i++){
            std::vector<Edge* > aretes;
            getEdges((eDirOnCoFace)i, aretes);

            if (aretes.size() == 2){

                if (aretes[0]->getNbNodes() != aretes[1]->getNbNodes()){
                    TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
                    messErr << "Erreur avec la face commune structurée "<<getName()
                            <<", 2 arêtes n'ont pas la même discrétisation, vérifier [";
                    for (uint j=0; j<aretes[0]->getNbCoEdges(); j++){
                    	if (j)
                    		messErr<<", ";
                    	messErr << aretes[0]->getCoEdge(j)->getName()<<" ("<<(short)aretes[0]->getCoEdge(j)->getNbMeshingEdges()<<")";
                    }
                    messErr<<"] et [";
                    for (uint j=0; j<aretes[1]->getNbCoEdges(); j++){
                    	if (j)
                    		messErr<<", ";
                    	messErr << aretes[1]->getCoEdge(j)->getName()<<" ("<<(short)aretes[1]->getCoEdge(j)->getNbMeshingEdges()<<")";
                    }
                    messErr<<"]";
                    throw TkUtil::Exception(messErr);
                }
            }
        } // end for i<2
    } // end if (isStructured())
    // sinon on ne vérifie rien pour la face

    for(uint i=0; i < getNbEdges();i++)
       getEdge(i)->check();

    if (getNbFaces()>2){
        TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
        messErr << "La face commune "<<getName()<<" pointe sur plus de 2 faces";
        throw TkUtil::Exception(messErr);
    }

    for (uint i=0; i<getNbFaces(); i++)
        if (getFace(i)->isDestroyed()){
            std::cerr<<"CoFace::check() pour la face commune "<<getName()<<std::endl;
            throw TkUtil::Exception (TkUtil::UTF8String ("Une face commune pointe sur une face détruite",  TkUtil::Charset::UTF_8));
        }

    if (getGeomAssociation() && getGeomAssociation()->isDestroyed()){
        TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur avec la face commune "<<getName()
                <<", elle pointe sur "<<getGeomAssociation()->getName()
                <<" qui est détruite !";
        throw TkUtil::Exception(messErr);
    }

    if (isStructured()){
        // calcul le nombre de bras pour chacune des directions
        uint nbBrasDir[2];
        getNbMeshingEdges(nbBrasDir[0], nbBrasDir[1]);
        return nbBrasDir[0]*nbBrasDir[1];
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
bool CoFace::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const CoFace & f)
{
    o << " " << f.getName() << " (uniqueId "<<f.getUniqueId()<<")";

    if (f.isStructured()){
        uint nbI, nbJ;
        f.getNbMeshingEdges(nbI, nbJ);
        o << " (structurée "<<(short)nbI<<"x"<<(short)nbJ<<") ";
    } else {
        o << " (non structurée) ";
    }
    o << f.getMeshLawName();

    o << (f.isMeshed()?" (maillée)":" (non maillée)");
    o << (f.isDestroyed()?" (DETRUITE)":"");
    o << (f.isEdited()?" (EN COURS D'EDITION)":"");

    if (f.getGeomAssociation()){
        o << ", projetée sur " << f.getGeomAssociation()->getName()
        <<(f.getGeomAssociation()->isDestroyed()?" (DETRUITE)":"");

        std::vector<std::string> gn;
        f.getGeomAssociation()->getGroupsName(gn);
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

    o << " avec "<<(short)f.getNbEdges()<<" arêtes:\n";
    const std::vector<Topo::Edge* > & edges = f.getEdges();
    for (uint i=0; i<edges.size();i++)
        o <<" "<<edges[i]->getName();
//      o <<*edges[i];
    o << "\n";

    o << " " << f.getName()<< " face commune avec "<<(short)f.getNbVertices()<<" sommets:\n";
    const std::vector<Topo::Vertex* > & vertices = f.getVertices();
    for (uint i=0; i<vertices.size();i++){
        o <<"   "<<*vertices[i]<<"\n";
        if (f.getNbFaces() == 0){
            const std::vector<Topo::CoEdge* > & coedges = vertices[i]->getCoEdges();
            o << " ce sommet pointe sur les arêtes communes :";
            for (uint j=0; j<coedges.size(); j++){
                o << " " << coedges[j]->getName();
                if (coedges[j]->isDestroyed())
                    o << "  [DETRUITE] ";
            }
            o <<"\n";
        }
    }

    const std::vector<Topo::Face* > & faces = f.getFaces();

    if (faces.empty()){
        o << "    cette face commune ne pointe sur aucune face.";
    }
    else {
        o << "   " << (faces.size()==1?"cette face commune pointe sur la face :":"cette face pointe sur les faces :");
        for (uint i=0; i<faces.size();i++){
            o << " " << faces[i]->getName();
            if (faces[i]->isDestroyed())
                o << "  [DETRUITE] ";
        }
    }
    o << "\n";
    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const CoFace & f)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << f;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo CoFace::getInfos() const
{
	Topo::TopoInfo infos;
	infos.name = getName();
	infos.dimension = getDim();
	getVertices(infos.incident_vertices);
	getCoEdges(infos.incident_coedges);
	getEdges(infos.incident_edges);
	getFaces(infos.incident_faces);
	getBlocks(infos.incident_blocks);
	infos.geom_entity = getGeomAssociation();
	return infos;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String CoFace::getInfoBlock() const
{
    // recherche de la face (unique)
    const std::vector<Face* >& faces = getFaces();
    if (faces.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getInfoBlock() ne fonctionne pas sans une Face",  TkUtil::Charset::UTF_8));
    else if (faces.size()!=1)
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getInfoBlock() ne fonctionne pas sans une unique Face",  TkUtil::Charset::UTF_8));

    Face* face = faces[0];

    // recherche du bloc (unique)
    const std::vector<Block* >& blocs = face->getBlocks();
    if (blocs.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getInfoBlock() ne fonctionne pas sans un bloc pour la face",  TkUtil::Charset::UTF_8));
    else if (blocs.size()!=1)
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getInfoBlock() ne fonctionne pas sans un unique bloc pour la face",  TkUtil::Charset::UTF_8));

    Block* bloc = blocs[0];

    if (!bloc->isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getInfoBlock() ne fonctionne pas pour un bloc non structuré",  TkUtil::Charset::UTF_8));

    uint ind = bloc->getIndex(face);

	TkUtil::UTF8String	str (TkUtil::Charset::UTF_8);

    str << bloc->getName()<<" Block::"<<Block::toString((Block::eFaceOnBlock)ind);

    return str;
}
/*----------------------------------------------------------------------------*/
void CoFace::
getNodes(Topo::Vertex* sommet1, Topo::Vertex* sommet2, Topo::Vertex* sommet3, Topo::Vertex* sommet4,
        std::vector<gmds::Node> &vectNd)
{
    uint nbVtx = getNbVertices();
#ifdef _DEBUG
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "CoFace::getNodes("
            <<sommet1->getName()<<","
            <<sommet2->getName()<<","
            <<sommet3->getName();
    if (nbVtx == 4)
        message1 <<","<<sommet4->getName();
    message1 <<") pour la face commune "
            <<getName()<<"\n";
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
#endif

    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getNodes n'est pas possible avec une face non structurée",  TkUtil::Charset::UTF_8));

    // calcul le décalage entre les sommets internes et ceux donnés
    uint decalage;
    for (decalage=0; decalage<nbVtx && sommet1!=getVertex(decalage); decalage++)
        ;


#ifdef _DEBUG
    if (decalage==nbVtx){
        TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
        message << "on ne trouve pas le sommet "<<sommet1->getUniqueId()
                <<" dans ("<<getVertex(0)->getUniqueId()
                <<", "<<getVertex(1)->getUniqueId()
                <<", "<<getVertex(2)->getUniqueId();
        if (nbVtx == 4)
            message <<", "<<getVertex(3)->getUniqueId();
        message  <<")";
        throw TkUtil::Exception(message);
    }
#endif

    bool sens_directe;

    if (sommet2 == getVertex((decalage+1)%nbVtx))
        sens_directe = true;
    else if (sommet2 == getVertex((decalage+nbVtx-1)%nbVtx))
        sens_directe = false;
    else {
#ifdef _DEBUG
        TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
        message << "on ne trouve pas le 2ème sommet  "<<sommet2->getUniqueId()
                <<" dans ("<<getVertex(0)->getUniqueId()
                <<", "<<getVertex(1)->getUniqueId()
                <<", "<<getVertex(2)->getUniqueId();
        if (nbVtx == 4)
            message <<", "<<getVertex(3)->getUniqueId();
        message <<") avec decalage de "<<(short)decalage;
        throw TkUtil::Exception(message);
#endif
    }

#ifdef _DEBUG
    // vérification sur l'ensemble des sommets
    if (((nbVtx == 4) &&
         ((sens_directe && (sommet1!=getVertex(decalage)
                || sommet2 != getVertex((decalage+1)%4)
                || sommet3 != getVertex((decalage+2)%4)
                || sommet4 != getVertex((decalage+3)%4)))
                || (!sens_directe && (sommet1!=getVertex(decalage)
                        || sommet2 != getVertex((decalage+3)%4)
                        || sommet3 != getVertex((decalage+2)%4)
                        || sommet4 != getVertex((decalage+1)%4)))))
        || // cas à 3 sommets
        ((nbVtx == 3) &&
          ((sens_directe && (sommet1!=getVertex(decalage)
                || sommet2 != getVertex((decalage+1)%3)
                || sommet3 != getVertex((decalage+2)%3)))
                || (!sens_directe && (sommet1!=getVertex(decalage)
                        || sommet2 != getVertex((decalage+2)%3)
                        || sommet3 != getVertex((decalage+1)%3)))))){
		TkUtil::UTF8String	message_err (TkUtil::Charset::UTF_8);
        message_err << "CoFace::getNodes("
                <<sommet1->getName()<<","
                <<sommet2->getName()<<","
                <<sommet3->getName()<<","
                <<sommet4->getName()<<")\n";
        message_err << "sens_directe = "<<(short)sens_directe
                <<" decalage = "<<(short)decalage<<"\n";
        message_err << *this;
        std::cout<<message_err;
        throw TkUtil::Exception (TkUtil::UTF8String ("echec sur le test global des sommets",  TkUtil::Charset::UTF_8));
    }

#endif

    // pour le dégénéré, si le sens est inversé
    // alors on prend le dernier noeud dégénéré, donc un décalage de 3
    if (decalage == 0 && nbVtx == 3 && !sens_directe)
    	decalage = 3;

    getNodes(decalage, sens_directe, vectNd);
}
/*----------------------------------------------------------------------------*/
void CoFace::
getNodes(uint decalage,
        bool sens_directe,
        std::vector<gmds::Node> &vectNd)
{
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    if (!m_mesh_data->isMeshed()){
        message1 <<" ne peut se faire car la face n'est pas maillée";
        throw TkUtil::Exception (message1);
    }

#ifdef _DEBUG2
    std::cout << "CoFace::getNodes("
              <<(short)decalage<<","
              <<(short)sens_directe<<", vectNd) pour la face commune "
              << getName()<<std::endl;
    //std::cout << "this :"<<*this;
#endif

    const uint nbBrasI = getEdge(j_min)->getNbMeshingEdges();
    const uint nbBrasJ = getEdge(i_min)->getNbMeshingEdges();

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;


    // construction des tableaux pour les dep et pas en i et j,
    // pour parcours du tableau local suivant les sommets demandés
    int tabDep[4] = {0, 0, (int)nbBrasI, (int)(nbBrasJ*nbNoeudsI)};
    int tabPas[4] = {1, (int)nbNoeudsI, -1, (int)-nbNoeudsI};

    int idep = (sens_directe?tabDep[decalage]:tabDep[(decalage+3)%4]);
    int ipas = (sens_directe?tabPas[decalage]:tabPas[(decalage+3)%4]);
    int jdep = (sens_directe?tabDep[(decalage+1)%4]:tabDep[(decalage+2)%4]);
    int jpas = (sens_directe?tabPas[(decalage+1)%4]:tabPas[(decalage+2)%4]);

    // nombre de points dans les 2 directions locales
    int nbPtI, nbPtJ;
    if ( (sens_directe && (decalage%2 == 0)) || (!sens_directe && (decalage%2 != 0))){
        nbPtI = nbNoeudsI;
        nbPtJ = nbNoeudsJ;
    }
    else {
        nbPtI = nbNoeudsJ;
        nbPtJ = nbNoeudsI;
    }
#ifdef _DEBUG2
    std::cout << "nbPtI = " << nbPtI << std::endl;
    std::cout << "nbPtJ = " << nbPtJ << std::endl;
    std::cout << "idep = " << idep << std::endl;
    std::cout << "ipas = " << ipas << std::endl;
    std::cout << "jdep = " << jdep << std::endl;
    std::cout << "jpas = " << jpas << std::endl;
#endif

    vectNd.resize((nbNoeudsI)*(nbNoeudsJ));
#ifdef _DEBUG
    for (int i=0; i<(nbNoeudsI)*(nbNoeudsJ); i++)
        vectNd[i] = gmds::Node();
#endif

    gmds::IGMesh& gmds_mesh = getContext().getMeshManager().getMesh()->getGMDSMesh();

    std::vector<gmds::TCellID>& nodeIDs = nodes();
    std::vector<gmds::Node> myNodes;
    myNodes.resize(nodeIDs.size());
#ifdef _DEBUG2
    std::cout<<"======================="<<std::endl;
    {
        std::cout<<"Nb nodes face nodes: "<<nodeIDs.size()<<std::endl;
        std::cout<<"Nb nodes global    : "<<gmds_mesh.getNbNodes()<<std::endl;
    }
    std::cout<<"======================="<<std::endl;
    std::cout<<"COFACE NODES: "<<nodeIDs.size()<<std::endl;
#endif
    for(unsigned int i=0;i<nodeIDs.size();i++)
    {
#ifdef _DEBUG2
        std::cout<<"id: "<<nodeIDs[i]<<std::endl;
#endif
        gmds::Node current = gmds_mesh.get<gmds::Node>(nodeIDs[i]);
        myNodes[i]=current;
    }
    for (int j=0, jface=jdep; j<nbPtJ; j++, jface+=jpas)
        for (int i=0, iface=idep; i<nbPtI; i++, iface+=ipas){
#ifdef _DEBUG2
            std::cout <<"("<<i<<","<<j<<"): vectNd["<<i+j*nbPtI<<"] = nodes()["<<iface+jface<<"]\n";
#endif
            vectNd[i+j*nbPtI] = myNodes[iface+jface];
        }
#ifdef _DEBUG2
    std::cout << "sens_directe = " << sens_directe << " , decalage = " <<decalage<< std::endl;
    std::cout << "idep = " << idep << " ,ipas = " <<ipas<< std::endl;
    std::cout << "jdep = " << jdep << " ,jpas = " <<jpas<< std::endl;
    std::cout << "CoFace::getNodes retourne :\n";
    for (int j=0; j<nbPtJ; j++)
        for (int i=0; i<nbPtI; i++)
            std::cout << "  noeud["<<i<<","<<j<<"] = "
            << getContext().getMeshManager().getMesh()->getInfo(vectNd[i+j*nbPtI])
            << std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CoFace::
fuse(CoFace* face_B,
        Internal::InfoCommand* icmd)
{
	if (face_B == this){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
		err << "CoFace::fuse impossible, les faces sont déjà fusionnées en une face: "
				<<getName();
		throw TkUtil::Exception(err);
	}

    // recherche des couples de sommets confondus
    const std::vector<Topo::Vertex* > & vertices_A = getVertices();
    const std::vector<Topo::Vertex* > & vertices_B = face_B->getVertices();
    std::map<Topo::Vertex*, Topo::Vertex*> corr_vertex_A_B;

    Vertex::findNearlyVertices(vertices_A, vertices_B, corr_vertex_A_B);

    std::vector<Topo::Vertex* > vertices_B_new;
    for (std::vector<Topo::Vertex* >::const_iterator iter = vertices_A.begin();
            iter != vertices_A.end(); ++iter){
        Topo::Vertex* hv(corr_vertex_A_B[*iter]);
        if (0 == hv){
			TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
            err << "CoFace::fuse impossible, on ne trouve pas de sommet en corespondance avec le sommet "
                    <<(*iter)->getName();
            throw TkUtil::Exception(err);
        }
        vertices_B_new.push_back(hv);
    }

    // vérification de la bijection, que les sommets de vertices_B_new sont pris une unique fois
    std::map<Topo::Vertex*, uint> filtre_B;
    for (uint i=0; i<vertices_B_new.size(); i++)
    	if (filtre_B[vertices_B_new[i]] == 0)
    		filtre_B[vertices_B_new[i]] = 1;
    	else {
			TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    		err << "Fusion de faces impossible, 2 sommets sont projetés sur le sommet "
    				<<vertices_B_new[i]->getName()
					<<" , il est indispensable que les sommets soient suffisamment proches pour que la fusion se fasse sans ambiguïté d'association.";
    		throw TkUtil::Exception(err);
    	}

    // appel à la fusion des faces
    fuse(face_B, vertices_A, vertices_B_new, icmd);
}
/*----------------------------------------------------------------------------*/
void CoFace::
fuse(CoFace* face_B,
        const std::vector<Topo::Vertex* >& l_sommets_A,
        const std::vector<Topo::Vertex* >& l_sommets_B,
        Internal::InfoCommand* icmd)
{
#ifdef _DEBUG
    // LOG
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "CoFace::fuse(...) \n";
    message1 << "## pour "<<*this;
    message1 << "## avec  face_B : "<<*face_B;
    message1 << "l_sommets_A : ";
    for (std::vector<Topo::Vertex* >::const_iterator iter = l_sommets_A.begin();
            iter != l_sommets_A.end(); ++iter)
        message1 << " " << **iter;
    message1 << "\n";
    message1 << "l_sommets_B : ";
    for (std::vector<Topo::Vertex* >::const_iterator iter = l_sommets_B.begin();
            iter != l_sommets_B.end(); ++iter)
        message1 << " " << **iter;
    message1 << "\n";
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
#endif

    // précondition: les listes de sommets doivent être de même taille
    if (l_sommets_A.size() != l_sommets_B.size()){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "CoFace::fuse impossible avec des listes de sommets de tailles différentes ("
                <<l_sommets_A.size()<<" != "<<l_sommets_B.size()<<")";
        throw TkUtil::Exception(err);
    }

    //-----------------------------------------------------------------------------
    //PHASE DE VERIFICATION DE VALIDITE DE FUSION DES ARETES
    //(Peut etre sortir ça dans une nouvelle methode et la porter a toutes les entités)
    for(uint i=0; i<l_sommets_A.size(); i++){

        //Les sommets des deux faces sont listés dans le meme ordre
        Topo::Vertex* som_1A = l_sommets_A[i];
        Topo::Vertex* som_2A = l_sommets_A[(i+1)%l_sommets_A.size()];

        Topo::Vertex* som_1B = l_sommets_B[i];
        Topo::Vertex* som_2B = l_sommets_B[(i+1)%l_sommets_B.size()];

        //On récupère les paires d'aretes opposées
        Edge* arete_A = getEdge(som_1A, som_2A);
        Edge* arete_B = face_B->getEdge(som_1B, som_2B);



        std::vector<CoEdge*> coedges_A;
        arete_A->getCoEdges(coedges_A);

        std::vector<CoEdge*> coedges_B;
        arete_B->getCoEdges(coedges_B);

        int nbDiscrA = 0;
        int nbDiscrB = 0;

        if (arete_A->getNbCoEdges() != 1 || arete_B->getNbCoEdges() != 1) {

            if(arete_A->getNbCoEdges() != arete_B->getNbCoEdges()
               && arete_A->getNbCoEdges() > 1 && arete_B->getNbCoEdges() > 1){
                TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
                err << "CoFace::fuse impossible (pour le moment) avec des arêtes composées d'un nombre différent d'arêtes communes ("
                    <<arete_A->getName()<<" et "<<arete_B->getName()<<")";
                throw TkUtil::Exception(err);
            }

            //Si on a plusieurs coedges pour une arete on vérifie que les sommes des discrétisations des coedges de chaque
            //arete sont égales

            for (auto coedge : coedges_A) {
                nbDiscrA += coedge->getNbMeshingEdges();
            }

            for (auto coedge : coedges_B) {
                nbDiscrB += coedge->getNbMeshingEdges();
            }

            if (nbDiscrA != nbDiscrB) {
                TkUtil::UTF8String message(TkUtil::Charset::UTF_8);
                message << "Fusion entre les deux listes d'arêtes [";
                for(int i_a=0; i_a<coedges_A.size()-1; i_a++)
                    message << coedges_A[i_a]->getName() << ", ";
                message << coedges_A.back()->getName() << "]"
                        << " et [";
                for(int i_b=0; i_b<coedges_B.size()-1; i_b++)
                    message << coedges_B[i_b]->getName() << ", ";
                message << coedges_B.back()->getName() << "]"
                        << " impossible car les discrétisations sont différentes (";
                message << (long) nbDiscrA
                        << " != ";
                message << (long) nbDiscrB
                        << ")";
                throw TkUtil::Exception(message);
            }
        }else{

            CoEdge* coedge_A = arete_A->getCoEdge(0);
            CoEdge* coedge_B = arete_B->getCoEdge(0);
            nbDiscrA = coedge_A->getNbMeshingEdges();
            nbDiscrB = coedge_B->getNbMeshingEdges();

            if (nbDiscrA != nbDiscrB) {
                TkUtil::UTF8String message(TkUtil::Charset::UTF_8);
                message << "Fusion entre les arêtes "
                        << coedge_A->getName()
                        << " et "
                        << coedge_B->getName()
                        << " impossible car les discrétisations sont différentes (";
                message << (long) nbDiscrA
                        << " != ";
                message << (long) nbDiscrB
                        << ")";
                throw TkUtil::Exception(message);
            }
        }
    }
    //-----------------------------------------------------------------------------

    // on effectue la fusion des sommets
    // (on ne fait que remplacer les sommets de l_sommets_B par ceux de l_sommets_A)
    std::vector<Topo::Vertex* >::const_iterator iterA = l_sommets_A.begin();
    std::vector<Topo::Vertex* >::const_iterator iterB = l_sommets_B.begin();
    for (; iterA != l_sommets_A.end(); ++iterA, ++iterB)
        (*iterA)->merge(*iterB, icmd);

    // on marque la face commune pour éviter de modifier la liste de ses arêtes
    if (face_B->isStructured() && !isStructured())
        setDestroyed(true);
    else
        face_B->setDestroyed(true);
    // on met au courant la commande de la destruction
    if (icmd)
        icmd->addTopoInfoEntity(face_B,Internal::InfoCommand::DELETED);

    // on effectue la fusion des arêtes
    // (on recherche les arêtes de chacunes des faces,
    // puis on substitue celles de B par celles de this)
    for (uint i=0; i<l_sommets_A.size(); i++){
        Topo::Vertex* som_1 = l_sommets_A[i];
        Topo::Vertex* som_2 = l_sommets_A[(i+1)%l_sommets_A.size()];

        Edge* arete_A = getEdge(som_1, som_2);
        Edge* arete_B = face_B->getEdge(som_1, som_2);


        if (arete_A->getNbCoEdges() != 1 || arete_B->getNbCoEdges() != 1){
        	// s'il y en a autant dans les 2 arêtes, on tente de faire la fusion
        	// en ordonnant les arêtes depuis un sommet
        	if (arete_A->getNbCoEdges() == arete_B->getNbCoEdges()){
        		std::vector<CoEdge* > coedges_A;
        		arete_A->getCoEdges(coedges_A);
        		std::vector<CoEdge* > coedges_A_ord;
        		TopoHelper::getCoEdgesBetweenVertices(som_1, som_2, coedges_A, coedges_A_ord);
        		std::vector<CoEdge* > coedges_B;
        		arete_B->getCoEdges(coedges_B);
        		std::vector<CoEdge* > coedges_B_ord;
        		TopoHelper::getCoEdgesBetweenVertices(som_1, som_2, coedges_B, coedges_B_ord);

        		// fusion des sommets interne à l'Edge
        		for (uint j=0; j<coedges_A.size()-1; j++){
        			Topo::Vertex* som_A = TopoHelper::getCommonVertex(coedges_A_ord[j], coedges_A_ord[j+1]);
        			Topo::Vertex* som_B = TopoHelper::getCommonVertex(coedges_B_ord[j], coedges_B_ord[j+1]);
        			som_A->merge(som_B, icmd);
        		}

        		for (uint j=0; j<coedges_A.size(); j++)
        			coedges_A_ord[j]->merge(coedges_B_ord[j], icmd);
        	}
        	else if (arete_A->getNbCoEdges() == 1) {
        		arete_B->merge(arete_A, icmd, true);
        	}
        	else if (arete_B->getNbCoEdges() == 1) {
        		arete_A->merge(arete_B, icmd, true);
        	}
        	else {
        	    //Redondant avec la vérification mais on le laisse pour le moment, a retirer quand on sera sur que
        	    //La vérification empeche bien d'arriver dans ce cas
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        		err << "CoFace::fuse impossible (pour le moment) avec des arêtes composées d'un nombre différent d'arêtes communes ("
        				<<arete_A->getName()<<" et "<<arete_B->getName()<<")";
        		throw TkUtil::Exception(err);
        	}
        }
        else {
        	// cas avec une seule arête de part et d'autre
        	CoEdge* coedge_A = arete_A->getCoEdge(0);
        	CoEdge* coedge_B = arete_B->getCoEdge(0);

        	coedge_A->merge(coedge_B, icmd);
        }
    }

    // on effectue la fusion des faces
    // (on remplace face_B par face_A dans le bloc associé)
    // ou l'inverse si le type de maillage est prioritaire dans face_B
    if (face_B->isStructured() && !isStructured())
        face_B->merge(this, icmd);
    else
        merge(face_B, icmd);
}
/*----------------------------------------------------------------------------*/
void CoFace::
saveCoFaceMeshingProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_property == 0){
        	if (isMeshed()){
        		TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
        		message << "On ne peut pas modifier une face alors qu'elle est déjà maillée.";
        		throw TkUtil::Exception (message);
        	}
        	else
        		m_save_mesh_property = m_mesh_property->clone();
        }
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::
switchCoFaceMeshingProperty(Internal::InfoCommand* icmd, CoFaceMeshingProperty* prop)
{
	if (isMeshed()){
		TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
		message << "On ne peut pas modifier une face ("<<getName()<<") alors qu'elle est déjà maillée.";
		throw TkUtil::Exception (message);
	}

    // propagation aux entités topologiques de niveau supérieur
    if (icmd){
        bool old_strutured = isStructured();
        bool new_structured = prop->isStructured();
        // on propage seulement la destructuration
        if ((old_strutured != new_structured) && old_strutured){
            std::vector<Face*> faces;
            getFaces(faces);
            for (std::vector<Face*>::iterator iter = faces.begin(); iter != faces.end(); ++iter)
                (*iter)->setStructured(icmd, new_structured);
        }
    }


    if (icmd && m_save_mesh_property == 0)
        m_save_mesh_property = m_mesh_property;
    else
        delete m_mesh_property;

    m_mesh_property = prop->clone();
#ifdef _DEBUG2
    std::cout<<"CoFace::switchCoFaceMeshingProperty() de "<<getName()
    		<<", avec m_mesh_property en "<<m_mesh_property->getMeshLawName()<<std::endl;
#endif
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CoFace::
saveCoFaceTopoProperty(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
    std::cout<<"CoFace::saveCoFaceTopoProperty() pour "<<getName()<<", m_save_topo_property "<<(m_save_topo_property?"initialisé":"vide")<<std::endl;
#endif
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
#ifdef _DEBUG2
        std::cout<<"            change = "<<(change?"vrai":"faux")<<std::endl;
#endif
        if (change && m_save_topo_property == 0){
        	if (isMeshed()){
        		TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
        		message << "On ne peut pas modifier une face alors qu'elle est déjà maillée.";
        		throw TkUtil::Exception (message);
        	}
        	else
        		m_save_topo_property = m_topo_property->clone();
        }
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::
saveCoFaceMeshingData(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_data == 0)
            m_save_mesh_data = m_mesh_data->clone();
    }
}
/*----------------------------------------------------------------------------*/
CoFaceMeshingProperty* CoFace::
setProperty(CoFaceMeshingProperty* prop)
{
#ifdef _DEBUG2
	std::cout<<"CoFace::setProperty pour "<<getName()<<" avec prop = "<<prop->getMeshLawName()<<" en remplacement de "<<m_mesh_property->getMeshLawName()<<std::endl;
#endif
    CoFaceMeshingProperty* tmp = m_mesh_property;
    m_mesh_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
CoFaceTopoProperty* CoFace::
setProperty(CoFaceTopoProperty* prop)
{
#ifdef _DEBUG2
    std::cout<<"CoFace::setProperty() pour "<<getName()<<std::endl;
#endif
    CoFaceTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
CoFaceMeshingData* CoFace::
setProperty(CoFaceMeshingData* prop)
{
    CoFaceMeshingData* tmp = m_mesh_data;
    m_mesh_data = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void CoFace::
saveInternals(CommandEditTopo* cet)
{
#ifdef _DEBUG2
    std::cout<<"CoFace::saveInternals() pour "<<getName()<<(m_save_topo_property?" initialisé":" vide")<<std::endl;
#endif
    TopoEntity::saveInternals (cet);

    if (m_save_mesh_property) {
        cet->addCoFaceInfoMeshingProperty(this, m_save_mesh_property);
        m_save_mesh_property = 0;
    }
    if (m_save_topo_property) {
        cet->addCoFaceInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_mesh_property) {
    	ccm->addCoFaceInfoMeshingProperty(this, m_save_mesh_property);
        m_save_mesh_property = 0;
    }
    if (m_save_mesh_data) {
        ccm->addCoFaceInfoMeshingData(this, m_save_mesh_data);
        m_save_mesh_data = 0;
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::
permuteToJmaxEdge(Edge* edge, Internal::InfoCommand* icmd)
{
//    std::cout << "CoFace::permuteToJmaxEdge(...) pour "<<*this;
//    std::cout << " avec edge : "<<*edge<<std::endl;

    // prérequis
    if (getNbVertices() != 4 || getNbEdges() != 4 || !isStructured()){
		TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
        message1 << "CoFace::permuteToJmaxEdge(...) pour "<<*this;
        message1 << " avec edge : "<<*edge;
        log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::permuteToJmaxEdge n'est possible qu'avec une face commune structurée à 4 sommets et 4 arêtes",  TkUtil::Charset::UTF_8));
    }

    // recherche de l'indice de l'arête
    uint ind = 0;
    bool trouve = false;
    for (ind = 0; ind<getNbEdges() && !trouve; ind++)
        if (edge == getEdge(ind))
            trouve = true;
    ind--; // pour revenir à l'indice
    if (!trouve)
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::permuteToJmaxEdge ne trouve pas l'arête",  TkUtil::Charset::UTF_8));

    if (ind != 3){
        uint dec = j_max - ind;
        std::vector<Edge* > initial_edges;
        std::vector<Edge* > sorted_edges;
        std::vector<Topo::Vertex* > initial_vertices;
        std::vector<Topo::Vertex* > sorted_vertices;
        sorted_edges.resize(4);
        sorted_vertices.resize(4);
        m_topo_property->getEdgeContainer().get(initial_edges);
        m_topo_property->getVertexContainer().get(initial_vertices);

        for (uint i=0; i<4; i++){
            sorted_edges[(i+dec)%4] = initial_edges[i];
            sorted_vertices[(i+dec)%4] = initial_vertices[i];
        }

        m_topo_property->getEdgeContainer().clear();
        m_topo_property->getEdgeContainer().add(sorted_edges);
        m_topo_property->getVertexContainer().clear();
        m_topo_property->getVertexContainer().add(sorted_vertices);

        if (dec%2 && getMeshLaw() == CoFaceMeshingProperty::directional){
            saveCoFaceMeshingProperty(icmd);
            FaceMeshingPropertyDirectional* fmp = dynamic_cast<FaceMeshingPropertyDirectional*>(getCoFaceMeshingProperty());
            CHECK_NULL_PTR_ERROR(fmp);
            fmp->permDir();
        }
        else if (dec%2 && getMeshLaw() == CoFaceMeshingProperty::orthogonal){
            saveCoFaceMeshingProperty(icmd);
            FaceMeshingPropertyOrthogonal* fmp = dynamic_cast<FaceMeshingPropertyOrthogonal*>(getCoFaceMeshingProperty());
            CHECK_NULL_PTR_ERROR(fmp);
            fmp->permDir();
        }
    } // end if (ind != 4)
}
/*----------------------------------------------------------------------------*/
void CoFace::
permuteToLastEdge(Edge* edge, Internal::InfoCommand* icmd)
{
//    std::cout << "CoFace::permuteToLastEdge(...) pour "<<*this;
//    std::cout << " avec edge : "<<*edge<<std::endl;

    // prérequis
    if (isStructured() && getNbVertices() == getNbEdges()){
		TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
        message1 << "CoFace::permuteToLastEdge(...) pour "<<*this;
        message1 << " avec edge : "<<*edge;
        log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::permuteToLastEdge n'est possible qu'avec une face commune non-structurée", TkUtil::Charset::UTF_8));
    }

    // recherche de l'indice de l'arête
    uint ind = 0;
    bool trouve = false;
    for (ind = 0; ind<getNbEdges() && !trouve; ind++)
        if (edge == getEdge(ind))
            trouve = true;
    ind--; // pour revenir à l'indice
    if (!trouve)
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::permuteToLastEdge ne trouve pas l'arête", TkUtil::Charset::UTF_8));

    uint nb = getNbEdges();

    if (ind != nb-1){
        uint dec = nb-1 - ind;
        std::vector<Edge* > initial_edges;
        std::vector<Edge* > sorted_edges;
        std::vector<Topo::Vertex* > initial_vertices;
        std::vector<Topo::Vertex* > sorted_vertices;
        sorted_edges.resize(nb);
        sorted_vertices.resize(nb);
        m_topo_property->getEdgeContainer().get(initial_edges);
        m_topo_property->getVertexContainer().get(initial_vertices);

        for (uint i=0; i<nb; i++){
            sorted_edges[(i+dec)%nb] = initial_edges[i];
            sorted_vertices[(i+dec)%nb] = initial_vertices[i];
        }

        m_topo_property->getEdgeContainer().clear();
        m_topo_property->getEdgeContainer().add(sorted_edges);
        m_topo_property->getVertexContainer().clear();
        m_topo_property->getVertexContainer().add(sorted_vertices);

    } // end if (ind != nb-1)
}
/*----------------------------------------------------------------------------*/
void CoFace::
getEdges(eDirOnCoFace dir, std::vector<Edge* > & edges) const
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getEdges(dir) n'est pas possible avec une surface non structurée", TkUtil::Charset::UTF_8));
   edges.clear();
    if (dir == i_dir){
        edges.push_back(getEdge(1));
        if (getNbEdges() == 4)
            edges.push_back(getEdge(3));
    } else if (dir == j_dir){
        edges.push_back(getEdge(0));
        edges.push_back(getEdge(2));
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getEdges avec une direction non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CoFace::
getOrientedCoEdges(std::vector<CoEdge* > & iCoedges,
                   std::vector<CoEdge* > & jCoedges) const
{
    if (!isStructured()){
        TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
        message<<"CoFace::getOrientedCoEdges(,) n'est pas possible avec une face topologique non structurée ("
                << getName()<<")";
        throw TkUtil::Exception(message);
    }
    iCoedges.clear();
    jCoedges.clear();

    // il suffit de cumuler les CoEdges pour chacune des arêtes suivant chaque direction
    std::vector<Edge* > edges;

    getEdges(i_dir, edges);
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter){
        const std::vector<CoEdge* > & coedges = (*iter)->getCoEdges();

        iCoedges.insert(iCoedges.end(), coedges.begin(), coedges.end());
    }

    getEdges(j_dir, edges);
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter){
        const std::vector<CoEdge* > & coedges = (*iter)->getCoEdges();

        jCoedges.insert(jCoedges.end(), coedges.begin(), coedges.end());
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::
getNbMeshingEdges(uint& nbI, uint& nbJ) const
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getNbMeshingEdges(,) n'est pas possible avec une surface commune non structurée", TkUtil::Charset::UTF_8));

    if (getNbEdges()){
        nbI = getEdge(1)->getNbMeshingEdges();
        nbJ = getEdge(0)->getNbMeshingEdges();
    }
    else {
        nbI = 0;
        nbJ = 0;
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::
getCoEdges(std::vector<CoEdge* >& coedges, bool unique) const
{
    if (unique){
        coedges.clear();
        std::list<Topo::CoEdge*> l_e;
        for(uint j=0; j < getNbEdges();j++) {
            const std::vector<CoEdge* > & local_coedges = getEdge(j)->getCoEdges();
            l_e.insert(l_e.end(), local_coedges.begin(), local_coedges.end());
        }

        l_e.sort(Utils::Entity::compareEntity);
        l_e.unique();

        coedges.insert(coedges.end(),l_e.begin(),l_e.end());
    }
    else {
        coedges.clear();
        for(uint j=0; j < getNbEdges();j++) {
            const std::vector<CoEdge* > & local_coedges = getEdge(j)->getCoEdges();
            coedges.insert(coedges.end(), local_coedges.begin(), local_coedges.end());
        }
    }
}
/*----------------------------------------------------------------------------*/
CoFace::eDirOnCoFace CoFace::getDir(Vertex* v1, Vertex* v2) const
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getDir(V1, v2) n'est pas possible avec une surface non structurée", TkUtil::Charset::UTF_8));

    uint nbVtx = getNbVertices();
    // on se base uniquement sur l'ordre des sommets dans la face commune
    if (  (v1 == getVertex(0) && v2 == getVertex(1))
            || (v1 == getVertex(1) && v2 == getVertex(0))
            || (nbVtx == 4 && v1 == getVertex(2) && v2 == getVertex(3))
            || (nbVtx == 4 && v1 == getVertex(3) && v2 == getVertex(2))
            || (nbVtx == 3 && v1 == getVertex(2) && v2 == getVertex(0))
            || (nbVtx == 3 && v1 == getVertex(0) && v2 == getVertex(2)) )
        return j_dir;
    else if (  (v1 == getVertex(1) && v2 == getVertex(2))
            || (v1 == getVertex(2) && v2 == getVertex(1))
            || (nbVtx == 4 && v1 == getVertex(0) && v2 == getVertex(3))
            || (nbVtx == 4 && v1 == getVertex(3) && v2 == getVertex(0)) )
        return i_dir;
    else {
        std::cerr<<"CoFace::getDir("<<v1->getName()<<", "<<v2->getName()<<") dans la face commune : "<<*this<<std::endl;
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas les sommets dans la face commune pour getDir()", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CoFace::getAllVertices(std::vector<Vertex* >& vertices, const bool unique) const
{
    vertices.clear();
    if (unique){
    	std::list<Topo::Vertex*> l_v;
    	for(uint j=0; j < getNbEdges();j++) {
    		const std::vector<CoEdge* > & local_coedges = getEdge(j)->getCoEdges();
    		for (uint i=0; i<local_coedges.size(); i++) {
    			const std::vector<Vertex* > & local_vertices = local_coedges[i]->getVertices();

    			l_v.insert(l_v.end(), local_vertices.begin(), local_vertices.end());
    		}
    	}

    	l_v.sort(Utils::Entity::compareEntity);
    	l_v.unique();

    	vertices.insert(vertices.end(),l_v.begin(),l_v.end());
    }
    else {
    	for(uint j=0; j < getNbEdges();j++) {
    		const std::vector<CoEdge* > & local_coedges = getEdge(j)->getCoEdges();
    		for (uint i=0; i<local_coedges.size(); i++) {
    			const std::vector<Vertex* > & local_vertices = local_coedges[i]->getVertices();

    			vertices.insert(vertices.end(), local_vertices.begin(), local_vertices.end());
    		}
    	}
    }

}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty* CoFace::
getMeshingProperty(eDirOnCoFace dir, bool & is_inverted)
{
	is_inverted = false;
	// recherche parmi les Edges, une arête qui n'ait qu'une seule arête commune
    // et pas de ratio
    std::vector<Edge* > edges;
    getEdges(dir, edges);

    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        if ((*iter)->getNbCoEdges() == 1){
            CoEdge* coedge = (*iter)->getCoEdge(0);
            if ((*iter)->getRatio(coedge) == 1){
            	std::vector<Vertex*> vertices;
            	coedge->getVertices(vertices);
            	uint ind_edge = getIndex(coedge);
            	if (getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnFace[ind_edge][0]) == vertices[0]
            	     && getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnFace[ind_edge][1]) == vertices[1])
            		is_inverted = false;
            	else if (getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnFace[ind_edge][0]) == vertices[1]
            	     && getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnFace[ind_edge][1]) == vertices[0])
            		is_inverted = true;
            	else
            		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CoFace::getMeshingProperty, on ne retrouve pas les noeuds de l'arête", TkUtil::Charset::UTF_8));
                return (*iter)->getMeshingProperty();
            } // end if ((*iter)->getRatio(coedge) == 1)
        }
//    TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
//    messErr << "Erreur avec la face structurée commune "<<getName()
//            <<", il n'est pas possible d'utiliser une discrétisation suivant une direction"
//            <<" car toutes ses arêtes sont soit composées, soit avec un déraffinement";
//    throw TkUtil::Exception(messErr);
    return 0;
}
/*----------------------------------------------------------------------------*/
void CoFace::
getNeighbour(std::vector<CoFace* >& cofaces)
{
    std::list<Topo::CoFace*> l_f;

    for(uint j=0; j < getNbEdges();j++) {
        const std::vector<CoEdge* > & coedges = getEdge(j)->getCoEdges();

        for (std::vector<CoEdge* >::const_iterator iter1 = coedges.begin();
                iter1 != coedges.end(); ++iter1){
            const std::vector<Edge* > & edges = (*iter1)->getEdges();

            for (std::vector<Edge* >::const_iterator iter2 = edges.begin();
                    iter2 != edges.end(); ++iter2){
                const std::vector<CoFace* > & loc_cofaces = (*iter2)->getCoFaces();

                l_f.insert(l_f.end(), loc_cofaces.begin(), loc_cofaces.end());
            }
        }
    }

    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();

    // on supprime la CoFace de départ
    std::list<Topo::CoFace*>::iterator iter;
    iter = std::find(l_f.begin(), l_f.end(), this);
    if (iter != l_f.end())
        l_f.erase(iter);
    else {
    	std::cerr<<" this : "<<*this<<std::endl;
        TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur interne, CoFace::getNeighbour ne trouve pas this ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }
    cofaces.insert(cofaces.end(),l_f.begin(),l_f.end());
}
/*----------------------------------------------------------------------------*/
int CoFace::getNbMeshingFaces()
{
    if (isMeshed())
        return faces().size();

    if (isStructured()) {
        uint nbBrasI = 0;
        uint nbBrasJ = 0;

        getNbMeshingEdges(nbBrasI, nbBrasJ);

        return nbBrasI*nbBrasJ;
    }
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("CoFace::getNbMeshingFaces() n'est pas possible avec une face non structurés (à moins de la mailler)", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
bool CoFace::
isEdited() const
{
    return TopoEntity::isEdited()
    || m_save_topo_property != 0
    || m_save_mesh_property != 0
    || m_save_mesh_data != 0;
}
/*----------------------------------------------------------------------------*/
void CoFace::
getBlocks(std::vector<Block* >& blocks) const
{
    std::list<Topo::Block*> l_b;

    std::vector<Face* > faces;
    getFaces(faces);

    for (std::vector<Face* >::iterator iter1 = faces.begin();
            iter1 != faces.end(); ++iter1){
        std::vector<Block* > loc_bl;
        (*iter1)->getBlocks(loc_bl);

        l_b.insert(l_b.end(), loc_bl.begin(), loc_bl.end());
    }

    l_b.sort(Utils::Entity::compareEntity);
    l_b.unique();

    blocks.insert(blocks.end(),l_b.begin(),l_b.end());
}
/*----------------------------------------------------------------------------*/
uint CoFace::getNbBlocks() const
{
    std::vector<Block* > blocks;
    getBlocks(blocks);
    return blocks.size();
}
/*----------------------------------------------------------------------------*/
void CoFace::
selectBasicMeshLaw(Internal::InfoCommand* icmd, bool forceCompute)
{
//#define _DEBUG_selectBasicMeshLaw
#ifdef _DEBUG_selectBasicMeshLaw
    std::cout<<"selectBasicMeshLaw pour "<<getName()<<" cas d'une coface avec MeshLaw à "<<getMeshLawName()<<std::endl;
#endif

    if (!forceCompute && getMeshLaw() == CoFaceMeshingProperty::directional){
#ifdef _DEBUG_selectBasicMeshLaw
        std::cout<<"  on conserve "<<getMeshLawName()<< " pour "<<getName()<<std::endl;
#endif
        return;
    }

    std::vector<Topo::CoEdge* > iCoedges[2];
    getOrientedCoEdges(iCoedges[0], iCoedges[1]);

    // la direction sélectionnée, à définir
    FaceMeshingPropertyDirectional::meshDirLaw dirLaw = FaceMeshingPropertyDirectional::dir_undef;

    // dirI ok ?
    {
        std::vector<std::vector<CoEdge* > > coedges_dirs;
        std::vector<CoEdge* > coedges_dir1;
        std::vector<CoEdge* > coedges_dir2;

        // il faut qu'un des côté soit en une seule arête sans ratio
        bool unCoteOk = false;

        TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(2), iCoedges[0], coedges_dir1);
        coedges_dirs.push_back(coedges_dir1);
        if (coedges_dir1.size() == 1 && getEdge(getVertex(1), getVertex(2))->getRatio(coedges_dir1[0]) == 1)
        	unCoteOk = true;

        if (getNbVertices() == 4){
            TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(3), iCoedges[0], coedges_dir2);
            coedges_dirs.push_back(coedges_dir2);
            if (coedges_dir2.size() == 1 && getEdge(getVertex(0), getVertex(3))->getRatio(coedges_dir2[0]) == 1)
            	unCoteOk = true;
       }

        if (TopoHelper::isUnidirectionalMeshable(coedges_dirs) && unCoteOk)
            dirLaw = FaceMeshingPropertyDirectional::dir_i;
    }

    // dirJ ok ?
    {
        std::vector<std::vector<CoEdge* > > coedges_dirs;
        std::vector<CoEdge* > coedges_dir1;
        std::vector<CoEdge* > coedges_dir2;

        // il faut qu'un des côté soit en une seule arête sans ratio
        bool unCoteOk = false;

        TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(0), iCoedges[1], coedges_dir1);
        coedges_dirs.push_back(coedges_dir1);
        if (coedges_dir1.size() == 1 && getEdge(getVertex(1), getVertex(2))->getRatio(coedges_dir1[0]) == 1)
        	unCoteOk = true;

        Vertex* vtx3 = getVertex((getNbVertices()==4)?3:0);
        TopoHelper::getCoEdgesBetweenVertices(getVertex(2), vtx3, iCoedges[1], coedges_dir2);
        coedges_dirs.push_back(coedges_dir2);
        if (coedges_dir2.size() == 1 && getEdge(getVertex(2), vtx3)->getRatio(coedges_dir2[0]) == 1)
        	unCoteOk = true;

        if (TopoHelper::isUnidirectionalMeshable(coedges_dirs) && unCoteOk)
            dirLaw = FaceMeshingPropertyDirectional::dir_j;
    }

    // cas où l'on trouve que le maillage peut se faire suivant une direction
    if (dirLaw != FaceMeshingPropertyDirectional::dir_undef){
        saveCoFaceMeshingProperty(icmd);
        FaceMeshingPropertyDirectional* mp = new FaceMeshingPropertyDirectional(dirLaw);
        switchCoFaceMeshingProperty(icmd, mp);
        delete mp;

#ifdef _DEBUG_selectBasicMeshLaw
        std::cout<<"  on attribue "<<getMeshLawName()<< " à "<<getName()<<std::endl;
#endif
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point CoFace::getBarycentre() const
{
	Utils::Math::Point barycentre;

	std::vector<Topo::Vertex* > vertices;
	getVertices(vertices);
	if (vertices.size()){
		for (uint i=0; i<vertices.size(); i++)
			barycentre += vertices[i]->getCoord();
		barycentre /= (double)vertices.size();
	}
	return barycentre;
}
/*----------------------------------------------------------------------------*/
void CoFace::getGroupsName (std::vector<std::string>& gn, bool byGeom, bool byTopo) const
{
	if (byGeom)
		TopoEntity::getGroupsName(gn, byGeom, byTopo);

	if (byTopo)
		for (uint i = 0; i<m_topo_property->getGroupsContainer().size(); ++i)
		        gn.push_back(m_topo_property->getGroupsContainer().get(i)->getName());
}
/*----------------------------------------------------------------------------*/
unsigned long CoFace::getNbInternalMeshingNodes()
{
	if (isStructured()){
		uint nbI;
		uint nbJ;
		getNbMeshingEdges(nbI, nbJ);

		return (nbI-1)*(nbJ-1);
	}
	else {
		unsigned short res = m_mesh_data->nodes().size();
		for (uint i=0;i<getNbVertices(); i++)
			res -= getVertex(i)->getNbInternalMeshingNodes();

		std::vector<CoEdge*> coedges;
		getCoEdges(coedges, false);
		for (uint i=0;i<coedges.size(); i++)
			res -= coedges[i]->getNbInternalMeshingNodes();

		return res;
	}
}
/*----------------------------------------------------------------------------*/
void CoFace::MAJInterpolated(CoFace* newCoface, std::vector<Edge*>& newEdges)
{
	//std::cout<<"MAJInterpolated("<<newCoface->getName()<<")"<<std::endl;
	std::vector<Topo::CoEdge*> coedges;

	// on marque dans un premier temps les nouvelles arêtes
	std::map<CoEdge*, uint> filtre_coedges;
	for (uint i=0; i<newEdges.size(); i++){
		newEdges[i]->getCoEdges(coedges);
		for (uint j=0; j<coedges.size(); j++)
			filtre_coedges[coedges[j]] = 1;
	}

	newCoface->getCoEdges(coedges);
	std::string oldCoFaceName = getName();
	std::string newCoFaceName = newCoface->getName();
	// d'abord les anciennes arêtes
	//std::cout<<"les anciennes arêtes"<<std::endl;
	for (uint i=0; i<coedges.size(); i++){
		Topo::CoEdge* coedge = coedges[i];
		if (filtre_coedges[coedge] == 0
				&& coedge->getMeshingProperty()->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
			EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyInterpolate*>(coedge->getMeshingProperty());
			CHECK_NULL_PTR_ERROR(interpol);
			if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface
					&& interpol->getCoFace() == oldCoFaceName){
				//std::cout<<" changement pour "<<coedge->getName()<<std::endl;
				interpol->setCoFace(newCoFaceName);
			}
		}
	}

	// ensuite les nouvelles, celles insérées pour couper la coface
	//std::cout<<"les nouvelles arêtes"<<std::endl;
	for (uint i=0; i<coedges.size(); i++){
		Topo::CoEdge* coedge = coedges[i];
		if (filtre_coedges[coedge] == 1
				&& coedge->getMeshingProperty()->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
			//std::cout<<" recherche pour "<<coedge->getName()<<std::endl;
			EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyInterpolate*>(coedge->getMeshingProperty());
			CHECK_NULL_PTR_ERROR(interpol);
			if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface
					&& interpol->getCoFace() == oldCoFaceName){
				// recherche si l'une des arêtes qui fait face est interpolée et si oui, est-ce vers cette coface ?
				bool isSwitchOK = true;

				// rechercher l'edge dans laquelle est cette coedge, à partir de la coface.
				Edge* edge_dep = 0;
				try {
					edge_dep = newCoface->getEdgeContaining(coedge);
				}
				catch (TkUtil::Exception &e){
					TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
					message << "Erreur avec l'interpolation de l'arête "
							<< coedge->getName()
							<<", elle référence la face "<<getName()
							<<" alors qu'elles ne sont pas en relation.";
					throw TkUtil::Exception (message);
				}

				Edge* edge_ref = newCoface->getOppositeEdge(edge_dep);
				std::vector<Topo::CoEdge*> coedges_ref;
				edge_ref->getCoEdges(coedges_ref);

				for (uint j=0; j<coedges_ref.size(); j++)
					if (coedges_ref[j]->getMeshingProperty()->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
						//std::cout<<"  prise en compte de l'arête "<<coedges_ref[j]->getName()<<std::endl;
						EdgeMeshingPropertyInterpolate* interpol_ref =
								dynamic_cast<EdgeMeshingPropertyInterpolate*>(coedges_ref[j]->getMeshingProperty());
						CHECK_NULL_PTR_ERROR(interpol_ref);
						//std::cout<<"   interpol_ref->getCoFace() = "<<interpol_ref->getCoFace()<<std::endl;
						if (interpol_ref->getType() == EdgeMeshingPropertyInterpolate::with_coface
								&& newCoFaceName == interpol_ref->getCoFace())
							isSwitchOK = false;
					}

				if (isSwitchOK){
					//std::cout<<" changement pour "<<coedge->getName()<<std::endl;
					interpol->setCoFace(newCoFaceName);
				}
			}
		}
	}

}
/*----------------------------------------------------------------------------*/
void CoFace::MAJInterpolated(CoEdgeMeshingProperty* cemp)
{
	// traitement du cas d'une interpolation utilisant une face autre que this
	// cette interpolation (qui est une copie de celle au bord) doit pointer sur this dans un premier temps
	if (cemp->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
		std::string oldCoFaceName = getName();
		EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyInterpolate*>(cemp);
		CHECK_NULL_PTR_ERROR(interpol);
		if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface
				&& interpol->getCoFace() != oldCoFaceName){
			interpol->setCoFace(oldCoFaceName);
		}
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
