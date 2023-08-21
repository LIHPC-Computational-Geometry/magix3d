/*----------------------------------------------------------------------------*/
/*
 * PropertyHollowSpherePart.h
 *
 *  Created on: 29 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYHOLLOWSPHEREPART_H_
#define PROPERTYHOLLOWSPHEREPART_H_
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
 * \class PropertyHollowSpherePart
 * \brief Décrit les propriétés d'une aiguille creuse
 */
class PropertyHollowSpherePart: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param radius_int le rayon interne de l'aiguille creuse
     *  \param radius_ext le rayon externe de l'aiguille creuse
     *  \param angleY   l'angle d'ouverture autour de x dans plan Oxy
     *  \param angleZ   l'angle d'ouverture autour de x dans plan Oxz
     */
	PropertyHollowSpherePart(const double radius_int,
			const double radius_ext,
			const double angleY,
			const double angleZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyHollowSpherePart();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon interne de l'aiguille
     */
    double getRadiusInt() const;
    void setRadiusInt(const double r);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon externe de l'aiguille
     */
    double getRadiusExt() const;
    void setRadiusExt(const double r);

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

    /** rayon interne */
    double m_radius_int;
    /** rayon externe */
    double m_radius_ext;
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
#endif /* PROPERTYHOLLOWSPHEREPART_H_ */
/*----------------------------------------------------------------------------*/

