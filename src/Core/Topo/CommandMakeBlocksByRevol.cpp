/*----------------------------------------------------------------------------*/
/*
 * \file CommandMakeBlocksByRevol.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 12/6/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandMakeBlocksByRevol.h"

#include "Utils/Common.h"
#include "Utils/Matrix33.h"
#include "Utils/MgxNumeric.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
#include "Topo/FaceMeshingPropertyRotational.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Topo/BlockMeshingPropertyRotational.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"

#include "Geom/CommandExtrudeRevolution.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"

//#define _DEBUG_REVOL
//#define _DEBUG_INTERPOLATE
//#define _DEBUG_MESH_LAW
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandMakeBlocksByRevol::
CommandMakeBlocksByRevol(Internal::Context& c,
        Geom::CommandExtrudeRevolution* commandGeom,
        std::vector<CoEdge*>& coedges,
        const  Utils::Portion::Type& dt,
		double ratio_ogrid)
:CommandEditTopo(c, "Construction Topo 3D avec o-grid par révolution")
, m_commandGeom(commandGeom)
, m_coedges(coedges)
, m_portion(dt)
, m_axis(XAxis)
, m_pt1(0,0,0) // ne sert pas par défaut
, m_pt2(1,0,0) // ne sert pas par défaut
, m_ni(0) // il faut le rechercher
, m_ratio_ogrid(ratio_ogrid)
{
	for (uint i=0; i<coedges.size(); i++){
		CoEdge* coedge = coedges[i];
		if (isOnAxis(coedge->getVertex(0)) && isOnAxis(coedge->getVertex(1))){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	    	message<<"Il n'est pas permis de démarrer avec une arête ayant les 2 extrémités sur l'axe\n";
	    	message<<"NB: penser à découper les faces pour placer le tracé de l'o-grid";
	    	throw TkUtil::Exception (message);
		}
	}

}
/*----------------------------------------------------------------------------*/
CommandMakeBlocksByRevol::
~CommandMakeBlocksByRevol()
{
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandMakeBlocksByRevol::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );
#ifdef _DEBUG2
    m_commandGeom->printInfoAssociations();
#endif

    // remplit les filtres en démarrant le parcours depuis l'arête sur l'o-grid
    // pour identifier la position / o-grid
    std::map<Vertex*, uint> filtre_vertex;
    std::map<CoEdge*, uint> filtre_coedge;
    std::map<CoFace*, uint> filtre_coface;
    if (m_coedges.empty())
    	computeFiltersWithAllFaces(filtre_vertex, filtre_coedge, filtre_coface);
    else
    	computeOgridFilters(filtre_vertex, filtre_coedge, filtre_coface);

//#ifdef _DEBUG_REVOL
//    // récupération de toutes les coedges sur l'ogrid (celles marquées à 1)
//    std::vector<CoEdge*> coedges_ogrid;
//    TopoHelper::getMarqued(filtre_coedge, 1, coedges_ogrid);
//
//    // on sélectionne les arêtes sur l'ogrid pour un test visuel
//    getContext().getSelectionManager().clearSelection();
//    for (std::vector<CoEdge*>::iterator iter = coedges_ogrid.begin();
//            iter != coedges_ogrid.end(); ++iter){
//          vector<Utils::Entity*>	entities;
//          getContext().getSelectionManager().addToSelection(entities);
////        getContext().getSelectionManager().addToSelection(**iter);
//    }
//#endif

    // récupération des cofaces concernées par la révolution (celle avec une marque autre que 0)
    std::vector<CoFace*> cofaces_0;
    TopoHelper::getMarqued(filtre_coface, cofaces_0);

    // force la sauvegarde des relations topologiques pour toutes les entités et celles de niveau inférieur
    TopoHelper::saveTopoEntities(cofaces_0, &getInfoCommand());

    // modifie les filtres pour identifier la position / axe
    computeAxisFilters(filtre_vertex, filtre_coedge, filtre_coface);

    // détection des sommets entre l'axe et l'ogrid
    detectVerticesUnderOgrid(filtre_vertex);

//#ifdef _DEBUG_REVOL
//    // affichage du contenu des différents filtres
//    std::cout<<"filtre_vertex : "<<std::endl;
//    for (std::map<Vertex*, uint>::iterator iter = filtre_vertex.begin();
//                iter != filtre_vertex.end(); ++iter)
//    	std::cout<<" "<<(*iter).first->getName()<<" "<<(*iter).second<<std::endl;
//    std::cout<<"filtre_coedge : "<<std::endl;
//    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedge.begin();
//                iter != filtre_coedge.end(); ++iter)
//    	std::cout<<" "<<(*iter).first->getName()<<" "<<(*iter).second<<std::endl;
//    std::cout<<"filtre_coface : "<<std::endl;
//    for (std::map<CoFace*, uint>::iterator iter = filtre_coface.begin();
//                iter != filtre_coface.end(); ++iter)
//    	std::cout<<" "<<(*iter).first->getName()<<" "<<(*iter).second<<std::endl;
//#endif

    // calcul le nombre de bras pour l'o-grid (m_ni)
    // et les ni locaux pour les vertex sur l'ogrid et les cofaces de part et d'autre de cet ogrid
    std::map<Vertex*, uint> ni_vtx;
    std::map<CoEdge*, uint> ni_coedge;
    std::map<CoFace*, uint> ni_coface;
    computeNi(filtre_vertex, filtre_coedge, ni_vtx, ni_coedge, ni_coface);

    // Modification et construction de la topologie 2D avec révolution
    // Les sommets sur l'axe sont conservés
    // Les autres sont dupliqués et tournés / axe
    // Les sommets sur l'ogrid subissent en plus une homothétie (sqrt(2) / axe)

    if (m_portion == Utils::Portion::QUART
        || m_portion == Utils::Portion::TIERS
        || m_portion == Utils::Portion::CINQUIEME
		|| m_portion == Utils::Portion::SIXIEME){ // révolution d'un quart, tiers, cinquième, ou sixième

        // Construction des CoFaces par copie et révolution de 45
        // les CoFace sous l'o-grid n'ont pas de CoFace à 45 visible
        std::vector<CoFace*> cofaces_45;
        duplicate(cofaces_0, filtre_vertex, filtre_coedge, filtre_coface, cofaces_45);
        double angle = 0;
        if (m_portion == Utils::Portion::QUART)
        	angle = 45;
        else if (m_portion == Utils::Portion::TIERS)
        	angle = 60;
        else if (m_portion == Utils::Portion::CINQUIEME)
        	angle = 36;
        else if (m_portion == Utils::Portion::SIXIEME)
        	angle = 30;
        else
        	throw TkUtil::Exception ("Erreur interne sur demi-angle de rotation");

        rotate(angle, cofaces_45);

        // copie des ratios
        copyNi(ni_vtx, ni_coedge, ni_coface, cofaces_0, cofaces_45);

        // recherche parmis les cofaces, les sommets marqués comme étant sur l'ogrid
        std::vector<Vertex*> vertices_45_ogrid;
        getMarquedOnOgrid(cofaces_45, filtre_vertex, vertices_45_ogrid);

        // on déplace les sommets pour les mettre aux sommets des blocs des o-grids
        // pour le 1/5 on s'arrange pour que les arêtes qui partent du sommet déplacé soient placées
        // suivant des angles égaux (120 degrés)
        double ratio_scale = 0;
        if (m_portion == Utils::Portion::QUART)
        	ratio_scale = m_ratio_ogrid*(std::sqrt(2.0)-1.0)+1.0; // 1.41
        else if (m_portion == Utils::Portion::TIERS)
        	ratio_scale = m_ratio_ogrid*(std::sqrt(2.0)-1.0)+1.0;
        else if (m_portion == Utils::Portion::CINQUIEME)
        	ratio_scale = m_ratio_ogrid*(1.148-1.0)+1.0;
        else if (m_portion == Utils::Portion::SIXIEME)
        	ratio_scale = m_ratio_ogrid*(1.148-1.0)+1.0;
        else
        	throw TkUtil::Exception ("Erreur interne sur ratio_scale");

        scale(ratio_scale, vertices_45_ogrid);

        // projette les sommets du tracé de l'ogrid
        updateGeomAssociationOGrid(vertices_45_ogrid, filtre_vertex, filtre_coedge);

        // Construction des CoFaces par copie et révolution de 90
        std::vector<CoFace*> cofaces_90;
        duplicate(cofaces_0, filtre_vertex, filtre_coedge, filtre_coface, cofaces_90);
        if (m_portion == Utils::Portion::QUART)
        	angle = 90;
        else if (m_portion == Utils::Portion::TIERS)
        	angle = 120;
        else if (m_portion == Utils::Portion::CINQUIEME)
        	angle = 72;
        else if (m_portion == Utils::Portion::SIXIEME)
        	angle = 60;
        else
        	throw TkUtil::Exception ("Erreur interne sur demi-angle de rotation");
        rotate(angle, cofaces_90);

        // Construction des arêtes (CoEdge) entre les différents couples de topo 2D
        std::map<Vertex*, std::vector<CoEdge* > > vtx2coedges;
        constructRevolCoEdges(cofaces_0,  cofaces_45, filtre_vertex, filtre_coedge, 1, true, vtx2coedges, ni_vtx, true);
        constructRevolCoEdges(cofaces_45, cofaces_90, filtre_vertex, filtre_coedge, 1, false, vtx2coedges, ni_vtx, false);

        // Construction des faces (CoFace et Edge) entre les différents couples de topo 2D
        // sauf pour les zones sous l'ogrid
        std::map<CoEdge*, std::vector<CoFace* > > coedge2cofaces;
        constructRevolFaces(cofaces_0,   cofaces_45, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 1);
        constructRevolFaces(cofaces_45,  cofaces_90, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 1);

        // Construction des faces (CoFace et Edge) situées sous l'ogrid
        constructRevolFacesInnerOgrid(cofaces_0, cofaces_90, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces);

        // Construction des blocs entre les différents couples de topo 2D
        constructRevolBlocks(cofaces_0,   cofaces_45, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 1);
        constructRevolBlocks(cofaces_45,  cofaces_90, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 1);

        // Construction des blocs sous l'ogrid
        constructRevolBlocksInnerOgrid_4(cofaces_0, cofaces_45, cofaces_90, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface);

        // Mise à jour des projections pour les entités 2D et leurs copies
        CommandEditTopo::updateGeomAssociation(cofaces_0,
                m_commandGeom->getAssociationV2V(),
                m_commandGeom->getAssociationC2C(),
                m_commandGeom->getAssociationS2S(),
				true);

        CommandEditTopo::updateGeomAssociation(cofaces_45,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        CommandEditTopo::updateGeomAssociation(cofaces_90,
                m_commandGeom->getAssociationV2VOpp(),
                m_commandGeom->getAssociationC2COpp(),
                m_commandGeom->getAssociationS2SOpp(),
				true);

        // suppression des cofaces et des autres entités qui sont sous l'ogrid
        freeUnused(cofaces_45,  filtre_vertex, filtre_coedge, filtre_coface);

    } else if (m_portion == Utils::Portion::DEMI){ // révolution d'un demi

        // Construction des CoFaces par copie et révolution de 45
        // les CoFace sous l'o-grid n'ont pas de CoFace à 45 visible
        std::vector<CoFace*> cofaces_45;
        duplicate(cofaces_0, filtre_vertex, filtre_coedge, filtre_coface, cofaces_45);
        rotate(45, cofaces_45);

        // recherche parmis les cofaces, les sommets marqués comme étant sur l'ogrid
        std::vector<Vertex*> vertices_45_ogrid;
        getMarquedOnOgrid(cofaces_45, filtre_vertex, vertices_45_ogrid);

        // on déplace les sommets pour les mettre aux sommets des blocs des o-grids
        scale(m_ratio_ogrid*(std::sqrt(2.0)-1.0)+1.0, vertices_45_ogrid);

        // projette les sommets du tracé de l'ogrid
        updateGeomAssociationOGrid(vertices_45_ogrid, filtre_vertex, filtre_coedge);

        // on réutilise les cofaces à 45 pour faire celles à 135
        std::vector<CoFace*> cofaces_135;
        duplicate(cofaces_45, filtre_vertex, filtre_coedge, filtre_coface, cofaces_135);
        rotate(90, cofaces_135);

        // copie des ratios
        copyNi(ni_vtx, ni_coedge, ni_coface, cofaces_0, cofaces_45);
        copyNi(ni_vtx, ni_coedge, ni_coface, cofaces_0, cofaces_135);

        // Construction des CoFaces par copie et révolution de 180
        std::vector<CoFace*> cofaces_180;
        duplicate(cofaces_0, filtre_vertex, filtre_coedge, filtre_coface, cofaces_180);
        rotate(180, cofaces_180);

        // Construction des arêtes (CoEdge) entre les différents couples de topo 2D
        std::map<Vertex*, std::vector<CoEdge* > > vtx2coedges;
        constructRevolCoEdges(cofaces_0,   cofaces_45,  filtre_vertex, filtre_coedge, 1, true, vtx2coedges, ni_vtx, true);
        constructRevolCoEdges(cofaces_45,  cofaces_135, filtre_vertex, filtre_coedge, 2, true, vtx2coedges, ni_vtx, false);
        constructRevolCoEdges(cofaces_135, cofaces_180, filtre_vertex, filtre_coedge, 1, false, vtx2coedges, ni_vtx, false);

        // Construction des faces (CoFace et Edge) entre les différents couples de topo 2D
        // sauf pour les zones sous l'ogrid
        std::map<CoEdge*, std::vector<CoFace* > > coedge2cofaces;
        constructRevolFaces(cofaces_0,    cofaces_45,  filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 1);
        constructRevolFaces(cofaces_45,   cofaces_135, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 2);
        constructRevolFaces(cofaces_135,  cofaces_180, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 1);

        // Construction des faces (CoFace et Edge) situées sous l'ogrid
        constructRevolFacesInnerOgrid(cofaces_0, cofaces_180, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces);

        // Construction des blocs entre les différents couples de topo 2D
        constructRevolBlocks(cofaces_0,   cofaces_45, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 1);
        constructRevolBlocks(cofaces_45,  cofaces_135, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 2);
        constructRevolBlocks(cofaces_135,  cofaces_180, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 1);

        // Construction des blocs sous l'ogrid
        constructRevolBlocksInnerOgrid_2(cofaces_0, cofaces_45, cofaces_135, cofaces_180, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface);

        // Mise à jour des projections pour les entités 2D et leurs copies
        CommandEditTopo::updateGeomAssociation(cofaces_0,
                m_commandGeom->getAssociationV2V(),
                m_commandGeom->getAssociationC2C(),
                m_commandGeom->getAssociationS2S(),
				true);

        CommandEditTopo::updateGeomAssociation(cofaces_45,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        CommandEditTopo::updateGeomAssociation(cofaces_135,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        CommandEditTopo::updateGeomAssociation(cofaces_180,
                m_commandGeom->getAssociationV2VOpp(),
                m_commandGeom->getAssociationC2COpp(),
                m_commandGeom->getAssociationS2SOpp(),
				true);

        // suppression des cofaces et des autres entités qui sont sous l'ogrid
        freeUnused(cofaces_45,  filtre_vertex, filtre_coedge, filtre_coface);
        freeUnused(cofaces_135, filtre_vertex, filtre_coedge, filtre_coface);

    } else if (m_portion == Utils::Portion::ENTIER){ // révolution complète

        // Construction des CoFaces par révolution de 45
        std::vector<CoFace*> cofaces_45 = cofaces_0;
        rotate(45, cofaces_45);

        // recherche parmis les cofaces, les sommets marqués comme étant sur l'ogrid
        std::vector<Vertex*> vertices_45_ogrid;
        getMarquedOnOgrid(cofaces_45, filtre_vertex, vertices_45_ogrid);

        // on déplace les sommets pour les mettre aux sommets des blocs des o-grids
        scale(m_ratio_ogrid*(std::sqrt(2.0)-1.0)+1.0, vertices_45_ogrid);

        // projette les sommets du tracé de l'ogrid
        updateGeomAssociationOGrid(vertices_45_ogrid, filtre_vertex, filtre_coedge);

        // Construction des CoFaces par copie (3x) et révolution de 90 (3x)
        std::vector<CoFace*> cofaces_135;
        duplicate(cofaces_45, filtre_vertex, filtre_coedge, filtre_coface, cofaces_135);
        rotate(90, cofaces_135);

        std::vector<CoFace*> cofaces_225;
        duplicate(cofaces_135, filtre_vertex, filtre_coedge, filtre_coface, cofaces_225);
        rotate(90, cofaces_225);

        std::vector<CoFace*> cofaces_315;
        duplicate(cofaces_225, filtre_vertex, filtre_coedge, filtre_coface, cofaces_315);
        rotate(90, cofaces_315);

        // copie des ratios
        copyNi(ni_vtx, ni_coedge, ni_coface, cofaces_0, cofaces_135);
        copyNi(ni_vtx, ni_coedge, ni_coface, cofaces_0, cofaces_225);
        copyNi(ni_vtx, ni_coedge, ni_coface, cofaces_0, cofaces_315);


        // Construction des arêtes (CoEdge) entre les différents couples de topo 2D
        std::map<Vertex*, std::vector<CoEdge* > > vtx2coedges;
        constructRevolCoEdges(cofaces_45, cofaces_135,  filtre_vertex, filtre_coedge, 2, true, vtx2coedges, ni_vtx, false);
        constructRevolCoEdges(cofaces_135, cofaces_225, filtre_vertex, filtre_coedge, 2, true, vtx2coedges, ni_vtx, false);
        constructRevolCoEdges(cofaces_225, cofaces_315, filtre_vertex, filtre_coedge, 2, true, vtx2coedges, ni_vtx, false);
        constructRevolCoEdges(cofaces_315, cofaces_45,  filtre_vertex, filtre_coedge, 2, true, vtx2coedges, ni_vtx, false);

        // Construction des faces (CoFace et Edge) entre les différents couples de topo 2D
        // sauf pour les zones sous l'ogrid
        std::map<CoEdge*, std::vector<CoFace* > > coedge2cofaces;
        constructRevolFaces(cofaces_45,   cofaces_135, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 2);
        constructRevolFaces(cofaces_135,  cofaces_225, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 2);
        constructRevolFaces(cofaces_225,  cofaces_315, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 2);
        constructRevolFaces(cofaces_315,  cofaces_45,  filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces, ni_coedge, 2);

        // Construction des faces (CoFace et Edge) situées sous l'ogrid
        constructRevolFacesInnerOgrid(cofaces_0, cofaces_0, filtre_vertex, filtre_coedge, vtx2coedges, coedge2cofaces);

        // Construction des blocs entre les différents couples de topo 2D
        constructRevolBlocks(cofaces_45,   cofaces_135, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 2);
        constructRevolBlocks(cofaces_135,  cofaces_225, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 2);
        constructRevolBlocks(cofaces_225,  cofaces_315, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 2);
        constructRevolBlocks(cofaces_315,  cofaces_45, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface, 2);

        // Construction des blocs sous l'ogrid
        constructRevolBlocksInnerOgrid_1(cofaces_45, cofaces_135, cofaces_225, cofaces_315, filtre_vertex, filtre_coedge, filtre_coface, vtx2coedges, coedge2cofaces, ni_vtx, ni_coface);

        // Mise à jour des projections pour les entités 2D et leurs copies
        CommandEditTopo::updateGeomAssociation(cofaces_45,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        CommandEditTopo::updateGeomAssociation(cofaces_135,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        CommandEditTopo::updateGeomAssociation(cofaces_225,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        CommandEditTopo::updateGeomAssociation(cofaces_315,
                        m_commandGeom->getAssociationV2C(),
                        m_commandGeom->getAssociationC2S(),
                        filtre_vertex,
                        filtre_coedge);

        // pour le cas des arêtes détruites (sur l'axe) qui seraient référencées via l'interpolation
        updateInterpolate(cofaces_45,  filtre_coedge, filtre_coface);
        updateInterpolate(cofaces_135, filtre_coedge, filtre_coface);
        updateInterpolate(cofaces_225, filtre_coedge, filtre_coface);
        updateInterpolate(cofaces_315, filtre_coedge, filtre_coface);

        // suppression des cofaces et des autres entités qui sont sous l'ogrid
        freeUnused(cofaces_45,  filtre_vertex, filtre_coedge, filtre_coface, true);
        freeUnused(cofaces_135, filtre_vertex, filtre_coedge, filtre_coface);
        freeUnused(cofaces_225, filtre_vertex, filtre_coedge, filtre_coface);
        freeUnused(cofaces_315, filtre_vertex, filtre_coedge, filtre_coface);

    } else {
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandMakeBlocksByRevol avec une portion non prévue", TkUtil::Charset::UTF_8));
    }

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

//    std::cout<<"Dans CommandMakeBlocksByRevol: "<<std::endl;
//    std::cout<< getInfoCommand() <<std::endl;

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
computeFiltersWithAllFaces(std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface)
{
	std::vector<CoFace*> cofaces;
	getContext().getLocalTopoManager().getCoFaces(cofaces);

	for (uint i=0; i<cofaces.size(); i++){
		CoFace* coface = cofaces[i];
		filtre_coface[coface] = 3;

		std::vector<CoEdge*> coedges;
		coface->getCoEdges(coedges, false);

		for (uint j=0; j<coedges.size(); j++){
			CoEdge* coedge = coedges[j];

			filtre_coedge[coedge] = 3;

			std::vector<Vertex*> vertices;
			coedge->getVertices(vertices);

			for (uint k=0; k<vertices.size(); k++)
				filtre_vertex[vertices[k]] = 3;
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
computeOgridFilters(std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface)
{
#ifdef _DEBUG_REVOL
    std::cout<<"CommandMakeBlocksByRevol::computeOgridFilters..."<<std::endl;
#endif

    // indique une erreur s'il n'y a pas d'arête de sélectionnée
    if (m_coedges.empty()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message<<"Erreur, il faut sélectionner au moin une arête pour identifier le tracé de l'o-grid.\n";
    	message<<"Il est possible de sélectionner une arête sur l'axe, l'o-grid est alors dégénéré.\n";
    	throw TkUtil::Exception (message);
    }


    // identification du cas où il n'y a pas d'ogrid (utilisation de l'axe comme si c'était l'ogrid)
    bool withOgrid = true;
    for (uint iar=0; iar<m_coedges.size(); ++iar) {
    	// si l'une des arêtes est sur l'axe, alors pas d'ogrid
    	if (isOnAxis(m_coedges[iar]->getVertex(0)) && isOnAxis(m_coedges[iar]->getVertex(1)))
    		withOgrid = false;
    }


    // on marque l'arête et ses sommets (dans le cas avec ogrid)
    for (uint iar=0; iar<m_coedges.size() && withOgrid; ++iar) {

    	CoEdge* arete = m_coedges[iar];

#ifdef _DEBUG_REVOL
    std::cout<<"arete = "<<arete->getName()<<std::endl;
#endif

    	{
    		filtre_coedge[arete] = 1;
    		//std::cout<<"arete "<<arete->getName()<<" marquée à "<<1<<std::endl;

    		const std::vector<Vertex* > & vertices = arete->getVertices();
    		for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
    				iter1 != vertices.end(); ++iter1)
    			filtre_vertex[*iter1] = 1;
    	}

    	// initialisation avec les 2 faces associées à l'arêtes
    	// on marque les arêtes et les sommets des 2 faces
    	{
    		// les 2 Faces communes
    		std::vector<CoFace* > cofaces;
    		arete->getCoFaces(cofaces);
    		if (cofaces.size() != 2){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    			message<<"Erreur, il faut sélectionner une arête entre 2 faces pour rechercher le tracé de l'ogrid.\n";
    			message<<"L'arête "<<arete->getName()<<" est relié à "<<(short)cofaces.size();
    			if (cofaces.size()>1)
    				message<<" faces";
    			else
    				message<<" face";
    			throw TkUtil::Exception (message);
    		}

    		uint marques[2];
    		if (isNearestAxis(cofaces[0], cofaces[1])){
    			marques[0] = 2;
    			marques[1] = 3;
    		}
    		else {
    			marques[1] = 2;
    			marques[0] = 3;
    		}

    		for (uint i=0; i<2; i++){
    			CoFace* coface = cofaces[i];

    			filtre_coface[coface] = marques[i];

    			std::vector<CoEdge* > coedges;
    			coface->getCoEdges(coedges);
    			for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    					iter2 != coedges.end(); ++iter2){
    				if (filtre_coedge[*iter2] == 0){
    					//std::cout<<"arete "<<(*iter2)->getName()<<" marquée à "<<marques[i]<<std::endl;
    					filtre_coedge[*iter2] = marques[i];
    				}

    				const std::vector<Vertex* > & vertices = (*iter2)->getVertices();
    				for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
    						iter1 != vertices.end(); ++iter1)
    					if (filtre_vertex[*iter1] == 0)
    						filtre_vertex[*iter1] = marques[i];
    			}
    		}
    	}

    	// on part d'un sommet de l'arête de départ (puis de l'autre)
    	// on va de proche en proche pour sélectionner toutes les arêtes
    	// qui vont constituer l'ogrid
    	for (uint i=0; i<arete->getNbVertices(); i++){

    		Vertex* vtx_dep = arete->getVertex(i);

    		do {
#ifdef _DEBUG_REVOL
    			std::cout<<"vtx_dep = "<<vtx_dep->getName()<<std::endl;
#endif
    			// recherche d'une arête suivante
    			CoEdge* coedge_suiv = 0;

    			std::vector<CoEdge* > coedges;
    			vtx_dep->getCoEdges(coedges);
#ifdef _DEBUG_REVOL
    			std::cout<<" -> coedges :";
    			for (uint i2=0; i2<coedges.size(); i2++)
    				std::cout<<" "<<coedges[i2]->getName();
    			std::cout<<std::endl;
#endif

    			if (isOnAxis(vtx_dep)){
    				// cas d'un sommet sur l'axe de symétrie
    				if (coedges.size() == 3){
    					// cas d'arret (par exemple, un quart de cercle avec ogrid qui se termine sur la dégénérescence
    				}
    				else if (coedges.size() == 4){
    					// cas d'un passage de l'o-grid au centre de la dégénérescence
    					// on va prendre l'arête qui est marquée comme étant au dessus
    					for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    							iter2 != coedges.end(); ++iter2)
    						if (filtre_coedge[*iter2] == 3)
    							coedge_suiv = *iter2;
    					if(0 == coedge_suiv)
    						throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête marquée au dessus parmi les 4 disponibles", TkUtil::Charset::UTF_8));
    				}
    				else if (coedges.size() == 5){
    					// idem au précédent avec une face coupée en 2 verticalement au centre
    					// on va prendre l'arête qui n'est marquée et qui est en dehors de l'axe
    					for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    							iter2 != coedges.end(); ++iter2){
    						Vertex* vtx_suiv = (*iter2)->getOppositeVertex(vtx_dep);
    						if (filtre_coedge[*iter2] == 0 && !isOnAxis(vtx_suiv))
    							coedge_suiv = *iter2;
    					}
    					if(0 == coedge_suiv)
    						throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête non marquée au dessus de l'axe parmi les 5 disponibles", TkUtil::Charset::UTF_8));
    				}
    				else {
    					throw TkUtil::Exception (TkUtil::UTF8String ("Erreur lors de la recherche du tracé de l'ogrid (bloque sur l'axe), avez-vous sélectionné l'arête judicieusement ?", TkUtil::Charset::UTF_8));
    				}
    				// on marque les arêtes sous l'ogrid
    				if (0 != coedge_suiv){
    					//std::cout<<"coedge_suiv = "<<coedge_suiv->getName()<<std::endl;
    					//std::cout<<"arete "<<coedge_suiv->getName()<<" marquée à "<<1<<std::endl;
    					filtre_coedge[coedge_suiv] = 1;
    					for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    							iter2 != coedges.end(); ++iter2)
    						if (filtre_coedge[*iter2] == 0){
    							//std::cout<<"arete "<<(*iter2)->getName()<<" marquée à "<<2<<std::endl;
    							filtre_coedge[*iter2] = 2;
    						}
    				}
    			}
    			else {
    				// cas d'un sommet hors de l'axe de symétrie
    				if (coedges.size() == 3){
    					// si le sommet est relié à 3 faces, on est certainement dans le cas d'un o-grid en 2D
    					// sinon c'est terminé
    					std::vector<CoFace* > cofaces_vtx;
    					vtx_dep->getCoFaces(cofaces_vtx);
#ifdef _DEBUG_REVOL
    					std::cout<<"   -> cofaces_vtx :";
    					for (uint i2=0; i2<cofaces_vtx.size(); i2++)
    						std::cout<<" "<<cofaces_vtx[i2]->getName();
    					std::cout<<std::endl;
#endif

    					if (cofaces_vtx.size() == 2){
    						// cas ou on a terminé
    					}
    					else if (cofaces_vtx.size() == 3){
    						// on va prendre l'arête qui n'est pas reliée à l'axe et qui est la seule à être marquée comme étant au dessus
    						for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    								iter2 != coedges.end(); ++iter2)
    							if (filtre_coedge[*iter2] == 3)
    								coedge_suiv = *iter2;
    						if(0 == coedge_suiv)
    							throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête marquée au dessus parmi les 3 disponibles", TkUtil::Charset::UTF_8));
    					}
    					else
    						throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve un nombre cohérents de faces autour d'un sommet sur l'o-grid", TkUtil::Charset::UTF_8));
    				}
    				else if (coedges.size() == 4){
    					// on recherche l'unique arête non marquée
    					for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    							iter2 != coedges.end(); ++iter2){
    						//std::cout<<"recherche ... "<<(*iter2)->getName()<<" a pour marque "<<filtre_coedge[*iter2]<<std::endl;
    						if (filtre_coedge[*iter2] == 0)
    							coedge_suiv = *iter2;
    					}
    					// s'il n'y en a pas, on cherche une arête marquée au dessus entourée de 2 faces, l'une marquée au dessus, l'autre non marquée
    					if(0 == coedge_suiv)
    						for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    								iter2 != coedges.end(); ++iter2)
    							if (filtre_coedge[*iter2] == 3){
    								std::vector<CoFace* > cofaces_coedge;
    								(*iter2)->getCoFaces(cofaces_coedge);
    								if (cofaces_coedge.size() == 2 &&
    										((filtre_coface[cofaces_coedge[0]] == 3 && filtre_coface[cofaces_coedge[1]] == 0)
    												||
    												(filtre_coface[cofaces_coedge[1]] == 3 && filtre_coface[cofaces_coedge[0]] == 0)))
    									coedge_suiv = *iter2;
    							}

    					if(0 == coedge_suiv)
    						throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête non marquée parmi les 4 disponibles", TkUtil::Charset::UTF_8));

    				}
//    				else if (coedges.size() == 5){
//    					// on cherche une arête marquée au dessus entourée de 2 faces, l'une marquée au dessus, l'autre non marquée
//    					for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
//    							iter2 != coedges.end(); ++iter2)
//    						if (filtre_coedge[*iter2] == 3){
//    							std::vector<CoFace* > cofaces_coedge;
//    							(*iter2)->getCoFaces(cofaces_coedge);
//    							if (cofaces_coedge.size() == 2 &&
//    									((filtre_coface[cofaces_coedge[0]] == 3 && filtre_coface[cofaces_coedge[1]] == 0)
//    											||
//												(filtre_coface[cofaces_coedge[1]] == 3 && filtre_coface[cofaces_coedge[0]] == 0)))
//    								coedge_suiv = *iter2;
//    						}
//
//    					if(0 == coedge_suiv)
//    						throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête non marquée parmi les 5 disponibles", TkUtil::Charset::UTF_8));
//
//    				}
    				else {
    					throw TkUtil::Exception (TkUtil::UTF8String ("Erreur lors de la recherche du tracé de l'ogrid, avez-vous sélectionné l'arête judicieusement ?", TkUtil::Charset::UTF_8));
    				}
    			}


    			if (0 != coedge_suiv){
    				//std::cout<<"coedge_suiv = "<<coedge_suiv->getName()<<std::endl;
    				//std::cout<<"arete "<<coedge_suiv->getName()<<" marquée à "<<1<<std::endl;
    				filtre_coedge[coedge_suiv] = 1;

    				// marque ce qui ne l'est pas déjà
    				for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
    						iter2 != coedges.end(); ++iter2)
    					if (filtre_coedge[*iter2] > 1) {
    						uint marque = filtre_coedge[*iter2];
    						marqueCoFaceCoEdgeVertices(*iter2, marque, filtre_vertex, filtre_coedge, filtre_coface);
    					}

    				Vertex* vtx_suiv = coedge_suiv->getOppositeVertex(vtx_dep);
    				filtre_vertex[vtx_suiv] = 1;
    				vtx_dep = vtx_suiv;
    			}
    			else {
    				vtx_dep = 0;
    				//std::cout<<"coedge_suiv non trouvée."<<std::endl;
    			}

    		} while (0 != vtx_dep);

    	} // end for i<arete->getNbVertices()

    } // end for iar<m_coedges.size()

    // on marque les arêtes sélectionnées comme si elles étaient au dessus de l'o-grid
    if (!withOgrid)
    	for (uint iar=0; iar<m_coedges.size(); ++iar)
    		filtre_coedge[m_coedges[iar]] = 3;


	// il faut marquer les faces, arêtes et sommets qui n'ont pas été vus lors
	// de la recherche précédente (faces qui ne touchent pas l'ogrid)
	{
		uint marques[2];
		marques[0] = 2; // le dessous
		marques[1] = 3; // le dessus

		for (uint i=0; i<2; i++){
			std::vector<CoEdge*> coedges;
			TopoHelper::getMarqued(filtre_coedge, marques[i], coedges);

			uint old_size = 0;

			do {
				old_size = coedges.size();

				// marque ce qui ne l'est pas déjà
				for (std::vector<CoEdge*>::iterator iter2 = coedges.begin();
						iter2 != coedges.end(); ++iter2)
					if (filtre_coedge[*iter2] == marques[i])
						marqueCoFaceCoEdgeVertices(*iter2, marques[i], filtre_vertex, filtre_coedge, filtre_coface);

				// on boucle tant que le nombre d'arêtes marquées change
				TopoHelper::getMarqued(filtre_coedge, marques[i], coedges);

			} while (old_size != coedges.size());
		}
	}

	// cas où il faut chercher les sommets et les arêtes sur l'axe pour les marquer
	if (!withOgrid){
		std::vector<CoEdge*> coedges;
		TopoHelper::getMarqued(filtre_coedge, 3, coedges);
		for (std::vector<CoEdge*>::iterator iter2 = coedges.begin();
				iter2 != coedges.end(); ++iter2){
			if (isOnAxis((*iter2)->getVertex(0)))
				filtre_vertex[(*iter2)->getVertex(0)] = 1;
			if (isOnAxis((*iter2)->getVertex(1)))
				filtre_vertex[(*iter2)->getVertex(1)] = 1;
			if (filtre_vertex[(*iter2)->getVertex(0)] == 1 && filtre_vertex[(*iter2)->getVertex(1)] == 1)
				filtre_coedge[(*iter2)] = 1;
		}
	}

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
computeAxisFilters(std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface)
{
    // on marque en premier les sommets
    for (std::map<Vertex*, uint>::iterator iter = filtre_vertex.begin();
            iter != filtre_vertex.end(); ++iter)
        if ((*iter).second && isOnAxis((*iter).first))
                (*iter).second += 10;

    // on marque ensuite les CoEdges
    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedge.begin();
            iter != filtre_coedge.end(); ++iter)
        if ((*iter).second) {
            CoEdge* coedge = (*iter).first;
            if (coedge->getNbVertices() != 2)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandMakeBlocksByRevol::computeAxisFilters trouve une arête avec autre chose que 2 sommets", TkUtil::Charset::UTF_8));
            Vertex* vtx1 = coedge->getVertex(0);
            Vertex* vtx2 = coedge->getVertex(1);

            bool vtx1OnAxis = (filtre_vertex[vtx1]>=10);
            bool vtx2OnAxis = (filtre_vertex[vtx2]>=10);

            if (vtx1OnAxis && vtx2OnAxis)
                (*iter).second += 20;
            else if (vtx1OnAxis || vtx2OnAxis)
                (*iter).second += 10;
        }

    // on marque ensuite les CoFace (+10 si elle touche l'axe)
    for (std::map<CoFace*, uint>::iterator iter = filtre_coface.begin();
            iter != filtre_coface.end(); ++iter)
        if ((*iter).second) {
            CoFace* coface = (*iter).first;
            std::vector<CoEdge* > coedges;
            coface->getCoEdges(coedges);
            bool cofaceOnAxis = false;
            for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
                    iter2 != coedges.end(); ++iter2)
                if (filtre_coedge[*iter2] >= 10)
                    cofaceOnAxis = true;
            if (cofaceOnAxis)
                (*iter).second += 10;
        }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
