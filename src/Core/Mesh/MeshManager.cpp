/*----------------------------------------------------------------------------*/
/*
 * \file MeshManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/CommandManager.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandNewBlocksMesh.h"
#include "Mesh/CommandNewFacesMesh.h"
#include "Mesh/Compare2Meshes.h"
#include "Mesh/CommandMeshExplorer.h"
#include "Mesh/CommandReadMLI.h"
#include "Mesh/CommandWriteMLI.h"
#include "Mesh/CommandWriteVTK.h"
#include "Mesh/CommandWriteCGNS.h"
#include "Mesh/CommandModifyMesh.h"
#include "Mesh/SubVolume.h"
#include "Topo/Block.h"
#include "Internal/M3DCommandResult.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <Mesh/CommandCreateSubVolumeBetweenSheets.h>
#include <TkUtil/MemoryError.h>

#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/

MeshManager::MeshManager(const std::string& name, Internal::Context* c)
:Mesh::MeshManagerIfc(name, c)
, m_mesh_itf(new MeshImplementation(c))
, m_strategy(MODIFIABLE)
, m_coface_allways_in_groups(false)
{
}
/*----------------------------------------------------------------------------*/

MeshManager::~MeshManager()
{
    if (m_mesh_itf)
        delete m_mesh_itf;
    m_clouds.deleteAndClear();
    m_surfaces.deleteAndClear();
    m_volumes.deleteAndClear();
}
/*----------------------------------------------------------------------------*/
void MeshManager::clear()
{
    if (m_mesh_itf){
        delete m_mesh_itf;
        m_mesh_itf = new MeshImplementation(&getLocalContext());
    }
    m_clouds.deleteAndClear();
    m_surfaces.deleteAndClear();
    m_volumes.deleteAndClear();
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Cloud* cl)
{
    m_clouds.add(cl);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Cloud* cl)
{
    m_clouds.remove(cl, true);
}
/*----------------------------------------------------------------------------*/
Cloud* MeshManager::getCloud(const std::string& name, const bool exceptionIfNotFound) const
{
    Cloud* cloud = 0;
    const std::vector<Cloud*>& clouds = m_clouds.get();
    for (std::vector<Cloud*>::const_iterator iter = clouds.begin();
            iter != clouds.end(); ++iter)
        if (name == (*iter)->getName())
            cloud = (*iter);

    if (exceptionIfNotFound && cloud == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getCloud impossible, entité \""<<name
                <<"\" n'a pas été trouvée dans le MeshManager";
        throw TkUtil::Exception(message);
    }

    return cloud;
}
/*----------------------------------------------------------------------------*/
void MeshManager::getClouds(std::vector<Cloud*>& AClouds) const
{
    AClouds = m_clouds.get();
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Line* ln)
{
    m_lines.add(ln);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Line* ln)
{
	m_lines.remove(ln, true);
}
/*----------------------------------------------------------------------------*/
Line* MeshManager::getLine(const std::string& name, const bool exceptionIfNotFound) const
{
	Line* line = 0;
    const std::vector<Line*>& lines = m_lines.get();
    for (std::vector<Line*>::const_iterator iter = lines.begin();
            iter != lines.end(); ++iter)
        if (name == (*iter)->getName())
            line = (*iter);

    if (exceptionIfNotFound && line == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getLine impossible, entité \""<<name
                <<"\" n'a pas été trouvée dans le MeshManager";
        throw TkUtil::Exception(message);
    }

    return line;
}
/*----------------------------------------------------------------------------*/
void MeshManager::getLines(std::vector<Line*>& ALines) const
{
	ALines = m_lines.get();
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Surface* sf)
{
    m_surfaces.add(sf);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Surface* sf)
{
    m_surfaces.remove(sf, true);
}
/*----------------------------------------------------------------------------*/
Surface* MeshManager::getSurface(const std::string& name, const bool exceptionIfNotFound) const
{
    Surface* surf = 0;

    const std::vector<Surface*>& surfaces = m_surfaces.get();
    for (std::vector<Surface* >::const_iterator iter = surfaces.begin();
            iter != surfaces.end(); ++iter)
        if (name == (*iter)->getName())
            surf = (*iter);

    if (exceptionIfNotFound && surf == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getSurface impossible, entité \""<<name
                <<"\" n'a pas été trouvée dans le MeshManager";
        throw TkUtil::Exception(message);
    }

    return surf;
}
/*----------------------------------------------------------------------------*/
void MeshManager::getSurfaces(std::vector<Surface*>& ASurfaces) const
{
    ASurfaces = m_surfaces.get();
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Volume* vo)
{
    m_volumes.add(vo);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Volume* vo)
{
    //std::cout<<"MeshManager::remove("<<vo->getName()<<")"<<std::endl;
    m_volumes.remove(vo, true);
}
/*----------------------------------------------------------------------------*/
Volume* MeshManager::getVolume(const std::string& name, const bool exceptionIfNotFound) const
{
    Volume* vol = 0;

    const std::vector<Volume*>& volumes = m_volumes.get();
    for (std::vector<Volume* >::const_iterator iter = volumes.begin();
            iter != volumes.end(); ++iter)
        if (name == (*iter)->getName())
            vol = (*iter);

    if (exceptionIfNotFound && vol == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getVolume impossible, entité \""<<name
                <<"\" n'a pas été trouvée dans le MeshManager";
        throw TkUtil::Exception(message);
    }

    return vol;
}
/*----------------------------------------------------------------------------*/
SubVolume* MeshManager::getNewSubVolume(const std::string& gr_name, Internal::InfoCommand* icmd)
{
	CHECK_NULL_PTR_ERROR(icmd);

	if (gr_name.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Création d'un sous-volume impossible sans un nom", TkUtil::Charset::UTF_8));

	Volume* vol = 0;
	const std::vector<Volume*>& volumes = m_volumes.get();
	for (std::vector<Volume* >::const_iterator iter = volumes.begin();
			iter != volumes.end(); ++iter)
		if (gr_name == (*iter)->getName())
			vol = (*iter);

	if (vol && vol->getType() != Utils::Entity::MeshSubVolume){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"Création d'un sous-volume de mailles impossible avec \""<<gr_name
				<<"\", ce groupe existe déjà et est un volume";
		throw TkUtil::Exception(message);
	}

	SubVolume* sv = 0;
	if (vol)
		sv = dynamic_cast<SubVolume*>(vol);

	if (sv == 0){
		sv = new Mesh::SubVolume(
				getLocalContext(),
				getLocalContext().newProperty(Utils::Entity::MeshSubVolume, gr_name),
				getLocalContext().newDisplayProperties(Utils::Entity::MeshSubVolume),
				0);
		icmd->addMeshInfoEntity(sv, Internal::InfoCommand::CREATED);
		getLocalContext().newGraphicalRepresentation (*sv);
		add(sv);
	}
	else if (sv->isDestroyed()){
		sv->setDestroyed(false);
		icmd->addMeshInfoEntity(sv,Internal::InfoCommand::ENABLE);
	}

	return sv;
}
/*----------------------------------------------------------------------------*/
void MeshManager::getVolumes(std::vector<Volume*>& AVolumes) const
{
    AVolumes = m_volumes.get();
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManager::newBlocksMesh(std::vector<std::string>& names)
{
    std::vector<Mgx3D::Topo::Block*> entities;
    for(int i=0;i<names.size();i++)
        entities.push_back(getLocalContext().getLocalTopoManager().getBlock(names[i]));
    return newBlocksMesh(entities);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManager::newBlocksMesh(std::vector<Mgx3D::Topo::Block*>& blocks)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newBlocksMesh( liste de "<<blocks.size()<<" blocs )";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewBlocksMesh* command = new Mesh::CommandNewBlocksMesh(getLocalContext(), blocks, 0);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newBlocksMesh ( [\"";
    for (int i=0;i<blocks.size();i++){
        if (i!=0)
             cmd <<"\", \"";
        cmd << blocks[i]->getName();
    }
    cmd <<"\"] )";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return (cmdResult);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManager::newFacesMesh(std::vector<std::string>& names)
{
    std::vector<Mgx3D::Topo::CoFace*> entities;
    for(int i=0;i<names.size();i++)
        entities.push_back(getLocalContext().getLocalTopoManager().getCoFace(names[i]));
    return newFacesMesh(entities);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManager::newFacesMesh(std::vector<Mgx3D::Topo::CoFace*>& faces)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newFacesMesh( liste de "<<faces.size()<<" faces )";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewFacesMesh* command = new Mesh::CommandNewFacesMesh(getLocalContext(), faces, 0);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newFacesMesh ( [\"";
    for (int i=0;i<faces.size();i++){
        if (i!=0)
             cmd <<"\", \"";
        cmd << faces[i]->getName();
    }
    cmd <<"\"] )";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManager::newAllBlocksMesh()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newAllBlocksMesh()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewBlocksMesh* command = new Mesh::CommandNewBlocksMesh(getLocalContext(),0);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newAllBlocksMesh()";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManager::newAllFacesMesh()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newAllFacesMesh()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewFacesMesh* command = new Mesh::CommandNewFacesMesh(getLocalContext(),0);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newAllFacesMesh()";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
MeshManager::newSubVolumeBetweenSheets(std::vector<std::string>& blocks_name, std::string narete,
		int pos1, int pos2, std::string groupName)
{
    std::vector<Mgx3D::Topo::Block*> entities;
    for(int i=0;i<blocks_name.size();i++)
        entities.push_back(getLocalContext().getLocalTopoManager().getBlock(blocks_name[i]));

    return newSubVolumeBetweenSheets(entities,
    		getLocalContext().getLocalTopoManager().getCoEdge(narete),
			pos1, pos2, groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
MeshManager::newSubVolumeBetweenSheets(std::vector<Mgx3D::Topo::Block*>& blocks, Topo::CoEdge* coedge,
		int pos1, int pos2, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newSubVolumeBetweenSheets( liste de "<<blocks.size()<<" blocs )";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandCreateSubVolumeBetweenSheets* command =
    		new Mesh::CommandCreateSubVolumeBetweenSheets(getLocalContext(), blocks, coedge, pos1, pos2, groupName);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newSubVolumeBetweenSheets ( [\"";
    for (int i=0;i<blocks.size();i++){
        if (i!=0)
             cmd <<"\", \"";
        cmd << blocks[i]->getName();
    }
    cmd <<"\"], \""<<coedge->getName()<<"\", "
        <<(short)pos1<<", "<<(short)pos2
		<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return (cmdResult);
}
/*----------------------------------------------------------------------------*/
void MeshManager::readMli(std::string nom, std::string prefix)
{
    // création de la commande d'écriture du maillage
    CommandReadMLI* command = new CommandReadMLI(getLocalContext(), nom, prefix);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().readMli(\""
    		<< nom << "\", \"" << prefix << "\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void MeshManager::writeMli(std::string nom)
{
    // création de la commande d'écriture du maillage
    CommandWriteMLI* command = new CommandWriteMLI(getLocalContext(), nom);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().writeMli(\"" << nom << "\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void MeshManager::writeVTK(std::string nom)
{
    // création de la commande d'écriture du maillage
    CommandWriteVTK* command = new CommandWriteVTK(getLocalContext(), nom);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().writeVTK(\"" << nom << "\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void MeshManager::writeCGNS(std::string nom)
{
    // création de la commande d'écriture du maillage
    CommandWriteCGNS* command = new CommandWriteCGNS(getLocalContext(), nom);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().writeCGNS(\"" << nom << "\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void MeshManager::smooth()
{
    // création de la commande d'écriture du maillage
	std::string str("lissage du maillage");
    CommandModifyMesh* command = new CommandModifyMesh(getLocalContext(), str);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().smooth(\"" << "poyop" << "\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);
}
/*------------------------------------------------------------------------*/
bool MeshManager::compareWithMesh(std::string nom)
{
    bool ok = true;
    MeshImplementation* mesh = (MeshImplementation*)m_mesh_itf;
    gmds::IGMesh& gmdsMesh1 = mesh->getGMDSMesh();

    uint id = mesh->createNewGMDSMesh();
    gmds::IGMesh& gmdsMesh2 = mesh->getGMDSMesh(id);

    mesh->readMli(nom, id);

    // on ajoute les groupes de mailles de gmds
    mesh->createGMDSGroups();

    // on pourrait prévoir plus de chose en retour
    Compare2Meshes cmp(&getLocalContext(), gmdsMesh1, gmdsMesh2);
    ok = cmp.perform();

    // on retire les groupes
    mesh->deleteGMDSGroups();

    mesh->deleteLastGMDSMesh();

    return ok;
}
/*----------------------------------------------------------------------------*/
std::string MeshManager::getInfos(const std::string& name, int dim) const
{
    switch(dim){
    case(3):
        return getInfos(MeshManager::getVolume (name, true));
    break;
    case(2):
        return getInfos(MeshManager::getSurface (name, true));
    break;
    case(0):
        return getInfos(MeshManager::getCloud (name, true));
    break;
    default:
        throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour MeshManager::getInfos", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
std::string MeshManager::getInfos(const Cloud* me) const
{
    if (me){
            TkUtil::UTF8String us (TkUtil::Charset::UTF_8);
            us << *me;
            return us.iso();
        }
        else
            return std::string("Nuage non trouvé !");
}
/*----------------------------------------------------------------------------*/
std::string MeshManager::getInfos(const Surface* me) const
{
    if (me){
            TkUtil::UTF8String us (TkUtil::Charset::UTF_8);
            us << *me;
            return us.iso();
        }
        else
            return std::string("Surface non trouvée !");
}
/*----------------------------------------------------------------------------*/
std::string MeshManager::getInfos(const Volume* me) const
{
    if (me){
            TkUtil::UTF8String us (TkUtil::Charset::UTF_8);
            us << *me;
            return us.iso();
        }
        else
            return std::string("Volume non trouvé !");
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbClouds(bool onlyVisible) const
{
    if (onlyVisible)
        return m_clouds.getVisibleNb();
    else
        return m_clouds.getNb();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbSurfaces(bool onlyVisible) const
{
    if (onlyVisible)
        return m_surfaces.getVisibleNb();
    else
        return m_surfaces.getNb();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbVolumes(bool onlyVisible) const
{
    if (onlyVisible)
        return m_volumes.getVisibleNb();
    else
        return m_volumes.getNb();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbNodes()
{
    return getMesh()->getNbNodes();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbFaces()
{
    return getMesh()->getNbFaces();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbRegions()
{
    return getMesh()->getNbRegions();
}
/*----------------------------------------------------------------------------*/
CommandMeshExplorer* MeshManager::newExplorer(CommandMeshExplorer* oldExplo, int inc, std::string narete, bool asCommand)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newExplorer()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandMeshExplorer* command =
            new Mesh::CommandMeshExplorer(
                    getLocalContext(),
                    oldExplo,
                    inc,
                    getLocalContext().getLocalTopoManager().getCoEdge(narete));

    /// pas de trace dans le script
    command->setScriptable(asCommand);

    if (true == asCommand)
        getCommandManager().addCommand(command, Utils::Command::DO);

    return command;
}
/*----------------------------------------------------------------------------*/
CommandMeshExplorer* MeshManager::endExplorer(CommandMeshExplorer* oldExplo, bool asCommand)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::endExplorer()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandMeshExplorer* command =
            new Mesh::CommandMeshExplorer(
                    getLocalContext(),
                    oldExplo);

    /// pas de trace dans le script
    command->setScriptable(asCommand);

    if (true == asCommand)
        getCommandManager().addCommand(command, Utils::Command::DO);

    return command;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
