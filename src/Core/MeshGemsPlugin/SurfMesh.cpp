// GMDS File Headers
#include "MeshGemsPlugin/SurfMesh.h"
#include "MeshGemsPlugin/SurfMeshParam.h"
#include "MeshGemsPlugin/MGManager.h"
#include "MeshGemsPlugin/MGCurve.h"
#include "MeshGemsPlugin/MGSurface.h"
#include "MeshGemsPlugin/MGPoint.h"

// This is for the license key
#include <meshgems/meshgems_key_9624.h>
/*----------------------------------------------------------------------------*/
#include <sstream>
// TODO : the fenv include is here on order to deactivate 
//        some fe_exceptions. Needs investigation with Distene
#include <fenv.h>
/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/
SurfMesh::SurfMesh(MGManager *geom,	SurfMeshParam *param)
{
	this->geom = geom;
	this->param = param;

	c = NULL;
	dc = NULL;
	css = NULL;
	ctx = NULL;
	omsh = NULL;
	vtx = NULL;
	face = NULL;
	status = NULL;
	nv = 0;
	nel = 0;
	nf = 0;
}
/*--------------------------------------------------------------------------*/
SurfMesh::~SurfMesh()
{

	if(css)
		cadsurf_session_delete(css);

	if(vtx)
		delete[] vtx;

	if(face)
		delete[] face;

	if(status)
		delete[] status;

	if(c)
		cad_delete(c);

	if(dc)
		dcad_delete(dc);

	if(ctx)
		context_delete(ctx);

}
/*--------------------------------------------------------------------------*/
status_t SurfMesh::surf_function(real *uv, real *xyz, real *du, real *dv,
		real *duu, real *duv, real *dvv, void *user_data)
{
	status_t ret;
	MGSurface *surf = (MGSurface *)user_data;
	double PU, PV;
	gmds::math::Point XYZ;
	gmds::math::Vector D1U, D1V, D2U, D2V, D2UV;

	PU = uv[0];
	PV = uv[1];

	surf->d2(PU, PV, XYZ, D1U, D1V, D2U, D2UV, D2V);

	if(xyz){
		xyz[0] = XYZ.X();
		xyz[1] = XYZ.Y();
		xyz[2] = XYZ.Z();
	}

	if(du && dv){
		du[0] = D1U.X();
		du[1] = D1U.Y();
		du[2] = D1U.Z();

		dv[0] = D1V.X();
		dv[1] = D1V.Y();
		dv[2] = D1V.Z();
	}

	if(duu && duv && dvv){
		duu[0] = D2U.X();
		duu[1] = D2U.Y();
		duu[2] = D2U.Z();

		dvv[0] = D2V.X();
		dvv[1] = D2V.Y();
		dvv[2] = D2V.Z();

		duv[0] = D2UV.X();
		duv[1] = D2UV.Y();
		duv[2] = D2UV.Z();
	}

	return STATUS_OK;
}
/*--------------------------------------------------------------------------*/
status_t SurfMesh::curv_function(real t, real *uv, real *dt, real *dtt, void *user_data)
{
	status_t ret;
	SurfMesh::curve_ud *cud = (SurfMesh::curve_ud *)user_data;
	MGSurface *surf;
	MGCurve *curv;
	double T, UV[2], DT[2], DTT[2];
	int pid;

	surf = cud->surf;
	curv = cud->curv;

	T = t;
	curv->parametricData(surf, T, UV, DT, DTT);

	if(uv){
		uv[0] = UV[0];
		uv[1] = UV[1];
	}

	if(dt){
		dt[0] = DT[0];
		dt[1] = DT[1];
	}

	if(dtt){
		dtt[0] = DTT[0];
		dtt[1] = DTT[1];
	}

	return STATUS_OK;
}
/*--------------------------------------------------------------------------*/
status_t SurfMesh::number_of_vertices_cad_edge_function(integer eid, integer *np, void *ud)
{
	std::map<int, SurfMeshParam::esize_t> *emap = (std::map<int, SurfMeshParam::esize_t> *)ud;
	std::map<int, SurfMeshParam::esize_t>::iterator it;

	it = emap->find(eid);
	if(it != emap->end()){
		/* This face already contains some size specification */

		if(it->second.np && !it->second.t)
			*np = it->second.np;
	}else{
		*np = 0;
	}

	return STATUS_OK;
}
/*--------------------------------------------------------------------------*/
/*
 * This function specifies the sizemap on a curve.
 * The edge_id parameter is an integer identifying which curve PreCAD/cadsurf
 * is querying. This eid is the same as the one given in
 * cad_edge_new
 */
