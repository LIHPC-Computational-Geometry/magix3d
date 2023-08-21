/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetFaceMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/10/13
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetFaceMeshingProperty.h"
#include "Topo/FaceMeshingPropertyOrthogonal.h"

#include "Utils/Common.h"
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
CommandSetFaceMeshingProperty::
CommandSetFaceMeshingProperty(Internal::Context& c, CoFaceMeshingProperty& emp, CoFace* fa)
:CommandEditTopo(c, std::string("Changement de discrétisation pour ") + fa->getName())
, m_prop(emp.clone())
{
	m_cofaces.push_back(fa);
}
/*----------------------------------------------------------------------------*/
CommandSetFaceMeshingProperty::
CommandSetFaceMeshingProperty(Internal::Context& c, CoFaceMeshingProperty& emp,
		std::vector<CoFace*>& cofaces)
:CommandEditTopo(c, std::string("Changement de discrétisation pour des faces"))
, m_prop(emp.clone())
, m_cofaces(cofaces)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Changement de discrétisation pour les faces";
	for (uint i=0; i<cofaces.size() && i<5; i++)
		comments << " " << cofaces[i]->getName();
	if (cofaces.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSetFaceMeshingProperty::
~CommandSetFaceMeshingProperty()
{
    delete m_prop;
}
/*----------------------------------------------------------------------------*/
void CommandSetFaceMeshingProperty::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSetFaceMeshingProperty::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    for (std::vector<CoFace*>::iterator iter = m_cofaces.begin(); iter!=m_cofaces.end(); ++iter){

    	CoFace* coface = *iter;

    	// remplace la propriété (fait une copie) et met l'ancienne dans la sauvegarde interne
    	coface->switchCoFaceMeshingProperty(&getInfoCommand(), m_prop);

    	// initialise la direction si nécessaire
    	coface->getCoFaceMeshingProperty()->initDir(coface);

    	// transmission de la demande d'orthogonalité aux arêtes
    	if (m_prop->getMeshLaw() == CoFaceMeshingProperty::orthogonal){
    		FaceMeshingPropertyOrthogonal* mp = dynamic_cast<FaceMeshingPropertyOrthogonal*>(coface->getCoFaceMeshingProperty());
    		CHECK_NULL_PTR_ERROR(mp);

    		setOrthogonalCoEdges(coface, mp);

    	} // end if (m_prop->getMeshLaw() == CoFaceMeshingProperty::orthogonal)
    }

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSetFaceMeshingProperty::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