detectVerticesUnderOgrid(std::map<Vertex*, uint>& filtre_vertex)
{
    for (std::map<Vertex*, uint>::iterator iter = filtre_vertex.begin();
            iter != filtre_vertex.end(); ++iter)
        if (filtre_vertex[(*iter).first] == 2){
            // on a trouvé un sommet entre l'axe et l'ogrid
            Vertex* vtx0 = (*iter).first;

            std::vector<CoEdge* > edges;
            vtx0->getCoEdges(edges);
            CoEdge* coedgeAxe = 0;
            CoEdge* coedgeOgrid = 0;
            for (std::vector<CoEdge* >::iterator iter=edges.begin();
                    iter != edges.end(); ++iter){
                Vertex* vtxOpp = (*iter)->getOppositeVertex(vtx0);
                if (filtre_vertex[vtxOpp] == 1){
                    coedgeOgrid = *iter;
                    filtre_vertex[vtxOpp] = 6;
                }
                else if (filtre_vertex[vtxOpp] == 12)
                    coedgeAxe = *iter;
            }
            if (0 == coedgeAxe){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "La détection des sommets entre l'axe et le tracé de l'ogrid a échoué avec le sommet "<<vtx0->getName()
                                        <<", on ne trouve pas d'arête reliée à l'axe";
                throw TkUtil::Exception(messErr);
            }

            if (0 == coedgeOgrid){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "La détection des sommets entre l'axe et le tracé de l'ogrid a échoué avec le sommet "<<vtx0->getName()
                                        <<", on ne trouve pas d'arête reliée à l'ogrid";
                throw TkUtil::Exception(messErr);
            }
        }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
computeNi(std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<Vertex*, uint>& ni_vtx,
        std::map<CoEdge*, uint>& ni_coedge,
        std::map<CoFace*, uint>& ni_coface)
{
#ifdef _DEBUG_REVOL
    std::cout<<"computeNi"<<std::endl;
#endif
    // la coedge qui détermine le ni
    CoEdge* coedge_ref = 0;

    // on parcours les arêtes, on se base sur celles marquées à 12 (liées à l'axe et à l'ogrid ou à un point entre les 2)
    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedge.begin();
            iter != filtre_coedge.end(); ++iter)
        if ((*iter).second == 12) {
            CoEdge* coedge = (*iter).first;
            uint ni_loc = 0;
            if (coedge->getNbVertices() != 2)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandMakeBlocksByRevol::computeNi trouve une arête avec autre chose que 2 sommets", TkUtil::Charset::UTF_8));
            Vertex* vtx1 = coedge->getVertex(0);
            Vertex* vtx2 = coedge->getVertex(1);
            // sommet sur l'ogrid
            Vertex* vtxo = 0;
            if (filtre_vertex[vtx1] == 1)
            	vtxo = vtx1;
            else if (filtre_vertex[vtx2] == 1)
            	vtxo = vtx2;

            if (filtre_vertex[vtx1] == 2 || filtre_vertex[vtx2] == 2){
                // on doit chercher l'arête reliée à l'ogrid
                Vertex* vtx0 = (filtre_vertex[vtx1] == 2 ? vtx1 : vtx2);
                vtxo = vtx0;

                std::vector<CoEdge* > edges;
                vtx0->getCoEdges(edges);
                CoEdge* coedge2 = 0;
                for (std::vector<CoEdge* >::iterator iter=edges.begin();
                        iter != edges.end(); ++iter){
                    Vertex* vtxOpp = (*iter)->getOppositeVertex(vtx0);
                    if (filtre_vertex[vtxOpp] == 6)
                        coedge2 = *iter;
                }

                if (0 == coedge2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandMakeBlocksByRevol::computeNi ne trouve pas l'arête au dessus du point entre axe et ogrid", TkUtil::Charset::UTF_8));

                ni_loc = coedge->getNbMeshingEdges() + coedge2->getNbMeshingEdges();
            }
            else {
                ni_loc = coedge->getNbMeshingEdges();

                const std::vector<Edge*>& edges = coedge->getEdges();
                for (uint i=0; i<edges.size(); i++){
                	uint ni_edge = edges[i]->getNbMeshingEdges();
                	const std::vector<CoFace*>& cofaces = edges[i]->getCoFaces();
                	for (uint j=0; j<cofaces.size(); j++){
                		ni_coface[cofaces[j]] = ni_edge;
#ifdef _DEBUG_REVOL
                		std::cout<<"ni_coface pour "<<cofaces[j]->getName()<<" = "<<ni_edge<<std::endl;
#endif
                	}
                }

            }

            if (vtxo){
            	ni_vtx[vtxo] = ni_loc;
#ifdef _DEBUG_REVOL
                std::cout<<"ni_vtx pour "<<vtxo->getName()<<" = "<<ni_loc<<std::endl;
#endif
            }
            else
            	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandMakeBlocksByRevol::computeNi ne trouve pas le sommet sur l'ogrid", TkUtil::Charset::UTF_8));

            if (m_ni == 0){
            	m_ni = ni_loc;
            	coedge_ref = coedge;
            }
            else if (m_ni != ni_loc) {

            	// on prend le plus raffiné
            	if (ni_loc>m_ni){
            		m_ni = ni_loc;
            		coedge_ref = coedge;
            	}
            }
        } // end if ((*iter).second == 12)


#ifdef _DEBUG_REVOL
    std::cout<<"CommandMakeBlocksByRevol::computeNi trouve m_ni = "<<m_ni<<std::endl;
#endif

    // on donne une valeur par défaut (cas sans o-grid)
    if (m_ni == 0)
    	m_ni = getContext().getTopoManager().getDefaultNbMeshingEdges();


    // copie le ni d'une coface sous l'ogrid vers celle au dessus
    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedge.begin();
            iter != filtre_coedge.end(); ++iter)
        if ((*iter).second == 1) {
            CoEdge* coedge = (*iter).first;
            std::vector<CoFace*> cofaces;
            coedge->getCoFaces(cofaces);

            if (cofaces.size() == 2){
            	uint ni_loc = 0;
            	if (ni_coface[cofaces[0]] != 0){
            		ni_coface[cofaces[1]] = ni_coface[cofaces[0]];
#ifdef _DEBUG_REVOL
            		std::cout<<"ni_coface pour "<<cofaces[1]->getName()<<" = "<<ni_coface[cofaces[0]]<<std::endl;
#endif
            	}
            	else if (ni_coface[cofaces[1]] != 0){
            		ni_coface[cofaces[0]] = ni_coface[cofaces[1]];
#ifdef _DEBUG_REVOL
            		std::cout<<"ni_coface pour "<<cofaces[0]->getName()<<" = "<<ni_coface[cofaces[1]]<<std::endl;
#endif
            	}
            } // end if (cofaces.size() == 2)
        }  // end if ((*iter).second == 1)


    // Calcul les ni pour les sommets au dessus de l'ogrid
    for (std::map<Vertex*, uint>::iterator iter = filtre_vertex.begin();
               iter != filtre_vertex.end(); ++iter){
    	Vertex* vtx = (*iter).first;
           if ((*iter).second == 3 && ni_vtx[vtx] == 0) {

               // on prend le plus grand des ni des cofaces voisines
               uint ni_loc = 0;
               std::vector<CoFace*> cofaces;
               vtx->getCoFaces(cofaces);
               for (uint j=0; j<cofaces.size(); j++){
            	   uint ni = ni_coface[cofaces[j]];
            	   if (ni > ni_loc)
            		   ni_loc = ni;
               }
               if (ni_loc == 0)
            	   ni_loc = m_ni;

               ni_vtx[vtx] = ni_loc;
#ifdef _DEBUG_REVOL
               std::cout<<"ni_vtx pour "<<vtx->getName()<<" = "<<ni_loc<<std::endl;
#endif

           } // end if ((*iter).second == 3 && ni_vtx[vtx] == 0)
    } // end for iter = filtre_vertex.begin()

    // boucle sur coedges pour calculer un ni par coedge
    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedge.begin();
            iter != filtre_coedge.end(); ++iter){
    	// on prend le ni le plus grand entre ceux des cofaces autours
    	uint ni = 0;
    	CoEdge* coedge = iter->first;
    	std::vector<CoFace*> cofaces;
    	coedge->getCoFaces(cofaces);

    	for (uint i=0; i<cofaces.size(); i++){
    		CoFace* coface = cofaces[i];

    		uint ni_loc = ni_coface[coface];
    		if (ni_loc == 0)
    			ni_loc = m_ni;
    		if (ni_loc > ni)
    			ni = ni_loc;
    	}

    	ni_coedge[coedge] = ni;
#ifdef _DEBUG_REVOL
    	std::cout<<"ni_coedge pour "<<coedge->getName()<<" = "<<ni<<std::endl;
#endif

    } // end for iter = filtre_coedge.begin()

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::copyNi(std::map<Vertex*, uint>& ni_vtx,
        std::map<CoEdge*, uint>& ni_coedge,
        std::map<CoFace*, uint>& ni_coface,
		std::vector<CoFace*>& cofaces_dep,
		std::vector<CoFace*>& cofaces_arr)
{
#ifdef _DEBUG_REVOL
    std::cout<<"CommandMakeBlocksByRevol::copyNi"<<std::endl;
#endif
	std::vector<CoFace*>::iterator iter1 = cofaces_dep.begin();
	std::vector<CoFace*>::iterator iter2 = cofaces_arr.begin();

    for ( ; iter1 != cofaces_dep.end(); ++iter1, ++iter2){
    	ni_coface[*iter2] = ni_coface[*iter1];
#ifdef _DEBUG_REVOL
    	std::cout<<"ni_coface pour "<<(*iter2)->getName()<<" = "<<ni_coface[*iter1]<<" par copie de "<<(*iter1)->getName()<<std::endl;
#endif

    	std::vector<CoEdge*> coedges1;
    	std::vector<CoEdge*> coedges2;
    	(*iter1)->getCoEdges(coedges1, false);
    	(*iter2)->getCoEdges(coedges2, false);

    	std::vector<CoEdge*>::const_iterator iter3 = coedges1.begin();
    	std::vector<CoEdge*>::const_iterator iter4 = coedges2.begin();
    	for ( ; iter3 != coedges1.end(); ++iter3, ++iter4){
    		ni_coedge[*iter4] = ni_coedge[*iter3];
#ifdef _DEBUG_REVOL
    		std::cout<<"ni_coedge pour "<<(*iter4)->getName()<<" = "<<ni_coedge[*iter3]<<" par copie de "<<(*iter3)->getName()<<std::endl;
#endif

        	std::vector<Vertex*> vertices1;
        	std::vector<Vertex*> vertices2;
        	(*iter3)->getVertices(vertices1);
        	(*iter4)->getVertices(vertices2);

        	std::vector<Vertex*>::const_iterator iter5 = vertices1.begin();
        	std::vector<Vertex*>::const_iterator iter6 = vertices2.begin();
        	for ( ; iter5 != vertices1.end(); ++iter5, ++iter6){
        		ni_vtx[*iter6] = ni_vtx[*iter5];
#ifdef _DEBUG_REVOL
        		std::cout<<"ni_vtx pour "<<(*iter6)->getName()<<" = "<<ni_vtx[*iter5]<<" par copie de "<<(*iter5)->getName()<<std::endl;
#endif
        	} // end for iter5 != vertices1.end()
    	} // end for iter3 != coedges1.end()
    } // end for iter1 != cofaces_dep.end()

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
marqueCoFaceCoEdgeVertices(CoEdge* coedge, const uint marque,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface)
{
	//std::cout<<"marqueCoFaceCoEdgeVertices ..."<<std::endl;
    std::vector<CoFace* > cofaces;
    coedge->getCoFaces(cofaces);

    for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
            iter3 != cofaces.end(); ++iter3)
        if (filtre_coface[*iter3] == 0){
            filtre_coface[*iter3] = marque;
            //std::cout<<"coface "<<(*iter3)->getName()<<" marquée à "<<marque<<std::endl;

            std::vector<CoEdge* > coedges;
            (*iter3)->getCoEdges(coedges);

            for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
                    iter2 != coedges.end(); ++iter2)
                if (filtre_coedge[*iter2] == 0){
                    filtre_coedge[*iter2] = marque;
                    //std::cout<<"coedge "<<(*iter2)->getName()<<" marquée à "<<marque<<std::endl;

                    const std::vector<Vertex* > & vertices = (*iter2)->getVertices();

                    for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
                            iter1 != vertices.end(); ++iter1)
                        if (filtre_vertex[*iter1] == 0){
                            filtre_vertex[*iter1] = marque;
                        }
                }
        }
}
/*----------------------------------------------------------------------------*/
bool CommandMakeBlocksByRevol::
isOnAxis(Vertex* vtx)
{
    if (m_axis == XAxis){
        return Utils::Math::MgxNumeric::isNearlyZero(vtx->getY()) && Utils::Math::MgxNumeric::isNearlyZero(vtx->getZ());
    } else if (m_axis == YAxis){
        return Utils::Math::MgxNumeric::isNearlyZero(vtx->getX()) && Utils::Math::MgxNumeric::isNearlyZero(vtx->getZ());
    } else if (m_axis == ZAxis){
        return Utils::Math::MgxNumeric::isNearlyZero(vtx->getX()) && Utils::Math::MgxNumeric::isNearlyZero(vtx->getY());
    } else {
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandMakeBlocksByRevol::isOnAxis est à terminer pour le cas d'une révolution autre ...", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
bool CommandMakeBlocksByRevol::
isNearestAxis(CoFace* coface1, CoFace* coface2)
{
    std::vector<CoFace*> cofaces;
    cofaces.push_back(coface1);
    cofaces.push_back(coface2);
    // on distingue les 2 faces suivant la sommes des coordonnées de ses sommets dans les autres directions

    if (m_axis == XAxis){
        double sommesCoords[2];
        for (uint i=0; i<2; i++){
            sommesCoords[i] = 0.0;
            const std::vector<Vertex* > & vertices = cofaces[i]->getVertices();

            for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
                    iter1 != vertices.end(); ++iter1)
                sommesCoords[i] += (*iter1)->getY() + (*iter1)->getZ(); // l'une est certainement nulle
        }
        return sommesCoords[0] < sommesCoords[1];
    } else if (m_axis == YAxis){
        double sommesCoords[2];
        for (uint i=0; i<2; i++){
            sommesCoords[i] = 0.0;
            const std::vector<Vertex* > & vertices = cofaces[i]->getVertices();

            for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
                    iter1 != vertices.end(); ++iter1)
                sommesCoords[i] += (*iter1)->getX() + (*iter1)->getZ(); // l'une est certainement nulle
        }
        return sommesCoords[0] < sommesCoords[1];
    } else if (m_axis == ZAxis){
        double sommesCoords[2];
        for (uint i=0; i<2; i++){
            sommesCoords[i] = 0.0;
            const std::vector<Vertex* > & vertices = cofaces[i]->getVertices();

            for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
                    iter1 != vertices.end(); ++iter1)
                sommesCoords[i] += (*iter1)->getX() + (*iter1)->getY(); // l'une est certainement nulle
        }
        return sommesCoords[0] < sommesCoords[1];
    } else {
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandMakeBlocksByRevol::isNearestAxis est à terminer pour le cas d'une révolution autre ...", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
rotate(uint angle,
        std::vector<CoFace*>& cofaces)
{
    // on marque les sommets pour ne les toucher qu'une fois
    std::map<Vertex*, uint> filtre_vu;
    // on marque les arêtes pour ne les toucher qu'une fois
    std::map<CoEdge*, uint> filtre_ce;

    // la matrice de rotation avec un angle en radians
    m_rotMatrix = Utils::Math::Matrix33(angle*M_PI/180.0, m_axis);

    for (std::vector<CoFace*>::const_iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){

        const std::vector<Edge* > & edges = (*iter1)->getEdges();
        getInfoCommand().addTopoInfoEntity(*iter1,Internal::InfoCommand::DISPMODIFIED);

        for (std::vector<Edge* >::const_iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){

            const std::vector<CoEdge* > & coedges = (*iter2)->getCoEdges();
            getInfoCommand().addTopoInfoEntity(*iter2,Internal::InfoCommand::DISPMODIFIED);

            for (std::vector<CoEdge* >::const_iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3){

            	if (filtre_ce[*iter3] == 0)
            		rotate(angle, *iter3);

            	filtre_ce[*iter3] = 1;

                const std::vector<Vertex* > & vertices = (*iter3)->getVertices();
                getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DISPMODIFIED);

                for (std::vector<Vertex* >::const_iterator iter4 = vertices.begin();
                        iter4 != vertices.end(); ++iter4){

                    if (filtre_vu[*iter4] == 0){

                        rotate(angle, *iter4);
                        getInfoCommand().addTopoInfoEntity(*iter4,Internal::InfoCommand::DISPMODIFIED);

                        filtre_vu[*iter4] = 1;
                    }
                } // end for vertices
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
rotate(uint angle,
        Vertex* vtx)
{
    if (isOnAxis(vtx))
        return;

    vtx->saveVertexGeomProperty(&getInfoCommand());
    Utils::Math::Point oldPos = vtx->getCoord();
    Utils::Math::Point newPos = m_rotMatrix * oldPos;
    vtx->setCoord(newPos);
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
rotate(uint angle,
            CoEdge* co)
{
	if (co->getMeshingProperty()->isPolarCut()){
		Utils::Math::Point oldPos = co->getMeshingProperty()->getPolarCenter();
		Utils::Math::Point newPos = m_rotMatrix * oldPos;
		co->saveCoEdgeMeshingProperty(&getInfoCommand());
		co->getMeshingProperty()->setPolarCenter(newPos);
	}
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
scale(double ratio,
        std::vector<Vertex*>& vertices)
{
    for (std::vector<Vertex* >::iterator iter4 = vertices.begin();
                                iter4 != vertices.end(); ++iter4){
        Vertex* vtx = *iter4;
        getInfoCommand().addTopoInfoEntity(vtx,Internal::InfoCommand::DISPMODIFIED);
        if (!isOnAxis(vtx)){

            vtx->saveVertexGeomProperty(&getInfoCommand());
            Utils::Math::Point newPos = vtx->getCoord();

            if (m_axis == XAxis){
                newPos.setY(newPos.getY() * ratio);
                newPos.setZ(newPos.getZ() * ratio);
            } else if (m_axis == YAxis){
                newPos.setX(newPos.getX() * ratio);
                newPos.setZ(newPos.getZ() * ratio);
            } else if (m_axis == ZAxis){
                newPos.setX(newPos.getX() * ratio);
                newPos.setY(newPos.getY() * ratio);
            } else {
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandMakeBlocksByRevol::scale est à terminer pour le cas d'une révolution autre ...", TkUtil::Charset::UTF_8));
            }

            vtx->setCoord(newPos);
        }
    }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
getMarquedOnOgrid(std::vector<CoFace*>& cofaces,
        std::map<Vertex*, uint>& filtre_vertex,
        std::vector<Vertex*>& vertices_ogrid)
{
    // filtre sur les sommets pour ne les prendre qu'une fois
    std::map<Vertex*, uint> filtre_vu;

    for (std::vector<CoFace*>::const_iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){

        const std::vector<Edge* > & edges = (*iter1)->getEdges();

        for (std::vector<Edge* >::const_iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){

            const std::vector<CoEdge* > & coedges = (*iter2)->getCoEdges();

            for (std::vector<CoEdge* >::const_iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3){

                const std::vector<Vertex* > & vertices = (*iter3)->getVertices();

                for (std::vector<Vertex* >::const_iterator iter4 = vertices.begin();
                        iter4 != vertices.end(); ++iter4){

                    if (filtre_vertex[*iter4]%5 == 1 && filtre_vu[*iter4] == 0){
                        vertices_ogrid.push_back(*iter4);
                        filtre_vu[*iter4] = 1;
                    }
                } // end for vertices
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
freeUnused(std::vector<CoFace*>& cofaces,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface,
        bool memorizeFreezedCofaces)
{
    // les CoFace sous l'o-grid n'ont plus de raison d'être, on les supprime
    for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1)
        if (filtre_coface[*iter1]%10 == 2){
            CoFace* coface = *iter1;

            std::vector<Edge* > edges;
            (*iter1)->getEdges(edges);

            for (std::vector<Edge* >::iterator iter2 = edges.begin();
                    iter2 != edges.end(); ++iter2){

                std::vector<CoEdge* > coedges;
                (*iter2)->getCoEdges(coedges);

                for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                        iter3 != coedges.end(); ++iter3)
                    if ((filtre_coedge[*iter3] == 2 || filtre_coedge[*iter3] == 12)
                            && (filtre_vertex[(*iter3)->getVertex(0)] != 2
                                    || filtre_vertex[(*iter3)->getVertex(1)] != 2)){

                        // supprime les arêtes sous l'ogrid sauf celles sur l'axe et celles // à l'axe
                        (*iter3)->setDestroyed(true);
                        getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DELETED);
                        (*iter3)->free(&getInfoCommand());

                    } // end for coedges sous l'ogrid
                    else if (m_portion == Utils::Portion::ENTIER && filtre_coedge[*iter3] == 22) {
                        // cas d'une arête sur l'axe
                        // on supprime égallement ses sommets
                        std::vector<Vertex* > vertices;
                        (*iter3)->getVertices(vertices);
                        for (std::vector<Vertex* >::iterator iter4 = vertices.begin();
                        		iter4 != vertices.end(); ++iter4)
                        	// on ne détruit pas un sommet sur l'ogrid (cas de l'ogrid qui passe par l'axe)
                        	if (filtre_vertex[*iter4] == 12){
                        		(*iter4)->setDestroyed(true);
                        		getInfoCommand().addTopoInfoEntity(*iter4,Internal::InfoCommand::DELETED);
                        	}
                        	else {
                        		// la géométrie 2D étant détruite, on supprime l'association
                        		(*iter4)->saveTopoProperty();
                        		getInfoCommand().addTopoInfoEntity(*iter4,Internal::InfoCommand::OTHERMODIFIED);
#ifdef _DEBUG_REVOL
                        		std::cout<<"freeUnused: setGeomAssociation(0) pour "<<(*iter4)->getName()<<std::endl;
#endif
                        		(*iter4)->setGeomAssociation(0);
                        	}

                        (*iter3)->setDestroyed(true);
                        getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DELETED);
                        (*iter3)->free(&getInfoCommand());

                    }
            } // end for edges

            coface->free(&getInfoCommand());
            // il faut repérer les faces qui sont dans le manager
            if (memorizeFreezedCofaces)
                m_deleted_cofaces.push_back(coface);

            // on fait le ménage parmi les Edge qui ne sont plus référencées par aucune CoFace
            for (std::vector<Edge* >::iterator iter2 = edges.begin();
                                iter2 != edges.end(); ++iter2){
            	if ((*iter2)->getNbCoFaces() == 0){
            		(*iter2)->setDestroyed(true);
            		getInfoCommand().addTopoInfoEntity(*iter2,Internal::InfoCommand::DELETED);
            		(*iter2)->free(&getInfoCommand());
            	}
            }

        } // end for cofaces sous l'ogrid
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
updateInterpolate(std::vector<CoFace*>& cofaces,
    		std::map<CoEdge*, uint>& filtre_coedge,
			std::map<CoFace*, uint>& filtre_coface)
{
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"updateInterpolate en cours"<<std::endl;
#endif
	for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
			iter1 != cofaces.end(); ++iter1)
		if (filtre_coface[*iter1] == 3){
			CoFace* coface = *iter1;

			std::vector<Edge* > edges;
			coface->getEdges(edges);

			std::vector<CoEdge* > coface_coedges;
			coface->getCoEdges(coface_coedges, false);

			for (std::vector<Edge* >::iterator iter2 = edges.begin();
					iter2 != edges.end(); ++iter2){

				std::vector<CoEdge* > coedges;
				(*iter2)->getCoEdges(coedges);

				for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
						iter3 != coedges.end(); ++iter3)
					if (filtre_coedge[*iter3] == 3 && (*iter3)->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
						EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyInterpolate*>((*iter3)->getMeshingProperty());
						CHECK_NULL_PTR_ERROR(interpol);
						std::vector<std::string> coedges_names = interpol->getCoEdges();
						for (uint i=0; i<coedges_names.size(); i++){
							CoEdge* coedge = getContext().getLocalTopoManager().getCoEdge(coedges_names[i], false);

							if (filtre_coedge[coedge] == 22){
#ifdef _DEBUG_INTERPOLATE
								std::cout<<" Arête à traiter "<<(*iter3)->getName()<<" car elle pointe sur "<<coedge->getName()<<" qui est à détruire"<<std::endl;
#endif

								// recherche si l'une des autres arêtes est sur l'ogrid
								for (std::vector<CoEdge* >::iterator iter4 = coface_coedges.begin();
														iter4 != coface_coedges.end(); ++iter4)
									if (filtre_coedge[*iter4] == 1){
#ifdef _DEBUG_INTERPOLATE
										std::cout<<" Arête qui est sur l'ogrid "<<(*iter4)->getName()<<std::endl;
#endif
										// on fait le changement ...
										EdgeMeshingPropertyInterpolate* new_interpol = (EdgeMeshingPropertyInterpolate*)interpol->clone();
										std::vector<std::string> new_coedges_names;
										new_coedges_names.push_back((*iter4)->getName());
										new_interpol->setCoEdges(new_coedges_names);
										(*iter3)->switchCoEdgeMeshingProperty(&getInfoCommand(), new_interpol);
										delete new_interpol;

									}

							}
						}
					}
			}// end for edges
		}// end for cofaces
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"updateInterpolate terminé"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolCoEdges(std::vector<CoFace*>& cofaces_0,
        std::vector<CoFace*>& cofaces_1,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        uint nbDemiOgrid,
        bool sensNormal,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<Vertex*, uint>& ni_vtx,
		bool preserveSurfaceAssociation)
{
#ifdef _DEBUG_REVOL
    std::cout<<"constructRevolCoEdges avec "<<cofaces_0.size()<<" et "<<cofaces_1.size()<<" CoFace"<<std::endl;
#endif
    // filtre sur les sommets pour ne les prendre qu'une fois
    std::map<Vertex*, uint> filtre_vu;


    // construction des arêtes communes avec ni bras entre les sommets des 2 topo 2D
    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolCoEdges avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        std::vector<Edge* > edges_0;
        std::vector<Edge* > edges_1;
        (*iter1_0)->getEdges(edges_0);
        (*iter1_1)->getEdges(edges_1);

        if (edges_0.size() != edges_1.size())
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolCoEdges avec des nombres de Edge différents", TkUtil::Charset::UTF_8));

        std::vector<Edge* >::iterator iter2_0 = edges_0.begin();
        std::vector<Edge* >::iterator iter2_1 = edges_1.begin();

        for (; iter2_0 != edges_0.end(); ++iter2_0, ++iter2_1){

            std::vector<CoEdge* > coedges_0;
            std::vector<CoEdge* > coedges_1;
            (*iter2_0)->getCoEdges(coedges_0);
            (*iter2_1)->getCoEdges(coedges_1);

            if (coedges_0.size() != coedges_1.size())
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolCoEdges avec des nombres de CoEdge différents", TkUtil::Charset::UTF_8));

            std::vector<CoEdge* >::iterator iter3_0 = coedges_0.begin();
            std::vector<CoEdge* >::iterator iter3_1 = coedges_1.begin();

            for (; iter3_0 != coedges_0.end(); ++iter3_0, ++iter3_1){

                std::vector<Vertex* > vertices_0;
                std::vector<Vertex* > vertices_1;
                (*iter3_0)->getVertices(vertices_0);
                (*iter3_1)->getVertices(vertices_1);

                if (vertices_0.size() != vertices_1.size())
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolCoEdges avec des nombres de Vertex différents", TkUtil::Charset::UTF_8));

                std::vector<Vertex* >::iterator iter4_0 = vertices_0.begin();
                std::vector<Vertex* >::iterator iter4_1 = vertices_1.begin();

                for (; iter4_0 != vertices_0.end(); ++iter4_0, ++iter4_1){
                    Vertex* vtx0 = *iter4_0;
                    Vertex* vtx1 = *iter4_1;
                    // le ni pour ce sommet
                    uint ni_loc = ni_vtx[vtx0];
                    if (ni_loc == 0)
                    	ni_loc = m_ni;
#ifdef _DEBUG_REVOL
                    std::cout<<"ni_loc pour "<<vtx0->getName()<<" = "<<ni_loc<<std::endl;
#endif

                    if (filtre_vu[vtx0] == 0){
                        if (filtre_vertex[vtx0] == 2){
                            // construction d'une arête avec autant de bras que celle reliée à l'axe

                            // on recherche le nombre de bras entre le sommet et l'axe,
                            uint nbBrasVersAxe = 0;
                            // pour cela on recherche les arêtes reliées à l'axe
                            {
                                std::vector<CoEdge* > edges;
                                vtx0->getCoEdges(edges);
                                CoEdge* coedgeAxe = 0;
                                for (std::vector<CoEdge* >::iterator iter=edges.begin();
                                        iter != edges.end(); ++iter){
                                    Vertex* vtxOpp = (*iter)->getOppositeVertex(vtx0);
                                    if (filtre_vertex[vtxOpp] == 12)
                                        coedgeAxe = *iter;
                                }
                                if (0 == coedgeAxe){
									TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                                    messErr << "La construction d'une arête par révolution a échouée pour le sommet "<<vtx0->getName()
                                                <<", sommet entre l'axe et le tracé de l'ogrid, on ne trouve pas d'arête reliée à l'axe";
                                    throw TkUtil::Exception(messErr);
                                }
                                else
                                    nbBrasVersAxe = coedgeAxe->getNbMeshingEdges();
                            }

                            EdgeMeshingPropertyUniform empAxe(nbBrasVersAxe*nbDemiOgrid);
                            CoEdge* newCoEdge = new Topo::CoEdge(getContext(), &empAxe, vtx0, vtx1);
                            getInfoCommand().addTopoInfoEntity(newCoEdge, Internal::InfoCommand::CREATED);
#ifdef _DEBUG_REVOL
                            std::cout<<"CoEdge (courte) "<<newCoEdge->getName()<<" entre  "<<vtx0->getName()<<" et "<<vtx1->getName()<<std::endl;
#endif
                            vtx2coedges[vtx0].push_back(newCoEdge);
                            filtre_coedge[newCoEdge] = 4;
                            // mise à jour de la projection suivant la géom qui a été extrudée
                            updateGeomAssociation(vtx0->getGeomAssociation(), newCoEdge);
                        }
                        else if (filtre_vertex[vtx0] == 6){
                            // il faut faire 2 ou 3 arêtes (3 dans le cas nbDemiOgrid = 2)

                            // recherche du nombre de bras entre ce sommet et le sommet vers l'axe
                            uint nbBrasVersOgrid = 0;
                            {
                                std::vector<CoEdge* > edges;
                                vtx0->getCoEdges(edges);
                                CoEdge* coedgeAxe = 0;
                                for (std::vector<CoEdge* >::iterator iter=edges.begin();
                                        iter != edges.end(); ++iter){
                                    Vertex* vtxOpp = (*iter)->getOppositeVertex(vtx0);
                                    if (filtre_vertex[vtxOpp] == 2)
                                        coedgeAxe = *iter;
                                }
                                if (0 == coedgeAxe){
									TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                                    messErr << "La construction d'une arête par révolution a échouée pour le sommet "<<vtx0->getName()
                                            <<", sommet sur l'ogrid, on ne trouve pas d'arête en direction de l'axe";
                                    throw TkUtil::Exception(messErr);
                                }
                                else
                                    nbBrasVersOgrid = coedgeAxe->getNbMeshingEdges();
                            }
                            uint nbBrasVersAxe = ni_loc - nbBrasVersOgrid;
                            EdgeMeshingPropertyUniform empAxe(nbBrasVersAxe*nbDemiOgrid);
                            EdgeMeshingPropertyUniform empOgrid(nbBrasVersOgrid);

                            if (nbDemiOgrid == 1){
                                Vertex* vtx2 = vtx0->clone();
                                getInfoCommand().addTopoInfoEntity(vtx2, Internal::InfoCommand::CREATED);
                                updateGeomAssociationToSurf(vtx0->getGeomAssociation(), vtx2);
                                filtre_vertex[vtx2] = 4;

                                if (!sensNormal){
                                    // permutation des sommets
                                    Vertex* vtmp = vtx0; vtx0 = vtx1; vtx1 = vtmp;
                                }
                                double ratio = (double)nbBrasVersAxe/(double)ni_loc;
                                Utils::Math::Point pos = vtx0->getCoord() + (vtx1->getCoord() - vtx0->getCoord())*ratio;
                                vtx2->setCoord(pos);
                                CoEdge* newCoEdge1 = new Topo::CoEdge(getContext(), &empAxe, vtx0, vtx2);
                                CoEdge* newCoEdge2 = new Topo::CoEdge(getContext(), &empOgrid, vtx2, vtx1);
#ifdef _DEBUG_REVOL
                                std::cout<<"CoEdge (multiple) "<<newCoEdge1->getName()<<" entre  "<<vtx0->getName()<<" et "<<vtx2->getName()<<std::endl;
                                std::cout<<"CoEdge (multiple) "<<newCoEdge2->getName()<<" entre  "<<vtx2->getName()<<" et "<<vtx1->getName()<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(newCoEdge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoEdge2, Internal::InfoCommand::CREATED);
                                filtre_coedge[newCoEdge1] = 4;
                                filtre_coedge[newCoEdge2] = 4;
                                if (!sensNormal){
                                    // permutation des sommets (retour à ce qu'il y avait avant)
                                    Vertex* vtmp = vtx0; vtx0 = vtx1; vtx1 = vtmp;
                                    vtx2coedges[vtx0].push_back(newCoEdge2);
                                    vtx2coedges[vtx0].push_back(newCoEdge1);
                                }
                                else {
                                    vtx2coedges[vtx0].push_back(newCoEdge1);
                                    vtx2coedges[vtx0].push_back(newCoEdge2);
                                }

                                // mise à jour de la projection suivant la géom qui a été extrudée
                                if (vtx0->getGeomAssociation()
                                        && vtx0->getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
                                    newCoEdge1->setGeomAssociation(vtx0->getGeomAssociation());
                                    newCoEdge2->setGeomAssociation(vtx0->getGeomAssociation());
                                }
                                else if (vtx1->getGeomAssociation() &&
                                        vtx1->getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
                                    newCoEdge1->setGeomAssociation(vtx1->getGeomAssociation());
                                    newCoEdge2->setGeomAssociation(vtx1->getGeomAssociation());
                                }
                            }
                            else {
                                Vertex* vtx2 = vtx0->clone();
                                Vertex* vtx3 = vtx0->clone();
                                getInfoCommand().addTopoInfoEntity(vtx2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(vtx3, Internal::InfoCommand::CREATED);
                                updateGeomAssociationToSurf(vtx0->getGeomAssociation(), vtx2);
                                updateGeomAssociationToSurf(vtx0->getGeomAssociation(), vtx3);
                                filtre_vertex[vtx2] = 4;
                                filtre_vertex[vtx3] = 4;

                                double ratio = (double)nbBrasVersAxe/(double)(2*ni_loc);
                                //std::cout<<"ratio = "<<ratio<<std::endl;
                                Utils::Math::Point pos2 = vtx0->getCoord() + (vtx1->getCoord() - vtx0->getCoord())*(0.5-ratio);
                                Utils::Math::Point pos3 = vtx0->getCoord() + (vtx1->getCoord() - vtx0->getCoord())*(0.5+ratio);
                                vtx2->setCoord(pos2);
                                vtx3->setCoord(pos3);
                                CoEdge* newCoEdge1 = new Topo::CoEdge(getContext(), &empOgrid, vtx0, vtx2);
                                CoEdge* newCoEdge2 = new Topo::CoEdge(getContext(), &empAxe, vtx2, vtx3);
                                CoEdge* newCoEdge3 = new Topo::CoEdge(getContext(), &empOgrid, vtx3, vtx1);
#ifdef _DEBUG_REVOL
                                std::cout<<"CoEdge (multiple) "<<newCoEdge1->getName()<<" entre  "<<vtx0->getName()<<" et "<<vtx2->getName()<<std::endl;
                                std::cout<<"CoEdge (multiple) "<<newCoEdge2->getName()<<" entre  "<<vtx2->getName()<<" et "<<vtx3->getName()<<std::endl;
                                std::cout<<"CoEdge (multiple) "<<newCoEdge3->getName()<<" entre  "<<vtx3->getName()<<" et "<<vtx1->getName()<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(newCoEdge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoEdge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoEdge3, Internal::InfoCommand::CREATED);
                                filtre_coedge[newCoEdge1] = 4;
                                filtre_coedge[newCoEdge2] = 4;
                                filtre_coedge[newCoEdge3] = 4;
                                vtx2coedges[vtx0].push_back(newCoEdge1);
                                vtx2coedges[vtx0].push_back(newCoEdge2);
                                vtx2coedges[vtx0].push_back(newCoEdge3);

                                // mise à jour de la projection suivant la géom qui a été extrudée
                                if (vtx0->getGeomAssociation()
                                        && vtx0->getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
                                    newCoEdge1->setGeomAssociation(vtx0->getGeomAssociation());
                                    newCoEdge2->setGeomAssociation(vtx0->getGeomAssociation());
                                    newCoEdge3->setGeomAssociation(vtx0->getGeomAssociation());
                                }
                                else if (vtx1->getGeomAssociation()
                                        && vtx1->getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
                                    newCoEdge1->setGeomAssociation(vtx1->getGeomAssociation());
                                    newCoEdge2->setGeomAssociation(vtx1->getGeomAssociation());
                                    newCoEdge3->setGeomAssociation(vtx1->getGeomAssociation());
                                }
                            }
                        }
                        else if (filtre_vertex[vtx0] >= 10) {
                            // on ne construit pas d'arête pour les sommets sur l'axe
                        }
                        else {
                            // la discrétisation pour les arêtes qui suivent la révolution
                            EdgeMeshingPropertyUniform emp(ni_loc*nbDemiOgrid);
                            CoEdge* newCoEdge = new Topo::CoEdge(getContext(), &emp, vtx0, vtx1);
                            getInfoCommand().addTopoInfoEntity(newCoEdge, Internal::InfoCommand::CREATED);
#ifdef _DEBUG_REVOL
                            std::cout<<"CoEdge "<<newCoEdge->getName()<<" entre  "<<vtx0->getName()<<" et "<<vtx1->getName()<<std::endl;
#endif
                            filtre_coedge[newCoEdge] = 4;
                            vtx2coedges[vtx0].push_back(newCoEdge);
                            // mise à jour de la projection suivant la géom qui a été extrudée
                            if (filtre_vertex[vtx0]%5 == 1){
                                if (vtx0->getGeomAssociation()
                                        && vtx0->getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
                                    if (vtx0->getGeomAssociation()->isDestroyed()){
                                    	if (!preserveSurfaceAssociation){
                                    		vtx0->setGeomAssociation(0);
#ifdef _DEBUG_REVOL
                                    		std::cout<<"constructRevolCoEdges: setGeomAssociation(0) pour vtx0 "<<vtx0->getName()<<std::endl;
#endif
                                    	}
                                    }
                                    else
                                        newCoEdge->setGeomAssociation(vtx0->getGeomAssociation());
                                }
                                else if (vtx1->getGeomAssociation()
                                        && vtx1->getGeomAssociation()->getType() == Utils::Entity::GeomSurface)
                                    if (vtx1->getGeomAssociation()->isDestroyed()){
                                    	if (!preserveSurfaceAssociation){
                                    		vtx1->setGeomAssociation(0);
#ifdef _DEBUG_REVOL
                                    		std::cout<<"constructRevolCoEdges: setGeomAssociation(0) pour vtx1 "<<vtx1->getName()<<std::endl;
#endif
                                    	}
                                  }
                                    else
                                        newCoEdge->setGeomAssociation(vtx1->getGeomAssociation());
                                else
                                    newCoEdge->setGeomAssociation(0);
                            }
                            else
                                updateGeomAssociation(vtx0->getGeomAssociation(), newCoEdge);

                        } // end else if filtre_vertex

                        filtre_vu[vtx0] = 1;

                    } // end if filtre_vu
                } // end for vertices
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolFaces(std::vector<CoFace*>& cofaces_0,
        std::vector<CoFace*>& cofaces_1,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
        std::map<CoEdge*, uint>& ni_coedge,
        uint facteur_ni)
{
#ifdef _DEBUG_REVOL
    std::cout<<"constructRevolFaces avec "<<cofaces_0.size()<<" et "<<cofaces_1.size()<<" CoFace"<<std::endl;
#endif
    // filtre sur les arêtes pour ne les prendre qu'une fois
    std::map<CoEdge*, uint> filtre_vu;

    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFaces avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        std::vector<Edge* > edges_0;
        std::vector<Edge* > edges_1;
        (*iter1_0)->getEdges(edges_0);
        (*iter1_1)->getEdges(edges_1);

        if (edges_0.size() != edges_1.size())
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFaces avec des nombres de Edge différents", TkUtil::Charset::UTF_8));

        std::vector<Edge* >::iterator iter2_0 = edges_0.begin();
        std::vector<Edge* >::iterator iter2_1 = edges_1.begin();

        for (; iter2_0 != edges_0.end(); ++iter2_0, ++iter2_1){

            std::vector<CoEdge* > coedges_0;
            std::vector<CoEdge* > coedges_1;
            (*iter2_0)->getCoEdges(coedges_0);
            (*iter2_1)->getCoEdges(coedges_1);

            if (coedges_0.size() != coedges_1.size())
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFaces avec des nombres de CoEdge différents", TkUtil::Charset::UTF_8));

            std::vector<CoEdge* >::iterator iter3_0 = coedges_0.begin();
            std::vector<CoEdge* >::iterator iter3_1 = coedges_1.begin();

            for (; iter3_0 != coedges_0.end(); ++iter3_0, ++iter3_1){
                CoEdge* coedge_0 = *iter3_0;
                CoEdge* coedge_1 = *iter3_1;

                if (filtre_vu[coedge_0] == 0){
                    filtre_vu[coedge_0] = 1;

                    uint ni_loc_coedge = ni_coedge[coedge_0]*facteur_ni;
                    if (ni_loc_coedge == 0)
                    	ni_loc_coedge = m_ni*facteur_ni;

                    std::vector<Vertex* > vertices_0;
                    std::vector<Vertex* > vertices_1;
                    (*iter3_0)->getVertices(vertices_0);
                    (*iter3_1)->getVertices(vertices_1);

                    if (vertices_0.size() != vertices_1.size())
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFaces avec des nombres de Vertex différents", TkUtil::Charset::UTF_8));
                    if (vertices_0.size() != 2)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFaces avec arêtes n'ayant pas 2 sommets", TkUtil::Charset::UTF_8));

                    Vertex* vtx0_0 = vertices_0[0];
                    Vertex* vtx1_0 = vertices_0[1];

                    // on évite les arêtes qui vont donnés des faces à cheval sur plusieurs Topo2D
                    // (cas des zones sous l'ogrid type deto)
                    // ainsi que les arêtes reliées à l'axe hors de l'ogrid
                    if (((filtre_vertex[vtx0_0] == 2 && filtre_vertex[vtx1_0] == 2)
                            || (filtre_vertex[vtx0_0] != 2 && filtre_vertex[vtx1_0] != 2))
                            && filtre_vertex[vtx0_0] != 12 && filtre_vertex[vtx1_0] != 12){

                        std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0_0];
                        std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1_0];

                        CoEdge* coedge_dep = coedge_0;
                        CoEdge* coedge_arr = coedge_1;

                        if (coedges0.size() != coedges1.size()){
                            // cas de la création d'une face avec des arêtes qui peuvent être composées
                            Vertex* vtx0_1 = vertices_1[0];
                            Vertex* vtx1_1 = vertices_1[1];

                            Edge* edge0 = 0;
                            Edge* edge1 = new Topo::Edge(getContext(), coedge_dep);
                            Edge* edge2 = 0;
                            Edge* edge3 = new Topo::Edge(getContext(), coedge_arr);

                            if (coedges0.size() == 1)
                                edge0 = new Topo::Edge(getContext(), coedges0[0]);
                            else if (coedges0.size() > 1)
                                edge0 = new Topo::Edge(getContext(), vtx0_0, vtx0_1, coedges0);

                            if (coedges1.size() == 1)
                                 edge2 = new Topo::Edge(getContext(), coedges1[0]);
                             else if (coedges1.size() > 1)
                                 edge2 = new Topo::Edge(getContext(), vtx1_0, vtx1_1, coedges1);

                            CoFace* newCoFace = 0;
                            if (edge0 == 0 && edge2 == 0)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFaces avec 2 arêtes sans arêtes communes", TkUtil::Charset::UTF_8));
                            else if (edge0 == 0)
                                newCoFace = new Topo::CoFace(getContext(), edge1, edge2, edge3);
                            else if (edge2 == 0)
                                newCoFace = new Topo::CoFace(getContext(), edge1, edge0, edge3);
                            else {
                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
                                // recherche des ratios possibles pour edge0 et edge2
                                uint ni0 = edge0->getNbMeshingEdges();
                                uint ni2 = edge2->getNbMeshingEdges();
                                if (ni0>ni_loc_coedge){
                                	uint ratio = ni0/ni_loc_coedge;
                                	for (uint i=0; i<coedges0.size(); i++)
                                		edge0->setRatio(coedges0[i], ratio);
                                }
                                if (ni2>ni_loc_coedge){
                                	uint ratio = ni2/ni_loc_coedge;
                                	for (uint i=0; i<coedges1.size(); i++)
                                		edge2->setRatio(coedges1[i], ratio);
                                }

                            }
#ifdef _DEBUG_REVOL
                            std::cout<<" Création de la CoFace "<<newCoFace->getName()<<" par révolution, avec une des arêtes composée"<<std::endl;
#endif
                            if (edge0 != 0)
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                            getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                            if (edge2 != 0)
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                            getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                            getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                            coedge2cofaces[coedge_0].push_back(newCoFace);

                            updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);

                        } // end if (coedges0.size() != coedges1.size())
                        else {
                            std::vector<CoEdge* >::iterator iter4_0 = coedges0.begin();
                            std::vector<CoEdge* >::iterator iter4_1 = coedges1.begin();
                            for (;iter4_0 != coedges0.end(); ++iter4_0, ++iter4_1){
                                CoEdge* coedge0 = *iter4_0;
                                CoEdge* coedge1 = *iter4_1;

                                // il peut y avoir plusieurs arêtes depuis un sommet, donc on cherche les 2 autres sommets de la face
                                Vertex* vtx2 = coedge0->getOppositeVertex(vtx0_0);
                                Vertex* vtx3 = coedge1->getOppositeVertex(vtx1_0);

                                if (filtre_vertex[vtx2] == 4){
                                    // cas d'une nouvelle arête interne
                                    coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                    getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                }
                                else
                                    coedge_arr = coedge_1;

                                Edge* edge0 = new Topo::Edge(getContext(), coedge0);
                                Edge* edge1 = new Topo::Edge(getContext(), coedge_dep);
                                Edge* edge2 = new Topo::Edge(getContext(), coedge1);
                                Edge* edge3 = new Topo::Edge(getContext(), coedge_arr);

                                CoFace* newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);

                                // recherche des ratios possibles pour edge0 et edge2
                                uint ni0 = edge0->getNbMeshingEdges();
                                uint ni2 = edge2->getNbMeshingEdges();
                                if (ni0>ni_loc_coedge){
                                	uint ratio = ni0/ni_loc_coedge;
                                	edge0->setRatio(coedge0, ratio);
#ifdef _DEBUG_REVOL
                                	std::cout<<" setRatio "<<ratio<<" pour "<<edge0->getName()<<std::endl;
#endif
                                }
                                if (ni2>ni_loc_coedge){
                                	uint ratio = ni2/ni_loc_coedge;
                                	edge2->setRatio(coedge1, ratio);
#ifdef _DEBUG_REVOL
                                	std::cout<<" setRatio "<<ratio<<" pour "<<edge2->getName()<<std::endl;
#endif
                               }

#ifdef _DEBUG_REVOL
//                                std::cout<<"  ni0 = "<<ni0<<std::endl;
//                                std::cout<<"  ni2 = "<<ni2<<std::endl;
//                                std::cout<<"  ni_loc_coedge = "<<ni_loc_coedge<<std::endl;
                                std::cout<<" Création de la CoFace "<<newCoFace->getName()<<" par révolution"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);

                                vtx0_0 = vtx2;
                                vtx1_0 = vtx3;
                                coedge_dep = coedge_arr;

                                // mise à jour de la projection suivant la géom qui a été extrudée
                                if (filtre_coedge[coedge_0]%10 == 1)
                                    newCoFace->setGeomAssociation(0);
                                else
                                    updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);

#ifdef _DEBUG_MESH_LAW
                                std::cout<<"filtre_coedge["<<coedge_dep->getName()<<"] = "<<filtre_coedge[coedge_dep]<<std::endl;
#endif
                                // pour les faces communes en relation avec l'ogrid et celles issues d'une compositions de cofaces
                                // (cas d'un bloc sous l'ogrid)
                                if ((filtre_coedge[coedge_dep] == 1 || filtre_coedge[coedge_dep] == 0)
                                		&& coedge_dep->getMeshLaw() != CoEdgeMeshingProperty::interpolate){
                                    // on remplace la méthode créée par défaut, par celle directionnelle en I
                                    FaceMeshingPropertyDirectional* newProp = new FaceMeshingPropertyDirectional(FaceMeshingPropertyDirectional::dir_i);
                                    CoFaceMeshingProperty* oldProp = newCoFace->setProperty(newProp);
                                    delete oldProp;
#ifdef _DEBUG_MESH_LAW
                                    std::cout<<"On utilise la méthode direction (dirI) pour mailler "<<newCoFace->getName()<<std::endl;
#endif
                                }
                                else {
                                    // recherche si l'arête a un sommet qui touche l'ogrid
                                    bool touche = false;
                                    for (uint i=0; i<coedge_dep->getNbVertices(); i++)
                                        if (filtre_vertex[coedge_dep->getVertex(i)]%5 == 1)
                                            touche = true;
                                    if (!touche){
                                        // on remplace la méthode créée par défaut, par celle rotationnelle suivant J
                                        FaceMeshingPropertyRotational* newProp =
                                                new FaceMeshingPropertyRotational
                                                       (FaceMeshingPropertyRotational::dir_j,
                                                        Utils::Math::Point(0,0,0),
                                                        Utils::Math::Point(1,0,0));
                                        CoFaceMeshingProperty* oldProp = newCoFace->setProperty(newProp);
                                        delete oldProp;
#ifdef _DEBUG_MESH_LAW
                                        std::cout<<"On utilise la méthode rotation (rotJ) pour mailler "<<newCoFace->getName()<<std::endl;
#endif
                                    }
                                }

                            } // end for coedges0
                        } // end else / if coedges0.size() != coedges1.size()
                    } // end cas de la construction d'une face
                } // end if filtre_vu
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolFacesInnerOgrid(std::vector<CoFace*>& cofaces_0,
        std::vector<CoFace*>& cofaces_1,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces)
{
    // filtre sur les arêtes pour ne les prendre qu'une fois
    std::map<CoEdge*, uint> filtre_vu;

    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        std::vector<Edge* > edges_0;
        std::vector<Edge* > edges_1;
        (*iter1_0)->getEdges(edges_0);
        (*iter1_1)->getEdges(edges_1);

        if (edges_0.size() != edges_1.size())
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec des nombres de Edge différents", TkUtil::Charset::UTF_8));

        uint ratio_ni = 1;

        std::vector<Edge* >::iterator iter2_0 = edges_0.begin();
        std::vector<Edge* >::iterator iter2_1 = edges_1.begin();

        for (; iter2_0 != edges_0.end(); ++iter2_0, ++iter2_1){

            std::vector<CoEdge* > coedges_0;
            std::vector<CoEdge* > coedges_1;
            (*iter2_0)->getCoEdges(coedges_0);
            (*iter2_1)->getCoEdges(coedges_1);

            if (coedges_0.size() != coedges_1.size())
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec des nombres de CoEdge différents", TkUtil::Charset::UTF_8));

            std::vector<CoEdge* >::iterator iter3_0 = coedges_0.begin();
            std::vector<CoEdge* >::iterator iter3_1 = coedges_1.begin();

            for (; iter3_0 != coedges_0.end(); ++iter3_0, ++iter3_1){
                CoEdge* coedge_0 = *iter3_0;
                CoEdge* coedge_1 = *iter3_1;

                if (filtre_vu[coedge_0] == 0 && filtre_coedge[coedge_0] != 22){
                    filtre_vu[coedge_0] = 1;

                    std::vector<Vertex* > vertices_0;
                    std::vector<Vertex* > vertices_1;
                    (*iter3_0)->getVertices(vertices_0);
                    (*iter3_1)->getVertices(vertices_1);

                    if (vertices_0.size() != vertices_1.size())
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec des nombres de Vertex différents", TkUtil::Charset::UTF_8));
                    if (vertices_0.size() != 2)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec arêtes n'ayant pas 2 sommets", TkUtil::Charset::UTF_8));

                    Vertex* vtx0_0 = vertices_0[0];
                    Vertex* vtx1_0 = vertices_0[1];

                    // on sélectionne les arêtes qui vont donner des faces sous l'ogrid
                    if (filtre_vertex[vtx0_0] == 12 || filtre_vertex[vtx1_0] == 12) {
                        // cas avec une face au centre de l'o-grid
                        Vertex* vtx0 = vtx0_0;
                        Vertex* vtx1 = vtx1_0;
                        if (filtre_vertex[vtx1] == 12){
                            // permutation pour avoir vtx0 sur l'axe
                            Vertex* vtmp = vtx0; vtx0 = vtx1; vtx1 = vtmp;
                        }
#ifdef _DEBUG_REVOL
                        std::cout<<"Cas sous l'ogrid, au centre\n";
                        std::cout<<" vtx0 : "<<vtx0->getName()<<std::endl;
                        std::cout<<" vtx1 : "<<vtx1->getName()<<std::endl;
#endif
                        Edge* edge0 = 0;
                        Edge* edge1 = 0;
                        Edge* edge2 = 0;
                        Edge* edge3 = 0;

                        if (m_portion == Utils::Portion::QUART
                        		|| m_portion == Utils::Portion::TIERS
                        		|| m_portion == Utils::Portion::CINQUIEME
								|| m_portion == Utils::Portion::SIXIEME){
                            edge0 = new Topo::Edge(getContext(), coedge_0);

                            std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1];
                            if (coedges1.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx1", TkUtil::Charset::UTF_8));
                            edge1 = new Topo::Edge(getContext(), coedges1[0]);

                            Vertex* vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                            std::vector<CoEdge* >& coedges2 = vtx2coedges[vtx2];
                            if (coedges2.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx2", TkUtil::Charset::UTF_8));
                            edge2 = new Topo::Edge(getContext(), coedges2[0]);

                            edge3 = new Topo::Edge(getContext(), coedge_1);

                        } else if (m_portion == Utils::Portion::DEMI){
                            std::vector<CoEdge* > coedges0;
                            coedges0.push_back(coedge_0);
                            coedges0.push_back(coedge_1);
                            edge0 = new Topo::Edge(getContext(), vtx1, coedge_1->getOppositeVertex(vtx0), coedges0);

                            std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1];
                            if (coedges1.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx1", TkUtil::Charset::UTF_8));
                            edge1 = new Topo::Edge(getContext(), coedges1[0]);

                            Vertex* vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                            std::vector<CoEdge* >& coedges2 = vtx2coedges[vtx2];
                            if (coedges2.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx2", TkUtil::Charset::UTF_8));
                            edge2 = new Topo::Edge(getContext(), coedges2[0]);

                            Vertex* vtx3 = coedges2[0]->getOppositeVertex(vtx2);
                            std::vector<CoEdge* >& coedges3 = vtx2coedges[vtx3];
                            if (coedges3.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx3", TkUtil::Charset::UTF_8));
                            edge3 = new Topo::Edge(getContext(), coedges3[0]);

                        } else if (m_portion == Utils::Portion::ENTIER){
                            std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1];
                            if (coedges1.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx1", TkUtil::Charset::UTF_8));
                            edge1 = new Topo::Edge(getContext(), coedges1[0]);

                            Vertex* vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                            std::vector<CoEdge* >& coedges2 = vtx2coedges[vtx2];
                            if (coedges2.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx2", TkUtil::Charset::UTF_8));
                            edge2 = new Topo::Edge(getContext(), coedges2[0]);

                            Vertex* vtx3 = coedges2[0]->getOppositeVertex(vtx2);
                            std::vector<CoEdge* >& coedges3 = vtx2coedges[vtx3];
                            if (coedges3.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx3", TkUtil::Charset::UTF_8));
                            edge3 = new Topo::Edge(getContext(), coedges3[0]);

                            Vertex* vtx4 = coedges3[0]->getOppositeVertex(vtx3);
                            std::vector<CoEdge* >& coedges4 = vtx2coedges[vtx4];
                            if (coedges4.size() != 1)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose qu'une unique arête pour vtx4", TkUtil::Charset::UTF_8));
                            edge0 = new Topo::Edge(getContext(), coedges4[0]);
                       } // end else if ... m_portion
                        else {
                        	MGX_FORBIDDEN("Cas non prévu");
                        }

                        CoFace* newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                        std::cout<<" Création de la CoFace "<<newCoFace->getName()<<" au centre de l'ogrid"<<std::endl;
#endif
                        getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                        getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                        getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                        getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                        getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                        coedge2cofaces[coedge_0].push_back(newCoFace);

                        updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);

                    }
                    else if ((filtre_vertex[vtx0_0] == 2 && filtre_vertex[vtx1_0] != 2)
                            || (filtre_vertex[vtx0_0] != 2 && filtre_vertex[vtx1_0] == 2)) {
                        // cas avec 3, 5 ou 8 faces suivant  m_portion, autour de la face au centre de l'ogrid

                        Vertex* vtx0 = vtx0_0;
                        Vertex* vtx1 = vtx1_0;
                        if (filtre_vertex[vtx0] == 6){
                            // permutation pour avoir vtx1 sur l'ogrid
                            Vertex* vtmp = vtx0; vtx0 = vtx1; vtx1 = vtmp;
                        }

                        Edge* edge0 = 0;
                        Edge* edge1 = 0;
                        Edge* edge2 = 0;
                        Edge* edge3 = 0;
                        Vertex* vtx2 = 0;
                        Vertex* vtx3 = 0;
                        CoFace* newCoFace = 0;

#ifdef _DEBUG_REVOL
                        std::cout<<"Cas sous l'ogrid, autour du centre\n";
                        std::cout<<" vtx0 : "<<vtx0->getName()<<std::endl;
                        std::cout<<" vtx1 : "<<vtx1->getName()<<std::endl;
#endif
                        if (filtre_vertex[vtx0] != 2)
                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid trouve une marque autre que 2 pour vtx0", TkUtil::Charset::UTF_8));
                        if (filtre_vertex[vtx1] != 6)
                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid trouve une marque autre que 6 pour vtx1", TkUtil::Charset::UTF_8));

                        if (m_portion == Utils::Portion::QUART
                        		|| m_portion == Utils::Portion::TIERS
                        		|| m_portion == Utils::Portion::CINQUIEME
								|| m_portion == Utils::Portion::SIXIEME){
                            CoEdge* coedge_dep = 0; // une arête dont on conservera un ptr d'une face à l'autre
                            // première face
                            {
                                coedge_dep = coedge_0;
                                edge0 = new Topo::Edge(getContext(), coedge_dep);

                                std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1];
                                if (coedges1.size() != 2)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 2 arêtes pour vtx1", TkUtil::Charset::UTF_8));
                                edge1 = new Topo::Edge(getContext(), coedges1[0]);

                                std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0];
                                if (coedges0.size() != 1)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 1 arête pour vtx0", TkUtil::Charset::UTF_8));
                                edge3 = new Topo::Edge(getContext(), coedges0[0]);

                                vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                                vtx3 = coedges0[0]->getOppositeVertex(vtx0);
                                CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                edge2 = new Topo::Edge(getContext(), coedge_arr);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 1ère CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                coedge_dep = coedges1[1];
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                            }
                            // deuxième face
                            {
                                edge0 = new Topo::Edge(getContext(), coedge_dep);
                                // vtx3 inchangé
                                vtx0 = vtx2;
                                vtx1 = coedge_dep->getOppositeVertex(vtx0);
                                edge3 = edge2;
                                std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1];
                                if (coedges1.size() != 2)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 2 arêtes pour vtx1 (2ème)", TkUtil::Charset::UTF_8));
                                vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                                edge1 = new Topo::Edge(getContext(), coedges1[0]);

                                CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                edge2 = new Topo::Edge(getContext(), coedge_arr);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 2ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                coedge_dep = coedges1[1];
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                            }
                            // troisième face
                            {
                                vtx0 = vtx3;
                                edge0 = edge2;
                                edge1 = new Topo::Edge(getContext(), coedge_dep);
                                std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0];
                                if (coedges0.size() != 1)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 1 arête pour vtx0 (3ème)", TkUtil::Charset::UTF_8));
                                edge3 = new Topo::Edge(getContext(), coedges0[0]);

                                edge2 = new Topo::Edge(getContext(), coedge_1);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 3ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);

                            }
                        } else if (m_portion == Utils::Portion::DEMI){
                            CoEdge* coedge_dep = 0; // une arête dont on conservera un ptr d'une face à l'autre
                            std::vector<CoEdge* > coedges1; // le vecteur des arêtes issues de vtx1 (sur ogrid) par révolution
                            // première face
                            {
                                coedge_dep = coedge_0;
                                edge0 = new Topo::Edge(getContext(), coedge_dep);

                                coedges1 = vtx2coedges[vtx1];
                                if (coedges1.size() != 2)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 2 arêtes pour vtx1", TkUtil::Charset::UTF_8));
                                edge1 = new Topo::Edge(getContext(), coedges1[0]);

                                std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0];
                                if (coedges0.size() != 1)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 1 arête pour vtx0", TkUtil::Charset::UTF_8));
                                edge3 = new Topo::Edge(getContext(), coedges0[0]);

                                vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                                vtx3 = coedges0[0]->getOppositeVertex(vtx0);
                                CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                edge2 = new Topo::Edge(getContext(), coedge_arr);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 1ère CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                            }
                            // deuxième face
                            {
                                coedge_dep = coedges1[1];
                                edge0 = new Topo::Edge(getContext(), coedge_dep);
                                // vtx3 inchangé
                                vtx0 = vtx2;
                                vtx1 = coedge_dep->getOppositeVertex(vtx0);
                                edge3 = edge2;
                                coedges1 = vtx2coedges[vtx1];
                                if (coedges1.size() != 3)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 3 arêtes pour vtx1 (2ème)", TkUtil::Charset::UTF_8));
                                vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                                edge1 = new Topo::Edge(getContext(), coedges1[0]);

                                CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                edge2 = new Topo::Edge(getContext(), coedge_arr);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 2ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                            }
                            // troisième face
                            {
                                vtx0 = vtx3;
                                vtx1 = vtx2;
                                vtx2 = coedges1[1]->getOppositeVertex(vtx1);
                                edge0 = edge2;
                                edge1 = new Topo::Edge(getContext(), coedges1[1]);
                                std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0];
                                if (coedges0.size() != 1)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 1 arête pour vtx0 (3ème)", TkUtil::Charset::UTF_8));
                                edge3 = new Topo::Edge(getContext(), coedges0[0]);

                                vtx3 = coedges0[0]->getOppositeVertex(vtx0);
                                CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                edge2 = new Topo::Edge(getContext(), coedge_arr);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 3ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                            }
                            // quatrième face
                            {
                                coedge_dep = coedges1[2];
                                edge0 = new Topo::Edge(getContext(), coedge_dep);
                                // vtx3 inchangé
                                vtx0 = vtx2;
                                vtx1 = coedge_dep->getOppositeVertex(vtx0);
                                edge3 = edge2;
                                coedges1 = vtx2coedges[vtx1];
                                if (coedges1.size() != 2)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 2 arêtes pour vtx1 (4ème)", TkUtil::Charset::UTF_8));
                                vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                                edge1 = new Topo::Edge(getContext(), coedges1[0]);

                                CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                edge2 = new Topo::Edge(getContext(), coedge_arr);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 4ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                            }
                            // cinquième face
                            {
                                coedge_dep = coedges1[1];
                                vtx0 = vtx3;
                                edge0 = edge2;
                                edge1 = new Topo::Edge(getContext(), coedge_dep);
                                std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0];
                                if (coedges0.size() != 1)
                                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 1 arête pour vtx0 (5ème)", TkUtil::Charset::UTF_8));
                                edge3 = new Topo::Edge(getContext(), coedges0[0]);

                                edge2 = new Topo::Edge(getContext(), coedge_1);

                                newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la 5ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                coedge2cofaces[coedge_0].push_back(newCoFace);
                                updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);

                            }
                        } else if (m_portion == Utils::Portion::ENTIER){
                            CoEdge* coedge_dep = 0; // une arête dont on conservera un ptr d'une face à l'autre
                            std::vector<CoEdge* > coedges1; // le vecteur des arêtes issues de vtx1 (sur ogrid) par révolution

                            // on place vtx1 sur le sommet au bout de la première arête par révolution du point sur ogrid
                            coedges1 = vtx2coedges[vtx1];
                            if (coedges1.size() != 3)
                                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 3 arêtes pour vtx1", TkUtil::Charset::UTF_8));
                            vtx1 = coedges1[0]->getOppositeVertex(vtx1);
// TODO
                            // on initialise le processus avec la première arête
                            coedge_dep = new Topo::CoEdge(getContext(), coedges1[0]->getMeshingProperty(), vtx0, vtx1);
                            getInfoCommand().addTopoInfoEntity(coedge_dep, Internal::InfoCommand::CREATED);

                            // on mémorise cette toute première pour ne la construire qu'une fois
                            CoEdge* coedge_dep0 = coedge_dep;

                            for (uint i=0; i<4; i++){
                                // première face
                                {
                                    edge0 = new Topo::Edge(getContext(), coedge_dep);

                                    edge1 = new Topo::Edge(getContext(), coedges1[1]);

                                    std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0];
                                    if (coedges0.size() != 1)
                                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 1 arête pour vtx0", TkUtil::Charset::UTF_8));
                                    edge3 = new Topo::Edge(getContext(), coedges0[0]);

                                    vtx2 = coedges1[1]->getOppositeVertex(vtx1);
                                    vtx3 = coedges0[0]->getOppositeVertex(vtx0);
                                    CoEdge* coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                    getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                    edge2 = new Topo::Edge(getContext(), coedge_arr);

                                    newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                    std::cout<<" Création de la 1ère CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                    getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                    coedge2cofaces[coedge_0].push_back(newCoFace);
                                    updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                    updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                                }
                                // deuxième face
                                {
                                    coedge_dep = coedges1[2];
                                    edge0 = new Topo::Edge(getContext(), coedge_dep);
                                    // vtx3 inchangé
                                    vtx0 = vtx2;
                                    vtx1 = coedge_dep->getOppositeVertex(vtx0);
                                    edge3 = edge2;
                                    coedges1 = vtx2coedges[vtx1];
                                    if (coedges1.size() != 3)
                                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolFacesInnerOgrid avec autre chose que 3 arêtes pour vtx1", TkUtil::Charset::UTF_8));
                                    vtx2 = coedges1[0]->getOppositeVertex(vtx1);
                                    edge1 = new Topo::Edge(getContext(), coedges1[0]);
                                    CoEdge* coedge_arr = 0;
                                    if (i==3)
                                        coedge_arr = coedge_dep0;
                                    else {
                                    	coedge_arr = new Topo::CoEdge(getContext(), coedge_dep->getMeshingProperty(), vtx2, vtx3);
                                    	getInfoCommand().addTopoInfoEntity(coedge_arr, Internal::InfoCommand::CREATED);
                                    }
                                    edge2 = new Topo::Edge(getContext(), coedge_arr);

                                    newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
#ifdef _DEBUG_REVOL
                                    std::cout<<" Création de la 2ème CoFace "<<newCoFace->getName()<<" autour de la face au centre de l'ogrid"<<std::endl;
#endif
                                    getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                                    getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                                    coedge2cofaces[coedge_0].push_back(newCoFace);
                                    updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);
                                    updateGeomAssociation(coedge_0->getGeomAssociation(), coedge_arr);

                                    // pour le prochain i
                                    vtx0 = vtx3;
                                    vtx1 = vtx2;
                                    coedge_dep = coedge_arr;

                                }

                            } // end for i

                        } // end else if ... m_portion
                    } // end cas de la construction d'une face
                } // end if filtre_vu
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolBlocks(std::vector<CoFace*>& cofaces_0,
        std::vector<CoFace*>& cofaces_1,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
        std::map<Vertex*, uint>& ni_vtx,
        std::map<CoFace*, uint>& ni_coface,
        uint facteur_ni)
{
#ifdef _DEBUG_REVOL
    std::cout<<"\nconstructRevolBlocks\n";
#endif
    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocks avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        CoFace* coface_0 = *iter1_0;
        CoFace* coface_1 = *iter1_1;

        uint ni_loc_coface = ni_coface[coface_0]*facteur_ni;
        if (ni_loc_coface == 0)
        	ni_loc_coface = m_ni*facteur_ni;

#ifdef _DEBUG_REVOL
        std::cout<<"Coface observée: "<<coface_0->getName()<<", filtre : "<<filtre_coface[coface_0]<<std::endl;
#endif
        if (filtre_coface[coface_0]%10 != 3)
        	continue;


        std::vector<Face* > faces;
        std::vector<Vertex* > vertices;

        faces.push_back(new Topo::Face(getContext(), coface_0));
        faces.push_back(new Topo::Face(getContext(), coface_1));
        getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
        getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
#ifdef _DEBUG_REVOL
        //            std::cout<<" coface_0 : "<<*coface_0<<std::endl;
        //            std::cout<<" coface_1 : "<<*coface_1<<std::endl;
        std::cout<<"coface_0->getNbEdges() = "<<coface_0->getNbEdges()<<std::endl;
        std::cout<<"coface_1->getNbEdges() = "<<coface_1->getNbEdges()<<std::endl;
#endif
        if (!((coface_0->getNbEdges() == 4 && coface_1->getNbEdges() == 4)
        		|| (coface_0->getNbEdges() == 3 && coface_1->getNbEdges() == 3)))
        {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "Erreur interne, la création d'un bloc ne peut se faire pour la face "<<coface_0->getName()
                        		<<", nous n'avons les bons nombres de côtés";
        	throw TkUtil::Exception(messErr);
        }

        // si l'une des arêtes est sur l'axe de symétrie,
        // alors il faudra adapter la création du bloc à la dégénérecence (la mettre en k_max)
        uint indDeg = 3; // cas dit normal, indice de l'arête qui porte la dég... s'il y a lieu
        for (uint i=0; i<coface_0->getNbEdges(); i++){
        	Edge* edge_0 = coface_0->getEdge(i);
        	if (isOnAxis(edge_0->getVertex(0)) && isOnAxis(edge_0->getVertex(1)))
        		indDeg = i;
        }
        uint tabIndVtx[4] = {1, 2, 0, 3};
        // on ordonne les Edges pour être compatible avec l'ordre des Face pour le Block
        uint tabIndFace[4] = {0, 2, 1, 3};

        if (indDeg == 0){
        	tabIndFace[0] = 1;
        	tabIndFace[1] = 3;
        	tabIndFace[2] = 2;
        	tabIndFace[3] = 0;
        	tabIndVtx[0] = 2;
        	tabIndVtx[1] = 3;
        	tabIndVtx[2] = 1;
        	tabIndVtx[3] = 0;
        }
        else if (indDeg == 1){
        	tabIndFace[0] = 0;
        	tabIndFace[1] = 2;
        	tabIndFace[2] = 3;
        	tabIndFace[3] = 1;
        	tabIndVtx[0] = 0;
        	tabIndVtx[1] = 3;
        	tabIndVtx[2] = 1;
        	tabIndVtx[3] = 2;
        }
        else if (indDeg == 2){
        	tabIndFace[0] = 1;
        	tabIndFace[1] = 3;
        	tabIndFace[2] = 0;
        	tabIndFace[3] = 2;
        	tabIndVtx[0] = 1;
        	tabIndVtx[1] = 0;
        	tabIndVtx[2] = 2;
        	tabIndVtx[3] = 3;
        }
#ifdef _DEBUG_REVOL
        std::cout<<"indDeg = "<<indDeg<<std::endl;
#endif

        // on ordonne les sommets pour être compatible avec l'ordre pour le Block
        vertices.push_back(coface_0->getVertex(tabIndVtx[0]));
        vertices.push_back(coface_1->getVertex(tabIndVtx[0]));
        vertices.push_back(coface_0->getVertex(tabIndVtx[1]));
        vertices.push_back(coface_1->getVertex(tabIndVtx[1]));
        vertices.push_back(coface_0->getVertex(tabIndVtx[2]));
        if (coface_0->getNbEdges() == 4){
        	if (vertices.back() != coface_1->getVertex(tabIndVtx[2]))
        		vertices.push_back(coface_1->getVertex(tabIndVtx[2]));
        	vertices.push_back(coface_0->getVertex(tabIndVtx[3]));
        	if (vertices.back() != coface_1->getVertex(tabIndVtx[3]))
        		vertices.push_back(coface_1->getVertex(tabIndVtx[3]));
        }
        else if (filtre_vertex[coface_1->getVertex(tabIndVtx[2])] != 11)
        	// cas d'un bloc dégénéré qui n'est pas sur l'axe
        	vertices.push_back(coface_1->getVertex(tabIndVtx[2]));

#ifdef _DEBUG_REVOL
        std::cout<<" vertices pour bloc:";
        for (uint j=0; j<vertices.size(); j++)
        	std::cout<<" "<<vertices[j]->getName();
        std::cout<<std::endl;
#endif


        for (uint i=0; i<coface_0->getNbEdges(); i++){

        	Edge* edge_0 = coface_0->getEdge(tabIndFace[i]);
        	Edge* edge_1 = coface_1->getEdge(tabIndFace[i]);

        	std::vector<Vertex* > face_vertices;
        	Vertex* vtx0 = edge_0->getVertex(0);
        	Vertex* vtx1 = edge_0->getVertex(1);
        	Vertex* vtx2 = edge_1->getVertex(1);
        	Vertex* vtx3 = edge_1->getVertex(0);

        	if (vtx1!=vtx2){
        		face_vertices.push_back(vtx0);
        		face_vertices.push_back(vtx1);
        		face_vertices.push_back(vtx2);
        		if (vtx0!=vtx3)
        			face_vertices.push_back(vtx3);
        	}
        	else {
        		face_vertices.push_back(vtx1);
        		face_vertices.push_back(vtx0);
        		face_vertices.push_back(vtx3);
        	}

        	std::vector<CoFace* > face_cofaces;

        	std::vector<CoEdge* > coedges_0;
        	edge_0->getCoEdges(coedges_0);

        	for (std::vector<CoEdge* >::iterator iter3 = coedges_0.begin();
        			iter3 != coedges_0.end(); ++iter3){
        		std::vector<CoFace* >& revol_cofaces = coedge2cofaces[*iter3];
        		for (std::vector<CoFace* >::iterator iter4 = revol_cofaces.begin();
        				iter4 != revol_cofaces.end(); ++iter4)
        			face_cofaces.push_back(*iter4);
        	}
#ifdef _DEBUG_REVOL
        	std::cout<<" création d'une Face avec cofaces:";
        	for (uint j=0; j<face_cofaces.size(); j++)
        		std::cout<<" "<<face_cofaces[j]->getName();
        	std::cout<<std::endl;
        	std::cout<<" et avec vertices:";
        	for (uint j=0; j<face_vertices.size(); j++)
        		std::cout<<" "<<face_vertices[j]->getName();
        	std::cout<<std::endl;
#endif
        	if (!face_cofaces.empty()){
        		faces.push_back(new Topo::Face(getContext(), face_cofaces, face_vertices, true));
        		getInfoCommand().addTopoInfoEntity(faces.back(), Internal::InfoCommand::CREATED);

        		// reprise de la semi-conformité pour les faces de part et d'autre de l'arête dont on fait la révolution
        		for (std::vector<CoEdge* >::iterator iter3 = coedges_0.begin();
        				iter3 != coedges_0.end(); ++iter3){
        			uint ratio = edge_0->getRatio(*iter3);
        			if (ratio != 1){
#ifdef _DEBUG_REVOL
        				std::cout<<" ratio de "<<ratio<<" reporté de "<<edge_0->getName()<<" vers "<<faces.back()->getName()<<std::endl;
#endif
        				std::vector<CoFace* >& revol_cofaces = coedge2cofaces[*iter3];
        				for (std::vector<CoFace* >::iterator iter4 = revol_cofaces.begin();
        						iter4 != revol_cofaces.end(); ++iter4)
        					faces.back()->setRatio(*iter4, ratio, 1);
        			}
        		}

        		// prise en compte de la semi-conformité suivant la révolution
        		computeFaceRatio(faces.back(), 0, ni_loc_coface);

        	} // end if (!face_cofaces.empty())

        } // end for i<coface_0->getNbEdges()

        Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
        getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
        updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
        std::cout<<"\nCréation du bloc à l'extérieur de l'ogrid : "<<*newBlock;
        //newBlock->check();
#endif

        // on cherche si l'une des directions peut servir pour définir un maillage unidirectionnel
        // on se base sur la face de départ
        // non valable pour le cas des blocs dégénérés
        bool findDir = false;
        if (vertices.size() == 8){
        	Face* face = newBlock->getFace(0);
        	if (face->getNbCoFaces() == 1){
        		CoFaceMeshingProperty::meshLaw ml = face->getCoFace(0)->getMeshLaw();
        		findDir = (ml == CoFaceMeshingProperty::directional);
        	}
        }

        if (vertices.size() == 8 && findDir){
        	std::vector<CoEdge* > iCoedges;
        	std::vector<CoEdge* > jCoedges;

        	Face* face = newBlock->getFace(0);

        	face->getOrientedCoEdges(iCoedges, jCoedges);

        	// les 2 groupes d'arêtes pour une même direction
        	{
        		std::vector<std::vector<CoEdge* > > coedges_dirs;

        		std::vector<CoEdge* > coedges_dir1;
        		TopoHelper::getCoEdgesBetweenVertices(face->getVertex(1),
        				face->getVertex(2),
        				iCoedges,
        				coedges_dir1);
        		coedges_dirs.push_back(coedges_dir1);

        		if (face->getNbVertices() == 4){
        			std::vector<CoEdge* > coedges_dir2;
        			TopoHelper::getCoEdgesBetweenVertices(face->getVertex(0),
        					face->getVertex(3),
        					iCoedges,
        					coedges_dir2);
        			coedges_dirs.push_back(coedges_dir2);
        		}

        		if (TopoHelper::isUnidirectionalMeshable(coedges_dirs)){
#ifdef _DEBUG_MESH_LAW
        			std::cout<<"On utilise la méthode dirJ pour mailler "<<newBlock->getName()<<" ainsi que les faces ..."<< std::endl;
#endif
        			BlockMeshingProperty* new_ppty = new BlockMeshingPropertyDirectional(BlockMeshingProperty::dir_j);
        			newBlock->setMeshLaw(new_ppty);
        		}
        	}

        	if (newBlock->getMeshLaw() == BlockMeshingProperty::transfinite){

        		std::vector<std::vector<CoEdge* > > coedges_dirs;

        		std::vector<CoEdge* > coedges_dir1;
        		TopoHelper::getCoEdgesBetweenVertices(face->getVertex(1),
        				face->getVertex(0),
        				jCoedges,
        				coedges_dir1);
        		coedges_dirs.push_back(coedges_dir1);

        		std::vector<CoEdge* > coedges_dir2;
        		TopoHelper::getCoEdgesBetweenVertices(face->getVertex(2),
        				face->getVertex(face->getNbVertices()==4?3:0),
        				jCoedges,
        				coedges_dir2);
        		coedges_dirs.push_back(coedges_dir2);

        		if (TopoHelper::isUnidirectionalMeshable(coedges_dirs)){
#ifdef _DEBUG_MESH_LAW
        			std::cout<<"On utilise la méthode dirK pour mailler "<<newBlock->getName()<<" ainsi que les faces ..."<< std::endl;
#endif
        			BlockMeshingProperty* new_ppty = new BlockMeshingPropertyDirectional(BlockMeshingProperty::dir_k);
        			newBlock->setMeshLaw(new_ppty);
        		}
        	}

        	// cas des blocs au dessus de l'ogrid et qui ne le touche pas
        	if (newBlock->getMeshLaw() == BlockMeshingProperty::transfinite){

        		// recherche si la coface a un sommet qui touche l'ogrid
        		bool touche = false;
        		for (uint i=0; i<coface_0->getNbVertices(); i++)
        			if (filtre_vertex[coface_0->getVertex(i)]%5 == 1)
        				touche = true;
        		if (!touche){
#ifdef _DEBUG_MESH_LAW
        			std::cout<<"On utilise la méthode rotation (rotI) pour mailler "<<newBlock->getName()<<std::endl;
#endif
        			BlockMeshingProperty* new_ppty = new BlockMeshingPropertyRotational
        					(BlockMeshingProperty::dir_i, Utils::Math::Point(0,0,0), Utils::Math::Point(1,0,0));
        			newBlock->setMeshLaw(new_ppty);
        		}
        	}
        } // end if (vertices.size() == 8 && findDir)
        // on reste sur la méthode transfinie (car directionnelle pas compatible avec contours courbes dans le 2D)
//        else if (vertices.size() != 8){
//        	// On évite la méthode rotationnelle pour les blocs dégénérés
//        	// car elle n'est pas adaptée aux cas avec un sommet point triple de l'ogrid sur une surface
//        	// on utilise donc la direction vers la dégénérescence
//#ifdef _DEBUG_MESH_LAW
//        	std::cout<<"On utilise la méthode direction (dirK) pour mailler "<<newBlock->getName()<<std::endl;
//#endif
//        	BlockMeshingProperty* new_ppty = new BlockMeshingPropertyDirectional(BlockMeshingProperty::dir_k);
//        	newBlock->setMeshLaw(new_ppty);
//        }

    } // end for cofaces
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolBlocksInnerOgrid_4(std::vector<CoFace*>& cofaces_0,
        std::vector<CoFace*>& cofaces_45,
        std::vector<CoFace*>& cofaces_90,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
        std::map<Vertex*, uint>& ni_vtx,
        std::map<CoFace*, uint>& ni_coface)
{
#ifdef _DEBUG_REVOL
    std::cout<<"\nconstructRevolBlocksInnerOgrid_4\n";
#endif
    if (m_portion != Utils::Portion::QUART
    		&& m_portion != Utils::Portion::TIERS
    		&& m_portion != Utils::Portion::CINQUIEME
			&& m_portion != Utils::Portion::SIXIEME)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 n'est pas possible avec la portion actuelle", TkUtil::Charset::UTF_8));

    // tableau pour ordonner les arêtes des faces en fonction de l'arête de départ
    uint tabIndArFace[4][4] = {{0,2,1,3},
            {1,3,0,2},
            {2,0,1,3},
            {3,1,0,2} };
    // idem avec les sommets
    uint tabIndSomFace[4][4] = {{1,2,0,3},
            {1,0,2,3},
            {2,1,3,0},
            {0,1,3,2} };
    // idem avec les sommets pour le cas dégénéré
    uint tabIndSomFaceDeg[4][3] = {{1,2,0},
            {1,0,2},
            {2,1,0},
            {0,1,2} };

    // tableau de correspondance entre ind arête et Face
    uint tabIndArToFace[4] = {2,3,4,5};

    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_45 = cofaces_45.begin();
    std::vector<CoFace*>::iterator iter1_90 = cofaces_90.begin();

    if (cofaces_0.size() != cofaces_45.size() || cofaces_0.size() != cofaces_90.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_45, ++iter1_90){

        CoFace* coface_0 = *iter1_0;
        CoFace* coface_45 = *iter1_45;
        CoFace* coface_90 = *iter1_90;

        uint ni_loc_coface = ni_coface[coface_0];
        if (ni_loc_coface == 0)
        	ni_loc_coface = m_ni;

#ifdef _DEBUG_REVOL
        std::cout<<"Construction du ou des blocs pour la Coface : "<<coface_0->getName()<<", filtre : "<<filtre_coface[coface_0]<<std::endl;
#endif
        if (filtre_coface[coface_0] == 12){
            // -------------------------------------------------------------------------------
            // cas d'un bloc sur l'axe
            // -------------------------------------------------------------------------------

            std::vector<Face* > faces;
            std::vector<Vertex* > vertices;
            if (coface_0->getNbEdges() == 4)
                faces.resize(6);
            else
                faces.resize(5);

            // recherche de l'indice de l'arête sur l'axe dans la coface,
            uint indEdgeNearAxe = 0;
            for (uint i=0; i<coface_0->getNbEdges(); i++)
                if (filtre_coedge[coface_0->getEdge(i)->getCoEdge(0)] >= 20)
                    indEdgeNearAxe = i;

            faces[0] = new Topo::Face(getContext(), coface_0);
            faces[2] = new Topo::Face(getContext(), coface_90);
#ifdef _DEBUG_REVOL
            std::cout<<"indEdgeNearAxe = "<<indEdgeNearAxe<<std::endl;
            std::cout<<"Construction de la faces[0] "<<faces[0]->getName()<<std::endl;
            std::cout<<"Construction de la faces[2] "<<faces[2]->getName()<<std::endl;
#endif
            getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
            getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);

            if (((coface_0->getNbEdges() == 4 && coface_45->getNbEdges() == 4 && coface_90->getNbEdges() == 4))
                    || (coface_0->getNbEdges() == 3 && coface_45->getNbEdges() == 3 && coface_90->getNbEdges() == 3)) {

                // on ordonne les Edges pour être compatible avec l'ordre des Face pour le Block
                for (uint i=0; i<coface_0->getNbEdges(); i++){

                    Edge* edge_0 = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i]);
                    Edge* edge_45 = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][i]);
                    Edge* edge_90 = coface_90->getEdge(tabIndArFace[indEdgeNearAxe][i]);

                    std::vector<CoEdge* > coedges_0;
                    std::vector<CoEdge* > coedges_45;
                    std::vector<CoEdge* > coedges_90;
                    edge_0->getCoEdges(coedges_0);
                    edge_45->getCoEdges(coedges_45);
                    edge_90->getCoEdges(coedges_90);

                    // pour la construction d'une face à partir d'une arête qui pointe sur plusieurs CoEdge
                    // cas de la face qui suit la révol, dans le cas d'un deto
                    std::vector<CoFace* > face_cofaces;

                    std::vector<CoEdge* >::iterator iter3_0 = coedges_0.begin();
                    std::vector<CoEdge* >::iterator iter3_45 = coedges_45.begin();
                    std::vector<CoEdge* >::iterator iter3_90 = coedges_90.begin();

                    if (coedges_0.size() == 1){
#ifdef _DEBUG_REVOL
                        std::cout<<"CoEdge observée: "<<(*iter3_0)->getName()<<", filtre : "<<filtre_coedge[*iter3_0]<<std::endl;
                        std::cout<<"CoEdge observée: "<<(*iter3_45)->getName()<<", filtre : "<<filtre_coedge[*iter3_45]<<std::endl;
                        std::cout<<"CoEdge observée: "<<(*iter3_90)->getName()<<", filtre : "<<filtre_coedge[*iter3_90]<<std::endl;
                        std::cout<<"CAS coedges_0.size() == 1\n";
#endif
                        // identification du nombre de faces à créer
                        uint nbFacesCreated = 0;

                        if (filtre_coedge[*iter3_0] == 1)
                            nbFacesCreated = 2;
                        else if (filtre_coedge[*iter3_0] == 12)
                            nbFacesCreated = 1;
                        else if (filtre_coedge[*iter3_0] == 22)
                            nbFacesCreated = 0;
                        else if (filtre_coedge[*iter3_0] == 11)
                            nbFacesCreated = 2;
                        else if (filtre_coedge[*iter3_0] == 2){
                            if (filtre_vertex[(*iter3_0)->getVertex(0)] == 6
                                    || filtre_vertex[(*iter3_0)->getVertex(1)] == 6)
                                nbFacesCreated = 3;
                            else
                                nbFacesCreated = 2;
                        }
                        else
                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec filtre sur arête non prévu", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_REVOL
                        std::cout<<"nbFacesCreated = "<<nbFacesCreated<<std::endl;
#endif
                        if (nbFacesCreated == 1){
                            std::vector<CoFace* >& revol_cofaces_0 = coedge2cofaces[*iter3_0];
                            faces[tabIndArToFace[i]] = new Topo::Face(getContext(), revol_cofaces_0[0]);
#ifdef _DEBUG_REVOL
                            std::cout<<"Construction de la faces["<<tabIndArToFace[i]<<"] "<<faces[tabIndArToFace[i]]->getName()<<std::endl;
#endif
                            getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);

                            // prise en compte de la semi-conformité
                            computeFaceRatio(faces[tabIndArToFace[i]], 2, ni_loc_coface);

                        }
                        else if (nbFacesCreated == 2){
                            std::vector<CoFace* >& revol_cofaces_0 = coedge2cofaces[*iter3_0];
                            std::vector<CoFace* >& revol_cofaces_45 = coedge2cofaces[*iter3_45];

                            //                            std::cout<<" revol_cofaces_0 :";
                            //                             for (uint j=0;j<revol_cofaces_0.size(); j++)
                            //                                std::cout<<" "<<revol_cofaces_0[j]->getName();
                            //                             std::cout<<std::endl;
                            //                             std::cout<<" revol_cofaces_45 :";
                            //                              for (uint j=0;j<revol_cofaces_45.size(); j++)
                            //                                 std::cout<<" "<<revol_cofaces_45[j]->getName();
                            //                              std::cout<<std::endl;

                            if (revol_cofaces_0.size() == 1 && revol_cofaces_45.size() == 1){
                                faces[3] = new Topo::Face(getContext(), revol_cofaces_0[0]);
                                faces[1] = new Topo::Face(getContext(), revol_cofaces_45[0]);
#ifdef _DEBUG_REVOL
                                std::cout<<"Construction de la faces[3] "<<faces[3]->getName()<<std::endl;
                                std::cout<<"Construction de la faces[1] "<<faces[1]->getName()<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                            }
                            else {
                                std::vector<Vertex* > face_vertices;

                                face_vertices.push_back(edge_0->getVertex(0));
                                face_vertices.push_back(edge_0->getVertex(1));
                                face_vertices.push_back(edge_45->getVertex(1));
                                face_vertices.push_back(edge_45->getVertex(0));
                                faces[3] = new Topo::Face(getContext(), revol_cofaces_0, face_vertices, true);
#ifdef _DEBUG_REVOL
                                std::cout<<"Construction de la faces[3] "<<faces[3]->getName()<<std::endl;
#endif
                                face_vertices.clear();
                                face_vertices.push_back(edge_45->getVertex(0));
                                face_vertices.push_back(edge_45->getVertex(1));
                                face_vertices.push_back(edge_90->getVertex(1));
                                face_vertices.push_back(edge_90->getVertex(0));

                                faces[1] = new Topo::Face(getContext(), revol_cofaces_45, face_vertices, true);
#ifdef _DEBUG_REVOL
                                std::cout<<"Construction de la faces[1] "<<faces[1]->getName()<<std::endl;
#endif
                                getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                                getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                            }
                        }
                        else if (nbFacesCreated == 3){
                            // on ne devrait pas être dans cette config
                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec 3 faces à créer et une coface de départ qui touche l'axe", TkUtil::Charset::UTF_8));
                        } // end else nbFacesCreated == ...

                    } // end if (coedges_0.size() == 1)
                    else if (filtre_coedge[*iter3_0] != 22){
#ifdef _DEBUG_REVOL
                        std::cout<<"CAS coedges_0.size() != 1\n";
#endif
                        // cas d'une Edge pointant sur plusieurs CoEdges
                        for (; iter3_0 != coedges_0.end(); ++iter3_0){
#ifdef _DEBUG_REVOL
                            std::cout<<"CoEdge observée: "<<(*iter3_0)->getName()<<", filtre : "<<filtre_coedge[*iter3_0]<<std::endl;
//                            std::cout<<"CoEdge observée: "<<(*iter3_45)->getName()<<", filtre : "<<filtre_coedge[*iter3_45]<<std::endl;
//                            std::cout<<"CoEdge observée: "<<(*iter3_90)->getName()<<", filtre : "<<filtre_coedge[*iter3_90]<<std::endl;
#endif
                            std::vector<CoFace* >& revol_cofaces_0 = coedge2cofaces[*iter3_0];
                            face_cofaces.insert(face_cofaces.end(), revol_cofaces_0.begin(), revol_cofaces_0.end());

                        } // end for coedges_0

                        std::vector<Vertex* > face_vertices;

                        // recherche de l'indice du sommet sur l'ogrid
                        uint indSomOnOgrid = 0;
                        if (filtre_vertex[edge_0->getVertex(0)]%5 == 1)
                            indSomOnOgrid = 0;
                        else if (filtre_vertex[edge_0->getVertex(1)]%5 == 1) {
                            indSomOnOgrid = 1;
                        } else {
                            std::cerr<<"Recherche sur le filtre pour l'arête "<<*edge_0<<std::endl;
                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 ne trouve pas le sommet sur l'axe pour une Edge", TkUtil::Charset::UTF_8));
                        }

                        face_vertices.push_back(edge_0->getVertex(indSomOnOgrid));
                        face_vertices.push_back(edge_45->getVertex(indSomOnOgrid));
                        face_vertices.push_back(edge_90->getVertex(indSomOnOgrid));
                        face_vertices.push_back(edge_0->getVertex((indSomOnOgrid+1)%2));

                        faces[tabIndArToFace[i]] = new Topo::Face(getContext(), face_cofaces, face_vertices, true);
                        getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
#ifdef _DEBUG_REVOL
                        std::cout<<"Construction de la faces["<<tabIndArToFace[i]<<"] "<<faces[tabIndArToFace[i]]->getName()<<std::endl;
#endif

                    } // end else / if (coedges_0.size() == 1)

                } // end for i<coface_0->getNbEdges()

                // on ordonne les sommets pour être compatible avec l'ordre pour le Block
                vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][0]));
                vertices.push_back(coface_90->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
                vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
                vertices.push_back(coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]));

                if (coface_0->getNbEdges() == 3){
                    // cas dégénéré (indEdgeNearAxe vaut 0 ou 2)
                    vertices.push_back(coface_0->getVertex(tabIndSomFaceDeg[indEdgeNearAxe][2]));
                }
                else {
                    vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][2]));
                    vertices.push_back(coface_90->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                    vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                    vertices.push_back(coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                }

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du bloc à l'intérieur de l'ogrid (sur l'axe) : "<<*newBlock;
                //newBlock->check();
#endif
            }
            else
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec nombres d'arêtes non prévus", TkUtil::Charset::UTF_8));

        } // end if (filtre_coface[coface_0] == 12)
        else if (filtre_coface[coface_0] == 2){
            // -------------------------------------------------------------------------------
            // cas des 3 blocs au centre de l'ogrid, mais autour d'un bloc central (type deto)
            // -------------------------------------------------------------------------------

            std::vector<Face* > faces;
            std::vector<Vertex* > vertices;
            if (coface_0->getNbEdges() == 4)
                faces.resize(6);
            else
                faces.resize(5);

            // recherche de l'indice de l'arête // et la plus proche dans le cas de la zone sous ogrid ne touchant pas l'axe (cas deto)
            uint indEdgeNearAxe = 0;
            for (uint i=0; i<coface_0->getNbEdges(); i++)
                if (filtre_vertex[coface_0->getEdge(i)->getCoEdge(0)->getVertex(0)] == 2
                        && filtre_vertex[coface_0->getEdge(i)->getCoEdge(0)->getVertex(1)] == 2){
                    //  if (coface_0->getNbEdges() == 3 && i == 2 && filtre_vertex[coface_0->getVertex(0)] == 11)
                    //     invVertexOrder = true;
                    indEdgeNearAxe = i;
                }

#ifdef _DEBUG_REVOL
            std::cout<<"indEdgeNearAxe = "<<indEdgeNearAxe<<std::endl;
#endif
            // la coface créée entre 2 blocs
            CoFace* newCoface = 0;

            // les sommets pour un bloc (réutilisable pour le bloc suivant
            Vertex *vtx0, *vtx1, *vtx2, *vtx3, *vtx4, *vtx5, *vtx6, *vtx7;

            // il nous faut construire 3 blocs et les 2 faces communes entre chacuns des blocs

            // le 1er bloc
            {
                vtx0 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx2 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx4 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][2]);
                vtx6 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                vtx1 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx5 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][2]);

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
#endif
                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx2];
                if (coedges0.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec autre chose que 2 arêtes pour vtx2", TkUtil::Charset::UTF_8));
                vtx3 = coedges0[0]->getOppositeVertex(vtx2);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
#endif
                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx6];
                if (coedges1.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec autre chose que 2 arêtes pour vtx6", TkUtil::Charset::UTF_8));
                vtx7 = coedges1[0]->getOppositeVertex(vtx6);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif

                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                faces[0] = new Topo::Face(getContext(), coface_0);
                getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);

                // récupération au passage des Edges pour la construction de la nouvelle face commune
                std::vector<Edge*> edges_newcoface;
                for (uint i=0; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);

                    // l'arête opposée
                    Edge* edge = coface->getEdgeContaining(coedge);
