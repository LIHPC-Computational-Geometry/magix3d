/*----------------------------------------------------------------------------*/
/*
 * \file CommandGlue2Blocks.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/3/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandGlue2Blocks.h"

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
CommandGlue2Blocks::
CommandGlue2Blocks(Internal::Context& c, Block* bl_A, Block* bl_B)
:CommandEditTopo(c, std::string("Fusion entre blocs ")
                    + bl_A->getName() + " et " + bl_B->getName())
, m_bl_A(bl_A)
, m_bl_B(bl_B)
{
}
/*----------------------------------------------------------------------------*/
CommandGlue2Blocks::
~CommandGlue2Blocks()
{
}
/*----------------------------------------------------------------------------*/
void CommandGlue2Blocks::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandGlue2Blocks::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // il faut rechercher dans un premier temps les Faces concernées dans chacun des blocs
    Face* faceA = getNearestFace(m_bl_A, m_bl_B->getBarycentre());
    Face* faceB = getNearestFace(m_bl_B, m_bl_A->getBarycentre());

    std::vector<CoFace*> cofaces_A;
    std::vector<CoFace*> cofaces_B;

    faceA->getCoFaces(cofaces_A);
    faceB->getCoFaces(cofaces_B);

    if (cofaces_A.size() > cofaces_B.size()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandGlue2Blocks impossible entre les blocs "<<m_bl_A->getName()<<" et "<<m_bl_B->getName()
        		<<", il y a plus de faces commune dans "
        		<<faceA->getName()<<" que dans "<<faceB->getName();
        throw TkUtil::Exception(message);
    }

    for (uint i=0; i<cofaces_A.size(); i++){
    	CoFace* coface_A = cofaces_A[i];
    	// recherche de la coface la plus proche en face
    	CoFace* coface_B = getNearestCoFace(cofaces_B, coface_A->getBarycentre());

    	//std::cout<<"CommandGlue2Blocks => fuse (A) "<<coface_A->getName()<<" avec (B) "<<coface_B->getName()<<std::endl;

    	coface_A->fuse(coface_B, &getInfoCommand());
    }

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandGlue2Blocks::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
Face* CommandGlue2Blocks::getNearestFace(Block* bl, Utils::Math::Point pt)
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
CoFace* CommandGlue2Blocks::getNearestCoFace(std::vector<CoFace*>& cofaces, Utils::Math::Point pt)
{
	uint id_best = 0;
	double dist2 = cofaces[0]->getBarycentre().length2(pt);

	for (uint i=1; i<cofaces.size(); i++){
		double d2 = cofaces[i]->getBarycentre().length2(pt);
		if (d2<dist2){
			dist2 = d2;
			id_best = i;
		}
	}
	return cofaces[id_best];
}
/*----------------------------------------------------------------------------*/
void CommandGlue2Blocks::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