status_t SurfMesh::size_on_edge_function(integer eid, real t, real *s, void *ud)
{
	std::map<int, SurfMeshParam::esize_t> *emap = (std::map<int, SurfMeshParam::esize_t> *)ud;
	std::map<int, SurfMeshParam::esize_t>::iterator it;

	it = emap->find(eid);
	if(it != emap->end()){
		/* This face already contains some size specification */

		if(it->second.s)
			*s = it->second.s;
	}

	return STATUS_OK;
}
/*--------------------------------------------------------------------------*/
status_t SurfMesh::size_on_face_function(integer fid, real *uv, real *s, void *ud)
{
	std::map<int, double> *fmap = (std::map<int, double> *)ud;
	std::map<int, double>::iterator it;

	it = fmap->find(fid);
	if(it != fmap->end()){
		/* This face already contains some size specification */

		if(it->second)
			*s = it->second;
	}

	return STATUS_OK;
}
/*--------------------------------------------------------------------------*/
int SurfMesh::treat_error_status(int fid, MeshStatus state)
{
	std::map<int, int>::iterator it;

	it = imap.find(fid);
	if(it != imap.end()){
		status[it->second] = state;
	}else{
		return -1;
	}

	return 0;
}
/*--------------------------------------------------------------------------*/
status_t SurfMesh::message_cb(message_t *msg, void *user_data)
{
	char *desc;
	integer code, id;
	integer n, n_idata;
	double per;
	SurfMesh *sm = (SurfMesh*)(user_data);
	status_t ret;
	int res;

	ret = STATUS_OK;

	/* Get the error number */
	ret = message_get_number(msg, &code);
	if(ret != STATUS_OK)
		return ret;

	if(code == MESHGEMS_POINTCAD_CODE(5341)){

	}else if(code == MESHGEMS_CADSURF_CODE(8001)){
		/* cadsurf warning : A CAD face cannot be meshed */

		ret = message_get_integer_data(msg, 1, 1, &id);
		if(ret != STATUS_OK)
			return ret;

		res = sm->treat_error_status(id, SurfMesh::MESH_STATUS_INTERNAL_ERROR);
		if(res)
			return STATUS_ERROR;

	}else if(code == MESHGEMS_POINTCAD_CODE(8002)){
		/* PreCAD warning : a CAD face was discarded because a correct
		   definition could not be recovered */

		ret = message_get_integer_data(msg, 1, 1, &id);
		if(ret != STATUS_OK)
			return ret;

		res = sm->treat_error_status(id, SurfMesh::MESH_STATUS_WRONG_CAD_DEFINITION);
		if(res)
			return STATUS_ERROR;

	}else if(code == MESHGEMS_POINTCAD_CODE(8003)){
		/* PreCAD warning : a CAD face was discarded for optimization purposes */

		ret = message_get_integer_data(msg, 1, 1, &id);
		if(ret != STATUS_OK)
			return ret;

		res = sm->treat_error_status(id, SurfMesh::MESH_STATUS_OPTIMIZED_OUT);
		if(res)
			return STATUS_ERROR;

	}else if(code == MESHGEMS_POINTCAD_CODE(8041)){
		/* A tiny CAD edge on face with id %d has been removed */

	}else if(code == MESHGEMS_POINTCAD_CODE(8042)){
		/* A tiny CAD edge on face with id %d was left in the CAD as a results of the specified options */

	}else if(code == MESHGEMS_CADSURF_CODE(8004)){
		/* cadsurf warning : The required discretization of a CAD edge
		   cannot be respected */

	}else if(code == MESHGEMS_POINTCAD_CODE(8005) ||
			code == MESHGEMS_POINTCAD_CODE(8006)){

	}else if(code == MESHGEMS_CADSURF_CODE(9001)){

		ret = message_get_real_data(msg, 1, 1, &per);
		if(ret != STATUS_OK)
			return ret;

		ret = message_get_description(msg, &desc);
		if(ret != STATUS_OK)
			return ret;

		printf("%s", desc);

	}else{

		ret = message_get_description(msg, &desc);
		if(ret != STATUS_OK)
			return ret;

		printf("PreCAD/cadsurf message : %s", desc);
	}

	return STATUS_OK;
}
/*--------------------------------------------------------------------------*/

