#ifndef _NORMALEQUALITYMETRIC_H_
#define _NORMALEQUALITYMETRIC_H_

#include <Mesquite.hpp>
#include <MeshInterface.hpp>
#include <VertexQM.hpp>
#include <AveragingQM.hpp>

using namespace Mesquite2;

namespace Mesquite2 {

class NormaleQualityMetric: public VertexQM, public AveragingQM {

  Mesquite2::MeshDomain * m_domain;
public:
  NormaleQualityMetric(Mesquite2::MeshDomain *);
  virtual std::string get_name() const;
  virtual int get_negate_flag() const;
  virtual ~NormaleQualityMetric();
protected:
	bool evaluate(Mesquite::PatchData & pd,
			size_t vertex_index,
			double &fval,
			Mesquite::MsqError & err);

  bool evaluate_with_indices( PatchData& pd,
                      size_t handle,
                      double& value,
                      std::vector<size_t>& indices,
                      MsqError& err );
};

}
;

#endif //_NORMALEQUALITYMETRIC_H_