#ifdef _DEBUG_REVOL
                    std::cout<<"Recherche de l'arête "<<edge->getName() << " dans la face "<<*coface<<std::endl;
#endif
                    edges_newcoface.push_back(coface->getEdge((coface->getIndex(edge)+2)%4));
                }

                newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[2], edges_newcoface[1], edges_newcoface[3]);
                getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

#ifdef _DEBUG_REVOL
                std::cout<<" newCoface : "<<*newCoface<<std::endl;
#endif

                faces[1] = new Topo::Face(getContext(), newCoface);
                getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 1er bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

            // le deuxième bloc
            {
                vtx6 = vtx7;
                vtx7 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                vtx2 = vtx3;
                vtx3 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx4 = vtx5;
                vtx0 = vtx1;

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif
                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx3];
                if (coedges0.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec autre chose que 2 arêtes pour vtx3", TkUtil::Charset::UTF_8));
                vtx1 = coedges0[0]->getOppositeVertex(vtx3);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
#endif

                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx7];
                if (coedges1.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_4 avec autre chose que 2 arêtes pour vtx5", TkUtil::Charset::UTF_8));
                vtx5 = coedges1[0]->getOppositeVertex(vtx7);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
#endif

                vertices.clear();
                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                faces[0] = new Topo::Face(getContext(), newCoface);
                getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);

                // récupération des Edges pour la construction de la nouvelle face commune
                std::vector<Edge*> edges_newcoface;

                // les faces 3 4 et 5
                for (uint i=1; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][1];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                }

                edges_newcoface.push_back(faces[0]->getEdge(vtx0, vtx4));
                edges_newcoface.push_back(faces[4]->getEdge(vtx0, vtx1));
                edges_newcoface.push_back(faces[5]->getEdge(vtx4, vtx5));

                // la face 1
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[1] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);

                    // l'arête opposée
                    Edge* edge = coface->getEdgeContaining(coedge);
