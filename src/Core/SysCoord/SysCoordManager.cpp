/*----------------------------------------------------------------------------*/
/*
 * \file SysCoordManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 mai 2018
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/CommandManager.h"
#include "Utils/TypeDedicatedNameManager.h"
#include "Utils/MgxException.h"
#include "Utils/Vector.h"
#include "Utils/Rotation.h"
#include <SysCoord/SysCoordManager.h>
#include <SysCoord/CommandNewSysCoord.h>
#include <SysCoord/CommandDuplicateSysCoord.h>
#include <SysCoord/CommandTranslateSysCoord.h>
#include <SysCoord/CommandRotateSysCoord.h>
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandComposite.h"
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
SysCoordManager::SysCoordManager(const std::string& name, Internal::ContextIfc* c)
:SysCoordManagerIfc(name, c)
{
}
/*----------------------------------------------------------------------------*/
SysCoordManager::SysCoordManager(const SysCoordManager& scm)
:SysCoordManagerIfc(scm)
{
	MGX_FORBIDDEN ("SysCoordManager copy constructor is not allowed.")
}
/*----------------------------------------------------------------------------*/
SysCoordManager& SysCoordManager::operator =(const SysCoordManager& scm)
{
	MGX_FORBIDDEN ("SysCoordManager::operator = is not allowed.")
	return *this;
}
/*----------------------------------------------------------------------------*/
SysCoordManager::~SysCoordManager ( )
{
}
/*----------------------------------------------------------------------------*/
void SysCoordManager::clear()
{
	m_reperes.clear();
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::newSysCoord(std::string groupName)
{
    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));
	CommandNewSysCoord* command = new CommandNewSysCoord(getLocalContext(), groupName);

	// trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().newSysCoord (\""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
               new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::newSysCoord(const Utils::Math::Point& p, std::string groupName)
{
    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));
	CommandNewSysCoord* command = new CommandNewSysCoord(getLocalContext(), p, groupName);

	// trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().newSysCoord ("
            <<p.getScriptCommand()<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
               new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::newSysCoord(const Utils::Math::Point& pCentre,
		const Utils::Math::Point& pX,
		const Utils::Math::Point& pY, std::string groupName)
{
    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

    CommandNewSysCoord* command = new CommandNewSysCoord(getLocalContext(),
			pCentre, pX, pY, groupName);

	// trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().newSysCoord ("
    		<<pCentre.getScriptCommand()<<", "
			<<pX.getScriptCommand()<<", "
			<<pY.getScriptCommand()<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
               new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::translate(std::string name, const Vector& dp)
{
	return translate(getSysCoord(name,true), dp);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::translate(SysCoord* syscoord, const Vector& dp)
{
    if (0 == syscoord){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la translation", TkUtil::Charset::UTF_8));
    }

    Internal::CommandInternal* command  = new CommandTranslateSysCoord(getLocalContext(), syscoord, dp);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().translate (\""
    		<< syscoord->getName() << "\", " << dp.getScriptCommand() <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::copyAndTranslate(std::string name, const Vector& dp, std::string groupName)
{
	return copyAndTranslate(getSysCoord(name,true), dp, groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::copyAndTranslate(SysCoord* syscoord, const Vector& dp, std::string groupName)
{
	if (0 == syscoord)
		throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la translation", TkUtil::Charset::UTF_8));
	if (groupName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

	Internal::CommandComposite* command = new Internal::CommandComposite(getLocalContext(), "Translation d'une copie d'un repère");

	CommandDuplicateSysCoord* commandCopy  = new CommandDuplicateSysCoord(getLocalContext(), syscoord, groupName);
	command->addCommand(commandCopy);
	Internal::CommandInternal* commandRotate  = new CommandTranslateSysCoord(getLocalContext(), commandCopy, dp);
	command->addCommand(commandRotate);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << "." << "getSysCoordManager().copyAndTranslate (\""
			<< syscoord->getName() << "\", " << dp.getScriptCommand() <<", \""<<groupName<<"\")";
	command->setScriptCommand(cmd);

	// on passe au gestionnaire de commandes qui exécute la commande en // ou non
	// et la stocke dans le gestionnaire de undo-redo si c'est une réussite
	getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*  cmdResult   =
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::rotate(std::string name,
				const Utils::Math::Rotation& rot)
{
	return rotate(getSysCoord(name,true), rot);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::rotate(SysCoord* syscoord,
				const Utils::Math::Rotation& rot)
{
    if (0 == syscoord)
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la rotation", TkUtil::Charset::UTF_8));


    Internal::CommandInternal* command  = new CommandRotateSysCoord(getLocalContext(), syscoord, rot);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().rotate (\""
    		<< syscoord->getName() << "\", " << rot.getScriptCommand() <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::copyAndRotate(std::string name,
			const Utils::Math::Rotation& rot, std::string groupName)
{
	return copyAndRotate(getSysCoord(name,true), rot, groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::copyAndRotate(SysCoord* syscoord,
			const Utils::Math::Rotation& rot, std::string groupName)
{
	if (0 == syscoord)
		throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la rotation", TkUtil::Charset::UTF_8));
	if (groupName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

	Internal::CommandComposite* command = new Internal::CommandComposite(getLocalContext(), "Translation d'une copie d'un repère");

	CommandDuplicateSysCoord* commandCopy  = new CommandDuplicateSysCoord(getLocalContext(), syscoord, groupName);
	command->addCommand(commandCopy);
	Internal::CommandInternal* commandRotate  = new CommandRotateSysCoord(getLocalContext(), commandCopy, rot);
	command->addCommand(commandRotate);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << "." << "getSysCoordManager().copyAndRotate (\""
			<< syscoord->getName() << "\", " << rot.getScriptCommand() <<", \""<<groupName<<"\")";
	command->setScriptCommand(cmd);

	// on passe au gestionnaire de commandes qui exécute la commande en // ou non
	// et la stocke dans le gestionnaire de undo-redo si c'est une réussite
	getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*  cmdResult   =
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::copy(std::string name, std::string groupName)
{
	return copy(getSysCoord(name,true), groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManager::copy(SysCoord* syscoord, std::string groupName)
{
    if (0 == syscoord)
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la copie", TkUtil::Charset::UTF_8));

    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

    Internal::CommandInternal* command  = new CommandDuplicateSysCoord(getLocalContext(), syscoord, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().copy (\""
    		<< syscoord->getName() << "\", \"" << groupName <<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
SysCoord* SysCoordManager::getSysCoord (const std::string& name, const bool exceptionIfNotFound) const
{
	SysCoord* rep = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::SysCoord)->renameWithShiftingId(name);
    else
        new_name = name;

    if (SysCoord::isA(new_name)){
        const std::vector<SysCoord* >& sysCoord = m_reperes.get();
        for (std::vector<SysCoord* >::const_iterator iter2 = sysCoord.begin();
                iter2 != sysCoord.end() && rep == 0; ++iter2){
            if (new_name == (*iter2)->getName())
                rep = (*iter2);
        }
    }
    else if (exceptionIfNotFound){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getSysCoord impossible, entité \""<<new_name<<"\" n'a pas un nom de repère";
        throw TkUtil::Exception(message);
    }

    if (rep != 0 && rep->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getSysCoord trouve l'entité \""<<new_name<<"\", mais il est détruit";
        throw Utils::IsDestroyedException(message);
        }

    if (exceptionIfNotFound && rep == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getSysCoord impossible, entité \""<<new_name<<"\" n'a pas été trouvé dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return rep;
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
