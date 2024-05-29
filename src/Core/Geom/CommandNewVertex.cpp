/*----------------------------------------------------------------------------*/
/** \file CommandNewVertex.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 15/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewVertex.h"
#include "Geom/GeomProperty.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewVertex::CommandNewVertex(Internal::Context& c,
                             const Utils::Math::Point& p,
                             const std::string& groupName)
:CommandCreateGeom(c, "Création d'un sommet", groupName), m_p(p)
{}
/*----------------------------------------------------------------------------*/
CommandNewVertex::
~CommandNewVertex()
{}
/*----------------------------------------------------------------------------*/
void CommandNewVertex::
internalExecute()
{
    double dx = m_p.getX();
    double dy = m_p.getY();
    double dz = m_p.getZ();
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewVertex::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( )
//		    << std::ios_base::fixed << TkUtil::setprecision (8)
		    << ". P ("
	        << Utils::Math::MgxNumeric::userRepresentation (dx) << ", "
		    << Utils::Math::MgxNumeric::userRepresentation (dy) << ", "
		    << Utils::Math::MgxNumeric::userRepresentation (dz) << ").";

    Vertex* v= EntityFactory(getContext()).newVertex(m_p);
    m_createdEntities.push_back(v);

    // stockage dans le manager géom
    split(v);

    // ajoute le sommet au groupe s'il y en a un de spécifié
    addToGroup(v);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du sommet "<<v->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewVertex::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewVertices(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
