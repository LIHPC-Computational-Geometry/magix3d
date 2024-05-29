/*----------------------------------------------------------------------------*/
/*
 * \file Matrix33.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_MATRIX33_H_
#define UTILS_MATRIX33_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
namespace Utils {
namespace Math {
/*----------------------------------------------------------------------------*/
/// classe utilitaire pour gérer les rotations par exemple
class Matrix33
{
public:
  /** Matrice identité */
  Matrix33();

  /**
         Matrice avec une rotation élémentaire
  @param angle en radians, rotation dans le sens directe autour de l'axe qui suit
  @param direction 0 pour axe Ox, 1 pour axe Oy et 2 pour axe Oz
  */
  Matrix33(double angle, int dir);

  Matrix33& operator=(const Matrix33& mat);

  /** Retourne vraie si la matrice est l'identité */
  bool isIdentite();

  /** Multiplication de la matrice par un point */
  Point operator* (const Point & pt) const;

  /** Multiplication de la matrice par une autre matrice */
  Matrix33 operator* (const Matrix33 & mat);

#ifndef SWIG
  friend std::ostream & operator << (std::ostream & , const Matrix33 &);
#endif

  /** retourne les valeurs de la matrice
  @param ligne indice de la ligne (0, 1 ou 2)
  @param colonne indice de colonne (0, 1 ou 2)
  */
  double getCoord(int ligne, int colonne) const;

  /** change les valeurs de la matrice
  @param ligne indice de la ligne (0, 1 ou 2)
  @param colonne indice de colonne (0, 1 ou 2)
  */
  void setCoord(int ligne, int colonne, double coord);

  // inverse la matrice actuelle
  void inverse();

private:
  // le stockage se fait ligne par ligne, ce qui facilite les opérations Matrix33*Point
  Point m_ligne1;
  Point m_ligne2;
  Point m_ligne3;

};
/*----------------------------------------------------------------------------*/
} // end namespace Math
} // end namespace Utils
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTILS_MATRIX33_H_ */