#ifdef _DEBUG_REVOL
                    std::cout<<"Recherche de l'arête "<<edge->getName() << " dans la face "<<*coface<<std::endl;
#endif
                    edges_newcoface.push_back(coface->getEdge((coface->getIndex(edge)+2)%4));
                }

                // la face 2
                newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[1], edges_newcoface[3], edges_newcoface[2]);
                getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

#ifdef _DEBUG_REVOL
                std::cout<<" newCoface : "<<*newCoface<<std::endl;
#endif
                faces[2] = new Topo::Face(getContext(), newCoface);
                getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 2ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

            // le troisième bloc
            {
                vtx6 = vtx4;
                vtx7 = vtx5;
                vtx3 = vtx1;
                vtx2 = vtx0;

                vtx0 = coface_90->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx1 = coface_90->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx4 = coface_90->getVertex(tabIndSomFace[indEdgeNearAxe][2]);
                vtx5 = coface_90->getVertex(tabIndSomFace[indEdgeNearAxe][3]);

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif
                vertices.clear();
                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                // la face 0
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][0])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[0] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                }
                // la face 1
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][1];
                    faces[1] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                }
                // la face 2
                {
                    faces[2] = new Topo::Face(getContext(), coface_90);
                    getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                }
                // la face 3
                {
                    faces[3] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                }
                // les faces 4 et 5
                for (uint i=2; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][2];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                }

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 3ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

        } // end else if (filtre_coface[coface_0] == 2)
    } // end for cofaces_0
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolBlocksInnerOgrid_2(std::vector<CoFace*>& cofaces_0,
        std::vector<CoFace*>& cofaces_45,
        std::vector<CoFace*>& cofaces_135,
        std::vector<CoFace*>& cofaces_180,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
        std::map<Vertex*, uint>& ni_vtx,
        std::map<CoFace*, uint>& ni_coface)
{
#ifdef _DEBUG_REVOL
    std::cout<<"\nconstructRevolBlocksInnerOgrid_2\n";
#endif
    if (m_portion != Utils::Portion::DEMI)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 n'est pas possible avec la portion actuelle", TkUtil::Charset::UTF_8));

    // tableau pour ordonner les arêtes des faces en fonction de l'arête de départ
    uint tabIndArFace[4][4] = {{0,2,1,3},
            {1,3,0,2},
            {2,0,1,3},
            {3,1,0,2} };
    // idem avec les sommets
    uint tabIndSomFace[4][4] = {{1,2,0,3},
            {1,0,2,3},
            {2,1,3,0},
            {0,1,3,2} };
    // idem avec les sommets pour le cas dégénéré
    uint tabIndSomFaceDeg[4][3] = {{1,2,0},
            {1,0,2},
            {2,1,0},
            {0,1,2} };

    // tableau de correspondance entre ind arête et Face
    uint tabIndArToFace[4] = {2,3,4,5};

    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_45 = cofaces_45.begin();
    std::vector<CoFace*>::iterator iter1_135 = cofaces_135.begin();
    std::vector<CoFace*>::iterator iter1_180 = cofaces_180.begin();

    if (cofaces_0.size() != cofaces_45.size()
            || cofaces_0.size() != cofaces_135.size()
            || cofaces_0.size() != cofaces_180.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_45, ++iter1_135, ++iter1_180){

        CoFace* coface_0 = *iter1_0;
        CoFace* coface_45 = *iter1_45;
        CoFace* coface_135 = *iter1_135;
        CoFace* coface_180 = *iter1_180;

        uint ni_loc_coface = ni_coface[coface_0];
        if (ni_loc_coface == 0)
        	ni_loc_coface = m_ni;

#ifdef _DEBUG_REVOL
        std::cout<<"Construction du ou des blocs pour la Coface : "<<coface_0->getName()<<", filtre : "<<filtre_coface[coface_0]<<std::endl;
#endif
        if (filtre_coface[coface_0] == 12){
            // -------------------------------------------------------------------------------
            // cas d'un bloc sur l'axe
            // -------------------------------------------------------------------------------

            std::vector<Face* > faces;
            std::vector<Vertex* > vertices;
            if (coface_0->getNbEdges() == 4)
                faces.resize(6);
            else
                faces.resize(5);

            // recherche de l'indice de l'arête sur l'axe dans la coface,
            uint indEdgeNearAxe = 0;
            for (uint i=0; i<coface_0->getNbEdges(); i++)
                if (filtre_coedge[coface_0->getEdge(i)->getCoEdge(0)] >= 20)
                    indEdgeNearAxe = i;
#ifdef _DEBUG_REVOL
            std::cout<<"indEdgeNearAxe = "<<indEdgeNearAxe<<std::endl;
#endif

            // Face 0
            {
                std::vector<Vertex* > face_vertices;
                std::vector<CoFace* > face_cofaces;
                face_cofaces.push_back(coface_0);
                face_cofaces.push_back(coface_180);
                if (coface_0->getNbEdges() == 4){
                	face_vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
                	face_vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                	face_vertices.push_back(coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                	face_vertices.push_back(coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
                }
                else {
                	face_vertices.push_back(coface_0->getVertex(tabIndSomFaceDeg[indEdgeNearAxe][2]));
                	face_vertices.push_back(coface_0->getVertex(tabIndSomFaceDeg[indEdgeNearAxe][1]));
                	face_vertices.push_back(coface_180->getVertex(tabIndSomFaceDeg[indEdgeNearAxe][1]));
                }
                faces[0] = new Topo::Face(getContext(), face_cofaces, face_vertices, true);
#ifdef _DEBUG_REVOL
                std::cout<<"Construction de la faces[0] "<<faces[0]->getName()<<std::endl;
#endif
                getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
            }

            // Face 1 à 3
            faces[1] = constructFaceWith1RevolCoFace(coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            faces[2] = constructFaceWith1RevolCoFace(coface_135->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            faces[3] = constructFaceWith1RevolCoFace(coface_0->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            // Face 4
            {
                Edge* edge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][2]);
                if (edge->getNbCoEdges() == 1){
                    faces[4] = constructFaceWith1RevolCoFace(edge, coedge2cofaces);
                    // prise en compte de la semi-conformité
                    computeFaceRatio(faces[4] , 0, ni_loc_coface);
                    computeFaceRatio(faces[4] , 1, ni_loc_coface*2);
                }
                else
                    faces[4] = constructFaceWithRevolCoFaces(edge, coedge2cofaces,
                            coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][1]),
                            coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]),
                            coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][1]),
                            coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            }

            // Face 5
            if (coface_0->getNbEdges() == 4) {
                Edge* edge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][3]);
                if (edge->getNbCoEdges() == 1){
                    faces[5] = constructFaceWith1RevolCoFace(edge, coedge2cofaces);
                    // prise en compte de la semi-conformité
                    computeFaceRatio(faces[5] , 0, ni_loc_coface);
                    computeFaceRatio(faces[5] , 1, ni_loc_coface*2);
                }
                else
                    faces[5] = constructFaceWithRevolCoFaces(edge, coedge2cofaces,
                            coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][3]),
                            coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]),
                            coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][3]),
                            coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
            }

            // Sommets ordonnés pour être compatible avec l'ordre pour le Block
            vertices.push_back(coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            vertices.push_back(coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            vertices.push_back(coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]));


            if (coface_0->getNbEdges() == 4){
                vertices.push_back(coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                vertices.push_back(coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                vertices.push_back(coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                vertices.push_back(coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
            }
            else{
            	vertices.push_back(coface_0->getVertex(tabIndSomFaceDeg[indEdgeNearAxe][2]));
            }

            // Block (unique)
            Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
            getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
            newBlock->selectBasicMeshLaw(&getInfoCommand());
            updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
            std::cout<<"\nCréation du bloc à l'intérieur de l'ogrid (sur l'axe) : "<<*newBlock;
            //newBlock->check();
#endif
        } // end if (filtre_coface[coface_0] == 12)
        else if (filtre_coface[coface_0] == 2){
            // -------------------------------------------------------------------------------
            // cas des 5 blocs au centre de l'ogrid, mais autour d'un bloc central (type deto)
            // -------------------------------------------------------------------------------

            std::vector<Face* > faces;
            std::vector<Vertex* > vertices;
            if (coface_0->getNbEdges() == 4)
                faces.resize(6);
            else
                faces.resize(5);

            // recherche de l'indice de l'arête // et la plus proche dans le cas de la zone sous ogrid ne touchant pas l'axe (cas deto)
            uint indEdgeNearAxe = 0;
            for (uint i=0; i<coface_0->getNbEdges(); i++)
                if (filtre_vertex[coface_0->getEdge(i)->getCoEdge(0)->getVertex(0)] == 2
                        && filtre_vertex[coface_0->getEdge(i)->getCoEdge(0)->getVertex(1)] == 2){
                    //  if (coface_0->getNbEdges() == 3 && i == 2 && filtre_vertex[coface_0->getVertex(0)] == 11)
                    //     invVertexOrder = true;
                    indEdgeNearAxe = i;
                }

#ifdef _DEBUG_REVOL
            std::cout<<"indEdgeNearAxe = "<<indEdgeNearAxe<<std::endl;
#endif
            // la coface créée entre 2 blocs
            CoFace* newCoface = 0;

            // les sommets pour un bloc (réutilisable bour le bloc suivant
            Vertex *vtx0, *vtx1, *vtx2, *vtx3, *vtx4, *vtx5, *vtx6, *vtx7;

            // il nous faut construire 5 blocs et les 4 faces communes entre chacuns des blocs

            // le 1er bloc
            {
                vtx0 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx2 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx4 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][2]);
                vtx6 = coface_0->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                vtx1 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx5 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][2]);

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
#endif
                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx2];
                if (coedges0.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 2 arêtes pour vtx2", TkUtil::Charset::UTF_8));
                vtx3 = coedges0[0]->getOppositeVertex(vtx2);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
#endif
                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx6];
                if (coedges1.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 2 arêtes pour vtx6", TkUtil::Charset::UTF_8));
                vtx7 = coedges1[0]->getOppositeVertex(vtx6);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif

                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                faces[0] = new Topo::Face(getContext(), coface_0);
                getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);

                // récupération au passage des Edges pour la construction de la nouvelle face commune
                std::vector<Edge*> edges_newcoface;
                for (uint i=0; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);

                    // l'arête opposée
                    Edge* edge = coface->getEdgeContaining(coedge);
