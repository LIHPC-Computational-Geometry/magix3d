/*----------------------------------------------------------------------------*/
/*
 * \file Matrix33.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Matrix33.h"
#include <sys/types.h>
#include <TkUtil/Exception.h>
#include <cstdio>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
namespace Math {
/*----------------------------------------------------------------------------*/
Matrix33::Matrix33()
  : m_ligne1(1,0,0)
  , m_ligne2(0,1,0)
  , m_ligne3(0,0,1)
{}
//---------------------------------------------------------------------------------------
Matrix33::Matrix33(double angle, int dir)
{
  double angle_rad = angle;//*M_PI/180.0;
  switch (dir){
  case 0:
    m_ligne1.setX(1);
    m_ligne2.setY(std::cos(angle_rad));
    m_ligne2.setZ(-std::sin(angle_rad));
    m_ligne3.setY(std::sin(angle_rad));
    m_ligne3.setZ(std::cos(angle_rad));
    break;
  case 1:
    m_ligne2.setY(1);
    m_ligne3.setZ(std::cos(angle_rad));
    m_ligne3.setX(-std::sin(angle_rad));
    m_ligne1.setZ(std::sin(angle_rad));
    m_ligne1.setX(std::cos(angle_rad));
    break;
  case 2:
    m_ligne3.setZ(1);
    m_ligne1.setX(std::cos(angle_rad));
    m_ligne1.setY(-std::sin(angle_rad));
    m_ligne2.setX(std::sin(angle_rad));
    m_ligne2.setY(std::cos(angle_rad));
    break;
  default:
      TkUtil::UTF8String   messageErr (TkUtil::Charset::UTF_8);
      messageErr << "direction non prévue  "<<(short)dir<<" pour Matrix33";
      throw TkUtil::Exception(messageErr);
  }
}
//---------------------------------------------------------------------------------------
Matrix33& Matrix33::operator=(const Matrix33& mat)
{
  if (&mat != this){
    m_ligne1 = mat.m_ligne1;
    m_ligne2 = mat.m_ligne2;
    m_ligne3 = mat.m_ligne3;
  }
  return (*this);
}
//---------------------------------------------------------------------------------------
bool Matrix33::isIdentite()
{
  return ((m_ligne1.getX() == 1.0)
      && (m_ligne2.getY() == 1.0)
      && (m_ligne3.getZ() == 1.0)); // la matrice est normalisé, on ne test pas que le reste est à 0
}
//---------------------------------------------------------------------------------------
Point Matrix33::operator* (const Point & pt) const
{
  return Point(m_ligne1.dot(pt), m_ligne2.dot(pt), m_ligne3.dot(pt));
}
//---------------------------------------------------------------------------------------
Matrix33 Matrix33::operator* (const Matrix33 & mat)
{
#ifdef DEBUG
  std::cout << "Matrix33::operator*(mat)\n";
  std::cout << " this: " << *this;
  std::cout << " mat: " << mat;
#endif

  Matrix33 newMat;

  Point A[3]; // les lignes de la matrice A (this)
  Point B[3]; // les colonnes de la matrice B (mat)

  A[0] = m_ligne1;
  A[1] = m_ligne2;
  A[2] = m_ligne3;

  B[0].setX(mat.m_ligne1.getX());
  B[0].setY(mat.m_ligne2.getX());
  B[0].setZ(mat.m_ligne3.getX());

  B[1].setX(mat.m_ligne1.getY());
  B[1].setY(mat.m_ligne2.getY());
  B[1].setZ(mat.m_ligne3.getY());

  B[2].setX(mat.m_ligne1.getZ());
  B[2].setY(mat.m_ligne2.getZ());
  B[2].setZ(mat.m_ligne3.getZ());

  for (int i=0; i<3; i++) {
    newMat.m_ligne1.setCoord(i,A[0].dot(B[i]));
    newMat.m_ligne2.setCoord(i,A[1].dot(B[i]));
    newMat.m_ligne3.setCoord(i,A[2].dot(B[i]));
  }

#ifdef DEBUG
  std::cout << " return: " << newMat;
#endif
  return newMat;
}
//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & o , const Matrix33 & mat)
{
  o << "Matrix33 :\n";
  o << "    " << mat.m_ligne1 << std::endl;
  o << "    " << mat.m_ligne2 << std::endl;
  o << "    " << mat.m_ligne3 << std::endl;
  return o;
}
//---------------------------------------------------------------------------------------
double Matrix33::getCoord(int ligne, int colonne) const
{
  switch (ligne){
  case 0:
    return m_ligne1.getCoord(colonne);
    break;
  case 1:
    return m_ligne2.getCoord(colonne);
    break;
  case 2:
    return m_ligne3.getCoord(colonne);
    break;
  default:
      TkUtil::UTF8String   messageErr (TkUtil::Charset::UTF_8);
      messageErr << "indice de ligne hors limite ("<<(short)ligne<<") pour Matrix33";
      throw TkUtil::Exception(messageErr);
  }
}
//---------------------------------------------------------------------------------------
void Matrix33::setCoord(int ligne, int colonne, double coord)
{
  switch (ligne){
  case 0:
    m_ligne1.setCoord(colonne,coord);
    break;
  case 1:
    m_ligne2.setCoord(colonne,coord);
    break;
  case 2:
    m_ligne3.setCoord(colonne,coord);
    break;
  default:
      TkUtil::UTF8String   messageErr (TkUtil::Charset::UTF_8);
      messageErr << "indice de ligne hors limite ("<<(short)ligne<<") pour Matrix33";
      throw TkUtil::Exception(messageErr);
  }
}
//---------------------------------------------------------------------------------------
void Matrix33::inverse()
{
  if (isIdentite())
    return;

  double mxx = getCoord(0,0);
  double mxy = getCoord(0,1);
  double mxz = getCoord(0,2);

  double myx = getCoord(1,0);
  double myy = getCoord(1,1);
  double myz = getCoord(1,2);

  double mzx = getCoord(2,0);
  double mzy = getCoord(2,1);
  double mzz = getCoord(2,2);

  // le déterminant
  double det =
    mxx*(myy*mzz-myz*mzy)
    - mxy*(myx*mzz-myz*mzx)
    + mxz*(myx*mzy-myy*mzx);

  double inv_d = 1.0 / det;

  setCoord(0,0,(-myz*mzy+myy*mzz)*inv_d);
  setCoord(1,0,( myz*mzx-myx*mzz)*inv_d);
  setCoord(2,0,(-myy*mzx+myx*mzy)*inv_d);

  setCoord(0,1,( mxz*mzy-mxy*mzz)*inv_d);
  setCoord(1,1,(-mxz*mzx+mxx*mzz)*inv_d);
  setCoord(2,1,( mxy*mzx-mxx*mzy)*inv_d);

  setCoord(0,2,(-mxz*myy+mxy*myz)*inv_d);
  setCoord(1,2,( mxz*myx-mxx*myz)*inv_d);
  setCoord(2,2,(-mxy*myx+mxx*myy)*inv_d);
}
/*----------------------------------------------------------------------------*/
} // end namespace Math
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
