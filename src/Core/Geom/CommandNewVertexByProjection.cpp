/*
 * CommandNewVertexByProjectionByProjection.cpp
 *
 *  Created on: 10 juil. 2012
 *      Author: ledouxf
 */
#include "Geom/CommandNewVertexByProjection.h"
#include "Geom/GeomProperty.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewVertexByProjection::CommandNewVertexByProjection(Internal::Context& c,
        const Geom::Vertex* v,
        Geom::Curve* curv,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'un sommet par projection sur une courbe", groupName)
, m_v(v),m_c(curv), m_s(0)
{}
/*----------------------------------------------------------------------------*/
CommandNewVertexByProjection::CommandNewVertexByProjection(Internal::Context& c,
        const Geom::Vertex* v,
		Geom::Surface* surf,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'un sommet par projection sur une surface", groupName)
, m_v(v),m_c(0), m_s(surf)
{}
/*----------------------------------------------------------------------------*/
CommandNewVertexByProjection::
~CommandNewVertexByProjection()
{}
/*----------------------------------------------------------------------------*/
void CommandNewVertexByProjection::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewVertexByProjection::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << ". (" << m_v->getName()<< ", " << (m_c?m_c->getName():m_s->getName()) << ").";


    Utils::Math::Point p(m_v->getX(),m_v->getY(),m_v->getZ());

    if (m_c)
    	m_c->project(p);
    else
    	m_s->project(p);

    Vertex* v= EntityFactory(getContext()).newVertex(p);
    m_createdEntities.push_back(v);

    // stockage dans le manager géom
    split(v);

    // ajoute le sommet au groupe s'il y en a un de spécifié
    addToGroup(v);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du sommet "<<v->getName();
    if (m_c)
    	comments << " par projection sur "<<m_c->getName();
    else
    	comments << " par projection sur "<<m_s->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewVertexByProjection::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewVertices(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