#ifdef _DEBUG_REVOL
                    std::cout<<"Recherche de l'arête "<<edge->getName() << " dans la face "<<*coface<<std::endl;
#endif
                    edges_newcoface.push_back(coface->getEdge((coface->getIndex(edge)+2)%4));
                }

                newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[2], edges_newcoface[1], edges_newcoface[3]);
                getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

#ifdef _DEBUG_REVOL
                std::cout<<" newCoface : "<<*newCoface<<std::endl;
#endif

                faces[1] = new Topo::Face(getContext(), newCoface);
                getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 1er bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

            // le deuxième bloc
            {
                vtx6 = vtx7;
                vtx7 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                vtx2 = vtx3;
                vtx3 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx4 = vtx5;
                vtx0 = vtx1;

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif
                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx3];
                if (coedges0.size() != 3)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 3 arêtes pour vtx3", TkUtil::Charset::UTF_8));
                vtx1 = coedges0[0]->getOppositeVertex(vtx3);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
#endif

                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx7];
                if (coedges1.size() != 3)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 3 arêtes pour vtx7", TkUtil::Charset::UTF_8));
                vtx5 = coedges1[0]->getOppositeVertex(vtx7);
#ifdef _DEBUG_REVOL
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
#endif

                vertices.clear();
                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                // la face 0
                {
                    faces[0] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                }

                // récupération des Edges pour la construction de la nouvelle face commune
                std::vector<Edge*> edges_newcoface;

                // les faces 3 4 et 5
                for (uint i=1; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][1];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                }

                edges_newcoface.push_back(faces[0]->getEdge(vtx0, vtx4));
                edges_newcoface.push_back(faces[4]->getEdge(vtx0, vtx1));
                edges_newcoface.push_back(faces[5]->getEdge(vtx4, vtx5));

                // la face 1
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[1] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);

                    // l'arête opposée
                    Edge* edge = coface->getEdgeContaining(coedge);
