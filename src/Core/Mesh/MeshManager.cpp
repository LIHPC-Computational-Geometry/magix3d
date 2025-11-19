/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/M3DCommandResult.h"
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
#include "Mesh/CommandExportBlocksForCGNS.h"
#include "Mesh/SubVolume.h"
#include "Mesh/CommandCreateSubVolumeBetweenSheets.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/

MeshManager::MeshManager(const std::string& name, Internal::Context* c)
: Internal::CommandCreator(name, c)
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
    Utils::deleteAndClear(m_clouds);
    Utils::deleteAndClear(m_surfaces);
    Utils::deleteAndClear(m_volumes);
}
/*----------------------------------------------------------------------------*/
void MeshManager::clear()
{
    if (m_mesh_itf){
        delete m_mesh_itf;
        m_mesh_itf = new MeshImplementation(&getContext());
    }
    Utils::deleteAndClear(m_clouds);
    Utils::deleteAndClear(m_surfaces);
    Utils::deleteAndClear(m_volumes);
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Cloud* cl)
{
    m_clouds.push_back(cl);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Cloud* cl)
{
    Utils::remove(cl, m_clouds);
}
/*----------------------------------------------------------------------------*/
Cloud* MeshManager::getCloud(const std::string& name, const bool exceptionIfNotFound) const
{
    return findByName(name, m_clouds, exceptionIfNotFound);
}
/*----------------------------------------------------------------------------*/
std::vector<Cloud*> MeshManager::getCloudsObj() const
{
    return Utils::filterAndSort(m_clouds);
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Line* ln)
{
    m_lines.push_back(ln);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Line* ln)
{
	Utils::remove(ln, m_lines);
}
/*----------------------------------------------------------------------------*/
Line* MeshManager::getLine(const std::string& name, const bool exceptionIfNotFound) const
{
    return findByName(name, m_lines, exceptionIfNotFound);
}
/*----------------------------------------------------------------------------*/
std::vector<Line*> MeshManager::getLinesObj() const
{
    return Utils::filterAndSort(m_lines);
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Surface* sf)
{
    m_surfaces.push_back(sf);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Surface* sf)
{
    Utils::remove(sf, m_surfaces);
}
/*----------------------------------------------------------------------------*/
Surface* MeshManager::getSurface(const std::string& name, const bool exceptionIfNotFound) const
{
    return findByName(name, m_surfaces, exceptionIfNotFound);
}
/*----------------------------------------------------------------------------*/
std::vector<Surface*> MeshManager::getSurfacesObj() const
{
    return Utils::filterAndSort(m_surfaces);
}
/*----------------------------------------------------------------------------*/
void MeshManager::add(Volume* vo)
{
    m_volumes.push_back(vo);
}
/*----------------------------------------------------------------------------*/
void MeshManager::remove(Volume* vo)
{
    Utils::remove(vo, m_volumes);
}
/*----------------------------------------------------------------------------*/
Volume* MeshManager::getVolume(const std::string& name, const bool exceptionIfNotFound) const
{
    return findByName(name, m_volumes, exceptionIfNotFound);
}
/*----------------------------------------------------------------------------*/
std::vector<Volume*> MeshManager::getVolumesObj() const
{
    return Utils::filterAndSort(m_volumes);
}
/*----------------------------------------------------------------------------*/
SubVolume* MeshManager::getNewSubVolume(const std::string& gr_name, Internal::InfoCommand* icmd)
{
	CHECK_NULL_PTR_ERROR(icmd);

	if (gr_name.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Création d'un sous-volume impossible sans un nom", TkUtil::Charset::UTF_8));

	Volume* vol = 0;
	for (std::vector<Volume* >::const_iterator iter = m_volumes.begin();
			iter != m_volumes.end(); ++iter)
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
				getContext(),
				getContext().newProperty(Utils::Entity::MeshSubVolume, gr_name),
				getContext().newDisplayProperties(Utils::Entity::MeshSubVolume),
				0);
		icmd->addMeshInfoEntity(sv, Internal::InfoCommand::CREATED);
		getContext().newGraphicalRepresentation (*sv);
		add(sv);
	}
	else if (sv->isDestroyed()){
		sv->setDestroyed(false);
		icmd->addMeshInfoEntity(sv,Internal::InfoCommand::ENABLE);
	}

	return sv;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult* MeshManager::newBlocksMesh(std::vector<std::string>& names)
{
    std::vector<Mgx3D::Topo::Block*> entities;
    for(int i=0;i<names.size();i++)
        entities.push_back(getContext().getTopoManager().getBlock(names[i]));
    return newBlocksMesh(entities);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult* MeshManager::newBlocksMesh(std::vector<Mgx3D::Topo::Block*>& blocks)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newBlocksMesh( liste de "<<blocks.size()<<" blocs )";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewBlocksMesh* command = new Mesh::CommandNewBlocksMesh(getContext(), blocks, 0);

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

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return (cmdResult);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult* MeshManager::newFacesMesh(std::vector<std::string>& names)
{
    std::vector<Mgx3D::Topo::CoFace*> entities;
    for(int i=0;i<names.size();i++)
        entities.push_back(getContext().getTopoManager().getCoFace(names[i]));
    return newFacesMesh(entities);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult* MeshManager::newFacesMesh(std::vector<Mgx3D::Topo::CoFace*>& faces)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newFacesMesh( liste de "<<faces.size()<<" faces )";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewFacesMesh* command = new Mesh::CommandNewFacesMesh(getContext(), faces, 0);

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

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult* MeshManager::newAllBlocksMesh()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newAllBlocksMesh()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewBlocksMesh* command = new Mesh::CommandNewBlocksMesh(getContext(),0);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newAllBlocksMesh()";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult* MeshManager::newAllFacesMesh()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newAllFacesMesh()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandNewFacesMesh* command = new Mesh::CommandNewFacesMesh(getContext(),0);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getMeshManager().newAllFacesMesh()";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
MeshManager::newSubVolumeBetweenSheets(std::vector<std::string>& blocks_name, std::string narete,
		int pos1, int pos2, std::string groupName)
{
    std::vector<Mgx3D::Topo::Block*> entities;
    for(int i=0;i<blocks_name.size();i++)
        entities.push_back(getContext().getTopoManager().getBlock(blocks_name[i]));

    return newSubVolumeBetweenSheets(entities,
    		getContext().getTopoManager().getCoEdge(narete),
			pos1, pos2, groupName);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResult*
MeshManager::newSubVolumeBetweenSheets(std::vector<Mgx3D::Topo::Block*>& blocks, Topo::CoEdge* coedge,
		int pos1, int pos2, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"MeshManager::newSubVolumeBetweenSheets( liste de "<<blocks.size()<<" blocs )";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_5));

    Mesh::CommandCreateSubVolumeBetweenSheets* command =
    		new Mesh::CommandCreateSubVolumeBetweenSheets(getContext(), blocks, coedge, pos1, pos2, groupName);

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

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return (cmdResult);
}
/*----------------------------------------------------------------------------*/
void MeshManager::readMli(std::string nom, std::string prefix)
{
    // création de la commande d'écriture du maillage
    CommandReadMLI* command = new CommandReadMLI(getContext(), nom, prefix);

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
    CommandWriteMLI* command = new CommandWriteMLI(getContext(), nom);

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
    CommandWriteVTK* command = new CommandWriteVTK(getContext(), nom);

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
    CommandWriteCGNS* command = new CommandWriteCGNS(getContext(), nom);

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
    CommandModifyMesh* command = new CommandModifyMesh(getContext(), str);

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
    gmds::Mesh& gmdsMesh1 = mesh->getGMDSMesh();

    uint id = mesh->createNewGMDSMesh();
    gmds::Mesh& gmdsMesh2 = mesh->getGMDSMesh(id);

    mesh->readMli(nom, id);

    // on ajoute les groupes de mailles de gmds
    mesh->createGMDSGroups();

    // on pourrait prévoir plus de chose en retour
    Compare2Meshes cmp(&getContext(), gmdsMesh1, gmdsMesh2);
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
int MeshManager::getNbClouds() const
{
    return getCloudsObj().size();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbSurfaces() const
{
    return getSurfacesObj().size();
}
/*----------------------------------------------------------------------------*/
int MeshManager::getNbVolumes() const
{
    return getVolumesObj().size();
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
                    getContext(),
                    oldExplo,
                    inc,
                    getContext().getTopoManager().getCoEdge(narete));

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
                    getContext(),
                    oldExplo);

    /// pas de trace dans le script
    command->setScriptable(asCommand);

    if (true == asCommand)
        getCommandManager().addCommand(command, Utils::Command::DO);

    return command;
}
/*----------------------------------------------------------------------------*/
    Internal::M3DCommandResult* MeshManager::exportBlocksForCGNS(int dim, const std::string& n)
    {
#ifdef _DEBUG2
        std::cout<<"exportBlocks"<<std::endl;
#endif

        //creation de la commande d'exportation
        CommandExportBlocksForCGNS *command = new CommandExportBlocksForCGNS(getContext(), dim, n);

        // trace dans le script
        TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
        cmd << getContextAlias() << "." << "getTopoManager().exportBlocksForCGNS(";
        cmd <<"\""<<n<<"\")";
        command->setScriptCommand(cmd);

        // on passe au gestionnaire de commandes qui exécute la commande en // ou non
        // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
        getCommandManager().addCommand(command, Utils::Command::DO);

        Internal::M3DCommandResult*  cmdResult   =
                new Internal::M3DCommandResult (*command);
        return cmdResult;
    }
/*----------------------------------------------------------------------------*/
template <typename T, typename = std::enable_if_t<std::is_base_of<MeshEntity, T>::value>>
T* MeshManager::findByName(const std::string& name, const std::vector<T*> entities, const bool exceptionIfNotFound) const
{
    T* entity = 0;
    for (T* e : entities)
        if (name == e->getName())
            entity = e;

    if (exceptionIfNotFound && entity == 0) {
        TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << name << " n'a pas été trouvée dans le MeshManager";
        throw TkUtil::Exception(message);
    }

    return entity;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
