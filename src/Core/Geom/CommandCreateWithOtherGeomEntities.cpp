/*
 * CommandCreateWithOtherGeomEntities.cpp
 *
 *  Created on: 29/08/2018
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>

/*----------------------------------------------------------------------------*/
using namespace TkUtil;
using namespace Mgx3D::Utils;
//#define _DEBUG_MEMENTO
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandCreateWithOtherGeomEntities::CommandCreateWithOtherGeomEntities(Internal::Context& c, std::string name,
        const std::string& groupName)
: Geom::CommandCreateGeom(c, name,groupName)
{
}
/*----------------------------------------------------------------------------*/
CommandCreateWithOtherGeomEntities::~CommandCreateWithOtherGeomEntities()
{
    deleteMementos();
}
/*----------------------------------------------------------------------------*/
void CommandCreateWithOtherGeomEntities::internalUndo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandCreateWithOtherGeomEntities::internalUndo pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex autoMutex (getMutex ( ));

    // permute toutes les propriétés internes avec leur sauvegarde
    permMementos();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandCreateWithOtherGeomEntities::internalRedo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandCreateWithOtherGeomEntities::internalRedo pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex autoMutex (getMutex ( ));

    startingOrcompletionLog (true);

    // permute toutes les propriétés internes avec leur sauvegarde
    permMementos();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    startingOrcompletionLog (false);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}

/*----------------------------------------------------------------------------*/
void CommandCreateWithOtherGeomEntities::saveMemento(GeomEntity* entity)
{
#ifdef _DEBUG_MEMENTO
	std::cout<<"saveMemento pour "<<entity->getName()<<std::endl;
#endif

    MementoGeomEntity mem;
	entity->createMemento(mem);
	m_mementos.insert(std::pair<GeomEntity*,MementoGeomEntity>(entity,mem));
}
/*----------------------------------------------------------------------------*/
void CommandCreateWithOtherGeomEntities::permMementos()
{
    std::map<GeomEntity*,MementoGeomEntity>::iterator it =  m_mementos.begin();
    for(;it!=m_mementos.end();it++){
        GeomEntity *e = it->first;
        MementoGeomEntity mem_saved = it->second;
        MementoGeomEntity mem_current;
        e->createMemento(mem_current);
        it->second = mem_current;
        e->setFromMemento(mem_saved);
#ifdef _DEBUG_MEMENTO
        std::cout<<"On a permute l'etat pour "<<e->getName()<<std::endl;
        std::cout<<" avant : "<<mem_saved.getCurves().size()<<" courbes"<<std::endl;
        std::cout<<" après : "<<mem_current.getCurves().size()<<" courbes"<<std::endl;
#endif
    }

}
/*----------------------------------------------------------------------------*/
void CommandCreateWithOtherGeomEntities::deleteMementos()
{
    m_mementos.clear();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
