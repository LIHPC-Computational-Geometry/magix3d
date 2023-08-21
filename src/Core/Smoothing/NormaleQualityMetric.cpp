#include "Smoothing/NormaleQualityMetric.h"
#include <MeshInterface.hpp>
#include <PatchData.hpp>

#include <TkUtil/Exception.h>

using namespace Mesquite2;

namespace Mesquite2 {

NormaleQualityMetric::NormaleQualityMetric(Mesquite2::MeshDomain * domaine) :
    AveragingQM(QualityMetric::MAX_OVER_MIN) {

  m_domain = domaine;
  MsqError err;
  MSQ_CHKERR(err);
}

std::string NormaleQualityMetric::get_name() const {
  return ("Normal based Quality Metric");
}

int NormaleQualityMetric::get_negate_flag() const {
  return (1);
}

NormaleQualityMetric::~NormaleQualityMetric() {
}

using std::vector;

bool NormaleQualityMetric::evaluate(Mesquite::PatchData & pd,
		size_t vertex_index,
		double &fval,
		Mesquite::MsqError & err)
{
  fval = 0;
  MsqMeshEntity *patchElemArray = pd.get_element_array(err);
  const MsqVertex *patchVerts = pd.get_vertex_array(err);

  vector<size_t> elem_indices;
  vector<size_t>::iterator iter;

  pd.get_vertex_element_indices(vertex_index, elem_indices, err);
  vector<size_t> temp_vert_indices;

  if (patchVerts[vertex_index].is_free_vertex() == false) {
    fval = 0.0;
    return (true);
  }

  Vector3D force(0.0, 0.0, 0.0);
  Vector3D noeud = patchVerts[vertex_index];
  Vector3D unit_normal = noeud;

  m_domain->vertex_normal_at((void *) 0, unit_normal);
  // Pour toutes les facettes contenant ce noeud
  while (!elem_indices.empty()) {
    std::vector<size_t> temp_vert_indices;
    size_t local_elm_vtx = elem_indices.back();
    Vector3D cell_centroid;
    patchElemArray[local_elm_vtx].get_centroid(cell_centroid, pd, err);

    patchElemArray[local_elm_vtx].get_connected_vertices(vertex_index,
	temp_vert_indices, err);

    Vector3D noeud4 = patchVerts[temp_vert_indices[1]];
    Vector3D noeud2 = patchVerts[temp_vert_indices[0]];

    Vector3D s = (cell_centroid - noeud) * (noeud4 - noeud2);
    if (s % unit_normal < 0.0)
      return (false);

//      double mass_contr =
//        fabs (s % (cell_centroid + 2.0 * noeud + 0.5 * (noeud2 + noeud4)));
//      s = s / mass_contr;
    force += s;
    elem_indices.pop_back();
  }

  force.normalize();

  Vector3D trans = force - (force % unit_normal) * unit_normal;
  fval = trans.length();

  return (true);

}

bool NormaleQualityMetric::
evaluate_with_indices( PatchData& pd,
                      size_t this_vert,
                      double& value,
                      std::vector<size_t>& indices,
                      MsqError& err )
{
	bool rval = evaluate(pd, this_vert, value, err);
	MSQ_ERRFALSE(err);

	indices.clear();

	MsqMeshEntity* elems = pd.get_element_array(err);
	size_t num_elems;
	const size_t *v_to_e_array = pd.get_vertex_element_adjacencies(this_vert,
			num_elems, err);
	MSQ_ERRZERO(err);

	//vector to hold the other verts which form a corner.
	vector<size_t> other_vertices;
	other_vertices.reserve(4);
	size_t i = 0;

	//loop over the elements attached to this vertex
	for (i = 0; i < num_elems; ++i) {
		//get the vertices connected to this vertex for this element
		elems[v_to_e_array[i]].get_connected_vertices(this_vert, other_vertices,
				err);
		MSQ_ERRZERO(err);
		for (unsigned j = 0; j < other_vertices.size(); ++j) {
			if (other_vertices[j] < pd.num_free_vertices())
				indices.push_back(other_vertices[j]);
		}
	}

	std::sort(indices.begin(), indices.end());
	indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
	if (this_vert < pd.num_free_vertices())
		indices.push_back(this_vert);
	return (rval);
}

}; // Mesquite2
