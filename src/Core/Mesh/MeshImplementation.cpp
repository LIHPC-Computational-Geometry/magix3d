/*----------------------------------------------------------------------------*/
/*
 * \file MeshImplementation.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 nov. 2011
 */
/*----------------------------------------------------------------------------*/

#include "Internal/ContextIfc.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"
#include "Mesh/CommandModifyMesh.h"

#include "Smoothing/MesquiteMeshImplAdapter.h"
#include "Smoothing/MesquiteDomainImplAdapter.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/MgxException.h"

#include "Internal/Context.h"
#include "Geom/GeomEntity.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/EntityFactory.h"
#include <gmds/io/IGMeshIOService.h>
#include <gmds/io/VTKWriter.h>
#include <gmds/io/VTKReader.h>
#include <gmds/io/LimaWriter.h>
#include <gmds/io/LimaReader.h>

/*----------------------------------------------------------------------------*/
/// OCC
#include <Standard_Failure.hxx>
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>

/*----------------------------------------------------------------------------*/
/// Qualif
#include <GQualif/QualifHelper.h>
#include "Vecteur.h"
#include "Triangle.h"
#include "Quadrangle.h"
#include "Hexaedre.h"
#include "Prisme.h"
#include "Tetraedre.h"
#include "Pyramide.h"

/*----------------------------------------------------------------------------*/
/// Mesquite
#include "MeshInterface.hpp"
#include "QualityAssessor.hpp"
#include "ParallelMeshImpl.hpp"

#include "ShapeImprovementWrapper.hpp"
#include "LaplaceWrapper.hpp"
#include "ShapeImprover.hpp"

#include "PaverMinEdgeLengthWrapper.hpp"
#include "SizeAdaptShapeWrapper.hpp"

/*----------------------------------------------------------------------------*/
/// CGNS
#include "cgnslib.h"

