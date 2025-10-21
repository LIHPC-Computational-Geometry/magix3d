/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Mesh/CommandReadMLI.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/SubVolume.h"
#include "Mesh/SubSurface.h"
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_READ
/*----------------------------------------------------------------------------*/
CommandReadMLI::CommandReadMLI(Internal::Context& c, std::string& nom,  std::string& prefix)
: Internal::CommandInternal (c, std::string("Lecture du maillage ")+nom)
, m_file_name(nom)
, m_prefix(prefix)
{
    getInfoCommand ( ).addContextInfo (Internal::InfoCommand::LENGTHUNITMODIFIED);
}
/*----------------------------------------------------------------------------*/
CommandReadMLI::~CommandReadMLI()
{
#ifdef _DEBUG_READ
	std::cout<<"CommandReadMLI::~CommandReadMLI()"<<std::endl;
#endif
	// suppression des sous-volumes et déréférencement dans LocalMeshManager
    std::vector <Internal::InfoCommand::MeshEntityInfo>&  mesh_entities_info = getInfoCommand().getMeshInfoEntities();
    for (std::vector <Internal::InfoCommand::MeshEntityInfo>::iterator iter_mei = mesh_entities_info.begin();
            iter_mei != mesh_entities_info.end(); ++iter_mei){
        Mesh::MeshEntity* me = (*iter_mei).m_mesh_entity;
        Mesh::SubVolume* sv = dynamic_cast<Mesh::SubVolume*>(me);
        Mesh::SubSurface* ss = dynamic_cast<Mesh::SubSurface*>(me);
        if (sv){
        	Group::Group3D* gr = getContext().getGroupManager().getGroup<Group::Group3D>(sv->getName(), true);
        	gr->remove(sv);
        	getContext().getMeshManager().remove(sv);
        	sv->clear();
        	delete sv;
        }
        else if (ss){
        	Group::Group2D* gr = getContext().getGroupManager().getGroup<Group::Group2D>(ss->getName(), true);
        	gr->remove(ss);
        	getContext().getMeshManager().remove(ss);
        	ss->clear();
        	delete ss;
        }
    }

	MeshImplementation* mesh = dynamic_cast<MeshImplementation*>(getContext().getMeshManager().getMesh());
	if (mesh == 0)
		std::cerr<<"Erreur interne dans CommandReadMLI::~CommandReadMLI, mesh == 0"<<std::endl;
	else
		mesh->deleteLastGMDSMesh();
}
/*----------------------------------------------------------------------------*/
void CommandReadMLI::internalExecute()
{
#ifdef _DEBUG_READ
	std::cout<<"CommandReadMLI pour "<<m_file_name<<", "<<m_prefix<<std::endl;
#endif
	MeshImplementation* mesh = dynamic_cast<MeshImplementation*>(getContext().getMeshManager().getMesh());
	if (mesh == 0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandReadMLI::internalExecute, mesh == 0", TkUtil::Charset::UTF_8));

	uint id = mesh->createNewGMDSMesh();
	mesh->readMli(m_file_name, id);
	gmds::Mesh& gmdsMesh = mesh->getGMDSMesh(id);

#ifdef _DEBUG_READ
	std::cout<<" nombre de volumes : "<<gmdsMesh.getNbGroups<gmds::Region>()<<std::endl;
#endif
	for(auto i=0; i< gmdsMesh.getNbGroups<gmds::Region>(); i++) {
		auto vol = gmdsMesh.getGroup<gmds::Region>(i);
		std::string nomGr = m_prefix+vol->name();
#ifdef _DEBUG_READ
	std::cout<<" importation du volume : "<<vol->name()<<" avec "<<vol->size()<<" polyèdres"<<std::endl;
#endif

		Mesh::SubVolume* sv= new Mesh::SubVolume(
                getContext(),
                getContext().newProperty(Utils::Entity::MeshSubVolume, nomGr),
                getContext().newDisplayProperties(Utils::Entity::MeshSubVolume),
                id);
		sv->getDisplayProperties().setDisplayed(false);
        getInfoCommand().addMeshInfoEntity(sv, Internal::InfoCommand::CREATED);

		for (auto region_id : vol->cells())
		{
			auto region = gmdsMesh.get<gmds::Region>(region_id);
				sv->addRegion(region);
		}

        getContext().newGraphicalRepresentation (*sv);

        Group::Group3D* gr = getContext().getGroupManager().getNewGroup<Group::Group3D>(nomGr, &getInfoCommand());

        if (!gr->empty()){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr <<"Le groupe "<<nomGr<<" n'est pas nouveau, cela n'est pas compatible avec la lecture du maillage\nVeuillez utiliser le préfix pour y remédier";
        	throw TkUtil::Exception(messErr);
        }
        gr->add(sv);

        getContext().getMeshManager().add(sv);

	} // end for iter

#ifdef _DEBUG_READ
	std::cout<<" nombre de surfaces : "<<gmdsMesh.getNbGroups<gmds::Face>()<<std::endl;
#endif

	for(auto i=0; i< gmdsMesh.getNbGroups<gmds::Face>(); i++) {
		auto surf = gmdsMesh.getGroup<gmds::Face>(i);
		std::string nomGr = m_prefix+surf->name();
#ifdef _DEBUG_READ
	std::cout<<" importation de la surface : "<<surf->name()<<" avec "<<surf->size()<<" polygones"<<std::endl;
#endif

		Mesh::SubSurface* ss= new Mesh::SubSurface(
                getContext(),
                getContext().newProperty(Utils::Entity::MeshSubSurface, nomGr),
                getContext().newDisplayProperties(Utils::Entity::MeshSubSurface),
                id);
		ss->getDisplayProperties().setDisplayed(false);
        getInfoCommand().addMeshInfoEntity(ss, Internal::InfoCommand::CREATED);


		for (auto face_id : surf->cells())
		{
			auto face = gmdsMesh.get<gmds::Face>(face_id);
			ss->addFace(face);
		}

        getContext().newGraphicalRepresentation (*ss);

        Group::Group2D* gr = getContext().getGroupManager().getNewGroup<Group::Group2D>(nomGr, &getInfoCommand());

        if (!gr->empty()){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr <<"Le groupe "<<nomGr<<" n'est pas nouveau, cela n'est pas compatible avec la lecture du maillage\nVeuillez utiliser le préfix pour y remédier";
        	throw TkUtil::Exception(messErr);
        }
        gr->add(ss);

        getContext().getMeshManager().add(ss);

	} // end for iter
}
/*----------------------------------------------------------------------------*/
void CommandReadMLI::internalUndo()
{
	// les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandReadMLI::internalRedo()
{
	// les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
