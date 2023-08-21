/*----------------------------------------------------------------------------*/
/*
 * \file Bounds.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17 sept. 2013
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_BOUNDS_H_
#define MGX3D_UTILS_BOUNDS_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/**
   @brief Gestion d'une boite englobante
 */
class Bounds {
public:
    /// Constructeur avec une première coordonnée
    Bounds(double x, double y, double z);
    Bounds(Math::Point pt);

    /// Destructeur
    ~Bounds()
    { }

    /// Ajoute une coordonnée
    void add(double x, double y, double z);
    void add(Math::Point pt);

    /// Retourne la boite englobante
    void get(double bounds[6]) const;

private:
    double m_x_min;
    double m_x_max;
    double m_y_min;
    double m_y_max;
    double m_z_min;
    double m_z_max;
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_UTILS_BOUNDS_H_ */
