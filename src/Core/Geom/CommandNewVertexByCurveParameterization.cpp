/*
 * CommandNewVertexByCurveParameterization.cpp
 *
 *  Created on: 18 juin 2013
 *      Author: ledouxf
 */

#include "Geom/CommandNewVertexByCurveParameterization.h"
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
CommandNewVertexByCurveParameterization::
CommandNewVertexByCurveParameterization(Internal::Context& c,
        Geom::Curve* curv,
        const double param,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'un sommet sur une courve par paramétrisation", groupName),
 m_c(curv), m_param(param)
{
    if (m_param<0.0 || m_param>1.0)
          throw TkUtil::Exception(TkUtil::UTF8String ("Le paramètre doit être compris entre 0 et 1", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandNewVertexByCurveParameterization::
~CommandNewVertexByCurveParameterization()
{}
/*----------------------------------------------------------------------------*/
void CommandNewVertexByCurveParameterization::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewVertexByCurveParameterization::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << ". (" << m_c->getName() <<", "<<m_param<< ").";


    Utils::Math::Point p;

    m_c->getPoint(m_param,p,true);

    Vertex* v= EntityFactory(getContext()).newVertex(p);
    m_createdEntities.push_back(v);

    // stockage dans le manager géom
    split(v);

    // ajoute le sommet au groupe s'il y en a un de spécifié
    addToGroup(v);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du sommet "<<v->getName()<<" sur la courbe "<<m_c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewVertexByCurveParameterization::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewVertices(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
