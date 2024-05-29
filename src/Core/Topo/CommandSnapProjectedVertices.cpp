/*----------------------------------------------------------------------------*/
/*
 * \file CommandSnapProjectedVertices.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSnapProjectedVertices.h"

#include "Utils/Common.h"
#include "Topo/Vertex.h"

#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

// OCC
#include <Standard_Failure.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandSnapProjectedVertices::
CommandSnapProjectedVertices(Internal::Context& c,
        std::vector<Vertex*> & vertices)
:CommandEditTopo(c, "Déplace des sommets topologiques suivant la projection sur la géométrie")
, m_vertices(vertices.begin(), vertices.end())
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Déplace les sommets topologiques ";
	for (uint i=0; i<vertices.size() && i<5; i++)
		comments << " " << vertices[i]->getName();
	if (vertices.size()>5)
		comments << " ... ";
	comments << " suivant la projection sur la géométrie";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSnapProjectedVertices::
CommandSnapProjectedVertices(Internal::Context& c)
:CommandEditTopo(c, "Déplace tous les sommets topologiques suivant la projection sur la géométrie")
{
    std::vector<Topo::Vertex* > vertices;

    c.getLocalTopoManager().getVertices(vertices);

    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
}
/*----------------------------------------------------------------------------*/
CommandSnapProjectedVertices::
~CommandSnapProjectedVertices()
{
}
/*----------------------------------------------------------------------------*/
void CommandSnapProjectedVertices::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSnapProjectedVertices::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    for (std::vector<Vertex*>::iterator iter = m_vertices.begin();
            iter != m_vertices.end(); ++iter){

        Vertex* ve = *iter;

        Utils::Math::Point pt1 = ve->getCoord();
        Utils::Math::Point pt2;

        if (ve->getGeomAssociation()){
            Geom::GeomEntity* ge = ve->getGeomAssociation();
            if (ge->getType() == Utils::Entity::GeomSurface
                    || ge->getType() == Utils::Entity::GeomCurve
                    || ge->getType() == Utils::Entity::GeomVertex){
                try {
                    ge->project(pt1, pt2);
                }
                catch (const Standard_Failure& exc){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "OCC a échoué, projection impossible pour le sommet "
                            << ve->getName() << ", sur "<<ge->getName();
                    throw TkUtil::Exception (message);
                }
            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << "Projection impossible pour le sommet \""
                        << ve->getName() << "\", \n";
                message << "il est projetée sur autre chose qu'un sommet, une courbe ou une surface";
                throw TkUtil::Exception (message);
            }

            if (!(pt1 == pt2)){
                ve->saveVertexGeomProperty(&getInfoCommand(), true);
                ve->setCoord(pt2);
            } // end if (pt1 != pt2)

        } // end if (ve->getGeomAssociation())

    } // end for iter

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSnapProjectedVertices::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
