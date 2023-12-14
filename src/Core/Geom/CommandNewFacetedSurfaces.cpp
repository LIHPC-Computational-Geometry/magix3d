/*----------------------------------------------------------------------------*/
/** \file CommandNewFacetedSurfaces.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 26/02/2018
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewFacetedSurfaces.h"
#include "Geom/EntityFactory.h"
#include "Utils/MgxException.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"

#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/MeshHelper.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include "gmds/ig/Mesh.h"
/*----------------------------------------------------------------------------*/
//#define _DEBUG_READ
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewFacetedSurfaces::CommandNewFacetedSurfaces(Internal::Context& c,
		std::string& nom)
:CommandCreateGeom(c, "Création d'une ou plusieurs surfaces facétisées", "")
, m_file_name(nom)
{}
/*----------------------------------------------------------------------------*/
CommandNewFacetedSurfaces::
~CommandNewFacetedSurfaces()
{
	Mesh::MeshImplementation* mesh = dynamic_cast<Mesh::MeshImplementation*>(getContext().getLocalMeshManager().getMesh());
	if (mesh == 0)
		std::cerr<<"Erreur interne dans CommandNewFacetedSurfaces::~CommandNewFacetedSurfaces, mesh == 0"<<std::endl;
	else
		mesh->deleteLastGMDSMesh();
}
/*----------------------------------------------------------------------------*/
void CommandNewFacetedSurfaces::
internalExecute()
{
#ifdef _DEBUG_READ
		std::cout<<"CommandNewFacetedSurfaces::internalExecute()"<<endl;
#endif

	Mesh::MeshImplementation* mesh = dynamic_cast<Mesh::MeshImplementation*>(getContext().getLocalMeshManager().getMesh());
	if (mesh == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne dans CommandNewFacetedSurfaces::internalExecute, mesh == 0", TkUtil::Charset::UTF_8));

	uint id = mesh->createNewGMDSMesh();

	// l'extension (.mli ou .vtk)
	std::string suffix = m_file_name;
	int suffix_start = m_file_name.find_last_of(".");
	suffix.erase(0,suffix_start+1);

	if (suffix == "vtk" || suffix == "vtp" || suffix == "vtu"){
#ifdef _DEBUG_READ
		std::cout<<"CommandNewFacetedSurfaces avec format VTK ("<<suffix<<")"<<endl;
#endif
		mesh->readVTK(m_file_name, id);
	}
	else {
#ifdef _DEBUG_READ
		std::cout<<"CommandNewFacetedSurfaces avec format MLI ("<<suffix<<")"<<endl;
#endif
		mesh->readMli(m_file_name, id);
	}
	gmds::Mesh& gmdsMesh = mesh->getGMDSMesh(id);


#ifdef _DEBUG_READ
	std::cout<<" nombre de surfaces : "<<gmdsMesh.getNbSurfaces()<<std::endl;
	std::cout<<" nombre de lignes : "<<gmdsMesh.getNbLines()<<std::endl;
	std::cout<<" nombre de nuages : "<<gmdsMesh.getNbClouds()<<std::endl;
#endif

	// création des surfaces

	for(auto i=0; i< gmdsMesh.getNbGroups<gmds::Face>(); i++) {
		auto surf = gmdsMesh.getGroup<gmds::Face>(i);
		std::string nomGr = surf->name();
#ifdef _DEBUG_READ
		std::cout<<" importation de la surface : "<<iter.name()<<" avec "<<iter.size()<<" polygones"<<std::endl;
#endif

		Group::Group2D* gr = getContext().getLocalGroupManager().getNewGroup2D(nomGr, &getInfoCommand());

		if (!gr->empty()){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr <<"Le groupe "<<nomGr<<" n'est pas nouveau, cela n'est pas compatible avec la lecture du maillage\nVeuillez utiliser le préfix pour y remédier";
			throw TkUtil::Exception(messErr);
		}

		std::vector<gmds::Face> faces;
		for (auto face_id : surf->cells())
		{
			auto face = gmdsMesh.get<gmds::Face>(face_id);
				faces.push_back(face);
		}

		Geom::Surface* sf = EntityFactory(getContext()).newFacetedSurface(id, faces);

		gr->add(sf);
		sf->add(gr);

		m_createdEntities.push_back(sf);

		// stockage dans le manager géom
	    store(sf);

	    getInfoCommand().addGroupInfoEntity(gr,Internal::InfoCommand::DISPMODIFIED);

	} // end for iter


	// création des courbes et sommets à partir des lignes
	std::map<uint, Geom::Vertex*> nd2vtx;

	for(auto i=0; i< gmdsMesh.getNbGroups<gmds::Edge>(); i++) {
		auto ligne = gmdsMesh.getGroup<gmds::Edge>(i);
		std::string nomGr = ligne->name();

#ifdef _DEBUG_READ
		std::cout<<" importation de la ligne : "<<ligne->name()<<" avec "<<ligne->size()<<" bras"<<std::endl;
#endif

		Group::Group1D* gr = getContext().getLocalGroupManager().getNewGroup1D(nomGr, &getInfoCommand());

		if (!gr->empty()){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr <<"Le groupe "<<nomGr<<" n'est pas nouveau, cela n'est pas compatible avec la lecture du maillage\nVeuillez utiliser le préfix pour y remédier";
			throw TkUtil::Exception(messErr);
		}

		// recherche d'une liste de noeuds ordonnés

		std::vector<gmds::Edge> edges;
		for (auto edge_id : ligne->cells())
		{
			auto edge = gmdsMesh.get<gmds::Edge>(edge_id);
				edges.push_back(edge);
		}

		std::vector<uint> nodes_id = Mesh::MeshHelper::getOrderedNodesId(edges);

		std::vector<gmds::Node> nodes;
		for (uint i=0; i<nodes_id.size(); i++)
			nodes.push_back(gmdsMesh.get<gmds::Node>(nodes_id[i]));

		Geom::Curve* cv = EntityFactory(getContext()).newFacetedCurve(id, nodes);

		gr->add(cv);
		cv->add(gr);

		m_createdEntities.push_back(cv);

		// stockage dans le manager géom
		store(cv);

		getInfoCommand().addGroupInfoEntity(gr,Internal::InfoCommand::DISPMODIFIED);

		// création des vertex aux extrémités
		gmds::Node nd = nodes[0];
		Geom::Vertex* vtx = nd2vtx[nd.id()];
		if (vtx == 0){
			vtx = EntityFactory(getContext()).newFacetedVertex(id, nd);
			m_createdEntities.push_back(vtx);
			store(vtx);
			nd2vtx[nd.id()] = vtx;
			cv->add(vtx);
			vtx->add(cv);
		}
		nd = nodes.back();
		vtx = nd2vtx[nd.id()];
		if (vtx == 0){
			vtx = EntityFactory(getContext()).newFacetedVertex(id, nd);
			m_createdEntities.push_back(vtx);
			store(vtx);
			nd2vtx[nd.id()] = vtx;
			cv->add(vtx);
			vtx->add(cv);
		}

	} // end for iter

	// mise à jour des dépendances crv <-> surf

	gmds::TInt mark = gmdsMesh.newMark<gmds::Node>();

	for(auto i=0; i< gmdsMesh.getNbGroups<gmds::Face>(); i++) {
		auto surf = gmdsMesh.getGroup<gmds::Face>(i);

		for (auto face_id : surf->cells()) {
			std::vector<gmds::Node> nodes;
			gmds::Face face = gmdsMesh.get<gmds::Face>(face_id);
			face.get(nodes);
			for(unsigned int iNode=0; iNode<nodes.size(); iNode++)
				gmdsMesh.mark(nodes[iNode], mark);
		}

		std::string nomGrSurf = surf->name();
		Group::Group2D* grsurf = getContext().getLocalGroupManager().getGroup2D(nomGrSurf, true);
		Geom::Surface* sf = 0;
		std::vector<Geom::Surface*>& geomSurfs = grsurf->getSurfaces();
		if (geomSurfs.size() == 1)
			sf = geomSurfs[0];
		else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr <<"Le groupe "<<nomGrSurf<<" contient plusieurs surfaces";
			throw TkUtil::Exception(messErr);
		}

		for(auto i=0; i< gmdsMesh.getNbGroups<gmds::Edge>(); i++) {
			auto ligne = gmdsMesh.getGroup<gmds::Edge>(i);
			bool isInSurface = true;

			std::vector<gmds::Node> nodes;
			for (auto edge_id : ligne->cells())
			{
				auto edge = gmdsMesh.get<gmds::Edge>(edge_id);
				for (auto node : edge.get<gmds::Node>())
					nodes.push_back(node);
			}

			for (uint i=0; i<nodes.size(); i++)
				if (!gmdsMesh.isMarked(nodes[i], mark))
					isInSurface = false;

			if (isInSurface){
				std::string nomGr = ligne->name();
				if (ligne->size() > 1){
					Group::Group1D* gr = getContext().getLocalGroupManager().getGroup1D(nomGr, true);

					Geom::Curve* crv = 0;
					std::vector<Geom::Curve*>& geomCurves = gr->getCurves();
					if (geomCurves.size() == 1)
						crv = geomCurves[0];
					else {
						TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
						messErr <<"Le groupe "<<nomGr<<" contient plusieurs courbes";
						throw TkUtil::Exception(messErr);
					}
					sf->add(crv);
					crv->add(sf);
				}
			} // end if (isInSurface)

		} // end for iter2

		gmdsMesh.unmarkAll<gmds::Node>(mark);

	} // end for iter1 = gmdsMesh.surfaces_begin()

	gmdsMesh.freeMark<gmds::Node>(mark);

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
