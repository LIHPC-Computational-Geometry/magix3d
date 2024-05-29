#ifndef _DISTANCEMDLQUALTITYMETRIC_HPP_
#define _DISTANCEMDLQUALTITYMETRIC_HPP_

#include <Mesquite.hpp>
#include <MeshInterface.hpp>
#include <VertexQM.hpp>
#include <AveragingQM.hpp>
#include <PatchData.hpp>

class DistanceMdlQualityMetric: public Mesquite2::VertexQM,
    public Mesquite2::AveragingQM {
private:
  Mesquite2::MeshDomain *m_domain;
public:
  DistanceMdlQualityMetric(Mesquite2::MeshDomain *);
  virtual ~DistanceMdlQualityMetric();

  virtual std::string get_name() const;
  virtual int get_negate_flag() const;

  bool evaluate(Mesquite2::PatchData & pd, size_t handle, double & fval,
      Mesquite2::MsqError &err);

  bool evaluate_with_indices(Mesquite2::PatchData& pd, size_t this_vert, double& value,
      std::vector<size_t>& indices, Mesquite2::MsqError& err);
};

#endif //_DISTANCEMDLQUALTITYMETRIC_HPP_
