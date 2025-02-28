/*----------------------------------------------------------------------------*/
/** \file    TetgenFacade.t.h
 *  \author  F. LEDOUX
 *  \date    16/05/2011
 */
/*----------------------------------------------------------------------------*/
#include "Triton2/TetgenInterface/TetgenFacade.h"
/*----------------------------------------------------------------------------*/
using namespace triton;
using namespace gmds;
/*----------------------------------------------------------------------------*/
TetgenFacade::TetgenFacade(){

}
/*----------------------------------------------------------------------------*/
void TetgenFacade::generateCDTMesh(gmds::Mesh& ABoundaryMesh,
									 gmds::Mesh& AVolMesh,
									 const double AQuality){
	if(AQuality<1.0 ||AQuality>2.0)
		throw gmds::GMDSException("The quality parameter must be included in [1.0,2.0]");
//	if (ABoundaryMesh.getNbQuadrilaterals()!=0 ||
//		ABoundaryMesh.getNbPolygons()!=0 )
//		throw gmds::GMDSException("The boundary mesh cannot contain quads or polygons");

	buildTetgenInput(ABoundaryMesh);

    char * param = const_cast<char*>("pq1.2a0.1");
    tetgenbehavior behav;
    behav.parse_commandline(param);
    tetrahedralize(&behav, &tetgenInput_, &tetgenOutput_);
	buildGMDSOutput(AVolMesh);
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::generateTetMesh(gmds::Mesh& ABoundaryMesh,
									 gmds::Mesh& AVolMesh,
									 const double AQuality){
	if(AQuality<1.0 ||AQuality>2.0)
		throw gmds::GMDSException("The quality parameter must be included in [1.0,2.0]");
//	if (ABoundaryMesh.getNbQuadrilaterals()!=0 ||
//		ABoundaryMesh.getNbPolygons()!=0 )
//		throw gmds::GMDSException("The boundary mesh cannot contain quads or polygons");

	buildTetgenInput(ABoundaryMesh);

    char * param = const_cast<char*>("pq1.414a0.1");
    tetgenbehavior behav;
    behav.parse_commandline(param);
    tetrahedralize(&behav, &tetgenInput_, &tetgenOutput_);
	buildGMDSOutput(AVolMesh);
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::generateTetMeshDistTet(gmds::Mesh& ABoundaryMesh,
									 gmds::Mesh& AVolMesh,
									 const double AQuality){
	if(AQuality<1.0 ||AQuality>2.0)
		throw gmds::GMDSException("The quality parameter must be included in [1.0,2.0]");
//	if (ABoundaryMesh.getNbQuadrilaterals()!=0 ||
//		ABoundaryMesh.getNbPolygons()!=0 )
//		throw gmds::GMDSException("The boundary mesh cannot contain quads or polygons");

	buildTetgenInput(ABoundaryMesh);

    char * param = const_cast<char*>("pq1.414a0.01");
    tetgenbehavior behav;
    behav.parse_commandline(param);
    tetrahedralize(&behav, &tetgenInput_, &tetgenOutput_);

	buildGMDSOutput(AVolMesh);
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::generateTetMesh(std::vector<gmds::Node>& ANodes,
		std::vector< std::vector<gmds::Face> >& AFacets,
		gmds::Mesh& AMesh,
		 const std::string&  AParam )
{

	buildTetgenInput(ANodes,AFacets);

//	char * param = const_cast<char*>(AParam.c_str());
    char * param = const_cast<char*>("Yq1.2pa0.1");
    tetgenbehavior behav;
    behav.parse_commandline(param);
    tetrahedralize(&behav, &tetgenInput_, &tetgenOutput_);

	fillGMDSOutput(AMesh, ANodes);
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::generateTetMesh(
		std::vector<gmds::Node>& ANodes,
		std::vector< std::vector<gmds::Face> >& AFacets,
		std::vector<Node>& ACreatedNodes,
		std::vector<Region>& ACreatedRegions,
		gmds::Mesh& AMesh,
		const std::string&  AParam )
{
	buildTetgenInput(ANodes,AFacets);
    
	char * param = const_cast<char*>(AParam.c_str());
    tetgenbehavior behav;
    behav.parse_commandline(param);
    tetrahedralize(&behav, &tetgenInput_, &tetgenOutput_);

	ACreatedNodes.clear();
	ACreatedRegions.clear();
	fillGMDSOutput(AMesh, ANodes, ACreatedNodes, ACreatedRegions);
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::buildTetgenInput(
		std::vector<gmds::Node>& ANodes,
		std::vector< std::vector<gmds::Face> >& AFacets)
{
	tetgenInput_.mesh_dim = 3;
	//all the indices start from 1
	tetgenInput_.firstnumber = 1;
	// we specify the number of nodes ...
	tetgenInput_.numberofpoints = ANodes.size();
	// ... and we allocate the xyz tabular for tetgen
	tetgenInput_.pointlist = new REAL[tetgenInput_.numberofpoints * 3];
	// no marker list associated to the points
	tetgenInput_.pointmarkerlist = NULL;
	/* now we fill in the XYZ tetgen tabular with the node coordinates coming from
	 * the GMDS structure */
	int index = 1;
	std::map<gmds::TCellID,int> gmds2tetgen_nodes;
	for(int i=0;i<ANodes.size();i++){
		Node n = ANodes[i];
		tetgenInput_.pointlist[(index - 1) * 3 + 0] = n.X();
		tetgenInput_.pointlist[(index - 1) * 3 + 1] = n.Y();
		tetgenInput_.pointlist[(index - 1) * 3 + 2] = n.Z();
		/* we keep in mind the association from the GMDS node to the corresponding node
		 * index in tetgen. */
		gmds2tetgen_nodes.insert(std::pair<TCellID,int>(n.id(),index));
		index++;
	}
	//==============================================================================
	std::vector<gmds::Face> allFaces;
	for(unsigned int surface_index=0;surface_index<AFacets.size(); surface_index++){
		std::vector<gmds::Face> local_faces = AFacets[surface_index];
		allFaces.insert(allFaces.end(),local_faces.begin(),local_faces.end());


	}

	tetgenInput_.numberoffacets = allFaces.size();
	tetgenInput_.facetlist = new tetgenio::facet[tetgenInput_.numberoffacets];
	// the marker list will be fill with the number index of the used surface.
	tetgenInput_.facetmarkerlist = new int[tetgenInput_.numberoffacets];
	/* now we traverse each single surface and not the global mesh to get the faces
	 * composing it	 */
	for(unsigned int index=0;index<allFaces.size(); index++){
		// we are going to fill the tetgen facet corresponding to the GMDS surface
		tetgenio::facet *f = &tetgenInput_.facetlist[index];
		tetgenio::init(f);
		f->numberofholes = 0;
		f->numberofpolygons = 1;
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];

		tetgenInput_.facetmarkerlist[index] = index;


		Face f_GMDS = allFaces[index];
		std::vector<TCellID> node_ids;
		f_GMDS.getIDs<Node>(node_ids);

		tetgenio::polygon *p = &f->polygonlist[0];
		tetgenio::init(p);
		p->numberofvertices = 3;
		p->vertexlist = new int[p->numberofvertices];
		for(int node_i =0; node_i<node_ids.size();node_i++)
			p->vertexlist[node_i] = gmds2tetgen_nodes[node_ids[node_i] ];

	}

	// ANCIENNE VERSION
//	/* we get the surface groups with the IMPORTANT hypothesis that each
//	 * surface corresponds to a different geometric surFace/
//	tetgenInput_.numberoffacets = AFacets.size();
//	tetgenInput_.facetlist = new tetgenio::facet[tetgenInput_.numberoffacets];
//	// the marker list will be fill with the number index of the used surface.
//	tetgenInput_.facetmarkerlist = new int[tetgenInput_.numberoffacets];
//	/* now we traverse each single surface and not the global mesh to get the faces
//	 * composing it	 */
//	for(unsigned int surface_index=0;surface_index<AFacets.size(); surface_index++){
//		// we are going to fill the tetgen facet corresponding to the GMDS surface
//		tetgenio::facet *f = &tetgenInput_.facetlist[surface_index];
//		tetgenio::init(f);
//		// the number of polygons is equal to the number of faces in the GMDS surface
//		std::vector<Face>& faces = AFacets[surface_index];
//		f->numberofpolygons = faces.size();
//		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
//		tetgenInput_.facetmarkerlist[surface_index] = surface_index;
//		std::vector<Face>::iterator it=faces.begin(), ite=faces.end();
//		int face_index = 0;
//		// for each GMDS face, we create the corresponding tetgen face in the facet
//		while(it!=ite){
//			Face f_GMDS = *it;
//			std::vector<id> node_ids;
//			f_GMDS->getNodeIDs(node_ids);
//			tetgenio::polygon *p = &f->polygonlist[face_index];
//			tetgenio::init(p);
//			p->numberofvertices = node_ids.size();
//			p->vertexlist = new int[p->numberofvertices];
//			for(int node_i =0; node_i<node_ids.size();node_i++)
//				p->vertexlist[node_i] = gmds2tetgen_nodes[node_ids[node_i] ];
//			face_index++;
//			it++;
//		}
//	}
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::buildTetgenInput(gmds::Mesh& AMesh)
{
	tetgenInput_.mesh_dim = 3;
	//all the indices start from 1
	tetgenInput_.firstnumber = 1;
	// we specify the number of nodes ...
	tetgenInput_.numberofpoints = AMesh.getNbNodes();
	// ... and we allocate the xyz tabular for tetgen
	tetgenInput_.pointlist = new REAL[tetgenInput_.numberofpoints * 3];
	// no marker list associated to the points
	tetgenInput_.pointmarkerlist = NULL;

	/* now we fill in the XYZ tetgen tabular with the node coordiantes coming from
	 * the GMDS structure */
	gmds::Variable<int>* node_index = AMesh.newVariable<int, GMDS_NODE>("index");
	int index = 1;
	for (auto i : AMesh.nodes()){
		auto n = AMesh.get<gmds::Node>(i);
		tetgenInput_.pointlist[(index - 1) * 3 + 0] = n.X();
		tetgenInput_.pointlist[(index - 1) * 3 + 1] = n.Y();
		tetgenInput_.pointlist[(index - 1) * 3 + 2] = n.Z();

		/* we keep in mind the association from the GMDS node to the corresponding node
		 * index in tetgen.
		 */
		(*node_index)[n.id()]=index++;
	}

	/* we get the surface groups with the IMPORTANT hypothesis that each
	 * surface corresponds to a different geometric surface
	 */
	tetgenInput_.numberoffacets = AMesh.getNbGroups<gmds::Face>();
	tetgenInput_.facetlist = new tetgenio::facet[tetgenInput_.numberoffacets];
	// the marker list will be fill with the number index of the used surface.
	tetgenInput_.facetmarkerlist = new int[tetgenInput_.numberoffacets];

	/* now we traverse each single surface and not the global mesh to get the faces
	 * composing it	 */

	int surface_index = 0;
    for (auto its = AMesh.groups_begin<gmds::Face>(); its != AMesh.groups_end<gmds::Face>(); ++its){
		auto surf = *its;
		// we are going to fill the tetgen facet corresponding to the GMDS surface
		tetgenio::facet *f = &tetgenInput_.facetlist[surface_index];
		tetgenio::init(f);
		// the facet has no holes (WARNING we have no guarantee)
		f->numberofholes = 0;
		// the number of polygons is equal to the number of faces in the GMDS surface
		f->numberofpolygons = surf->size();
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];

		tetgenInput_.facetmarkerlist[surface_index] = surface_index;

		int face_index = 0;
		// for each GMDS face, we create the corresponding tetgen face in the facet
		for (auto it : surf->cells()){
			gmds::Face f_GMDS = AMesh.get<gmds::Face>(it);
			std::vector<TCellID> node_ids;
			f_GMDS.getIDs<gmds::Node>(node_ids);

			tetgenio::polygon *p = &f->polygonlist[face_index];
			tetgenio::init(p);
			p->numberofvertices = node_ids.size();
			p->vertexlist = new int[p->numberofvertices];
			for(int node_i =0; node_i<node_ids.size();node_i++)
				p->vertexlist[node_i] = (*node_index)[node_ids[node_i] ];

			face_index++;
		}
		surface_index++;
	}

//	AMesh.deleteVariable(GMDS_NODE,node_index);

}
/*----------------------------------------------------------------------------*/
void TetgenFacade::buildGMDSOutput(gmds::Mesh& AMesh)
{
	std::vector<Node> tetgen2GMDSNode;
	tetgen2GMDSNode.reserve(tetgenOutput_.numberofpoints);
	Node n;
	for(int i = 0; i < tetgenOutput_.numberofpoints; i++){
		n= AMesh.newNode(	tetgenOutput_.pointlist[i * 3 + 0],
				tetgenOutput_.pointlist[i * 3 + 1],
				tetgenOutput_.pointlist[i * 3 + 2]);
		tetgen2GMDSNode.push_back(n);
	}
	std::cout<<"Result"<<"\n";
	std::cout<<"\t number of points: "<<tetgenOutput_.numberofpoints<<"\n";
	std::cout<<"\t number of edges : "<<tetgenOutput_.numberofedges<<"\n";
	std::cout<<"\t number of faces : "<<tetgenOutput_.numberoftrifaces<<"\n";
	std::cout<<"\t number of tet   : "<<tetgenOutput_.numberoftetrahedra<<"\n";

	for(int i = 0; i < tetgenOutput_.numberoftetrahedra; i++){
		AMesh.newTet(tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 0] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 1] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 2] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 3] -1]);
	}

