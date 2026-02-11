/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/CommandManager.h"
#include "Utils/MgxException.h"
#include "Utils/Vector.h"
#include "Utils/Rotation.h"
#include "Utils/TypeDedicatedNameManager.h"
#include "SysCoord/SysCoordManager.h"
#include "SysCoord/SysCoord.h"
#include "SysCoord/CommandNewSysCoord.h"
#include "SysCoord/CommandDuplicateSysCoord.h"
#include "SysCoord/CommandTranslateSysCoord.h"
#include "SysCoord/CommandRotateSysCoord.h"
#include "Internal/Context.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandComposite.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
SysCoordManager::SysCoordManager(const std::string& name, Internal::Context* c)
:Internal::CommandCreator(name, c)
{
}
/*----------------------------------------------------------------------------*/
SysCoordManager::SysCoordManager(const SysCoordManager& scm)
:Internal::CommandCreator(scm)
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
	for (auto iter = m_reperes.begin(); iter != m_reperes.end(); ++iter)
        delete *iter;
	m_reperes.clear();
}
/*----------------------------------------------------------------------------*/
void SysCoordManager::add(SysCoord* rep)
{
    m_reperes.push_back(rep);
}
/*----------------------------------------------------------------------------*/
void SysCoordManager::remove(SysCoord* rep)
{
    Utils::remove(rep, m_reperes);
}
/*----------------------------------------------------------------------------*/
int SysCoordManager::getNbSysCoords() const
{
    return m_reperes.size();
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::newSysCoord(std::string groupName)
{
    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));
	CommandNewSysCoord* command = new CommandNewSysCoord(getContext(), groupName);

	// trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().newSysCoord (\""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
               new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::newSysCoord(const Utils::Math::Point& p, std::string groupName)
{
    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));
	CommandNewSysCoord* command = new CommandNewSysCoord(getContext(), p, groupName);

	// trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().newSysCoord ("
            <<p.getScriptCommand()<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
               new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::newSysCoord(const Utils::Math::Point& pCentre,
		const Utils::Math::Point& pX,
		const Utils::Math::Point& pY, std::string groupName)
{
    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

    CommandNewSysCoord* command = new CommandNewSysCoord(getContext(),
			pCentre, pX, pY, groupName);

	// trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().newSysCoord ("
    		<<pCentre.getScriptCommand()<<", "
			<<pX.getScriptCommand()<<", "
			<<pY.getScriptCommand()<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
               new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::translate(std::string name, const Vector& dp)
{
	return translate(getSysCoord(name,true), dp);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::translate(SysCoord* syscoord, const Vector& dp)
{
    if (0 == syscoord){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la translation", TkUtil::Charset::UTF_8));
    }

    Internal::CommandInternal* command  = new CommandTranslateSysCoord(getContext(), syscoord, dp);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().translate (\""
    		<< syscoord->getName() << "\", " << dp.getScriptCommand() <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::copyAndTranslate(std::string name, const Vector& dp, std::string groupName)
{
	return copyAndTranslate(getSysCoord(name,true), dp, groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::copyAndTranslate(SysCoord* syscoord, const Vector& dp, std::string groupName)
{
	if (0 == syscoord)
		throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la translation", TkUtil::Charset::UTF_8));
	if (groupName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

	Internal::CommandComposite* command = new Internal::CommandComposite(getContext(), "Translation d'une copie d'un repère");

	CommandDuplicateSysCoord* commandCopy  = new CommandDuplicateSysCoord(getContext(), syscoord, groupName);
	command->addCommand(commandCopy);
	Internal::CommandInternal* commandRotate  = new CommandTranslateSysCoord(getContext(), commandCopy, dp);
	command->addCommand(commandRotate);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << "." << "getSysCoordManager().copyAndTranslate (\""
			<< syscoord->getName() << "\", " << dp.getScriptCommand() <<", \""<<groupName<<"\")";
	command->setScriptCommand(cmd);

	// on passe au gestionnaire de commandes qui exécute la commande en // ou non
	// et la stocke dans le gestionnaire de undo-redo si c'est une réussite
	getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResult*  cmdResult   =
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::rotate(std::string name,
				const Utils::Math::Rotation& rot)
{
	return rotate(getSysCoord(name,true), rot);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::rotate(SysCoord* syscoord,
				const Utils::Math::Rotation& rot)
{
    if (0 == syscoord)
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la rotation", TkUtil::Charset::UTF_8));


    Internal::CommandInternal* command  = new CommandRotateSysCoord(getContext(), syscoord, rot);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().rotate (\""
    		<< syscoord->getName() << "\", " << rot.getScriptCommand() <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::copyAndRotate(std::string name,
			const Utils::Math::Rotation& rot, std::string groupName)
{
	return copyAndRotate(getSysCoord(name,true), rot, groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::copyAndRotate(SysCoord* syscoord,
			const Utils::Math::Rotation& rot, std::string groupName)
{
	if (0 == syscoord)
		throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la rotation", TkUtil::Charset::UTF_8));
	if (groupName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

	Internal::CommandComposite* command = new Internal::CommandComposite(getContext(), "Translation d'une copie d'un repère");

	CommandDuplicateSysCoord* commandCopy  = new CommandDuplicateSysCoord(getContext(), syscoord, groupName);
	command->addCommand(commandCopy);
	Internal::CommandInternal* commandRotate  = new CommandRotateSysCoord(getContext(), commandCopy, rot);
	command->addCommand(commandRotate);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << "." << "getSysCoordManager().copyAndRotate (\""
			<< syscoord->getName() << "\", " << rot.getScriptCommand() <<", \""<<groupName<<"\")";
	command->setScriptCommand(cmd);

	// on passe au gestionnaire de commandes qui exécute la commande en // ou non
	// et la stocke dans le gestionnaire de undo-redo si c'est une réussite
	getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResult*  cmdResult   =
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::copy(std::string name, std::string groupName)
{
	return copy(getSysCoord(name,true), groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
SysCoordManager::copy(SysCoord* syscoord, std::string groupName)
{
    if (0 == syscoord)
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la copie", TkUtil::Charset::UTF_8));

    if (groupName.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Il faut mettre le nouveau repère dans un groupe", TkUtil::Charset::UTF_8));

    Internal::CommandInternal* command  = new CommandDuplicateSysCoord(getContext(), syscoord, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getSysCoordManager().copy (\""
    		<< syscoord->getName() << "\", \"" << groupName <<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
SysCoord* SysCoordManager::getSysCoord (const std::string& name, const bool exceptionIfNotFound) const
{
    return getContext().getNameManager().findByName(name, m_reperes, exceptionIfNotFound);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
