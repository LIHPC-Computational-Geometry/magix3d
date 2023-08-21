/*----------------------------------------------------------------------------*/
/*
 * \file ServiceGeomToTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/ServiceGeomToTopo.h"
#include "Utils/Common.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/CoEdgeMeshingProperty.h"
#include "Topo/BlockMeshingPropertyInsertion.h"

#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"

#include "Internal/Context.h"
#include "Internal/InfoCommand.h"

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <map>
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
ServiceGeomToTopo::ServiceGeomToTopo(Internal::Context& c, Geom::GeomEntity* ge,
		std::vector<Geom::Vertex*>& vertices, Internal::InfoCommand* icmd)
: m_context(c), m_volume(0), m_surface(0), m_icmd(icmd)
{
    Geom::Volume* vol = dynamic_cast<Geom::Volume*> (ge);
    Geom::Surface* surf = dynamic_cast<Geom::Surface*> (ge);
    if (!vol && !surf)
        throw TkUtil::Exception(TkUtil::UTF8String("ServiceGeomToTopo imposible avec une géométrie autre qu'un Volume ou une Surface", TkUtil::Charset::UTF_8));

    m_volume = vol;
    m_surface = surf;
    m_extrem_vertices.insert(m_extrem_vertices.end(), vertices.begin(), vertices.end());

    if (surf && !m_extrem_vertices.empty())
    	if (m_extrem_vertices.size() != 3 && m_extrem_vertices.size() != 4)
    		throw TkUtil::Exception(TkUtil::UTF8String ("ServiceGeomToTopo imposible avec une surface et des sommets extrémités autrement qu'avec 3 ou 4 sommets", TkUtil::Charset::UTF_8));

    if (!icmd)
        throw TkUtil::Exception(TkUtil::UTF8String ("ServiceGeomToTopo non prévu sans InfoCommand", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
ServiceGeomToTopo::~ServiceGeomToTopo()
{
}
/*----------------------------------------------------------------------------*/
bool ServiceGeomToTopo::convertBlockStructured(const int ni, const int nj, const int nk)
{
	const uint tabIndEdgeOnBlock[12] = {2, 1, 2, 1, 2, 1, 2, 1, 0, 0, 0, 0};

    Topo::Block* bloc = getBlock();

    if (bloc->structurable()){
        bloc->structure(m_icmd);

        bloc->selectBasicMeshLaw(m_icmd);

        // cas où le nombre de bras est donné par l'utilisateur
        if (ni>0 && nj>0 && nk>0){
        	std::vector<Topo::CoEdge*> coedges;
        	bloc->getCoEdges(coedges);
        	if (coedges.size() != 12){
        		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, on impose une discrétisation à un bloc qui n'a pas que 12 arêtes", TkUtil::Charset::UTF_8));
        	}
        	int nb[3] = {ni, nj, nk};

        	for (uint i=0; i<12; i++){
        		Topo::CoEdge* coedge = coedges[i];

        		Topo::CoEdgeMeshingProperty* mp = coedge->getMeshingProperty();
        		Topo::CoEdgeMeshingProperty* new_mp = mp->clone();
        		new_mp->setNbEdges(nb[tabIndEdgeOnBlock[i]]);
        		coedge->switchCoEdgeMeshingProperty(m_icmd, new_mp);
        		delete new_mp;
        	}
        }

        return false; // ok
    }
    else
        return true;
}
/*----------------------------------------------------------------------------*/
void ServiceGeomToTopo::convertInsertionBlock()
{
	Topo::Block* bloc = getBlock();
	Topo::BlockMeshingPropertyInsertion mp;
	bloc->switchBlockMeshingProperty(m_icmd, &mp);
}
/*----------------------------------------------------------------------------*/
Topo::Block* ServiceGeomToTopo::getBlock()
{
    return getBlock(m_volume);
}
/*----------------------------------------------------------------------------*/
bool ServiceGeomToTopo::convertCoFaceStructured()
{
	// création de la coface
    Topo::CoFace* coface = getCoFace();

    if (coface->structurable()){
        coface->structure(m_icmd);

        coface->selectBasicMeshLaw(m_icmd);

        return false; // ok
    }
    else
        return true;
}
/*----------------------------------------------------------------------------*/
Topo::CoFace* ServiceGeomToTopo::getCoFace()
{
    return getCoFace(m_surface);
}
/*----------------------------------------------------------------------------*/
Topo::Block* ServiceGeomToTopo::getBlock(Geom::Volume* vol)
{
    Topo::Block* bloc = m_vol_block[vol];

    if (bloc == 0){
        if (vol == 0)
            return 0;

        std::vector<Geom::Surface*> surfaces;
        vol->get(surfaces);

        std::vector<Topo::Face* > faces;
        for (std::vector<Geom::Surface*>::iterator iter1 = surfaces.begin();
                    iter1 != surfaces.end(); ++iter1){

            faces.push_back(getFace(*iter1));
        }

        // on récupère les sommets triés dans l'ordre suivant leur position géométrique
        std::vector<Geom::Vertex*> vertices;
        vol->getGeomSorted(vertices);

        std::vector<Topo::Vertex* > sommets;
        for (std::vector<Geom::Vertex*>::iterator iter = vertices.begin();
                iter != vertices.end(); ++iter){

            sommets.push_back(getVertex(*iter));
        }

        bloc = new Topo::Block(m_context, faces, sommets);
        m_icmd->addTopoInfoEntity(bloc, Internal::InfoCommand::CREATED);
        bloc->setGeomAssociation(vol);

        m_vol_block[vol] = bloc;

    } // end if (bloc == 0)

    return bloc;
}
/*----------------------------------------------------------------------------*/
Topo::Face* ServiceGeomToTopo::getFace(Geom::Surface* surf)
{
    Topo::Face* face = m_surf_face[surf];

    //std::cout<<"ServiceGeomToTopo::getFace pour surf "<<surf->getName()<<std::endl;

    if (face == 0){
        if (surf == 0)
            return 0;

        Topo::CoFace* coface = 0;

        // utilisation si possible d'une topologie s'il y en a une
        const std::vector<Topo::TopoEntity* >& topos = surf->getRefTopo();
        if (topos.size() > 1){

            // il faut récupérer les différentes cofaces qui pointent sur cette surface
            std::vector<Topo::CoFace* > cofaces;

            for (uint i=0; i<topos.size(); i++){
                Topo::TopoEntity* te = topos[i];
                if (te->getDim() == 2){
                    Topo::CoFace* coface = dynamic_cast<Topo::CoFace*>(te);
                    if (coface == 0)
                        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer la face commune", TkUtil::Charset::UTF_8));

                    cofaces.push_back(coface);
                }
            } // end for i

            // récupération des sommets aux extrémités de la surface
            std::vector<Topo::Vertex* > sommets;
            std::vector<Geom::Vertex*> vertices;
            surf->get(vertices);

            for (uint i=0; i<vertices.size(); i++){
                sommets.push_back(getVertex(vertices[i]));
            }

            face = new Topo::Face(m_context, cofaces, sommets, false);

        }
        else if (topos.size() == 1){
            Topo::TopoEntity* te = topos[0];
            if (te->getDim() != 2)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, l'entité topologique qui pointe sur une surface n'est pas de dimension 2", TkUtil::Charset::UTF_8));

            Topo::CoFace* coface = dynamic_cast<Topo::CoFace*>(te);
            if (coface == 0)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer la face", TkUtil::Charset::UTF_8));

            coface->saveCoFaceTopoProperty(m_icmd);

            face = new Topo::Face(m_context, coface);

        }
        else {

            std::vector<Geom::Curve*> curves;
            surf->get(curves);

            // constitution du vecteur d'arêtes en fonction des courbes de la surface

            std::vector<Topo::Edge* > edges;
            for (std::vector<Geom::Curve*>::iterator iter2 = curves.begin();
                    iter2 != curves.end(); ++iter2){
                //std::cout<<"  getEdge pour courbe "<<(*iter2)->getName()<<std::endl;
                edges.push_back(getEdge(*iter2));
            } // end for iter2 = curves.begin()

            Topo::CoFace* coface = new Topo::CoFace(m_context, edges);
            m_icmd->addTopoInfoEntity(coface, Internal::InfoCommand::CREATED);
            coface->setGeomAssociation(surf);

            face = new Topo::Face(m_context, coface);

        }

        m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);

        m_surf_face[surf] = face;

    } // end if (face == 0)

    return face;
}
/*----------------------------------------------------------------------------*/
Topo::CoFace* ServiceGeomToTopo::getCoFace(Geom::Surface* surf)
{
    Topo::CoFace* coface = m_surf_coface[surf];

    //std::cout<<"ServiceGeomToTopo::getCoFace pour surf "<<surf->getName()<<std::endl;

    if (coface == 0){
        if (surf == 0)
            return 0;

        // utilisation si possible d'une topologie s'il y en a une
        const std::vector<Topo::TopoEntity* >& topos = surf->getRefTopo();
        if (topos.size() > 1){

            throw TkUtil::Exception(TkUtil::UTF8String ("Erreur, la surface possède déjà une topologie composée de plusieurs entités topologiques", TkUtil::Charset::UTF_8));

        }
        else if (topos.size() == 1){
            Topo::TopoEntity* te = topos[0];
            if (te->getDim() != 2)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, l'entité topologique qui pointe sur une surface n'est pas de dimension 2", TkUtil::Charset::UTF_8));

            coface = dynamic_cast<Topo::CoFace*>(te);
            if (coface == 0)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer la face", TkUtil::Charset::UTF_8));

            coface->saveCoFaceTopoProperty(m_icmd);
            m_surf_coface[surf] = coface;

        }
        else {
    		std::vector<Topo::Edge* > edges;
    		std::vector<Geom::Curve*> curves;
    		surf->get(curves);
    		bool isStructured = false;
        	if (m_extrem_vertices.empty()){
        		// constitution du vecteur d'arêtes en fonction des courbes de la surface
        		isStructured = false;
        		for (std::vector<Geom::Curve*>::iterator iter2 = curves.begin();
        				iter2 != curves.end(); ++iter2){
        			//std::cout<<"  getEdge pour courbe "<<(*iter2)->getName()<<std::endl;
        			edges.push_back(getEdge(*iter2));
        		} // end for iter2 = curves.begin()


        	} // end if (m_extrem_vertices.empty())
        	else {
        		// cas d'une face dont les extrémités de la topologie sont connues
        		isStructured = true;
        		std::vector<Topo::CoEdge*> coedges;
        		for (uint i=0; i<curves.size(); i++)
        			coedges.push_back(getCoEdge(curves[i]));

        		edges = getEdges(coedges);

        	} // end else / if (m_extrem_vertices.empty())

    		coface = new Topo::CoFace(m_context, edges, isStructured);
    		m_icmd->addTopoInfoEntity(coface, Internal::InfoCommand::CREATED);
    		coface->setGeomAssociation(surf);
        }

    } // end if (coface == 0)

    if (coface == 0)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, ServiceGeomToTopo::getCoFace a échoué", TkUtil::Charset::UTF_8));

    return coface;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Edge*> ServiceGeomToTopo::getEdges(std::vector<Topo::CoEdge*>& coedges)
{
//	std::cout<<"coedges.size() = "<<coedges.size()<<std::endl;
//	for (uint i=0; i<m_extrem_vertices.size(); i++)
//		std::cout<<" m_extrem_vertices["<<i<<"] = "<<m_extrem_vertices[i]->getName()<<std::endl;

	std::vector<Topo::Edge*> edges;
	// on marque les sommets topologiques aux exrémités
	std::map<Topo::Vertex*, uint> filtre_vertices;
	// on va marquer les arêtes pour ne les prendre qu'une fois
	std::map<Topo::CoEdge*, uint> filtre_coedges;

	for (uint i=0; i<coedges.size(); i++)
		filtre_coedges[coedges[i]] = 1;

	Topo::Vertex* first_vertex = 0;
	for (uint i=0; i<m_extrem_vertices.size(); i++){
		Geom::Vertex* vtx = m_extrem_vertices[i];
		std::vector<Topo::TopoEntity*> topos;
		vtx->getRefTopo(topos);
		if (topos.size() == 1){
			if (topos[0]->getType() == Utils::Entity::TopoVertex){
				Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(topos[0]);
				CHECK_NULL_PTR_ERROR(vertex);
				filtre_vertices[vertex] = 1;
				if (first_vertex == 0)
					first_vertex = vertex;
			}
			else
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pas un sommet topo en réf d'un sommet géométrique", TkUtil::Charset::UTF_8));
		}
		else
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pas une unique référence vers la topologie pour un sommet géométrique", TkUtil::Charset::UTF_8));
	}


	if (first_vertex == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pas de premier sommet topo", TkUtil::Charset::UTF_8));
	Topo::Vertex* vtx1 = first_vertex;
	Topo::Vertex* vtx2 = 0;
	do {
//		std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
		// la liste des coedges à mettre dans une edge
		std::vector<Topo::CoEdge*> coedges_i;
		Topo::Vertex* vtx_i = vtx1;

		// accumulation des coedges pour une edge
		do {
//			std::cout<<"vtx_i = "<<vtx_i->getName()<<std::endl;
			std::vector<Topo::CoEdge*> coedges_loc;
			vtx_i->getCoEdges(coedges_loc);
//			std::cout<<"coedges_loc.size() = "<<coedges_loc.size()<<std::endl;
			// recherche d'une arête non marquée
			Topo::CoEdge* coedge = 0;
			for (uint i=0; i<coedges_loc.size(); i++)
				if (filtre_coedges[coedges_loc[i]] == 1)
					coedge = coedges_loc[i];
			if (coedge == 0)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, plus d'arête lors du parcours de proche en proche", TkUtil::Charset::UTF_8));
			filtre_coedges[coedge] = 2;
			coedges_i.push_back(coedge);
			vtx2 = coedge->getOppositeVertex(vtx_i);
//			std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
			vtx_i = vtx2;
		} while (filtre_vertices[vtx2] != 1);

//		std::cout<<"coedges_i.size() = "<<coedges_i.size()<<std::endl;

		Topo::Edge* edge = new Topo::Edge(m_context, vtx1, vtx2, coedges_i);
		edges.push_back(edge);

		vtx1 = vtx2;

	} while (first_vertex != vtx1);

//	std::cout<<"getEdges, edges.size() = "<<edges.size()<<std::endl;
	return edges;
}
/*----------------------------------------------------------------------------*/
Topo::Edge* ServiceGeomToTopo::getEdge(Geom::Curve* curve)
{
    Topo::Edge* edge = m_curve_edge[curve];

    if (edge == 0){


        // utilisation si possible d'une topologie s'il y en a une
        const std::vector<Topo::TopoEntity* >& topos = curve->getRefTopo();
        if (topos.size() > 1){

            // il faut récupérer les différentes coedges qui pointent sur cette courbe
            std::vector<Topo::CoEdge* > coedges;

            for (uint i=0; i<topos.size(); i++){
                Topo::TopoEntity* te = topos[i];
                if (te->getDim() == 1){
                    Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(te);
                    if (coedge == 0)
                        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer l'arête", TkUtil::Charset::UTF_8));

                    coedges.push_back(coedge);
                }
            } // end for i

            std::vector<Geom::Vertex*> vertices;
            curve->get(vertices);

            if (vertices.size() == 0 || vertices.size() > 2)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, une courbe avec autre chose que 1 ou 2 Vertex", TkUtil::Charset::UTF_8));

            // il se peut que ge1 == ge2
            Geom::Vertex* ge1 = vertices.front();
            Geom::Vertex* ge2 = vertices.back();

            edge = new Topo::Edge(m_context, getVertex(ge1), getVertex(ge2), coedges);

        }
        else if (topos.size() == 1){
            Topo::TopoEntity* te = topos[0];
            if (te->getDim() != 1)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, l'entité topologique qui pointe sur une courbe n'est pas de dimension 1", TkUtil::Charset::UTF_8));

            Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(te);
            if (coedge == 0)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer l'arête", TkUtil::Charset::UTF_8));

            coedge->saveCoEdgeTopoProperty(m_icmd);

            edge = new Topo::Edge(m_context, coedge);

        }
        else {
            std::vector<Geom::Vertex*> vertices;
            curve->get(vertices);

            if (vertices.size() == 0 || vertices.size() > 2){
                std::cerr<<"vertices.size() = "<<vertices.size()<<std::endl;
                std::cerr<<"Curve : "<<curve->getName()<<std::endl;
                std::cerr<<"Vertex : ";
                for (uint i=0; i<vertices.size(); i++)
                    std::cerr<<vertices[i]->getName()<<", ";
                std::cerr<<std::endl;
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, une courbe avec autre chose que 1 ou 2 Vertex", TkUtil::Charset::UTF_8));
            }

            Topo::EdgeMeshingPropertyUniform emp(m_context.getLocalTopoManager().getDefaultNbMeshingEdges());

            Geom::Vertex* ge1 = vertices.front();
            Geom::Vertex* ge2 = vertices.back();

            Topo::CoEdge* coedge = new Topo::CoEdge(m_context, &emp, getVertex(ge1), getVertex(ge2));
            m_icmd->addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);
            coedge->setGeomAssociation(curve);

            edge = new Topo::Edge(m_context, coedge);

        }

        m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);

        m_curve_edge[curve] = edge;

    } // end if (edge == 0)

    return edge;
}
/*----------------------------------------------------------------------------*/
Topo::CoEdge* ServiceGeomToTopo::getCoEdge(Geom::Curve* curve)
{
	Topo::CoEdge* coedge = 0;
	const std::vector<Topo::TopoEntity* >& topos = curve->getRefTopo();
	if (topos.size() == 1){
		Topo::TopoEntity* te = topos[0];
		if (te->getDim() != 1)
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, l'entité topologique qui pointe sur une courbe n'est pas de dimension 1", TkUtil::Charset::UTF_8));

		coedge = dynamic_cast<Topo::CoEdge*>(te);
		if (coedge == 0)
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer l'arête", TkUtil::Charset::UTF_8));
	}
	else {
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer l'arête commune existante", TkUtil::Charset::UTF_8));
	}
	return coedge;
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* ServiceGeomToTopo::getVertex(Geom::Vertex* gv)
{
    Topo::Vertex* tv = m_som_vtx[gv];

    if (tv == 0){

        // utilisation si possible d'une topologie s'il y en a une
        const std::vector<Topo::TopoEntity* >& topos = gv->getRefTopo();
        if (topos.size() > 1)
            throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, plusieurs entités topologiques pointent sur un sommet géométrique", TkUtil::Charset::UTF_8));
        else if (topos.size() == 1){
            Topo::TopoEntity* te = topos[0];
            if (te->getDim() != 0)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, l'entité topologique qui pointe sur un sommet géométrique n'est pas de dimension 0", TkUtil::Charset::UTF_8));

            tv = dynamic_cast<Topo::Vertex*>(te);
            if (tv == 0)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, pb pour récupérer le sommet topologique", TkUtil::Charset::UTF_8));

            tv->saveVertexTopoProperty(m_icmd);
        }
        else {
            tv = new Topo::Vertex(m_context, gv->getCoord());
            m_icmd->addTopoInfoEntity(tv, Internal::InfoCommand::CREATED);
            tv->setGeomAssociation(gv);
        }
        m_som_vtx[gv] = tv;

    } // end if (tv == 0)

    return tv;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
