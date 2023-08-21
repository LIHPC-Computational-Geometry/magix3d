/*----------------------------------------------------------------------------*/
/*
 * GeomRemoveImplementation.h
 *
 *  Created on: 10 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMREMOVEIMPLEMENTATION_H_
#define GEOMREMOVEIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
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
 * \class GeomRemoveImplementation
 * \brief Classe réalisant la suppression d'entités géométriques
 *
 */
class GeomRemoveImplementation: public GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param es            les entités à supprimer
     *  \param propagateDown vaut true si on veut supprimer les entités incidentes
     *                       de dimension inférieure
     */
    GeomRemoveImplementation(Internal::Context& c, std::vector<GeomEntity*>& es,
            bool propagateDown);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomRemoveImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la suppression
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


private:

    /** Entités qui vont être modifiées par l'algorithme. Purement technique pour
     * faire le lien entre initialize() et finalize()
     *
     */
    std::vector<GeomEntity*> entities_to_update;
    std::vector<GeomEntity*> entities_new;


    /* propagation ou non de la suppression */
    bool m_propagate;
    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMREMOVEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