#ifdef _DEBUG_REVOL
                    std::cout<<"Recherche de l'arête "<<edge->getName() << " dans la face "<<*coface<<std::endl;
#endif
                    edges_newcoface.push_back(coface->getEdge((coface->getIndex(edge)+2)%4));
                }

                // la face 2
                {
                    newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[1], edges_newcoface[3], edges_newcoface[2]);
                    getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

#ifdef _DEBUG_REVOL
                    std::cout<<" newCoface : "<<*newCoface<<std::endl;
#endif
                    faces[2] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                }

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 2ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

            // le troisième bloc
            {
                Vertex* vtx3B2 = vtx3;
                Vertex* vtx7B2 = vtx7;

                vtx6 = vtx4;
                vtx7 = vtx5;
                vtx3 = vtx1;
                vtx2 = vtx0;

                vtx0 = coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx4 = coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][2]);

                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx3B2];
                if (coedges0.size() != 3)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 3 arêtes pour vtx3B2 ", TkUtil::Charset::UTF_8));
                vtx1 = coedges0[1]->getOppositeVertex(vtx3);

                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx7B2];
                if (coedges1.size() != 3)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 3 arêtes pour vtx7B2 ", TkUtil::Charset::UTF_8));
                vtx5 = coedges1[1]->getOppositeVertex(vtx7);

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif
                vertices.clear();
                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                // la face 0
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][0])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[0] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                }
                // la face 1
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][1];
                    faces[1] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                }
                // la face 3
                {
                    faces[3] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                }
                // les faces 4 et 5
                for (uint i=2; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][2];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                }

                // la face 2
                {
                    // récupération des Edges pour la construction de la nouvelle face commune
                    std::vector<Edge*> edges_newcoface;
                    edges_newcoface.push_back(faces[0]->getEdge(vtx0, vtx4));
                    edges_newcoface.push_back(faces[4]->getEdge(vtx0, vtx1));
                    edges_newcoface.push_back(faces[1]->getEdge(vtx5, vtx1));
                    edges_newcoface.push_back(faces[5]->getEdge(vtx5, vtx4));

                    newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[1], edges_newcoface[2], edges_newcoface[3]);
                    getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

                    faces[2] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                }

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 3ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

            // le quatrième bloc
            {
                vtx6 = vtx4;
                vtx7 = vtx5;
                vtx3 = vtx1;
                vtx2 = vtx0;

                vtx1 = coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx5 = coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][3]);

                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx1];
                if (coedges0.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 2 arêtes pour vtx1", TkUtil::Charset::UTF_8));
                vtx0 = coedges0[0]->getOppositeVertex(vtx1);

                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx5];
                if (coedges1.size() != 2)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_2 avec autre chose que 2 arêtes pour vtx5", TkUtil::Charset::UTF_8));
                vtx4 = coedges1[0]->getOppositeVertex(vtx5);

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif

                vertices.clear();
                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                // récupération des Edges pour la construction de la nouvelle face commune
                std::vector<Edge*> edges_newcoface;

                // la face 1
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][2];
                    faces[1] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                }
                // la face 2
                {
                    CoEdge* coedge = coface_135->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[2] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                }
                // la face 3
                {
                    faces[3] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                }
                // les faces 4 et 5
                for (uint i=2; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][3];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                }

                // la face 0
                {
                    // récupération des Edges pour la construction de la nouvelle face commune
                    std::vector<Edge*> edges_newcoface;
                    edges_newcoface.push_back(faces[2]->getEdge(vtx0, vtx4));
                    edges_newcoface.push_back(faces[4]->getEdge(vtx0, vtx2));
                    edges_newcoface.push_back(faces[3]->getEdge(vtx6, vtx2));
                    edges_newcoface.push_back(faces[5]->getEdge(vtx6, vtx4));

                    newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[1], edges_newcoface[2], edges_newcoface[3]);
                    getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

                    faces[0] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                }

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 4ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

            // le cinquième bloc
            {
                vtx7 = vtx6;
                vtx3 = vtx2;
                vtx1 = vtx0;
                vtx5 = vtx4;

                vtx0 = coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx2 = coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                vtx4 = coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                vtx6 = coface_180->getVertex(tabIndSomFace[indEdgeNearAxe][2]);

#ifdef _DEBUG_REVOL
                std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
                std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif

                vertices.clear();
                vertices.push_back(vtx0);
                vertices.push_back(vtx1);
                vertices.push_back(vtx2);
                vertices.push_back(vtx3);
                vertices.push_back(vtx4);
                vertices.push_back(vtx5);
                vertices.push_back(vtx6);
                vertices.push_back(vtx7);

                // la face 0
                {
                    faces[0] = new Topo::Face(getContext(), coface_180);
                    getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                }

                // la face 1
                {
                    faces[1] = new Topo::Face(getContext(), newCoface);
                    getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                }
                // la face 2
                {
                    CoEdge* coedge = coface_135->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][1];
                    faces[2] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                }
                // la face 3
                {
                    CoEdge* coedge = coface_135->getEdge(tabIndArFace[indEdgeNearAxe][0])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    faces[3] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                }

                // les faces 4 et 5
                for (uint i=2; i<4; i++){
                    CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][4];
                    faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                    getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                }

                Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                newBlock->selectBasicMeshLaw(&getInfoCommand());
                updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                std::cout<<"\nCréation du 5ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                //newBlock->check();
