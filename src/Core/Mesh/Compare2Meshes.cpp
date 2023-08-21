/*----------------------------------------------------------------------------*/
/*
 * \file Compare2Meshes.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15 juin 2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/Compare2Meshes.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/IGMesh.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
Compare2Meshes::Compare2Meshes(Internal::Context* context, gmds::IGMesh& mesh1, gmds::IGMesh& mesh2)
: m_context(context)
, m_gmds_mesh1(mesh1)
, m_gmds_mesh2(mesh2)
{
}
/*----------------------------------------------------------------------------*/
bool Compare2Meshes::perform()
{
    bool ok = true;

    // nombre de noeuds
    if (m_gmds_mesh1.getNbNodes() != m_gmds_mesh2.getNbNodes()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de noeuds différents "
                <<(long)m_gmds_mesh1.getNbNodes()<<" != "<<(long)m_gmds_mesh2.getNbNodes();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de bras
    if (m_gmds_mesh1.getNbEdges() != m_gmds_mesh2.getNbEdges()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de bras différents "
                <<(long)m_gmds_mesh1.getNbEdges()<<" != "<<(long)m_gmds_mesh2.getNbEdges();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de polygones (tous confondus)
    if (m_gmds_mesh1.getNbFaces() != m_gmds_mesh2.getNbFaces()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de polygones différents "
                <<(long)m_gmds_mesh1.getNbFaces()<<" != "<<(long)m_gmds_mesh2.getNbFaces();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de polyèdres (tous confondus)
    if (m_gmds_mesh1.getNbRegions() != m_gmds_mesh2.getNbRegions()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de polyèdres différents "
                <<(long)m_gmds_mesh1.getNbRegions()<<" != "<<(long)m_gmds_mesh2.getNbRegions();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de nuages (de noeuds)
    if (m_gmds_mesh1.getNbClouds() != m_gmds_mesh2.getNbClouds()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de nuages différents "
                <<(long)m_gmds_mesh1.getNbClouds()<<" != "<<(long)m_gmds_mesh2.getNbClouds();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de lignes (de bras)
    if (m_gmds_mesh1.getNbLines() != m_gmds_mesh2.getNbLines()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de lignes différents "
                <<(long)m_gmds_mesh1.getNbLines()<<" != "<<(long)m_gmds_mesh2.getNbLines();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de surfaces (de polygones)
    if (m_gmds_mesh1.getNbSurfaces() != m_gmds_mesh2.getNbSurfaces()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de surfaces différents "
                <<(long)m_gmds_mesh1.getNbSurfaces()<<" != "<<(long)m_gmds_mesh2.getNbSurfaces();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de volumes (de polyèdres)
    if (m_gmds_mesh1.getNbVolumes() != m_gmds_mesh2.getNbVolumes()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de volumes différents "
                <<(long)m_gmds_mesh1.getNbVolumes()<<" != "<<(long)m_gmds_mesh2.getNbVolumes();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // LES COORDONNEES
    // ===============
    // Comparaison des coordonnées des noeuds dans l'ordre,
    // seulement s'il y en a autant de part et d'autre
    if (m_gmds_mesh1.getNbNodes() == m_gmds_mesh2.getNbNodes()) {
        uint nb_vtx_err = 0;
        gmds::IGMesh::node_iterator itNd1 = m_gmds_mesh1.nodes_begin();
        gmds::IGMesh::node_iterator itNd2 = m_gmds_mesh2.nodes_begin();

        double errMax = 0.0;
        uint idMax = 0;
        uint i = 0;
        for (; !itNd1.isDone(); itNd1.next(), itNd2.next(), i++){
            gmds::Node nd1 = itNd1.value();
            gmds::Node nd2 = itNd2.value();

            double x1 = nd1.X();
            double x2 = nd2.X();
            double y1 = nd1.Y();
            double y2 = nd2.Y();
            double z1 = nd1.Z();
            double z2 = nd2.Z();

            double err = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));

            if (!Utils::Math::MgxNumeric::isNearlyZero(err)){
                nb_vtx_err += 1;
                if (err > errMax){
                	errMax = err;
                	idMax = i;
                }
            }
        } // end for itNd1 et itNd2

        if (nb_vtx_err){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: positions des noeuds différentes pour "
                    <<(long)nb_vtx_err<<" sur "<<(long)m_gmds_mesh1.getNbNodes();
            message <<", plus grande erreur de "<<errMax<<" pour l'indice "<<(long)idMax;
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            //std::cerr<<message<<std::endl;
            ok = false;
        }

    } // end if (m_gmds_mesh1.getNbNodes() != m_gmds_mesh2.getNbNodes())


    // LES ELEMENTS
    // ============
    // Comparaison de la topologie des polygones et des polyèdres
    // comparaison en nombre de noeuds et id de ces derniers
    // On fait l'impasse sur les bras
    if (m_gmds_mesh1.getNbFaces() == m_gmds_mesh2.getNbFaces()){
        uint nb_poly_err = 0;
        gmds::IGMesh::face_iterator itPoly1 = m_gmds_mesh1.faces_begin();
        gmds::IGMesh::face_iterator itPoly2 = m_gmds_mesh2.faces_begin();

        for (; !itPoly1.isDone(); itPoly1.next(), itPoly2.next()){
            gmds::Face p1 = itPoly1.value();
            gmds::Face p2 = itPoly2.value();

            if (p1.getNbNodes() != p2.getNbNodes()){
                nb_poly_err += 1;
            }
            else {
                std::vector<gmds::TCellID> ids1 = p1.getIDs<gmds::Node>();
                std::vector<gmds::TCellID> ids2 = p2.getIDs<gmds::Node>();
                bool okId = true;
                for (uint i=0; i<ids1.size(); i++)
                    if (ids1[i] != ids2[i])
                        okId = false;
                if (!okId)
                    nb_poly_err += 1;
            }

        } // end for itPoly1 et itPoly2

        if (nb_poly_err){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: polygones topologiquement différents: "
                    <<(long)nb_poly_err<<" sur "<<(long)m_gmds_mesh1.getNbFaces();
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            ok = false;
        }

    } // end if (m_gmds_mesh1.getNbFaces() == m_gmds_mesh2.getNbFaces())

    if (m_gmds_mesh1.getNbRegions() == m_gmds_mesh2.getNbRegions()){
        uint nb_poly_err = 0;
        gmds::IGMesh::region_iterator itPoly1 = m_gmds_mesh1.regions_begin();
        gmds::IGMesh::region_iterator itPoly2 = m_gmds_mesh2.regions_begin();

        for (; !itPoly1.isDone(); itPoly1.next(), itPoly2.next()){
            gmds::Region p1 = itPoly1.value();
            gmds::Region p2 = itPoly2.value();

            if (p1.getNbNodes() != p2.getNbNodes()){
                nb_poly_err += 1;
            }
            else {
                std::vector<gmds::TCellID> ids1 = p1.getIDs<gmds::Node>();
                std::vector<gmds::TCellID> ids2 = p2.getIDs<gmds::Node>();
                bool okId = true;
                for (uint i=0; i<ids1.size(); i++)
                    if (ids1[i] != ids2[i])
                        okId = false;
                if (!okId)
                    nb_poly_err += 1;
            }

        } // end for itPoly1 et itPoly2

        if (nb_poly_err){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: polyèdres topologiquement différents: "
                    <<(long)nb_poly_err<<" sur "<<(long)m_gmds_mesh1.getNbRegions();
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            ok = false;
        }

    } // end if (m_gmds_mesh1.getNbRegions() == m_gmds_mesh2.getNbRegions())

    // LES GROUPES
    // ===========

    // Comparaison des nuages
    // ----------------------
    {
        // comparaison des noms des groupes
        gmds::IGMesh::clouds_iterator itGrp1 = m_gmds_mesh1.clouds_begin();
        gmds::IGMesh::clouds_iterator itGrp2 = m_gmds_mesh2.clouds_begin();
        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (; itGrp1 != m_gmds_mesh1.clouds_end(); ++itGrp1)
            liste1.push_back(itGrp1->name());
        for (; itGrp2 != m_gmds_mesh2.clouds_end(); ++itGrp2)
            liste2.push_back(itGrp2->name());

        std::vector<std::string> common;
        std::vector<std::string> add1;
        std::vector<std::string> add2;

        diff(liste1, liste2, common, add1, add2);
        if (!add1.empty() || !add2.empty()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: il n'y a pas les même nuages: il y en a "
                    << (long)common.size()<< " en commun sur "
                    <<(long)liste1.size()<< " et "<<(long)liste2.size();
            if (!add1.empty()){
                message << "\n  le premier maillage contient en plus de la partie commune les nuages :";
                for (uint i=0; i<add1.size(); i++)
                    message << " "<<add1[i];
            }
            if (!add2.empty()){
                message << "\n  le deuxième maillage contient en plus de la partie commune les nuages :";
                for (uint i=0; i<add2.size(); i++)
                    message << " "<<add2[i];
            }
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            ok = false;
        }

        // comparaison du contenu des groupes en communs (id des éléments seulement, dans le même ordre)
        for (uint i=0; i<common.size(); i++){
            gmds::IGMesh::cloud& grp1 = m_gmds_mesh1.getCloud(common[i]);
            gmds::IGMesh::cloud& grp2 = m_gmds_mesh2.getCloud(common[i]);
            std::vector<gmds::Node> cells1 = grp1.cells();
            std::vector<gmds::Node> cells2 = grp2.cells();

            if (cells1.size() == cells2.size()){
                std::vector<gmds::Node>::iterator ic1 = cells1.begin();
                std::vector<gmds::Node>::iterator ic2 = cells2.begin();
                uint nb_id_diff = 0;
                for (; ic1 != cells1.end(); ++ic1, ++ic2)
                    if ((*ic1).getID() != (*ic2).getID()){
//                        std::cout<<common[i]<<": noeud id "<<(*ic1)->getID()<<" != "<<(*ic2)->getID()<<std::endl;
                        nb_id_diff += 1;
                    }

                if (nb_id_diff){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"Comparaison de 2 maillages: noeuds du nuage "<<common[i]<< " différents: "
                            <<(long)nb_id_diff<<" noeuds de différents sur "<<(long)cells1.size();
                    getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                    ok = false;
                }
            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"Comparaison de 2 maillages: tailles du nuage "<<common[i]<< " différentes "
                        <<(long)cells1.size()<<" != "<<(long)cells2.size();
                getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                ok = false;
            }

        } // end for i<common.size()

    } // fin de Comparaison des nuages

    // Comparaison des lignes
    // ----------------------
    {
        // comparaison des noms des groupes
        gmds::IGMesh::lines_iterator itGrp1 = m_gmds_mesh1.lines_begin();
        gmds::IGMesh::lines_iterator itGrp2 = m_gmds_mesh2.lines_begin();
        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (; itGrp1 != m_gmds_mesh1.lines_end(); ++itGrp1)
            liste1.push_back(itGrp1->name());
        for (; itGrp2 != m_gmds_mesh2.lines_end(); ++itGrp2)
            liste2.push_back(itGrp2->name());

        std::vector<std::string> common;
        std::vector<std::string> add1;
        std::vector<std::string> add2;

        diff(liste1, liste2, common, add1, add2);
        if (!add1.empty() || !add2.empty()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: il n'y a pas les même lignes: il y en a "
                    << (long)common.size()<< " en commun sur "
                    <<(long)liste1.size()<< " et "<<(long)liste2.size();
            if (!add1.empty()){
                message << "\n  le premier maillage contient en plus de la partie commune les lignes :";
                for (uint i=0; i<add1.size(); i++)
                    message << " "<<add1[i];
            }
            if (!add2.empty()){
                message << "\n  le deuxième maillage contient en plus de la partie commune les lignes :";
                for (uint i=0; i<add2.size(); i++)
                    message << " "<<add2[i];
            }
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            ok = false;
        }

        // comparaison du contenu des groupes en communs (id des éléments seulement, dans le même ordre)
        for (uint i=0; i<common.size(); i++){
            gmds::IGMesh::line& grp1 = m_gmds_mesh1.getLine(common[i]);
            gmds::IGMesh::line& grp2 = m_gmds_mesh2.getLine(common[i]);
            std::vector<gmds::Edge> cells1 = grp1.cells();
            std::vector<gmds::Edge> cells2 = grp2.cells();

            if (cells1.size() == cells2.size()){
                std::vector<gmds::Edge>::iterator ic1 = cells1.begin();
                std::vector<gmds::Edge>::iterator ic2 = cells2.begin();
                uint nb_id_diff = 0;
                for (; ic1 != cells1.end(); ++ic1, ++ic2)
                    if ((*ic1).getID() != (*ic2).getID())
                        nb_id_diff += 1;

                if (nb_id_diff){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"Comparaison de 2 maillages: bras de la surface "<<common[i]<< " différents: "
                            <<(long)nb_id_diff<<" bras de différents sur "<<(long)cells1.size();
                    getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                    ok = false;
                }
            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"Comparaison de 2 maillages: tailles de la ligne "<<common[i]<< " différentes "
                        <<(long)cells1.size()<<" != "<<(long)cells2.size();
                getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                ok = false;
            }

        } // end for i<common.size()

    } // fin de Comparaison des lignes


    // Comparaison des surfaces
    // ------------------------
    {
        // comparaison des noms des groupes
        gmds::IGMesh::surfaces_iterator itGrp1 = m_gmds_mesh1.surfaces_begin();
        gmds::IGMesh::surfaces_iterator itGrp2 = m_gmds_mesh2.surfaces_begin();
        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (; itGrp1 != m_gmds_mesh1.surfaces_end(); ++itGrp1)
            liste1.push_back(itGrp1->name());
        for (; itGrp2 != m_gmds_mesh2.surfaces_end(); ++itGrp2)
            liste2.push_back(itGrp2->name());

        std::vector<std::string> common;
        std::vector<std::string> add1;
        std::vector<std::string> add2;

        diff(liste1, liste2, common, add1, add2);
        if (!add1.empty() || !add2.empty()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: il n'y a pas les même surfaces: il y en a "
                    << (long)common.size()<< " en commun sur "
                    <<(long)liste1.size()<< " et "<<(long)liste2.size();
            if (!add1.empty()){
                message << "\n  le premier maillage contient en plus de la partie commune les surfaces :";
                for (uint i=0; i<add1.size(); i++)
                    message << " "<<add1[i];
            }
            if (!add2.empty()){
                message << "\n  le deuxième maillage contient en plus de la partie commune les surfaces :";
                for (uint i=0; i<add2.size(); i++)
                    message << " "<<add2[i];
            }
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            ok = false;
        }

        // comparaison du contenu des groupes en communs (id des éléments seulement, dans le même ordre)
        for (uint i=0; i<common.size(); i++){
            gmds::IGMesh::surface& grp1 = m_gmds_mesh1.getSurface(common[i]);
            gmds::IGMesh::surface& grp2 = m_gmds_mesh2.getSurface(common[i]);
            std::vector<gmds::Face> cells1 = grp1.cells();
            std::vector<gmds::Face> cells2 = grp2.cells();

            if (cells1.size() == cells2.size()){
                std::vector<gmds::Face>::iterator ic1 = cells1.begin();
                std::vector<gmds::Face>::iterator ic2 = cells2.begin();
                uint nb_id_diff = 0;
                for (; ic1 != cells1.end(); ++ic1, ++ic2)
                    if ((*ic1).getID() != (*ic2).getID())
                        nb_id_diff += 1;

                if (nb_id_diff){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"Comparaison de 2 maillages: polygones de la surface "<<common[i]<< " différents: "
                            <<(long)nb_id_diff<<" polygones de différents sur "<<(long)cells1.size();
                    getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                    ok = false;
                }
            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"Comparaison de 2 maillages: tailles de la surface "<<common[i]<< " différentes "
                        <<(long)cells1.size()<<" != "<<(long)cells2.size();
                getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                ok = false;
            }

        } // end for i<common.size()

    } // fin de Comparaison des surfaces


    // Comparaison des volumes
    // -----------------------
    {
        // comparaison des noms des groupes
        gmds::IGMesh::volumes_iterator itGrp1 = m_gmds_mesh1.volumes_begin();
        gmds::IGMesh::volumes_iterator itGrp2 = m_gmds_mesh2.volumes_begin();
        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (; itGrp1 != m_gmds_mesh1.volumes_end(); ++itGrp1)
            liste1.push_back(itGrp1->name());
        for (; itGrp2 != m_gmds_mesh2.volumes_end(); ++itGrp2)
            liste2.push_back(itGrp2->name());

        std::vector<std::string> common;
        std::vector<std::string> add1;
        std::vector<std::string> add2;

        diff(liste1, liste2, common, add1, add2);
        if (!add1.empty() || !add2.empty()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Comparaison de 2 maillages: il n'y a pas les même volumes: il y en a "
                    << (long)common.size()<< " en commun sur "
                    <<(long)liste1.size()<< " et "<<(long)liste2.size();
            if (!add1.empty()){
                message << "\n  le premier maillage contient en plus de la partie commune les volumes :";
                for (uint i=0; i<add1.size(); i++)
                    message << " "<<add1[i];
            }
            if (!add2.empty()){
                message << "\n  le deuxième maillage contient en plus de la partie commune les volumes :";
                for (uint i=0; i<add2.size(); i++)
                    message << " "<<add2[i];
            }
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
            ok = false;
        }

        // comparaison du contenu des groupes en communs (id des éléments seulement, dans le même ordre)
        for (uint i=0; i<common.size(); i++){
            gmds::IGMesh::volume& grp1 = m_gmds_mesh1.getVolume(common[i]);
            gmds::IGMesh::volume& grp2 = m_gmds_mesh2.getVolume(common[i]);
            std::vector<gmds::Region> cells1 = grp1.cells();
            std::vector<gmds::Region> cells2 = grp2.cells();

            if (cells1.size() == cells2.size()){
                std::vector<gmds::Region>::iterator ic1 = cells1.begin();
                std::vector<gmds::Region>::iterator ic2 = cells2.begin();
                uint nb_id_diff = 0;
                for (; ic1 != cells1.end(); ++ic1, ++ic2)
                    if ((*ic1).getID() != (*ic2).getID())
                        nb_id_diff += 1;

                if (nb_id_diff){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"Comparaison de 2 maillages: polyèdres du volume "<<common[i]<< " différents: "
                            <<(long)nb_id_diff<<" polyèdres de différents sur "<<(long)cells1.size();
                    getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                    ok = false;
                }
            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"Comparaison de 2 maillages: tailles du volume "<<common[i]<< " différentes "
                        <<(long)cells1.size()<<" != "<<(long)cells2.size();
                getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
                ok = false;
            }

        } // end for i<common.size()

    } // fin de Comparaison des surfaces


    return ok;
}
/*----------------------------------------------------------------------------*/
void Compare2Meshes::
diff(std::vector<std::string> &liste1,
        std::vector<std::string> &liste2,
        std::vector<std::string> &common,
        std::vector<std::string> &add1,
        std::vector<std::string> &add2)
{
    // on va marqué à 1 ceux qui sont dans la première liste
    // on ajoute 2 à ceux qui sont dans la deuxième liste
    // on tri ensuite, sont marqués à 3 ceux qui sont dans les 2 listes
    std::map<std::string, uint> filtre_noms;

    for (std::vector<std::string>::iterator iter = liste1.begin();
            iter != liste1.end(); ++iter)
        filtre_noms[*iter] = 1;

    for (std::vector<std::string>::iterator iter = liste2.begin();
            iter != liste2.end(); ++iter)
        filtre_noms[*iter] = filtre_noms[*iter] + 2;

    for (std::map<std::string, uint>::iterator iter = filtre_noms.begin();
            iter != filtre_noms.end(); ++iter)
        if (iter->second == 1)
            add1.push_back(iter->first);
        else if (iter->second == 2)
            add2.push_back(iter->first);
        else if (iter->second == 3)
                    common.push_back(iter->first);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
