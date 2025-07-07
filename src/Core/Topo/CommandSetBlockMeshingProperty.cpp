/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetBlockMeshingProperty.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
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

    for (Block* block : m_blocks){

    	// cas avec passage d'une méthode non-structurée vers une méthode structurée
    	if (!block->isStructured() && m_prop->isStructured()) {
    		// vérification que les faces sont structurées
    		for (CoFace* coface : block->getCoFaces())
    			if (!coface->isStructured()){
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Il n'est pas possible de rendre le bloc "<<block->getName()
    				        << " structuré car sa face " <<coface->getName()
    				        <<" n'est pas structurée";
    				throw TkUtil::Exception(messErr);
    			}

    		// on va certainement modifier l'ordre des faces et des sommets
    		block->saveBlockMeshingProperty(&getInfoCommand());

    		Face* face_base = 0;
    		if (block->getFaces().size()==4 || block->getFaces().size()==5){
    			// recherche d'une face à l'opposé de la dégénérence

    			// filtre pour marquer à 1 les sommets qui sont à la dégénérence d'une face
    			std::map<Topo::Vertex*, uint> filtre_sommets;
    			for (Face* face : block->getFaces()){
    				if (face->getNbVertices()==3)
    					filtre_sommets[face->getVertex(0)] = 1;
    			}

    			// recherche une face dont aucun sommet n'est marqué
    			for (Face* face : block->getFaces()){
    				bool acceptable = true;
    				for (Vertex* vtx : face->getVertices())
    					if (filtre_sommets[vtx] == 1 )
    						acceptable = false;
    				if (acceptable)
    					face_base = face;
    			}

    		}
    		else if (block->getFaces().size()==6){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Il n'est pas possible de rendre le bloc "<<block->getName()
    			    	<< " structuré car il a " <<(short)block->getFaces().size()
    			    	<<" faces et non 6 (voir 4 ou 5 pour cas dégénérés)";
    			throw TkUtil::Exception(messErr);
    		} else {
    			// cas à 6 faces, on prend la première venue
    			face_base = block->getFace(0);
    		}

    		if (face_base == 0){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Il n'est pas possible de rendre le bloc "<<block->getName()
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
     		for (Vertex* vtx : sorted_vertices)
     			filtre_sommets[vtx] = 2;


     		// recherche le sommet suivant pour une face dans un bloc avec 2 sommets non marqués à 2
     		for (uint i=0; i<4; i++){
     			Vertex* vtx_suiv = getNextVertex(block,
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
     			messErr << "Erreur interne pour le bloc "<<block->getName()
     			    	<< ", le nombre de sommets pour bloc hexa n'est pas de 8 mais de "
     			    	<<(short)hexa_vertices.size() ;
     			throw TkUtil::Exception(messErr);
     		}

     		if (block->getVertices().size() != sorted_vertices.size()){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
     			messErr << "Erreur interne pour le bloc "<<block->getName()
     			    	<< ", le nombre de sommets a changé ("
     			    	<<(short)sorted_vertices.size() <<" trouvés au lieu de "
     			    	<<(short)block->getVertices().size()<<")";
     			throw TkUtil::Exception(messErr);
     		}

     		// change l'ordre des sommets
     		block->m_topo_property->getVertexContainer().clear();
    		block->m_topo_property->getVertexContainer().insert(
				block->m_topo_property->getVertexContainer().end(),
				sorted_vertices.begin(),
				sorted_vertices.end());

     		//	réordonne les faces en fonction des sommets
    		std::vector<Face* > sorted_faces;

    	    for (uint i=0; i<block->getFaces().size(); i++){
    	        sorted_faces.push_back(block->getFace(
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][0]],
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][1]],
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][2]],
    	        		hexa_vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][3]]));
    	    }

    	    block->m_topo_property->getFaceContainer().clear();
    	    block->m_topo_property->getFaceContainer().insert(
				block->m_topo_property->getFaceContainer().end(),
				sorted_faces.begin(),
				sorted_faces.end());

    	} // end if (!block->isStructured() && m_prop->isStructured())


    	// remplace la propriété (fait une copie) et met l'ancienne dans la sauvegarde interne
    	block->switchBlockMeshingProperty(&getInfoCommand(), m_prop);

    	// initialise la direction si nécessaire
    	block->getBlockMeshingProperty()->initDir(block);

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
	for (Face* face : bloc->getFaces()){
		//std::cout<<"face : "<<face->getName()<<std::endl;
		uint nb2 = 0; // sommets dans la face de base (autre que les 2 de départ)
		uint nb3 = 0; // sommets qui valide la face avec 2 des sommets sélectionnés
		for (Vertex* vtx : face->getVertices())
			if (filtre_sommets[vtx] == 3)
				nb3 += 1;
			else if (filtre_sommets[vtx] == 2)
				nb2 += 2;
		if (nb3 == 2 && nb2 == 0)
			face_select = face;
	}

	Vertex* vtx_suiv = 0;
	if (face_select){
		uint ind1 = face_select->getIndexOf(vtx1);
		uint ind2 = face_select->getIndexOf(vtx2);
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
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
