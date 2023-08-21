#include "Smoothing/OrthogonalQualityMetric.h"
#include <MeshInterface.hpp>
#include <PatchData.hpp>
using namespace Mesquite2;

namespace Mesquite2 {

OrthogonalQualityMetric::OrthogonalQualityMetric() :
    AveragingQM(QualityMetric::MAXIMUM) {

  MsqError err;
  MSQ_CHKERR(err);
}

OrthogonalQualityMetric::~OrthogonalQualityMetric() {
}

std::string OrthogonalQualityMetric::get_name() const {
  return ("OrthogonalQualityMetric");
}

int OrthogonalQualityMetric::get_negate_flag() const {
  return (1);
}
using std::vector;

//bool OrthogonalQualityMetric::evaluate(Mesquite2::PatchData & pd,
//    size_t vertex_index, double &fval, Mesquite2::MsqError & err) {
//  fval = 0;
//  MsqMeshEntity *patchElemArray = pd.get_element_array(err);
//  const MsqVertex *patchVerts = pd.get_vertex_array(err);
//
//  vector<size_t> elem_indices;
//  vector<size_t>::iterator iter;
//
//  pd.get_vertex_element_indices(vertex_index, elem_indices, err);
//  vector<size_t> temp_vert_indices;
//
//  double coeff = 1.0;
//  if (patchVerts[vertex_index].is_free_vertex() == false) {
//    return (true);
//  }
//
//  fval = 0.0;
//  Vector3D noeud = patchVerts[vertex_index];
//  double number = 0.0;
//
//  int num_samp = elem_indices.size();
//  double *met_vals = new double[num_samp];
//  double target_cosinus = cos(2.0 * M_PI / double(num_samp));
//  int i = 0;
//  // Pour toutes les facettes contenant ce noeud
//  while (!elem_indices.empty()) {
//    std::vector<size_t> temp_vert_indices;
//    size_t local_elm_vtx = elem_indices.back();
//    Vector3D cell_centroid;
//
//    patchElemArray[local_elm_vtx].get_connected_vertices(vertex_index,
//	temp_vert_indices, err);
//
//    Vector3D noeud4 = patchVerts[temp_vert_indices[1]];
//    Vector3D noeud2 = patchVerts[temp_vert_indices[0]];
//
//    Vector3D s1 = noeud4 - noeud;
//    Vector3D s2 = noeud2 - noeud;
//    double cosine = (s1 % s2) / s1.length() / s2.length();
//
//    double val = (cosine - target_cosinus) * (cosine - target_cosinus);
//    met_vals[i] = val;
//    elem_indices.pop_back();
//    i++;
//  }
//
//  fval = average_metrics(met_vals, num_samp, err);
//  MSQ_ERRZERO(err);
//  delete[] met_vals;
//  return true;
//
//}
//
//bool OrthogonalQualityMetric::evaluate_with_indices(PatchData& pd,
//    size_t this_vert, double& value, std::vector<size_t>& indices,
//    MsqError& err) {
//  bool rval = evaluate(pd, this_vert, value, err);
//  MSQ_ERRFALSE(err);
//
//  indices.clear();
//
//  MsqMeshEntity* elems = pd.get_element_array(err);
//  size_t num_elems;
//  const size_t *v_to_e_array = pd.get_vertex_element_adjacencies(this_vert,
//      num_elems, err);
//  MSQ_ERRZERO(err);
//
//  //vector to hold the other verts which form a corner.
//  vector<size_t> other_vertices;
//  other_vertices.reserve(4);
//  size_t i = 0;
//
//  //loop over the elements attached to this vertex
//  for (i = 0; i < num_elems; ++i) {
//    //get the vertices connected to this vertex for this element
//    elems[v_to_e_array[i]].get_connected_vertices(this_vert, other_vertices,
//	err);
//    MSQ_ERRZERO(err);
//    for (unsigned j = 0; j < other_vertices.size(); ++j) {
//      if (other_vertices[j] < pd.num_free_vertices())
//	indices.push_back(other_vertices[j]);
//    }
//  }
//
//  std::sort(indices.begin(), indices.end());
//  indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
//  if (this_vert < pd.num_free_vertices())
//    indices.push_back(this_vert);
//  return (rval);
//}

//#if 0
//bool OrthogonalQualityMetric::evaluate_element(PatchData & pd,
//    MsqMeshEntity * element, double &value, MsqError & err) {
//  int i;
//
//  value = MSQ_MAX_CAP;
//  const size_t *v_i = element->get_vertex_index_array();
//  int nb_vertex = element->vertex_count();
//
//  double met_vals[MSQ_MAX_NUM_VERT_PER_ENT];
//
//  size_t e_ind = pd.get_element_index(element);
//  //Vector3D temp_vec[6];
//  MsqVertex *vertices = pd.get_vertex_array(err);
//  MSQ_ERRZERO(err);
//
//  // Let us first suppose the user knows when to use this function
//  // so that it knows that element is either a triangle or a quadrangle !
//
//  if (nb_vertex != 4) {
//    value = 0;
//    return false;
//  }
//
//  Vector3D & n0 = vertices[v_i[0]];
//  Vector3D & n1 = vertices[v_i[1]];
//  Vector3D & n2 = vertices[v_i[2]];
//  Vector3D & n3 = vertices[v_i[3]];
//
//  Vector3D s1 = n1 - n0;
//  Vector3D s2 = n2 - n1;
//  Vector3D s3 = n3 - n2;
//  Vector3D s4 = n0 - n3;
//
//  s1.normalize();
//  s2.normalize();
//  s3.normalize();
//  s4.normalize();
//
//  value = (s1 % s2) * (s1 % s2) + (s2 % s3) * (s2 % s3) + (s3 % s4) * (s3 % s4)
//  + (s4 % s1) * (s4 % s1);
//
//  if (m_scalar != 0.0) {
//    // sur une idée originale de P. Knupp...
//
//    double length = (s1 % s1) + (s2 % s2) + (s3 % s3) + (s4 % s4);
//    value = m_scalar * value + (1.0 - m_scalar) * length;
//  }
//
//  MSQ_ERRZERO(err);
//  return true;
//}
//
//#endif


bool OrthogonalQualityMetric::evaluate (Mesquite::PatchData & pd,
	size_t element_index,
	double &value,
	Mesquite::MsqError & err)
  {
	int i;

	value = MSQ_MAX_CAP;

	std::vector<Vector3D> coords;
	pd.get_element_vertex_coordinates(element_index, coords, err);
	MSQ_ERRZERO (err);

	int nb_vertex = coords.size();

	double met_vals[MSQ_MAX_NUM_VERT_PER_ENT];

	// Let us first suppose the user knows when to use this function
	// so that it knows that element is either a triangle or a quadrangle !

	for (int i = 0; i < nb_vertex; i++)
	{
		int i0 = (i + (nb_vertex - 1)) % nb_vertex;
		int i2 = (i + 1) % nb_vertex;

		Vector3D &n0 = coords[i0];
		Vector3D &n1 = coords[i];
		Vector3D &n2 = coords[i2];

		Vector3D s1 = n2 - n1;
		Vector3D s2 = n0 - n1;
		double cosine = (s1 % s2) / s1.length () / s2.length ();
		double val = cosine * cosine;

		met_vals[i] = val;
	}

	value = average_metrics (met_vals, nb_vertex, err);
	MSQ_ERRZERO (err);
#ifdef _DEBUG2
	std::cout << "evaluate_element : avec "<<nb_vertex<<" sommets"
			<<" => " << value << std::endl;
#endif
	return true;
  }

}
;