/*----------------------------------------------------------------------------*/
//#define _DEBUG_MESH
//#define _DEBUG_GROUP_BY_TOPO_ENTITY
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MeshImplementation::MeshImplementation(Internal::Context* c)
: MeshItf(c)
{
	m_gmds_mesh.push_back(new gmds::Mesh(MeshItf::TMask3D));

    getGMDSMesh().initializeGeometryClassification();
}
/*----------------------------------------------------------------------------*/
MeshImplementation::~MeshImplementation()
{
	for (uint i=0; i<m_gmds_mesh.size(); i++)
		delete m_gmds_mesh[i];
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::updateMeshDim()
{
#ifdef _DEBUG
		std::cout<<"avant : getDim => "<<m_gmds_mesh[0]->getDim()<<std::endl;
#endif
	//delete m_gmds_mesh[0];
	if (getContext().getMeshDim() == Internal::ContextIfc::MESH2D){
#ifdef _DEBUG
		std::cout<<"new gmds::Mesh(MeshItf::TMask2D) ..."<<std::endl;
#endif
		m_gmds_mesh[0]->changeModel(MeshItf::TMask2D);
		//m_gmds_mesh[0] = new gmds::Mesh(MeshItf::TMask2D);
#ifdef _DEBUG
		std::cout<<"après : getDim => "<<m_gmds_mesh[0]->getDim()<<std::endl;
#endif
	}
	else if (getContext().getMeshDim() == Internal::ContextIfc::MESH3D){
		m_gmds_mesh[0]->changeModel(MeshItf::TMask3D);
		//m_gmds_mesh[0] = new gmds::Mesh(MeshItf::TMask3D);
#ifdef _DEBUG
		std::cout<<"new gmds::Mesh(MeshItf::TMask3D) ..."<<std::endl;
#endif
	}
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, dimension non prévuee pour MeshImplementation::updateDim", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String MeshImplementation::
getInfo(gmds::Node nd)
{
    if (nd.id()==gmds::NullID)
        throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::getInfo ne peut se faire, le noeud n'existe pas (pointeur nul)", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	mess (TkUtil::Charset::UTF_8);

    mess << "["<<nd.X()
            <<", "<<nd.Y()
            <<", "<<nd.Z()<<"] (id "
            << (long)nd.id()<<")";
     return mess;
}
/*----------------------------------------------------------------------------*/
const gmds::Mesh& MeshImplementation::
getGMDSMesh ( ) const
{
#ifdef _DEBUG
	if (m_gmds_mesh.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::getGMDSMesh() ne peut se faire, car m_gmds_mesh vide", TkUtil::Charset::UTF_8));
#endif

	return *m_gmds_mesh[0];
}
/*----------------------------------------------------------------------------*/
gmds::Mesh& MeshImplementation::
getGMDSMesh ( )
{
#ifdef _DEBUG
	if (m_gmds_mesh.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::getGMDSMesh() ne peut se faire, car m_gmds_mesh vide", TkUtil::Charset::UTF_8));
#endif

	return *m_gmds_mesh[0];
}
/*----------------------------------------------------------------------------*/
uint MeshImplementation::
createNewGMDSMesh ( )
{
	m_gmds_mesh.push_back(new gmds::Mesh(MeshItf::TMask3D));

	return m_gmds_mesh.size()-1;
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::
deleteLastGMDSMesh()
{
	gmds::Mesh* last_mesh = m_gmds_mesh.back();
	m_gmds_mesh.pop_back();
	delete last_mesh;
}
/*----------------------------------------------------------------------------*/
gmds::Mesh& MeshImplementation::
getGMDSMesh (uint id)
{
	if (id>=m_gmds_mesh.size())
		throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::getGMDSMesh(.) ne peut se faire, car indice trop grand", TkUtil::Charset::UTF_8));

	return *m_gmds_mesh[id];
}
/*----------------------------------------------------------------------------*/
int MeshImplementation::getNbNodes()
{
	return getGMDSMesh().getNbNodes();
}
/*----------------------------------------------------------------------------*/
int MeshImplementation::getNbEdges()
{
	return getGMDSMesh().getNbEdges();
}
/*----------------------------------------------------------------------------*/
int MeshImplementation::getNbFaces()
{
	return getGMDSMesh().getNbFaces();
}
/*----------------------------------------------------------------------------*/
int MeshImplementation::getNbRegions()
{
	return getGMDSMesh().getNbRegions();
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point MeshImplementation::getCoordNode(gmds::Node nd)
{
#ifdef _DEBUG
    if (nd.id() == gmds::NullID)
        throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::getCoordNode ne peut se faire, le noeud n'existe pas (pointeur nul)", TkUtil::Charset::UTF_8));
#endif
    return Utils::Math::Point(nd.X(), nd.Y(), nd.Z());
}
/*----------------------------------------------------------------------------*/
bool MeshImplementation::createGMDSGroups()
{
    std::cout<<"MeshImplementation::createGMDSGroups()"<<std::endl;
    // création des clouds, lines, surfaces et volumes gmds correspondants aux
    // Mgx3D::Mesh::Cloud, Line, Surface et Volume

    std::vector<Mesh::Cloud*> clouds;
    getContext().getLocalMeshManager().getClouds(clouds);
    std::vector<Mesh::Line*> lines;
    getContext().getLocalMeshManager().getLines(lines);
    std::vector<Mesh::Surface*> surfaces;
    getContext().getLocalMeshManager().getSurfaces(surfaces);
    std::vector<Mesh::Volume*> volumes;
    getContext().getLocalMeshManager().getVolumes(volumes);

    std::vector<gmds::CellGroup<gmds::Node>*> createdGMDSClouds;
    std::vector<gmds::CellGroup<gmds::Edge>*> createdGMDSLines;
    std::vector<gmds::CellGroup<gmds::Face>*> createdGMDSSurfaces;
    std::vector<gmds::CellGroup<gmds::Region>*> createdGMDSVolumes;

    try {

    for(unsigned int iCloud=0; iCloud<clouds.size(); iCloud++) {
        Mesh::Cloud* current_cloud = clouds[iCloud];
        // modification du nom du nuage pour éviter les conflits avec les noms de ligne
        std::string cloudName = current_cloud->getName() + std::string("ND");
        auto cl = getGMDSMesh().newGroup<gmds::Node>(cloudName);
        createdGMDSClouds.push_back(cl);

        std::vector<gmds::Node> nodes;
        current_cloud->getGMDSNodes(nodes);

        for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
            cl->add(nodes[iNode]);
        }
    }

//    std::string lineDefaultName = getContext().getLocalGroupManager().getDefaultName(1);
    for(unsigned int iLine=0; iLine<lines.size(); iLine++) {
        Mesh::Line* current_line = lines[iLine];

//        // on évite les lignes hors groupe s'il y a des groupes pour d'autres lignes
//        if (lines.size() > 1 && current_line->getName() == lineDefaultName)
//            continue;

        auto ln = getGMDSMesh().newGroup<gmds::Edge>(current_line->getName());
        createdGMDSLines.push_back(ln);

        std::vector<gmds::Edge> edges;
        current_line->getGMDSEdges(edges);

        for(unsigned int iEdge=0; iEdge<edges.size(); iEdge++) {
            ln->add(edges[iEdge]);
        }
    }

    for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {
        Mesh::Surface* current_surf = surfaces[iSurf];
        auto su = getGMDSMesh().newGroup<gmds::Face>(current_surf->getName());
        createdGMDSSurfaces.push_back(su);

        std::vector<Topo::CoFace* > coFaces;
        current_surf->getCoFaces(coFaces);

        std::vector<gmds::Face> faces;
        current_surf->getGMDSFaces(faces);

        for(unsigned int iFace=0; iFace<faces.size(); iFace++) {
            su->add(faces[iFace]);
        }
    }

    for(unsigned int iVol=0; iVol<volumes.size(); iVol++) {
        Mesh::Volume* current_vol = volumes[iVol];
        auto vo = getGMDSMesh().newGroup<gmds::Region>(current_vol->getName());
        createdGMDSVolumes.push_back(vo);

        std::vector<Topo::Block* > blocks;
        current_vol->getBlocks(blocks);

        std::vector<gmds::Region> regions;
        current_vol->getGMDSRegions(regions);

        for(unsigned int iRegion=0; iRegion<regions.size(); iRegion++) {
            vo->add(regions[iRegion]);
        }
    }

    } // try
    catch(gmds::GMDSException& e) {
    	std::cerr<<"MeshImplementation::createGMDSGroups "<<e.what()<<std::endl;

    	for(int i=0; i<createdGMDSClouds.size(); i++) {
            getGMDSMesh().deleteGroup<gmds::Node>(createdGMDSClouds[i]);
    	}
        for(int i=0; i<createdGMDSLines.size(); i++) {
            getGMDSMesh().deleteGroup<gmds::Edge>(createdGMDSLines[i]);
        }
    	for(int i=0; i<createdGMDSSurfaces.size(); i++) {
            getGMDSMesh().deleteGroup<gmds::Face>(createdGMDSSurfaces[i]);
    	}
    	for(int i=0; i<createdGMDSVolumes.size(); i++) {
            getGMDSMesh().deleteGroup<gmds::Region>(createdGMDSVolumes[i]);
    	}

    	return false;
    }

    return true;
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::writeMli(std::string nom)
{
    // on ajoute les groupes de mailles de gmds
    bool isCreateGMDSGroupsOK = createGMDSGroups();
    if(!isCreateGMDSGroupsOK) {
    	throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::writeMli ne peut créer les groupes. Vérifiez que la fenêtre qualité est fermée.", TkUtil::Charset::UTF_8));
    }

    try {
		bool	implemented	= false;
		//gmds::IGMeshIOService ioService(&getGMDSMesh());
		//gmds::LimaWriter lw(&ioService);
		gmds::LimaWriter lw(getGMDSMesh());

    	// Lima will work only in meters
    	Utils::Unit::lengthUnit lu = getContext().getLengthUnit();
    	double convertFactorToMeter = 1.;
    	if(lu != Utils::Unit::undefined) {
    		convertFactorToMeter = Utils::Unit::computeMeterFactor(lu);
    	}
    	lw.setLengthUnit(convertFactorToMeter);

		//lw.setCellOptions(gmds::R|gmds::F|gmds::E|gmds::N);
		//lw.setDataOptions(gmds::R|gmds::F|gmds::E|gmds::N);
		//lw.write(nom);
		lw.write(nom, gmds::R|gmds::F|gmds::E|gmds::N);
    	implemented	= true;
		if (false == implemented)
			throw TkUtil::Exception (TkUtil::UTF8String ("Absence d'écrivain GMDS pour le format mli.", TkUtil::Charset::UTF_8));
	} // try
	catch(gmds::GMDSException& e) {
		deleteGMDSGroups();

		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Pb lors de l'export Lima "
				<< ", message remonté : "<<e.what()
				<< "\nCela peut venir d'un chemin incorrect, d'un problème de permissions ou de quota.";
		throw TkUtil::Exception (message);
	}
    // on retire les groupes de mailles de gmds
    deleteGMDSGroups();
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::readVTK(std::string nom, uint id)
{
	try{
		gmds::IGMeshIOService ioService(&getGMDSMesh(id));
		gmds::VTKReader vtkReader(&ioService);
		vtkReader.read(nom);
	}
	catch(gmds::GMDSException& e) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Pb lors de la lecture VTK "
				<< ", message remonté : "<<e.what()
				<< "\nCela peut venir d'un chemin incorrect ou d'un problème de permissions.";
		throw TkUtil::Exception (message);
	}
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::writeVTK(std::string nom)
{
    // on ajoute les groupes de mailles de gmds
	bool isCreateGMDSGroupsOK = createGMDSGroups();
	if(!isCreateGMDSGroupsOK) {
	  	throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::writeVTK ne peut créer les groupes. Vérifiez que la fenêtre qualité est fermée.", TkUtil::Charset::UTF_8));
	}

	try{
		gmds::IGMeshIOService ioService(&getGMDSMesh());
		gmds::VTKWriter vtkWriter(&ioService);
		vtkWriter.setCellOptions(gmds::R|gmds::F|gmds::N);
		vtkWriter.setDataOptions(gmds::R|gmds::F|gmds::N);
		vtkWriter.write(nom);
	}
	catch(gmds::GMDSException& e) {
		deleteGMDSGroups();

		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Pb lors de l'export VTK "
				<< ", message remonté : "<<e.what()
				<< "\nCela peut venir d'un chemin incorrect, d'un problème de permissions ou de quota.";
		throw TkUtil::Exception (message);
	}

    // on retire les groupes de mailles de gmds
    deleteGMDSGroups();
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::writeCGNS(std::string nom)
{
//#define _DEBUG_CGNS
	// NB: bien que la lib cgns soit en C
	//     les indices sont à choisir comme en fortran (à partir de 1)
// REM CP : pour ne pas dépendre de l'option +scoping (sous spack) de CGNS on utilise
// ici la macro CGNS_ENUMV.

	std::vector<Topo::Block*> blocks;
	getContext().getLocalTopoManager().getBlocks(blocks, true);
	gmds::Mesh& gmdsMesh = getGMDSMesh();

	int index_file, icelldim, iphysdim, index_base;

	// cas 3D uniquement pour le moment
	icelldim = 3;
	iphysdim = 3;

	if (cg_open(nom.c_str(), CG_MODE_WRITE, &index_file))
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_open", TkUtil::Charset::UTF_8));

	// un seul "noeud" basename dans l'arbre
	char basename[33];
	strcpy(basename,"BaseMGX3D");

	if (cg_base_write(index_file,basename,icelldim,iphysdim,&index_base))
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_base_write", TkUtil::Charset::UTF_8));

	// on ajoute un à un tous les blocks maillés et structurés
	for (uint i=0; i<blocks.size(); i++){
		Topo::Block* bloc = blocks[i];
		if (bloc->isMeshed() && bloc->isStructured()){

			if (bloc->getNbVertices() != 8){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Erreur dans MeshImplementation::writeCGNS, cas dégénéré non implémenté pour "
						<<bloc->getName();
				throw TkUtil::Exception (message);
			}

			// nombres de bras par direction
			uint ni, nj, nk;
			bloc->getNbMeshingEdges(ni, nj, nk);
			// nombre de noeuds par direction
			ni++; nj++; nk++;
			uint nbNoeuds = ni*nj*nk;

			double* xx = new double[nbNoeuds];
			double* yy = new double[nbNoeuds];
			double* zz = new double[nbNoeuds];

			std::vector<gmds::TCellID>& l_nds = bloc->nodes();
			if (l_nds.size() != nbNoeuds){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Erreur interne dans MeshImplementation::writeCGNS, nombre de noeuds différents "
						<< (short)l_nds.size() << " != "<<(short)nbNoeuds<<" pour "<<bloc->getName();
				throw TkUtil::Exception (message);
			}
			for (uint i=0; i<nbNoeuds; i++){
				gmds::Node current = gmdsMesh.get<gmds::Node>(l_nds[i]);
				xx[i] = current.X();
				yy[i] = current.Y();
				zz[i] = current.Z();
			}

			cgsize_t isize[9];
			char zonename[33];
			int index_coord, index_zone;

			strcpy(zonename, bloc->getName().c_str());

			/* vertex size */
			isize[0]=ni;
			isize[1]=nj;
			isize[2]=nk;
			/* cell size */
			isize[3+0]=isize[0]-1;
			isize[3+1]=isize[1]-1;
			isize[3+2]=isize[2]-1;
			/* boundary vertex size (always zero for structured grids) */
			isize[6+0]=0;
			isize[6+1]=0;
			isize[6+2]=0;

#ifdef _DEBUG_CGNS
			std::cout<<"cg_1to1_write("<<index_file<<", "<<index_base<<", "<<zonename
					<<", ["<<isize[0]<<","<<isize[1]<<","<<isize[2]<<"])"<<std::endl;
#endif
			if (cg_zone_write(index_file,index_base,zonename,isize,CGNS_ENUMT(ZoneType_t)::CGNS_ENUMT(Structured),&index_zone))
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_zone_write", TkUtil::Charset::UTF_8));
#ifdef _DEBUG_CGNS
			std::cout<<" => index_zone = "<<index_zone<<std::endl;
#endif

			if (cg_coord_write(index_file,index_base,index_zone,CGNS_ENUMV(RealDouble),"CoordinateX",xx,&index_coord))
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_coord_write xx", TkUtil::Charset::UTF_8));
			if (cg_coord_write(index_file,index_base,index_zone,CGNS_ENUMV(RealDouble),"CoordinateY",yy,&index_coord))
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_coord_write yy", TkUtil::Charset::UTF_8));
			if (cg_coord_write(index_file,index_base,index_zone,CGNS_ENUMV(RealDouble),"CoordinateZ",zz,&index_coord))
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_coord_write zz", TkUtil::Charset::UTF_8));

			delete [] xx;
			delete [] yy;
			delete [] zz;


			// les relations avec les autres blocs
			std::vector<Topo::CoFace*> cofaces;
			bloc->getCoFaces(cofaces);
			for (uint j=0; j<cofaces.size(); j++){
				// on ne s'occupe que des cofaces entre 2 blocs
				std::vector<Topo::Block*> coface_blocks;
				cofaces[j]->getBlocks(coface_blocks);
				if (coface_blocks.size() == 2){
					Topo::CoFace* coface = cofaces[j];
					Topo::Block* bloc_vois = (coface_blocks[0] == bloc?coface_blocks[1]:coface_blocks[0]);
					cgsize_t ipnts[9]; // plus grand que nécessaire (6), ce qui permet de stocker l'"autre" sommet
					cgsize_t ipntsdonor[9]; // autre sommet utile pour déterminer la transformation
					int itranfrm[3];
					int index_conn;

					// initialisation par défaut ...
					itranfrm[0] = 1;
					itranfrm[1] = 2;
					itranfrm[2] = 3;
					for (uint k=0; k<9; k++){
						ipnts[k] = 0;
						ipntsdonor[k] = 0;
					}

					// recherche des indices de noeuds dans les blocs pour les extrémités (les sommets) des faces communes
					std::vector<Topo::Vertex*> coface_vertices;
					coface->getVertices(coface_vertices);

					// stockage des indices par sommet
					std::vector<std::vector<uint> > idxIJK_vertices;

					// recherche des indices pour les 4 sommets et on renseigne ipnts (indices extrémas)
					for (uint k=0; k<coface_vertices.size(); k++){
						uint idxI, idxJ, idxK;
						gmds::TCellID node_id = coface_vertices[k]->getNode();

						_getIndexNode(node_id,
								bloc->nodes(), ni, nj, nk,
								idxI, idxJ, idxK);
						std::vector<uint> idxIJK;
						idxIJK.push_back(idxI);
						idxIJK.push_back(idxJ);
						idxIJK.push_back(idxK);
						idxIJK_vertices.push_back(idxIJK);
#ifdef _DEBUG_CGNS
						std::cout<<" relation entre "<<bloc->getName()
								<<" et "<<bloc_vois->getName()<<" avec face commune "<<coface->getName()
								<<" ni "<<ni<<", nj "<<nj<<", nk "<<nk
								<<" pour sommet "<<coface_vertices[k]->getName()<<" node_id "<<(long)node_id
								<<" donne idxI "<<idxI<<", idxJ "<<idxJ<<", idxK "<<idxK
								<<std::endl;
#endif

						if (k==0){
							ipnts[0+0] = idxI;
							ipnts[3+0] = idxI;
							ipnts[0+1] = idxJ;
							ipnts[3+1] = idxJ;
							ipnts[0+2] = idxK;
							ipnts[3+2] = idxK;
						}
						else {
							if (ipnts[0+0] > idxI) ipnts[0+0] = idxI;
							if (ipnts[3+0] < idxI) ipnts[3+0] = idxI;
							if (ipnts[0+1] > idxJ) ipnts[0+1] = idxJ;
							if (ipnts[3+1] < idxJ) ipnts[3+1] = idxJ;
							if (ipnts[0+2] > idxK) ipnts[0+2] = idxK;
							if (ipnts[3+2] < idxK) ipnts[3+2] = idxK;
						}

					} // end for k<coface_vertices.size()

					// recherche à partir des extrémas du sommet et donc du noeud correspondant
					uint ipnts1[3];  // tableau intermédiaire qui évite entre autre de définir cgsize_t dans le .h
					uint ipnts2[3];
					for (uint k=0; k<3; k++){
						ipnts1[k] = ipnts[k];
						ipnts2[k] = ipnts[3+k];
					}
					uint ind_min = _getIndiceIJK(idxIJK_vertices, ipnts1);
					uint ind_max = _getIndiceIJK(idxIJK_vertices, ipnts2);

					gmds::TCellID node_min = coface_vertices[ind_min]->getNode();
					gmds::TCellID node_max = coface_vertices[ind_max]->getNode();

					// renseigne les ipntsdonor (indices extrémas dans bloc voisin)
					uint ni_vois, nj_vois, nk_vois;
					bloc_vois->getNbMeshingEdges(ni_vois, nj_vois, nk_vois);
					ni_vois++; nj_vois++; nk_vois++;
					{
						uint idxI, idxJ, idxK;
						_getIndexNode(node_min,
								bloc_vois->nodes(), ni_vois, nj_vois, nk_vois,
								idxI, idxJ, idxK);
						ipntsdonor[0] = idxI;
						ipntsdonor[1] = idxJ;
						ipntsdonor[2] = idxK;
#ifdef _DEBUG_CGNS
						std::cout<<" relation réciproque "<<bloc->getName()
								<<" et "<<bloc_vois->getName()<<" avec face commune "<<coface->getName()
								<<" ni (vois) "<<ni_vois<<", nj "<<nj_vois<<", nk "<<nk_vois
								<<" node_min "<<(long)node_min
								<<" donne idxI "<<idxI<<", idxJ "<<idxJ<<", idxK "<<idxK
								<<std::endl;
#endif

					}
					{
						uint idxI, idxJ, idxK;
						_getIndexNode(node_max,
								bloc_vois->nodes(), ni_vois, nj_vois, nk_vois,
								idxI, idxJ, idxK);
						ipntsdonor[3+0] = idxI;
						ipntsdonor[3+1] = idxJ;
						ipntsdonor[3+2] = idxK;
#ifdef _DEBUG_CGNS
						std::cout<<" relation réciproque "<<bloc->getName()
								<<" et "<<bloc_vois->getName()<<" avec face commune "<<coface->getName()
								<<" ni (vois) "<<ni_vois<<", nj "<<nj_vois<<", nk "<<nk_vois
								<<" node_max "<<(long)node_max
								<<" donne idxI "<<idxI<<", idxJ "<<idxJ<<", idxK "<<idxK
								<<std::endl;
#endif
					}

					char connectname[33];
					strcpy(connectname, coface->getName().c_str());

					char donorname[33];
					strcpy(donorname, bloc_vois->getName().c_str());

					// on met les indices ++ pour être >0
					for (uint k=0; k<6; k++){
						ipnts[k]++;
						ipntsdonor[k]++;
					}

					// calcul de itranfrm
					uint isize_vois[3];
					isize_vois[0] = ni_vois;
					isize_vois[1] = nj_vois;
					isize_vois[2] = nk_vois;
					// un filtre pour identifier les directions traitées
					bool filtre[3] = {false, false, false};
					// idem pour bloc voisin
					bool filtre_vois[3] = {false, false, false};

					// recherche indice dans tableau de 2 valeurs identiques et non marquée dans filtre
					uint ind1, ind2;
					uint val1, val2;
					for (uint k=0; k<3; k++){
						ipnts1[k] = ipnts[k];
						ipnts2[k] = ipnts[3+k];
					}
					_getIndicesIdAndVal(ipnts1, ipnts2, filtre, ind1, val1);
					filtre[ind1] = true;

					for (uint k=0; k<3; k++){
						ipnts1[k] = ipntsdonor[k];
						ipnts2[k] = ipntsdonor[3+k];
					}
					_getIndicesIdAndVal(ipnts1, ipnts2, filtre_vois, ind2, val2);
					filtre_vois[ind2] = true;

#ifdef _DEBUG_CGNS
					std::cout<<"_getIndicesIdAndVal => "<<ind1<<", "<<val1<<std::endl;
					std::cout<<"_getIndicesIdAndVal => "<<ind2<<", "<<val2<<std::endl;
#endif

					// repère si val1 est au min ou au max
					bool is_val1_min;
					if (val1 == 1)
						is_val1_min = true;
					else if (isize[ind1] == val1)
						is_val1_min = false;
					else {
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message << "Erreur dans cg_1to1_write, "
								<< " val1 = "<<(short)val1<<" n'est ni au min ni au max "<<(short)isize[ind1];
						throw TkUtil::Exception (message);
					}

					// idem avec val2
					bool is_val2_min;
					if (val2 == 1)
						is_val2_min = true;
					else if (isize_vois[ind2] == val2)
						is_val2_min = false;
					else {
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message << "Erreur dans cg_1to1_write, "
								<< " val2 = "<<(short)val2<<" n'est ni au min ni au max "<<(short)isize_vois[ind2];
						throw TkUtil::Exception (message);
					}

					// on en déduit une transformation (pas certain que ce ne soit pas la réciproque qu'il faille prendre ....)
					itranfrm[ind2] = ind1+1;
					if (is_val1_min == is_val2_min)
						itranfrm[ind2] = -itranfrm[ind2];


					// recherche d'un troisième sommet / ind_min ind_max
					uint ind_autre = 0;
					if (ind_autre == ind_min || ind_autre == ind_max)
						ind_autre++;
					if (ind_autre == ind_min || ind_autre == ind_max)
						throw TkUtil::Exception (TkUtil::UTF8String ("Ereur interne dans writeCGNS, on ne trouve pas de 3ème indice", TkUtil::Charset::UTF_8));

					gmds::TCellID node_other = coface_vertices[ind_autre]->getNode();

					ipnts[6+0] = idxIJK_vertices[ind_autre][0]+1;
					ipnts[6+1] = idxIJK_vertices[ind_autre][1]+1;
					ipnts[6+2] = idxIJK_vertices[ind_autre][2]+1;

					{
						uint idxI, idxJ, idxK;
						_getIndexNode(node_other,
								bloc_vois->nodes(), ni_vois, nj_vois, nk_vois,
								idxI, idxJ, idxK);
						ipntsdonor[6+0] = idxI+1;
						ipntsdonor[6+1] = idxJ+1;
						ipntsdonor[6+2] = idxK+1;
#ifdef _DEBUG_CGNS
						std::cout<<" relation réciproque "<<bloc->getName()
							     <<" et "<<bloc_vois->getName()<<" avec face commune "<<coface->getName()
							     <<" ni (vois) "<<ni_vois<<", nj "<<nj_vois<<", nk "<<nk_vois
							     <<" node_other "<<(long)node_other
							     <<" donne idxI "<<idxI<<", idxJ "<<idxJ<<", idxK "<<idxK
							     <<std::endl;
#endif
					}


					// recherche itransforme entre sommets ind_min et ind_autre
					for (uint k=0; k<3; k++){
						ipnts1[k] = ipnts[k]; // ind_min
						ipnts2[k] = ipnts[6+k]; // ind_autre
					}
					_getIndicesIdAndVal(ipnts1, ipnts2, filtre, ind1, val1);
					filtre[ind1] = true;

					for (uint k=0; k<3; k++){
						ipnts1[k] = ipntsdonor[k];
						ipnts2[k] = ipntsdonor[6+k];
					}
					_getIndicesIdAndVal(ipnts1, ipnts2, filtre_vois, ind2, val2);
					filtre_vois[ind2] = true;

#ifdef _DEBUG_CGNS
					std::cout<<"_getIndicesIdAndVal => "<<ind1<<", "<<val1<<std::endl;
					std::cout<<"_getIndicesIdAndVal => "<<ind2<<", "<<val2<<std::endl;
#endif

					is_val1_min = true; // de par la sélection de ind_min
					uint ind_vois_min = (ipntsdonor[ind2]<ipntsdonor[ind2+3]?ipntsdonor[ind2]:ipntsdonor[ind2+3]);
					is_val2_min = (val2 == ind_vois_min);

					// on en déduit une transformation (2ème paramètre)
					itranfrm[ind2] = ind1+1;
					if (is_val1_min == is_val2_min)
						itranfrm[ind2] = -itranfrm[ind2];


					// recherche itransforme entre sommets ind_max et ind_autre
					for (uint k=0; k<3; k++){
						ipnts1[k] = ipnts[3+k]; // ind_max
						ipnts2[k] = ipnts[6+k]; // ind_autre
					}
					_getIndicesIdAndVal(ipnts1, ipnts2, filtre, ind1, val1);
					filtre[ind1] = true;

					for (uint k=0; k<3; k++){
						ipnts1[k] = ipntsdonor[3+k];
						ipnts2[k] = ipntsdonor[6+k];
					}
					_getIndicesIdAndVal(ipnts1, ipnts2, filtre_vois, ind2, val2);
					filtre_vois[ind2] = true;

#ifdef _DEBUG_CGNS
					std::cout<<"_getIndicesIdAndVal => "<<ind1<<", "<<val1<<std::endl;
					std::cout<<"_getIndicesIdAndVal => "<<ind2<<", "<<val2<<std::endl;
#endif

					is_val1_min = false; // de par la sélection de ind_max
					ind_vois_min = (ipntsdonor[ind2]<ipntsdonor[ind2+3]?ipntsdonor[ind2]:ipntsdonor[ind2+3]);
					is_val2_min = (val2 == ind_vois_min);

					// on en déduit une transformation (3ème paramètre)
					itranfrm[ind2] = ind1+1;
					if (is_val1_min == is_val2_min)
						itranfrm[ind2] = -itranfrm[ind2];


#ifdef _DEBUG_CGNS
					std::cout<<"cg_1to1_write("<<index_file<<", "<<index_base
							<<", "<<index_zone<<", "<<connectname<<", "<<donorname
							<<", ipnts ["<<ipnts[0]<<","<<ipnts[1]<<","<<ipnts[2]
							<<"] ["<<ipnts[3]<<","<<ipnts[4]<<","<<ipnts[5]
							<<"], ipntsdonor ["<<ipntsdonor[0]<<","<<ipntsdonor[1]<<","<<ipntsdonor[2]
							<<"] ["<<ipntsdonor[3]<<","<<ipntsdonor[4]<<","<<ipntsdonor[5]
							<<"], itranfrm ["<<itranfrm[0]<<","<<itranfrm[1]<<","<<itranfrm[2]
							<<"]"<<std::endl;
#endif

					if (cg_1to1_write(index_file,index_base,index_zone,
							connectname,donorname,
							ipnts,ipntsdonor,itranfrm,
							&index_conn)){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message << "Erreur dans cg_1to1_write\n"
								<< "cg_get_error => "<<cg_get_error();
						throw TkUtil::Exception (message);
					}
#ifdef _DEBUG_CGNS
					std::cout<<" => index_conn = "<<index_conn<<std::endl;
#endif

				} // end if (coface_blocks.size() == 2)
			} // end for j<cofaces.size()

		} // end if (bloc->isMeshed() && bloc->isStructured())
		else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Attention, pas de sauvegarde pour " <<bloc->getName();
		    getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
		}
	} // end for i<blocks.size()

	/* close CGNS file */
	if (cg_close(index_file))
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur dans cg_close", TkUtil::Charset::UTF_8));

} // MeshImplementation::writeCGNS
/*----------------------------------------------------------------------------*/
void MeshImplementation::smooth()
{
    throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::smooth no more implemented.", TkUtil::Charset::UTF_8));
//	std::cout<<std::endl;
//	std::cout<<"MeshImplementation::smooth"<<std::endl;
////	throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::smooth pas encore implémenté.", TkUtil::Charset::UTF_8));
//
//	Mesquite::MsqError err;
//
//	MesquiteMeshImplAdapter* meshAdapter = new MesquiteMeshImplAdapter(
//			getContext().getLocalMeshManager(),
//			getContext().getLocalGeomManager());
//
//	std::map<Mesquite::Mesh::VertexHandle,gmds::Node*>* mesquite2GMDSNodes = meshAdapter->getMesquite2GMDSNodes();
//	std::map<Mesquite::Mesh::ElementHandle,gmds::Mesh<MeshItf::TMask>::Cell*>* mesquite2GMDSCells = meshAdapter->getMesquite2GMDSCells();
//
//	MesquiteDomainImplAdapter* domainAdapter = new MesquiteDomainImplAdapter(
//			getContext().getLocalMeshManager(),
//			getContext().getLocalTopoManager(),
//			*mesquite2GMDSNodes,
//			*mesquite2GMDSCells);
//
//	Mesquite::MeshDomainAssoc* myAssoc = new Mesquite::MeshDomainAssoc(meshAdapter, domainAdapter, false, false, true);
//
////  Mesquite::ShapeImprovementWrapper* wrapper = new Mesquite::ShapeImprovementWrapper();
////	Mesquite::LaplaceWrapper* wrapper = new Mesquite::LaplaceWrapper();
//	Mesquite::ShapeImprover* wrapper = new Mesquite::ShapeImprover();
//
////	Mesquite::PaverMinEdgeLengthWrapper* wrapper = new Mesquite::PaverMinEdgeLengthWrapper(10e-10);
////	Mesquite::SizeAdaptShapeWrapper* wrapper = new Mesquite::SizeAdaptShapeWrapper(10e-10);
//
////	Mesquite::Settings* dummySettings = new Mesquite::Settings();
////	Mesquite::QualityAssessor* qa = new Mesquite::QualityAssessor();
////
////	Mesquite::ParallelMeshImpl* poyop = new Mesquite::ParallelMeshImpl(meshAdapter);
//
//	wrapper->run_instructions(myAssoc,err);
//	MSQ_CHKERR (err);
//
////	wrapper->run_wrapper(
////			myAssoc,
////			poyop,
////			dummySettings,
////			qa,
////			err
////	);
//
//
//
//	std::vector<Mesquite::Mesh::VertexHandle> vertices;
//	meshAdapter->get_all_vertices(vertices,err);
//	if(err.error()) {
//		std::cout<<err.error_message()<<std::endl;
//	}
//
//	Mesquite::Mesh::VertexHandle verticesArray[vertices.size()];
//	for (unsigned int iVertex=0; iVertex<vertices.size(); iVertex++) {
//		verticesArray[iVertex] = vertices[iVertex];
//	}
//	Mesquite::MsqVertex coords[vertices.size()];
//	meshAdapter->vertices_get_coordinates(verticesArray,coords,vertices.size(),err);
//	MSQ_CHKERR (err);
//
//	gmds::Mesh<Mgx3D::Mesh::MeshItf::TMask>::nodes_iterator itn  = getContext().getLocalMeshManager().getMesh()->getGMDSMesh().nodes_begin();
//
//	for (unsigned int iVertex=0; iVertex<vertices.size(); iVertex++) {
//
////		if(mesquite2GMDSNodes->find(vertices[iVertex]) == mesquite2GMDSNodes->end()) {
////			std::cout<<"MeshImplementation::smooth pb with mesquite2GMDSNodes"<<std::endl;
//////			throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::smooth pb with mesquite2GMDSNodes", TkUtil::Charset::UTF_8));
////		}
////
////		gmds::Node* node = (*mesquite2GMDSNodes)[vertices[iVertex]];
//		gmds::Node* current_node = itn->currentItem();
//		current_node->setXYZ(coords[iVertex].x(),coords[iVertex].y(),coords[iVertex].z());
//
//		itn->next();
//	}

}
/*----------------------------------------------------------------------------*/
void MeshImplementation::deleteGMDSGroups()
{
    // destruction des clouds, surfaces et volumes gmds correspondants aux
    // Mgx3D::Mesh::Cloud, Surface et Volume

    std::vector<Mesh::Cloud*> clouds;
    getContext().getLocalMeshManager().getClouds(clouds);
    std::vector<Mesh::Line*> lines;
    getContext().getLocalMeshManager().getLines(lines);
    std::vector<Mesh::Surface*> surfaces;
    getContext().getLocalMeshManager().getSurfaces(surfaces);
    std::vector<Mesh::Volume*> volumes;
    getContext().getLocalMeshManager().getVolumes(volumes);


    for(unsigned int iCloud=0; iCloud<clouds.size(); iCloud++) {
        Mesh::Cloud* current_cloud = clouds[iCloud];

        // modification du nom du nuage pour éviter les conflits avec les noms de ligne
        std::string cloudName = current_cloud->getName() + std::string("ND");

        getGMDSMesh().deleteGroup<gmds::Node>(getGMDSMesh().getGroup<gmds::Node>(cloudName));
    }

    for(unsigned int iLine=0; iLine<lines.size(); iLine++) {
        Mesh::Line* current_line = lines[iLine];

        getGMDSMesh().deleteGroup<gmds::Edge>(getGMDSMesh().getGroup<gmds::Edge>(current_line->getName()));
    }

    for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {
        Mesh::Surface* current_surf = surfaces[iSurf];

        getGMDSMesh().deleteGroup<gmds::Face>(getGMDSMesh().getGroup<gmds::Face>(current_surf->getName()));
    }

    for(unsigned int iVol=0; iVol<volumes.size(); iVol++) {
        Mesh::Volume* current_vol = volumes[iVol];

        getGMDSMesh().deleteGroup<gmds::Region>(getGMDSMesh().getGroup<gmds::Region>(current_vol->getName()));
    }
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::readMli(std::string nom, uint id)
{
    bool implemented = false;
    gmds::IGMeshIOService ioService(&getGMDSMesh(id));
    gmds::LimaReader lr(getGMDSMesh(id));
    lr.read(nom, gmds::R|gmds::F|gmds::E|gmds::N);
    //std::cout<<"MeshImplementation::readMli avec comme unité "<<lr.getLengthUnit()<<std::endl;

    Utils::Unit::lengthUnit luCtx = getContext().getLengthUnit();
    Utils::Unit::lengthUnit luMli = Utils::Unit::computeUnit(lr.getLengthUnit());

    //std::cout<<"luCtx : "<<Utils::Unit::toString(luCtx)<<std::endl;
    //std::cout<<"luMli : "<<Utils::Unit::toString(luMli)<<std::endl;

    if (luCtx == Utils::Unit::undefined && luMli != Utils::Unit::undefined){
        //std::cout<<"setLengthUnit avec "<<Utils::Unit::toString(luMli)<<std::endl;
        getContext().m_length_unit = luMli;
    }
    else if (luCtx != Utils::Unit::undefined
            && luMli != Utils::Unit::undefined
            && luCtx != luMli){
        // il faut mettre le maillage à l'échelle
        double factor = Utils::Unit::computeFactor(luMli, luCtx);
        std::cout<<"factor = "<<factor<<std::endl;

        for (auto itn : getGMDSMesh(id).nodes()) {
            auto current_node = getGMDSMesh(id).get<gmds::Node>(itn);
            current_node.setXYZ(factor*current_node.X(),factor*current_node.Y(),factor*current_node.Z());
        }
    }
    implemented	= true;
    if (false == implemented)
        throw TkUtil::Exception (TkUtil::UTF8String ("Absence de lecteur GMDS pour le format mli.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command,
             std::vector<Topo::Block* >& blocs)
{
#ifdef _DEBUG_MESH
    std::cout <<"MeshImplementation::mesh pour "<<blocs.size()<<" blocs"<<std::endl;
#endif

    for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter)
        mesh(command, *iter);

	TkUtil::UTF8String	message2 (TkUtil::Charset::UTF_8);
    message2 <<"Quelques infos sur le maillage généré: \n";
    message2<< "    Nombre de blocs: "<<(short)blocs.size()<<"\n";
//TODO Ajouter
        message2<< "    Nombre de mailles 3D: "<<(short)getGMDSMesh().getNbRegions()<<"\n";
//    message2<< "     Nombre d'hexaèdres: "<<(short)getGMDSMesh().getNbHexahedra()<<"\n";
//    message2<< "     Nombre de tétraèdres: "<<(short)getGMDSMesh().getNbTetrahedra()<<"\n";
//    message2<< "     Nombre de prismes: "<<(short)getGMDSMesh().getNbPrisms3()<<"\n";
//    message2<< "     Nombre de pyramides: "<<(short)getGMDSMesh().getNbPyramids()<<"\n";
    message2<< "    Nombre de mailles 2D: "<<(short)getGMDSMesh().getNbFaces()<<"\n";
    message2<< "     Nombre de quadrangles: "<<(short)getGMDSMesh().getNbQuadrilaterals()<<"\n";
    message2<< "     Nombre de triangles: "<<(short)getGMDSMesh().getNbTriangles()<<"\n";
    getContext().getLogStream()->log (TkUtil::TraceLog (message2, TkUtil::Log::TRACE_5));
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command,
             std::vector<Topo::CoFace* >& faces)
{
#ifdef _DEBUG_MESH
    std::cout <<"MeshImplementation::mesh pour "<<faces.size()<<" faces communes"<<std::endl;
#endif

    for (std::vector<Topo::CoFace* >::iterator iter = faces.begin();
            iter != faces.end(); ++iter)
        mesh(command, *iter);

	TkUtil::UTF8String	message2 (TkUtil::Charset::UTF_8);
    message2 <<"Quelques infos sur le maillage généré: \n";
    message2<< "    Nombre de mailles 2D: "<<(short)getGMDSMesh().getNbFaces()<<"\n";
    message2<< "      Nombre de quadrangles: "<<(short)getGMDSMesh().getNbQuadrilaterals()<<"\n";
    message2<< "      Nombre de triangles: "<<(short)getGMDSMesh().getNbTriangles()<<"\n";
    getContext().getLogStream()->log (TkUtil::TraceLog (message2, TkUtil::Log::TRACE_5));
}
/*----------------------------------------------------------------------------*/
/// Construction des points du maillage d'un bloc
void MeshImplementation::preMesh(Internal::CommandInternal* command, Topo::Block* bl)
{
	if (!bl->isMeshed() && bl->getMeshLaw() <= Topo::BlockMeshingProperty::transfinite){

        bl->saveBlockMeshingData(&command->getInfoCommand());

        preMeshStrutured(bl);

		bl->getMeshingData()->setPreMeshed(true);
	}
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command, Topo::Block* bl)
{
#ifdef _DEBUG_MESH
    std::cout<<"Maillage du bloc : "<<bl->getName()<<std::endl;
#endif

    if (!bl->isMeshed()){

        if (getContext().getMeshDim() == Internal::ContextIfc::MESH2D)
        	throw TkUtil::Exception (TkUtil::UTF8String ("Il n'est pas possible de mailler des blocs alors que le maillage n'est pas 3D en sortie", TkUtil::Charset::UTF_8));

        if (Topo::BlockMeshingProperty::insertion != bl->getMeshLaw()){

            std::vector<Topo::Face* > faces;
            bl->getFaces(faces);
            for (uint i=0; i<faces.size(); i++) {
                mesh(command, faces[i]);
            }
        }

        bl->saveBlockMeshingData(&command->getInfoCommand());

        if (bl->getMeshLaw() <= Topo::BlockMeshingProperty::transfinite){
            meshStrutured(command,bl);
        }
        else if (bl->getMeshLaw() == Topo::BlockMeshingProperty::delaunayTetgen){
            meshDelaunayTetgen(command,bl);
        }
#ifdef USE_MESHGEMS
        else if (bl->getMeshLaw() == Topo::BlockMeshingProperty::delaunayMeshGemsVol){
                    meshDelaunayMeshGemsVol(command,bl);
                }
#endif	// USE_MESHGEMS
        else if (bl->getMeshLaw() == Topo::BlockMeshingProperty::insertion){
            meshInsertion(command,bl);
        }
        else {
            throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::mesh(Topo::Block* bl) pour une méthode inconnue", TkUtil::Charset::UTF_8));
        }
        bl->getMeshingData()->setMeshed(true);
    } // end if (!bl->isMeshed())
    else {
        if (bl->getMeshLaw() == Topo::BlockMeshingProperty::insertion) {
            throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::mesh(Topo::Block* bl) pour la méthode "
                    "insertion alors que le bloc est déjà maillé", TkUtil::Charset::UTF_8));
        }
    }

}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command, Topo::Face* fa)
{
    std::vector<Topo::CoFace* > cofaces;
    fa->getCoFaces(cofaces);
    for (uint i=0; i<cofaces.size(); i++) {
        mesh(command, cofaces[i]);
    }
}
/*----------------------------------------------------------------------------*/
/// Construction des points du maillage d'une face
void MeshImplementation::preMesh(Internal::CommandInternal* command, Topo::CoFace* fa)
{
	if (!fa->isMeshed() && fa->getMeshLaw() <= Topo::CoFaceMeshingProperty::transfinite){

		fa->saveCoFaceMeshingData(&command->getInfoCommand());

        preMeshStrutured(fa);

        fa->getMeshingData()->setPreMeshed(true);
	}
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
{
    if (!fa->isMeshed()){
#ifdef _DEBUG_MESH
        std::cout <<"Maillage de la face commune "<<fa->getName()<<std::endl;
#endif
//        std::vector<Topo::Edge* > edges;
//        fa->getEdges(edges);
//        for (uint i=0; i<edges.size(); i++)
//            mesh(command, edges[i]);

        fa->saveCoFaceMeshingData(&command->getInfoCommand());

        if (fa->getMeshLaw() <= Topo::CoFaceMeshingProperty::transfinite){
            meshStrutured(command, fa);
        }
        else if (fa->getMeshLaw() == Topo::CoFaceMeshingProperty::delaunayGMSH){
            meshDelaunayGMSH(command, fa);
        }
#ifdef USE_MESHGEMS
        else if (fa->getMeshLaw() == Topo::CoFaceMeshingProperty::MeshGems){
            meshTriangularMeshGems(command, fa);
        }
#endif	// USE_MESHGEMS
        else if (fa->getMeshLaw() == Topo::CoFaceMeshingProperty::quadPairing){
            //meshQuadPairing(command, fa);
        	MGX_NOT_YET_IMPLEMENTED("Appariement");
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        	message << "Méthode appariement non implémenté, pb pour "<<fa->getName();
        	throw TkUtil::Exception (message);
        }
        else {
            MGX_NOT_YET_IMPLEMENTED("MeshImplementation::mesh(Topo::Face* fa) pour une méthode");
        }
        fa->getMeshingData()->setMeshed(true);
    } // end if (!fa->isMeshed())

}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command, Topo::Edge* ed)
{
    std::vector<Topo::CoEdge* > coedges;
    ed->getCoEdges(coedges);
    for (uint i=0; i<coedges.size(); i++)
        mesh(command, coedges[i]);
}
/*----------------------------------------------------------------------------*/
/// Construction des points du maillage d'une arête
void MeshImplementation::preMesh(Internal::CommandInternal* command, Topo::CoEdge* ed)
{
	if (!ed->isMeshed() && !ed->isPreMeshed()){

		ed->saveCoEdgeMeshingData(&command->getInfoCommand());
#ifdef _DEBUG2
		std::cout<<"preMesh pour "<<ed->getName()<<std::endl;
#endif
		// nettoyage du preMesh
		ed->clearPoints();
		// calcul du preMesh
        ed->computePoints();

        ed->getMeshingData()->setPreMeshed(true);
        ed->getMeshingData()->updatePointsTime();
	}
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command, Topo::CoEdge* ed)
{
    if (!ed->isMeshed()){
#ifdef _DEBUG_MESH
        std::cout <<"Maillage de l'arête "<<ed->getName()<<std::endl;
#endif

        if (!ed->isPreMeshed()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        	message << "Erreur interne, les points du maillage structuré non précalculés, pour l'arête \""
        			<< ed->getName()<<"\"";
        	throw TkUtil::Exception (message);
        }

        ed->saveCoEdgeMeshingData(&command->getInfoCommand());

        // on met toujours les noeuds aux extrémités
        if (!ed->getVertex(0)->isMeshed())
        	mesh(command, ed->getVertex(0));
        ed->nodes().push_back(ed->getVertex(0)->getNode());

        const uint nbBrasI = ed->getNbMeshingEdges();
        for (uint i=1; i<nbBrasI; i++){
            const Utils::Math::Point &ptI = ed->points()[i];
            gmds::Node nd = getGMDSMesh().newNode(ptI.getX(), ptI.getY(), ptI.getZ());
            ed->nodes().push_back(nd.id());
            command->addCreatedNode(nd.id());
        }

        if (!ed->getVertex(1)->isMeshed())
        	mesh(command, ed->getVertex(1));
        ed->nodes().push_back(ed->getVertex(1)->getNode());

        // compute nodes position based on the endpoints
        if(ed->getMeshingProperty()->getMeshLaw() == Topo::CoEdgeMeshingProperty::uniforme_smoothfix) {
            gmds::math::Point pt0 = getGMDSMesh().get<gmds::Node>(ed->getVertex(0)->getNode()).point();
            gmds::math::Point pt1 = getGMDSMesh().get<gmds::Node>(ed->getVertex(1)->getNode()).point();
            // It is not necessary to accomodate for meshing direction as 
            // it is not used for mesh nodes order but for thing like 
            // geometric progression
            //if(!ed->getMeshingProperty()->getDirect()) {
            //    pt0 = getGMDSMesh().get<gmds::Node>(ed->getVertex(1)->getNode()).point();
            //    pt1 = getGMDSMesh().get<gmds::Node>(ed->getVertex(0)->getNode()).point();
            //}
            for (uint i=1; i<nbBrasI; i++){
                gmds::math::Point pt = (1. - ((double) i / (double) nbBrasI)) * pt0 + ((double) i / (double) nbBrasI) * pt1;
                gmds::Node node = getGMDSMesh().get<gmds::Node> (ed->nodes()[i]);
                node.setXYZ(pt.X(), pt.Y(), pt.Z());
            }
        }

        // ajoute les noeuds aux groupes suivant ce qui a été demandé
        _addNodesInClouds(command, ed);

        // création des bras
        for (uint i=0; i<nbBrasI; i++){
        	gmds::TCellID id1 = ed->nodes()[i];
        	gmds::TCellID id2 = ed->nodes()[i+1];
        	gmds::Edge edge = getGMDSMesh().newEdge(id1, id2);
			ed->edges().push_back(edge.id());
			command->addCreatedEdge(edge.id());
        }

        // ajoute les bras aux groupes suivant ce qui a été demandé
        _addEdgesInLines(command, ed);

		// nettoyage du preMesh
		ed->clearPoints();
		ed->getMeshingData()->setPreMeshed(false);
		ed->getMeshingData()->setMeshed(true);
    } // end if (!ed->isMeshed())
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::mesh(Mesh::CommandCreateMesh* command, Topo::Vertex* ve)
{
    if (!ve->isMeshed()){
#ifdef _DEBUG_MESH
        std::cout <<"Maillage du sommet "<<ve->getName()<<std::endl;
#endif

        // on utilise le point projeté sur l'entité géométrique de préférence
        Utils::Math::Point pt = ve->getCoord();

        ve->saveVertexMeshingData(&command->getInfoCommand());
        ve->getMeshingData()->setMeshed(true);

        if (ve->getGeomAssociation()){
            Geom::GeomEntity* ge = ve->getGeomAssociation();
            if (ge->getType() == Utils::Entity::GeomSurface
                    || ge->getType() == Utils::Entity::GeomCurve
                    || ge->getType() == Utils::Entity::GeomVertex){
                try {
                    ge->project(pt);
                }
                catch (const Standard_Failure& exc){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "OCC a échoué, projection impossible pour le sommet "
                            << ve->getName() << ", sur "<<ge->getName();
                    throw TkUtil::Exception (message);
                }
            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << "Projection impossible pour le sommet \""
                        << ve->getName() << "\", \n";
                message << "il est projetée sur autre chose qu'un sommet, une courbe ou une surface";
                throw TkUtil::Exception (message);
            }
        } // end if (ve->getGeomAssociation())


        gmds::Node nd = getGMDSMesh().newNode(pt.getX(), pt.getY(), pt.getZ());
        ve->setNode(nd.id());
        command->addCreatedNode(nd.id());
#ifdef _DEBUG_MESH
        std::cout <<"  setNode à "<<nd.id()<<" pour "<<ve->getName()<<std::endl;
#endif

        // ajoute le noeud aux groupes suivant ce qui a été demandé
        _addNodesInClouds(command, ve);

    }
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_addNodesInClouds(Mesh::CommandCreateMesh* command, Topo::CoEdge* ed)
{
    std::vector<std::string> groupsName;
    ed->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les arêtes en mode debug
    groupsName.push_back(ed->getName());
#endif


#ifdef _DEBUG_MESH
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 <<"MeshImplementation::_addNodesInClouds, pour arête "<<ed->getName()
            <<" avec "<<ed->nodes().size()<<" noeuds à mettre dans "<<groupsName.size()<<" groupes ";
    getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_5));
#endif

    // getStrategy() permet l'utilisation pour le cas normal, sinon (cas d'un jalon) on ne fait rien
    if (getContext().getLocalMeshManager().getStrategy() == MeshManager::MODIFIABLE)
        for (size_t i=0; i<groupsName.size(); i++){
            std::string& nom = groupsName[i];

            try {
                getContext().getLocalMeshManager().getCloud(nom);
                command->addModifiedCloud(nom);
            } catch (...) {
                command->addNewCloud(nom);
            }

            Mesh::Cloud* cl = getContext().getLocalMeshManager().getCloud(nom);
            cl->saveMeshCloudTopoProperty(&command->getInfoCommand());
            cl->addCoEdge(ed);
        } // end for i<groupsName.size()
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_addNodesInClouds(Mesh::CommandCreateMesh* command, Topo::Vertex* ve)
{
    std::vector<std::string> groupsName;
    ve->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les sommets en mode debug
    groupsName.push_back(ve->getName());
#endif


#ifdef _DEBUG_MESH
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 <<"MeshImplementation::_addNodesInClouds, pour sommet "<<ve->getName()
            <<" avec 1 noeud à mettre dans "<<groupsName.size()<<" groupes ";
    getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_5));
#endif

    // getStrategy() permet l'utilisation pour le cas normal, sinon (cas d'un jalon) on ne fait rien
    if (getContext().getLocalMeshManager().getStrategy() == MeshManager::MODIFIABLE)
        for (size_t i=0; i<groupsName.size(); i++){
            std::string& nom = groupsName[i];

            try {
                getContext().getLocalMeshManager().getCloud(nom);
                command->addModifiedCloud(nom);
            } catch (...) {
                command->addNewCloud(nom);
            }

            Mesh::Cloud* cl = getContext().getLocalMeshManager().getCloud(nom);
            cl->saveMeshCloudTopoProperty(&command->getInfoCommand());
            cl->addVertex(ve);
        } // end for i<groupsName.size()
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_addEdgesInLines(Mesh::CommandCreateMesh* command, Topo::CoEdge* ed)
{
    std::vector<std::string> groupsName;
    ed->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les arêtes en mode debug
    groupsName.push_back(ed->getName());
#endif


#ifdef _DEBUG_MESH
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 <<"MeshImplementation::_addEdgesInLines, pour arête "<<ed->getName()
            <<" avec "<<ed->edges().size()<<" bras à mettre dans "<<groupsName.size()<<" groupes ";
    getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_5));
#endif

    // getStrategy() permet l'utilisation pour le cas normal, sinon (cas d'un jalon) on ne fait rien
    if (getContext().getLocalMeshManager().getStrategy() == MeshManager::MODIFIABLE)
        for (size_t i=0; i<groupsName.size(); i++){
            std::string& nom = groupsName[i];

            try {
                getContext().getLocalMeshManager().getLine(nom);
                command->addModifiedLine(nom);
            } catch (...) {
                command->addNewLine(nom);
            }

            Mesh::Line* ln = getContext().getLocalMeshManager().getLine(nom);
            ln->saveMeshLineTopoProperty(&command->getInfoCommand());
            ln->addCoEdge(ed);
        } // end for i<groupsName.size()
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_getIndexNode(gmds::TCellID node, std::vector<gmds::TCellID>& nodes,
		uint ni, uint nj, uint nk,
		uint &idxI, uint &idxJ, uint &idxK)
{
	// on fait 6 boucles, une par face
	idxK = 0;
	for (idxI = 0; idxI<ni; idxI++)
		for (idxJ = 0; idxJ<nj; idxJ++)
			if (nodes[idxI + idxJ*ni + idxK*ni*nj] == node)
				return;

	idxK = nk-1;
	for (idxI = 0; idxI<ni; idxI++)
		for (idxJ = 0; idxJ<nj; idxJ++)
			if (nodes[idxI + idxJ*ni + idxK*ni*nj] == node)
				return;

	idxJ = 0;
	for (idxI = 0; idxI<ni; idxI++)
		for (idxK = 0; idxK<nk; idxK++)
			if (nodes[idxI + idxJ*ni + idxK*ni*nj] == node)
				return;

	idxJ = nj-1;
	for (idxI = 0; idxI<ni; idxI++)
		for (idxK = 0; idxK<nk; idxK++)
			if (nodes[idxI + idxJ*ni + idxK*ni*nj] == node)
				return;

	idxI = 0;
	for (idxK = 0; idxK<nk; idxK++)
		for (idxJ = 0; idxJ<nj; idxJ++)
			if (nodes[idxI + idxJ*ni + idxK*ni*nj] == node)
				return;

	idxI = ni-1;
	for (idxK = 0; idxK<nk; idxK++)
		for (idxJ = 0; idxJ<nj; idxJ++)
			if (nodes[idxI + idxJ*ni + idxK*ni*nj] == node)
				return;

#ifdef _DEBUG_CGNS
	std::cout<<"MeshImplementation::_getIndexNode("<<node<<", [";
	for (uint i=0; i<nodes.size(); i++)
		std::cout<<nodes[i]<<" ";
	std::cout<<" ], "<<ni<<", "<<nj<<", "<<nk<<")"<<std::endl;
#endif

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "MeshImplementation::_getIndexNode a échouer pour trouver le noeud d'id "<<(long)node;
    throw TkUtil::Exception (message);
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_getIndicesIdAndVal(uint* ipnts1, uint* ipnts2, bool* filtre,
    		uint &ind, uint &val)
{
#ifdef _DEBUG_CGNS
	std::cout<<"MeshImplementation::_getIndicesIdAndVal([ ";
	for (uint i=0; i<3; i++)
		std::cout<<ipnts1[i]<<" ";
	std::cout<<"], [ ";
	for (uint i=0; i<3; i++)
		std::cout<<ipnts2[i]<<" ";
	std::cout<<"], [ ";
	for (uint i=0; i<3; i++)
		std::cout<<(filtre[i]?"True ":"False ");
	std::cout<<"])"<<std::endl;
#endif

	for (uint i=0; i<3; i++){
		if ((filtre[i]==false) && (ipnts1[i] == ipnts2[i])){
			ind = i;
			val = ipnts1[i];
			return;
		}
	}

    throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::_getIndicesIdAndVal a échouer pour trouver des valeurs identiques ", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
uint MeshImplementation::_getIndiceIJK(std::vector<std::vector<uint> >& idxIJK_vertices,
		uint* idxIJK)
{
	for (uint i=0; i<idxIJK_vertices.size(); i++){
		bool ok = true;
		for (uint j=0; j<3; j++)
			if (idxIJK_vertices[i][j] != idxIJK[j])
				ok = false;
		if (ok)
			return i;
	}

	throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::_getIndiceIJK a échouer pour trouver un triplet", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::undoCreatedMesh(Mesh::CommandCreateMesh* command)
{
    // destruction des entités
    std::vector<gmds::TCellID>& regions = command->createdRegions();
    for (uint i=0; i<regions.size(); i++)
        getGMDSMesh().deleteRegion(regions[i]);

    std::vector<gmds::TCellID>& faces = command->createdFaces();
    for (uint i=0; i<faces.size(); i++)
        getGMDSMesh().deleteFace(faces[i]);

    std::vector<gmds::TCellID>& edges = command->createdEdges();
    for (uint i=0; i<edges.size(); i++)
        getGMDSMesh().deleteEdge(edges[i]);

    std::vector<gmds::TCellID>& nodes = command->createdNodes();
    for (uint i=0; i<nodes.size(); i++)
        getGMDSMesh().deleteNode(nodes[i]);
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::deleteMesh()
{
    getGMDSMesh().clear();
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
