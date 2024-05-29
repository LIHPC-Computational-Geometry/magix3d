/*----------------------------------------------------------------------------*/
#ifndef SURFMESH_H_
#define SURFMESH_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
extern "C" {
#include <meshgems/meshgems.h>
#include <meshgems/cadsurf.h>
}
/*----------------------------------------------------------------------------*/
#include "SurfMeshParam.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{

class MGCurve;
class MGManager;
class MGSurface;

class SurfMesh {

public:

	typedef enum MeshStatus_t{
		MESH_STATUS_OK,
		MESH_STATUS_INTERNAL_ERROR,
		MESH_STATUS_MEMORY_ERROR,
		MESH_STATUS_OPTIMIZED_OUT,
		MESH_STATUS_WRONG_CAD_DEFINITION
	}MeshStatus;

	typedef struct curve_ud_t{
		MGSurface *surf;
		MGCurve *curv;
	}curve_ud;

	typedef struct MeshVertex_t{

		double xyz[3];
		int geom_dim;
		int geom_id;

	}MeshVertex;

	typedef struct MeshFace_t{

		int nv;
		int *v;
		int fid;

	}MeshFace;

	SurfMesh(MGManager*geom, SurfMeshParam *param);
	~SurfMesh();
	int generateSurfMesh();
	int getMeshVertices(int &np, MeshVertex *vtx[]);
	int getMeshFaces(int &ntri, MeshFace *face[]);
	int getMeshStatus(int &nf, MeshStatus *status[]);

	/* Static callbacks for the MeshGems integration */
	static status_t surf_function(real *uv, real *xyz, real *du, real *dv,
			real *duu, real *duv, real *dvv, void *user_data);

	static status_t curv_function(real t, real *uv, real *dt, real *dtt, void *user_data);

	static status_t number_of_vertices_cad_edge_function(integer eid, integer *np, void *ud);
	static status_t size_on_edge_function(integer eid, real t, real *s, void *ud);
	static status_t size_on_face_function(integer eid, real *uv, real *s, void *ud);
	static status_t message_cb(message_t *msg, void *user_data);
	/* End of the static callbacks */

	int treat_error_status(int fid, MeshStatus status);
private:
      int generateAllSurfMesh();
      int generatePartialSurfMesh();
private:

	MGManager *geom;
	SurfMeshParam *param;

	int nv;
	MeshVertex *vtx;

	int nel;
	MeshFace *face;

	int nf;
	MeshStatus *status;
	std::map<int, int> imap;

	context_t *ctx;
	std::vector<curve_ud*> cud;
	cad_t *c;
	dcad_t *dc;

	cadsurf_session_t *css;
	mesh_t *omsh;

};
/*----------------------------------------------------------------------------*/
} // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* SURFMESHPARAM_H_ */


