/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetBlockMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/10/13
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetBlockMeshingProperty.h"
#include "Topo/BlockMeshingPropertyOrthogonal.h"
#include "Topo/FaceMeshingPropertyOrthogonal.h"
#include "Topo/Block.h"
#include "Topo/TopoHelper.h"

#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandSetBlockMeshingProperty::
CommandSetBlockMeshingProperty(Internal::Context& c, BlockMeshingProperty& emp, Block* bl)
:CommandEditTopo(c, std::string("Changement de discrétisation pour ") + bl->getName())
, m_prop(emp.clone())
{
	m_blocks.push_back(bl);
}
/*----------------------------------------------------------------------------*/
CommandSetBlockMeshingProperty::
CommandSetBlockMeshingProperty(Internal::Context& c, BlockMeshingProperty& emp,
		std::vector<Block*>& blocks)
:CommandEditTopo(c, std::string("Changement de discrétisation pour des blocs"))
, m_prop(emp.clone())
, m_blocks(blocks)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	comments << "Changement de discrétisation pour les blocs";
	for (uint i=0; i<blocks.size() && i<5; i++)
		comments << " " << blocks[i]->getName();
	if (blocks.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSetBlockMeshingProperty::
~CommandSetBlockMeshingProperty()
{
    delete m_prop;
}
/*----------------------------------------------------------------------------*/
void CommandSetBlockMeshingProperty::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSetBlockMeshingProperty::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    for (std::vector<Block*>::iterator iter = m_blocks.begin(); iter!=m_blocks.end(); ++iter){

    	// cas avec passage d'une méthode non-structurée vers une méthode structurée
    	if (!(*iter)->isStructured() && m_prop->isStructured()) {
    		// vérification que les faces sont structurées

    		std::vector<CoFace*> cofaces;
    		(*iter)->getCoFaces(cofaces);

    		for (uint i=0; i<cofaces.size(); i++)
    			if (!cofaces[i]->isStructured()){
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Il n'est pas possible de rendre le bloc "<<(*iter)->getName()
    				        << " structuré car sa face " <<cofaces[i]->getName()
    				        <<" n'est pas structurée";
    				throw TkUtil::Exception(messErr);
    			}

    		// on va certainement modifier l'ordre des faces et des sommets
    		(*iter)->saveBlockMeshingProperty(&getInfoCommand());

    		Face* face_base = 0;
    		if ((*iter)->getNbFaces()==4 || (*iter)->getNbFaces()==5){
    			// recherche d'une face à l'opposé de la dégénérence

    			// filtre pour marquer à 1 les sommets qui sont à la dégénérence d'une face
    			std::map<Topo::Vertex*, uint> filtre_sommets;
    			for (uint i=0; i<(*iter)->getNbFaces(); i++){
    				Face* face=(*iter)->getFace(i);
    				if (face->getNbVertices()==3)
    					filtre_sommets[face->getVertex(0)] = 1;
    			}

    			// recherche une face dont aucun sommet n'est marqué
    			for (uint i=0; i<(*iter)->getNbFaces(); i++){
    				Face* face=(*iter)->getFace(i);
    				std::vector<Vertex*> face_vertices;
    				face->getVertices(face_vertices);
    				bool acceptable = true;
    				for (uint j=0; j<face_vertices.size(); j++)
    					if (filtre_sommets[face_vertices[j]] == 1 )
    						acceptable = false;
    				if (acceptable)
    					face_base = face;
    			}

    		}
    		else if ((*iter)->getNbFaces()==6){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Il n'est pas possible de rendre le bloc "<<(*iter)->getName()
    			    	<< " structuré car il a " <<(short)(*iter)->getNbFaces()
    			    	<<" faces et non 6 (voir 4 ou 5 pour cas dégénérés)";
    			throw TkUtil::Exception(messErr);
    		} else {
    			// cas à 6 faces, on prend la première venue
    			face_base = (*iter)->getFace(0);
    		}

    		if (face_base == 0){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Il n'est pas possible de rendre le bloc "<<(*iter)->getName()
    			    	<< " structuré car on ne trouve pas de face non relié à un sommet de dégénérence";
    			throw TkUtil::Exception(messErr);
    		}
    		//	std::cout<<"  face_base = "<<face_base->getName()<<std::endl;

    		// les sommets réordonnés
     		std::vector<Vertex*> sorted_vertices;
     		// les sommets pour bloc hexaédrique (certains peuvent apparaitrent 2 fois ou plus)
     		std::vector<Vertex*> hexa_vertices;
     		sorted_vertices.push_back(face_base->getVertex(0));
     		sorted_vertices.push_back(face_base->getVertex(1));
     		sorted_vertices.push_back(face_base->getVertex(3));
     		sorted_vertices.push_back(face_base->getVertex(2));
     		hexa_vertices.insert(hexa_vertices.end(), sorted_vertices.begin(), sorted_vertices.end());

     		// marque à 2 les sommets vus (ceux de la face de base)
     		// on marque à 1 ceux vu en dehors de cette face
			std::map<Topo::Vertex*, uint> filtre_sommets;
     		for (uint i=0; i<sorted_vertices.size(); i++)
     			filtre_sommets[sorted_vertices[i]] = 2;


     		// recherche le sommet suivant pour une face dans un bloc avec 2 sommets non marqués à 2
     		for (uint i=0; i<4; i++){
     			Vertex* vtx_suiv = getNextVertex(*iter,
     					face_base->getVertex((i+1)%4),
     					face_base->getVertex(i),
     					filtre_sommets);

     			if (vtx_suiv){
     				hexa_vertices.push_back(vtx_suiv);
     				if (filtre_sommets[vtx_suiv] == 0){
     					sorted_vertices.push_back(vtx_suiv);
     					filtre_sommets[vtx_suiv] = 1;
     				}
    			}
     		} // end for i<4

     		if (sorted_vertices.size() == 8){
     			// on permute les 2 dernier car les sommets de la face sont dans un ordre circulaire
     			// ce qui n'est pas le cas des blocs
     			Vertex* tmp = sorted_vertices[7];
     			sorted_vertices[7] = sorted_vertices[6];
     			sorted_vertices[6] = tmp;
     		}
//    	    std::cout<<"sorted_vertices => ";
//    	    for (uint i=0; i<sorted_vertices.size(); i++)
//    	    	std::cout<<" "<<sorted_vertices[i]->getName();
//    	    std::cout<<std::endl;
//    	    std::cout<<"hexa_vertices (avant perm) => ";
//    	    for (uint i=0; i<hexa_vertices.size(); i++)
//    	    	std::cout<<" "<<hexa_vertices[i]->getName();
//    	    std::cout<<std::endl;

     		if (hexa_vertices.size() == 8){
     			Vertex* tmp = hexa_vertices[7];
     			hexa_vertices[7] = hexa_vertices[6];
     			hexa_vertices[6] = tmp;
     		}
     		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
     			messErr << "Erreur interne pour le bloc "<<(*iter)->getName()
     			    	<< ", le nombre de sommets pour bloc hexa n'est pas de 8 mais de "
     			    	<<(short)hexa_vertices.size() ;
     			throw TkUtil::Exception(messErr);
     		}

     		if ((*iter)->getNbVertices() != sorted_vertices.size()){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
     			messErr << "Erreur interne pour le bloc "<<(*iter)->getName()
     			    	<< ", le nombre de sommets a changé ("
     			    	<<(short)sorted_vertices.size() <<" trouvés au lieu de "
     			    	<<(short)(*iter)->getNbVertices()<<")";
     			throw TkUtil::Exception(messErr);
     		}

     		// change l'ordre des sommets
     		(*iter)->m_topo_property->getVertexContainer().clear();
    		(*iter)->m_topo_property->getVertexContainer().add(sorted_vertices);

     		//	réordonne les faces en fonction des sommets
    		std::vector<Face* > sorted_faces;

    	    for (uint i=0; i<(*iter)->getNbFaces(); i++){
    	        sorted_faces.push_back((*iter)->getFace(
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][0]],
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][1]],
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][2]],
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][3]]));

    	    }

    	    (*iter)->m_topo_property->getFaceContainer().clear();
    	    (*iter)->m_topo_property->getFaceContainer().add(sorted_faces);


    	} // end if (!(*iter)->isStructured() && m_prop->isStructured())


    	// remplace la propriété (fait une copie) et met l'ancienne dans la sauvegarde interne
    	(*iter)->switchBlockMeshingProperty(&getInfoCommand(), m_prop);

    	// initialise la direction si nécessaire
    	(*iter)->getBlockMeshingProperty()->initDir((*iter));


    	// transmission de la demande d'orthogonalité aux faces
    	if (m_prop->getMeshLaw() == BlockMeshingProperty::orthogonal){
    		BlockMeshingPropertyOrthogonal* mp = dynamic_cast<BlockMeshingPropertyOrthogonal*>((*iter)->getBlockMeshingProperty());
    		CHECK_NULL_PTR_ERROR(mp);

    		// les 8 sommets, quitte à en répéter certains en cas de dégénérecence
    		std::vector<Topo::Vertex* > vertices;
    		(*iter)->getHexaVertices(vertices);

    		if (mp->getDir() == 0){
    			// cas orthogonalité suivant I
    			setOrthogonalCoFaces((*iter)->getFace(2), vertices[0], vertices[1], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(3), vertices[2], vertices[3], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(4), vertices[0], vertices[1], mp->getNbLayers(), mp->getSide());
    			if ((*iter)->getNbFaces() == 6)
    				setOrthogonalCoFaces((*iter)->getFace(5), vertices[4], vertices[5], mp->getNbLayers(), mp->getSide());
    		}
    		else if (mp->getDir() == 1){
    			// cas orthogonalité suivant J
    			setOrthogonalCoFaces((*iter)->getFace(0), vertices[0], vertices[2], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(1), vertices[1], vertices[3], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(4), vertices[0], vertices[2], mp->getNbLayers(), mp->getSide());
    			if ((*iter)->getNbFaces() == 6)
    				setOrthogonalCoFaces((*iter)->getFace(5), vertices[5], vertices[7], mp->getNbLayers(), mp->getSide());
    		}
    		else if (mp->getDir() == 2){
    			// cas orthogonalité suivant K
    			setOrthogonalCoFaces((*iter)->getFace(0), vertices[0], vertices[4], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(1), vertices[3], vertices[7], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(2), vertices[0], vertices[4], mp->getNbLayers(), mp->getSide());
    			setOrthogonalCoFaces((*iter)->getFace(3), vertices[3], vertices[7], mp->getNbLayers(), mp->getSide());
    		}

    	} // end if (m_prop->getMeshLaw() == BlockMeshingProperty::orthogonal)

    } // end for (std::vector<Block*>::iterator iter = m_blocks.begin() ...

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}

