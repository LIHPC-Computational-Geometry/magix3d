/*----------------------------------------------------------------------------*/
/*
 * \file TopoHelper.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13 mars 2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <map>
#include <set>
#include <vector>
#include "Topo/Edge.h"
#include "Topo/Face.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"

#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/GeomEntity.h"

#include <TkUtil/Exception.h>
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/MgxNumeric.h"
#include <TkUtil/MemoryError.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*------------------------------------------------------------------------*/
TopoHelper::TopoHelper( )
{
    MGX_FORBIDDEN("TopoHelper::TopoHelper is not allowed.");
}   // TopoHelper::TopoHelper

/*----------------------------------------------------------------------------*/
TopoHelper::TopoHelper(const TopoHelper&)
{
    MGX_FORBIDDEN("TopoHelper::TopoHelper is not allowed.");
}   // TopoHelper::TopoHelper(const TopoHelper&)

/*----------------------------------------------------------------------------*/
TopoHelper& TopoHelper::operator = (const TopoHelper&)
{
    MGX_FORBIDDEN("TopoHelper::operator = is not allowed.");
    return *this;
}   // TopoHelper::operator =

/*----------------------------------------------------------------------------*/
TopoHelper::~TopoHelper ( )
{
    MGX_FORBIDDEN("TopoHelper::~TopoHelper is not allowed.");
}   // TopoHelper::~TopoHelper

/*------------------------------------------------------------------------*/
const uint TopoHelper::tabIndVtxByFaceOnBlock[6][4] = {{4, 0, 2, 6},
        {5, 1, 3, 7},
        {4, 0, 1, 5},
        {6, 2, 3, 7},
        {2, 0, 1, 3},
        {6, 4, 5, 7}};
/*------------------------------------------------------------------------*/
const uint TopoHelper::tabIndVtxByFaceOnDegBlock[5][4] = {{4, 0, 2, 5},
        {4, 1, 3, 5},
        {4, 0, 1, 5},
        {4, 2, 3, 5},
        {2, 0, 1, 3}};
/*------------------------------------------------------------------------*/
const uint TopoHelper::tab2IndVtxByFaceOnBlock[6][4] = {{4, 0, 6, 2},
        {7, 3, 5, 1},
        {5, 1, 4, 0},
        {6, 2, 7, 3},
        {2, 0, 3, 1},
        {7, 5, 6, 4}};
/*------------------------------------------------------------------------*/
const uint TopoHelper::tabIndVtxByEdgeAndDirOnBlock[3][4][2] = {
        {{0,1}, {2,3}, {4,5}, {6,7}},
        {{0,2}, {1,3}, {4,6}, {5,7}},
        {{0,4}, {1,5}, {2,6}, {3,7}}};
/*------------------------------------------------------------------------*/
const uint TopoHelper::tabIndVtxByEdgeOnFace[4][2] = {{1, 0}, {1, 2}, {2, 3}, {0, 3}};
/*------------------------------------------------------------------------*/
const uint TopoHelper::tabIndVtxByEdgeOnDegFace[3][2] = {{1, 0}, {1, 2}, {2, 0}};
/*------------------------------------------------------------------------*/
const uint TopoHelper::tabIndVtxByEdgeOnAlternateFace[4][2] = {{0, 1}, {3, 2}, {0, 3}, {1, 2}};
/*------------------------------------------------------------------------*/
/// classe d'intéret local pour le stockage des infos lors d'un split d'une face commune
class InfoSplit{
public:
    uint dirCoFaceSplit;
    std::vector<CoEdge*> coedge_dep; // il peut y avoir différentes arêtes communes de coupées
    Edge* edge_dep;
    std::vector<uint> nbMeshingEdges_dep; // une arête peut être coupée plusieurs fois
    std::vector<CoEdge*> coedge_ar;
    Edge* edge_ar;
    std::vector<uint> nbMeshingEdges_ar;
};

