/*----------------------------------------------------------------------------*/
/*
 * PropertyPrism.h
 *
 *  Created on: 21 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYPRISM_H_
#define PROPERTYPRISM_H_
/*----------------------------------------------------------------------------*/
//inclusion de fichiers de la STL
#include <string>
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class PropertyPrism
 * \brief Décrit les propriétés d'un prisme
 */
class PropertyPrism: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param base la base du prisme
     *  \param axis l'axe du prism
     */
    PropertyPrism(GeomEntity* base, const Utils::Math::Vector& axis);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyPrism();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur la base du prisme
     */
    GeomEntity* getBase() const;


    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur l'axe du cylindre
     */
    Utils::Math::Vector getAxis() const;
    void setAxis(const Utils::Math::Vector& axis);

    /*------------------------------------------------------------------------*/
    /** permet l'affichage des propriétés spécifiques de l'objet */
    virtual void addDescription(Utils::SerializedRepresentation& geomProprietes) const;

protected:

    /** base du prisme */
    GeomEntity* m_base;
    /** vecteur d'alignement du cylindre */
    Utils::Math::Vector m_axis;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROPERTYPRISM_H_ */
/*----------------------------------------------------------------------------*/

