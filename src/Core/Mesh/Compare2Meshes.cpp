/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/Compare2Meshes.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
Compare2Meshes::Compare2Meshes(Internal::Context* context, gmds::Mesh& mesh1, gmds::Mesh& mesh2)
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
    if (m_gmds_mesh1.getNbGroups<gmds::Node>() != m_gmds_mesh2.getNbGroups<gmds::Node>()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de nuages différents "
                <<(long)m_gmds_mesh1.getNbGroups<gmds::Node>()<<" != "<<(long)m_gmds_mesh2.getNbGroups<gmds::Node>();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de lignes (de bras)
    if (m_gmds_mesh1.getNbGroups<gmds::Edge>() != m_gmds_mesh2.getNbGroups<gmds::Edge>()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de lignes différents "
                <<(long)m_gmds_mesh1.getNbGroups<gmds::Edge>()<<" != "<<(long)m_gmds_mesh2.getNbGroups<gmds::Edge>();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de surfaces (de polygones)
    if (m_gmds_mesh1.getNbGroups<gmds::Face>() != m_gmds_mesh2.getNbGroups<gmds::Face>()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de surfaces différents "
                <<(long)m_gmds_mesh1.getNbGroups<gmds::Face>()<<" != "<<(long)m_gmds_mesh2.getNbGroups<gmds::Face>();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // nombre de volumes (de polyèdres)
    if (m_gmds_mesh1.getNbGroups<gmds::Region>() != m_gmds_mesh2.getNbGroups<gmds::Region>()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Comparaison de 2 maillages: nombre de volumes différents "
                <<(long)m_gmds_mesh1.getNbGroups<gmds::Region>()<<" != "<<(long)m_gmds_mesh2.getNbGroups<gmds::Region>();
        getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));
        ok = false;
    }

    // LES COORDONNEES
    // ===============
    // Comparaison des coordonnées des noeuds dans l'ordre,
    // seulement s'il y en a autant de part et d'autre
    if (m_gmds_mesh1.getNbNodes() == m_gmds_mesh2.getNbNodes()) {
        uint nb_vtx_err = 0;
        double errMax = 0.0;
        uint idMax = 0;
        uint i = 0;

        for (auto itNd1 = m_gmds_mesh1.nodes_begin(), itNd2 = m_gmds_mesh2.nodes_begin();
                itNd1 != m_gmds_mesh1.nodes_end(), itNd2 != m_gmds_mesh2.nodes_end();
                ++itNd1, ++itNd2){

            gmds::Node nd1 = m_gmds_mesh1.get<gmds::Node>(*itNd1);
            gmds::Node nd2 = m_gmds_mesh2.get<gmds::Node>(*itNd2);

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

        for (auto itPoly1 = m_gmds_mesh1.faces_begin(), itPoly2 = m_gmds_mesh2.faces_begin();
                itPoly1 != m_gmds_mesh1.faces_end(), itPoly2 != m_gmds_mesh2.faces_end();
                ++itPoly1, ++itPoly2){

            gmds::Face p1 = m_gmds_mesh1.get<gmds::Face>(*itPoly1);
            gmds::Face p2 = m_gmds_mesh2.get<gmds::Face>(*itPoly2);

            if (p1.nbNodes() != p2.nbNodes()){
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

        for (auto itPoly1 = m_gmds_mesh1.regions_begin(), itPoly2 = m_gmds_mesh2.regions_begin();
                itPoly1 != m_gmds_mesh1.regions_end(), itPoly2 != m_gmds_mesh2.regions_end();
                ++itPoly1, ++itPoly2){

            gmds::Region p1 = m_gmds_mesh1.get<gmds::Region>(*itPoly1);
            gmds::Region p2 = m_gmds_mesh2.get<gmds::Region>(*itPoly2);

            if (p1.nbNodes() != p2.nbNodes()){
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
        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (auto itGrp1 = m_gmds_mesh1.groups_begin<gmds::Node>(); itGrp1 != m_gmds_mesh1.groups_end<gmds::Node>(); ++itGrp1)
            liste1.push_back((*itGrp1)->name());

        for (auto itGrp2 = m_gmds_mesh2.groups_begin<gmds::Node>(); itGrp2 != m_gmds_mesh2.groups_end<gmds::Node>(); ++itGrp2)
            liste2.push_back((*itGrp2)->name());

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
            auto grp1 = m_gmds_mesh1.getGroup<gmds::Node>(common[i]);
            auto grp2 = m_gmds_mesh2.getGroup<gmds::Node>(common[i]);
            auto cells1 = grp1->cells();
            auto cells2 = grp2->cells();

            if (cells1.size() == cells2.size()){
                uint nb_id_diff = 0;
                for (auto ic1 = cells1.begin(), ic2 = cells2.begin(); ic1 != cells1.end(), ic2 != cells2.end(); ++ic1, ++ic2) {
                    auto cell1 = m_gmds_mesh1.get<gmds::Node>(*ic1);
                    auto cell2 = m_gmds_mesh2.get<gmds::Node>(*ic2);
                    if (cell1.id() != cell2.id()){
//                        std::cout<<common[i]<<": noeud id "<<(*ic1)->getID()<<" != "<<(*ic2)->getID()<<std::endl;
                        nb_id_diff += 1;
                    }
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
        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (auto itGrp1 = m_gmds_mesh1.groups_begin<gmds::Edge>(); itGrp1 != m_gmds_mesh1.groups_end<gmds::Edge>(); ++itGrp1){
            gmds::CellGroup<gmds::Edge> *current_group = *itGrp1;
            liste1.push_back(current_group->name());
        }
        for (auto itGrp2 = m_gmds_mesh2.groups_begin<gmds::Edge>(); itGrp2 != m_gmds_mesh2.groups_end<gmds::Edge>(); ++itGrp2){
            gmds::CellGroup<gmds::Edge> *current_group = *itGrp2;
            liste2.push_back(current_group->name());
        }
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
            auto grp1 = m_gmds_mesh1.getGroup<gmds::Edge>(common[i]);
            auto grp2 = m_gmds_mesh2.getGroup<gmds::Edge>(common[i]);
            auto cells1 = grp1->cells();
            auto cells2 = grp2->cells();

            if (cells1.size() == cells2.size()){
                uint nb_id_diff = 0;
                for (auto ic1 = cells1.begin(), ic2 = cells2.begin(); ic1 != cells1.end(), ic2 != cells2.end(); ++ic1, ++ic2) {
                    auto cell1 = m_gmds_mesh1.get<gmds::Node>(*ic1);
                    auto cell2 = m_gmds_mesh2.get<gmds::Node>(*ic2);
                    if (cell1.id() != cell2.id())
                        nb_id_diff += 1;
                }
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

        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (auto itGrp1 = m_gmds_mesh1.groups_begin<gmds::Face>(); itGrp1 != m_gmds_mesh1.groups_end<gmds::Face>(); ++itGrp1)
            liste1.push_back((*itGrp1)->name());
        for (auto itGrp2 = m_gmds_mesh2.groups_begin<gmds::Face>(); itGrp2 != m_gmds_mesh2.groups_end<gmds::Face>(); ++itGrp2)
            liste2.push_back((*itGrp2)->name());

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
            auto grp1 = m_gmds_mesh1.getGroup<gmds::Face>(common[i]);
            auto grp2 = m_gmds_mesh2.getGroup<gmds::Face>(common[i]);
            auto cells1 = grp1->cells();
            auto cells2 = grp2->cells();

            if (cells1.size() == cells2.size()){
                uint nb_id_diff = 0;
                for (auto ic1 = cells1.begin(), ic2 = cells2.begin(); ic1 != cells1.end(), ic2 != cells2.end(); ++ic1, ++ic2) {
                    auto cell1 = m_gmds_mesh1.get<gmds::Node>(*ic1);
                    auto cell2 = m_gmds_mesh2.get<gmds::Node>(*ic2);
                    if (cell1.id() != cell2.id())
                        nb_id_diff += 1;
                }

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

        std::vector<std::string> liste1;
        std::vector<std::string> liste2;
        for (auto itGrp1 = m_gmds_mesh1.groups_begin<gmds::Region>(); itGrp1 != m_gmds_mesh1.groups_end<gmds::Region>(); ++itGrp1)
            liste1.push_back((*itGrp1)->name());
        for (auto itGrp2 = m_gmds_mesh2.groups_begin<gmds::Region>(); itGrp2 != m_gmds_mesh2.groups_end<gmds::Region>(); ++itGrp2)
            liste2.push_back((*itGrp2)->name());

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
            auto grp1 = m_gmds_mesh1.getGroup<gmds::Region>(common[i]);
            auto grp2 = m_gmds_mesh2.getGroup<gmds::Region>(common[i]);
            auto cells1 = grp1->cells();
            auto cells2 = grp2->cells();

            if (cells1.size() == cells2.size()){
                uint nb_id_diff = 0;
                for (auto ic1 = cells1.begin(), ic2 = cells2.begin(); ic1 != cells1.end(), ic2 != cells2.end(); ++ic1, ++ic2){
                    auto cell1 = m_gmds_mesh1.get<gmds::Node>(*ic1);
                    auto cell2 = m_gmds_mesh2.get<gmds::Node>(*ic2);
                    if (cell1.id() != cell2.id())
                        nb_id_diff += 1;
                }

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