#endif
            }

        } // end else if (filtre_coface[coface_0] == 2)

    } // end for cofaces_0
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
constructRevolBlocksInnerOgrid_1(std::vector<CoFace*>& cofaces_45,
        std::vector<CoFace*>& cofaces_135,
        std::vector<CoFace*>& cofaces_225,
        std::vector<CoFace*>& cofaces_315,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface,
        std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
        std::map<Vertex*, uint>& ni_vtx,
        std::map<CoFace*, uint>& ni_coface)
{
#ifdef _DEBUG_REVOL
    std::cout<<"\nconstructRevolBlocksInnerOgrid_1\n";
#endif
    if (m_portion != Utils::Portion::ENTIER)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 n'est pas possible avec la portion actuelle", TkUtil::Charset::UTF_8));

    // tableau pour ordonner les arêtes des faces en fonction de l'arête de départ
    uint tabIndArFace[4][4] = {{0,2,1,3},
            {1,3,0,2},
            {2,0,1,3},
            {3,1,0,2} };
    // idem avec les sommets
    uint tabIndSomFace[4][4] = {{1,2,0,3},
            {1,0,2,3},
            {2,1,3,0},
            {0,1,3,2} };
    // idem avec les sommets pour le cas dégénéré
    uint tabIndSomFaceDeg[4][3] = {{1,2,0},
            {1,0,2},
            {2,1,0},
            {0,1,2} };

    // tableau de correspondance entre ind arête et Face
    uint tabIndArToFace[4] = {2,3,4,5};

    std::vector<CoFace*>::iterator iter1_45 = cofaces_45.begin();
    std::vector<CoFace*>::iterator iter1_135 = cofaces_135.begin();
    std::vector<CoFace*>::iterator iter1_225 = cofaces_225.begin();
    std::vector<CoFace*>::iterator iter1_315 = cofaces_315.begin();

    if (cofaces_45.size() != cofaces_135.size()
            || cofaces_45.size() != cofaces_225.size()
            || cofaces_45.size() != cofaces_315.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_45 != cofaces_45.end(); ++iter1_45, ++iter1_135, ++iter1_225, ++iter1_315){

        CoFace* coface_45 = *iter1_45;
        CoFace* coface_135 = *iter1_135;
        CoFace* coface_225 = *iter1_225;
        CoFace* coface_315 = *iter1_315;

        uint ni_loc_coface = ni_coface[coface_45];
        if (ni_loc_coface == 0)
        	ni_loc_coface = m_ni;

#ifdef _DEBUG_REVOL
        std::cout<<"Construction du ou des blocs pour la Coface : "<<coface_45->getName()<<", filtre : "<<filtre_coface[coface_45]<<std::endl;
#endif
        if (filtre_coface[coface_45] == 12){
            // -------------------------------------------------------------------------------
            // cas d'un bloc sur l'axe
            // -------------------------------------------------------------------------------

            std::vector<Face* > faces;
            std::vector<Vertex* > vertices;
            if (coface_45->getNbEdges() == 4)
                faces.resize(6);
            else
                faces.resize(5);

            // recherche de l'indice de l'arête sur l'axe dans la coface,
            uint indEdgeNearAxe = 0;
            for (uint i=0; i<coface_45->getNbEdges(); i++)
                if (filtre_coedge[coface_45->getEdge(i)->getCoEdge(0)] >= 20)
                    indEdgeNearAxe = i;
#ifdef _DEBUG_REVOL
            std::cout<<"indEdgeNearAxe = "<<indEdgeNearAxe<<std::endl;
#endif

            // Face 0 1 2 et 3
            faces[0] = constructFaceWith1RevolCoFace(coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            faces[3] = constructFaceWith1RevolCoFace(coface_135->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            faces[1] = constructFaceWith1RevolCoFace(coface_225->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            faces[2] = constructFaceWith1RevolCoFace(coface_315->getEdge(tabIndArFace[indEdgeNearAxe][1]),
                    coedge2cofaces);
            // Face 4
            {
            	Edge* edge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][2]);
            	if (edge->getNbCoEdges() == 1){
            		faces[4] = constructFaceWith1RevolCoFace(edge, coedge2cofaces);
            		// prise en compte de la semi-conformité
            		computeFaceRatio(faces[4] , 2, ni_loc_coface*2);
            	}
            	else
            		faces[4] = constructFaceWithRevolCoFaces(edge, coedge2cofaces,
            				coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]),
            				coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][1]),
            				coface_225->getVertex(tabIndSomFace[indEdgeNearAxe][1]),
            				coface_315->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            }

            // Face 5
            if (coface_45->getNbEdges() == 4) {
            	Edge* edge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][3]);
            	if (edge->getNbCoEdges() == 1){
            		faces[5] = constructFaceWith1RevolCoFace(edge, coedge2cofaces);
            		// prise en compte de la semi-conformité
            		computeFaceRatio(faces[5] , 2, ni_loc_coface*2);
            	}
            	else
                    faces[5] = constructFaceWithRevolCoFaces(edge, coedge2cofaces,
                            coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]),
                            coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][3]),
                            coface_225->getVertex(tabIndSomFace[indEdgeNearAxe][3]),
                            coface_315->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
            }

            // Sommets ordonnés pour être compatible avec l'ordre pour le Block
            vertices.push_back(coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            vertices.push_back(coface_315->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            vertices.push_back(coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            vertices.push_back(coface_225->getVertex(tabIndSomFace[indEdgeNearAxe][1]));
            if (coface_45->getNbEdges() == 4){
                vertices.push_back(coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                vertices.push_back(coface_315->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                vertices.push_back(coface_135->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
                vertices.push_back(coface_225->getVertex(tabIndSomFace[indEdgeNearAxe][3]));
            }
            else
                vertices.push_back(coface_45->getVertex(tabIndSomFaceDeg[indEdgeNearAxe][2]));

            // Block (unique)
            Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
            getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
            newBlock->selectBasicMeshLaw(&getInfoCommand());
            updateGeomAssociation(coface_45->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
            std::cout<<"\nCréation du bloc à l'intérieur de l'ogrid (sur l'axe) : "<<*newBlock;
            //newBlock->check();
#endif
        } // end if (filtre_coface[coface_0] == 12)
        else if (filtre_coface[coface_45] == 2){
            // -------------------------------------------------------------------------------
            // cas des 8 blocs au centre de l'ogrid, mais autour d'un bloc central (type deto)
            // -------------------------------------------------------------------------------

            std::vector<Face* > faces;
            std::vector<Vertex* > vertices;
            if (coface_45->getNbEdges() == 4)
                faces.resize(6);
            else
                faces.resize(5);

            // recherche de l'indice de l'arête // et la plus proche dans le cas de la zone sous ogrid ne touchant pas l'axe (cas deto)
            uint indEdgeNearAxe = 0;
            for (uint i=0; i<coface_45->getNbEdges(); i++)
                if (filtre_vertex[coface_45->getEdge(i)->getCoEdge(0)->getVertex(0)] == 2
                        && filtre_vertex[coface_45->getEdge(i)->getCoEdge(0)->getVertex(1)] == 2){
                    indEdgeNearAxe = i;
                }

#ifdef _DEBUG_REVOL
            std::cout<<"indEdgeNearAxe = "<<indEdgeNearAxe<<std::endl;
#endif

            // la coface créée entre 2 blocs
            CoFace* newCoface = 0;
            CoFace* firstNewCoface = 0;

            // les sommets pour un bloc (réutilisable bour le bloc suivant
            Vertex *vtx0, *vtx1, *vtx2, *vtx3, *vtx4, *vtx5, *vtx6, *vtx7;

            // inuitialisation de vtx 1 4 5 3 et 7
            // création d'une première newCoface (à mettre de côté)
            {
                vtx7 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                vtx3 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                vtx4 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][2]);
                vtx0 = coface_45->getVertex(tabIndSomFace[indEdgeNearAxe][0]);

                vtx6 = 0;
                vtx2 = 0;

                std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx3];
                if (coedges0.size() != 3)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec autre chose que 3 arêtes pour vtx3", TkUtil::Charset::UTF_8));
                vtx1 = coedges0[0]->getOppositeVertex(vtx3);

                std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx7];
                if (coedges1.size() != 3)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec autre chose que 3 arêtes pour vtx7", TkUtil::Charset::UTF_8));
                vtx5 = coedges1[0]->getOppositeVertex(vtx7);

                // récupération des Edges pour la construction de la nouvelle face commune
                std::vector<Edge*> edges_newcoface;

                edges_newcoface.push_back(coface_45->getEdge(tabIndArFace[indEdgeNearAxe][0]));

                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    edges_newcoface.push_back(coface->getEdge(vtx1, vtx5));
                }
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][2])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    edges_newcoface.push_back(coface->getEdge(vtx0, vtx1));
                }
                {
                    CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][3])->getCoEdge(0);
                    CoFace* coface = coedge2cofaces[coedge][0];
                    edges_newcoface.push_back(coface->getEdge(vtx4, vtx5));
                }

                newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[2], edges_newcoface[1], edges_newcoface[3]);
                getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);
            }

            firstNewCoface = newCoface;

            // boucle pour la création des 4 couples de 2 blocs
            for (uint j=0; j<4; j++){
                CoFace* coface_0 = coface_45;
                CoFace* coface_1 = coface_135;

                if (j==1){
                    coface_0 = coface_135;
                    coface_1 = coface_225;
                }
                else if (j==2){
                    coface_0 = coface_225;
                    coface_1 = coface_315;
                }
                else if (j==3){
                    coface_0 = coface_315;
                    coface_1 = coface_45;
                }

                // Création du premier bloc
                {
                    Vertex* vtx3B0 = vtx3;
                    Vertex* vtx7B0 = vtx7;

                    vtx2 = vtx1;
                    vtx6 = vtx5;
                    // vtx4 et vtx0 inchangés

                    std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx3B0];
                    if (coedges0.size() != 3)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec autre chose que 3 arêtes pour vtx3B0 ", TkUtil::Charset::UTF_8));
                    vtx3 = coedges0[1]->getOppositeVertex(vtx2);

                    std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx7B0];
                    if (coedges1.size() != 3)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec autre chose que 3 arêtes pour vtx7B0 ", TkUtil::Charset::UTF_8));
                    vtx7 = coedges1[1]->getOppositeVertex(vtx6);

                    vtx1 = coface_1->getVertex(tabIndSomFace[indEdgeNearAxe][0]);
                    vtx5 = coface_1->getVertex(tabIndSomFace[indEdgeNearAxe][2]);

