/*----------------------------------------------------------------------------*/
/*
 * \file CommandReadMLI.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2015
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandReadMLI.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/SubVolume.h"
#include "Mesh/SubSurface.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/IGMesh.h"
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
        	Group::Group3D* gr = getContext().getLocalGroupManager().getGroup3D(sv->getName(), true);
        	gr->remove(sv);
        	getContext().getLocalMeshManager().remove(sv);
        	sv->clear();
        	delete sv;
        }
        else if (ss){
        	Group::Group2D* gr = getContext().getLocalGroupManager().getGroup2D(ss->getName(), true);
        	gr->remove(ss);
        	getContext().getLocalMeshManager().remove(ss);
        	ss->clear();
        	delete ss;
        }
    }

	MeshImplementation* mesh = dynamic_cast<MeshImplementation*>(getContext().getLocalMeshManager().getMesh());
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
	MeshImplementation* mesh = dynamic_cast<MeshImplementation*>(getContext().getLocalMeshManager().getMesh());
	if (mesh == 0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandReadMLI::internalExecute, mesh == 0", TkUtil::Charset::UTF_8));

	uint id = mesh->createNewGMDSMesh();
	mesh->readMli(m_file_name, id);
	gmds::IGMesh& gmdsMesh = mesh->getGMDSMesh(id);

#ifdef _DEBUG_READ
	std::cout<<" nombre de volumes : "<<gmdsMesh.getNbVolumes()<<std::endl;
#endif
	for (gmds::IGMesh::volumes_iterator iter = gmdsMesh.volumes_begin();
			iter != gmdsMesh.volumes_end(); ++iter){

		gmds::IGMesh::volume vol = *iter;

		std::string nomGr = m_prefix+vol.name();
#ifdef _DEBUG_READ
	std::cout<<" importation du volume : "<<vol.name()<<" avec "<<vol.size()<<" polyèdres"<<std::endl;
#endif

		Mesh::SubVolume* sv= new Mesh::SubVolume(
                getContext(),
                getContext().newProperty(Utils::Entity::MeshSubVolume, nomGr),
                getContext().newDisplayProperties(Utils::Entity::MeshSubVolume),
                id);
		sv->getDisplayProperties().setDisplayed(false);
        getInfoCommand().addMeshInfoEntity(sv, Internal::InfoCommand::CREATED);

        sv->addRegion(vol.cells());
        getContext().newGraphicalRepresentation (*sv);

        Group::Group3D* gr = getContext().getLocalGroupManager().getNewGroup3D(nomGr, &getInfoCommand());

        if (!gr->empty()){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr <<"Le groupe "<<nomGr<<" n'est pas nouveau, cela n'est pas compatible avec la lecture du maillage\nVeuillez utiliser le préfix pour y remédier";
        	throw TkUtil::Exception(messErr);
        }
        gr->add(sv);

        getContext().getLocalMeshManager().add(sv);

	} // end for iter

#ifdef _DEBUG_READ
	std::cout<<" nombre de surfaces : "<<gmdsMesh.getNbSurfaces()<<std::endl;
#endif
	for (gmds::IGMesh::surfaces_iterator iter = gmdsMesh.surfaces_begin();
			iter != gmdsMesh.surfaces_end(); ++iter){

		gmds::IGMesh::surface surf = *iter;

		std::string nomGr = m_prefix+surf.name();
#ifdef _DEBUG_READ
	std::cout<<" importation de la surface : "<<surf.name()<<" avec "<<surf.size()<<" polygones"<<std::endl;
#endif

		Mesh::SubSurface* ss= new Mesh::SubSurface(
                getContext(),
                getContext().newProperty(Utils::Entity::MeshSubSurface, nomGr),
                getContext().newDisplayProperties(Utils::Entity::MeshSubSurface),
                id);
		ss->getDisplayProperties().setDisplayed(false);
        getInfoCommand().addMeshInfoEntity(ss, Internal::InfoCommand::CREATED);

        ss->addFace(surf.cells());
        getContext().newGraphicalRepresentation (*ss);

        Group::Group2D* gr = getContext().getLocalGroupManager().getNewGroup2D(nomGr, &getInfoCommand());

        if (!gr->empty()){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr <<"Le groupe "<<nomGr<<" n'est pas nouveau, cela n'est pas compatible avec la lecture du maillage\nVeuillez utiliser le préfix pour y remédier";
        	throw TkUtil::Exception(messErr);
        }
        gr->add(ss);

        getContext().getLocalMeshManager().add(ss);

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
