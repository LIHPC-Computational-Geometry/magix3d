/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Faces.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <Topo/CommandFuse2Faces.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandFuse2Faces::
CommandFuse2Faces(Internal::Context& c, CoFace* face_A, CoFace* face_B)
:CommandEditTopo(c, std::string("Fusion entre faces ")
                    + face_A->getName() + " et " + face_B->getName())
, m_face_A(face_A)
, m_face_B(face_B)
{
	if (face_A->getNbBlocks() == 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Fusion de 2 faces impossible avec "<<face_A->getName()<<", cette face est déjà entre 2 blocs";
        throw TkUtil::Exception(message);
	}
	if (face_B->getNbBlocks() == 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Fusion de 2 faces impossible avec "<<face_B->getName()<<", cette face est déjà entre 2 blocs";
        throw TkUtil::Exception(message);
	}
	if (face_A == face_B){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"Fusion d'une face avec elle même interdite";
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
CommandFuse2Faces::
~CommandFuse2Faces()
{
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Faces::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandGlue2Faces::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // fusion avec recherche des sommets les plus proche possible
    m_face_A->fuse(m_face_B, &getInfoCommand());

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Faces::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Faces::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
