#include "Smoothing/DistanceMdlQualityMetric.h"
#include <MsqVertex.hpp>
#include <MsqError.hpp>
#include <MeshInterface.hpp>
#include <AveragingQM.hpp>
#include <QualityMetric.hpp>
#include <PatchData.hpp>

DistanceMdlQualityMetric::DistanceMdlQualityMetric(Mesquite2::MeshDomain * mdl) :
    Mesquite2::AveragingQM(Mesquite2::QualityMetric::MAXIMUM) {
  m_domain = mdl;
  Mesquite2::MsqError err;
  MSQ_CHKERR(err);
}

std::string DistanceMdlQualityMetric::get_name() const {
  return ("Distance a la modelisation");
}
int DistanceMdlQualityMetric::get_negate_flag() const {
  return (1);
}

DistanceMdlQualityMetric::~DistanceMdlQualityMetric() {
}

bool DistanceMdlQualityMetric::evaluate(Mesquite2::PatchData & pd,
    size_t this_vert, double &fval, Mesquite2::MsqError & err) {

  const Mesquite2::MsqVertex *vertices = pd.get_vertex_array(err);
  fval = 0;
  Mesquite2::Vector3D vert = vertices[this_vert];
  MSQ_ERRZERO(err);

  if (vertices[this_vert].is_free_vertex()) {
    Mesquite2::Vector3D pos3d = vert;

    m_domain->snap_to((Mesquite2::Mesh::EntityHandle) 0, pos3d);
    fval = (pos3d - vert).length();
  }
  return (true);
}


bool DistanceMdlQualityMetric::evaluate_with_indices(
                                              Mesquite2::PatchData& pd,
                                              size_t this_vert,
                                              double& value,
                                              std::vector<size_t>& indices,
                                              Mesquite2::MsqError& err )
{
  bool rval = evaluate(pd, this_vert, value, err);
  MSQ_ERRFALSE(err);

  indices.clear();

  if (this_vert < pd.num_free_vertices())
    indices.push_back( this_vert );
  return (rval);
}

