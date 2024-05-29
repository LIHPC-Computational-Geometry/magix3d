/*----------------------------------------------------------------------------*/
/*
 * GeomNewPrismImplementation.h
 *
 *  Created on: 22 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMNEWPRISMIMPLEMENTATION_H_
#define GEOMNEWPRISMIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/PropertyPrism.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomNewPrismImplementation
 * \brief Classe permettant de créer un prisme à partir d'une surface en
 *        suivant une direction donnée par un vecteur. C'est une opération de
 *        modification car la face extrudée doit faire partie du nouveau volume
 *        créé
 *
 *	  OBSOLETE: du fait d'un plantage non résolu lors du undo-redo
 *	            et du fait que cela n'agisse que sur une unique surface
 *
 */
class GeomNewPrismImplementation: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param prop définit les propriétés du prime (sa face de départ et le
     *         vecteur de direction)
     */
    GeomNewPrismImplementation(Internal::Context& c,PropertyPrism* prop);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomNewPrismImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'opération
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();



protected:

    /* propriété du prisme */
    PropertyPrism* m_prop;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMNEWPRISMIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