//	std::set<std::string> used_tag;

//	if(MeshDescriptor<M>::hasFaces){
//		for(int i = 0; i < tetgenOutput_.numberoftrifaces; i++){
//			Face f = AMesh.newTriangle(
//						 tetgen2GMDSNode[tetgenOutput_.trifacelist[i * 3 + 0] -1],
//						 tetgen2GMDSNode[tetgenOutput_.trifacelist[i * 3 + 1] -1],
//						 tetgen2GMDSNode[tetgenOutput_.trifacelist[i * 3 + 2] -1]);
//			std::stringstream ost;
//			ost<<tetgenOutput_.trifacemarkerlist[i];
//
//			if(used_tag.find(ost.str())==used_tag.end()){
//				used_tag.insert(ost.str());
//				AMesh.newSurface(ost.str());
//			}
//
//			AMesh.getSurface(ost.str()).add(f);
//		}
//	}
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::fillGMDSOutput(gmds::Mesh& AMesh,
		 std::vector<gmds::Node>& ANodes)
{
	std::vector<Node> tetgen2GMDSNode;
	tetgen2GMDSNode.reserve(tetgenOutput_.numberofpoints);
	Node n;
	for(int i = 0; i < tetgenOutput_.numberofpoints; i++){
		/* First, we try to find if a node is already located in this
		 * location. It is so a very unstable (unpredicatable) function
		 * depending on the mesh precision
		 */
		math::Point p_netgen(tetgenOutput_.pointlist[i * 3 + 0],
					tetgenOutput_.pointlist[i * 3 + 1],
					tetgenOutput_.pointlist[i * 3 + 2]);
		bool found=false;
		TCellID found_id;
		unsigned int k=0;
		for(;k<ANodes.size() && !found;k++){
			math::Point p_init = ANodes[k].point();
			if(p_init==p_netgen){
				found =true;
				found_id=ANodes[k].id();
			}
		}

		if(!found){
			//new node. we add it
			n= AMesh.newNode(tetgenOutput_.pointlist[i * 3 + 0],
					tetgenOutput_.pointlist[i * 3 + 1],
					tetgenOutput_.pointlist[i * 3 + 2]);
			tetgen2GMDSNode.push_back(n);
		}
		else{
			//existing node

			n = AMesh.get<Node>(found_id);
			tetgen2GMDSNode.push_back(n);
		}
	}
//	std::cout<<"Result"<<"\n";
//	std::cout<<"\t number of points: "<<tetgenOutput_.numberofpoints<<"\n";
//	std::cout<<"\t number of edges : "<<tetgenOutput_.numberofedges<<"\n";
//	std::cout<<"\t number of faces : "<<tetgenOutput_.numberoftrifaces<<"\n";
//	std::cout<<"\t number of tet   : "<<tetgenOutput_.numberoftetrahedra<<"\n";

	for(int i = 0; i < tetgenOutput_.numberoftetrahedra; i++){
		AMesh.newTet(tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 0] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 1] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 2] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 3] -1]);
	}

