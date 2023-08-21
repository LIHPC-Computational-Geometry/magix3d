/*----------------------------------------------------------------------------*/
/** \file CommandProjectVerticesOnNearestGeomEntities.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/11/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandProjectVerticesOnNearestGeomEntities.h"
#include "Geom/EntityFactory.h"
#include "Topo/TopoManager.h"
#include "Topo/Vertex.h"

#include "Utils/Common.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
#include "Geom/GeomEntity.h"

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
/*----------------------------------------------------------------------------*/
CommandProjectVerticesOnNearestGeomEntities::
CommandProjectVerticesOnNearestGeomEntities(Internal::Context& c,
		std::vector<Vertex*> &vertices,
		std::vector<Geom::GeomEntity*> & geom_entities,
		bool move_vertices)
:CommandEditTopo(c, "Projection automatique pour des sommets topologiques")
, m_vertices(vertices.begin(), vertices.end())
, m_geom_entities(geom_entities.begin(), geom_entities.end())
, m_move_vertices(move_vertices)
{

	if (geom_entities.empty()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"La projection automatique pour des sommets topologiques ne peut se faire sans une liste d'entités géométriques";
		throw TkUtil::Exception(message);
	}

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	comments << "Projection automatique pour les sommets topologiques";

	for (uint i=0; i<vertices.size() && i<5; i++)
		comments << " " << vertices[i]->getName();
	if (vertices.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandProjectVerticesOnNearestGeomEntities::
~CommandProjectVerticesOnNearestGeomEntities()
{}
/*----------------------------------------------------------------------------*/
void CommandProjectVerticesOnNearestGeomEntities::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandProjectVerticesOnNearestGeomEntities::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( );

	// modification de la projection, avec gestion du undo/redo
	for (std::vector<Vertex*>::iterator iter = m_vertices.begin();
	        iter != m_vertices.end(); ++iter){
		Vertex* vtx = *iter;
		Geom::GeomEntity* ge_min = 0;
		double dist_min = DBL_MAX;

		for (std::vector<Geom::GeomEntity*>::iterator iter2 = m_geom_entities.begin();
				iter2 != m_geom_entities.end(); ++iter2){
			Geom::GeomEntity* ge = *iter2;
			Utils::Math::Point pt1 = vtx->getCoord();
			Utils::Math::Point pt2;
			ge->project(pt1, pt2);
			double dist = pt1.length2(pt2);

			if (dist<dist_min){
				dist_min = dist;
				ge_min = ge;
			}
		}

		CHECK_NULL_PTR_ERROR(ge_min);
		project(vtx, ge_min);
	}

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
///*----------------------------------------------------------------------------*/
void CommandProjectVerticesOnNearestGeomEntities::project(Vertex* vtx, Geom::GeomEntity* ge)
{
	vtx->saveTopoProperty();
	vtx->setGeomAssociation(ge);

	// déplace le sommet topologique si la projection implique un déplacement
	if (m_move_vertices){
		Utils::Math::Point posTopo = vtx->getCoord();
		Utils::Math::Point posGeom;
		ge->project(posTopo, posGeom);
		if (!posTopo.isEpsilonEqual(posGeom, Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon)){
			vtx->saveVertexGeomProperty(&getInfoCommand(), true);
			vtx->setCoord(posGeom);
		}

		// si on se retrouve à proximité d'un sommet géométrique
		// on peut alors associer ce sommet topo à ce dernier
		std::vector<Geom::Vertex*> vertices;
		ge->get(vertices);
		for (uint i=0; i<vertices.size(); i++){
			Utils::Math::Point posVtx = vertices[i]->getCenteredPosition();
			if (posGeom.isEpsilonEqual(posVtx, Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon))
				vtx->setGeomAssociation(vertices[i]);
		}
	}
	getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandProjectVerticesOnNearestGeomEntities::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
