/*----------------------------------------------------------------------------*/
/** \file CommandProjectFacesOnSurfaces.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/11/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandProjectFacesOnSurfaces.h"
#include "Geom/EntityFactory.h"
#include "Topo/TopoManager.h"
#include "Topo/CoEdge.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"

#include "Utils/Common.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Curve.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
CommandProjectFacesOnSurfaces::
CommandProjectFacesOnSurfaces(Internal::Context& c,
		std::vector<CoFace*> &cofaces)
:CommandEditTopo(c, "Projection automatique pour des faces topologiques")
, m_cofaces(cofaces.begin(), cofaces.end())
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	comments << "Projection automatique pour les faces topologiques";

	for (uint i=0; i<cofaces.size() && i<5; i++)
		comments << " " << cofaces[i]->getName();
	if (cofaces.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandProjectFacesOnSurfaces::
CommandProjectFacesOnSurfaces(Internal::Context& c)
:CommandEditTopo(c, "Projection automatique pour toutes les faces topologiques")
{
	std::vector<Topo::CoFace* > faces;
	c.getLocalTopoManager().getCoFaces(faces);
	m_cofaces.insert(m_cofaces.end(), faces.begin(), faces.end());
}
/*----------------------------------------------------------------------------*/
CommandProjectFacesOnSurfaces::
~CommandProjectFacesOnSurfaces()
{}
/*----------------------------------------------------------------------------*/
void CommandProjectFacesOnSurfaces::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandProjectFacesOnSurfaces::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( );

	// modification de la projection, avec gestion du undo/redo
	for (std::vector<CoFace*>::iterator iter = m_cofaces.begin();
	        iter != m_cofaces.end(); ++iter){
		CoFace* coface = *iter;

		Geom::Surface* surface = Internal::EntitiesHelper::getCommonSurface(coface);

		if (surface)
			project(coface, surface);
	}

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandProjectFacesOnSurfaces::project(CoFace* coface, Geom::Surface* surface)
{
	getInfoCommand().addTopoInfoEntity(coface, Internal::InfoCommand::DISPMODIFIED);
	coface->saveTopoProperty();
	coface->setGeomAssociation(surface);

	// on passe en mode transfini dès lors que l'on change l'association
	if (coface->getMeshLaw() < Topo::CoFaceMeshingProperty::transfinite){
		coface->saveCoFaceMeshingProperty(&getInfoCommand());
		CoFaceMeshingProperty* prop = new FaceMeshingPropertyTransfinite();
		coface->switchCoFaceMeshingProperty(&getInfoCommand(), prop);
		delete prop;
	}
}
/*----------------------------------------------------------------------------*/
void CommandProjectFacesOnSurfaces::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
