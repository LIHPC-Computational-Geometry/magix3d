//
//  Lissage orthogonal à la KNUPP AO en 3D
// Pour chaque noeud d'une surface entouré par 4 mailles quadrangulaires
// on calcule sa position ideale
//
#include <vector>

#include "Vector3D.hpp"
#include "Smoothing/OrthogonalSmoothing.h"

#include <MeshInterface.hpp>

#include "PatchData.hpp"
#include "MsqError.hpp"
#include <ElementPatches.hpp>
#include <VertexPatches.hpp>

namespace Mesquite2 {

OrthogonalSmoothing::OrthogonalSmoothing(int nb_iteration) :
    		m_nbiter(nb_iteration) {
}

OrthogonalSmoothing::~OrthogonalSmoothing() {
}

// Organise les 4 noeuds pour que le noeud_1 soit le second
static void organise(MsqMeshEntity * element, int transf[4], size_t noeud_1,
		MsqError & err) {
	int found = -1;
	size_t *elementVertArray = element->get_vertex_index_array();

	if (element->node_count() != 4) {
		MSQ_SETERR (err)(MsqError::INTERNAL_ERROR);
		return;
	}

	for (size_t j = 0; j < 4; ++j) {
		if (elementVertArray[j] == noeud_1) {
			found = j;
		}
	}

	if (found == -1) {
		MSQ_SETERR (err)(MsqError::INTERNAL_ERROR);
		return;
	}

	// Je l'ai trouvé !

	for (int j = 0; j < 4; j++) {
		transf[j] = (found + 3 + j) % 4;	// 3 est congru à -1 modulo 4
	}

}

double OrthogonalSmoothing::loop_over_mesh(MeshDomainAssoc * meshAndDomain,
		const Settings * settings, MsqError & err) {
	PatchData patch_data;
	patch_data.set_mesh(meshAndDomain->get_mesh());
	patch_data.set_domain(meshAndDomain->get_domain());

	// Clear culling flag, set hard fixed flag, etc on all vertices
	// A ne pas oublier !
	initialize_vertex_byte(meshAndDomain, settings, err);
	MSQ_ERRZERO(err);

//	ElementPatches elem_patches;
//	elem_patches.set_mesh(meshAndDomain->get_mesh());

	VertexPatches vert_patches(1,true);
	vert_patches.set_mesh(meshAndDomain->get_mesh());

	std::vector<PatchSet::PatchHandle> patches;
	std::vector<Mesh::ElementHandle> patch_elems;
	std::vector<Mesh::VertexHandle> patch_verts;

	//  elem_patches.get_patch_handles(patches, err);
	vert_patches.get_patch_handles(patches, err);
	MSQ_ERRZERO(err);
	size_t free_vtx = 0;
	//loop over the patches (ie, loop over the vertices.

	for (int niter = 0; niter < m_nbiter; niter++) {
		for (std::vector<PatchSet::PatchHandle>::iterator p = patches.begin();
				p != patches.end(); ++p) {
			vert_patches.get_patch(*p, patch_elems, patch_verts, err);
			MSQ_ERRZERO(err);

			bool is_structured_vertex = true;

			patch_data.set_mesh_entities(patch_elems, patch_verts, err);
			MSQ_ERRZERO(err);
			int num_vertices = patch_data.num_fixed_vertices()
			  + patch_data.num_free_vertices();
			int num_elements = patch_data.num_elements();
			if (num_vertices != 9 || num_elements != 4)
				is_structured_vertex = false;

			const MsqVertex *vertArray = patch_data.get_vertex_array(err);
			MsqMeshEntity *elementArray = patch_data.get_element_array(err);

			// On repère le noeud commun (c'est celui qui apparait 4 fois)

			if (patch_data.is_vertex_free(0))
			{
				if (is_structured_vertex) {


					int transf_quad[4][4];
					for (size_t i = 0; i < 4; i++)
						organise(&elementArray[i], transf_quad[i], free_vtx, err);

					int noeud[9];
					for (size_t i = 0; i < 9; i++)
						noeud[i] = -1;
					size_t *elementVertArray = elementArray[0].get_vertex_index_array();

					// Le premier quadrangle donne le premier quad rangle !
					noeud[3] = elementVertArray[transf_quad[0][0]];
					noeud[4] = elementVertArray[transf_quad[0][1]];
					noeud[1] = elementVertArray[transf_quad[0][2]];
					noeud[0] = elementVertArray[transf_quad[0][3]];

					// On cherche les trois autres quadrangles
					for (size_t i = 1; i < 4; i++) {
						elementVertArray = elementArray[i].get_vertex_index_array();
						if (elementVertArray[transf_quad[i][0]] == noeud[1]) {
							noeud[2] = elementVertArray[transf_quad[i][3]];
							noeud[5] = elementVertArray[transf_quad[i][2]];
						} else if (elementVertArray[transf_quad[i][2]] == noeud[3]) {
							noeud[6] = elementVertArray[transf_quad[i][3]];
							noeud[7] = elementVertArray[transf_quad[i][0]];
						} else
							noeud[8] = elementVertArray[transf_quad[i][3]];

					}

					// On calcule la position ideale du point...
					Vector3D p[9];
					for (size_t i = 0; i < 9; i++)
						p[i] = vertArray[noeud[i]];

					Vector3D ve = 0.5 * (p[5] - p[3]);
					Vector3D vn = 0.5 * (p[1] - p[7]);
					Vector3D vne = 0.25 * (p[2] - p[0] + p[6] - p[8]);

					double g11 = ve.length_squared();
					double g22 = vn.length_squared();

					if (g11 + g22 != 0.0) {

						Vector3D v53 = p[5] + p[3];
						Vector3D v17 = p[1] + p[7];

						// On va pouvoir bouger
						double x = (.5 * (g22 * v53.x() + g11 * v17.x())
								+ 2.0 * ve.x() * vn.x() * vne.x()
								+ 1.0 * vne.y() * (ve.x() * vn.y() + ve.y() * vn.x())
								+ 1.0 * vne.z() * (ve.x() * vn.z() + ve.z() * vn.x()))
	    		  / (g11 + g22);
						double y = (.5 * (g22 * v53.y() + g11 * v17.y())
								+ 2.0 * ve.y() * vn.y() * vne.y()
								+ 1.0 * vne.x() * (ve.x() * vn.y() + ve.y() * vn.x())
								+ 1.0 * vne.z() * (ve.y() * vn.z() + ve.z() * vn.y()))
	    		  / (g11 + g22);
						double z = (.5 * (g22 * v53.z() + g11 * v17.z())
								+ 2.0 * ve.z() * vn.z() * vne.z()
								+ 1.0 * vne.y() * (ve.z() * vn.y() + ve.y() * vn.z())
								+ 1.0 * vne.x() * (ve.x() * vn.z() + ve.z() * vn.x()))
	    		  / (g11 + g22);

						Vector3D ao(x, y, z);

						patch_data.set_vertex_coordinates(ao, free_vtx, err);
						patch_data.snap_vertex_to_domain(free_vtx, err);
						patch_data.update_mesh(err);
					}
				} else

				{
					// Not structured Vertex => on le place au milieu de ses voisins
					Vector3D somme = Vector3D(0.0, 0.0, 0.0);
					for (int i = 0; i < num_vertices; i++)
						if (i != free_vtx)
							somme += vertArray[i];
					somme = somme / (double) (num_vertices - 1);
					patch_data.set_vertex_coordinates(somme, free_vtx, err);
					patch_data.snap_vertex_to_domain(free_vtx, err);
					patch_data.update_mesh(err);
				}

			}
			else
				std::cout<<"un sommet est figé"<<std::endl;
		} // for p
	} // for niter
	return 0.0;
}

}
