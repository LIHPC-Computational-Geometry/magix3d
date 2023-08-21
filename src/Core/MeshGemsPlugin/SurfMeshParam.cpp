// GMDS File Headers
#include "MeshGemsPlugin/SurfMeshParam.h"

#include<string>

/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/

SurfMeshParam::SurfMeshParam()
{
	has_local_size_on_face = 0;
	has_local_size_on_edge = 0;
	has_required_discretisation = 0;
    has_selected_faces = 0;
	has_required_np = 0;
	gradation = 0;
}
SurfMeshParam::~SurfMeshParam()
{
	std::map<int, esize_t>::iterator it=emap.begin();
	while(it!=emap.end()){
		esize_t current = it->second;
		if(current.t!=0)
			delete[] current.t;
		if(current.xyz!=0)
			delete[] current.xyz;

		it++;
	}

}
void SurfMeshParam::setFaceToMesh(int fid, bool v){

	fmap_to_mesh.insert(std::pair<int, bool>(fid, v));
	has_selected_faces = 1;
}


int SurfMeshParam::setFaceSize(int fid, double size)
{
	std::map<int, double>::iterator it;
	double s;

	has_local_size_on_face = 1;
	it = fmap.find(fid);
	if(it != fmap.end()){
		/* This face already contains some size specification */
		s = it->second;
		if(s != size)
			return -1;
	}else{
		/* New face size specification */
		fmap.insert(std::pair<int, double>(fid, size));

	}
	return 0;
}

int SurfMeshParam::setEdgeSize(int eid, double size)
{
	std::map<int, esize_t>::iterator it;
	double s;

	has_local_size_on_edge = 1;
	it = emap.find(eid);
	if(it != emap.end()){
		/* This face already contains some size specification */
		s = it->second.s;
		if(s != size)
			return -1;
	}else{
		/* New face size specification */
		esize_t esiz;

		esiz.s = size;
		esiz.np = 0;
		esiz.t = NULL;
		esiz.xyz = NULL;

		emap.insert(std::pair<int, esize_t>(eid, esiz));

	}

	return 0;
}

int SurfMeshParam::setEdgeRequiredNbPoint(int eid, int np)
{
	std::map<int, esize_t>::iterator it;
	double s;

	has_required_np = 1;
	it = emap.find(eid);
	if(it != emap.end()){
		/* This face already contains some size specification */

		if(it->second.s)
			return -1;

		if(it->second.np != np)
			return -1;

		if(it->second.t || it->second.xyz)
			return -1;

	}else{
		/* New face size specification */

		esize_t esiz;

		esiz.s = 0;
		esiz.np = np;
		esiz.t = 0;
		esiz.xyz = 0;

		emap.insert(std::pair<int, esize_t>(eid, esiz));

	}

	return 0;
}

int SurfMeshParam::setEdgeRequiredDiscretization(int eid, int np, double *t, double *xyz)
{
	std::map<int, esize_t>::iterator it;
	double s;

	has_required_discretisation = 1;
	it = emap.find(eid);
	if(it != emap.end()){
		/* This edge already contains some size specification */

		if(it->second.s)
			return -1;

		if(it->second.np != np)
			return -1;

		if(it->second.t || it->second.xyz)
			return -1;

	}else{
		/* New face size specification */
		esize_t esiz;

		esiz.s = 0;
		esiz.np = np;
		esiz.t = new double[np];
		esiz.xyz = new double[3*np];
		for(int i=0; i<np; i++){
			esiz.t[i] = t[i];
			esiz.xyz[3*i]   = xyz[3*i];
			esiz.xyz[3*i+1] = xyz[3*i+1];
			esiz.xyz[3*i+2] = xyz[3*i+2];

		}
		emap.insert(std::pair<int, esize_t>(eid, esiz));

	}
	return 0;
}

int SurfMeshParam::setMeshGradation(double g)
{
	gradation = g;
	return 0;
}

}
/*----------------------------------------------------------------------------*/