/*----------------------------------------------------------------------------*/
Vertex* CommandSetBlockMeshingProperty::
getNextVertex(Block* bloc, Vertex* vtx1, Vertex* vtx2, std::map<Topo::Vertex*, uint>& filtre_sommets)
{
	//std::cout<<"getNextVertex("<<vtx1->getName()<<","<<vtx2->getName()<<")"<<std::endl;
	filtre_sommets[vtx1] = 3;
	filtre_sommets[vtx2] = 3;

	Face* face_select = 0;
	for (uint i=0; i<bloc->getNbFaces(); i++){
		Face* face=bloc->getFace(i);
		//std::cout<<"face : "<<face->getName()<<std::endl;
		std::vector<Vertex*> face_vertices;
		face->getVertices(face_vertices);
		uint nb2 = 0; // sommets dans la face de base (autre que les 2 de départ)
		uint nb3 = 0; // sommets qui valide la face avec 2 des sommets sélectionnés
		for (uint j=0; j<face_vertices.size(); j++)
			if (filtre_sommets[face_vertices[j]] == 3)
				nb3 += 1;
			else if (filtre_sommets[face_vertices[j]] == 2)
				nb2 += 2;
		if (nb3 == 2 && nb2 == 0)
			face_select = face;
	}

	Vertex* vtx_suiv = 0;
	if (face_select){
		uint ind1 = face_select->getIndex(vtx1);
		uint ind2 = face_select->getIndex(vtx2);
		uint ind3;
		uint nbVtx = face_select->getNbVertices();
		if (ind2==((ind1+1)%nbVtx))
			ind3 = (ind2+1)%nbVtx;
		else
			ind3 = (ind2-1+nbVtx)%nbVtx;
		//std::cout<<"ind1 = "<<ind1<<", ind2 = "<<ind2<<", ind3 = "<<ind3<<std::endl;
		vtx_suiv = face_select->getVertex(ind3);
//		std::cout<<"  face_select : "<<face_select->getName()
//				<<", vtx_suiv : "<< vtx_suiv->getName()<<std::endl;
	}

	filtre_sommets[vtx1] = 2;
	filtre_sommets[vtx2] = 2;
	return vtx_suiv;
}
/*----------------------------------------------------------------------------*/
void CommandSetBlockMeshingProperty::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
void CommandSetBlockMeshingProperty::setOrthogonalCoFaces(Face* face, Vertex* vtx1, Vertex* vtx2, int nbLayers, bool reverse)
{
	if (face->getNbCoFaces() == 1){
		CoFace* coface = face->getCoFace(0);

		Utils::Math::Point v2, v1;
		if (reverse){
			v2 = vtx1->getCoord();
			v1 = vtx2->getCoord();
		}
		else {
			v1 = vtx1->getCoord();
			v2 = vtx2->getCoord();
		}

		FaceMeshingPropertyOrthogonal* mp = new FaceMeshingPropertyOrthogonal(v1, v2, nbLayers);
		mp->initDir(coface); // initialisation avant ==

		if ((*mp) != (*coface->getCoFaceMeshingProperty())) {
			coface->switchCoFaceMeshingProperty(&getInfoCommand(), mp);

			setOrthogonalCoEdges(coface, mp);
		}
		delete mp;
	}
	else {
		MGX_NOT_YET_IMPLEMENTED("Propagation orthogonalité pour plusieurs faces sur bord de bloc");
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"Propagation orthogonalité pour face "<<face->getName()<<" sur bord d'un bloc n'est pas implémentée";
		message <<"Cette face est composée de plusieurs faces communes";
		getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
