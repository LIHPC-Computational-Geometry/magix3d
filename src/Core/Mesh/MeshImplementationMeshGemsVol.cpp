/*----------------------------------------------------------------------------*/
/*
 * \file MeshImplementationMeshGemsVol.cpp
 *
 *  \author Nicolas Le Goff
 *
 *  \date 14 fev. 2019
 */
/*----------------------------------------------------------------------------*/
#ifdef USE_MESHGEMS	// CP, portage CMake. Ne peut compiler sans meshgems car requiert statut_t, STATUS_OK, ...

#include <array>
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/TopoHelper.h"
#include "Utils/Common.h"
#include "Internal/Context.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <Topo/BlockMeshingPropertyDelaunayMeshGems.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#ifdef USE_MESHGEMS
extern "C" {
#include <meshgems/meshgems.h>
#include <meshgems/tetra.h>
}

// This is for the license key
#include <meshgems/meshgems_key_9624.h>
#endif
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
// temporary struct that will store the boundary triangles
// we will have to implement the callbacks
struct _bnd_mesh_data_t {

	int nbv; // number of vertices
	int nbe; // number of edges / ridges
	int nbt; // number of triangles

	std::vector<std::array<double, 3> > xyz; // vertices coords

	std::vector<std::array<int, 2> > e2n_id; // vertices ids of the edges / ridges
	std::vector<std::array<int, 3> > f2n_id; // vertices ids of the triangles
};

status_t mgtm_get_vertex_count ( integer * nbvtx , void * user_data )
{
	_bnd_mesh_data_t * mm = (_bnd_mesh_data_t *) user_data ;
	 * nbvtx = mm->nbv;
	 /* the number of vertices in your mesh */

	 return STATUS_OK ;
}

status_t mgtm_get_vertex_coordinates ( integer ivtx , real * xyz , void * user_data )
{
	_bnd_mesh_data_t * mm = (_bnd_mesh_data_t *) user_data ;
	xyz[0] = mm->xyz[ivtx][0];
	xyz[1] = mm->xyz[ivtx][1];
	xyz[2] = mm->xyz[ivtx][2];

	return STATUS_OK ;
}

status_t mgtm_get_edge_count ( integer * nbedge , void * user_data )
{
	_bnd_mesh_data_t * mm = (_bnd_mesh_data_t *) user_data ;
	*nbedge = mm->nbe;
	/* the number of edges in your mesh */

	return STATUS_OK ;
}

status_t mgtm_get_edge_vertices ( integer iedge , integer * vedge , void * user_data )
{
	_bnd_mesh_data_t * mm = (_bnd_mesh_data_t *) user_data ;
	vedge[0] = mm->e2n_id[iedge][0];
	vedge[1] = mm->e2n_id[iedge][1];
	/* vertex indices of the iedge ’ th edge */

	return STATUS_OK ;
}

status_t mgtm_get_triangle_count ( integer * nbtri , void * user_data )
{
	_bnd_mesh_data_t * mm = (_bnd_mesh_data_t *) user_data ;
	*nbtri = mm->nbt;
	/* the number of triangles in your mesh */

	return STATUS_OK ;
}

status_t mgtm_get_triangle_vertices ( integer itri , integer * vtri , void * user_data )
{
	_bnd_mesh_data_t * mm = (_bnd_mesh_data_t *) user_data ;
	vtri[0] = mm->f2n_id[itri][0];
	vtri[1] = mm->f2n_id[itri][1];
	vtri[2] = mm->f2n_id[itri][2];
	/* the vertex indices of the itri ’ th triangle */

	return STATUS_OK ;
}



