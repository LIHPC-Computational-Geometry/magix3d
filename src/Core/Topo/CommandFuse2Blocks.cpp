/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Blocks.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11/6/2015
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandFuse2Blocks.h"
#include "Topo/TopoHelper.h"

#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_FUSE
/*----------------------------------------------------------------------------*/
CommandFuse2Blocks::
CommandFuse2Blocks(Internal::Context& c, Block* bl_A, Block* bl_B)
:CommandEditTopo(c, std::string("Collage entre blocs ")
                    + bl_A->getName() + " et " + bl_B->getName())
, m_bl_A(bl_A)
, m_bl_B(bl_B)
{
	if (!bl_A->isStructured() || !bl_B->isStructured())
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage n'est possible qu'avec des blocs structurés", TkUtil::Charset::UTF_8));

	if (bl_A->getNbVertices()!=8 || bl_B->getNbVertices()!=8)
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage n'est possible qu'avec des blocs non dégénérés", TkUtil::Charset::UTF_8));

	if (bl_A->getGeomAssociation() != bl_B->getGeomAssociation())
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage n'est possible qu'avec des blocs associés à un même volume", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandFuse2Blocks::
~CommandFuse2Blocks()
{
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Blocks::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandFuse2Blocks::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // il faut rechercher dans un premier temps les Faces concernées dans chacun des blocs
    Face* faceA = getNearestFace(m_bl_A, m_bl_B->getBarycentre());
    Face* faceB = getNearestFace(m_bl_B, m_bl_A->getBarycentre());
    // vérification : les 2 faces doivent avoir les même sommets
    std::vector<Topo::Vertex*> verticesFA;
    std::vector<Topo::Vertex*> verticesFB;
    faceA->getVertices(verticesFA);
    faceB->getVertices(verticesFB);
    if (!TopoHelper::haveSame(verticesFA, verticesFB))
    	throw TkUtil::Exception (TkUtil::UTF8String ("On ne trouve pas de face en commun (avec les même sommets) pour les 2 blocs", TkUtil::Charset::UTF_8));

    // recherche des 8 sommets pour le nouveau bloc
    uint idA = m_bl_A->getIndex(faceA); // indice de la face dans le bloc A
    uint idB = m_bl_B->getIndex(faceB); // indice de la face dans le bloc B
    uint idA_op = ((idA/2)*2+(idA%2+1)%2); // indice de la face opposée
    uint idB_op = ((idB/2)*2+(idB%2+1)%2); // indice de la face opposée

#ifdef _DEBUG_FUSE
    std::cout<<"A "<<m_bl_A->getName()<<", B "<<m_bl_A->getName()<<std::endl;
    std::cout<<" idA = "<<idA<<std::endl;
    std::cout<<" idA_op = "<<idA_op<<std::endl;
    std::cout<<" idB = "<<idB<<std::endl;
    std::cout<<" idB_op = "<<idB_op<<std::endl;
    std::cout<<" verticesFA :";
    for (uint i=0; i<4; i++)
    	std::cout<<" "<<verticesFA[i]->getName();
    std::cout<<std::endl;
    std::cout<<" verticesFB :";
    for (uint i=0; i<4; i++)
    	std::cout<<" "<<verticesFB[i]->getName();
    std::cout<<std::endl;
#endif

    std::vector<Topo::Vertex*> newVertices;
    for (uint i=0; i<4; i++)
    	newVertices.push_back(TopoHelper::getOppositeVertex(m_bl_A, verticesFA[i], idA/2, idA%2));
    for (uint i=0; i<4; i++)
    	newVertices.push_back(TopoHelper::getOppositeVertex(m_bl_B, verticesFA[i], idB/2, idB%2));

#ifdef _DEBUG_FUSE
    std::cout<<" newVertices :";
    for (uint i=0; i<8; i++)
    	std::cout<<" "<<newVertices[i]->getName();
    std::cout<<std::endl;
#endif


    // création des 6 Faces à partir de celles des 2 blocs
    // 2 d'inchangées (face0 et face1)
    // 4 qui sont l'union des couples de faces (couples faces1[i] et faces2[i])

    Face* face0 = m_bl_A->getFace(newVertices[0], newVertices[1], newVertices[2], newVertices[3]);
    Face* face1 = m_bl_B->getFace(newVertices[4], newVertices[5], newVertices[6], newVertices[7]);

    Face* faces1[4];
    Face* faces2[4];
    for (uint i=0; i<4; i++){
    	uint id1 = TopoHelper::tabIndVtxByEdgeOnAlternateFace[i][0];
    	uint id2 = TopoHelper::tabIndVtxByEdgeOnAlternateFace[i][1];
    	faces1[i] = m_bl_A->getFace(newVertices[id1], newVertices[id2], verticesFA[id1], verticesFA[id2]);
    	faces2[i] = m_bl_B->getFace(newVertices[4+id1], newVertices[4+id2], verticesFA[id1], verticesFA[id2]);
    }

    std::vector<Face*> newFaces;
    newFaces.push_back(face0);
    newFaces.push_back(face1);
    for (uint i=0; i<4; i++){
    	uint id1 = TopoHelper::tabIndVtxByEdgeOnAlternateFace[i][0];
    	uint id2 = TopoHelper::tabIndVtxByEdgeOnAlternateFace[i][1];
    	std::vector<Vertex*> loc_vtx;
    	std::vector<CoFace*> loc_cofaces;
    	loc_vtx.push_back(newVertices[id1]);
    	loc_vtx.push_back(newVertices[id2]);
    	loc_vtx.push_back(newVertices[4+id2]);
    	loc_vtx.push_back(newVertices[4+id1]);
    	for (uint j=0; j<faces1[i]->getNbCoFaces(); j++)
    		loc_cofaces.push_back(faces1[i]->getCoFace(j));
    	for (uint j=0; j<faces2[i]->getNbCoFaces(); j++)
    		loc_cofaces.push_back(faces2[i]->getCoFace(j));
    	newFaces.push_back(new Face(getContext(), loc_cofaces, loc_vtx, true));
        getInfoCommand().addTopoInfoEntity(newFaces.back(), Internal::InfoCommand::CREATED);
    }

#ifdef _DEBUG_FUSE
    std::cout<<"  face0 : "<<face0->getName()<<std::endl;
    std::cout<<"  face1 : "<<face1->getName()<<std::endl;
    for (uint i=0; i<4; i++)
    	std::cout<<"  "<<faces1[i]->getName()<<" avec "<<faces2[i]->getName()
		         <<" => "<<newFaces[i+2]->getName()<<std::endl;
#endif

    // on réordonne les sommets en cohérence avec les faces pour la structuration
    std::vector<Topo::Vertex*> loc_vertices;
    loc_vertices.push_back(newVertices[0]);
    loc_vertices.push_back(newVertices[4]);
    loc_vertices.push_back(newVertices[3]);
    loc_vertices.push_back(newVertices[7]);
    loc_vertices.push_back(newVertices[1]);
    loc_vertices.push_back(newVertices[5]);
    loc_vertices.push_back(newVertices[2]);
    loc_vertices.push_back(newVertices[6]);

    // construction du bloc union des 2 précédents
    Block* newBlock = new Block(getContext(), newFaces, loc_vertices, true);
    getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);
    newBlock->setGeomAssociation(m_bl_A->getGeomAssociation());

    // destruction des faces et cofaces entre les 2
    std::vector<CoFace*> loc_cofaces;
    faceA->getCoFaces(loc_cofaces);
    for (uint i=0; i<loc_cofaces.size(); i++)
    	loc_cofaces[i]->free(&getInfoCommand());
    faceB->getCoFaces(loc_cofaces);
    for (uint i=0; i<loc_cofaces.size(); i++)
    	loc_cofaces[i]->free(&getInfoCommand());

    // destructions des faces inutiles
    faceA->free(&getInfoCommand());
    faceB->free(&getInfoCommand());
    for (uint i=0; i<4; i++)
    	faces1[i]->free(&getInfoCommand());
    for (uint i=0; i<4; i++)
    	faces2[i]->free(&getInfoCommand());

    // destruction des 2 blocs
    m_bl_A->free(&getInfoCommand());
    m_bl_B->free(&getInfoCommand());

    // bloc valide ?
    newBlock->check();

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Blocks::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
Face* CommandFuse2Blocks::getNearestFace(Block* bl, Utils::Math::Point pt)
{
	uint id_best = 0;
	double dist2 = bl->getFace(0)->getBarycentre().length2(pt);
	uint nb_faces = bl->getNbFaces();
	for (uint i=1; i<nb_faces; i++){
		double d2 = bl->getFace(i)->getBarycentre().length2(pt);
		if (d2<dist2){
			dist2 = d2;
			id_best = i;
		}
	}
	return bl->getFace(id_best);
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Blocks::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
