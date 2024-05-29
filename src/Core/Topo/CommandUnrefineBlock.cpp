/*----------------------------------------------------------------------------*/
/*
 * \file CommandUnrefineBlock.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/3/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandUnrefineBlock.h"

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
CommandUnrefineBlock::
CommandUnrefineBlock(Internal::Context& c, Block* bloc, CoEdge* arete, int ratio)
:CommandEditTopo(c, std::string("Déraffinement du bloc ") + bloc->getName())
, m_bloc(bloc)
, m_ratio(ratio)
{

    if (ratio<=0)
         throw TkUtil::Exception (TkUtil::UTF8String ("Le déraffinement ne peut se faire qu'avec un ratio entier strictement positif", TkUtil::Charset::UTF_8));

    // on recherche dans le bloc la direction désignée par l'arête
    m_dir = m_bloc->getDir(arete);
}
/*----------------------------------------------------------------------------*/
CommandUnrefineBlock::
~CommandUnrefineBlock()
{
}
/*----------------------------------------------------------------------------*/
void CommandUnrefineBlock::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandUnrefineBlock::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    if (!m_bloc->isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Le bloc doit être structuré pour être déraffiné avec cette méthode", TkUtil::Charset::UTF_8));

    // la modification de la topologie
    m_bloc->unrefine(m_dir, m_ratio, &getInfoCommand());

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandUnrefineBlock::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
