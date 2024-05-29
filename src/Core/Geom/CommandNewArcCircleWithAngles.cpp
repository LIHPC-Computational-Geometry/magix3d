/*----------------------------------------------------------------------------*/
/*
 * CommandNewArcCircleWithAngles.cpp
 *
 *  Created on: 29/9/2020
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandNewArcCircleWithAngles.h"
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
CommandNewArcCircleWithAngles::
CommandNewArcCircleWithAngles(Internal::Context& c,
                              const double& angleDep,
                              const double& angleFin,
                              const double& rayon,
                              CoordinateSystem::SysCoord* rep,
                              const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'un arc de cercle", groupName),
m_angleDep(angleDep), m_angleFin(angleFin), m_rayon(rayon), m_rep(rep)
{}
/*----------------------------------------------------------------------------*/
CommandNewArcCircleWithAngles::~CommandNewArcCircleWithAngles()
{}
/*----------------------------------------------------------------------------*/
void CommandNewArcCircleWithAngles:: 
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewArcCircleWithAngles::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << std::ios_base::fixed << TkUtil::setprecision (8)
            << ". angle départ " <<m_angleDep <<", angle fin "
            << m_angleFin << ", rayon "<<m_rayon;
    if (m_rep)
        message << " avec comme repère "<< m_rep->getName();
    else
        message << " sans repère";

    Curve* c = EntityFactory(getContext()).newArcCircle(m_angleDep, m_angleFin, m_rayon, m_rep);
    m_createdEntities.push_back(c);

    // stockage dans le manager géom
    store(c);

    // ajoute la courbe au groupe s'il y en a un de spécifié
    addToGroup(c);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de l'arc de cercle "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewArcCircleWithAngles::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