/*----------------------------------------------------------------------------*/
//#define _DEBUG_getCoEdgesBetweenVertices
bool TopoHelper::getCoEdgesBetweenVertices(Vertex* v1, Vertex* v2,
        const std::vector<CoEdge* >& coedges,
        std::vector<CoEdge* >& coedges_between)
{
    coedges_between.clear();
#ifdef _DEBUG_getCoEdgesBetweenVertices
    std::cout<<"getCoEdgesBetweenVertices("<<v1->getName()<<", "<<v2->getName()<<") avec les coedges :"<<std::endl;;
#endif
    if (v1 == v2)
        return true; // ok, mais liste vide

    // on marque tous les coedges
    std::map<CoEdge*, uint> filtre_aretes;
    for (std::vector<CoEdge* >::const_iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
#ifdef _DEBUG_getCoEdgesBetweenVertices
        //std::cout<<" "<<(*iter)->getName();
        std::cout<<" "<<(**iter);
#endif
        filtre_aretes[*iter] = 1;
    }
#ifdef _DEBUG_getCoEdgesBetweenVertices
    std::cout<<std::endl;
#endif

    // on part d'un sommet v1 et on avance jusquà trouver le sommet v2
    Vertex* v_dep = v1;
    Vertex* v_suiv = 0;

#ifdef _DEBUG_getCoEdgesBetweenVertices
    std::cout<<"Sélectionne les CoEdges ";
#endif
    bool relanceFaite = false;
    do {
        // recherche parmis les arêtes reliées à v_dep, une arête marquée à 1
        const std::vector<CoEdge* > & v_coedges = v_dep->getCoEdges();
        CoEdge* coedge = 0;
        for (std::vector<CoEdge* >::const_iterator iter = v_coedges.begin();
                iter != v_coedges.end() && coedge==0; ++iter)
            if (filtre_aretes[*iter] == 1) {
                filtre_aretes[*iter] = 2;
                coedge = *iter;
            }
        if (0 == coedge){
            coedges_between.clear();
            // on essait de relancé depuis le sommet de départ
            if (relanceFaite == false){
#ifdef _DEBUG_getCoEdgesBetweenVertices
                std::cout<<" Erreur on relance, ";
#endif
                relanceFaite = true;
                v_dep = v1;
                continue;
            }
            else {
#ifdef _DEBUG_getCoEdgesBetweenVertices
                std::cout<<" Erreur .... "<<std::endl;
#endif
                return false;
            }
        }
        coedges_between.push_back(coedge);
#ifdef _DEBUG_getCoEdgesBetweenVertices
        std::cout<<", "<<coedge->getName();
#endif
        v_suiv = coedge->getOppositeVertex(v_dep);
        v_dep = v_suiv;

        if (v_dep == 0){
#ifdef _DEBUG_getCoEdgesBetweenVertices
        	std::cout<<" Erreur (pas de sommet trouvé) .... "<<std::endl;
#endif
        	return false;
        }
    } while (v_suiv != v2);
#ifdef _DEBUG_getCoEdgesBetweenVertices
    std::cout<<std::endl;
#endif

    return true;
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getInnerVertices(std::vector<CoEdge* >& coedges, std::vector<Topo::Vertex*>& innerVertices)
{
	if (coedges.size()<2)
		return;

	for (uint i=0; i<coedges.size()-1; i++){
		Vertex* vtx_dep = getCommonVertex(coedges[i], coedges[i+1]);
		innerVertices.push_back(vtx_dep);
	}
}
/*----------------------------------------------------------------------------*/
uint TopoHelper::getInternalDir(Vertex* v1, Vertex* v2,
        std::vector<CoEdge* >& iCoedges,
        std::vector<CoEdge* >& jCoedges)
{
    // on recherche un chemin parmis les 2 listes de CoEdges pour en déduire la direction
    std::vector<CoEdge* > coedges_between;

    bool sens12_i_ok = TopoHelper::getCoEdgesBetweenVertices(v1, v2, iCoedges, coedges_between);
    uint nbCoedges12_i = coedges_between.size();

    bool sens12_j_ok = TopoHelper::getCoEdgesBetweenVertices(v1, v2, jCoedges, coedges_between);
    uint nbCoedges12_j = coedges_between.size();

    if (sens12_i_ok && sens12_j_ok){
        return 2;
//        if (nbCoedges12_i<nbCoedges12_j)
//            return 0;
//        else if (nbCoedges12_i>nbCoedges12_j)
//            return 1;
//        else {
//            std::cerr <<"getInternalDir("<<v1->getName()<<", "<<v2->getName()<<") avec iCoedges et jCoedges"<<std::endl;
//            std::cerr <<"iCoedges : ";
//            for (uint i=0; i<iCoedges.size(); i++)
//                std::cerr <<" "<< iCoedges[i]->getName();
//            std::cerr <<std::endl;
//            std::cerr <<"jCoedges : ";
//            for (uint i=0; i<jCoedges.size(); i++)
//                std::cerr <<" "<< jCoedges[i]->getName();
//            std::cerr <<std::endl;
//            throw TkUtil::Exception (TkUtil::UTF8String ("getInternalDir(...) ne trouve pas de plus court chemin (les 2 font la même longueur)");
//        }
    }
    else if (sens12_i_ok)
        return 0;
    else if (sens12_j_ok)
        return 1;
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("getInternalDir(...) ne trouve pas de chemin entre les 2 sommets", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getCoEdges(const std::vector<Edge* > & edges,
        std::vector<CoEdge* > & coedges)
{
    for (std::vector<Edge* >::const_iterator iter = edges.begin();
                iter != edges.end(); ++iter){
        const std::vector<CoEdge* > & l_coedges = (*iter)->getCoEdges();
        coedges.insert(coedges.end(), l_coedges.begin(), l_coedges.end());
    }
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getCoFaces(const std::vector<Block* > & blocs,
            std::vector<CoFace* > & cofaces)
{
	std::list<CoFace*> liste;

	for (std::vector<Block* >::const_iterator iter = blocs.begin();
				iter != blocs.end(); ++iter){
			std::vector<CoFace* > l_cofaces;
			(*iter)->getCoFaces(l_cofaces);
			liste.insert(liste.end(), l_cofaces.begin(), l_cofaces.end());
		}

	liste.sort(Utils::Entity::compareEntity);
	liste.unique();

	cofaces.insert(cofaces.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getCoEdges(const std::vector<CoFace* > & faces,
            std::vector<CoEdge* > & coedges)
{
	std::list<CoEdge*> liste;

	for (std::vector<CoFace* >::const_iterator iter = faces.begin();
			iter != faces.end(); ++iter){
		std::vector<CoEdge* > l_coedges;
		(*iter)->getCoEdges(l_coedges, false);
		liste.insert(liste.end(), l_coedges.begin(), l_coedges.end());
	}

	liste.sort(Utils::Entity::compareEntity);
	liste.unique();

	coedges.insert(coedges.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getCoEdges(const std::vector<Block* > & blocs,
            std::vector<CoEdge* > & coedges)
{
	std::list<CoEdge*> liste;

	for (std::vector<Block* >::const_iterator iter = blocs.begin();
			iter != blocs.end(); ++iter){
		std::vector<CoEdge* > l_coedges;
		(*iter)->getCoEdges(l_coedges, false);
		liste.insert(liste.end(), l_coedges.begin(), l_coedges.end());
	}

	liste.sort(Utils::Entity::compareEntity);
	liste.unique();

	coedges.insert(coedges.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getVertices(const std::vector<CoFace* > & faces,
    		std::vector<Vertex* > & vertices)
{
	std::list<Vertex*> liste;

	for (std::vector<CoFace* >::const_iterator iter = faces.begin();
			iter != faces.end(); ++iter){
		std::vector<Vertex* > l_vertices;
		(*iter)->getAllVertices(l_vertices, false);
		liste.insert(liste.end(), l_vertices.begin(), l_vertices.end());
	}

	liste.sort(Utils::Entity::compareEntity);
	liste.unique();

	vertices.insert(vertices.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getVertices(const std::vector<Block* > & blocs,
    		std::vector<Vertex* > & vertices)
{
	std::list<Vertex*> liste;

	for (std::vector<Block* >::const_iterator iter = blocs.begin();
			iter != blocs.end(); ++iter){
		std::vector<Vertex* > l_vertices;
		(*iter)->getAllVertices(l_vertices, false);
		liste.insert(liste.end(), l_vertices.begin(), l_vertices.end());
	}

	liste.sort(Utils::Entity::compareEntity);
	liste.unique();

	vertices.insert(vertices.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Volume*>& volumes, std::set<Topo::Block*>& blocks)
{
	for (uint j=0; j<volumes.size(); j++){
		std::vector<Topo::Block*> loc_blocks;
		volumes[j]->get(loc_blocks);
		blocks.insert(loc_blocks.begin(), loc_blocks.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Volume*>& volumes, std::set<Topo::CoFace*>& cofaces, bool propagate)
{
	for (uint j=0; j<volumes.size(); j++){
		// les cofaces depuis les blocs
		std::vector<Topo::Block*> blocs;
		volumes[j]->get(blocs);
		Topo::TopoHelper::get(blocs, cofaces);

		if (propagate){
			// on parcours les surfaces pour les cas sans blocs ...
			std::vector<Geom::Surface*> surfaces;
			volumes[j]->get(surfaces);
			Topo::TopoHelper::get(surfaces, cofaces);
		}
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Volume*>& volumes, std::set<Topo::CoEdge*>& coedges, bool propagate)
{
	for (uint j=0; j<volumes.size(); j++){
		// les coedges depuis les blocs
		std::vector<Topo::Block*> blocs;
		volumes[j]->get(blocs);
		Topo::TopoHelper::get(blocs, coedges);

		if (propagate){
			// on parcours les surfaces pour les cas sans blocs ...
			std::vector<Geom::Surface*> surfaces;
			volumes[j]->get(surfaces);
			Topo::TopoHelper::get(surfaces, coedges, propagate);

			// on parcours les courbes pour les cas sans faces ...
			std::vector<Geom::Curve*> curves;
			volumes[j]->get(curves);
			Topo::TopoHelper::get(curves, coedges);
		}
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Volume*>& volumes, std::set<Topo::Vertex*>& vertices, bool propagate)
{
	for (uint j=0; j<volumes.size(); j++){
		// les vertices depuis les blocs
		std::vector<Topo::Block*> blocs;
		volumes[j]->get(blocs);
		Topo::TopoHelper::get(blocs, vertices);

		if (propagate){
			// on parcours les surfaces pour les cas sans blocs ...
			std::vector<Geom::Surface*> surfaces;
			volumes[j]->get(surfaces);
			Topo::TopoHelper::get(surfaces, vertices, propagate);

			// on parcours les courbes pour les cas sans faces ...
			std::vector<Geom::Curve*> curves;
			volumes[j]->get(curves);
			Topo::TopoHelper::get(curves, vertices, propagate);

			// on parcours les sommets pour les cas sans arêtes ...
			std::vector<Geom::Vertex*> vtx;
			volumes[j]->get(vtx);
			Topo::TopoHelper::get(vtx, vertices);
		}
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Surface*>& surfaces, std::set<Topo::CoFace*>& cofaces)
{
	for (uint k=0; k<surfaces.size(); k++){
		std::vector<Topo::CoFace* > loc_cofaces;
		surfaces[k]->get(loc_cofaces);
		cofaces.insert(loc_cofaces.begin(), loc_cofaces.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Surface*>& surfaces, std::set<Topo::CoEdge*>& coedges, bool propagate)
{
	for (uint j=0; j<surfaces.size(); j++){
		// les coedges depuis les faces
		std::vector<Topo::CoFace*> cofaces;
		surfaces[j]->get(cofaces);
		Topo::TopoHelper::get(cofaces, coedges);

		if (propagate){
			// on parcours les courbes pour les cas sans faces ...
			std::vector<Geom::Curve*> curves;
			surfaces[j]->get(curves);
			Topo::TopoHelper::get(curves, coedges);
		}
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Surface*>& surfaces, std::set<Topo::Vertex*>& vertices, bool propagate)
{
	for (uint j=0; j<surfaces.size(); j++){

		// les vertices depuis les faces
		std::vector<Topo::CoFace*> cofaces;
		surfaces[j]->get(cofaces);
		Topo::TopoHelper::get(cofaces, vertices);

		if (propagate){
			// on parcours les courbes pour les cas sans faces ...
			std::vector<Geom::Curve*> curves;
			surfaces[j]->get(curves);
			Topo::TopoHelper::get(curves, vertices, propagate);

			// on parcours les sommets pour les cas sans arêtes ...
			std::vector<Geom::Vertex*> vtx;
			surfaces[j]->get(vtx);
			Topo::TopoHelper::get(vtx, vertices);
		}
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Curve*>& curves, std::set<Topo::CoEdge*>& coedges)
{
	for (uint k=0; k<curves.size(); k++){
		std::vector<Topo::CoEdge* > loc_coedges;
		curves[k]->get(loc_coedges);
		coedges.insert(loc_coedges.begin(), loc_coedges.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Curve*>& curves, std::set<Topo::Vertex*>& vertices, bool propagate)
{
	for (uint k=0; k<curves.size(); k++){
		std::vector<Topo::CoEdge* > loc_coedges;
		curves[k]->get(loc_coedges);
		Topo::TopoHelper::get(loc_coedges, vertices);

		if (propagate){
			// on parcours les sommets pour les cas sans arêtes ...
			std::vector<Geom::Vertex*> vtx;
			curves[k]->get(vtx);
			Topo::TopoHelper::get(vtx, vertices);
		}
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Geom::Vertex*>& vtx, std::set<Topo::Vertex*>& vertices)
{
	for (uint k=0; k<vtx.size(); k++){
		std::vector<Topo::Vertex* > loc_vertices;
		vtx[k]->get(loc_vertices);
		vertices.insert(loc_vertices.begin(), loc_vertices.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Topo::Block*>& blocks, std::set<Topo::CoFace*>& cofaces)
{
	for (uint k=0; k<blocks.size(); k++){
		std::vector<Topo::CoFace* > loc_cofaces;
		blocks[k]->getCoFaces(loc_cofaces);
		cofaces.insert(loc_cofaces.begin(), loc_cofaces.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Topo::Block*>& blocks, std::set<Topo::CoEdge*>& coedges)
{
	for (uint k=0; k<blocks.size(); k++){
		std::vector<Topo::CoEdge* > loc_coedges;
		blocks[k]->getCoEdges(loc_coedges);
		coedges.insert(loc_coedges.begin(), loc_coedges.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Topo::Block*>& blocks, std::set<Topo::Vertex*>& vertices)
{
	for (uint k=0; k<blocks.size(); k++){
		std::vector<Topo::Vertex* > loc_vertices;
		blocks[k]->getVertices(loc_vertices);
		vertices.insert(loc_vertices.begin(), loc_vertices.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Topo::CoFace*>& cofaces, std::set<Topo::CoEdge*>& coedges)
{
	for (uint k=0; k<cofaces.size(); k++){
		std::vector<Topo::CoEdge* > loc_coedges;
		cofaces[k]->getCoEdges(loc_coedges);
		coedges.insert(loc_coedges.begin(), loc_coedges.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Topo::CoFace*>& cofaces, std::set<Topo::Vertex*>& vertices)
{
	for (uint k=0; k<cofaces.size(); k++){
		std::vector<Topo::Vertex* > loc_vertices;
		cofaces[k]->getVertices(loc_vertices);
		vertices.insert(loc_vertices.begin(), loc_vertices.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::get(std::vector<Topo::CoEdge*>& coedges, std::set<Topo::Vertex*>& vertices)
{
	for (uint k=0; k<coedges.size(); k++){
		std::vector<Topo::Vertex* > loc_vertices;
		coedges[k]->getVertices(loc_vertices);
		vertices.insert(loc_vertices.begin(), loc_vertices.end());
	}
}
/*----------------------------------------------------------------------------*/
void TopoHelper::permuteVector(std::vector<CoEdge* > & coedges1,
        std::vector<CoEdge* > & coedges2)
{
    std::vector<CoEdge* > coedges_tmp = coedges1;
    coedges1 = coedges2;
    coedges2 = coedges_tmp;
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getVerticesTip(const std::vector<CoEdge* > & coedges, Vertex* &v1, Vertex* &v2, bool isSorted)
{
#ifdef _DEBUG2
	std::cout<<"getVerticesTip isSorted = "<<(isSorted?"vrai":"faux")<<std::endl;
	for (uint i=0; i<coedges.size(); i++)
		std::cout<<*coedges[i]<<std::endl;
#endif
	if (coedges.size() == 1){
        CoEdge* coedge = coedges[0];
        v1 = coedge->getVertex(0);
        v2 = coedge->getVertex(1);
    }
    else if (coedges.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("getVerticesTip(...) ne peut se faire sans au moins une CoEdge", TkUtil::Charset::UTF_8));
    else if (isSorted){
        // recherche du sommet de départ
        if (coedges[0]->getVertex(1) == coedges[1]->getVertex(0)
                || coedges[0]->getVertex(1) == coedges[1]->getVertex(1))
            v1 = coedges[0]->getVertex(0);
        else if (coedges[0]->getVertex(0) == coedges[1]->getVertex(0)
                || coedges[0]->getVertex(0) == coedges[1]->getVertex(1))
            v1 = coedges[0]->getVertex(1);
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("getVerticesTip(...) ne trouve pas de sommet commun entre les 2 premières", TkUtil::Charset::UTF_8));

        // recherche du dernier sommet opposé au sommet de départ, de proche en proche
        v2 = v1;
        for (uint i=0; i<coedges.size(); i++){
            v2 = coedges[i]->getOppositeVertex(v2);
        }
    }
    else {
#ifdef _DEBUG2
	std::cout<<" utilisation de la map "<<std::endl;
#endif
        // cas où les coedges ne sont pas triées, on utilise une map
        std::map<Vertex*, uint> filtre_vtx;
        for (uint i=0; i<coedges.size(); i++)
            for (uint j=0; j<2; j++)
                filtre_vtx[coedges[i]->getVertex(j)] += 1;

        // cette map doit contenir 2 vertex vus une seul fois
        uint nb_vtx = 0;
        for (std::map<Vertex*, uint>::iterator iter = filtre_vtx.begin();
                iter != filtre_vtx.end(); ++iter){
            uint nb_vu = iter->second;
            Vertex* vtx = iter->first;
            if (nb_vu == 1){
                if (0 == nb_vtx)
                    v1 = vtx;
                else if (1 == nb_vtx)
                    v2 = vtx;
                nb_vtx++;
            }
        }
        if (nb_vtx != 2)
            throw TkUtil::Exception (TkUtil::UTF8String ("getVerticesTip(...) ne trouve pas uniquement 2 sommets", TkUtil::Charset::UTF_8));
    }
#ifdef _DEBUG2
        std::cout<<"getVerticesTip trouve v1: "<<v1->getName()<<std::endl;
        std::cout<<"getVerticesTip trouve v2: "<<v2->getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT
void TopoHelper::splitFaces(std::vector<CoFace* > cofaces,
        CoEdge* arete, double ratio_dec, double ratio_ogrid,
        bool boucleDemandee, bool rebondAutorise,
        std::vector<Edge* > &splitingEdges,
        Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"splitFaces(...) avec "<< cofaces.size()<<" cofaces, arete = "<<arete->getName()
            <<", ratio_dec = "<< ratio_dec
            <<", ratio_ogrid = "<< ratio_ogrid
            <<", boucleDemandee = "<< boucleDemandee
            << std::endl;
#endif

    // on marque les CoFaces autorisées à 1,
    // et on incrémente à chaque fois que l'on sélectionne la face
    std::map<CoFace*, uint> filtre_cofaces;
    for (std::vector<CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter)
        filtre_cofaces[*iter] = 1;


    // on recherche le nombre de bras de maillage correspondant au ratio
    // (on ne tient pas compte du type de discrétisation, seulement le nombre de bras)
    uint nbMeshingEdges_dep = (uint)((double)arete->getNbMeshingEdges()*ratio_dec+0.5);
    CoEdge* coedge_dep = arete;

    // on part d'une CoEdge et d'un nombre de bras de maillage à mettre entre le premier sommet et la coupe,
    // on sélectionne une CoFace adjacente parmi la sélection
    // on recherche la CoEdge en face et le nombre de bras de maillage similaire au cas précédent
    bool boucleTerminee = false;

    // dans le cas dégénéré, il sera peut-être nécessaire de refaire un démarage de l'algo au point de départ,
    // car on ne peut pas forcément boucler (cas d'une face dégénérée à 3 côtés)
    bool boucleRelancee = false;
    bool tenteRelance = false;

    // on fait une première passe pour repérer les arêtes
    // on stocke le résultat
    std::map<uint, InfoSplit*> cofaceInfoSplit;
    std::map<uint, CoFace*> id2coface;
    CoFace* coface_prec = 0;
    // on repère la première coface vue pour ne pas la reprendre en cas de reprise dans l'autre sens
    CoFace* coface_begin = 0;

    /* mémorise si l'arête de départ est après la coupe ou non
     * nécessaire lorque l'on est à une extrémité d'une arête pour commencer
     * ce qui est le cas si cette arête a déjà été coupée
     */
    bool need_find_coedge = (ratio_dec == 0.0 || ratio_dec == 1.0);
    bool coedge_after_cut = false;
    bool init_coedge_after_cut = true;
    bool sens_coedge_dep = true;
    bool sens_coedge_ar = true;

    while (!boucleTerminee){
        uint nbMeshingEdges_ar;
        CoEdge* coedge_ar = 0;

        // vrai si l'on souhaite reprendre cette face comme s'il y avait une symétrie
        // utile pour le cas où l'on arrive sur une face dégénérée par un côté qui ne fait pas face a la dégénérescence
        bool reprendreCetteFace = false;

#ifdef _DEBUG_SPLIT
        std::cout<<std::endl;
        std::cout<<"nbMeshingEdges_dep = "<<nbMeshingEdges_dep<<std::endl;
        std::cout<<"coedge_dep = "<<coedge_dep->getName()<<std::endl;
        std::cout<<"coface_prec = "<<(coface_prec?coface_prec->getName():"0")<<std::endl;
#endif

        std::vector<CoFace* > loc_cofaces;
        coedge_dep->getCoFaces(loc_cofaces);

        // on en recherche une parmi la sélection et qui ne soit pas encore vue
        // dans le cas d'une boucle (faces d'un bloc) on ne veut prendre les faces qu'une fois
        // dans le cas sans boucle, on veut juste passer d'une face à la voisine avec un retour possible
        // lors du passage par un point au centre d'une boule
        CoFace* coface = 0;
        for (std::vector<CoFace* >::iterator iter = loc_cofaces.begin();
                iter != loc_cofaces.end() && 0 == coface; ++iter){
            if (filtre_cofaces[*iter] == 1 && coface_prec != *iter)
                coface = *iter;
        }

        if (0 == coface){
            // détection du cas d'une arête dans le prolongement de l'arête de départ
            // ce qui apparait dans le cas d'une face déjà coupée
            // mais qui n'a pas été détecté par coedge_dep == arete
            // ou une terminaison pour une coupe localisée
            if (coedge_dep->getVertex(0) == arete->getVertex(0)
                    || coedge_dep->getVertex(0) == arete->getVertex(1)
                    || coedge_dep->getVertex(1) == arete->getVertex(0)
                    || coedge_dep->getVertex(1) == arete->getVertex(1)
                    || !boucleDemandee){
                boucleTerminee = true;
                continue;
            }
            else
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Topo::splitFaces ne retrouve pas de face commune pour avancer", TkUtil::Charset::UTF_8));
        }
#ifdef _DEBUG_SPLIT
        std::cout<<"coface trouvée : "<<coface->getName()<<std::endl;
#endif
        // pour ne pas réutiliser cette coface
        if (boucleDemandee)
            filtre_cofaces[coface] = 2;
        else if (0 == coface_begin)
            coface_begin = coface;

        // recherche du côté dans lequel est cette arête, et le nombre de bras de maillage jusqu'à la coupe
        Edge* edge_dep = coface->getEdgeContaining(coedge_dep);
        uint ind_edge_dep = coface->getIndex(edge_dep);
#ifdef _DEBUG_SPLIT
        std::cout<<"edge_dep = "<<edge_dep->getName()<<std::endl;
#endif

        // direction de la face qui est coupée
        CoFace::eDirOnCoFace dirCoFaceSplit = (ind_edge_dep%2?CoFace::i_dir:CoFace::j_dir);
#ifdef _DEBUG_SPLIT
        std::cout<<"dirCoFaceSplit = "<<(dirCoFaceSplit==CoFace::i_dir?"i_dir":"j_dir")<<std::endl;
#endif

        Vertex* vertex1;
        Vertex* vertex2;
        if (dirCoFaceSplit == CoFace::i_dir){
            vertex1 = coface->getVertex(1);
            vertex2 = coface->getVertex(0);
        } else {
            vertex1 = coface->getVertex(1);
            vertex2 = coface->getVertex(2);
        }

        if (ind_edge_dep>1){
            Vertex* vertex_tmp = vertex1;
            vertex1 = vertex2;
            vertex2 = vertex_tmp;
        }

        // calcul le nombre de bras entre le sommet de départ et un noeud sur une CoEdge
        uint nbMeshingEdges_edge;
        edge_dep->computeCorrespondingNbMeshingEdges(vertex1, coedge_dep, nbMeshingEdges_dep,
                nbMeshingEdges_edge, sens_coedge_dep);

        if (need_find_coedge){
        	if (init_coedge_after_cut){
        		coedge_after_cut = (nbMeshingEdges_dep != coedge_dep->getNbMeshingEdges());
        		if (!sens_coedge_dep)
        			coedge_after_cut = !coedge_after_cut;
        		// pour ne faire l'initialisation qu'une fois
        		init_coedge_after_cut = false;
        	}
        	else {
        		if (sens_coedge_dep != sens_coedge_ar)
        			coedge_after_cut = !coedge_after_cut;
        	}
        }

#ifdef _DEBUG_SPLIT
        std::cout<<"nbMeshingEdges_dep =  "<<nbMeshingEdges_dep<<std::endl;
        std::cout<<"nbMeshingEdges_edge =  "<<nbMeshingEdges_edge<<std::endl;
        std::cout<<"sens_coedge_dep = "<<(sens_coedge_dep?"vrai":"faux")<<std::endl;
        std::cout<<"vertex1 = "<<vertex1->getName()<<std::endl;
        std::cout<<"vertex2 = "<<vertex2->getName()<<std::endl;
        if (need_find_coedge)
        	std::cout<<"coedge_after_cut = "<<(coedge_after_cut?"vrai":"faux")<<std::endl;
#endif

        Edge* edge_ar = 0;
        // si on est sur côté face à la dégénérescence, on met à 0 l'arête d'arrivée
        // ce qui permet de signaler un redémarage nécessaire
        if (coface->getNbEdges() == 4){
            uint ind_edge_ar = (ind_edge_dep+2)%4;
            edge_ar = coface->getEdge(ind_edge_ar);
        } else {
        	if (boucleDemandee){
        		// cas 3D
        		if (ind_edge_dep == 0)
        			edge_ar = coface->getEdge(2);
        		else if (ind_edge_dep == 2)
        			edge_ar = coface->getEdge(0);
        		else
        			edge_ar = 0;
        	}
        	else {
        		// cas 2D
        		edge_ar = 0;
        		if (ind_edge_dep != 1)
        			reprendreCetteFace = true;

        		// on test pour détecter le cas d'une zone avec 1 seul sommet sur l'axe
        		uint nb_sur_axe = 0;
                if (Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(0)->getY()))
                	nb_sur_axe++;
                if (Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(1)->getY()))
                	nb_sur_axe++;
                if (Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(2)->getY()))
                	nb_sur_axe++;
                if (nb_sur_axe == 1){
    				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                	messErr << "Echec lors du découpage de la face "<<coface->getName()
                			<<", elle n'a qu'un unique sommet sur l'axe, ce qui n'est pas compatible avec le passage en blocs structurés en 3D";
                	throw TkUtil::Exception(messErr);
                }
        	}
        }

        // réciproque: recherche la CoEdge touchée par le noeud
        if (0 != edge_ar){
            edge_ar->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex2, nbMeshingEdges_edge, coedge_after_cut,
                    coedge_ar, nbMeshingEdges_ar, sens_coedge_ar);

#ifdef _DEBUG_SPLIT
            std::cout<<"nbMeshingEdges_ar =  "<<nbMeshingEdges_ar<<std::endl;
            std::cout<<"coedge_ar =  "<<coedge_ar->getName()<<std::endl;
            std::cout<<"edge_ar =  "<<edge_ar->getName()<<std::endl;
            std::cout<<"sens_coedge_ar = "<<(sens_coedge_ar?"vrai":"faux")<<std::endl;
#endif
        }
        else {
            nbMeshingEdges_ar = 0;
            coedge_ar = 0;
#ifdef _DEBUG_SPLIT
            std::cout<<"edge_ar et coedge_ar  == 0\n";
#endif
        }

        // on recherche si cette face est déjà coupée
        InfoSplit* is = cofaceInfoSplit[coface->getUniqueId()];

        if (is == 0){
        	if (coface->getNbEdges() == 3 && ind_edge_dep != 1 && !boucleDemandee){
        		// cas où on arrive sur face dégénérée par un côté autre que celui qui fait face à la dégénérescence
#ifdef _DEBUG_SPLIT
        		std::cout<<"on ne fait rien sur cette face en arrivant par ce côté\n";
        		std::cout<<"coedge_dep =  "<<(coedge_dep?coedge_dep->getName():"0")<<std::endl;
        		std::cout<<" on en profite pour recalculer nbMeshingEdges_edge ...\n";
        		std::cout<<"nbMeshingEdges_edge =  "<<nbMeshingEdges_edge<<std::endl;
        		std::cout<<"vertex1 =  "<<(vertex1?vertex1->getName():"0")<<std::endl;
        		std::cout<<"vertex2 =  "<<(vertex2?vertex2->getName():"0")<<std::endl;
#endif
        		// on va calculer nbMeshingEdges_edge pour la partie sous l'ogrid
        		if (!Utils::Math::MgxNumeric::isNearlyZero(vertex1->getY()))
        			nbMeshingEdges_edge = edge_dep->getNbMeshingEdges() - nbMeshingEdges_edge;
#ifdef _DEBUG_SPLIT
        		std::cout<<" => nbMeshingEdges_edge =  "<<nbMeshingEdges_edge<<std::endl;
#endif
        	}
        	else {
#ifdef _DEBUG_SPLIT
        		std::cout<<"# nouveau InfoSplit\n";
        		std::cout<<"coedge_dep =  "<<(coedge_dep?coedge_dep->getName():"0")<<std::endl;
        		std::cout<<"coedge_ar =  "<<(coedge_ar?coedge_ar->getName():"0")<<std::endl;
        		std::cout<<"nbMeshingEdges_dep =  "<<nbMeshingEdges_dep<<std::endl;
        		std::cout<<"nbMeshingEdges_ar =  "<<nbMeshingEdges_ar<<std::endl;
#endif
        		is = new InfoSplit();
        		is->dirCoFaceSplit = dirCoFaceSplit;
        		is->coedge_dep.push_back(coedge_dep);
        		is->edge_dep = edge_dep;
        		is->coedge_ar.push_back(coedge_ar);
        		is->edge_ar = edge_ar;
        		is->nbMeshingEdges_dep.push_back(nbMeshingEdges_dep);
        		is->nbMeshingEdges_ar.push_back(nbMeshingEdges_ar);

        		cofaceInfoSplit[coface->getUniqueId()] = is;
        		id2coface[coface->getUniqueId()] = coface;
        	}
        }
        else {
            // si la face a déjà été vu, c'est peut-être en parcourant dans l'autre sens, il faut peut-être inverser
        	uint ind_dep_0 = coface->getIndex(is->coedge_dep[0]);
        	uint ind_dep_1 = coface->getIndex(coedge_dep);
        	bool inverse = (ind_dep_0 != ind_dep_1);

#ifdef _DEBUG_SPLIT
            std::cout<<"InfoSplit complété "<<(inverse?"en inversant\n":"sans inverser\n");
            std::cout<<"coedge_dep =  "<<(coedge_dep?coedge_dep->getName():"0")<<std::endl;
            std::cout<<"coedge_ar =  "<<(coedge_ar?coedge_ar->getName():"0")<<std::endl;
            std::cout<<"nbMeshingEdges_dep =  "<<nbMeshingEdges_dep<<std::endl;
            std::cout<<"nbMeshingEdges_ar =  "<<nbMeshingEdges_ar<<std::endl;
#endif

            if (inverse){
            	is->coedge_dep.push_back(coedge_ar);
            	is->coedge_ar.push_back(coedge_dep);
            	is->nbMeshingEdges_dep.push_back(nbMeshingEdges_ar);
            	is->nbMeshingEdges_ar.push_back(nbMeshingEdges_dep);
            }
            else {
            	is->coedge_dep.push_back(coedge_dep);
            	is->coedge_ar.push_back(coedge_ar);
            	is->nbMeshingEdges_dep.push_back(nbMeshingEdges_dep);
            	is->nbMeshingEdges_ar.push_back(nbMeshingEdges_ar);
            }
        }

        // cas d'un "rebond" au niveau du centre d'une "boule"
        // c'est à dire une topologie équivalente à un demi-cercle, sur l'axe de symétrie
        if (0 == coedge_ar && rebondAutorise && coface->getNbEdges() == 3
                && Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(0)->getY())
                && Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(1)->getY())
                && Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(2)->getY()) ){
            // on coupe une deuxième fois l'arête

            // on prend le complément pour la coupe
            nbMeshingEdges_edge = edge_dep->getNbMeshingEdges() - nbMeshingEdges_edge;
            edge_dep->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex1, nbMeshingEdges_edge, false,
                                coedge_dep, nbMeshingEdges_dep, sens_coedge_ar);

            if (is == 0){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            	messErr << "Echec lors du découpage. Serait-ce une arête sur l'axe ? ";
            	throw TkUtil::Exception(messErr);
            }

#ifdef _DEBUG_SPLIT
            std::cout<<" détection d'un rebond ...\n";
            std::cout<<"nbMeshingEdges_edge =  "<<nbMeshingEdges_edge<<std::endl;
            std::cout<<"nbMeshingEdges_dep =  "<<nbMeshingEdges_dep<<std::endl;
            std::cout<<"nbMeshingEdges_ar =  "<<nbMeshingEdges_ar<<std::endl;
            std::cout<<"coedge_dep =  "<<(coedge_dep?coedge_dep->getName():"0")<<std::endl;
            std::cout<<"coedge_ar =  "<<(coedge_ar?coedge_ar->getName():"0")<<std::endl;
#endif

            is->coedge_dep.push_back(coedge_dep);
            is->coedge_ar.push_back(coedge_ar);
            is->nbMeshingEdges_dep.push_back(nbMeshingEdges_dep);
            is->nbMeshingEdges_ar.push_back(nbMeshingEdges_ar);

        }
        else if (0 == coedge_ar && rebondAutorise && coface->getNbEdges() == 3
                && Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(0)->getY())
                && (Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(1)->getY())
                 || Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(2)->getY())) ){
            // on est peut-être dans le cas d'un rebond avec 2 zones pour la "boule" centrale

#ifdef _DEBUG_SPLIT
        	std::cout<<" cas avec symétrie possible ...\n";
#endif
            //  on cherche l'arête qui couperait cette boule
            Edge* edge_sym = 0;
            for (uint i=0; i<3; i+=2){
                Edge* edge = coface->getEdge(i);
                // le 2ème sommet de l'arête qui n'est pas à la dégénérescence
                Vertex* vtx2 = edge->getOppositeVertex(coface->getVertex(0));
                if (!Utils::Math::MgxNumeric::isNearlyZero(vtx2->getY()))
                    edge_sym =  edge;
            }
#ifdef _DEBUG_SPLIT
            std::cout<<"edge_sym =  "<<(edge_sym?edge_sym->getName():"0")<<std::endl;
#endif
            if (0 != edge_sym && edge_sym->getNbCoEdges() == 1){
                CoEdge* coedge_sym = edge_sym->getCoEdge(0);
                // recherche de la face de l'autre côté de cette arête si elle existe
                CoFace* coface_sym = 0;
                std::vector<CoFace* > cofaces_vois;
                coedge_sym->getCoFaces(cofaces_vois);
                if (cofaces_vois.size() == 2){
                    if (coface == cofaces_vois[0])
                        coface_sym = cofaces_vois[1];
                    else
                        coface_sym = cofaces_vois[0];
                }
                if (reprendreCetteFace){
                	coface_sym = coface;
                	// dans ce cas il faut recalculer nbMeshingEdges_edge
                	nbMeshingEdges_edge = coface_sym->getEdge(1)->getNbMeshingEdges() - nbMeshingEdges_edge;
#ifdef _DEBUG_SPLIT
                	std::cout<<" cas où il faut adapter nbMeshingEdges_edge ...\n";
                	std::cout<<"   => nbMeshingEdges_edge =  "<<nbMeshingEdges_edge<<std::endl;
#endif


                } // end if reprendreCetteFace

#ifdef _DEBUG_SPLIT
                std::cout<<"coface_sym =  "<<(coface_sym?coface_sym->getName():"0")<<std::endl;
#endif
                if (coface_sym){
                	// on test pour détecter le cas d'une zone avec 1 seul sommet sur l'axe
                	uint nb_sur_axe = 0;
                	if (Utils::Math::MgxNumeric::isNearlyZero(coface_sym->getVertex(0)->getY()))
                		nb_sur_axe++;
                	if (Utils::Math::MgxNumeric::isNearlyZero(coface_sym->getVertex(1)->getY()))
                		nb_sur_axe++;
                	if (Utils::Math::MgxNumeric::isNearlyZero(coface_sym->getVertex(2)->getY()))
                		nb_sur_axe++;
                	if (nb_sur_axe == 1){
                		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                		messErr << "Echec lors du découpage de la face "<<coface_sym->getName()
                					<<", elle n'a qu'un unique sommet sur l'axe, ce qui n'est pas compatible avec le passage en blocs structurés en 3D";
                		throw TkUtil::Exception(messErr);
                	}
                }

                if (coface_sym && filtre_cofaces[coface_sym] == 1){
                	if (coface_sym->getNbEdges() == 3){

                		// on recherche dans cette face symétrique de la précédente
                		// la positition de la coupe

                		edge_ar = coface_sym->getEdge(1);

                		// recherche du sommet symétrique à vertex1 / edge_sym
                		if (edge_sym->find(vertex1))
                			vertex2 = vertex1;
                		else {
                			vertex2 = edge_dep->getOppositeVertex(vertex1);
                			vertex2 = edge_ar->getOppositeVertex(vertex2);
                		}

                		edge_ar->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex2, nbMeshingEdges_edge, false,
                				coedge_ar, nbMeshingEdges_ar, sens_coedge_ar);


                		nbMeshingEdges_dep = nbMeshingEdges_ar;
                		coedge_dep = coedge_ar;
                		edge_dep = edge_ar;

                		coedge_ar = 0;
                		nbMeshingEdges_ar = 0;
                		edge_ar = 0;

                		coface = coface_sym;
#ifdef _DEBUG_SPLIT
                		std::cout<<" détection d'un rebond avec arête de symétrie ...\n";
                        std::cout<<"vertex2 = "<<vertex2->getName()<<std::endl;
                		std::cout<<"# nouveau InfoSplit (2ème)\n";
                		std::cout<<"nbMeshingEdges_dep =  "<<nbMeshingEdges_dep<<std::endl;
                		std::cout<<"nbMeshingEdges_ar =  "<<nbMeshingEdges_ar<<std::endl;
                		std::cout<<"coedge_dep =  "<<(coedge_dep?coedge_dep->getName():"0")<<std::endl;
                		std::cout<<"coedge_ar =  "<<(coedge_ar?coedge_ar->getName():"0")<<std::endl;
#endif

                		is = new InfoSplit();
                		is->dirCoFaceSplit = dirCoFaceSplit;
                		is->coedge_dep.push_back(coedge_dep);
                		is->edge_dep = edge_dep;
                		is->coedge_ar.push_back(coedge_ar);
                		is->edge_ar = edge_ar;
                		is->nbMeshingEdges_dep.push_back(nbMeshingEdges_dep);
                		is->nbMeshingEdges_ar.push_back(nbMeshingEdges_ar);

                		cofaceInfoSplit[coface->getUniqueId()] = is;
                		id2coface[coface->getUniqueId()] = coface;
                	} // end if (coface_sym->getNbEdges() == 3)
                	else {

                		// recherche de l'arête hors de l'axe
                		if (Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(1)->getY()))
                			edge_ar = coface->getEdge(2);
                		else if (Utils::Math::MgxNumeric::isNearlyZero(coface->getVertex(2)->getY()))
                			edge_ar = coface->getEdge(0);

                		// si le sommet 1 n'est pas sur l'axe on prend le complément pour nbMeshingEdges_edge
                		if (!Utils::Math::MgxNumeric::isNearlyZero(vertex1->getY()))
                			nbMeshingEdges_edge = edge_dep->getNbMeshingEdges() - nbMeshingEdges_edge;

                		edge_ar->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex2, nbMeshingEdges_edge, false,
                				coedge_ar, nbMeshingEdges_ar, sens_coedge_ar);

#ifdef _DEBUG_SPLIT
                		std::cout<<" détection d'un passage par zone à 3 côtés et avec arête de symétrie ...\n";
                        std::cout<<"nbMeshingEdges_edge =  "<<nbMeshingEdges_edge<<std::endl;
                		std::cout<<"nbMeshingEdges_ar =  "<<nbMeshingEdges_ar<<std::endl;
                		std::cout<<"coedge_ar =  "<<(coedge_ar?coedge_ar->getName():"0")<<std::endl;
                		std::cout<<"vertex1 =  "<<(vertex1?vertex1->getName():"0")<<std::endl;
                		std::cout<<"vertex2 =  "<<(vertex2?vertex2->getName():"0")<<std::endl;
#endif
                		nbMeshingEdges_dep = nbMeshingEdges_ar;
                		coedge_dep = coedge_ar;

                		if (coedge_dep == arete)
                			boucleTerminee = true;
                	}
                } // end if (filtre_cofaces[coface_sym] == 1)
                else {
                	// il faut arréter de ce côté ...
                	tenteRelance = true;
                }
            } // end if (0 != edge_sym && edge_sym->getNbCoEdges() == 1)
        }
        else if (0 == coedge_ar || (!boucleDemandee && coedge_ar->getNbEdges() == 1 && coedge_ar->getNbCofaces() == 1)) { // terminaison ou relance ?
            // redémarrage
        	tenteRelance = true;
        }
        else { // cas par défaut, on continue à partir de l'arête d'arrivée (face à celle de départ)
#ifdef _DEBUG_SPLIT
            std::cout<<" on continue ..."<<std::endl;
#endif
            nbMeshingEdges_dep = nbMeshingEdges_ar;
            coedge_dep = coedge_ar;

            if (boucleDemandee && coedge_dep == arete)
                boucleTerminee = true;
        }

        if (tenteRelance){
        	tenteRelance = false;
#ifdef _DEBUG_SPLIT
            std::cout<<" on tente de relancer ..."<<std::endl;
#endif
            if (boucleRelancee){
#ifdef _DEBUG_SPLIT
                std::cout<<" cela se termine ..."<<std::endl;
#endif
                // on ne relance qu'une fois
                boucleTerminee = true;
            } else {
#ifdef _DEBUG_SPLIT
                std::cout<<" on relance ..."<<std::endl;
#endif
                boucleRelancee = true;
                nbMeshingEdges_dep = (uint)((double)arete->getNbMeshingEdges()*ratio_dec+0.5);
                coedge_dep = arete;
                coface = coface_begin;
            }

        }

        if (!boucleDemandee && coedge_dep->getNbEdges() == 1 && coedge_dep->getNbCofaces() == 1){
#ifdef _DEBUG_SPLIT
            std::cout<<" cela se termine vraiment ..."<<std::endl;
#endif
            boucleTerminee = true;
        }

        // pour éviter de revenir en arrière
        coface_prec = coface;

    } // while (!boucleTerminee)




    // map de stockage des Edges initiales et des 2 ou 3 arêtes qui découlent de la coupe
    std::map<Edge*, std::vector<Edge*> > old2newEdges;
    // map de stockage des CoEdges initiales et des nouveaux sommets créés
    std::map<CoEdge*, std::vector<Vertex*> > oldCoEdge2newVertices;

    // on parcours à nouveau les faces et arêtes à couper pour y faire le split
    for (std::map<uint, InfoSplit*>::iterator iter = cofaceInfoSplit.begin();
            iter != cofaceInfoSplit.end(); ++iter) {
        uint id = (*iter).first;
        CoFace* coface = id2coface[id];
        InfoSplit* is = (*iter).second;

        CHECK_NULL_PTR_ERROR(is)

#ifdef _DEBUG_SPLIT
        std::cout<<std::endl;
        std::cout<<"coface découpée : "<<coface->getName()<<", unique id : "<<id<<std::endl;
        std::cout<<*coface;
        std::cout<<"coedge_dep :";
        for (uint i=0; i<is->coedge_dep.size(); i++)
            std::cout<<" "<<(is->coedge_dep[i]?is->coedge_dep[i]->getName():"undef");
        std::cout<<"\nedge_dep :"<<(is->edge_dep?is->edge_dep->getName():"undef");
        std::cout<<"\ncoedge_ar :";
        for (uint i=0; i<is->coedge_ar.size(); i++)
            std::cout<<" "<<(is->coedge_ar[i]?is->coedge_ar[i]->getName():"undef");
        std::cout<<"\nedge_ar :"<<(is->edge_ar?is->edge_ar->getName():"undef");
        std::cout<<std::endl;
#endif

        std::vector<Vertex*> newVertices1; // sommet inséré sur l'arête de départ
        std::vector<Vertex*> newVertices2; // sommet inséré sur l'arête suivante / départ
        std::vector<Vertex*> newVertices3; // sommet inséré sur l'arête opposée à celle de départ
        std::vector<Vertex*> newVertices4; // sommet inséré sur l'arête prédente / départ

        // choix de ni en fonction des autres coupes (nombre de bras entre axe et o-grid)
        uint ni=0;

        // 3 cas (dans l'ordre):
        // 1 coedge à couper une fois
        // 1 coedge à couper deux fois
        // 2 coedges à couper chacune une fois
        // optionnellement, si ratio_ogrid>0 et la face dégénérée, il faut couper les autres arêtes

        if (is->coedge_dep.size() == 1){
            // 1 coedge à couper une fois
            newVertices1 = oldCoEdge2newVertices[is->coedge_dep[0]];
            if (newVertices1.empty()){
                newVertices1 = is->coedge_dep[0]->split(is->nbMeshingEdges_dep, icmd);
                oldCoEdge2newVertices[is->coedge_dep[0]] = newVertices1;
            }
            CoEdge* coedge0 = newVertices1[0]->getCoEdge(0);
            CoEdge* coedge1 = newVertices1[0]->getCoEdge(1);
            // recherche de l'arête qui touche l'axe
            if (Utils::Math::MgxNumeric::isNearlyZero(coedge0->getOppositeVertex(newVertices1[0])->getY()))
            	ni = coedge0->getNbMeshingEdges();
            else
            	ni = coedge1->getNbMeshingEdges();

        } else if (is->coedge_dep.size() == 2){
            if (is->coedge_dep[0] == is->coedge_dep[1]){
                // 1 coedge à couper deux fois
                newVertices1 = oldCoEdge2newVertices[is->coedge_dep[0]];
                if (newVertices1.empty()){
                    newVertices1 = is->coedge_dep[0]->split(is->nbMeshingEdges_dep, icmd);
                    oldCoEdge2newVertices[is->coedge_dep[0]] = newVertices1;
                }
            }
            else {
                // 2 coedges à couper chacune une fois
                std::vector<Vertex*> vtx1 = oldCoEdge2newVertices[is->coedge_dep[0]];
                if (vtx1.empty()){
                    Vertex* vtx = is->coedge_dep[0]->split(is->nbMeshingEdges_dep[0], icmd);
                    vtx1.push_back(vtx);
                    oldCoEdge2newVertices[is->coedge_dep[0]] = vtx1;
                }
                std::vector<Vertex*> vtx2 = oldCoEdge2newVertices[is->coedge_dep[1]];
                if (vtx2.empty()){
                    Vertex* vtx = is->coedge_dep[1]->split(is->nbMeshingEdges_dep[1], icmd);
                    vtx2.push_back(vtx);
                    oldCoEdge2newVertices[is->coedge_dep[1]] = vtx2;
                }
                newVertices1.push_back(vtx1[0]);
                newVertices1.push_back(vtx2[0]);
            }
        }
        if (coface->getNbEdges() == 3 && ratio_ogrid>0){
#ifdef _DEBUG_SPLIT
            std::cout<<"split avec ogrid, on coupe les arêtes ..."<<std::endl;
#endif

            // détection d'un cas pathologique possible: un seul bras sur bras reliés à la dégénérescence
            if (coface->getEdge(0)->getNbMeshingEdges() == 1 || coface->getEdge(2)->getNbMeshingEdges() == 1){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            	messErr << "Il n'est pas possible de découper en o-grid une face ("<<coface->getName()<<") avec un seul bras autour de la dégérescence";
            	throw TkUtil::Exception(messErr);
            }

            // on découpe les 2 autres arêtes
            // indice de l'arête / face (1 pour les face dégénérées)
            uint ind_ar_dep = coface->getIndex(is->edge_dep);
            if (ind_ar_dep!=1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Topo::splitFaces ne trouve pas l'arête de départ en position 1", TkUtil::Charset::UTF_8));

            Edge* edge4 = coface->getEdge(0);
            if (edge4->getNbCoEdges() == 1){
                newVertices4 = oldCoEdge2newVertices[edge4->getCoEdge(0)];
            }
            else if (edge4->getNbCoEdges() == 2){
                // on considère que l'arête est déjà coupée en 2
                newVertices4.push_back(TopoHelper::getCommonVertex(edge4->getCoEdge(0), edge4->getCoEdge(1)));
            }
            else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Il n'est pas prévu d'avoir une arête composée autour d'un point de dégénérescence, ce qui est le cas dans la face "
                        <<coface->getName();
                throw TkUtil::Exception(messErr);
            }


            Edge* edge2 = coface->getEdge(2);
            if (edge2->getNbCoEdges() == 1){
                newVertices2 = oldCoEdge2newVertices[edge2->getCoEdge(0)];
            }
            else if (edge2->getNbCoEdges() == 2){
                // on considère que l'arête est déjà coupée en 2
                newVertices2.push_back(TopoHelper::getCommonVertex(edge2->getCoEdge(0), edge2->getCoEdge(1)));
            }
            else{
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Il n'est pas prévu d'avoir une arête composée autour d'un point de dégénérescence, ce qui est le cas dans la face "
                        <<coface->getName();
                throw TkUtil::Exception(messErr);
            }


            // le sommet à la dégénérescence
            Vertex* vtx0 = coface->getVertex(0);

#ifdef _DEBUG_SPLIT
            std::cout<<" ni trouvé : "<<ni<<std::endl;
#endif
            // calcul du ni, nombre de bras entre le bloc central et le contour extérieur de la face
            if (ni){
            	if (edge2->getNbMeshingEdges()>ni)
            		ni = edge2->getNbMeshingEdges()-ni;
            	else
            		ni = 1;
            }
            else
            	// version inexacte ...
            	ni = (uint)((double)edge2->getNbMeshingEdges() * (1.0-ratio_ogrid));

            if (ni == 0)
                ni = 1;
#ifdef _DEBUG_SPLIT
            std::cout<<" ni calculé : "<<ni<<std::endl;
#endif

            // découpe de l'arête 2
            if (newVertices2.empty()){
                // vérif du sens pour prendre ni ou le complément
                uint ni_loc = ni;
                if (vtx0 == edge2->getCoEdge(0)->getVertex(0))
                    ni_loc = edge2->getNbMeshingEdges() - ni;
#ifdef _DEBUG_SPLIT
                std::cout<<" ni_loc : "<<ni_loc<<std::endl;
#endif
                newVertices2.push_back(edge2->getCoEdge(0)->split(ni_loc, icmd));
                oldCoEdge2newVertices[edge2->getCoEdge(0)] = newVertices2;
            }

            std::vector<Edge*> & newEdges2 = old2newEdges[edge2];
            if (newEdges2.empty())
                edge2->split(newVertices2, newEdges2, icmd);

            // découpe de l'arête 4 (ou 0 en base 4)
            if (newVertices4.empty()){
                // vérif du sens pour prendre ni ou le complément
                uint ni_loc = ni;
                if (vtx0 == edge4->getCoEdge(0)->getVertex(0))
                    ni_loc = edge4->getNbMeshingEdges() - ni;
#ifdef _DEBUG_SPLIT
                std::cout<<" ni_loc : "<<ni_loc<<std::endl;
#endif
                newVertices4.push_back(edge4->getCoEdge(0)->split(ni_loc, icmd));
                oldCoEdge2newVertices[edge4->getCoEdge(0)] = newVertices4;
            }

            std::vector<Edge*> & newEdges4 = old2newEdges[edge4];
            if (newEdges4.empty())
                edge4->split(newVertices4, newEdges4, icmd);

            // on replace les sommets en fonction du ratio_ogrid
            Vertex* vtx_opp;
            double ratio = ratio_ogrid/std::sqrt(2.0);
            vtx_opp = edge4->getOppositeVertex(vtx0);
            CoEdge* coedge4 = edge4->getCoEdge(newVertices4[0], vtx_opp);
            newVertices4[0]->setCoord(vtx0->getCoord() + (vtx_opp->getCoord() - vtx0->getCoord()) * ratio);
            vtx_opp = edge2->getOppositeVertex(vtx0);
            CoEdge* coedge2 = edge2->getCoEdge(newVertices2[0], vtx_opp);
            newVertices2[0]->setCoord(vtx0->getCoord() + (vtx_opp->getCoord() - vtx0->getCoord()) * ratio);

        } // end if (coface->getNbEdges() == 3 && ratio_ogrid>0)


        std::vector<Edge*> & newEdges_dep = old2newEdges[is->edge_dep];
        if (newEdges_dep.empty()){
#ifdef _DEBUG_SPLIT
            std::cout<<"edge_dep à découper "<<is->edge_dep->getName()<<"\n";
            std::cout<<"newVertices1 : ";
            for (uint i=0; i<newVertices1.size(); i++)
                std::cout<<" "<<(newVertices1[i]?newVertices1[i]->getName():"undef");
            std::cout<<std::endl;
#endif
            is->edge_dep->split(newVertices1, newEdges_dep, icmd);
        }


        if (is->coedge_ar.size() == 1){
            if (is->coedge_ar[0]){
                newVertices3 = oldCoEdge2newVertices[is->coedge_ar[0]];
                if (newVertices3.empty()){
                    newVertices3 = is->coedge_ar[0]->split(is->nbMeshingEdges_ar, icmd);
                    oldCoEdge2newVertices[is->coedge_ar[0]] = newVertices3;
                }
            }
            else {
                newVertices3.push_back(coface->getVertex(0)); // le sommet à la dégénérescence
            }
        } else if (is->coedge_ar.size() == 2){
            if (is->coedge_ar[0]){

                if (is->coedge_ar[0] == is->coedge_ar[1]){
                    newVertices3 = oldCoEdge2newVertices[is->coedge_ar[0]];
                    if (newVertices3.empty()){
                        newVertices3 = is->coedge_ar[0]->split(is->nbMeshingEdges_ar, icmd);
                        oldCoEdge2newVertices[is->coedge_ar[0]] = newVertices3;
                    }
                }
                else {
                    std::vector<Vertex*> vtx1 = oldCoEdge2newVertices[is->coedge_ar[0]];
                    if (vtx1.empty()){
                        Vertex* vtx = is->coedge_ar[0]->split(is->nbMeshingEdges_ar[0], icmd);
                        vtx1.push_back(vtx);
                        oldCoEdge2newVertices[is->coedge_ar[0]] = vtx1;
                    }
                    std::vector<Vertex*> vtx2 = oldCoEdge2newVertices[is->coedge_ar[1]];
                    if (vtx2.empty()){
                        Vertex* vtx = is->coedge_ar[1]->split(is->nbMeshingEdges_ar[1], icmd);
                        vtx2.push_back(vtx);
                        oldCoEdge2newVertices[is->coedge_ar[1]] = vtx2;
                    }
                    newVertices3.push_back(vtx1[0]);
                    newVertices3.push_back(vtx2[0]);
                }
            }
            else {
                newVertices3.push_back(coface->getVertex(0)); // le sommet à la dégénérescence
                newVertices3.push_back(coface->getVertex(0));
            }
        }

        std::vector<Edge*> & newEdges_ar = old2newEdges[is->edge_ar];
        if (newEdges_ar.empty() && is->edge_ar){
#ifdef _DEBUG_SPLIT
            std::cout<<"edge_ar à découper "<<is->edge_ar->getName()<<"\n";
            std::cout<<"newVertices3 : ";
            for (uint i=0; i<newVertices3.size(); i++)
                std::cout<<" "<<(newVertices3[i]?newVertices3[i]->getName():"undef");
            std::cout<<std::endl;
#endif
            is->edge_ar->split(newVertices3, newEdges_ar, icmd);
        }

        if (coface->getNbEdges() == 3 && ratio_ogrid>0){
            // découpage en 3 ou 4 CoFace

            // NB on ne retourne pas les arêtes créées, cela n'a pas d'utilité

            Edge* oldEdge0 = coface->getEdge(0);
            Edge* oldEdge2 = coface->getEdge(2);

            coface->splitOgrid((CoFace::eDirOnCoFace)is->dirCoFaceSplit,
                    old2newEdges[oldEdge0],
                    newEdges_dep,
                    old2newEdges[oldEdge2],
                    newVertices4,
                    newVertices1,
                    newVertices2,
                    ratio_ogrid,
                    icmd);

            // Destruction des arêtes inutiles (celles sans relation vers une CoFace)
            if (oldEdge0->getNbCoFaces() == 0)
                oldEdge0->free(icmd);
            if (oldEdge2->getNbCoFaces() == 0)
                oldEdge2->free(icmd);
        }
        else {
            // découpage de la CoFace en deux ou trois
            std::vector<Edge*> newEdges = coface->split((CoFace::eDirOnCoFace)is->dirCoFaceSplit,
                    newEdges_dep, newEdges_ar, newVertices1, newVertices3, icmd);

            splitingEdges.insert(splitingEdges.begin(), newEdges.begin(), newEdges.end());

        }

        // Destruction des arêtes inutiles (celles sans relation vers une CoFace)
        if (0 != is->edge_dep && is->edge_dep->getNbCoFaces() == 0)
            is->edge_dep->free(icmd);
        if (0 != is->edge_ar && is->edge_ar->getNbCoFaces() == 0)
            is->edge_ar->free(icmd);

        // libération de la mémoire
        delete is;

    } // for (std::map<CoFace*, InfoSplit*>::iterator iter = cofaceInfoSplit.begin() ...

}
/*----------------------------------------------------------------------------*/
void TopoHelper::getMarqued(std::map<CoEdge*, uint> filtre, const uint marque, std::vector<CoEdge*>& out)
{
    out.clear();

    std::list<CoEdge*> liste;
    for (std::map<CoEdge*, uint>::iterator iter = filtre.begin(); iter != filtre.end(); ++iter)
        if (marque == (*iter).second)
            liste.push_back((*iter).first);

    liste.sort(Utils::Entity::compareEntity);

    out.insert(out.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::getMarqued(std::map<CoFace*, uint> filtre, std::vector<CoFace*>& out)
{
    out.clear();

    std::list<CoFace*> liste;
    for (std::map<CoFace*, uint>::iterator iter = filtre.begin(); iter != filtre.end(); ++iter)
        if ((*iter).second)
            liste.push_back((*iter).first);

    liste.sort(Utils::Entity::compareEntity);

    out.insert(out.end(), liste.begin(), liste.end());
}
/*----------------------------------------------------------------------------*/
void TopoHelper::saveTopoEntities(std::vector<CoFace*>& cofaces, Internal::InfoCommand* icmd)
{
    for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){

        (*iter1)->saveTopoProperty();
        (*iter1)->saveCoFaceTopoProperty(icmd);

        const std::vector<Edge* > & edges = (*iter1)->getEdges();

        for (std::vector<Edge* >::const_iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){

            (*iter2)->saveTopoProperty();
            (*iter2)->saveEdgeTopoProperty(icmd);

            const std::vector<CoEdge* > & coedges = (*iter2)->getCoEdges();

            for (std::vector<CoEdge* >::const_iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3){

                (*iter3)->saveTopoProperty();
                (*iter3)->saveCoEdgeTopoProperty(icmd);

                const std::vector<Vertex* > & vertices = (*iter3)->getVertices();

                for (std::vector<Vertex* >::const_iterator iter4 = vertices.begin();
                        iter4 != vertices.end(); ++iter4){
                    (*iter4)->saveTopoProperty();
                    (*iter4)->saveVertexTopoProperty(icmd);
                } // end for vertices
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void TopoHelper::copyMeshingProperty(const Edge* edge, CoEdge* coedge)
{
    if (edge->getNbCoEdges() == 1){
    	CoEdgeMeshingProperty* new_cmp = edge->getCoEdge(0)->getMeshingProperty()->clone();
    	new_cmp->setPolarCut(false); // ce n'est pas compatible avec les autres côtés
        CoEdgeMeshingProperty* old_cmp = coedge->setProperty(new_cmp);
        delete old_cmp;
    }
    else {
        Topo::CoEdgeMeshingProperty* cmp1 =
                new Topo::EdgeMeshingPropertyUniform(edge->getNbMeshingEdges());
        CoEdgeMeshingProperty* cmp = coedge->setProperty(cmp1);
        delete cmp;
    }
}
/*----------------------------------------------------------------------------*/
void TopoHelper::sortVertices(Vertex* & vtx0, Vertex* & vtx1)
{
    if (vtx0->getUniqueId()>vtx1->getUniqueId()){
        Vertex* vtxTmp = vtx0;
        vtx0 = vtx1;
        vtx1 = vtxTmp;
    }
}
/*----------------------------------------------------------------------------*/
void TopoHelper::deleteAllAdjacentTopoEntitiesGeomAssociation(Vertex* vtx)
{
    if (vtx->getGeomAssociation()){
        vtx->saveTopoProperty();
        vtx->setGeomAssociation(0);
    }
    std::vector<CoEdge* > loc_coedges;
    vtx->getCoEdges(loc_coedges);
    for (std::vector<CoEdge* >::iterator iter = loc_coedges.begin();
            iter != loc_coedges.end(); ++iter){
        CoEdge* coedge = *iter;
        if (coedge->getGeomAssociation()){
            coedge->saveTopoProperty();
            coedge->setGeomAssociation(0);
        }
    }
    std::vector<CoFace* > loc_cofaces;
    vtx->getCoFaces(loc_cofaces);
    for (std::vector<CoFace* >::iterator iter = loc_cofaces.begin();
            iter != loc_cofaces.end(); ++iter){
        CoFace* coface = *iter;
        if (coface->getGeomAssociation()){
            coface->saveTopoProperty();
            coface->setGeomAssociation(0);
        }
    }
}
/*----------------------------------------------------------------------------*/
Vertex* TopoHelper::getCommonVertex(CoEdge* coedge1, CoEdge* coedge2)
{
    if (coedge1->getVertex(0) == coedge2->getVertex(0)
            || coedge1->getVertex(0) == coedge2->getVertex(1))
        return coedge1->getVertex(0);
    else if (coedge1->getVertex(1) == coedge2->getVertex(0)
            || coedge1->getVertex(1) == coedge2->getVertex(1))
        return coedge1->getVertex(1);

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Pas de sommet commun entre "<<*coedge1<< " et "<<*coedge2;
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
Vertex* TopoHelper::getCommonVertex(Face* face1, Face* face2)
{
	// on marque les sommets de la première face
	std::set<Vertex*> filtre_vertices;

	std::vector<Vertex* > vertices;
	face1->getVertices(vertices);

	for (std::vector<Vertex*>::iterator iter=vertices.begin(); iter!=vertices.end(); ++iter)
		filtre_vertices.insert(*iter);

	// on recherche parmis les sommets de la deuxième faces les somets marqués
	std::vector<Vertex* > selected_vertices;

	face2->getVertices(vertices);
	for (std::vector<Vertex*>::iterator iter=vertices.begin(); iter!=vertices.end(); ++iter)
		if (filtre_vertices.find(*iter) != filtre_vertices.end())
			selected_vertices.push_back(*iter);

	if (1 != selected_vertices.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr <<"TopoHelper::getCommonVertex ne trouve pas un unique noeud commun aux deux faces "
				<<face1->getName()<<" et "<<face2->getName()<<", mais en trouve "<<(short)selected_vertices.size();
		throw TkUtil::Exception(messErr);
	}

	return selected_vertices[0];
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_isUnidirectionalMeshable
bool TopoHelper::isUnidirectionalMeshable(std::vector<std::vector<CoEdge* > > coedges_dirs)
{
#ifdef _DEBUG_isUnidirectionalMeshable
    std::cout<<"isUnidirectionalMeshable ... "<<std::endl;
#endif
    // chacun des groupes doit être composé d'arêtes rectilignes et colinéaires
    // s'il y a plusieurs arêtes, il faut qu'elles soient toutes avec des bras de même taille pour une direction donnée

    uint nb_coedges_vues = 0;

    for (uint i=0; i<coedges_dirs.size(); i++){
        std::vector<CoEdge* > & coedges = coedges_dirs[i];
#ifdef _DEBUG_isUnidirectionalMeshable
        std::cout<<" i = "<<i<<", coedges.size() = "<<coedges.size() <<std::endl;
#endif
        if (coedges.empty())
            continue;
        nb_coedges_vues += coedges.size();
        //Geom::Curve* curve_prec = 0;
        double tailleBras_prec = 0.0;

        for (uint j=0; j<coedges.size(); j++){
            CoEdge* coedge = coedges[j];
            CoEdge* coedge_prec = (j?coedges[j]:0);

            if (coedge->getMeshLaw() != CoEdgeMeshingProperty::uniforme){
#ifdef _DEBUG_isUnidirectionalMeshable
                std::cout<<" non uniforme"<<std::endl;
#endif
                return false;
            }

            double tailleBras = (coedge->getVertex(0)->getCoord()
                                 -coedge->getVertex(1)->getCoord()).norme()
                            /(double)coedge->getNbMeshingEdges();

            if (j>=1 && !Utils::Math::MgxNumeric::isNearlyZero(tailleBras_prec-tailleBras) ){
#ifdef _DEBUG_isUnidirectionalMeshable
                std::cout<<" taille de bras qui change"<<std::endl;
#endif
                return false;
            }
            tailleBras_prec = tailleBras;

#ifdef _DEBUG_isUnidirectionalMeshable
            std::cout<<"coedge = "<<coedge->getName()<<std::endl;
#endif
            Geom::Curve* curve = 0;
            Geom::Surface* surface = 0;
            if (coedge->getGeomAssociation()){
                curve = dynamic_cast<Geom::Curve*>(coedge->getGeomAssociation());
                surface = dynamic_cast<Geom::Surface*>(coedge->getGeomAssociation());
            }
#ifdef _DEBUG_isUnidirectionalMeshable
            std::cout<<"curve = "<<(curve?curve->getName():"0")<<std::endl;
            std::cout<<"surface = "<<(surface?surface->getName():"0")<<std::endl;
#endif

            // on accepte comme linéaire les arêtes sans projection,
            // les segments, les arêtes projetées sur un plan
            if ((curve && curve->isLinear())
                 || (surface && surface->isPlanar())
                 || coedge->getGeomAssociation() == 0){
#ifdef _DEBUG_isUnidirectionalMeshable
                std::cout<<"  cas linéaire "<<std::endl;
#endif
                // s'il y en a plusieur, il faut tester la colinéarité
                if (j){
                    Utils::Math::Vector v1(coedge_prec->getVertex(0)->getCoord(), coedge_prec->getVertex(1)->getCoord());
                    Utils::Math::Vector v2(coedge->getVertex(0)->getCoord(), coedge->getVertex(1)->getCoord());

                    Utils::Math::Vector v3 = v1*v2;
                    if (!Utils::Math::MgxNumeric::isNearlyZero(v3.abs2())){
#ifdef _DEBUG_isUnidirectionalMeshable
                        std::cout<<"  cas non colinéaire "<<std::endl;
#endif
                        return false;
                    }
                } // end if (j)
            }
            else {
#ifdef _DEBUG_isUnidirectionalMeshable
                std::cout<<"  cas non linéaire"<<std::endl;
#endif
                return false;
            }
        } // end for j
    } // end for i

    // on n'accepte pas une direction sans aucune arête
    if (nb_coedges_vues){
#ifdef _DEBUG_isUnidirectionalMeshable
        std::cout<<"  cas sans arête commune"<<std::endl;
#endif
        return true;
    }
    else {
#ifdef _DEBUG_isUnidirectionalMeshable
        std::cout<<"  cas accepté"<<std::endl;
#endif
        return false;
    }
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::TopoEntity*> TopoHelper::getTopoEntities(std::vector<Geom::GeomEntity*>& geomEntities)
{
//    std::cout<<"TopoHelper::getTopoEntities avec "<<geomEntities.size()<<" entités géométriques"<<std::endl;
	std::list<Topo::TopoEntity*> initTopoEntities;
    //std::set<Topo::TopoEntity*> initTopoEntities;
    for (uint i=0; i<geomEntities.size(); i++){
//        std::cout<<" "<<geomEntities[i]->getName()<<" ->";
        std::vector<Topo::TopoEntity* > vte;
        geomEntities[i]->getRefTopo(vte);
//        for (uint j=0; j<vte.size();j++)
//            std::cout<<" "<<vte[j]->getName();
//        std::cout<<std::endl;
        initTopoEntities.insert(initTopoEntities.end(), vte.begin(), vte.end());
    }

    initTopoEntities.sort(Utils::Entity::compareEntity);
    initTopoEntities.unique();

    std::vector<Topo::TopoEntity*> topoEntities;
    topoEntities.insert(topoEntities.end(), initTopoEntities.begin(), initTopoEntities.end());

//    std::cout<<"TopoHelper::getTopoEntities retourne "<<topoEntities.size()<<" entités topologiques"<<std::endl;

    return topoEntities;
}
/*----------------------------------------------------------------------------*/
std::vector<Geom::GeomEntity*> TopoHelper::getGeomEntities(std::vector<Topo::TopoEntity*>& topoEntities)
{
    std::set<Geom::GeomEntity*> initGeomEntities;
    for (uint i=0; i<topoEntities.size(); i++){
        if (topoEntities[i]->getGeomAssociation())
            initGeomEntities.insert(topoEntities[i]->getGeomAssociation());
    }

    std::vector<Geom::GeomEntity*> geomEntities;
    geomEntities.insert(geomEntities.end(), initGeomEntities.begin(), initGeomEntities.end());

    return geomEntities;
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* TopoHelper::getVertexAtPosition(std::vector<Topo::Vertex*>& vertices, const Utils::Math::Point& pt, const double& tol)
{
    //std::cout<<"TopoHelper::getVertexAtPosition avec "<<vertices.size()<<" sommets, tolérance "<<setprecision(14)<<tol<<std::endl;
    double tol2 = tol*tol;
    for (std::vector<Topo::Vertex*>::iterator iter = vertices.begin(); iter != vertices.end(); ++iter){
    	//std::cout<<"  pour "<<(*iter)->getName()<<" norme() = "<<setprecision(14)<<(pt-(*iter)->getCoord()).norme()<<std::endl;
    	if ((pt-(*iter)->getCoord()).norme2()<tol2)
            return *iter;
    }

    return 0;
}
/*----------------------------------------------------------------------------*/
std::vector<Vertex*> TopoHelper::getVerticesNeighbour(Vertex* vtx)
{
    std::vector<Vertex*> vertices;
    std::vector<CoEdge*> coedges;
    vtx->getCoEdges(coedges);

    for (std::vector<Topo::CoEdge*>::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        if (!(*iter)->isDestroyed() && (*iter)->getNbVertices() == 2)
            vertices.push_back((*iter)->getOppositeVertex(vtx));

    return vertices;
}
/*----------------------------------------------------------------------------*/
std::vector<CoEdge*> TopoHelper::getBorder(std::vector<CoFace*>& cofaces)
{
	// on marque les coedges suivant le nombre de fois où elles sont référencées par une coface
	std::map<CoEdge*, uint> filtre_coedges;

	for (uint i=0; i<cofaces.size(); i++){
		std::vector<CoEdge* > coedges;
		// les coedges qui apparaissent 2 fois dans une même coface ne seront pas au bord
		cofaces[i]->getCoEdges(coedges, false);

		for (uint j=0; j<coedges.size(); j++)
			filtre_coedges[coedges[j]] += 1;
	}

	std::vector<CoEdge*> coedges;
	for (std::map<CoEdge*, uint>::iterator iter = filtre_coedges.begin();
			iter != filtre_coedges.end(); ++iter)
		if (iter->second == 1)
			coedges.push_back(iter->first);

	return coedges;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoFace*> TopoHelper::getBorder(std::vector<Topo::Block*>& blocks)
{
	// on marque les cofaces suivant le nombre de fois où elles sont référencées par un bloc
	std::map<CoFace*, uint> filtre_cofaces;

	for (uint i=0; i<blocks.size(); i++){
		std::vector<CoFace* > cofaces;
		// les coedges qui apparaissent 2 fois dans une même coface ne seront pas au bord
		blocks[i]->getCoFaces(cofaces);

		for (uint j=0; j<cofaces.size(); j++)
			filtre_cofaces[cofaces[j]] += 1;
	}

	std::vector<CoFace*> cofaces;
	for (std::map<CoFace*, uint>::iterator iter = filtre_cofaces.begin();
			iter != filtre_cofaces.end(); ++iter)
		if (iter->second == 1)
			cofaces.push_back(iter->first);

	return cofaces;
}
/*----------------------------------------------------------------------------*/
std::vector<CoEdge*> TopoHelper::getCommonCoEdges(Block* bloc1, Block* bloc2)
{
	// on marque les arêtes du premier bloc
	std::set<CoEdge*> filtre_coedges;

	std::vector<CoEdge* > coedges;
	bloc1->getCoEdges(coedges);

	for (std::vector<CoEdge*>::iterator iter=coedges.begin(); iter!=coedges.end(); ++iter)
		filtre_coedges.insert(*iter);

	// on recherche parmis les arêtes du deuxième bloc les arêtes marquées
	std::vector<CoEdge* > selected_coedges;

	bloc2->getCoEdges(coedges);
	for (std::vector<CoEdge*>::iterator iter=coedges.begin(); iter!=coedges.end(); ++iter)
		if (filtre_coedges.find(*iter) != filtre_coedges.end())
			selected_coedges.push_back(*iter);

	return selected_coedges;
}
/*----------------------------------------------------------------------------*/
std::vector<CoEdge*> TopoHelper::getCommonCoEdges(CoFace* face1, CoFace* face2)
{
	// on marque les arêtes de la première coface
	std::set<CoEdge*> filtre_coedges;

	std::vector<CoEdge* > coedges;
	face1->getCoEdges(coedges);

	for (std::vector<CoEdge*>::iterator iter=coedges.begin(); iter!=coedges.end(); ++iter)
		filtre_coedges.insert(*iter);

	// on recherche parmis les arêtes de la deuxième coface les arêtes marquées
	std::vector<CoEdge* > selected_coedges;

	face2->getCoEdges(coedges);
	for (std::vector<CoEdge*>::iterator iter=coedges.begin(); iter!=coedges.end(); ++iter)
		if (filtre_coedges.find(*iter) != filtre_coedges.end())
			selected_coedges.push_back(*iter);

	return selected_coedges;
}
/*----------------------------------------------------------------------------*/
CoEdge* TopoHelper::getCommonCoEdge(Topo::Vertex* v0, Topo::Vertex* v1)
{
	std::vector<CoEdge*> coedges;
	v0->getCoEdges(coedges);
	for (uint i=0; i<coedges.size(); i++)
		if (v1 == coedges[i]->getOppositeVertex(v0))
			return coedges[i];

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Erreur interne dans TopoHelper::getCommonCoEdge avec sommets "
    		<<v0->getName()<<" et "<<v1->getName();
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
CoEdge* TopoHelper::
getCoedge(std::vector<Vertex*>& vertices, std::vector<CoEdge*>& coedges)
{
#ifdef _DEBUG2
	std::cout<<"getCoedge avec vertices:";
	for (uint i=0;i<vertices.size(); i++)
		std::cout<<" "<<vertices[i]->getName();
	std::cout<<std::endl;
#endif

	std::map<Vertex*,uint> filtre_vtx;

	// marque les sommets que doit posséder la coedge recherchée
	for (std::vector<Vertex*>::iterator iter = vertices.begin();
			iter != vertices.end(); ++iter)
		filtre_vtx[*iter] = 1;

	for (std::vector<CoEdge*>::iterator iter1 = coedges.begin();
			iter1 != coedges.end(); ++iter1){
		std::vector<Vertex*> loc_vertices;
		(*iter1)->getVertices(loc_vertices);
		uint nbSommetsMarques = 0;
		for (std::vector<Vertex*>::iterator iter2 = loc_vertices.begin();
				iter2 != loc_vertices.end(); ++iter2)
			nbSommetsMarques += filtre_vtx[*iter2];
#ifdef _DEBUG2
		std::cout<<"   "<<(*iter1)->getName()<<":";
		for (uint i=0;i<loc_vertices.size(); i++)
			std::cout<<" "<<loc_vertices[i]->getName();
		std::cout<<" nbSommetsMarques = "<<nbSommetsMarques<<std::endl;
#endif
		if (nbSommetsMarques == vertices.size())
			return *iter1;
	} // end for iter1 = coedges.begin()

	return 0;
}
/*----------------------------------------------------------------------------*/
CoFace* TopoHelper::
getCoface(std::vector<Vertex*>& vertices, std::vector<CoFace*>& cofaces)
{
#ifdef _DEBUG2
	std::cout<<"getCoface avec vertices:";
	for (uint i=0;i<vertices.size(); i++)
		std::cout<<" "<<vertices[i]->getName();
	std::cout<<std::endl;
#endif

	std::map<Vertex*,uint> filtre_vtx;

	// marque les sommets que doit posséder la coface recherchée
	for (std::vector<Vertex*>::iterator iter = vertices.begin();
			iter != vertices.end(); ++iter)
		filtre_vtx[*iter] = 1;

	for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
			iter1 != cofaces.end(); ++iter1){
		std::vector<Vertex*> loc_vertices;
		(*iter1)->getAllVertices(loc_vertices);
		uint nbSommetsMarques = 0;
		for (std::vector<Vertex*>::iterator iter2 = loc_vertices.begin();
				iter2 != loc_vertices.end(); ++iter2)
			nbSommetsMarques += filtre_vtx[*iter2];
#ifdef _DEBUG2
		std::cout<<"   "<<(*iter1)->getName()<<":";
		for (uint i=0;i<loc_vertices.size(); i++)
			std::cout<<" "<<loc_vertices[i]->getName();
		std::cout<<" nbSommetsMarques = "<<nbSommetsMarques<<std::endl;
#endif
		if (nbSommetsMarques == vertices.size())
			return *iter1;
	} // end for iter1 = cofaces.begin()

	return 0;
}
/*----------------------------------------------------------------------------*/
void TopoHelper::
getPoints(Topo::Vertex* vtx1, Topo::Vertex* vtx2,
		std::vector<Topo::CoEdge*>& coedges_between,
		std::map<Topo::CoEdge*,uint>& ratios,
		std::vector<Utils::Math::Point>& points)
{
#ifdef _DEBUG2
	std::cout<<"getPoints avec :"<<std::endl;
	std::cout<<" vtx1 = "<<vtx1->getName()<<std::endl;
	std::cout<<" vtx2 = "<<vtx2->getName()<<std::endl;
	std::cout<<" coedges_between :";
	for (uint i=0;i<coedges_between.size(); i++){
		std::cout<<" "<<coedges_between[i]->getName();
		if (ratios[coedges_between[i]] != 1)
			std::cout<<" (ratio "<<ratios[coedges_between[i]]<<")";
	}
	std::cout<<std::endl;
#endif

	Topo::Vertex* vtx_dep = vtx1;
	for (uint i=0; i<coedges_between.size(); i++){
		CoEdge* coedge = coedges_between[i];
		Topo::Vertex* vtx_arr = coedge->getOppositeVertex(vtx_dep);
		uint ratio = ratios[coedge];

		std::vector<Utils::Math::Point> loc_points;
		coedge->getPoints(loc_points);

		int ind_dep, inc, nb_pt;
		if (coedge->getVertex(0) == vtx_dep){
			ind_dep = 0;
			inc = ratio;
		}
		else {
			ind_dep = loc_points.size()-1;
			inc = -ratio;
		}

		nb_pt = (loc_points.size()-1)/ratio;

		if (i==coedges_between.size()-1)
			nb_pt += 1;
#ifdef _DEBUG2
		std::cout<<"vtx_dep : "<<vtx_dep->getName()<<std::endl;
		std::cout<<"vtx_arr : "<<vtx_arr->getName()<<std::endl;
		std::cout<<"ind_dep = "<<ind_dep<<std::endl;
		std::cout<<"nb_pt = "<<nb_pt<<std::endl;
		std::cout<<"inc = "<<inc<<std::endl;
#endif
		for (uint j=ind_dep, nb=0; nb<nb_pt; nb++,j+=inc)
			points.push_back(loc_points[j]);

		vtx_dep = vtx_arr;
	} // end for i


#ifdef _DEBUG2
	std::cout<<"  => points :"<<std::endl;
	for (uint i=0;i<points.size(); i++)
		std::cout<<"    "<<i<<" "<<points[i]<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
bool TopoHelper::
haveSame(std::vector<Topo::Vertex*>& verticesA, std::vector<Topo::Vertex*>& verticesB)
{
	if (verticesA.size() != verticesB.size())
		return false;

	std::map<Topo::Vertex*, uint> filtre;
	for (std::vector<Topo::Vertex*>::iterator iter=verticesA.begin(); iter!=verticesA.end(); ++iter)
		filtre[*iter] += 1;

	for (std::vector<Topo::Vertex*>::iterator iter=verticesB.begin(); iter!=verticesB.end(); ++iter){
		if (filtre[*iter] != 1)
			return false;
		filtre[*iter] += 1;
	}
	return true;
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* TopoHelper::
getOppositeVertex(Block* bloc, Topo::Vertex* vtx, uint indDir, uint indCote)
{
	//uint tabIndVtxByEdgeAndDirOnBlock[3][4][2];
	int indVtx = -1;
	for (uint i=0; i<4; i++)
		if (vtx == bloc->getVertex(tabIndVtxByEdgeAndDirOnBlock[indDir][i][indCote]))
			indVtx = i;

	if (indVtx == -1){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur interne dans TopoHelper::getOppositeVertex pour bloc "
        		<<bloc->getName()<<" et sommet "<<vtx->getName();
        throw TkUtil::Exception(messErr);
    }

	return bloc->getVertex(tabIndVtxByEdgeAndDirOnBlock[indDir][indVtx][(indCote+1)%2]);
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_NORMALE
Utils::Math::Vector TopoHelper::
computeNormale(Topo::Vertex* vtx0, const Topo::CoEdge* coedge)
{
#ifdef _DEBUG_NORMALE
	std::cout<<"computeNormale en "<<vtx0->getName()<<" autour de "<<coedge->getName()<<std::endl;
#endif
    Topo::Vertex* vtxN = coedge->getOppositeVertex(vtx0);
	Utils::Math::Point pt0 = vtx0->getCoord();
	Utils::Math::Point ptN = vtxN->getCoord();

	Utils::Math::Point vect = (ptN - pt0);

	// il faut identifier les cofaces reliées au sommet qui sont orthogonales à l'arête
	// et pour ces cofaces en déduire un couple de coedeges reliées au sommet
	// chaque couple de coedges permet d'avoir une approximation de la normale
	// à partir des discrétisations

	std::map<Topo::CoFace*, uint> filtre_coface;
	Utils::Math::Vector normale;

	// les blocs autour de l'arête pour se limiter aux cofaces orthogonales
	// on marque les cofaces autour directement reliées à l'arête
	// pour ne pas les prendre
	std::vector<Topo::Block*> blocks;
	coedge->getBlocks(blocks);
	std::vector<Topo::CoFace*> cofaces;
	coedge->getCoFaces(cofaces);
	for (uint i=0; i<cofaces.size(); i++)
		filtre_coface[cofaces[i]] = 1;

	for (uint i=0; i<blocks.size(); i++){
		// recherche d'une coface reliée au sommet mais pas encore marquée
		Topo::CoFace* coface = 0;
		blocks[i]->getCoFaces(cofaces);

		for (uint j=0; j<cofaces.size(); j++)
			if (filtre_coface[cofaces[j]] == 0 && cofaces[j]->find(vtx0))
				coface = cofaces[j];

		if (coface == 0){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Erreur interne dans TopoHelper::computeNormale pour sommet "
					<<vtx0->getName()<<" et arête "<<coedge->getName()
					<<" lors de la recherche de la face dans le bloc "<<blocks[i]->getName();
			throw TkUtil::Exception(messErr);
		}
		else {
			filtre_coface[coface] = 2;

			// recherche d'une paire de coedges dans cette face, reliés au sommet
			std::vector<Topo::CoEdge*> coedges_adjacentes;
			std::vector<Topo::CoEdge*> coedges;
			coface->getCoEdges(coedges, true);
			for (uint j=0; j<coedges.size(); j++)
				if (coedges[j]->find(vtx0))
					coedges_adjacentes.push_back(coedges[j]);

			if (coedges_adjacentes.size() == 2){

#ifdef _DEBUG_NORMALE
				std::cout<<"computeNormale pour "<<vtx0->getName()<<" et "<<coedge->getName()
						<<" trouve les 2 arêtes "<<coedges_adjacentes[0]->getName()
						<<" et "<<coedges_adjacentes[1]->getName()<<std::endl;
#endif

				Utils::Math::Vector loc_normale;

				// on recupère le couple de points du maillage en partant du sommet
				std::vector<Utils::Math::Vector> V12;
				for (uint j=0; j<2; j++){
					// création d'une discrétisation tabulée pour avoir un point proche du sommet
					std::vector<double> tabulation;
					double dist = 0.001;
					if (vtx0 == coedges_adjacentes[j]->getVertex(0)){
						tabulation.push_back(dist);
						tabulation.push_back(1-dist);
					}
					else {
						tabulation.push_back(1-dist);
						tabulation.push_back(dist);
					}

					EdgeMeshingPropertyTabulated emp(tabulation);

					std::vector<Utils::Math::Point> points;
					coedges_adjacentes[j]->getPoints(&emp, points, true);

#ifdef _DEBUG_NORMALE
					std::cout<<"EdgeMeshingPropertyTabulated => "<<points.size()<<" points:";
					for (uint k=0; k<points.size(); k++)
						std::cout<<" "<<points[k];
					std::cout<<std::endl;
#endif

					V12.push_back(points[1]-vtx0->getCoord());
				} // end for j

				loc_normale = V12[0]*V12[1];

				if (Utils::Math::scaMul(vect, loc_normale)<0.0)
					loc_normale *= -1;

				normale+=loc_normale;
			}
			else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
				messErr << "Erreur interne dans TopoHelper::computeNormale pour sommet "
						<<vtx0->getName()<<" et arête "<<coedge->getName()
						<<" lors de la recherche des 2 arêtes dans la coface "<<coface->getName();
				throw TkUtil::Exception(messErr);
			}

		} // end else / if coface == 0


	} // end for i<blocks.size()


	normale/=normale.norme();

	return normale;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_INVERTED
void TopoHelper::
computeInvertedDir(std::vector<Topo::CoFace*>& cofaces, std::map<Topo::CoFace*, bool>& isInverted)
{
	// marque pour savoir si une coface est parmis celles qui nous intéressent (1) et si elle a été vue (2)
	std::map<Topo::CoFace*, uint> filtre_coface;
	for (uint i=0; i<cofaces.size(); i++)
		filtre_coface[cofaces[i]] = 1;

	// vecteur d'accumulation des cofaces vues et à partir desquelles on cherche une relation avec les voisines
	std::vector<Topo::CoFace*> cofacesVues;

	// une première dite dans le sens normal
	cofacesVues.push_back(cofaces[0]);
	filtre_coface[cofaces[0]] = 2;
	isInverted[cofaces[0]] = false;

	for (uint k=0; k<cofacesVues.size(); k++){
		Topo::CoFace* coface = cofacesVues[k];
#ifdef _DEBUG_INVERTED
		std::cout<<"computeInvertedDir avec "<<coface->getName()<<std::endl;
#endif
		// parcours les cofaces voisines en passant par les coedges
		std::vector<CoEdge* > coedges;
		coface->getCoEdges(coedges, true);
		for (uint j=0; j<coedges.size(); j++){
			std::vector<Topo::CoFace*> cofaces_vois;
			coedges[j]->getCoFaces(cofaces_vois);

			for (uint i=0; i<cofaces_vois.size(); i++){
				Topo::CoFace* coface_vois = cofaces_vois[i];
				if (filtre_coface[coface_vois] == 1){
#ifdef _DEBUG_INVERTED
					std::cout<<"  coface_vois "<<coface_vois->getName()<<std::endl;
#endif

					bool isSameDir = cofacesInverted(coface, coface_vois, coedges[j]);
					cofacesVues.push_back(coface_vois);
					filtre_coface[coface_vois] = 2;
					if (isSameDir)
						isInverted[coface_vois] = not isInverted[coface];
					else
						isInverted[coface_vois] = isInverted[coface];

				} // end if (filtre_coface[coface_vois] == 1)
			} // end for i
		} // end for j
	}// end for k

	if (cofacesVues.size() != cofaces.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "Erreur dans TopoHelper::computeInvertedDir, "
				<<cofacesVues.size()<<" cofaces vues au lieu de "<< cofaces.size();
		throw TkUtil::Exception(messErr);
	}
#ifdef _DEBUG_INVERTED
	for (uint i=0; i<cofaces.size(); i++)
		std::cout<<cofaces[i]->getName()<<" isInverted "<<(isInverted[cofaces[i]]?"vrai":"faux")<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
bool TopoHelper::
cofacesInverted(Topo::CoFace* coface, Topo::CoFace* coface_vois, Topo::CoEdge* coedge)
{
	uint ind1 = coface->getIndex(coedge);
	Topo::Edge* edge1 = coface->getEdge(ind1);
	bool sens1 = edge1->isSameSense(coface->getVertex(ind1),coedge);

	uint ind2 = coface_vois->getIndex(coedge);
	Topo::Edge* edge2 = coface_vois->getEdge(ind2);
	bool sens2 = edge2->isSameSense(coface_vois->getVertex(ind2),coedge);

	return sens1 == sens2;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
