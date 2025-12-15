/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/ImportMDLImplementation.h"
#include "Internal/InfoCommand.h"
#include "Geom/GeomManager.h"
#include "Geom/EntityFactory.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyBigeometric.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/TopoHelper.h"
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <sstream>
#include <iostream>
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MDL
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
ImportMDLImplementation::
ImportMDLImplementation(Internal::Context& c,
        const std::string& n,
        const bool all,
		const bool useAreaName,
		int deg_min, int deg_max)
: ImportMDLItf(c, n) , m_importAll(all), m_useAreaName(useAreaName), m_icmd(0)
, m_deg_min(deg_min)
, m_deg_max(deg_max)
{}
/*----------------------------------------------------------------------------*/
ImportMDLImplementation::
ImportMDLImplementation(Internal::Context& c,
        const std::string& n,
        std::string groupe)
: ImportMDLItf(c, n) , m_importAll(false), m_useAreaName(false), m_icmd(0), m_groupe(groupe)
, m_deg_min(1)
, m_deg_max(2)
{}
/*----------------------------------------------------------------------------*/
ImportMDLImplementation::
ImportMDLImplementation(Internal::Context& c,
        const std::string& n,
        std::vector<std::string>& zones)
: ImportMDLItf(c, n) , m_importAll(false), m_useAreaName(false), m_icmd(0), m_zones(zones)
, m_deg_min(1)
, m_deg_max(2)
{}
/*----------------------------------------------------------------------------*/
ImportMDLImplementation::~ImportMDLImplementation()
{
}
/*----------------------------------------------------------------------------*/
void ImportMDLImplementation::performGeom(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_MDL
    std::cout << "ImportMDLImplementation::performGeom() en cours ..."<<std::endl;
#endif

    if (icmd == 0)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne: InfoCommand non initialisé", TkUtil::Charset::UTF_8));

    m_icmd = icmd;

    readMDL();


    if (m_do_scale){
    	//m_scale_factor = pow(10.0, m_mdl_info->length_unit);
#ifdef _DEBUG_MDL
    	std::cout<<"Mise à l'échelle avec length_unit = "<<m_mdl_info->length_unit<<", m_scale_factor = "<<m_scale_factor<<std::endl;
#endif
    }


    for(uint i=0; i<m_mdl_info->nb_commands; ++i){
        T_MdlCommand& current_command = m_mdl_info->commands[i];
        switch (current_command.type){
        // quoi qu'en dise Coverity, la suite est correcte
        case MdlPoint:   m_vCmdPt.push_back(&current_command); break;
        case MdlArea:    m_vCmdArea.push_back(&current_command); break;
        case MdlLineSup: m_vCmdSup.push_back(&current_command);
        default:         m_cor_name_mdlCmd[current_command.name.str()] = &current_command;
        }
    }


    if (m_importAll){
        // boucle sur tous les points,
        // les points sont demandés donc créés
        for (uint ptId = 0; ptId<m_vCmdPt.size(); ptId++){
            Geom::Vertex* v = getVertex(ptId);
        }
    }


    // cas du filtrage des zones suivant une liste déterminée ou un groupe donné
    std::set<std::string> filter; // les noms des zones acceptées
    bool isFiltered = getFilter(filter);

    // les surfaces
    Group::GroupManager& gm = m_context.getGroupManager();
    for (std::vector<const T_MdlCommand*>::const_iterator iter = m_vCmdArea.begin(); iter!=m_vCmdArea.end(); ++iter){
        const T_MdlCommand& current_command = **iter;

        // cas d'une zone qui n'est pas demandée
        if (isFiltered && filter.find(current_command.name.str()) == filter.end())
        	continue;

        std::vector<Geom::Curve*> curves;
        uint nb_cut_data = current_command.u.area.nb_cut_data;
        for( uint j=0; j<nb_cut_data; j++ ){
            T_MdlCutData &mcd = current_command.u.area.data[j];
            Geom::Curve* crv = getCurve(mcd.name.str());
            curves.push_back(crv);

            if (!mcd.groups_name.nil()){
                std::istringstream isgroups(TkUtil::UTF8String(mcd.groups_name.str()).trim());
                uint id_grp=0;
                while (!isgroups.eof()){
                    std::string name;
                    isgroups>>name;
                    if (!name.empty()){
                    	id_grp++;
                    	if (!m_prefixName.empty())
                    		name = m_prefixName + name;
                        Group::Group1D* group = gm.getNewGroup<Group::Group1D>(name, icmd);
                        if (id_grp == 1)
                        	group->setLevel(2);
                        else
                        	group->setLevel(3);
                        if (!group->find(crv)){
                			// il faut peut-être enlever le groupe par défaut
                            std::vector<Group::GroupEntity*> crv_groups = gm.getGroupsFor(crv);
                			if (crv_groups.size() == 1){
                                Group::GroupEntity* grp = crv_groups[0];
                				if (grp->getName() == gm.getDefaultName(1)){
                                    gm.removeGroupFor(crv, grp);
                					grp->remove(crv);
                					icmd->addGroupInfoEntity(grp, Internal::InfoCommand::DISPMODIFIED);
                				}
                			}

                            gm.addGroupFor(crv, group);
                            group->add(crv);
                       }
                    }
                }
            }

        } // end for j<nb_cut_data

#ifdef _DEBUG_MDL
        std::cout << "Création de la Surface "<<current_command.name<<std::endl;
#endif

        Geom::Surface* surf = Geom::EntityFactory(m_context).newSurface(curves);
#ifdef _DEBUG_MDL
        std::cout << "  avec comme nom "<<surf->getName()<<std::endl;
        std::cout << "  groups_name = \""<<current_command.u.area.groups_name<<"\""<<std::endl;
#endif
        m_newGeomEntities.push_back(surf);
        m_cor_name_surface[current_command.name.str()] = surf;

        //creation des connections topologiques surf <-> curv
        for(unsigned int i=0;i<curves.size();i++)
        {
            surf->add(curves[i]);
            curves[i]->add(surf);
        }

        if (current_command.u.area.groups_name
                && !std::string(current_command.u.area.groups_name).empty()){
        	uint id_grp=0;
            std::istringstream isgroups(TkUtil::UTF8String(current_command.u.area.groups_name).trim());
            std::string name;
            while (!isgroups.eof()){
                isgroups>>name;
                if (!name.empty()){
#ifdef _DEBUG_MDL
                    std::cout << "groups_name => \""<<name<<"\"\n";
#endif
                    id_grp++;
                	if (!m_prefixName.empty())
                		name = m_prefixName + name;
                    Group::Group2D* group = gm.getNewGroup<Group::Group2D>(name, icmd);
                    if (id_grp == 1)
                    	group->setLevel(2);
                    else
                    	group->setLevel(3);
                    gm.addGroupFor(surf, group);
                    group->add(surf);
                }
            }
        }

        if (m_useAreaName || current_command.u.area.groups_name == 0 || std::string(current_command.u.area.groups_name).empty()) {
#ifdef _DEBUG_MDL
            std::cout << "utilisation du nom de la zone\n";
#endif
            std::string nomDef = current_command.name.str();
        	if (!m_prefixName.empty())
        		nomDef = m_prefixName + nomDef;
            Group::Group2D* group = gm.getNewGroup<Group::Group2D>(nomDef, icmd);
            group->setLevel(1);
            gm.addGroupFor(surf, group);
            group->add(surf);
        }


    } // end for iter = m_vCmdArea.begin()

    //Ajout des groupes 0D
    for (uint ptId = 0; ptId<m_vCmdPt.size(); ptId++){
        T_MdlCommand& current_command = *m_vCmdPt[ptId];

        if(current_command.u.point.groups_name && !std::string(current_command.u.point.groups_name).empty()){
            double x = current_command.u.point.cd.x1;
            double y = current_command.u.point.cd.x2;

            for(auto ge : m_newGeomEntities){
                if(ge->getDim() == 0){
                    Geom::Vertex* v = dynamic_cast<Geom::Vertex*>(ge);

                    if(v->getX() == x && v->getY() == y){
                        uint id_grp=0;
                        std::istringstream isgroups(TkUtil::UTF8String(current_command.u.point.groups_name).trim());
                        std::string name;
                        while (!isgroups.eof()){
                            isgroups>>name;
                            if (!name.empty()){
                                id_grp++;
                                if (!m_prefixName.empty())
                                    name = m_prefixName + name;
                                Group::Group0D* group = gm.getNewGroup<Group::Group0D>(name, icmd);
                                if (id_grp == 1)
                                    group->setLevel(2);
                                else
                                    group->setLevel(3);
                                gm.addGroupFor(v, group);
                                group->add(v);
                            }
                        }
                    }
                }
            }
        }
    }

    if (m_importAll){
        // boucle sur toutes les commandes de contour,
        // les contours sont demandés donc créés
        for(std::map<std::string, T_MdlCommand*>::iterator iter = m_cor_name_mdlCmd.begin();
                iter != m_cor_name_mdlCmd.end(); ++iter){
            Geom::Curve* crv = getCurve(iter->first);
        }

        // recherche des supports non utilisés (par une ligne)
        for (uint indSup = 0; indSup<m_vCmdSup.size(); indSup++) {
            if (m_filtre_support[m_vCmdSup[indSup]->name.str()] == 0)
                getCurve(m_vCmdSup[indSup]->name.str());
        }
    }

    for(int i=0;i<m_newGeomEntities.size();i++){
        Geom::GeomEntity* ge = m_newGeomEntities[i];
        icmd->addGeomInfoEntity (ge, Internal::InfoCommand::CREATED);
        m_context.getGeomManager().addEntity(ge);
    }
}
/*----------------------------------------------------------------------------*/
void ImportMDLImplementation::performTopo(Internal::InfoCommand* icmd)
{
    if (icmd == 0)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne: InfoCommand non initialisé", TkUtil::Charset::UTF_8));

    m_icmd = icmd;

#ifdef _DEBUG_MDL
    std::cout << "ImportMDLImplementation::performTopo() en cours ..."<<std::endl;
#endif

    readMDL();

    // pour retrouver la description mdl d'un contour en fonction du nom du contour
    std::map <std::string, T_MdlCutData*> cor_model1d_MdlCutData;

    for (std::vector<const T_MdlCommand*>::const_iterator iter = m_vCmdArea.begin(); iter!=m_vCmdArea.end(); ++iter){
        const T_MdlCommand& current_command = **iter;
        uint nb_cut_data = current_command.u.area.nb_cut_data;
        for( uint j=0; j<nb_cut_data; j++ ){
            T_MdlCutData &mcd = current_command.u.area.data[j];

            if (mcd.type!=MdlCutPreCut){
                cor_model1d_MdlCutData[std::string(mcd.name.str())] = &mcd;
            }
        } // end for j<nb_cut_data
    } // end for iter = m_vCmdArea.begin()

    // pour prendre les precut lorsqu'il n'y a pas de zone voisine (pris comme uniforme)
    // et pour repérer les cas non-conformes
#ifdef _DEBUG_MDL
    std::cout<<"# première boucle sur les zones, construction de contours"<<std::endl;
#endif
    for (std::vector<const T_MdlCommand*>::const_iterator iter = m_vCmdArea.begin(); iter!=m_vCmdArea.end(); ++iter){
        const T_MdlCommand& current_command = **iter;
        uint nb_cut_data = current_command.u.area.nb_cut_data;
        for( uint j=0; j<nb_cut_data; j++ ){
            T_MdlCutData &mcd = current_command.u.area.data[j];
            T_MdlCutData *mcdv = cor_model1d_MdlCutData[std::string(mcd.name.str())];
            bool nonConforme = false;

            // on passe le cas où le MdlCommand référencé est celui en cours d'observation
            if (&mcd == mcdv)
                continue;

            // cas avec PreCut sans voisin
            if (mcdv == 0){
                if (mcd.type==MdlCutPreCut){
                    cor_model1d_MdlCutData[std::string(mcd.name.str())] = &mcd;
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Dans la zone "<<current_command.name.str()<<" le contour " << mcd.name.str()
                            <<" est en pré-Découpé, mais n'a pas de voisin ! (On continue avec un découpage uniforme)";
                    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
                }
            }
            else { // cas avec un voisin connu
                if (mcd.type!=MdlCutPreCut) {
                    // cas non-conforme sans Precut
                    nonConforme = true;
                } else { // cas avec PreCut
                    // on se limite aux cas avec precut moins discrétisé
                    if (mcd.nb_mesh > mcdv->nb_mesh) {
						TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                        messErr << "Erreur, dans la zone "<<current_command.name.str()
                                <<" pour le contour "<<mcd.name.str()
                                <<", le prédécoupage est plus fin ("<<(short)mcd.nb_mesh
                                <<") que le découpage de référence ("<<(short)mcdv->nb_mesh<<")";
                        throw TkUtil::Exception(messErr);
                    }
                    uint ratio = mcdv->nb_mesh/mcd.nb_mesh;
                    uint reste = mcdv->nb_mesh - ratio*mcd.nb_mesh;

                    if (reste){
                        // cela serait accepté à condition de na pas mettre PreCut
						TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                        messErr << "Erreur, dans la zone "<<current_command.name.str()
                                <<" pour le contour "<<mcd.name.str()
                                <<", le prédécoupage est anormallement non-conforme ("<<(short)mcd.nb_mesh
                                <<" et "<<(short)mcdv->nb_mesh<<")";
                        throw TkUtil::Exception(messErr);
                        // nonConforme = true;
                    }
                    else if (ratio > 1) {
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                        message << "Dans la zone "<<current_command.name.str()<<" le contour " << mcd.name.str()
                                <<" est détecté comme étant semi-conforme";
                        m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
                    }
                    // sinon c'est conforme

                } // end cas avec PreCut

                if (nonConforme){
                    cor_model1d_MdlCutData[std::string(mcd.name.str())] = 0;
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Dans la zone "<<current_command.name.str()<<" le contour " << mcd.name.str()
                            <<" est détecté comme étant non-conforme";
                    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
                }

            } // end cas avec un voisin connu
        } // end for j<nb_cut_data
    } // end for iter = m_vCmdArea.begin()

    // boucle sur les zones pour la création des Edges et des CoFaces
#ifdef _DEBUG_MDL
    std::cout<<"# deuxième boucle sur les zones, création des Edges et des CoFaces"<<std::endl;
#endif
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<const T_MdlCommand*>::const_iterator iter = m_vCmdArea.begin(); iter!=m_vCmdArea.end(); ++iter){
        const T_MdlCommand& current_command = **iter;

#ifdef _DEBUG_MDL
        std::cout << "zone : "<< current_command.name << std::endl;
#endif

        // TODO [EB] tenir compte de la demande de lissage ; current_command.u.area.algo_smooth==0

        // identification de la structuration à partir du nom de la méthode de maillage
        std::string meshAlgo(current_command.u.area.algo_mesh);
        bool isStructured = (meshAlgo == "Takmiu1" || meshAlgo == "Takeo" || meshAlgo == "InterpolationTransfinie");
        // si c'est non structuré, on ne tient pas compte des id pour les côtés logiques

        uint nb_cut_data = current_command.u.area.nb_cut_data;
        // on détermine la répartition des contours (nb de model1d / côté logique)
        uint nb_cotes_logiques = current_command.u.area.data[nb_cut_data-1].logical_id+1;
        if (!isStructured)
        	nb_cotes_logiques = nb_cut_data;
        uint* nb_model1d = new uint[nb_cotes_logiques]; // répartition par côté logique
        uint* nb_mesh_cote = new uint[nb_cotes_logiques]; // nombre de bras par côté logique
        for ( uint j=0; j<nb_cotes_logiques; j++ ){
            nb_model1d[j] = 0;
            nb_mesh_cote[j] = 0;
        }
        for( uint j=0; j<nb_cut_data; j++ ){
            T_MdlCutData &mcd = current_command.u.area.data[j];
            if (isStructured){
            	nb_model1d[mcd.logical_id]+=1;
            	nb_mesh_cote[mcd.logical_id]+=mcd.nb_mesh;
            }
            else {
            	nb_model1d[j]+=1;
            	nb_mesh_cote[j]+=mcd.nb_mesh;
            }
#ifdef _DEBUG_MDL
            std::cout << " model1d : " << mcd.name
                    << ", id: " << mcd.id
                    << ", logical_id: " << mcd.logical_id
                    << ", nb_mesh: " << mcd.nb_mesh
                    << ", is_inverse: " << mcd.is_inverse
                    << std::endl;
#endif
        }

        // liste d'arêtes avec une interpolation qui doit référencer cette face topo
        std::vector<Topo::CoEdge* > coedges_interpolated;

        // parcours par côté logique
        uint ind_data = 0;
        std::vector<Topo::Edge* > edges;
        for ( uint j=0; j<nb_cotes_logiques; j++ ){
#ifdef _DEBUG_MDL
        	std::cout << "+ côté logique j = "<< j << " nb mesh "<<nb_mesh_cote[j]<< std::endl;
#endif
        	bool isConforme = true;
        	std::vector<Topo::CoEdge* > coedges;
        	// ratios pour la semi-conformité
        	std::vector<uint> ratios;
        	uint nb_mesh_cote_j = 0;
        	for ( uint k=0; k<nb_model1d[j]; k++) {
        		T_MdlCutData &mcd = current_command.u.area.data[ind_data++];

        		Geom::Curve*  contour = m_cor_name_curve[mcd.name.str()];
        		// le voisin si nécesssaire pour la description
        		T_MdlCutData *mcdv = cor_model1d_MdlCutData[std::string(mcd.name.str())];
        		if (mcdv == 0){
        			// cas non-conforme
        			mcdv = &mcd;
        			isConforme = false;
        		}

        		// construction des arêtes communes
        		Topo::CoEdge* coedge = getCoEdge(mcdv, isConforme);
        		coedges.push_back(coedge);

        		if (mcd.type == MdlCutInterpol)
        			coedges_interpolated.push_back(coedge);

        		// cas semi-conforme ?
        		if (isConforme && mcd.nb_mesh < mcdv->nb_mesh){
        			ratios.push_back(mcdv->nb_mesh/mcd.nb_mesh);
        		}
        		else
        			ratios.push_back(1);

        		nb_mesh_cote_j += mcd.nb_mesh;

        	} // end for k<nb_model1d[j]

        	// les 2 sommets pour ce côté logique
        	Topo::Vertex* vtx1 = 0;
        	Topo::Vertex* vtx2 = 0;
        	Topo::TopoHelper::getVerticesTip(coedges, vtx1, vtx2, true);

        	// construction de l'arête associée à un côté logique
        	Topo::Edge* edge = new Topo::Edge(m_context, vtx1, vtx2, coedges);
        	m_newTopoEntities.push_back(edge);
        	edges.push_back(edge);

        	// prise en compte de la semi-conformité
        	for (uint i=0; i<coedges.size(); i++){
        		if (ratios[i]>1)
        			edge->setRatio(coedges[i], ratios[i]);
        	}

        } // end for j<nb_cotes_logiques

        // construction de la CoFace
#ifdef _DEBUG_MDL
        std::cout<<"Construction de la face commune avec l'algo de maillage "<<current_command.u.area.algo_mesh<<std::endl;
#endif

        Topo::CoFace* coface;
        if (isStructured){
            // cas d'une zone structurée pour Magix (2d)
            if (edges.size() == 3)
                coface = new Topo::CoFace(m_context, edges[0], edges[1], edges[2]);
            else if (edges.size() == 4)
                coface = new Topo::CoFace(m_context, edges[0], edges[1], edges[2], edges[3]);
            else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur, dans la zone "<<current_command.name.str()
                        <<", on n'est pas dans le cas à 3 ou 4 côtés logiques ("<<(short)edges.size()
                        <<")";
                throw TkUtil::Exception(messErr);
            }
            // sélection de la méthode transfinie ou directionnelle si possible
            m_icmd->addTopoInfoEntity (coface, Internal::InfoCommand::CREATED);

        } else {
            // cas non-structuré
            coface = new Topo::CoFace(m_context, edges);
        }
        m_newTopoEntities.push_back(coface);
        cofaces.push_back(coface);

        coface->setGeomAssociation(m_cor_name_surface[current_command.name.str()]);

        // mise à jour des interpolations / coface
        MAJInterpolated(coedges_interpolated, coface);

