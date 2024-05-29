/*----------------------------------------------------------------------------*/
/** \file GeomProperty.h
 *
 *  \author Franck Ledoux
 *
 *  \date 18/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMPROPERTY_H_
#define GEOMPROPERTY_H_
/*----------------------------------------------------------------------------*/
//inclusion de fichiers de la STL
#include <string>
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class PropertyBox;
class PropertyCylinder;
class PropertyHollowCylinder;
class PropertyCone;
class PropertySphere;
class PropertyHollowSphere;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomProperty
 * \brief Définit une interface que devront implémenter les propriétés que l'on
 *        attache à un objet géométrique. Par exemple, une boite a comme
 *        propriétés sa longueur, sa largeur et sa hauteur. Un cylindre son
 *        rayon et sa hauteur.
 *
 *        TODO [FL EB] Quant une propriété est modifiée, l'objet associé doit l'être.
 *        Par exemple, si on change la hauteur d'une boite dans la propriété
 *        associé l'objet géométrique représentant cette boite doit être modifié.
 */
class GeomProperty{

public:

    typedef enum{
        DEFAULT,
        BOX,
        CYLINDER,
        CONE,
        SPHERE,
        PRISM,
        REVOL,
        HOLLOW_CYLINDER,
        HOLLOW_SPHERE,
		SPHERE_PART,
		HOLLOW_SPHERE_PART
    } type;

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GeomProperty();

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GeomProperty();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type
     */
    virtual type getType() const;

    /*------------------------------------------------------------------------*/
    /** Retourne une chaine avec le nom correspondant au type */
    static std::string typeToString(type& t);

    /*------------------------------------------------------------------------*/
    /** \brief  Convertit this en une propriété sur une boite. Renvoie un
     *          pointeur nul si cette conversion n'est pas valide.
     */
    PropertyBox* toPropertyBox();

    /*------------------------------------------------------------------------*/
    /** \brief  Convertit this en une propriété sur un cylindre. Renvoie un
     *          pointeur nul si cette conversion n'est pas valide.
     */
    PropertyCylinder* toPropertyCylinder();

    /*------------------------------------------------------------------------*/
    /** \brief  Convertit this en une propriété sur un cylindre creux. Renvoie un
     *          pointeur nul si cette conversion n'est pas valide.
     */
    PropertyHollowCylinder* toPropertyHollowCylinder();

    /*------------------------------------------------------------------------*/
    /** \brief  Convertit this en une propriété sur un cone. Renvoie un
     *          pointeur nul si cette conversion n'est pas valide.
     */
    PropertyCone* toPropertyCone();

    /*------------------------------------------------------------------------*/
    /** \brief  Convertit this en une propriété sur une sphère. Renvoie un
     *          pointeur nul si cette conversion n'est pas valide.
     */
    PropertySphere* toPropertySphere();

    /*------------------------------------------------------------------------*/
    /** \brief  Convertit this en une propriété sur une sphère creuse. Renvoie un
     *          pointeur nul si cette conversion n'est pas valide.
     */
    PropertyHollowSphere* toPropertyHollowSphere();

    /*------------------------------------------------------------------------*/
    /** permet l'affichage des propriétés spécifiques de l'objet */
    virtual void addDescription(Utils::SerializedRepresentation& geomProprietes) const;


};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMPROPERTY_H_ */
/*----------------------------------------------------------------------------*/


