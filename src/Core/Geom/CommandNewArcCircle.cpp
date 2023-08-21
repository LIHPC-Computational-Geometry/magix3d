/*----------------------------------------------------------------------------*/
/*
 * CommandNewArcCircle.cpp
 *
 *  Created on: 2 avr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandNewArcCircle.h"
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
CommandNewArcCircle::
CommandNewArcCircle(Internal::Context& c,
        Geom::Vertex* pc,
        Geom::Vertex* pd, Geom::Vertex* pe,
        const bool direct,
        const Vector& normal,
        const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'un arc de cercle", groupName), m_center(pc),
 m_start(pd), m_end(pe), m_normal(normal), m_direction(direct)
{}
/*----------------------------------------------------------------------------*/
CommandNewArcCircle::~CommandNewArcCircle()
{}
/*----------------------------------------------------------------------------*/
void CommandNewArcCircle::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewArcCircle::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << std::ios_base::fixed << TkUtil::setprecision (8)
            << ". center " << *m_center<<", start "
            << *m_start<< ", end "<<*m_end
            <<", direction"<<(short)m_direction
            <<", normal "<<m_normal
            <<".";


    // sauvegarde des relations (connections topologiques) avant modification
    saveMemento(m_start);
    saveMemento(m_end);

    Curve* c = EntityFactory(getContext()).newArcCircle(m_center,m_start,m_end,m_direction, m_normal);
    m_createdEntities.push_back(c);

    // stockage dans le manager géom
    store(c);

    // ajoute la courbe au groupe s'il y en a un de spécifié
    addToGroup(c);

    //creation des connections topologiques
    c->add(m_start);
    m_start->add(c);
    if(m_end!=m_start){
        c->add(m_end);
        m_end->add(c);
    }

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de l'arc de cercle "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewArcCircle::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
