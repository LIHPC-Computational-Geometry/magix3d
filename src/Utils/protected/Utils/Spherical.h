/*----------------------------------------------------------------------------*/
/*
 * \file Spherical.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_SPERICAL_H_
#define UTILS_SPERICAL_H_
/*----------------------------------------------------------------------------*/
#include <iostream>
#include "Utils/Point.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
namespace Utils {
namespace Math {
/*----------------------------------------------------------------------------*/
class Spherical {
  /// distance de l'origine O au point p
  double m_rho;
  /// angle (en degré) entre Oz et droite 0p
  double m_theta;
  /// angle (en degré) entre 0x et 0p' où p' est le projeté de p dans le plan 0xy
  double m_phi;

public:
  Spherical () : m_rho(0.0), m_theta(0.0), m_phi(0.0) {}
  Spherical (double rho, double theta, double phi) : m_rho(rho), m_theta(theta), m_phi(phi) {}

  explicit Spherical ( const Point & p)
  {
    double x = p.getX();
    double y = p.getY();
    double z = p.getZ();
    double rayon = std::sqrt(x*x+y*y+z*z);
    double inv = (rayon>0.0 ? 1.0 / rayon:0.0);
    x *= inv;
    y *= inv;
    z *= inv;

    m_rho = rayon;
    m_theta = std::acos(z);
    double sintheta = std::sin (m_theta);
    if (MgxNumeric::isNearlyZero(sintheta) || x==0.0)
    	m_phi = 0.0;
    else
    	m_phi = std::atan2 (y / sintheta, x / sintheta)*180/M_PI;
    m_theta *= 180/M_PI;
  }

  double getRho() const { return m_rho; }
  double getTheta() const { return m_theta; }
  double getPhi () const { return m_phi; }
  void setRho(double rho) { m_rho = rho; }
  void setTheta(double theta) { m_theta = theta; }
  void setPhi (double phi) { m_phi = phi; }

#ifndef SWIG
  friend std::ostream & operator << (std::ostream & o, const Spherical & s)
  { // theta,phi en degrés
    o << " [ " << s.m_rho << ", " <<s.m_theta << ", " << s.m_phi << "] ";
    return o;
  }
#endif

};
/*----------------------------------------------------------------------------*/
} // end namespace Math
} // end namespace Utils
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTILS_SPERICAL_H_ */