//	std::set<std::string> used_tag;
//
//	if(MeshDescriptor<M>::hasFaces){
//		for(int i = 0; i < tetgenOutput_.numberoftrifaces; i++){
//			Face f = AMesh.newTriangle(
//						 tetgen2GMDSNode[tetgenOutput_.trifacelist[i * 3 + 0] -1],
//						 tetgen2GMDSNode[tetgenOutput_.trifacelist[i * 3 + 1] -1],
//						 tetgen2GMDSNode[tetgenOutput_.trifacelist[i * 3 + 2] -1]);
//			std::stringstream ost;
//			ost<<tetgenOutput_.trifacemarkerlist[i];
//
//			if(used_tag.find(ost.str())==used_tag.end()){
//				used_tag.insert(ost.str());
//				AMesh.newSurface(ost.str());
//			}
//
//			AMesh.getSurface(ost.str()).add(f);
	//		}
	//	}
}
/*----------------------------------------------------------------------------*/
void TetgenFacade::fillGMDSOutput(gmds::Mesh& AMesh,
		std::vector<gmds::Node>& ANodes,
		std::vector<Node>& ACreatedNodes,
		std::vector<Region>& ACreatedRegions)
{
	std::vector<Node> tetgen2GMDSNode;
	tetgen2GMDSNode.reserve(tetgenOutput_.numberofpoints);
	Node n;
	for(int i = 0; i < tetgenOutput_.numberofpoints; i++){
		/* First, we try to find if a node is already located in this
		 * location. It is so a very unstable (unpredicatable) function
		 * depending on the mesh precision
		 */
		math::Point p_netgen(tetgenOutput_.pointlist[i * 3 + 0],
					tetgenOutput_.pointlist[i * 3 + 1],
					tetgenOutput_.pointlist[i * 3 + 2]);
		bool found=false;
		TCellID found_id;
		unsigned int k=0;
		for(;k<ANodes.size() && !found;k++){
			math::Point p_init = ANodes[k].point();
			if(p_init==p_netgen){
				found =true;
				found_id=ANodes[k].id();
			}
		}

		if(!found){
			//new node. we add it
			n= AMesh.newNode(tetgenOutput_.pointlist[i * 3 + 0],
					tetgenOutput_.pointlist[i * 3 + 1],
					tetgenOutput_.pointlist[i * 3 + 2]);
			tetgen2GMDSNode.push_back(n);
			ACreatedNodes.push_back(n);
		}
		else{
			//existing node

			n = AMesh.get<Node>(found_id);
			tetgen2GMDSNode.push_back(n);
		}
	}

//	std::cout<<"Result"<<"\n";
//	std::cout<<"\t number of points: "<<tetgenOutput_.numberofpoints<<"\n";
//	std::cout<<"\t number of edges : "<<tetgenOutput_.numberofedges<<"\n";
//	std::cout<<"\t number of faces : "<<tetgenOutput_.numberoftrifaces<<"\n";
//	std::cout<<"\t number of tet   : "<<tetgenOutput_.numberoftetrahedra<<"\n";

	for(int i = 0; i < tetgenOutput_.numberoftetrahedra; i++){
		Region r =
				AMesh.newTet(tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 0] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 1] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 2] -1],
					 tetgen2GMDSNode[tetgenOutput_.tetrahedronlist[i * 4 + 3] -1]);

		ACreatedRegions.push_back(r);
	}
}
/*----------------------------------------------------------------------------*/