int SurfMesh::generateSurfMesh(){
	if(param->has_selected_faces)
		return generatePartialSurfMesh();
	else
		return generateAllSurfMesh();
}

/*--------------------------------------------------------------------------*/
int SurfMesh::generateAllSurfMesh()
{
	status_t ret;
	int i, j, fid;

	/* First import the geometry into a MeshGems CAD */

	ctx = context_new();
	c = cad_new(ctx);
	if(!c)
		return -1;

	if(param->has_required_discretisation){
		dc = dcad_new(c);
		if(!dc)
			return -1;
	}

	std::vector<MGSurface*> surfaces;
	geom->getSurfaces(surfaces);
	status = new MeshStatus[surfaces.size()];
	nf = surfaces.size();

	for(i=0;i<nf;++i){
		MGSurface* current_surface = surfaces[i];
		fid = current_surface->getId();

		imap.insert(std::pair<int, int>(fid, i));
		status[i] = MESH_STATUS_OK;

		cad_face_t *fce = cad_face_new(c, fid, surf_function, current_surface);

		ret = cad_face_set_tag(fce, fid);
		if(ret != STATUS_OK)
			return ret;

		ret = cad_face_set_uid(fce, fid);
		if(ret != STATUS_OK)
			return ret;

		if(current_surface->orientation() != MG_FORWARD){
			ret = cad_face_set_orientation(fce, CAD_ORIENTATION_REVERSED);
			if(ret != STATUS_OK)
				return ret;
		} else {
			ret = cad_face_set_orientation(fce, CAD_ORIENTATION_FORWARD);
			if(ret != STATUS_OK)
				return ret;
		}

		std::vector<MGCurve*> curves;
		current_surface->get(curves);

		for(j=0;j<curves.size();++j){
			MGCurve* current_curve = curves[j];
			double tmin, tmax;
			int eid, pid1, pid2;
			curve_ud *user_data;
			real xyz[3];
			cad_vertex_t *vv;

			user_data = new SurfMesh::curve_ud;

			user_data->surf = current_surface;
			user_data->curv = current_curve;

			cud.push_back(user_data);

			current_curve->bounds(current_surface, tmin, tmax);
			eid = current_curve->getId();

			cad_edge_t *edg = cad_edge_new(fce, eid, tmin, tmax, curv_function, user_data);

			ret = cad_edge_set_tag(edg, eid);
			if(ret != STATUS_OK)
				return ret;

			/* by default an edge is a boundary edge */
			if(current_curve->orientation() == MG_INTERNAL){
				ret = cad_edge_set_property(edg, EDGE_PROPERTY_INTERNAL);
				if(ret != STATUS_OK)
					return ret;
			}

			if(current_curve->orientation() != MG_FORWARD){
				ret = cad_edge_set_orientation(edg, CAD_ORIENTATION_REVERSED);
				if(ret != STATUS_OK)
					return ret;
			} else {
				ret = cad_edge_set_orientation(edg, CAD_ORIENTATION_FORWARD);
				if(ret != STATUS_OK)
					return ret;
			}

			MGPoint* p1 = current_curve->firstPoint();
			MGPoint* p2 = current_curve->secondPoint();

			pid1 = p1->getId();
			pid2 = p2->getId();

			ret = cad_edge_set_extremities(edg, pid1, pid2);
			if(ret != STATUS_OK)
				return ret;

			ret = cad_edge_set_extremities_tag(edg, pid1, pid2);
			if(ret != STATUS_OK)
				return ret;

			ret = cad_edge_set_uid(edg, eid);
			if(ret != STATUS_OK)
				return ret;

			xyz[0] = p1->X();
			xyz[1] = p1->Y();
			xyz[2] = p1->Z();
			vv = cad_vertex_new(c, pid1, xyz);
			if(!vv)
				return STATUS_NOMEM;

			xyz[0] = p2->X();
			xyz[1] = p2->Y();
			xyz[2] = p2->Z();
			vv = cad_vertex_new(c, pid2, xyz);
			if(!vv)
				return STATUS_NOMEM;

			if(param->has_required_discretisation){
				std::map<int, SurfMeshParam::esize_t>::iterator it;
				dcad_edge_discretization_t *de = 0;
				real t, xyz[3];
				int k;

				it = param->emap.find(eid);
				if(it != param->emap.end()){
					/* This face contains some size specification */

					if(it->second.t && it->second.xyz){
						/* A required discretization has_been specified on this edge */

						if(!dc)
							return -1;

						ret = dcad_get_edge_discretization(dc, edg, &de);
						if(ret != STATUS_OK)
							return ret;

						/* Set its vertex count (extremities included) */
						ret = dcad_edge_discretization_set_vertex_count(de, it->second.np);
						if(ret != STATUS_OK)
							return ret;

						for(k=1;k<=it->second.np;k++){
							t = it->second.t[k-1];
							xyz[0] = it->second.xyz[3*(k-1)];
							xyz[1] = it->second.xyz[3*(k-1) + 1];
							xyz[2] = it->second.xyz[3*(k-1) + 2];
							ret = dcad_edge_discretization_set_vertex_coordinates(de, k, t, 0, xyz);
							if(ret != STATUS_OK)
								return ret;
						}

						/* Mark this discretization as required */
						ret = dcad_edge_discretization_set_property(de, DCAD_PROPERTY_REQUIRED);
						if(ret != STATUS_OK)
							return ret;
					}
				}
			}
		}
	}

	/* The CAD has been imported into the MeshGems working structure, now let's create the sizemaps */
	sizemap_t *s1, *s2, *s3;

	s1 = 0;
	s2 = 0;
	s3 = 0;

	s1 = sizemap_new(c, meshgems_sizemap_type_number_of_vertices_cad_edge, (void *)number_of_vertices_cad_edge_function, &param->emap);
	if(!s1)
		return -1;

	s2 = sizemap_new(c, meshgems_sizemap_type_iso_cad_edge, (void *)size_on_edge_function, &param->emap);
	if(!s2)
		return -1;

	s3 = sizemap_new(c, meshgems_sizemap_type_iso_cad_face, (void *)size_on_face_function, &param->fmap);
	if(!s3)
		return -1;

	/* Set MG-CADSurf environment */
	css = cadsurf_session_new(ctx);
	if(!css)
		return -1;

	if(!param->has_required_discretisation){
		// this is for the license key
		meshgems_sign_cad(c);
		
		ret = cadsurf_set_cad(css, c);
		if(ret != STATUS_OK)
			return -1;
	}else{
		// this is for the license key
		meshgems_sign_cad(c);

		ret = cadsurf_set_dcad(css, dc);
		if(ret != STATUS_OK)
			return -1;
	}

	ret = cadsurf_set_message_callback(css, message_cb, this);
	if(ret != STATUS_OK)
		return -1;

	/* The parameters */

	cadsurf_set_param(css, "use_precad", "1");
	cadsurf_set_param(css, "tags", "respect");
	cadsurf_set_param(css, "required_entities", "respect");
	cadsurf_set_param(css, "physical_size_mode", "local");

	/* Required number of points */
	cadsurf_set_sizemap(css, s1);

	/* Local size on edge */
	cadsurf_set_sizemap(css, s2);

	/* Local size on face */
	cadsurf_set_sizemap(css, s3);

	std::ostringstream o;
	o << param->gradation;
	if(param->gradation)
		cadsurf_set_param(css, "gradation", o.str().c_str());

	/* Compute the mesh */
	ret = cadsurf_compute_mesh(css);
	if(ret != STATUS_OK)
		return -1;

	ret = cadsurf_get_mesh(css, &(omsh));
	if(ret != STATUS_OK)
		return -1;

	/* Clean up */
	for(i=0;i<cud.size();++i)
		delete cud[i];

	/* For debugging reasons, we write the mesh file */
	//	ret = mesh_write_mesh(omsh, "mymeshfile.mesh");
	//	if(ret != STATUS_OK)
	//		return -1;

	if(s1)
		sizemap_delete(s1);

	if(s2)
		sizemap_delete(s2);

	if(s3)
		sizemap_delete(s3);

	return 0;

}
/*--------------------------------------------------------------------------*/
int SurfMesh::generatePartialSurfMesh()
{
	status_t ret;
	int i, j, fid;

	/* First import the geometry into a MeshGems CAD */

	ctx = context_new();
	c = cad_new(ctx);
	if(!c)
		return -1;

	if(param->has_required_discretisation){
		dc = dcad_new(c);
		if(!dc)
			return -1;
	}

	std::vector<MGSurface*> surfaces;
	geom->getSurfaces(surfaces);

	status = new MeshStatus[surfaces.size()];
	nf = surfaces.size();

	for(i=0;i<nf;++i){
		MGSurface* current_surface = surfaces[i];

		fid = current_surface->getId();
		if(param->fmap_to_mesh[fid]==true) {
			imap.insert(std::pair<int, int>(fid, i));
			status[i] = MESH_STATUS_OK;

			cad_face_t *fce = cad_face_new(c, fid, surf_function, current_surface);

			ret = cad_face_set_tag(fce, fid);
			if(ret != STATUS_OK)
				return ret;

			ret = cad_face_set_uid(fce, fid);
			if(ret != STATUS_OK)
				return ret;

			if(current_surface->orientation() != MG_FORWARD){
				ret = cad_face_set_orientation(fce, CAD_ORIENTATION_REVERSED);
				if(ret != STATUS_OK)
					return ret;
			} else {
				ret = cad_face_set_orientation(fce, CAD_ORIENTATION_FORWARD);
				if(ret != STATUS_OK)
					return ret;
			}

			std::vector<MGCurve*> curves;
			current_surface->get(curves);

			for(j=0;j<curves.size();++j){
				MGCurve* current_curve = curves[j];
				double tmin, tmax;
				int eid, pid1, pid2;
				curve_ud *user_data;
				real xyz[3];
				cad_vertex_t *vv;

				user_data = new SurfMesh::curve_ud;

				user_data->surf = current_surface;
				user_data->curv = current_curve;

				cud.push_back(user_data);

				current_curve->bounds(current_surface, tmin, tmax);
				eid = current_curve->getId();//j;// geom->getIndexOf(current_curve)+1;
				bool already_done =false;
				for(int k=0;k<j && !already_done;k++){
					MGCurve* c_k = curves[k];
					int c_id = c_k->getId();
					if(c_id==eid)
						already_done=true;
				}
				cad_edge_t *edg = 0;
				if(!already_done) {
					std::cout<<" bounded by curve "<<eid;
					edg = cad_edge_new(fce, eid, tmin, tmax, curv_function, user_data);
				}
				else{
					std::cout<<" bounded by existing curve "<<eid;
					cad_face_get_edge(fce, eid,&edg);
				}
				ret = cad_edge_set_tag(edg, eid);
				if(ret != STATUS_OK)
					return ret;

				/* by default an edge is a boundary edge */
				if(current_curve->orientation() == MG_INTERNAL){
					std::cout<<"\t which is INTERNAL"<<std::endl;
					ret = cad_edge_set_property(edg, EDGE_PROPERTY_INTERNAL);
					if(ret != STATUS_OK)
						return ret;
				}
				else if(current_curve->orientation() != MG_FORWARD){
					ret = cad_edge_set_orientation(edg, CAD_ORIENTATION_REVERSED);
					std::cout<<"\t which is REVERSED"<<std::endl;
					if(ret != STATUS_OK)
						return ret;
				}
				else {
					ret = cad_edge_set_orientation(edg, CAD_ORIENTATION_FORWARD);
					std::cout<<"\t which is FORWARD"<<std::endl;
					if(ret != STATUS_OK)
						return ret;
				}

				MGPoint* p1 = current_curve->firstPoint();
				MGPoint* p2 = current_curve->secondPoint();

				pid1 = p1->getId();
				pid2 = p2->getId();
				std::cout<<" ("<<pid1<<", "<<pid2<<")"<<std::endl;
				ret = cad_edge_set_extremities(edg, pid1, pid2);
				if(ret != STATUS_OK)
					return ret;

				ret = cad_edge_set_extremities_tag(edg, pid1, pid2);
				if(ret != STATUS_OK)
					return ret;

				ret = cad_edge_set_uid(edg, eid);
				if(ret != STATUS_OK)
					return ret;

				xyz[0] = p1->X();
				xyz[1] = p1->Y();
				xyz[2] = p1->Z();
				vv = cad_vertex_new(c, pid1, xyz);
				if(!vv)
					return STATUS_NOMEM;

				xyz[0] = p2->X();
				xyz[1] = p2->Y();
				xyz[2] = p2->Z();
				vv = cad_vertex_new(c, pid2, xyz);
				if(!vv)
					return STATUS_NOMEM;

				if(param->has_required_discretisation){
					std::map<int, SurfMeshParam::esize_t>::iterator it;
					dcad_edge_discretization_t *de = 0;
					real t, xyz[3];
					int k;

					it = param->emap.find(eid);
					if(it != param->emap.end()){
						/* This face contains some size specification */

						if(it->second.t && it->second.xyz){
							/* A required discretization has_been specified on this edge */

							if(!dc){
								std::cout<<"\t **** ERROR 1"<<std::endl;
								return -1;
							}
							ret = dcad_get_edge_discretization(dc, edg, &de);
							if(ret != STATUS_OK){

								std::cout<<"\t **** ERROR 2"<<std::endl;
								return ret;
							}

							/* Set its vertex count (extremities included) */
							ret = dcad_edge_discretization_set_vertex_count(de, it->second.np);
							if(ret != STATUS_OK){

								std::cout<<"\t **** ERROR 3"<<std::endl;
								return ret;
							}

							for(k=1;k<=it->second.np;k++){
								t = it->second.t[k-1];
								xyz[0] = it->second.xyz[3*(k-1)];
								xyz[1] = it->second.xyz[3*(k-1) + 1];
								xyz[2] = it->second.xyz[3*(k-1) + 2];
								ret = dcad_edge_discretization_set_vertex_coordinates(de, k, t, 0, xyz);
								if(ret != STATUS_OK){

									std::cout<<"\t **** ERROR 4"<<std::endl;
									return ret;
								}
							}

							/* Mark this discretization as required */
							ret = dcad_edge_discretization_set_property(de, DCAD_PROPERTY_REQUIRED);
							if(ret != STATUS_OK){
								std::cout<<"\t **** ERROR 5"<<std::endl;
								return ret;
							}
						}
					}
				}

			}
		}
	}

	/* The CAD has been imported into the MeshGems working structure, now let's create the sizemaps */
	sizemap_t *s1, *s2, *s3;

	s1 = 0;
	s2 = 0;
	s3 = 0;

	s1 = sizemap_new(c, meshgems_sizemap_type_number_of_vertices_cad_edge, (void *)number_of_vertices_cad_edge_function, &param->emap);
	if(!s1)
		return -1;

	s2 = sizemap_new(c, meshgems_sizemap_type_iso_cad_edge, (void *)size_on_edge_function, &param->emap);
	if(!s2)
		return -1;

	s3 = sizemap_new(c, meshgems_sizemap_type_iso_cad_face, (void *)size_on_face_function, &param->fmap);
	if(!s3)
		return -1;

	/* Set MG-CADSurf environment */
	css = cadsurf_session_new(ctx);
	if(!css)
		return -1;

	if(!param->has_required_discretisation){
		// this is for the license key
		meshgems_sign_cad(c);
		
		ret = cadsurf_set_cad(css, c);
		if(ret != STATUS_OK)
			return -1;
	}else{
		// this is for the license key
		meshgems_sign_cad(c);
		
		ret = cadsurf_set_dcad(css, dc);
		if(ret != STATUS_OK)
			return -1;
	}

	ret = cadsurf_set_message_callback(css, message_cb, this);
	if(ret != STATUS_OK)
		return -1;

	/* The parameters */

	cadsurf_set_param(css, "use_precad", "1");
	cadsurf_set_param(css, "tags", "respect");
	cadsurf_set_param(css, "required_entities", "respect");
	cadsurf_set_param(css, "physical_size_mode", "local");

	/* Required number of points */
	cadsurf_set_sizemap(css, s1);

	/* Local size on edge */
	cadsurf_set_sizemap(css, s2);

	/* Local size on face */
	cadsurf_set_sizemap(css, s3);

	std::ostringstream o;
	o << param->gradation;
	if(param->gradation)
		cadsurf_set_param(css, "gradation", o.str().c_str());

	/* Compute the mesh */
	// TODO : contact Distene for advice on what to do with fe_exceptions
	const int FloatExceptFlags = FE_DIVBYZERO|FE_INVALID;
	fedisableexcept(FloatExceptFlags);
	ret = cadsurf_compute_mesh(css);
	if(ret != STATUS_OK)
		return -1;

	ret = cadsurf_get_mesh(css, &(omsh));
	if(ret != STATUS_OK)
		return -1;

	/* Clean up */
	for(i=0;i<cud.size();++i)
		delete cud[i];

	/* For debugging reasons, we write the mesh file */
//	ret = mesh_write_mesh(omsh, "mymeshfile.mesh");
//	if(ret != STATUS_OK)
//		return -1;

	if(s1)
		sizemap_delete(s1);

	if(s2)
		sizemap_delete(s2);

	if(s3)
		sizemap_delete(s3);

	return 0;

}
/*--------------------------------------------------------------------------*/

