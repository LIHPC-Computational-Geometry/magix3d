/*----------------------------------------------------------------------------*/
/*
 * \file CommandExtrudeTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/10/2017
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandExtrudeTopo.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/FaceMeshingPropertyDirectional.h"

#include "Geom/CommandExtrusion.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"

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
CommandExtrudeTopo::
CommandExtrudeTopo(Internal::Context& c,
		Geom::CommandExtrusion* commandGeom,
		std::vector<TopoEntity*>& cofaces,
		const Utils::Math::Vector& dv)
:CommandTransformTopo(c, "Extrusion de la topologie", cofaces)
, m_commandGeom(commandGeom)
, m_dv(dv)
{


}
/*----------------------------------------------------------------------------*/
CommandExtrudeTopo::
~CommandExtrudeTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::
init()
{
	if (m_cofaces.empty())
		for (std::vector<Topo::TopoEntity*>::const_iterator iter=getEntities().begin();
				iter!=getEntities().end(); ++iter){
			Topo::CoFace* coface = dynamic_cast<Topo::CoFace*>(*iter);
			CHECK_NULL_PTR_ERROR(coface);
			m_cofaces.push_back(coface);
		}
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandExtrudeTopo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );
#ifdef _DEBUG2
    m_commandGeom->printInfoAssociations();
#endif

    // remplissage m_cofaces
    init();

    // non nécessaire dans notre cas
    std::map<Vertex*, uint> filtre_vertex;
    std::map<CoEdge*, uint> filtre_coedge;
    std::map<CoFace*, uint> filtre_coface;

    // force la sauvegarde des relations topologiques pour toutes les entités et celles de niveau inférieur
    TopoHelper::saveTopoEntities(m_cofaces, &getInfoCommand());

    // copie la topo sur la base
    std::vector<CoFace*> new_cofaces;
    duplicate(m_cofaces, filtre_vertex, filtre_coedge, filtre_coface, new_cofaces);

    // translation de la copie

    // création de l'opérateur de translation via OCC
    gp_Trsf transf;
    gp_Vec vec(m_dv.getX(),m_dv.getY(),m_dv.getZ());
    transf.SetTranslation(vec);

    // filtre sur les entités pour ne les prendre qu'une fois
    std::map<TopoEntity*, bool> filtre;

    for (uint i=0; i<new_cofaces.size(); i++)
    	transform(new_cofaces[i], &transf, filtre);

    // mise à jour des association entre les nouvelles faces et la géométrie en face de la base
    CommandEditTopo::updateGeomAssociation(new_cofaces,
    		m_commandGeom->getAssociationV2VOpp(),
			m_commandGeom->getAssociationC2COpp(),
			m_commandGeom->getAssociationS2SOpp(),
			false);

    // nombre de bras suivant l'extrusion
    uint ni = getContext().getTopoManager().getDefaultNbMeshingEdges();


    // création des arêtes entre les 2
    std::map<Vertex*, std::vector<CoEdge* > > vtx2coedges;
    constructExtrudeCoEdges(m_cofaces, new_cofaces,
    		vtx2coedges, ni);

    // création des faces entre les 2
    std::map<CoEdge*, std::vector<CoFace* > > coedge2cofaces;
    constructExtrudeFaces(m_cofaces, new_cofaces,
    		vtx2coedges, coedge2cofaces);

    // création des blocs entre les 2
    constructExtrudeBlocks(m_cofaces, new_cofaces,
    		vtx2coedges, coedge2cofaces, ni);

    // mise à jour des association entre les faces de la base et la géométrie
    CommandEditTopo::updateGeomAssociation(m_cofaces,
    		m_commandGeom->getAssociationV2V(),
			m_commandGeom->getAssociationC2C(),
			m_commandGeom->getAssociationS2S(),
			false);

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

//    std::cout<<"Dans CommandExtrudeTopo: "<<std::endl;
//    std::cout<< getInfoCommand() <<std::endl;

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}

