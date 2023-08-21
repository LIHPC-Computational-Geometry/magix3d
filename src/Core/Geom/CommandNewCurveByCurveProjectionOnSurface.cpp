/*----------------------------------------------------------------------------*/
/*
 * CommandNewCurveByCurveProjectionOnSurface.cpp
 *
 *  Created on: 9/3/2015
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewCurveByCurveProjectionOnSurface.h"
#include "Geom/PropertyBox.h"
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
CommandNewCurveByCurveProjectionOnSurface::
CommandNewCurveByCurveProjectionOnSurface(Internal::Context& c,
		Geom::Curve* curve,
		Geom::Surface* surface,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une courbe par projection sur une surface", groupName),
 m_curve(curve), m_surface(surface)
{}
/*----------------------------------------------------------------------------*/
CommandNewCurveByCurveProjectionOnSurface::~CommandNewCurveByCurveProjectionOnSurface()
{}
/*----------------------------------------------------------------------------*/
void CommandNewCurveByCurveProjectionOnSurface::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewCurveByCurveProjectionOnSurface::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << m_curve->getName()<<", "<<m_surface->getName()<<".";

    Curve* c = EntityFactory(getContext()).newCurveByCurveProjectionOnSurface(m_curve, m_surface);
    m_createdEntities.push_back(c);


    // ajoute la courbe au groupe s'il y en a un de spécifié
    addToGroup(c);

    //aucune creation de connections topologiques
    split(c);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du segment "<<c->getName()<<" par projection de "
    		<<m_curve->getName()<<" sur "<<m_surface->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
