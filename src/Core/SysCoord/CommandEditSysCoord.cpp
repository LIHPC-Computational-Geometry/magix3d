/*----------------------------------------------------------------------------*/
/*
 * \file CommandEditSysCoord.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Group/Group3D.h"
#include <SysCoord/CommandEditSysCoord.h>
#include <SysCoord/SysCoordManager.h>

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
CommandEditSysCoord::CommandEditSysCoord(Internal::Context& c, const std::string& name)
: Internal::CommandInternal (c, name)
, m_syscoord(0)
, m_save_ppty(0)
{
}
/*----------------------------------------------------------------------------*/
CommandEditSysCoord::~CommandEditSysCoord()
{
	if (m_save_ppty)
		delete m_save_ppty;
}
/*----------------------------------------------------------------------------*/
void CommandEditSysCoord::internalUndo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // permute toutes les propriétés internes avec leur sauvegarde
     permInternalsStats();
     m_syscoord->update();
}
/*----------------------------------------------------------------------------*/
void CommandEditSysCoord::internalRedo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // permute toutes les propriétés internes avec leur sauvegarde
     permInternalsStats();
     m_syscoord->update();
}
/*----------------------------------------------------------------------------*/
void CommandEditSysCoord::setInfoProperty(SysCoord* syscoord, SysCoordProperty* save_ppty)
{
	if (m_syscoord || m_save_ppty)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandEditSysCoord::setInfoProperty déjà fait ?", TkUtil::Charset::UTF_8));

	m_syscoord = syscoord;
	m_save_ppty = save_ppty;
}
/*----------------------------------------------------------------------------*/
void CommandEditSysCoord::permInternalsStats()
{
	m_save_ppty = m_syscoord->setProperty(m_save_ppty);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

