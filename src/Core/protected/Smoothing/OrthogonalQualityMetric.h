#ifndef OTHOGONALQUALITYMETRIC_HPP_
#define OTHOGONALQUALITYMETRIC_HPP_

#include <Mesquite.hpp>
#include <MeshInterface.hpp>
#include <ElementQM.hpp>
#include <AveragingQM.hpp>

namespace Mesquite2 {

class OrthogonalQualityMetric: public ElementQM, public AveragingQM {
public:
  OrthogonalQualityMetric();
  virtual ~ OrthogonalQualityMetric();
protected:
  virtual std::string get_name() const;
  virtual int get_negate_flag() const;

  bool evaluate(PatchData & pd, size_t handle, double & fval,
      MsqError &err);
/*  bool evaluate_with_indices( PatchData& pd,
                      size_t handle,
                      double& value,
                      std::vector<size_t>& indices,
                      MsqError& err );
*/
};

};

#endif /*OTHOGONALQUALITYMETRIC_HPP_ */
