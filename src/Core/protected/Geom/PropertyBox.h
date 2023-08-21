/*----------------------------------------------------------------------------*/
/** \file PropertyBox.h
 *
 *  \author Franck Ledoux
 *
 *  \date 18/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYBOX_H_
#define PROPERTYBOX_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomProperty.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class PropertyBox
 * \brief Décrit les propriétés d'une boite parallépipédique parallèle aux axes
 *        Ox, Oy, Oz
 */
class PropertyBox: public GeomProperty{

public:


    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param pnt point min de la boite
     *  \param dx  dimension selon Ox
     *  \param dy  dimension selon Oy
     *  \param dz  dimension selon Oz
     */
    PropertyBox(const Utils::Math::Point& pnt = Utils::Math::Point(0,0,0),
                const double& dx=1, const double& dy=1, const double& dz=1);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyBox();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le point
     */
    Utils::Math::Point getPnt() const;
    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur en X
     */
    double getX() const;
    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur en Y
     */
    double getY() const;
    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur en X
     */
    double getZ() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Modificateur du point min
     */
    void setPnt(Utils::Math::Point& pnt);
    /*------------------------------------------------------------------------*/
    /** \brief  Modificateur en X
     */
    void setX(const double& dx);
    /*------------------------------------------------------------------------*/
    /** \brief  Modificateur en Y
     */
    void setY(const double& dy);
    /*------------------------------------------------------------------------*/
    /** \brief  Modificateur en Z
     */
    void setZ(const double& dz);

    /*------------------------------------------------------------------------*/
    /** permet l'affichage des propriétés spécifiques de l'objet */
    virtual void addDescription(Utils::SerializedRepresentation& geomProprietes) const;

protected:

    Utils::Math::Point m_pnt;
    double m_x;
    double m_y;
    double m_z;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROPERTYBOX_H_ */
/*----------------------------------------------------------------------------*/
