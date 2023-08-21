/*----------------------------------------------------------------------------*/
/*
 * \file CommandReverseDirection.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4/2/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandReverseDirection.h"

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
CommandReverseDirection::
CommandReverseDirection(Internal::Context& c,
		std::vector<CoEdge*>& coedges)
:CommandEditTopo(c, std::string("Changement de discrétisation pour des arêtes"))
, m_coedges(coedges)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Changement de discrétisation pour les arêtes ";
	for (uint i=0; i<coedges.size() && i<5; i++)
		comments << " " << coedges[i]->getName();
	if (coedges.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandReverseDirection::
~CommandReverseDirection()
{
}
/*----------------------------------------------------------------------------*/
void CommandReverseDirection::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandReverseDirection::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    for (std::vector<CoEdge*>::iterator iter = m_coedges.begin(); iter!=m_coedges.end(); ++iter){
    	// sauvegarde pour permettre le undo
    	(*iter)->saveCoEdgeMeshingProperty(&getInfoCommand());

    	// change le sens pour la discrétisation
    	CoEdgeMeshingProperty* mp = (*iter)->getMeshingProperty();
    	mp->setDirect(!mp->getDirect());
    }

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandReverseDirection::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
