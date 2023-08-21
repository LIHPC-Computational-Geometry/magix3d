/*----------------------------------------------------------------------------*/
/** \file CommandProjectEdgesOnCurves.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/11/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandProjectEdgesOnCurves.h"
#include "Geom/EntityFactory.h"
#include "Topo/TopoManager.h"
#include "Topo/CoEdge.h"

#include "Utils/Common.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/GeomHelper.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandProjectEdgesOnCurves::
CommandProjectEdgesOnCurves(Internal::Context& c,
		std::vector<CoEdge*> &coedges)
:CommandEditTopo(c, "Projection automatique pour des arêtes topologiques")
, m_coedges(coedges.begin(), coedges.end())
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Projection automatique pour les arêtes topologiques";
	for (uint i=0; i<coedges.size() && i<5; i++)
		comments << " " << coedges[i]->getName();
	if (coedges.size()>5)
		comments << " ... ";
	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandProjectEdgesOnCurves::
CommandProjectEdgesOnCurves(Internal::Context& c)
:CommandEditTopo(c, "Projection automatique pour toutes les arêtes topologiques")
{
	std::vector<Topo::CoEdge* > edges;
	c.getLocalTopoManager().getCoEdges(edges);
	m_coedges.insert(m_coedges.end(), edges.begin(), edges.end());
}
/*----------------------------------------------------------------------------*/
CommandProjectEdgesOnCurves::
~CommandProjectEdgesOnCurves()
{}
/*----------------------------------------------------------------------------*/
void CommandProjectEdgesOnCurves::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandProjectEdgesOnCurves::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( );

	// modification de la projection, avec gestion du undo/redo
	for (std::vector<CoEdge*>::iterator iter = m_coedges.begin();
	        iter != m_coedges.end(); ++iter){
		CoEdge* coedge = *iter;

		// recherche des courbes associées au premier sommet topo
		std::vector<Geom::Curve*> curves1;
		Internal::EntitiesHelper::getAssociatedCurves(coedge->getVertex(0), curves1);

		// idem avec le 2ème sommet
		std::vector<Geom::Curve*> curves2;
		Internal::EntitiesHelper::getAssociatedCurves(coedge->getVertex(1), curves2);

		// recherche de la courbe commune entre les deux groupes
		Geom::GeomEntity* ge = Geom::GeomHelper::getCommonCurve(curves1, curves2);

		if (ge == 0){
			// on tente avec une surface
			// recherche des surfaces associées au premier sommet topo
			std::vector<Geom::Surface*> surf1;
			Internal::EntitiesHelper::getAssociatedSurfaces(coedge->getVertex(0), surf1);

			// idem avec le 2ème sommet
			std::vector<Geom::Surface*> surf2;
			Internal::EntitiesHelper::getAssociatedSurfaces(coedge->getVertex(1), surf2);

			// recherche de la surface commune entre les deux groupes
			ge = Geom::GeomHelper::getCommonSurface(surf1, surf2);
		}

		if (ge)
			project(coedge, ge);
	}

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandProjectEdgesOnCurves::project(CoEdge* coedge, Geom::GeomEntity* ge)
{
	getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::DISPMODIFIED);
	coedge->saveTopoProperty();
	coedge->setGeomAssociation(ge);
}
/*----------------------------------------------------------------------------*/
void CommandProjectEdgesOnCurves::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