#ifdef _DEBUG_REVOL
                    std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                    std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                    std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                    std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                    std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                    std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
                    std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                    std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif
                    vertices.clear();
                    vertices.push_back(vtx0);
                    vertices.push_back(vtx1);
                    vertices.push_back(vtx2);
                    vertices.push_back(vtx3);
                    vertices.push_back(vtx4);
                    vertices.push_back(vtx5);
                    vertices.push_back(vtx6);
                    vertices.push_back(vtx7);


                    // la face 0
                    {
                        faces[0] = new Topo::Face(getContext(), newCoface);
                        getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                    }
                    // la face 2
                    {
                        CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][0])->getCoEdge(0);
                        CoFace* coface = coedge2cofaces[coedge][0];
                        faces[2] = new Topo::Face(getContext(), coface);
                        getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                    }
                    // la face 3
                    {
                         CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                         CoFace* coface = coedge2cofaces[coedge][1];
                         faces[3] = new Topo::Face(getContext(), coface);
                         getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                     }
                    // les faces 4 et 5
                    for (uint i=2; i<4; i++){
                        CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                        CoFace* coface = coedge2cofaces[coedge][j*2];
                        faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                        getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                    }
                    // la face 1
                    {
                        // récupération des Edges pour la construction de la nouvelle face commune
                        std::vector<Edge*> edges_newcoface;
                        edges_newcoface.push_back(faces[2]->getEdge(vtx1, vtx5));
                        edges_newcoface.push_back(faces[4]->getEdge(vtx1, vtx3));
                        edges_newcoface.push_back(faces[3]->getEdge(vtx7, vtx3));
                        edges_newcoface.push_back(faces[5]->getEdge(vtx7, vtx5));

                        newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[1], edges_newcoface[2], edges_newcoface[3]);
                        getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

                        faces[1] = new Topo::Face(getContext(), newCoface);
                        getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                    }

                    Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                    getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                    newBlock->selectBasicMeshLaw(&getInfoCommand());
                    updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                    std::cout<<"\nCréation du "<<j*2<<"ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                    //newBlock->check();
#endif
                }

                // Création du deuxième bloc
                {
                    vtx6 = vtx7;
                    vtx7 = coface_1->getVertex(tabIndSomFace[indEdgeNearAxe][3]);
                    vtx2 = vtx3;
                    vtx3 = coface_1->getVertex(tabIndSomFace[indEdgeNearAxe][1]);
                    vtx4 = vtx5;
                    vtx0 = vtx1;

                    std::vector<CoEdge* > &coedges0 = vtx2coedges[vtx3];
                    if (coedges0.size() != 3)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec autre chose que 3 arêtes pour vtx3", TkUtil::Charset::UTF_8));
                    vtx1 = coedges0[0]->getOppositeVertex(vtx3);

                    std::vector<CoEdge* > &coedges1 = vtx2coedges[vtx7];
                    if (coedges1.size() != 3)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructRevolBlocksInnerOgrid_1 avec autre chose que 3 arêtes pour vtx7", TkUtil::Charset::UTF_8));
                    vtx5 = coedges1[0]->getOppositeVertex(vtx7);

#ifdef _DEBUG_REVOL
                    std::cout<<"vtx0 = "<<vtx0->getName()<<std::endl;
                    std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
                    std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
                    std::cout<<"vtx3 = "<<vtx3->getName()<<std::endl;
                    std::cout<<"vtx4 = "<<vtx4->getName()<<std::endl;
                    std::cout<<"vtx5 = "<<vtx5->getName()<<std::endl;
                    std::cout<<"vtx6 = "<<vtx6->getName()<<std::endl;
                    std::cout<<"vtx7 = "<<vtx7->getName()<<std::endl;
#endif
                    vertices.clear();
                    vertices.push_back(vtx0);
                    vertices.push_back(vtx1);
                    vertices.push_back(vtx2);
                    vertices.push_back(vtx3);
                    vertices.push_back(vtx4);
                    vertices.push_back(vtx5);
                    vertices.push_back(vtx6);
                    vertices.push_back(vtx7);

                    // la face 0
                    {
                        faces[0] = new Topo::Face(getContext(), newCoface);
                        getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
                    }
                    // la face 1
                    {
                         CoEdge* coedge = coface_1->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                         CoFace* coface = coedge2cofaces[coedge][0];
                         faces[1] = new Topo::Face(getContext(), coface);
                         getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);
                     }
                    // la face 3
                    {
                         CoEdge* coedge = coface_0->getEdge(tabIndArFace[indEdgeNearAxe][1])->getCoEdge(0);
                         CoFace* coface = coedge2cofaces[coedge][2];
                         faces[3] = new Topo::Face(getContext(), coface);
                         getInfoCommand().addTopoInfoEntity(faces[3], Internal::InfoCommand::CREATED);
                     }
                    // les faces 4 et 5
                    for (uint i=2; i<4; i++){
                        CoEdge* coedge = coface_45->getEdge(tabIndArFace[indEdgeNearAxe][i])->getCoEdge(0);
                        CoFace* coface = coedge2cofaces[coedge][j*2+1];
                        faces[tabIndArToFace[i]] = new Topo::Face(getContext(), coface);
                        getInfoCommand().addTopoInfoEntity(faces[tabIndArToFace[i]], Internal::InfoCommand::CREATED);
                    }
                    // la face 2
                    if (j==3){
                        faces[2] = new Topo::Face(getContext(), firstNewCoface);
                        getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                    }
                    else {
                        // récupération des Edges pour la construction de la nouvelle face commune
                        std::vector<Edge*> edges_newcoface;
                        edges_newcoface.push_back(faces[0]->getEdge(vtx0, vtx4));
                        edges_newcoface.push_back(faces[4]->getEdge(vtx0, vtx1));
                        edges_newcoface.push_back(faces[1]->getEdge(vtx5, vtx1));
                        edges_newcoface.push_back(faces[5]->getEdge(vtx5, vtx4));

                        newCoface = new Topo::CoFace(getContext(), edges_newcoface[0], edges_newcoface[1], edges_newcoface[2], edges_newcoface[3]);
                        getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

                        faces[2] = new Topo::Face(getContext(), newCoface);
                        getInfoCommand().addTopoInfoEntity(faces[2], Internal::InfoCommand::CREATED);
                    }

                    Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
                    getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
                    newBlock->selectBasicMeshLaw(&getInfoCommand());
                    updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG_REVOL
                    std::cout<<"\nCréation du "<<j*2+1<<"ème bloc à l'intérieur de l'ogrid (autour du bloc central) : "<<*newBlock;
                    //newBlock->check();
#endif
                }
            } // end for (uint i=0; i<1; i++)

        } // end else if (filtre_coface[coface_0] == 2)

    } // end for cofaces_0
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
updateGeomAssociation(Geom::GeomEntity* ge, CoEdge* coedge)
{
    //std::cout<<"updateGeomAssociation("<<ge->getName()<<", "<<coedge->getName()<<")\n";
//    coedge->saveTopoProperty();
//    getInfoCommand().addTopoInfoEntity(coedge,Internal::InfoCommand::DISPMODIFIED);

    Geom::Vertex* vtx = dynamic_cast<Geom::Vertex*>(ge);
    if (vtx)
        coedge->setGeomAssociation(m_commandGeom->getAssociationV2C()[vtx]);
    else {
        Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
        if (crv)
            coedge->setGeomAssociation(m_commandGeom->getAssociationC2S()[crv]);
        else {
            Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
            if (surf)
                coedge->setGeomAssociation(0);
        }
    }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
updateGeomAssociation(Geom::GeomEntity* ge, CoFace* coface)
{
//    coface->saveTopoProperty();
//    getInfoCommand().addTopoInfoEntity(coface,Internal::InfoCommand::DISPMODIFIED);

    Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
    if (crv)
        coface->setGeomAssociation(m_commandGeom->getAssociationC2S()[crv]);
    else {
        Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
        if (surf)
            coface->setGeomAssociation(0);
    }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
updateGeomAssociation(Geom::GeomEntity* ge, Block* block)
{
//    block->saveTopoProperty();
//    getInfoCommand().addTopoInfoEntity(block,Internal::InfoCommand::DISPMODIFIED);

    Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
    if (surf)
        block->setGeomAssociation(m_commandGeom->getAssociationS2V()[surf]);
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
updateGeomAssociationToSurf(Geom::GeomEntity* ge, Vertex* vertex)
{
//    std::cout<<"updateGeomAssociationToSurf() pour "<<*vertex<<std::endl;

//    vertex->saveTopoProperty();
//    getInfoCommand().addTopoInfoEntity(vertex,Internal::InfoCommand::DISPMODIFIED);

    Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
    if (crv)
        vertex->setGeomAssociation(m_commandGeom->getAssociationC2S()[crv]);
    else if (ge && (ge->getType() != Utils::Entity::GeomSurface)){
#ifdef _DEBUG_REVOL
    	std::cout<<"freeUnused: setGeomAssociation(0) pour "<<vertex->getName()<<std::endl;
#endif
        vertex->setGeomAssociation(0);
    }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
updateGeomAssociationOGrid(std::vector<Vertex*>& vertices,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge)
{
//    std::cout<<"updateGeomAssociationOGrid ...\n";
    for (std::vector<Vertex* >::iterator iter4 = vertices.begin();
                                iter4 != vertices.end(); ++iter4){
        Vertex* vtx = *iter4;

        if (filtre_vertex[vtx] != 11){

            std::vector<CoEdge* > coedges;
            vtx->getCoEdges(coedges);

            Geom::Curve* crv_ogrid = 0;
            for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3){
                // recherche d'une courbe sur laquelle est fait la projection d'une arête
                // en dehors de l'ogrid. Arête adjacente au sommet.
                Geom::GeomEntity* ge = (*iter3)->getGeomAssociation();

                if (ge && ge->getType() == Utils::Entity::GeomCurve && filtre_coedge[*iter3]%10 != 1) {
                    Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
                    CHECK_NULL_PTR_ERROR(crv);
                    if (crv_ogrid && crv_ogrid != crv){
						TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                        messErr << "L'association du sommet "<<vtx->getName()<<
                                " sur une surface ne peut se faire.\n";
                        messErr <<"Les arêtes de part et d'autres du sommet et sur le tracé de l'ogrid sont projetées sur des courbes disjointes: "
                                <<crv_ogrid->getName()<<" et "<<crv->getName();
                        messErr <<"\nIl est nécessaire de fusionner ces deux courbes.";
                        throw TkUtil::Exception(messErr);
                    }
                    crv_ogrid = crv;
                }
            } // end for coedges

            if (crv_ogrid){
                if (getInfoCommand().addTopoInfoEntity(vtx,Internal::InfoCommand::DISPMODIFIED))
                    vtx->saveTopoProperty();

                //std::cout<<"updateGeomAssociationOGrid(C2V) setGeomAssociation(...) pour "<<vtx->getName()<<std::endl;
                vtx->setGeomAssociation(m_commandGeom->getAssociationC2S()[crv_ogrid]);
//                std::cout<<" Sommet "<<vtx->getName()<<" projeté sur la surface issue de la révolution de la courbe "<<crv_ogrid->getName()
//                            <<", surface : "<<(vtx->getGeomAssociation()?vtx->getGeomAssociation()->getName():"vide !")<<std::endl;
            }
            else {
//                std::cout<<" Sommet "<<vtx->getName()<<" sans courbe !"<<std::endl;
            }
        } // end if (filtre_vertex[vtx] != 11)
    } // end for vertices
}
/*----------------------------------------------------------------------------*/
Face* CommandMakeBlocksByRevol::
constructFaceWith1RevolCoFace(Edge* edge,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces)
{
    if (edge->getNbCoEdges() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructFaceWith1RevolCoFace avec une arête composée", TkUtil::Charset::UTF_8));
    CoEdge* coedge = edge->getCoEdge(0);
    std::vector<CoFace* >& revol_cofaces = coedge2cofaces[coedge];
    if (revol_cofaces.size() != 1){
    	std::cerr<<"coedge : "<<coedge->getName()<<std::endl;
    	for (uint i=0; i<revol_cofaces.size(); i++)
    		std::cerr<<*revol_cofaces[i];
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructFaceWith1RevolCoFace avec autre chose qu'une unique CoFace pour une CoEdge", TkUtil::Charset::UTF_8));
    }
    Face* newFace = new Topo::Face(getContext(), revol_cofaces[0]);
#ifdef _DEBUG_REVOL
    std::cout<<"Construction de la face "<<newFace->getName()<<std::endl;
#endif
    getInfoCommand().addTopoInfoEntity(newFace, Internal::InfoCommand::CREATED);

    return newFace;
}
/*----------------------------------------------------------------------------*/
Face* CommandMakeBlocksByRevol::
constructFaceWithRevolCoFaces(Edge* edge,
        std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
        Vertex* vtx0, Vertex* vtx1, Vertex* vtx2, Vertex* vtx3)
{
    // cas d'une Edge pointant sur plusieurs CoEdges
    std::vector<CoEdge* > coedges;
    edge->getCoEdges(coedges);
    std::vector<Vertex* > face_vertices;
    std::vector<CoFace* > face_cofaces;
    for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
            iter3 != coedges.end(); ++iter3){
        std::vector<CoFace* >& revol_cofaces = coedge2cofaces[*iter3];
        face_cofaces.insert(face_cofaces.end(), revol_cofaces.begin(), revol_cofaces.end());
    }

    face_vertices.push_back(vtx0);
    face_vertices.push_back(vtx1);
    face_vertices.push_back(vtx2);
    face_vertices.push_back(vtx3);

    Face* newFace = new Topo::Face(getContext(), face_cofaces, face_vertices, true);

#ifdef _DEBUG_REVOL
    std::cout<<"Construction de la face composée "<<newFace->getName()<<std::endl;
#endif
    getInfoCommand().addTopoInfoEntity(newFace, Internal::InfoCommand::CREATED);

    return newFace;
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::
computeFaceRatio(Face* face, uint dir, uint ni_loc_coface)
{
    uint nbI, nbJ;
    face->getNbMeshingEdges(nbI, nbJ);

#ifdef _DEBUG_REVOL
    std::cout<<"computeFaceRatio"<<std::endl;
    std::cout<<"ni_loc_coface = "<<ni_loc_coface<<std::endl;
    std::cout<<"nbI = "<<nbI<<std::endl;
    std::cout<<"nbJ = "<<nbJ<<std::endl;
    std::cout<<"dir = "<<dir<<std::endl;
#endif

    if (dir == 2 || dir == 0){
    	uint ratio = nbI/ni_loc_coface;
    	if (ratio != 1)
    		for (uint i=0; i<face->getNbCoFaces(); i++){
    			face->setRatio(face->getCoFace(i), ratio, 0);
#ifdef _DEBUG_REVOL
    			std::cout<<"   ratio pour "<<face->getCoFace(i)->getName()
    					<<" => "<<face->getRatio(face->getCoFace(i), 0)
    					<<" pour dir 0"<<std::endl;
#endif
    		}
    }
    if (dir == 2 || dir == 1){
    	uint ratio = nbJ/ni_loc_coface;
    	if (ratio != 1)
    		for (uint i=0; i<face->getNbCoFaces(); i++){
    			face->setRatio(face->getCoFace(i), ratio, 1);
#ifdef _DEBUG_REVOL
    			std::cout<<"   ratio pour "<<face->getCoFace(i)->getName()
    					<<" => "<<face->getRatio(face->getCoFace(i), 1)
    					<<" pour dir 1"<<std::endl;
#endif
    		}
    }
}
/*----------------------------------------------------------------------------*/
void CommandMakeBlocksByRevol::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