/*----------------------------------------------------------------------------*/
void MeshImplementation::
meshDelaunayMeshGemsVol(Mesh::CommandCreateMesh* command, Topo::Block* bl)
{
#ifdef USE_MESHGEMS
#ifdef _DEBUG_MESH
    std::cout <<"Maillage du bloc "<<bl->getName()<<" avec la méthode de Delaunay (version MeshGemsVol)"<<std::endl;
#endif

    // TODO [FL] TEMPORAIRE POUR LES PYRAMIDES
    std::vector<Topo::Vertex* > topo_vertices;
    bl->getVertices(topo_vertices);
    double x=0, y=0, z=0;
    for(unsigned int i=0;i<topo_vertices.size();i++){
        Topo::Vertex*  vi = topo_vertices[i];
        x+=vi->getX();
        y+=vi->getY();
        z+=vi->getZ();
    }
    x/=topo_vertices.size();
    y/=topo_vertices.size();
    z/=topo_vertices.size();
    gmds::math::Point block_center(x,y,z);
    // on recupere les faces topologiques
    std::vector<Topo::Face* >faces;
    bl->getFaces(faces);
    std::vector<std::vector<gmds::Face> > mesh_faces;
    std::vector<gmds::Face> mesh_faces_in1Surf;
    std::vector<gmds::Node> mesh_nodes;
    int node_mark = getGMDSMesh().newMark<gmds::Node>();

    std::vector<gmds::Region> pyramids;

    Topo::BlockMeshingPropertyDelaunayMeshGems* meshingProp =
        		dynamic_cast<Topo::BlockMeshingPropertyDelaunayMeshGems*>(bl->getBlockMeshingProperty());


    double ratio_hauteur_pyramides = meshingProp->getRatioPyramidSize();

    //std::cout<<"ratio_hauteur_pyramides = "<<ratio_hauteur_pyramides<<std::endl;

    for(unsigned int i=0;i<faces.size();i++){
        std::vector<Topo::CoFace* > cofaces;
        faces[i]->getCoFaces(cofaces);
        for(unsigned int k=0;k<cofaces.size();k++){

            std::vector<gmds::Face> local_mesh_faces;

            // pour chaque face topologique, on regarde si la face est maille en triangles
            // Si oui, on ne fait rien, sinon, on ajoute des pyramides. Dans les
            // 2 cas, on remplit une structure pour le maillage via netgen

            std::vector<gmds::TCellID> face_elem = cofaces[k]->faces();
#ifdef _DEBUG_MESH
            std::cout<<"NB FACES FOR COFACE "<<k<<" -> "<<face_elem.size()<<std::endl;
#endif
            if (cofaces[k]->isStructured()){
#ifdef _DEBUG_MESH
            	std::cout<<"--> Surface de contact entre maillages hex. et tet."<<std::endl;
#endif
            	// on cree 1 pyramide dans le maillage
            	bool withPyramides = true;
            	for(unsigned int j=0;j<face_elem.size();j++){
            		gmds::Face q = getGMDSMesh().get<gmds::Face>(face_elem[j]);
                    if (q.type() == gmds::GMDS_QUAD){
            			// q est un QUAD
            			std::vector<gmds::Node> nodes = q.getAll<gmds::Node>();

            			gmds::Node n1 = nodes[0];
            			gmds::Node n2 = nodes[1];
            			gmds::Node n3 = nodes[3];
            			gmds::Node n4 = nodes[2];

            			if(!withPyramides){
            				gmds::Face t1 = getGMDSMesh().newTriangle(n1,n2,n3);
            				gmds::Face t2 = getGMDSMesh().newTriangle(n2,n3,n4);
            				local_mesh_faces.push_back(t1);
            				local_mesh_faces.push_back(t2);
            				mesh_faces_in1Surf.push_back(t1);
            				mesh_faces_in1Surf.push_back(t2);
            			}
            			else {
                            gmds::math::Vector3d v1(n2.point()-n1.point());
                            gmds::math::Vector3d v3(n3.point()-n1.point());
                            gmds::math::Vector3d nq = ratio_hauteur_pyramides*(v1.cross(v3));

                            gmds::math::Vector3d ref(block_center-n1.point());

            				gmds::Node p0, p1, p2,  p3, p4;
            				if(nq.dot(ref)>0.0){
            					// on suit le sens direct de la maille
            					p0=nodes[0];
            					p1=nodes[1];
            					p2=nodes[2];
            					p3=nodes[3];
            					gmds::math::Point center = q.center();
            					p4 = getGMDSMesh().newNode(
            							center.X()+nq[0],
            							center.Y()+nq[1],
            							center.Z()+nq[2]);
            				}
            				else{
            					//on inverse
            					p0=nodes[0];
            					p1=nodes[3];
            					p2=nodes[2];
            					p3=nodes[1];
                                gmds::math::Point center = q.center();
            					p4 = getGMDSMesh().newNode(center.X()-nq[0],
            							center.Y()-nq[1],
            							center.Z()-nq[2]);
            				}

            				pyramids.push_back(getGMDSMesh().newPyramid(p0,p1,p2,p3,p4));

            				gmds::Face t1 = getGMDSMesh().newTriangle(p0,p1,p4);
            				gmds::Face t2 = getGMDSMesh().newTriangle(p1,p2,p4);
            				gmds::Face t3 = getGMDSMesh().newTriangle(p2,p3,p4);
            				gmds::Face t4 = getGMDSMesh().newTriangle(p3,p0,p4);
            				local_mesh_faces.push_back(t1);
            				local_mesh_faces.push_back(t2);
            				local_mesh_faces.push_back(t3);
            				local_mesh_faces.push_back(t4);

            				mesh_faces_in1Surf.push_back(t1);
            				mesh_faces_in1Surf.push_back(t2);
            				mesh_faces_in1Surf.push_back(t3);
            				mesh_faces_in1Surf.push_back(t4);
            				mesh_nodes.push_back(p4);
            				getGMDSMesh().mark(p4,node_mark);
            			}
            		} // end if (q.getType() == gmds::GMDS_QUAD)
            		else {
            			local_mesh_faces.push_back(q);
            			mesh_faces_in1Surf.push_back(q);
            		}
                } // for j
            } // if ... transfini
            else{
                for(unsigned int j=0;j<face_elem.size();j++){
                    local_mesh_faces.push_back(getGMDSMesh().get<gmds::Face>(face_elem[j]));
                    mesh_faces_in1Surf.push_back(getGMDSMesh().get<gmds::Face>(face_elem[j]));
                }
            }
#ifdef _DEBUG_MESH
            std::cout<<"NB LOC FACES -> "<<local_mesh_faces.size()<<std::endl;
#endif

            mesh_faces.push_back(local_mesh_faces);
            // on recupere toutes les faces
            // on recupere les noeuds qui n'ont pas deja ete recuperes
            for(unsigned int j=0;j<local_mesh_faces.size();j++){
                std::vector<gmds::Node> local_nodes =local_mesh_faces[j].getAll<gmds::Node>();
                for(unsigned int k=0;k<local_nodes.size();k++){
                    gmds::Node n =local_nodes[k];
                    if(getGMDSMesh().isMarked(n,node_mark))
                        continue;

                    mesh_nodes.push_back(n);
                    getGMDSMesh().mark(n,node_mark);
                }
            }
        } // end for k<cofaces.size()
    } // end for i<faces.size()

    //============================================================================
    //  REORIENTATION DU MAILLAGE SURFACIQUE
    //============================================================================
//    //marque pour les face orientees
//    int orientMark = getGMDSMesh().getNewMark();
//
//    //liste des faces composant le bord
//    std::list<Face*> allFaces;
//    // pour les noeuds de ces faces, on construit une connectivité indirecte
//    std::map<Node*,std::vector<Face*> > N2Faces;
//    //============================================================================
//    //recuperation des faces sur lesquelles on travaillera
//    for(unsigned int i=0;i<mesh_faces.size();i++){
//        std::vector<Face*> mfaces = mesh_faces[i];
//        allFaces.insert(allFaces.end(),mfaces.begin(),mfaces.end());
//    }
//    //marquage des faces sur lesquelles on travaill
//    std::list<Face*>::iterator itOF =  allFaces.begin();
//    Face *currentF;
//    for(;itOF!=allFaces.end();itOF++)
//    {
//        currentF = *itOF;
//        //creation de N2F
//        std::vector<Node*> currentNodes = currentF->getNodes();
//        for(unsigned int i =0;i<currentNodes.size();i++){
//            Node *ni=currentNodes[i];
//            N2Faces[ni].push_back(currentF);
//        }
//    }
//    //============================================================================
//    //on prend comme orientation le premier triangle
//    //============================================================================
//    currentF = *(allFaces.begin());
//
//    std::vector<Node*> currentNodes = currentF->getNodes();
//    //pour chaque arete, on recupere le triangle adjacent
//    Node *n0=currentNodes[0];
//    Node *n1=currentNodes[1];
//    Node *n2=currentNodes[2];
//
//    std::vector<Node*> nnn;
//    nnn.push_back(n0);
//    nnn.push_back(n2);
//    nnn.push_back(n1);
//    currentF->setNodes(nnn);
//
//    getGMDSMesh().mark(currentF,orientMark);
//
//    bool finishOrient=false;
//    int nbPermutations = 0;
//    allFaces.size();
//    do{
//        int nbMarkedFaces=0;
//        std::cout<<"================================"<<std::endl;
//        std::list<Face*>::iterator itOF =  allFaces.begin();
//        for(;itOF!=allFaces.end();itOF++)
//        {
//            currentF = *itOF;
//            bool reoriented = false;
//            if(!getGMDSMesh().isMarked(currentF,orientMark)){
//                std::cout<<"FACE A REORIENTER"<<std::endl;
//                // On a une face que l'on a pas encore orientée.
//                // On recupere ces noeuds
//                std::vector<Node*> currentNodes = currentF->getNodes();
//                //pour chaque arete, on recupere le triangle adjacent
//                Node *n0=currentNodes[0];
//                Node *n1=currentNodes[1];
//                Node *n2=currentNodes[2];
//
//                std::vector<Face*> n0Faces = N2Faces[n0];
//                std::vector<Face*> n1Faces = N2Faces[n1];
//                std::vector<Face*> n2Faces = N2Faces[n2];
//                //ARETE 01
//                Face* f01=0;
//                for(unsigned int i0=0;i0<n0Faces.size();i0++){
//                    Face *f0 = n0Faces[i0];
//                    for(unsigned int i1=0;i1<n1Faces.size();i1++){
//                        Face *f1 = n1Faces[i1];
//                        if(f0==f1 && f0!=currentF)
//                            f01 = f0;
//                    }
//                }
//                //ARETE 02
//                Face* f02=0;
//                for(unsigned int i0=0;i0<n0Faces.size();i0++){
//                    Face *f0 = n0Faces[i0];
//                    for(unsigned int i2=0;i2<n2Faces.size();i2++){
//                        Face *f2 = n2Faces[i2];
//                        if(f0==f2 && f0!=currentF)
//                            f02 = f0;
//                    }
//                }
//                //ARETE 12
//                Face* f12=0;
//                for(unsigned int i2=0;i2<n2Faces.size();i2++){
//                    Face *f2 = n2Faces[i2];
//                    for(unsigned int i1=0;i1<n1Faces.size();i1++){
//                        Face *f1 = n1Faces[i1];
//                        if(f2==f1 && f2!=currentF)
//                            f12 = f1;
//                    }
//                }
//
//                // On a nos trois triangles adjacents
//                if(f01==0)
//                    std::cout<<"PROBLEME F01"<<std::endl;
//                if(f02==0)
//                    std::cout<<"PROBLEME F02"<<std::endl;
//                if(f12==0)
//                    std::cout<<"PROBLEME F12"<<std::endl;
//                else{
//                    std::cout<<"VOISINAGE "<<f01->getID()<<" "<<f02->getID()<<" "<<f12->getID()<<std::endl;
//                    std::cout<<"ORIENTATION "<<getGMDSMesh().isMarked(f01,orientMark)
//                             <<" "<<getGMDSMesh().isMarked(f02,orientMark)
//                             <<" "<<getGMDSMesh().isMarked(f12,orientMark)<<std::endl;
//                }
//
//                bool toReverse = false;
//                int f01_to[3];
//
//                if(getGMDSMesh().isMarked(f01,orientMark)){
//                    std::vector<Node*> f01Nodes = f01->getNodes();
//                    for(unsigned int i=0;i<f01Nodes.size();i++){
//                        if(f01Nodes[i]==n0)
//                            f01_to[i]=0;
//                        else if(f01Nodes[i]==n1)
//                            f01_to[i]=1;
//                        else if(f01Nodes[i]==n2)
//                            f01_to[i]=2;
//                        else
//                            f01_to[i]=-1;
//                    }
//                    reoriented=true;
//                }
//                else if(getGMDSMesh().isMarked(f02,orientMark)){
//                    std::vector<Node*> f02Nodes = f02->getNodes();
//
//                    for(unsigned int i=0;i<f02Nodes.size();i++){
//                        if(f02Nodes[i]==n0)
//                            f01_to[i]=0;
//                        else if(f02Nodes[i]==n1)
//                            f01_to[i]=1;
//                        else if(f02Nodes[i]==n2)
//                            f01_to[i]=2;
//                        else
//                            f01_to[i]=-1;
//                    }
//                    reoriented=true;
//                }
//                else if(getGMDSMesh().isMarked(f12,orientMark)){
//                    std::vector<Node*> f12Nodes = f12->getNodes();
//
//                    for(unsigned int i=0;i<f12Nodes.size();i++){
//                        if(f12Nodes[i]==n0)
//                            f01_to[i]=0;
//                        else if(f12Nodes[i]==n1)
//                            f01_to[i]=1;
//                        else if(f12Nodes[i]==n2)
//                            f01_to[i]=2;
//                        else
//                            f01_to[i]=-1;
//                    }
//                    reoriented=true;
//                }
//                if(reoriented){
//
//                    //l'arete 01 vu de la face adjacent est l'arete commune
//                    if(f01_to[0]!=-1 && f01_to[1]!=-1){
//                        if(f01_to[0]<f01_to[1])
//                            toReverse=true;
//                    }
//                    //l'arete 02 vu de la face adjacent est l'arete commune
//                    else if(f01_to[0]!=-1 && f01_to[2]!=-1){
//                        if(f01_to[2]<f01_to[0])
//                            toReverse=true;
//                    }
//                    //l'arete 12 vu de la face adjacent est l'arete commune
//                    else if(f01_to[1]!=-1 && f01_to[2]!=-1){
//                        if(f01_to[1]<f01_to[2])
//                            toReverse=true;
//                    }
//
//                    if(toReverse){
//                        std::vector<Node*> newNodes;
//                        newNodes.push_back(n0);
//                        newNodes.push_back(n2);
//                        newNodes.push_back(n1);
//                        currentF->setNodes(newNodes);
//                        nbPermutations++;
//                    }
//
//                    getGMDSMesh().mark(currentF,orientMark);
//                    nbMarkedFaces++;
//                }
//
//            }
//        }
//
//        if(nbMarkedFaces==0)
//            finishOrient=true;
//    }
//    while(finishOrient==false);
//
//    std::cout<<"REORIENTATION EFFECTUEE, nb permutations = "<<nbPermutations<<std::endl;
//
//
//    //============================================================================
//    for(unsigned int i=0;i<mesh_faces.size();i++){
//        std::vector<Face*> mfaces = mesh_faces[i];
//        for(unsigned int j=0;j<mfaces.size();j++) {
//            getGMDSMesh().unmark(mfaces[j],orientMark);
//        }
//    }
//    getGMDSMesh().freeMark(orientMark);

    //============================================================================
    // LE MAILLAGE DES FACES AU BORD A ETE RECUPERE, ON MAILLE MAINTENANT LE
    // VOLUME
    //============================================================================
    for(unsigned int i=0;i<mesh_nodes.size();i++){
        getGMDSMesh().unmark(mesh_nodes[i],node_mark);
    }
    getGMDSMesh().freeMark<gmds::Node>(node_mark);


#ifdef _DEBUG_MESH
    std::cout<<"GMDS mesh stats"<<getGMDSMesh().getMaxLocalID(0)<<" "<<getGMDSMesh().getMaxLocalID(2)<<std::endl;
#endif
    /////////////////////

    std::vector<gmds::Region> newRegions;
    std::vector<gmds::Node> newNodes;

#ifdef _DEBUG_MESH
    std::cout<<"BEFORE MESHGEMS- Nb Nodes, nb faces (outer), nb regions :"<<getGMDSMesh().getNbNodes()
            <<" "<<mesh_faces_in1Surf.size()
            <<" "<<getGMDSMesh().getNbRegions()<<std::endl;
#endif




    _bnd_mesh_data_t bnd_mesh;
    bnd_mesh.nbv = mesh_nodes.size();
    bnd_mesh.nbe = 0;
    bnd_mesh.nbt = mesh_faces_in1Surf.size();

    bnd_mesh.xyz.resize(bnd_mesh.nbv + 1); // + 1 because numbering starts at 1 in MeshGems (Fortran)
    bnd_mesh.f2n_id.resize(bnd_mesh.nbt + 1); // + 1 because numbering starts at 1 in MeshGems (Fortran)

    std::map<int, int> gmds2meshgems_nids;
    std::map<int, int> meshgems2gmds_nids;

    std::map<gmds::TCellID, int> gmds2meshgems_fids;

    for(unsigned int i=0; i<mesh_nodes.size(); i++){
    	gmds2meshgems_nids[mesh_nodes[i].id()] = i+1;
    	meshgems2gmds_nids[i+1] = mesh_nodes[i].id();
    	bnd_mesh.xyz[i+1][0] = mesh_nodes[i].X();
    	bnd_mesh.xyz[i+1][1] = mesh_nodes[i].Y();
    	bnd_mesh.xyz[i+1][2] = mesh_nodes[i].Z();
    }

    for(unsigned int i=0; i<mesh_faces_in1Surf.size(); i++){
    	std::vector<gmds::TCellID> nids = mesh_faces_in1Surf[i].getIDs<gmds::Node> ();
    	bnd_mesh.f2n_id[i+1][0] = gmds2meshgems_nids[nids[0]];
    	bnd_mesh.f2n_id[i+1][1] = gmds2meshgems_nids[nids[1]];
    	bnd_mesh.f2n_id[i+1][2] = gmds2meshgems_nids[nids[2]];
    }

#ifdef _DEBUG_MESH
    std::cout<<"input stats "<<bnd_mesh.nbv<<" "<<bnd_mesh.nbt<<std::endl;

#endif

    // MESHGEMS
    meshgems_context_t * mg_ctx = meshgems_context_new();
    if(NULL == mg_ctx) {
    	std::cerr<<"MeshGemsVol : pb create context"<<std::endl;
    }

    meshgems_tetra_session_t * mg_tms = meshgems_tetra_session_new(mg_ctx);
    if(NULL == mg_tms) {
    	std::cerr<<"MeshGemsVol : pb create tetra session"<<std::endl;
    }

    meshgems_mesh_t* mg_m_in = meshgems_mesh_new(mg_ctx);
    if(NULL == mg_m_in) {
        	std::cerr<<"MeshGemsVol : pb create input mesh"<<std::endl;
        }

    meshgems_status_t ret ;
    ret = meshgems_mesh_set_get_vertex_count ( mg_m_in , mgtm_get_vertex_count , &bnd_mesh );
    if(STATUS_OK != ret) {
    	std::cerr<<"MeshGemsVol : pb set callback"<<std::endl;
    }
    ret = meshgems_mesh_set_get_vertex_coordinates ( mg_m_in , mgtm_get_vertex_coordinates , &bnd_mesh );
    if(STATUS_OK != ret) {
    	std::cerr<<"MeshGemsVol : pb set callback"<<std::endl;
    }
    ret = meshgems_mesh_set_get_edge_count ( mg_m_in , mgtm_get_edge_count , &bnd_mesh ) ;
    if(STATUS_OK != ret) {
    	std::cerr<<"MeshGemsVol : pb set callback"<<std::endl;
    }
    ret = meshgems_mesh_set_get_edge_vertices ( mg_m_in , mgtm_get_edge_vertices , &bnd_mesh );
    if(STATUS_OK != ret) {
    	std::cerr<<"MeshGemsVol : pb set callback"<<std::endl;
    }
    ret = meshgems_mesh_set_get_triangle_count ( mg_m_in , mgtm_get_triangle_count , &bnd_mesh );
    if(STATUS_OK != ret) {
    	std::cerr<<"MeshGemsVol : pb set callback"<<std::endl;
    }
    ret = meshgems_mesh_set_get_triangle_vertices ( mg_m_in , mgtm_get_triangle_vertices , &bnd_mesh );
    if(STATUS_OK != ret) {
    	std::cerr<<"MeshGemsVol : pb set callback"<<std::endl;
    }

    meshgems_sign_mesh(mg_m_in);
    meshgems_status_t st_a = meshgems_tetra_set_surface_mesh(mg_tms, mg_m_in);
    if(STATUS_OK != st_a) {
    	std::cerr<<"MeshGemsVol : pb set bnd mesh"<<std::endl;
    }

    meshgems_mesh_compute_statistics ( mg_m_in ) ;


    ret = meshgems_tetra_set_param ( mg_tms , "verbose" , std::to_string(meshingProp->getVerbosity()).c_str()) ;
    if ( ret != STATUS_OK ) {
    	std::cerr<<"MeshGemsVol : pb set param verbose"<<std::endl;
    }
    ret = meshgems_tetra_set_param ( mg_tms , "gradation" , std::to_string(meshingProp->getGradation()).c_str()) ;
    if ( ret != STATUS_OK ) {
    	std::cerr<<"MeshGemsVol : pb set param gradation"<<std::endl;
    }
    ret = meshgems_tetra_set_param ( mg_tms , "min_size" , std::to_string(meshingProp->getMinSize()).c_str()) ;
    if ( ret != STATUS_OK ) {
    	std::cerr<<"MeshGemsVol : pb set param min_size"<<std::endl;
    }
    ret = meshgems_tetra_set_param ( mg_tms , "max_size" , std::to_string(meshingProp->getMaxSize()).c_str()) ;
    if ( ret != STATUS_OK ) {
    	std::cerr<<"MeshGemsVol : pb set param verbose"<<std::endl;
    }
    if(meshingProp->getOptimWorstElem()) {
    	ret = meshgems_tetra_set_param ( mg_tms , "optimise_worst_elements" , "yes");
    } else {
    	ret = meshgems_tetra_set_param ( mg_tms , "optimise_worst_elements" , "no");
    }
    if ( ret != STATUS_OK ) {
    	std::cerr<<"MeshGemsVol : pb set param verbose"<<std::endl;
    }
    if(meshingProp->getOptimLvl() == Topo::BlockMeshingPropertyDelaunayMeshGems::MeshGemsOptimizationLevel::vertices) {
    	ret = meshgems_tetra_set_param ( mg_tms , "optimisation_level" , "vertices");
    } else if(meshingProp->getOptimLvl() == Topo::BlockMeshingPropertyDelaunayMeshGems::MeshGemsOptimizationLevel::light) {
    	ret = meshgems_tetra_set_param ( mg_tms , "optimisation_level" , "light");
    } else if(meshingProp->getOptimLvl() == Topo::BlockMeshingPropertyDelaunayMeshGems::MeshGemsOptimizationLevel::standard) {
    	ret = meshgems_tetra_set_param ( mg_tms , "optimisation_level" , "standard");
    } else if(meshingProp->getOptimLvl() == Topo::BlockMeshingPropertyDelaunayMeshGems::MeshGemsOptimizationLevel::strong) {
    	ret = meshgems_tetra_set_param ( mg_tms , "optimisation_level" , "strong");
    }
    if ( ret != STATUS_OK ) {
    	std::cerr<<"MeshGemsVol : pb set param optim lvl"<<std::endl;
    }



//    status_t st_b = meshgems_tetra_mesh_boundary(mg_tms);
    meshgems_status_t st_b = meshgems_tetra_compute_mesh(mg_tms);

    if(STATUS_OK != st_b) {
    	std::cerr<<"MeshGemsVol : pb generating tet mesh"<<std::endl;
    }

    meshgems_mesh_t* mg_m_out = meshgems_mesh_new(mg_ctx);
    if(NULL == mg_m_out) {
    	std::cerr<<"MeshGemsVol : pb create output mesh"<<std::endl;
    }

    meshgems_status_t st_c = meshgems_tetra_get_mesh(mg_tms, &mg_m_out);
    if(STATUS_OK != st_c) {
    	std::cerr<<"MeshGemsVol : pb get output mesh"<<std::endl;
    }

    int nvm;
    int ntm;


    /* First get all the vertices */
    ret = meshgems_mesh_get_vertex_count ( mg_m_out , & nvm ) ;
//    if ( ret != STATUS_OK )
//    RETURN _ W I T H _ M E S S A G E ( ret , " unable to get resulting vertex count ") ;
    ret = meshgems_mesh_get_tetrahedron_count ( mg_m_out , & ntm ) ;
//    if ( ret != STATUS_OK )
//    RETURN _ W I T H _ M E S S A G E ( ret , " unable to get resulting tetrahedron count ") ;
#ifdef _DEBUG_MESH
    std::cout<<"nvm "<<nvm<<" ntm "<<ntm<<std::endl;
#endif
    std::vector<gmds::TCellID>& nodes = bl->nodes();

    for ( int i = bnd_mesh.nbv; i <= nvm ; i ++) {
    	double coo[3];
    	ret = meshgems_mesh_get_vertex_coordinates ( mg_m_out , i , coo ) ;
//    	if ( ret != STATUS_OK )
//    		RE T U R N _ W I T H _ M E S S A G E ( ret , " unable to get resulting vertices ") ;
//    	if (0) {
//    		fprintf ( stdout , " vertex % d coordinates are : % e % e % e \n" , i ,
//    				coo [0] , coo [1] , coo [2]) ;
//    	}
    	gmds::Node n = getGMDSMesh().newNode(coo[0], coo[1], coo[2]);
    	meshgems2gmds_nids[i] = n.id();
    	nodes.push_back(n.id());
    	command->addCreatedNode(n.id());
    }

    std::vector<gmds::TCellID>& elem = bl->regions();

    for ( int i = 1; i <= ntm ; i ++) {
    	integer vtx [4] , tag ;
    	ret = meshgems_mesh_get_tetrahedron_vertices ( mg_m_out , i , vtx ) ;
//    	if ( ret != STATUS_OK )
//    		RE T U R N _ W I T H _ M E S S A G E ( ret , " unable to get resulting tetrahedra ") ;
//    	if (0) {
//    		fprintf ( stdout , " tetra % d vertices are : %d % d % d % d \ n", i ,
//    				vtx [0] , vtx [1] , vtx [2] , vtx [3]) ;
//    	}

    	gmds::Region r = getGMDSMesh().newTet(meshgems2gmds_nids[vtx [0]] ,
    			meshgems2gmds_nids[vtx [1]] ,
				meshgems2gmds_nids[vtx [2]] ,
				meshgems2gmds_nids[vtx [3]]);
    	elem.push_back(r.id());
    	command->addCreatedRegion(r.id());
    }

    for(unsigned int i=0;i<pyramids.size();i++){
    	gmds::Region r = pyramids[i];
    	elem.push_back(r.id());
    	command->addCreatedRegion(r.id());
    }


    meshgems_tetra_regain_mesh ( mg_tms , mg_m_out ) ;

    meshgems_tetra_session_delete(mg_tms);

    meshgems_context_delete(mg_ctx);

    // MESHGEMS


//    triton::TetgenFacade tetgen_caller;
//    Topo::BlockMeshingPropertyDelaunayMeshGems* meshingProp =
//    		dynamic_cast<Topo::BlockMeshingPropertyDelaunayMeshGems*>(bl->getBlockMeshingProperty());
//    CHECK_NULL_PTR_ERROR(meshingProp);
//
//    std::string meshParams = meshingProp->serializeParameters().ascii();
//    try
//    {
//        tetgen_caller.generateTetMesh(mesh_nodes,mesh_faces,newNodes,newRegions,getGMDSMesh(),meshParams);
//    }
//    catch (int errorCode) // we should call terminatetetgen
//    {
//		TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
//        error << "Erreur interne, dans Tetgen: ";
//        if (errorCode == 1){
//            error << "mémoire insuffisante";
//        }
//        else if (errorCode == 2){
//            error << "bug connu de Tetgen";
//        }
//        else if (errorCode == 3){
//            error << "A self-intersection was detected. Program stopped. Input error";
//        }
//        else if (errorCode == 4){
//        	error << "A very small input feature size was detected. Program stopped. Try to set smaller tolerance (-T option). Possible input error";
//        }
//        else if (errorCode == 5){
//        	error << "Two very close input facets were detected. Program stopped.Try -Y option to avoid adding Steiner points in boundary. Possible input error";
//        }
//        else if (errorCode == 10){
//        	error << "An input error was detected Program stopped.";
//        }
//        else {
//            error << "non documenté";
//        }
//        error << ", pour le bloc topologique "<<bl->getName();
//
//        throw TkUtil::Exception (error);
//    }


//    triton::NetgenParams params;
//    params.fineness = 0.8;
//    params.grading = 0.1;
//    params.closeedgefact=1.0;
//
//    triton::NetgenFacade<TMask> tetgen_caller;
//    tetgen_caller.generateTetMesh(mesh_nodes, mesh_faces_in1Surf,
//            newNodes,newRegions,getGMDSMesh() ,params);

#ifdef _DEBUG_MESH
    std::cout<<"newNodes = "<<newNodes.size()<<", newRegions = "<<newRegions.size()<<std::endl;
    std::cout<<"AFTER MESHGEMS- Nb Nodes, nb elems :"<<getGMDSMesh().getNbNodes()<<" "<<getGMDSMesh().getNbRegions()<<std::endl;
#endif

//    std::vector<gmds::TCellID>& nodes = bl->nodes();
//    std::vector<gmds::TCellID>& elem = bl->regions();
//
//    for(unsigned int i=0;i<newNodes.size();i++){
//        gmds::Node  n = newNodes[i];
//        nodes.push_back(n.getID());
//        command->addCreatedNode(n.getID());
//    }
//    for(unsigned int i=0;i<newRegions.size();i++){
//        gmds::Region r = newRegions[i];
//        elem.push_back(r.getID());
//        command->addCreatedRegion(r.getID());
//    }
//
//    for(unsigned int i=0;i<pyramids.size();i++){
//        gmds::Region r = pyramids[i];
//        elem.push_back(r.getID());
//        command->addCreatedRegion(r.getID());
//    }


    // ajout des polyedres aux volumes

    std::vector<std::string> groupsName;
    bl->getGroupsName(groupsName);


#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les blocs en mode debug
    if (groupsName.empty()){
        groupsName.push_back(bl->getName());
		TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
        message1 <<"Le bloc "<<bl->getName()<<" n'a pas de groupe pour les mailles";
        getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::ERROR));
    }
