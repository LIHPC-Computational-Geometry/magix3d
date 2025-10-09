/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Geom/CommandCreateWithOtherGeomEntities.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/ReferencedMutex.h>
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
void CommandCreateWithOtherGeomEntities::internalUndo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandCreateWithOtherGeomEntities::internalUndo pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex autoMutex (getMutex ( ));

    // permute toutes les propriétés internes avec leur sauvegarde
    m_memento_service.permMementos();

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
    m_memento_service.permMementos();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    startingOrcompletionLog (false);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
