/*----------------------------------------------------------------------------*/
/*
 * \file Cylindrical.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/5/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_CYLINDRICAL_H_
#define UTILS_CYLINDRICAL_H_
/*----------------------------------------------------------------------------*/
#include <iostream>
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
namespace Utils {
namespace Math {
/*----------------------------------------------------------------------------*/
class Cylindrical {
  /// distance de l'origine O au point p' où p' est le projeté de p dans le plan 0xy
  double m_rho;
  /// angle (en degré) entre 0x et 0p' où p' est le projeté de p dans le plan 0xy
  double m_phi;
  /// élévation z
  double m_z;

public:
  Cylindrical () : m_rho(0.0), m_phi(0.0), m_z(0.0) {}
  Cylindrical (double rho, double phi, double z) : m_rho(rho), m_phi(phi), m_z(z) {}

  explicit Cylindrical ( const Point & p)
  {
    double x = p.getX();
    double y = p.getY();
    double z = p.getZ();
    double rayon = std::sqrt(x*x+y*y);
    double inv = (rayon>0.0 ? 1.0 / rayon:0.0);
    x *= inv;
    y *= inv;

    m_rho = rayon;
    m_z = z;
    m_phi = (x==0.0?0.0:std::atan2 (y , x))*180/M_PI;
  }

  double getRho() const { return m_rho; }
  double getZ() const { return m_z; }
  double getPhi () const { return m_phi; }
  void setRho(double rho) { m_rho = rho; }
  void setZ(double z) { m_z = z; }
  void setPhi (double phi) { m_phi = phi; }

#ifndef SWIG
  friend std::ostream & operator << (std::ostream & o, const Cylindrical & s)
  { // theta,phi en degrés
    o << " [ " << s.m_rho << ", " << s.m_phi << ", " <<s.m_z<< "] ";
    return o;
  }
#endif

};
/*----------------------------------------------------------------------------*/
} // end namespace Math
} // end namespace Utils
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTILS_CYLINDRICAL_H_ */