#endif

    for (size_t i=0; i<groupsName.size(); i++){

        std::string& nom = groupsName[i];

        bool isNewVolume = false;
        try {
            getContext().getLocalMeshManager().getVolume(nom);
            command->addModifiedVolume(nom);
        } catch (...) {
            command->addNewVolume(nom);
            isNewVolume = true;
        }
        // le volume de maillage que l'on vient de créer/modifier
        Mesh::Volume* vol = getContext().getLocalMeshManager().getVolume(nom);
        vol->addBlock(bl);

        if (isNewVolume)
            getContext().newGraphicalRepresentation (*vol);
#ifdef _DEBUG_MESH
        std::cout<<"AJOUT DU BLOC"<<std::endl;
#endif
    } // end for i<groupsName.size()

#else
    throw TkUtil::Exception (TkUtil::UTF8String ("meshDelaunayMeshGemsVol non actif, define USE_MESHGEMS.", TkUtil::Charset::UTF_8));
#endif

} // end meshDelaunayMeshGemsVol(Block*)
/*----------------------------------------------------------------------------*/
//void MeshImplementation::_addFacesInSurfacesForDelaunay(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
//{
//    std::vector<std::string> groupsName;
//    fa->getGroupsName(groupsName);
//
//#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
//    // on ajoute un groupe pour distinguer les faces en mode debug
//    groupsName.push_back(fa->getName());
//#endif
//
//    // création des polygones si une surface les attend et si c'est autre chose qu'une arête
//    if (!groupsName.empty()){
//
//        // TODO [FL] implémenter une méthode non structurée
//        uint nbBrasI = 0;
//        uint nbBrasJ = 0;
//        bool isStructured = fa->isStructured(); // valeur initiale
//
//        // [EB] NB, cette façon de faire n'est plus adaptée depuis Octobre 2013
////        if (!isStructured)
////            fa->structure();
////        fa->getNbMeshingEdges(nbBrasI, nbBrasJ); // cette méthode n'est pas acceptée, elle est réservée au cas structuré
////        if (!isStructured)
////            fa->unstructure();
//
//        const uint nbNoeudsI = nbBrasI + 1;
//        const uint nbNoeudsJ = nbBrasJ + 1;
//
//        std::vector<gmds::Node*>& nodes = fa->nodes();
//        std::vector<gmds::Face*>& elem = fa->faces();
//
//#define nodeIJ(ii,jj) nodes[ii+(jj)*nbNoeudsI]
//
//        uint iBegin = 0, iEnd = nbBrasI;
//        uint jBegin = 0, jEnd = nbBrasJ;
//
//        // les triangles (juste à une extrémité)
//        if (fa->getNbVertices() == 3){
//            jEnd-=1;
//            for (uint i=iBegin; i<iEnd; i++)
//                for (uint j=jEnd; j<jEnd+1; j++){
//                    gmds::Node* nd1 = nodeIJ(i,j);
//                    gmds::Node* nd2 = nodeIJ(i+1,j);
//                    gmds::Node* nd3 = nodeIJ(i+1,j+1);
//
//                    gmds::Face* f = getGMDSMesh().newTriangle(nd1,nd2,nd3);
//                    elem.push_back(f);
//                    command->addCreatedFace(f);
//                }
//        }
//
//        // les quadrangles (reste de la surface)
//        for (uint i=iBegin; i<iEnd; i++)
//            for (uint j=jBegin; j<jEnd; j++){
//                gmds::Node* nd1 = nodeIJ(i,j);
//                gmds::Node* nd2 = nodeIJ(i+1,j);
//                gmds::Node* nd3 = nodeIJ(i+1,j+1);
//                gmds::Node* nd4 = nodeIJ(i,j+1);
//
//
//                gmds::Face* f = getGMDSMesh().newTriangle(nd1,nd2,nd3);
//                //std::cout<<"Création du quad d'id "<<f->getID()<< " ("<<nd1<<", "<<nd2<<", "<<nd3<<", "<<nd4<<")\n";
//                elem.push_back(f);
//                command->addCreatedFace(f);
//                f = getGMDSMesh().newTriangle(nd1,nd3,nd4);
//                //std::cout<<"Création du quad d'id "<<f->getID()<< " ("<<nd1<<", "<<nd2<<", "<<nd3<<", "<<nd4<<")\n";
//                elem.push_back(f);
//                command->addCreatedFace(f);
//            }
//#undef nodeIJ
//    } // end if (!groupsName.empty() && !fa->isDegenerated())
//
//    // ajout des polygones aux surfaces
//    for (size_t i=0; i<groupsName.size(); i++){
//        std::string& nom = groupsName[i];
//
//        try {
//            gmds::Mesh<TMask>::surface& sf = getGMDSMesh().getSurface(nom);
//            _addFacesInSurface(fa->faces(), sf);
//            command->addModifiedSurface(nom);
//        } catch (...) {
//            gmds::Mesh<TMask>::surface& sf = getGMDSMesh().newSurface(nom);
//            _addFacesInSurface(fa->faces(), sf);
//            command->addNewSurface(nom);
//        }
//        // la surface de maillage que l'on vient de créer/modifier
//        Mesh::Surface* sf = getContext().getLocalMeshManager().getSurface(nom);
//        sf->addCoFace(fa);
//    } // end for i<groupsName.size()
//
//} // end _addFacesInSurfacesForDelaunay
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif	// USE_MESHGEMS