//        if (current_command.u.area.groups_name){
//            std::istringstream isgroups(current_command.u.area.groups_name);
//            std::string name;
//            std::string last_name;
//            while (!isgroups.eof()){
//                isgroups>>name;
//                if (!name.empty() && last_name!=name){
//#ifdef _DEBUG_MDL
//                    std::cout << "groups_name =>\""<<name<<"\"\n";
//#endif
//                    coface->groupsName().push_back(name);
//                    last_name = name;
//                }
//            }
//        }

#ifdef _DEBUG_MDL
        std::cout << "Face commune obtenue: "<<*coface;
#endif

        delete [] nb_model1d;
        delete [] nb_mesh_cote;
    } // end for iter = m_vCmdArea.begin()

    // séparation des CoFaces (duplique les sommets) pour être réellement non-conforme
    // et cela pour permettre le glissement.
    dupVertexNonConformalRelation(cofaces);

    // enregistrement des nouvelles entités topologiques
    for(std::vector<Topo::TopoEntity*>::iterator iter = m_newTopoEntities.begin();
            iter != m_newTopoEntities.end(); ++iter){
        Topo::TopoEntity* te = *iter;
        m_icmd->addTopoInfoEntity (te, Internal::InfoCommand::CREATED);
    }

}
/*----------------------------------------------------------------------------*/
void ImportMDLImplementation::
MAJInterpolated(std::vector<Topo::CoEdge*>& coedges, Topo::CoFace* coface)
{
	std::string nom = coface->getName();
	for (uint i=0; i<coedges.size(); i++){
		if (coface->isStructured()){
			Topo::EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<Topo::EdgeMeshingPropertyInterpolate*>(coedges[i]->getMeshingProperty());
			CHECK_NULL_PTR_ERROR(interpol);
			interpol->setCoFace(nom);
		}
		else {
			// on remplace l'interpolation par de l'uniforme pour le cas non-structuré
			Topo::CoEdgeMeshingProperty* old_ppty = coedges[i]->setProperty(new Topo::EdgeMeshingPropertyUniform(coedges[i]->getNbMeshingEdges()));
			delete old_ppty;
		}
	}
}
/*----------------------------------------------------------------------------*/
Geom::Vertex* ImportMDLImplementation::getVertex(uint ptId)
{
    Geom::Vertex* vtx = m_cor_ptId_vertex[ptId];
    Group::GroupManager& gm = m_context.getGroupManager();

    if (!vtx){
        const T_MdlCommand& command_pt = *m_vCmdPt[ptId];
        vtx = Geom::EntityFactory(m_context).newVertex(Utils::Math::Point(command_pt.u.point.cd.x1,command_pt.u.point.cd.x2, 0.0)*m_scale_factor);
        m_newGeomEntities.push_back(vtx);
        m_cor_ptId_vertex[ptId] = vtx;
        Group::Group0D* group = gm.getNewGroup<Group::Group0D>("", m_icmd);
        gm.addGroupFor(vtx, group);
        group->add(vtx);
    }

    if (!vtx){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur Interne avec ImportMDLImplementation::getVertex, on n'arrive pas à obtenir un Vertex pour l'id "<<(short)ptId;
        throw TkUtil::Exception(messErr);
    }

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getVertex("<<(short)ptId << ") retourne le Vertex "<<vtx->getName()<< " : "<<*vtx;
    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    return vtx;
}
/*----------------------------------------------------------------------------*/
Geom::Curve* ImportMDLImplementation::getCurve(const std::string name)
{
    Geom::Curve* crv = m_cor_name_curve[name];
    Group::GroupManager& gm = m_context.getGroupManager();

    if (!crv){
        T_MdlCommand* ptr_current_command = m_cor_name_mdlCmd[name];
        if (ptr_current_command != 0){

            const T_MdlCommand& current_command = *ptr_current_command;

            if (current_command.type == MdlSegment){
                Geom::Vertex* vtx1 = getVertex(current_command.model1d.pt1);
                Geom::Vertex* vtx2 = getVertex(current_command.model1d.pt2);
                if(vtx1!=vtx2)
                {
                    crv = Geom::EntityFactory(m_context).newSegment(vtx1,vtx2);

                    //creation des connections topologiques
                    crv->add(vtx1);
                    crv->add(vtx2);
                    vtx1->add(crv);
                    vtx2->add(crv);
                }
                else {
                	//std::cerr<<"IMPORT D'UN SEGMENT DE LONGUEUR NULLE NON EFFECTUE ("<<name<<")"<<std::endl;
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                	messErr << "On ne peut créer un segment de taille nulle pour "<<name;
                	throw TkUtil::Exception(messErr);
                }


            }
            else if (current_command.type == MdlEllipse) {
                Geom::Vertex* vtx1 = getVertex(current_command.model1d.pt1);
                Geom::Vertex* vtx2 = getVertex(current_command.model1d.pt2);
                Geom::Vertex* vtxcenter = getVertex(current_command.u.arc.center);
                bool dir = bool(current_command.u.arc.direction);
                //std::cout<<"direction = "<<current_command.u.arc.direction<<" => dir = "<<(dir?"true":"false")<<std::endl;
                if(vtx1!=vtx2)
                {
                    if(current_command.u.arc.is_circle)
                        crv = Geom::EntityFactory(m_context).newArcCircle2D(vtxcenter,vtx1,vtx2, dir);
                    else
                        crv = Geom::EntityFactory(m_context).newArcEllipse(vtxcenter,vtx1,vtx2, dir);

                    //creation des connections topologiques
                    crv->add(vtx1);
                    vtx1->add(crv);
                    if(vtx2!=vtx1){
                        crv->add(vtx2);
                        vtx2->add(crv);
                    }
                }
                else {
                	//std::cerr<<"IMPORT D'UNE COURBE AVEC DEUX POINTS IDENTIQUES NON EFFECTUE ("<<name<<")"<<std::endl;
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                	messErr << "On ne peut créer une courbe avec un point de départ et un point d'arrivée identiques pour "<<name;
                	throw TkUtil::Exception(messErr);
                }
         }
            else if (current_command.type == MdlLine){
                // on va créer des Vertex même pour les points interne au support
                // de manière à rendre se dernier modifiable
                const T_MdlCommand& command_support = *(m_vCmdSup[current_command.u.line.line]);
                m_filtre_support[command_support.name.str()] = 1; // marque le support comme utilisé
                int id1 = current_command.u.line.nbegin;
                int id2 = current_command.u.line.nend;

                Geom::Vertex* vtx1 = getVertex(current_command.model1d.pt1);
                Geom::Vertex* vtx2 = getVertex(current_command.model1d.pt2);

                std::vector<Utils::Math::Point> vertices;
                vertices.push_back(vtx1->getPoint());
                if(id1<=id2)
                    for(uint i=1, id=id1; id<=id2; ++i, ++id){
                        vertices.push_back(Utils::Math::Point(command_support.u.linesup.x[id-1],
                                command_support.u.linesup.y[id-1], 0.0)*m_scale_factor);
                    }
                else
                    for(uint i=1, id=id1; id>=id2; ++i, --id){

                        vertices.push_back(Utils::Math::Point(command_support.u.linesup.x[id-1],
                                command_support.u.linesup.y[id-1], 0.0)*m_scale_factor);
                    }
                vertices.push_back(vtx2->getPoint());

                crv = Geom::EntityFactory(m_context).newBSpline(vertices, m_deg_min, m_deg_max);

                //creation des connections topologiques
                crv->add(vtx1);
                crv->add(vtx2);
                vtx1->add(crv);
                vtx2->add(crv);
            }
            else if (current_command.type == MdlLineSup) {
                // on recherche le support dans la liste des support, pour en déduire son indice
                uint indSup = 0;
                for (; indSup<m_vCmdSup.size() && std::string(m_vCmdSup[indSup]->name.str()) != name; indSup++)
                {}
                if (indSup == m_vCmdSup.size()){
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                    messErr << "Erreur Interne avec ImportMDLImplementation::getCurve, on n'arrive pas retrouver le support "
                            <<name<<" dans la liste des supports";
                    throw TkUtil::Exception(messErr);
                }
                const T_MdlCommand& command_support = *(m_vCmdSup[indSup]);

                // on recherche les points qui pointent sur ce support, pour éviter de les créer une seconde fois
                std::vector<Utils::Math::Point> vertices;
                std::vector<bool> vertices_built;
                vertices.resize(command_support.u.linesup.nb);
                vertices_built.resize(command_support.u.linesup.nb,false);
                for (uint ptId = 0; ptId<m_vCmdPt.size(); ptId++){
                    const T_MdlCommand& command_point = *(m_vCmdPt[ptId]);
                    if (command_point.u.point.is_from_line && command_point.u.point.line_id == indSup){
                        vertices[command_point.u.point.pos-1] = getVertex(ptId)->getPoint();
                        vertices_built[command_point.u.point.pos-1] = true;
                    }
                }

                // remplissage du reste du vecteur avec de nouveaux sommets
                for (uint i=0; i<command_support.u.linesup.nb; i++){
                    if (vertices_built[i] == false){
                        vertices[i] = Utils::Math::Point(command_support.u.linesup.x[i],
                                command_support.u.linesup.y[i], 0.0)*m_scale_factor;
                        vertices_built[i]=true;
                    }
                }

                crv = Geom::EntityFactory(m_context).newBSpline(vertices, m_deg_min, m_deg_max);

                //creation des sommets extrémités
                Geom::Vertex* vtx1 = Geom::EntityFactory(m_context).newVertex(vertices[0]*m_scale_factor);
                Geom::Vertex* vtx2 = Geom::EntityFactory(m_context).newVertex(vertices.back()*m_scale_factor);
                m_newGeomEntities.push_back(vtx1);
                m_newGeomEntities.push_back(vtx2);

                Group::Group0D* group = gm.getNewGroup<Group::Group0D>("", m_icmd);
                gm.addGroupFor(vtx1, group);
                gm.addGroupFor(vtx2, group);
                group->add(vtx1);
                group->add(vtx2);

                //creation des connections topologiques
                crv->add(vtx1);
                crv->add(vtx2);
                vtx1->add(crv);
                vtx2->add(crv);
            }
            else {
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur, Type de contour non prévu dans ImportMDLImplementation::getCurve", TkUtil::Charset::UTF_8));
            }

            if (crv){
            	m_newGeomEntities.push_back(crv);
            	m_cor_name_curve[name] = crv;

            	Group::Group1D* group = gm.getNewGroup<Group::Group1D>("", m_icmd);
                gm.addGroupFor(crv, group);
            	group->add(crv);
            }
        }
    }

    if (!crv){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur Interne avec ImportMDLImplementation::getCurve, on n'arrive pas à obtenir une Curve pour "<<name;
        throw TkUtil::Exception(messErr);
    }

#ifdef _DEBUG
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getCurve("<<name<< ") retourne la Curve "<<crv->getName();
    auto vertices = crv->getVertices();
    message<<", composée des sommets:";
    for (uint i=0; i<vertices.size(); i++)
        message<<" "<<vertices[i]->getName();
    message<<"\n";
    for (uint i=0; i<vertices.size(); i++)
        message<<"   "<<vertices[i]->getName()<<" : "<<*vertices[i]<<"\n";
    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));
