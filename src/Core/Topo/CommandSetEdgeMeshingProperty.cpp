/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetEdgeMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetEdgeMeshingProperty.h"

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
CommandSetEdgeMeshingProperty::
CommandSetEdgeMeshingProperty(Internal::Context& c, CoEdgeMeshingProperty& emp, CoEdge* ed)
:CommandEditTopo(c, std::string("Changement de discrétisation pour ") + ed->getName())
, m_prop(emp.clone())
{
	m_coedges.push_back(ed);
}
/*----------------------------------------------------------------------------*/
CommandSetEdgeMeshingProperty::
CommandSetEdgeMeshingProperty(Internal::Context& c, CoEdgeMeshingProperty& emp,
		std::vector<CoEdge*>& coedges)
:CommandEditTopo(c, std::string("Changement de discrétisation pour des arêtes"))
, m_prop(emp.clone())
, m_coedges(coedges)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	comments << "Changement de discrétisation pour les arêtes";
	for (uint i=0; i<coedges.size() && i<5; i++)
		comments << " " << coedges[i]->getName();
	if (coedges.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSetEdgeMeshingProperty::
~CommandSetEdgeMeshingProperty()
{
    delete m_prop;
}
/*----------------------------------------------------------------------------*/
void CommandSetEdgeMeshingProperty::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSetEdgeMeshingProperty::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // remplace la propriété (fait une copie) et met l'ancienne dans la sauvegarde interne
    for (std::vector<CoEdge*>::iterator iter = m_coedges.begin(); iter!=m_coedges.end(); ++iter){
    	(*iter)->switchCoEdgeMeshingProperty(&getInfoCommand(), m_prop);
    	if (m_prop->needGeomUpdate())
    		(*iter)->getMeshingProperty()->update((*iter)->getGeomAssociation());
    }

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSetEdgeMeshingProperty::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
