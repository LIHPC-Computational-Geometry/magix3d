/*----------------------------------------------------------------------------*/
/*
 * CommandNewCircle.cpp
 *
 *  Created on: 28 oct. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandNewCircle.h"
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
CommandNewCircle::
CommandNewCircle(Internal::Context& c,
        Geom::Vertex* p1, Geom::Vertex* p2, Geom::Vertex* p3,
        const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'un cercle", groupName),
 m_p1(p1), m_p2(p2), m_p3(p3)
{}
/*----------------------------------------------------------------------------*/
CommandNewCircle::~CommandNewCircle()
{}
/*----------------------------------------------------------------------------*/
void CommandNewCircle::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewCircle::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << std::ios_base::fixed << TkUtil::setprecision (8)
            << ". p1 " << *m_p1<<", p2 "
            << *m_p2<< ", p3 "<<*m_p3<<".";

    // sauvegarde des relations (connections topologiques) avant modification
    saveMemento(m_p1);
    saveMemento(m_p2);
    saveMemento(m_p3);

    Curve* c = EntityFactory(getContext()).newCircle(m_p1,m_p2,m_p3);
    m_createdEntities.push_back(c);

    // ajoute la courbe au groupe s'il y en a un de spécifié
    addToGroup(c);

    //aucune creation de connections topologiques
    split(c);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du cercle "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
