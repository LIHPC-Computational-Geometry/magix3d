/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 * \file ImportMDL2Commandes.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/7/16
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/ImportMDL2Commandes.h"
#include "Geom/GeomManager.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomHelper.h"
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
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
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
ImportMDL2Commandes::
ImportMDL2Commandes(Internal::Context& c,
        const std::string& n,
		const bool withTopo)
: ImportMDLItf(c, n) ,m_withTopo(withTopo)
{}
/*----------------------------------------------------------------------------*/
ImportMDL2Commandes::~ImportMDL2Commandes()
{
}
/*----------------------------------------------------------------------------*/
void ImportMDL2Commandes::perform()
{
#ifdef _DEBUG_MDL
    std::cout << "ImportMDL2Commandes::perform() en cours ..."<<std::endl;
#endif

    readMDL();

    if (m_do_scale){
#ifdef _DEBUG_MDL
    	std::cout<<"Mise à l'échelle avec length_unit = "<<m_mdl_info->length_unit<<", m_scale_factor = "<<m_scale_factor<<std::endl;
#endif
    }

    performGeom();

    if (m_withTopo)
    	performTopo();
}
/*----------------------------------------------------------------------------*/
void ImportMDL2Commandes::performGeom()
{

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

    // boucle sur tous les points,
     // les points sont demandés donc créés
     for (uint ptId = 0; ptId<m_vCmdPt.size(); ptId++)
         Geom::Vertex* v = getVertex(ptId);

     // les surfaces
     for (std::vector<const T_MdlCommand*>::const_iterator iter = m_vCmdArea.begin(); iter!=m_vCmdArea.end(); ++iter){
    	 const T_MdlCommand& current_command = **iter;

    	 std::vector<Geom::Curve*> curves;
    	 uint nb_cut_data = current_command.u.area.nb_cut_data;
    	 for( uint j=0; j<nb_cut_data; j++ ){
    		 T_MdlCutData &mcd = current_command.u.area.data[j];
    		 Geom::Curve* crv = getCurve(mcd.name.str());
    		 curves.push_back(crv);

    		 if (!mcd.groups_name.nil()){
    			 std::istringstream isgroups(TkUtil::UTF8String(mcd.groups_name.str()).trim());
    			 while (!isgroups.eof()){
    				 std::string name;
    				 isgroups>>name;
    				 if (!name.empty()){
    					 std::vector<std::string> ve;
    					 ve.push_back(crv->getName());
    					 m_context.getGeomManager().addToGroup(ve, 1, name);
    				 }
    			 }
    		 }

    	 } // end for j<nb_cut_data
#ifdef _DEBUG_MDL
    	 std::cout << "Création de la Surface "<<current_command.name<<std::endl;
#endif

    	 Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalGeomManager().newPlanarSurface(curves, "");
    	 Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
    	 CHECK_NULL_PTR_ERROR(cmd);
    	 Geom::Surface* surf = cmd->getSurfaceObj();

#ifdef _DEBUG_MDL
        std::cout << "  avec comme nom "<<surf->getName()<<std::endl;
        std::cout << "  groups_name = \""<<current_command.u.area.groups_name<<"\""<<std::endl;
#endif
        m_cor_name_surface[current_command.name.str()] = surf;

        if (current_command.u.area.groups_name
                && !std::string(current_command.u.area.groups_name).empty()){
            std::istringstream isgroups(TkUtil::UTF8String(current_command.u.area.groups_name).trim());
            std::string name;
            while (!isgroups.eof()){
                isgroups>>name;
                if (!name.empty()){
#ifdef _DEBUG_MDL
                    std::cout << "groups_name => \""<<name<<"\"\n";
#endif
                    std::vector<std::string> ve;
                    ve.push_back(surf->getName());
                    m_context.getGeomManager().addToGroup(ve, 2, name);
                }
            }
        }
        else {
            // cas avec un nom de groupe par défaut
#ifdef _DEBUG_MDL
            std::cout << "groups_name par défaut\n";
#endif
            std::string nomDef = current_command.name.str();
            std::vector<std::string> ve;
            ve.push_back(surf->getName());
            m_context.getGeomManager().addToGroup(ve, 2, nomDef);
        }

    } // end for iter = m_vCmdArea.begin()

}
/*----------------------------------------------------------------------------*/
void ImportMDL2Commandes::performTopo()
{
	for (uint ptId = 0; ptId<m_vCmdPt.size(); ptId++)
		Topo::Vertex* v = getTopoVertex(ptId);


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
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                    messErr << "Dans la zone "<<current_command.name.str()<<" le contour " << mcd.name.str()
                            <<" est détecté comme étant non-conforme, ce n'est pas supporté actuellement";
                    throw TkUtil::Exception(messErr);
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

        // liste d'arêtes avec une interpolation qui doit référencer cette face topo
        std::vector<Topo::CoEdge* > coedges_interpolated;

        // TODO [EB] tenir compte de la demande de lissage ; current_command.u.area.algo_smooth==0

        // on identifie la structuration ou non de la zone en fonction du nom de l'algo
        std::string meshAlgo(current_command.u.area.algo_mesh);
        uint nb_cotes_logiques = current_command.u.area.data[current_command.u.area.nb_cut_data-1].logical_id+1;
        if (meshAlgo == "Takmiu1"
            || meshAlgo == "Takeo"
            || meshAlgo == "InterpolationTransfinie"){


        	uint nb_cut_data = current_command.u.area.nb_cut_data;
        	// on détermine la répartition des contours (nb de model1d / côté logique)
        	uint nb_cotes_logiques = current_command.u.area.data[nb_cut_data-1].logical_id+1;
        	uint* nb_model1d = new uint[nb_cotes_logiques]; // répartition par côté logique
        	uint* nb_mesh_cote = new uint[nb_cotes_logiques]; // nombre de bras par côté logique
        	for ( uint j=0; j<nb_cotes_logiques; j++ ){
        		nb_model1d[j] = 0;
        		nb_mesh_cote[j] = 0;
        	}
        	for( uint j=0; j<nb_cut_data; j++ ){
        		T_MdlCutData &mcd = current_command.u.area.data[j];
        		nb_model1d[mcd.logical_id]+=1;
        		nb_mesh_cote[mcd.logical_id]+=mcd.nb_mesh;
#ifdef _DEBUG_MDL
        		std::cout << " model1d : " << mcd.name
        				<< ", id: " << mcd.id
						<< ", logical_id: " << mcd.logical_id
						<< ", nb_mesh: " << mcd.nb_mesh
						<< ", is_inverse: " << mcd.is_inverse
						<< std::endl;
#endif
        	}

        	// parcours par côté logique
        	uint ind_data = 0;
        	std::vector<Geom::Vertex*> vertices;

        	// recherche du sommet commun à la première courbe et dernière
        	T_MdlCutData &mcd0 = current_command.u.area.data[0];
        	T_MdlCutData &mcdN = current_command.u.area.data[nb_cut_data-1];
        	Geom::Curve*  crv0 = m_cor_name_curve[mcd0.name.str()];
        	Geom::Curve*  crvN = m_cor_name_curve[mcdN.name.str()];
        	Geom::Vertex* vtx = Geom::GeomHelper::getCommonVertex(crv0, crvN);
        	vertices.push_back(vtx);

        	Geom::Curve*  crv_last = 0;

        	for ( uint j=0; j<nb_cotes_logiques; j++ ){
#ifdef _DEBUG_MDL
        		std::cout << "+ côté logique j = "<< j << " nb mesh "<<nb_mesh_cote[j]<< std::endl;
#endif

        		bool isConforme = true;
        		//std::vector<Topo::CoEdge* > coedges;
        		// ratios pour la semi-conformité
        		std::vector<uint> ratios;
        		uint nb_mesh_cote_j = 0;
        		for ( uint k=0; k<nb_model1d[j]; k++) {
        			T_MdlCutData &mcd = current_command.u.area.data[ind_data++];

        			Geom::Curve*  contour = m_cor_name_curve[mcd.name.str()];
        			if (k==0 && crv_last){
        				Geom::Vertex* vtx = Geom::GeomHelper::getCommonVertex(contour, crv_last);
        				vertices.push_back(vtx);
        			}

        			// le voisin si nécesssaire pour la description
        			T_MdlCutData *mcdv = cor_model1d_MdlCutData[std::string(mcd.name.str())];
        			if (mcdv == 0){
        				// cas non-conforme
        				mcdv = &mcd;
        				isConforme = false;
        			}

        			// construction des arêtes communes
        			Topo::CoEdge* coedge = getCoEdge(mcdv, isConforme);
        			//coedges.push_back(coedge);

            		if (mcd.type == MdlCutInterpol)
            			coedges_interpolated.push_back(coedge);

        			// cas semi-conforme ?
        			if (isConforme && mcd.nb_mesh < mcdv->nb_mesh){
        				ratios.push_back(mcdv->nb_mesh/mcd.nb_mesh);
        			}
        			else
        				ratios.push_back(1);

        			nb_mesh_cote_j += mcd.nb_mesh;

        			crv_last = contour;
        		} // end for k<nb_model1d[j]

        		if (!isConforme){
                	TkUtil::UTF8String   messErr (TkUtil::Charset::UTF_8);
                	messErr << "Erreur, dans la zone "<<current_command.name.str()
                	      	<<", on ne supporte pas le cas semi-conforme pour le moment";
                	throw TkUtil::Exception(messErr);
                }

        	} // end for j<nb_cotes_logiques

            delete [] nb_model1d;
            delete [] nb_mesh_cote;

            // création de la CoFace structurée
        	Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalTopoManager()
        			.newStructuredTopoOnSurface(m_cor_name_surface[current_command.name.str()], vertices);

        	// mise à jour des interpolations / coface
        	MAJInterpolated(coedges_interpolated, cmdIfc->getFace());

        }
        else {
        	// cas non structuré
        	Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalTopoManager().newUnstructuredTopoOnGeometry(m_cor_name_surface[current_command.name.str()]);
        }

    } // end for iter = m_vCmdArea.begin()
}
/*----------------------------------------------------------------------------*/
void ImportMDL2Commandes::
MAJInterpolated(std::vector<Topo::CoEdge*>& coedges, std::string coface)
{
	for (uint i=0; i<coedges.size(); i++){
		Topo::CoEdge* coedge = coedges[i];
		Topo::EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<Topo::EdgeMeshingPropertyInterpolate*>(coedge->getMeshingProperty());
		CHECK_NULL_PTR_ERROR(interpol);
		interpol->setCoFace(coface);

		std::vector<Topo::CoEdge*> loc_coedges;
		loc_coedges.push_back(coedge);
		m_context.getLocalTopoManager().setMeshingProperty(*interpol, loc_coedges);
	}
}
/*----------------------------------------------------------------------------*/
Geom::Vertex* ImportMDL2Commandes::getVertex(uint ptId)
{
    Geom::Vertex* vtx = m_cor_ptId_vertex[ptId];

    if (!vtx){
        const T_MdlCommand& command_pt = *m_vCmdPt[ptId];
        Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalGeomManager().newVertex(Utils::Math::Point(command_pt.u.point.cd.x1,command_pt.u.point.cd.x2, 0.0)*m_scale_factor);
        Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
        CHECK_NULL_PTR_ERROR(cmd);
        vtx = cmd->getVertexObj();
        m_cor_ptId_vertex[ptId] = vtx;
    }

    if (!vtx){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur Interne avec ImportMDL2Commandes::getVertex, on n'arrive pas à obtenir un Vertex pour l'id "<<(short)ptId;
        throw TkUtil::Exception(messErr);
    }

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getVertex("<<(short)ptId << ") retourne le Vertex "<<vtx->getName()<< " : "<<*vtx;
    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    return vtx;
}
/*----------------------------------------------------------------------------*/
Geom::Curve* ImportMDL2Commandes::getCurve(const std::string name)
{
    Geom::Curve* crv = m_cor_name_curve[name];

    if (!crv){
        T_MdlCommand* ptr_current_command = m_cor_name_mdlCmd[name];
        if (ptr_current_command != 0){

            const T_MdlCommand& current_command = *ptr_current_command;

            if (current_command.type == MdlSegment){
            	Geom::Vertex* vtx1 = getVertex(current_command.model1d.pt1);
            	Geom::Vertex* vtx2 = getVertex(current_command.model1d.pt2);
            	if(vtx1!=vtx2)
            	{
            		Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalGeomManager().newSegment(vtx1,vtx2);
            		Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
            		CHECK_NULL_PTR_ERROR(cmd);
            		crv = cmd->getCurveObj();
            	}
            	else {
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
                Internal::M3DCommandResultIfc* cmdIfc = 0;
                if(current_command.u.arc.is_circle)
                	cmdIfc = m_context.getLocalGeomManager().newArcCircle(vtxcenter,vtx1,vtx2, dir);
                else
                	cmdIfc = m_context.getLocalGeomManager().newArcEllipse(vtxcenter,vtx1,vtx2, dir);

                Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
                CHECK_NULL_PTR_ERROR(cmd);
                crv = cmd->getCurveObj();
            }
            else if (current_command.type == MdlLine){

                // on va utiliser des Vertex aux extrémités uniquement
                const T_MdlCommand& command_support = *(m_vCmdSup[current_command.u.line.line]);
                m_filtre_support[command_support.name.str()] = 1; // marque le support comme utilisé
                int id1 = current_command.u.line.nbegin;
                int id2 = current_command.u.line.nend;

                Geom::Vertex* vtx1 = getVertex(current_command.model1d.pt1);
                Geom::Vertex* vtx2 = getVertex(current_command.model1d.pt2);

                std::vector<Utils::Math::Point> vertices;
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

                Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalGeomManager().newBSpline(vtx1,vertices, vtx2, 1, 2, "");
                Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
                CHECK_NULL_PTR_ERROR(cmd);
                crv = cmd->getCurveObj();
            }
            else if (current_command.type == MdlLineSup) {
            	// on ne fait rien ...
            }
            else {
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur, Type de contour non prévu dans ImportMDL2Commandes::getCurve", TkUtil::Charset::UTF_8));
            }

            if (crv)
            	m_cor_name_curve[name] = crv;

        }
    }

    if (!crv){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur Interne avec ImportMDL2Commandes::getCurve, on n'arrive pas à obtenir une Curve pour "<<name;
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
Topo::Vertex* ImportMDL2Commandes::getTopoVertex(uint ptId)
{
    Topo::Vertex* vtx = m_cor_ptId_topoVertex[ptId];

    if (!vtx){
    	Geom::Vertex* gtx = getVertex(ptId);
    	Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalTopoManager().newTopoOnGeometry(gtx);
    	Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
    	CHECK_NULL_PTR_ERROR(cmd);
    	vtx = cmd->getTopoVertexObj();
    	m_cor_ptId_topoVertex[ptId] = vtx;
    }

    if (!vtx){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur Interne avec ImportMDL2Commandes::getTopoVertex, on n'arrive pas à obtenir un Vertex pour l'id "<<(short)ptId;
        throw TkUtil::Exception(messErr);
    }

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getTopoVertex("<<(short)ptId << ") retourne le Topo::Vertex "<<vtx->getName()<< " : "<<*vtx;
    m_context.getLogDispatcher().log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    return vtx;
}
/*----------------------------------------------------------------------------*/
Topo::CoEdge* ImportMDL2Commandes::
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
        		emp = new Topo::EdgeMeshingPropertyInterpolate(mcd->nb_mesh, "coface encore inconnue");
        	else
        		emp = new Topo::EdgeMeshingPropertyUniform(mcd->nb_mesh);
        }

        T_MdlCommand* ptr_current_command = m_cor_name_mdlCmd[mcd->name.str()];
        if (ptr_current_command == 0)
            throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, ImportMDL2Commandes::getCoEdge, commande Mdl non trouvée", TkUtil::Charset::UTF_8));


        Internal::M3DCommandResultIfc* cmdIfc = m_context.getLocalTopoManager().newTopoOnGeometry(curve);
        Internal::M3DCommandResult* cmd = dynamic_cast<Internal::M3DCommandResult*>(cmdIfc);
        CHECK_NULL_PTR_ERROR(cmd);
        coedge = cmd->getEdgeObj();

        std::vector<Topo::CoEdge*> coedges;
		coedges.push_back(coedge);
        m_context.getLocalTopoManager().setMeshingProperty(*emp, coedges);

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

        m_cor_name_coedge[mcd->name.str()] = coedge;
    }

#ifdef _DEBUG_MDL
    std::cout<<" retourne => "<<coedge->getName()<<std::endl;
#endif

    return coedge;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> ImportMDL2Commandes::
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
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/
