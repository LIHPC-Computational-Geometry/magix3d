/*----------------------------------------------------------------------------*/
/*
 * GeomCutImplementation.h
 *
 *  Created on: 14 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMCUTIMPLEMENTATION_H_
#define GEOMCUTIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
#include <vector>
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
 * \class GeomCutImplementation
 * \brief Classe réalisant la différence Booléenne de deux entités
 *        géométriques volumiques (dim=3)
 *
 */
class GeomCutImplementation: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e l'entité à conserver
     *  \param es les entités à retirer de e
     */
    GeomCutImplementation(Internal::Context& c,
                          GeomEntity* e, std::vector<GeomEntity*> es);
    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomCutImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la coupe
     *
     *  \param res les entités géométriques créées par l'opération (sans compter
     *             les sous-entités)
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();



protected:

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la différence de volumes
     *
     *  \param res les entités géométriques créées par l'opération (sans compter
     *             les sous-entités)
     */
    void cutVolumes(std::vector<GeomEntity*>& res);

//    /*------------------------------------------------------------------------*/
//    /** \brief  réalisation de la différence de 2 volumes
//     *
//     *  \param e1 l'entité géométrique à conserver
//     *  \param e2 l'entité géométrique à conserver
//     *  \param res les entités obtenues
//     */
//    void cutVolumes(GeomEntity* e1, GeomEntity* e2,
//                    std::vector<GeomEntity*>& res);
//

    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMCUTIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