#endif

    return crv;
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* ImportMDLImplementation::getTopoVertex(uint ptId)
{
    Topo::Vertex* vtx = m_cor_ptId_topoVertex[ptId];

    if (!vtx){
        Geom::Vertex* gtx = getVertex(ptId);
        const T_MdlCommand& command_pt = *m_vCmdPt[ptId];
        vtx = new Topo::Vertex(m_context, gtx->getCoord());
        m_newTopoEntities.push_back(vtx);
        m_cor_ptId_topoVertex[ptId] = vtx;
        vtx->setGeomAssociation(gtx);
    }

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getTopoVertex("<<(short)ptId << ") retourne le Topo::Vertex "<<vtx->getName()<< " : "<<*vtx;
    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    return vtx;
}
/*----------------------------------------------------------------------------*/
Topo::CoEdge* ImportMDLImplementation::
getCoEdge(T_MdlCutData *mcd, bool isConforme)
{
    Topo::CoEdge* coedge = m_cor_name_coedge[mcd->name.str()];
#ifdef _DEBUG_MDL
    std::cout<<"getCoEdge pour "<<mcd->name.str()<<" => "<<(coedge?coedge->getName():"arête à construire")<<std::endl;
#endif
    if (!isConforme || !coedge){
        // la courbe sur laquelle repose l'arête
        Geom::Curve* curve = getCurve(mcd->name.str());

#ifdef _DEBUG_MDL
        std::cout<<"getCoEdge pour "<<curve->getName()<<", type = "<<mcd->type<<std::endl;
#endif

        Topo::CoEdgeMeshingProperty* emp;
        // polaire ?
        bool is_polar = false;
        Utils::Math::Point polar_center;
        if (mcd->pole != -1){
        	is_polar = true;
        	const T_MdlCommand& command_pt = *m_vCmdPt[mcd->pole];
        	polar_center = Utils::Math::Point(command_pt.u.point.cd.x1,command_pt.u.point.cd.x2, 0.0)*m_scale_factor;
        }

        if (is_polar){
        	if (mcd->type == MdlCutProg)
        		emp = new Topo::EdgeMeshingPropertyGeometric(mcd->nb_mesh, mcd->is_inverse?1.0/mcd->prog1:mcd->prog1, polar_center);
        	else if (mcd->type == MdlCutBiGeom){
        		if (mcd->is_inverse)
        			emp = new Topo::EdgeMeshingPropertyBigeometric(mcd->nb_mesh, mcd->prog2, mcd->arm2, mcd->prog1, mcd->arm1, polar_center);
        		else
        			emp = new Topo::EdgeMeshingPropertyBigeometric(mcd->nb_mesh, mcd->prog1, mcd->arm1, mcd->prog2, mcd->arm2, polar_center);
        	}
        	else if (mcd->type == MdlCutInterpol)
        		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, ImportMDLImplementation pour un cas non prévu: interpolé et polaire", TkUtil::Charset::UTF_8));
        	else
        		emp = new Topo::EdgeMeshingPropertyUniform(mcd->nb_mesh, polar_center);
        }
        else {
        	if (mcd->type == MdlCutProg)
        		emp = new Topo::EdgeMeshingPropertyGeometric(mcd->nb_mesh, mcd->is_inverse?1.0/mcd->prog1:mcd->prog1);
        	else if (mcd->type == MdlCutBiGeom){
        		if (mcd->is_inverse)
        			emp = new Topo::EdgeMeshingPropertyBigeometric(mcd->nb_mesh, mcd->prog2, mcd->arm2, mcd->prog1, mcd->arm1);
        		else
        			emp = new Topo::EdgeMeshingPropertyBigeometric(mcd->nb_mesh, mcd->prog1, mcd->arm1, mcd->prog2, mcd->arm2);
        	}
        	else if (mcd->type == MdlCutInterpol)
        		emp = new Topo::EdgeMeshingPropertyInterpolate(mcd->nb_mesh, "coface"); // le nom de la coface reste à renseigner
        	else
        		emp = new Topo::EdgeMeshingPropertyUniform(mcd->nb_mesh);
        }

        T_MdlCommand* ptr_current_command = m_cor_name_mdlCmd[mcd->name.str()];
        if (ptr_current_command == 0)
            throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, ImportMDLImplementation::getCoEdge, commande Mdl non trouvée", TkUtil::Charset::UTF_8));

        const T_MdlCommand& current_command = *ptr_current_command;

        Topo::Vertex* vtx1 = getTopoVertex(current_command.model1d.pt1);
        Topo::Vertex* vtx2 = getTopoVertex(current_command.model1d.pt2);

        coedge = new Topo::CoEdge(m_context, emp, vtx1, vtx2);
        m_newTopoEntities.push_back(coedge);
        delete emp;

//        // gestion des groupes pour le contour
//        if (mcd->groups_name){
//            std::istringstream isgroups(mcd->groups_name);
//            while (!isgroups.eof()){
//                std::string name;
//                isgroups>>name;
//                if (!name.empty())
//                    coedge->groupsName().push_back(name);
//            }
//        }

        // la projection sur le contour
        coedge->setGeomAssociation(curve);

        m_cor_name_coedge[mcd->name.str()] = coedge;
    }

#ifdef _DEBUG_MDL
    std::cout<<" retourne => "<<coedge->getName()<<std::endl;
#endif

    return coedge;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> ImportMDLImplementation::
getCoEdges(const T_MdlCommand& current_command,
		std::map <std::string, T_MdlCutData*>& cor_model1d_MdlCutData,
		uint id, uint nb0, uint nb1)
{
#ifdef _DEBUG_MDL
    std::cout<<"getCoEdges pour "<<current_command.name.str()
    		 <<", "<<id<<", "<<nb0<<", "<<nb1<<std::endl;
#endif
    std::vector<std::string> coedgesNames;

    uint nb_cut_data = current_command.u.area.nb_cut_data;

    bool liste_ok = false;
    bool liste_start = false;
    uint nb_mesh_cote_j = 0;
    for( uint j=0; j<nb_cut_data; j++ ){
    	T_MdlCutData &mcd = current_command.u.area.data[j];
    	if (mcd.logical_id==id){

    		// cas où on recherche le premier contour à l'intérieur de la zone
    		if (!liste_start && nb0 == nb_mesh_cote_j)
    			liste_start = true;

    		if (liste_start) {
    			liste_ok = true;
    			// cas où on cumule les arêtes
    			if (nb_mesh_cote_j+mcd.nb_mesh<=nb1){
    				// le voisin si nécesssaire pour la description
    				T_MdlCutData *mcdv = cor_model1d_MdlCutData[std::string(mcd.name.str())];
    				bool isConforme = true;
    				if (mcdv == 0){
    					// cas non-conforme
    					mcdv = &mcd;
    					isConforme = false;
    				}
    				Topo::CoEdge* coedge = getCoEdge(mcdv, isConforme);
    				coedgesNames.push_back(coedge->getName());
#ifdef _DEBUG_MDL
    				std::cout<<" coedge acceptée "<<coedge->getName()<<std::endl;
#endif
    			}
    			if (nb_mesh_cote_j+mcd.nb_mesh == nb1)
    				// on a ce que l'on désire
    				liste_start = false;
    			else if (nb_mesh_cote_j+mcd.nb_mesh > nb1){
    				liste_start = false;
    				liste_ok = false;

					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    				message << "Dans la zone "<<current_command.name.str()<<" le contour " << mcd.name.str()
    				        <<" est en Interpolé, mais cette configuration avec des contours partiellements interpolés n'est pas acceptée actuellement. ";
    				message << "La méthode uniforme sera utilisée à la place.";
    				m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::WARNING));
    			}
    		} // end if (liste_start)

    		nb_mesh_cote_j += mcd.nb_mesh;

    	} // if (mcd.logical_id==id)
    }

    if (!liste_ok)
    	coedgesNames.clear();

    return coedgesNames;
}
/*----------------------------------------------------------------------------*/
std::vector<Geom::GeomEntity*>& ImportMDLImplementation::
getNewGeomEntities()
{
    return m_newGeomEntities;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::TopoEntity*>& ImportMDLImplementation::
getNewTopoEntities()
{
    return m_newTopoEntities;
}
/*----------------------------------------------------------------------------*/
void ImportMDLImplementation::
dupVertexNonConformalRelation(std::vector<Topo::CoFace* >& cofaces)
{
#ifdef _DEBUG_MDL
    std::cout << "ImportMDLImplementation::dupVertexNonConformalRelation("<<cofaces.size()<<" cofaces) en cours ..."<<std::endl;
#endif

    if (cofaces.empty())
    	return;

    // marque des cofaces suivant leur groupe d'appartenance (marque = indice + 1)
    std::map<Topo::CoFace*, uint> filtre_coface;

    // parcours des cofaces en passant par les CoEdges communes à deux faces
    uint nb_cofaces_vues = 0;
    uint num_groupe = 1;
    do {
#ifdef _DEBUG_MDL
    std::cout << " Constitution du groupe "<<num_groupe<<std::endl;
#endif
        // recherche d'une CoFace non vue
        Topo::CoFace* coface_dep = 0;
        for (uint i=0; i<cofaces.size() && coface_dep==0; i++)
            if (filtre_coface[cofaces[i]] == 0)
                coface_dep = cofaces[i];

        if (coface_dep == 0)
        	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, ImportMDLImplementation::dupVertexNonConformalRelation ne trouve pas de CoFace pour démarrer", TkUtil::Charset::UTF_8));

        // on part d'une liste de faces et on obtient une liste de faces voisines non-vues
        std::vector<Topo::CoFace*> cofaces_dep;
        cofaces_dep.push_back(coface_dep);
        nb_cofaces_vues += 1;
        filtre_coface[coface_dep] = num_groupe;
#ifdef _DEBUG_MDL
    std::cout << "   CoFace : "<<coface_dep->getName()<<std::endl;
#endif

        do {
            std::vector<Topo::CoFace*> cofaces_vois;

            for (std::vector<Topo::CoFace*>::iterator iter1 = cofaces_dep.begin();
                    iter1 != cofaces_dep.end(); ++iter1){
                Topo::CoFace* coface = *iter1;
                std::vector<Topo::CoFace* > cofaces_vois_i;
                coface->getNeighbour(cofaces_vois_i);

                for (std::vector<Topo::CoFace* >::iterator iter2 = cofaces_vois_i.begin();
                                    iter2 != cofaces_vois_i.end(); ++iter2){
                    Topo::CoFace* coface_vois = *iter2;
                    if (filtre_coface[coface_vois] == 0){
                        filtre_coface[coface_vois] = num_groupe;
                        nb_cofaces_vues += 1;
                        cofaces_vois.push_back(coface_vois);
#ifdef _DEBUG_MDL
    std::cout << "   CoFace : "<<coface_vois->getName()<<std::endl;
#endif
                    }
                }
            }

            cofaces_dep = cofaces_vois;
        } while (!cofaces_dep.empty());

        // un nouveau groupe de face commune reliées
        num_groupe += 1;

    } while (nb_cofaces_vues != cofaces.size());

    // On marque des CoEdge suivant leur groupe d'appartenance (marque = indice + 1)
    std::map<Topo::CoEdge*, uint> filtre_coedge;

    // marque les sommets en fonction du groupe dans lesquels ils sont vus (en dernier)
    std::map<Topo::Vertex*, uint> filtre_vertex;

    // liste des Vertex à dupliquer (vus dans deux groupes)
    std::list<Topo::Vertex*> verticesToDup;

    std::list<Topo::CoFace*> liste_cofaces;

    for (std::map<Topo::CoFace*, uint>::iterator iter1 = filtre_coface.begin();
                iter1 != filtre_coface.end(); ++iter1)
    	liste_cofaces.push_back(iter1->first);
    liste_cofaces.sort(Utils::Entity::compareEntity);

#ifdef _DEBUG_MDL
    std::cout << " Identification des sommets à dupliquer:"<<std::endl;
#endif
    for (std::list<Topo::CoFace*>::iterator iter1 = liste_cofaces.begin();
            iter1 != liste_cofaces.end(); ++iter1){

        Topo::CoFace* coface = *iter1;
        num_groupe = filtre_coface[coface];

        for (Topo::CoEdge* coedge : coface->getCoEdges()){
            filtre_coedge[coedge] = num_groupe;

            const std::vector<Topo::Vertex* > & vertices = coedge->getVertices();

            for (std::vector<Topo::Vertex* >::const_iterator iter3 = vertices.begin();
                    iter3 != vertices.end(); ++iter3){
                Topo::Vertex* vertex = *iter3;

                uint vtx_groupe = filtre_vertex[vertex];
                if (vtx_groupe && vtx_groupe!=num_groupe){
                    verticesToDup.push_back(vertex);
#ifdef _DEBUG_MDL
    std::cout << "   Vertex : "<<vertex->getName()<<std::endl;
#endif
                }
                filtre_vertex[vertex] = num_groupe;
            }
        }
    }
    // on évite de référencer les sommets plusieur fois
    verticesToDup.sort(Utils::Entity::compareEntity);
    verticesToDup.unique();

    // pour tous les vertex à dupliquer ...
#ifdef _DEBUG_MDL
    std::cout << " On duplique les sommets:"<<std::endl;
#endif
    for (std::list<Topo::Vertex*>::iterator iter1 = verticesToDup.begin();
            iter1 != verticesToDup.end(); ++iter1){
        Topo::Vertex* vertex = *iter1;

        // correspondance entre le groupe et le sommet par lequel on remplace le sommet à dupliquer
        std::map<uint, Topo::Vertex*> cor_grp_vtx;
        // on garde le sommet originel pour l'un des groupes (celui du filtre par ex)
        cor_grp_vtx[filtre_vertex[vertex]] = vertex;

        // les groupes auquels appartient ce sommet
        for (Topo::CoEdge* coedge : vertex->getCoEdges()){
            Topo::Vertex* newVtx = cor_grp_vtx[filtre_coedge[coedge]];
            if (newVtx == 0){
                newVtx = vertex->clone();
                cor_grp_vtx[filtre_coedge[coedge]] = newVtx;
                m_newTopoEntities.push_back(newVtx);
            }

            if (newVtx != vertex){
#ifdef _DEBUG_MDL
    std::cout << "   Vertex : "<<vertex->getName()<<" remplacé par "<<newVtx->getName()
            <<" dans "<<coedge->getName()<<std::endl;
#endif
                coedge->replace(vertex, newVtx, true, false, 0);
            }
        }
    }

}
/*----------------------------------------------------------------------------*/
bool ImportMDLImplementation::getFilter(std::set<std::string>& filter)
{
	if (!m_zones.empty()){

		for (uint i=0; i<m_zones.size(); i++)
			filter.insert(m_zones[i]);

		return true;

	} else if (!m_groupe.empty()){

	    for (std::vector<const T_MdlCommand*>::const_iterator iter = m_vCmdArea.begin(); iter!=m_vCmdArea.end(); ++iter){
	        const T_MdlCommand& current_command = **iter;
	        if (current_command.u.area.groups_name
	                && !std::string(current_command.u.area.groups_name).empty()){
	            std::istringstream isgroups(TkUtil::UTF8String(current_command.u.area.groups_name).trim());
	            std::string name;
	            while (!isgroups.eof()){
	                isgroups>>name;
	                if (!name.empty() && name == m_groupe){
	                	filter.insert(current_command.name.str());
	                }
	            }
	        }
	    } // end for iter

		return true;

	} else
		return false;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/
