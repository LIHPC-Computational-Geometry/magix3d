/*----------------------------------------------------------------------------*/
/*
 * PropertySpherePart.h
 *
 *  Created on: 21 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYSPHEREPART_H_
#define PROPERTYSPHEREPART_H_
/*----------------------------------------------------------------------------*/
//inclusion de fichiers de la STL
#include <string>
/*----------------------------------------------------------------------------*/
#include "Utils/Constants.h"
#include "Geom/GeomProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class PropertySpherePart
 * \brief Décrit les propriétés d'une aiguille pleine
 */
class PropertySpherePart: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param radius  le rayon de la sphère
     *  \param angleY   l'angle d'ouverture autour de x dans plan Oxy
     *  \param angleZ   l'angle d'ouverture autour de x dans plan Oxz
     */
    PropertySpherePart(const double radius,
                   	   const double angleY,  const double angleZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertySpherePart();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon de l'aiguille
     */
    double getRadius() const;
    void setRadius(const double r);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur l'angle suivant y
     */
    double getAngleY() const;
    void setAngleY(const double r);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur l'angle suivant z
     */
    double getAngleZ() const;
    void setAngleZ(const double r);

    /*------------------------------------------------------------------------*/
    /** permet l'affichage des propriétés spécifiques de l'objet */
    virtual void addDescription(Utils::SerializedRepresentation& geomProprietes) const;

protected:

    /** rayon */
    double m_radius;
    /** angle Y */
    double m_angleY;
    /** angle Z */
    double m_angleZ;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROPERTYSPHEREPART_H_ */
/*----------------------------------------------------------------------------*/