/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::constructExtrudeCoEdges(std::vector<CoFace*>& cofaces_0,
    		std::vector<CoFace*>& cofaces_1,
			std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
			uint ni)
{
    // filtre sur les sommets pour ne les prendre qu'une fois
    std::map<Vertex*, uint> filtre_vu;

    // construction des arêtes communes avec ni bras entre les sommets des 2 topo 2D
    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeCoEdges avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));


    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        std::vector<Edge* > edges_0;
        std::vector<Edge* > edges_1;
        (*iter1_0)->getEdges(edges_0);
        (*iter1_1)->getEdges(edges_1);

        if (edges_0.size() != edges_1.size())
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeCoEdges avec des nombres de Edge différents", TkUtil::Charset::UTF_8));

        std::vector<Edge* >::iterator iter2_0 = edges_0.begin();
        std::vector<Edge* >::iterator iter2_1 = edges_1.begin();

        for (; iter2_0 != edges_0.end(); ++iter2_0, ++iter2_1){

            std::vector<CoEdge* > coedges_0;
            std::vector<CoEdge* > coedges_1;
            (*iter2_0)->getCoEdges(coedges_0);
            (*iter2_1)->getCoEdges(coedges_1);

            if (coedges_0.size() != coedges_1.size())
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeCoEdges avec des nombres de CoEdge différents", TkUtil::Charset::UTF_8));

            std::vector<CoEdge* >::iterator iter3_0 = coedges_0.begin();
            std::vector<CoEdge* >::iterator iter3_1 = coedges_1.begin();

            for (; iter3_0 != coedges_0.end(); ++iter3_0, ++iter3_1){

                std::vector<Vertex* > vertices_0;
                std::vector<Vertex* > vertices_1;
                (*iter3_0)->getVertices(vertices_0);
                (*iter3_1)->getVertices(vertices_1);

                if (vertices_0.size() != vertices_1.size())
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeCoEdges avec des nombres de Vertex différents", TkUtil::Charset::UTF_8));

                std::vector<Vertex* >::iterator iter4_0 = vertices_0.begin();
                std::vector<Vertex* >::iterator iter4_1 = vertices_1.begin();

                for (; iter4_0 != vertices_0.end(); ++iter4_0, ++iter4_1){
                    Vertex* vtx0 = *iter4_0;
                    Vertex* vtx1 = *iter4_1;


                    if (filtre_vu[vtx0] == 0){

                    	// la discrétisation pour les arêtes qui suivent la révolution
                    	EdgeMeshingPropertyUniform emp(ni);
                    	CoEdge* newCoEdge = new Topo::CoEdge(getContext(), &emp, vtx0, vtx1);
                    	getInfoCommand().addTopoInfoEntity(newCoEdge, Internal::InfoCommand::CREATED);
#ifdef _DEBUG2
                    	std::cout<<"CoEdge "<<newCoEdge->getName()<<" entre  "<<vtx0->getName()<<" et "<<vtx1->getName()<<std::endl;
#endif
                    	updateGeomAssociation(vtx0->getGeomAssociation(), newCoEdge);

                    	filtre_vu[vtx0] = 1;
                    	vtx2coedges[vtx0].push_back(newCoEdge);
                    } // end if filtre_vu
                } // end for vertices
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::constructExtrudeFaces(std::vector<CoFace*>& cofaces_0,
    		std::vector<CoFace*>& cofaces_1,
			std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
			std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces)
{

    // filtre sur les arêtes pour ne les prendre qu'une fois
    std::map<CoEdge*, uint> filtre_vu;

    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeFaces avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        std::vector<Edge* > edges_0;
        std::vector<Edge* > edges_1;
        (*iter1_0)->getEdges(edges_0);
        (*iter1_1)->getEdges(edges_1);

        if (edges_0.size() != edges_1.size())
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeFaces avec des nombres de Edge différents", TkUtil::Charset::UTF_8));

        std::vector<Edge* >::iterator iter2_0 = edges_0.begin();
        std::vector<Edge* >::iterator iter2_1 = edges_1.begin();

        for (; iter2_0 != edges_0.end(); ++iter2_0, ++iter2_1){

            std::vector<CoEdge* > coedges_0;
            std::vector<CoEdge* > coedges_1;
            (*iter2_0)->getCoEdges(coedges_0);
            (*iter2_1)->getCoEdges(coedges_1);

            if (coedges_0.size() != coedges_1.size())
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeFaces avec des nombres de CoEdge différents", TkUtil::Charset::UTF_8));

            std::vector<CoEdge* >::iterator iter3_0 = coedges_0.begin();
            std::vector<CoEdge* >::iterator iter3_1 = coedges_1.begin();

            for (; iter3_0 != coedges_0.end(); ++iter3_0, ++iter3_1){
                CoEdge* coedge_0 = *iter3_0;
                CoEdge* coedge_1 = *iter3_1;

                if (filtre_vu[coedge_0] == 0){
                    filtre_vu[coedge_0] = 1;

                    std::vector<Vertex* > vertices_0;
                    std::vector<Vertex* > vertices_1;
                    (*iter3_0)->getVertices(vertices_0);
                    (*iter3_1)->getVertices(vertices_1);

                    if (vertices_0.size() != vertices_1.size())
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeFaces avec des nombres de Vertex différents", TkUtil::Charset::UTF_8));
                    if (vertices_0.size() != 2)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeFaces avec arêtes n'ayant pas 2 sommets", TkUtil::Charset::UTF_8));

                    Vertex* vtx0_0 = vertices_0[0];
                    Vertex* vtx1_0 = vertices_0[1];

                    std::vector<CoEdge* >& coedges0 = vtx2coedges[vtx0_0];
                    std::vector<CoEdge* >& coedges1 = vtx2coedges[vtx1_0];

                    CoEdge* coedge_dep = coedge_0;
                    CoEdge* coedge_arr = coedge_1;

                    if (coedges0.size() != 1 || coedges1.size() != 1){
                    	std::cerr<<"coedges0.size() = "<<coedges0.size()<<std::endl;
                    	std::cerr<<"coedges1.size() = "<<coedges1.size()<<std::endl;
                    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeFaces avec autre chose qu'une arête suivant extrusion", TkUtil::Charset::UTF_8));
                    }

                    CoEdge* coedge0 = coedges0[0];
                    CoEdge* coedge1 = coedges1[0];


                    Vertex* vtx2 = coedge0->getOppositeVertex(vtx0_0);
                    Vertex* vtx3 = coedge1->getOppositeVertex(vtx1_0);

                    Edge* edge0 = new Topo::Edge(getContext(), coedge0);
                    Edge* edge1 = new Topo::Edge(getContext(), coedge_dep);
                    Edge* edge2 = new Topo::Edge(getContext(), coedge1);
                    Edge* edge3 = new Topo::Edge(getContext(), coedge_arr);

                    CoFace* newCoFace = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);


#ifdef _DEBUG2
                    std::cout<<" Création de la CoFace "<<newCoFace->getName()<<" par extrusion"<<std::endl;
#endif
                    getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
                    getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
                    getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
                    getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
                    getInfoCommand().addTopoInfoEntity(newCoFace, Internal::InfoCommand::CREATED);

                    coedge2cofaces[coedge_0].push_back(newCoFace);

                    // mise à jour de la projection suivant la géom qui a été extrudée
                    updateGeomAssociation(coedge_0->getGeomAssociation(), newCoFace);

                    // maillage directionnel, suivant extrusion
                    FaceMeshingPropertyDirectional* newProp = new FaceMeshingPropertyDirectional(FaceMeshingPropertyDirectional::dir_j);
                    CoFaceMeshingProperty* oldProp = newCoFace->setProperty(newProp);
                    delete oldProp;

                } // end if filtre_vu
            } // end for coedges
        } // end for edges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::constructExtrudeBlocks(std::vector<CoFace*>& cofaces_0,
		std::vector<CoFace*>& cofaces_1,
		std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
		std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
		uint ni)
{

    std::vector<CoFace*>::iterator iter1_0 = cofaces_0.begin();
    std::vector<CoFace*>::iterator iter1_1 = cofaces_1.begin();

    if (cofaces_0.size() != cofaces_1.size())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, constructExtrudeBlocks avec des nombres de CoFaces différents", TkUtil::Charset::UTF_8));

    for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1){

        CoFace* coface_0 = *iter1_0;
        CoFace* coface_1 = *iter1_1;

        std::vector<Face* > faces;
        std::vector<Vertex* > vertices;

        faces.push_back(new Topo::Face(getContext(), coface_0));
        faces.push_back(new Topo::Face(getContext(), coface_1));
        getInfoCommand().addTopoInfoEntity(faces[0], Internal::InfoCommand::CREATED);
        getInfoCommand().addTopoInfoEntity(faces[1], Internal::InfoCommand::CREATED);

        if (!((coface_0->getNbEdges() == 4 && coface_1->getNbEdges() == 4)
        		|| (coface_0->getNbEdges() == 3 && coface_1->getNbEdges() == 3)))
        {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "Erreur interne, la création d'un bloc ne peut se faire pour la face "<<coface_0->getName()
                        		<<", nous n'avons les bons nombres de côtés";
        	throw TkUtil::Exception(messErr);
        }

        uint tabIndVtx[4] = {1, 2, 0, 3};
        // on ordonne les Edges pour être compatible avec l'ordre des Face pour le Block
        uint tabIndFace[4] = {0, 2, 1, 3};

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
        else
        	// cas d'un bloc dégénéré
        	vertices.push_back(coface_1->getVertex(tabIndVtx[2]));

#ifdef _DEBUG2
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
#ifdef _DEBUG2
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
#ifdef _DEBUG2
        				std::cout<<" ratio de "<<ratio<<" reporté de "<<edge_0->getName()<<" vers "<<faces.back()->getName()<<std::endl;
#endif
        				std::vector<CoFace* >& revol_cofaces = coedge2cofaces[*iter3];
        				for (std::vector<CoFace* >::iterator iter4 = revol_cofaces.begin();
        						iter4 != revol_cofaces.end(); ++iter4)
        					faces.back()->setRatio(*iter4, ratio, 1);
        			}
        		}