int SurfMesh::getMeshStatus(int &size, MeshStatus *state[])
{
	size = nf;
	*state = status;

	return 0;
}
/*--------------------------------------------------------------------------*/

int SurfMesh::getMeshVertices(int &onv, MeshVertex *ovtx[])
{
	status_t ret;
	integer nvl, uid, exact;
	real t, uv[2];

	if(!omsh)
		return -1;

	ret = mesh_get_vertex_count(omsh, &nv);
	if(ret != STATUS_OK)
		return -1;

	vtx = new MeshVertex[nv];
	if(!vtx)
		return -1;

	for(int i=1;i<=nv;++i){
		real xyz[3];
		integer tag, ntag, itag;

		ret = mesh_get_vertex_coordinates(omsh, i, vtx[i-1].xyz);

		if(ret != STATUS_OK)
			return -1;

		ret = mesh_get_vertex_tag(omsh, i, &tag);
		if(ret != STATUS_OK)
			return -1;

		nvl = 0;
		ret = cadsurf_get_vertex_location_on_vertex_count(css, i, &nvl);
		if(ret != STATUS_OK)
			return -1;

		if(nvl){
			/* The mesh vertex is on a CAD edge extremity */
			vtx[i-1].geom_dim = 0;
			vtx[i-1].geom_id = tag;
			
			// TODO : the tag is not a good way to get the geom entity ?
			//        Do this instead
			// TODO : uid is always zero, maybe because the dcad does not consider the 
			//        discretization of the geom vertices 
			cadsurf_get_vertex_location_on_vertex(css, i, 1, &uid, &exact);
			vtx[i-1].geom_id = uid;			
			
			continue;
		}

		nvl = 0;
		ret = cadsurf_get_vertex_location_on_edge_count(css, i, &nvl);
		if(ret != STATUS_OK)
			return -1;

		if(nvl){
			/* The mesh vertex is inside a CAD edge */
			ret = cadsurf_get_vertex_location_on_edge(css, i, 1, &uid, &t, &exact);
			if(ret != STATUS_OK)
				return -1;

			vtx[i-1].geom_dim = 1;
			vtx[i-1].geom_id = uid;
			continue;
		}

		nvl = 0;
		ret = cadsurf_get_vertex_location_on_face_count(css, i, &nvl);
		if(ret != STATUS_OK)
			return -1;

		if(nvl){
			/* The mesh vertex is inside a CAD face */
			ret = cadsurf_get_vertex_location_on_face(css, i, 1, &uid, uv, &exact);
			if(ret != STATUS_OK)
				return -1;

			vtx[i-1].geom_dim = 2;
			vtx[i-1].geom_id = uid;
			continue;
		}

		/* We should not end up here */
		return -1;
	}

	*ovtx = vtx;
	onv = nv;
	return 0;
}
/*--------------------------------------------------------------------------*/

int SurfMesh::getMeshFaces(int &onf, MeshFace *oface[])
{
	status_t ret;
	integer nt;

	if(!omsh)
		return -1;

	ret = mesh_get_triangle_count(omsh, &nt);
	if(ret != STATUS_OK)
		return -1;

	face = new MeshFace[nt];
	if(!face)
		return -1;

	for(int i=1;i<=nt;++i){
		real xyz[3];
		integer tag, ntag, itag;

		face[i-1].nv = 3;
		face[i-1].v = new int[3];
		ret = mesh_get_triangle_vertices(omsh, i, face[i-1].v);
		if(ret != STATUS_OK)
			return ret;

		ret = mesh_get_triangle_tag(omsh, i, &tag);
		if(ret != STATUS_OK)
			return ret;

		face[i-1].fid = tag;
	}

	*oface = face;
	onf = nt;
	return 0;
}

/*--------------------------------------------------------------------------*/

}
/*--------------------------------------------------------------------------*/