//        		// prise en compte de la semi-conformité suivant la révolution
//        		computeFaceRatio(faces.back(), 0, ni_loc_coface);

        	} // end if (!face_cofaces.empty())

        } // end for i<coface_0->getNbEdges()

        Block* newBlock = new Topo::Block(getContext(), faces, vertices, true);
        getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
        updateGeomAssociation(coface_0->getGeomAssociation(), newBlock);
#ifdef _DEBUG2
        std::cout<<"\nCréation du bloc : "<<*newBlock;
        //newBlock->check();
#endif

		BlockMeshingProperty* new_ppty = new BlockMeshingPropertyDirectional(BlockMeshingProperty::dir_i);
		newBlock->setMeshLaw(new_ppty);

    } // for (; iter1_0 != cofaces_0.end(); ++iter1_0, ++iter1_1)


}
/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeTopo::
updateGeomAssociation(Geom::GeomEntity* ge, CoEdge* coedge)
{
    //std::cout<<"updateGeomAssociation("<<ge->getName()<<", "<<coedge->getName()<<")\n";
//    coedge->saveTopoProperty();
//    getInfoCommand().addTopoInfoEntity(coedge,Internal::InfoCommand::DISPMODIFIED);

    Geom::Vertex* vtx = dynamic_cast<Geom::Vertex*>(ge);
    if (vtx){
    	//std::cout<<"setGeomAssociation "<<vtx->getName()<<" => "<<(m_commandGeom->getAssociationV2C()[vtx]?m_commandGeom->getAssociationV2C()[vtx]->getName():"vide")<<std::endl;
        coedge->setGeomAssociation(m_commandGeom->getAssociationV2C()[vtx]);
    }
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
void CommandExtrudeTopo::
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
void CommandExtrudeTopo::
updateGeomAssociation(Geom::GeomEntity* ge, Block* block)
{
//    block->saveTopoProperty();
//    getInfoCommand().addTopoInfoEntity(block,Internal::InfoCommand::DISPMODIFIED);

    Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
    if (surf)
        block->setGeomAssociation(m_commandGeom->getAssociationS2V()[surf]);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
