/*----------------------------------------------------------------------------*/
/*
 * GeomNewVolumeImplementation.h
 *
 *  Created on: 3 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMNEWVOLUMEIMPLEMENTATION_H_
#define GEOMNEWVOLUMEIMPLEMENTATION_H_
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
 * \class GeomNewVolumeImplementation
 * \brief Classe réalisant la création d'un volume à partir d'un ensemble de
 *        surfaces
 *
 */
class GeomNewVolumeImplementation: public GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param es les surfaces entourant le volume à créer

     */
    GeomNewVolumeImplementation(Internal::Context& c, std::vector<Surface*>& es);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomNewVolumeImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la création du volume
     *          Les entités créées sont stockées dans res
     */
    void perform(std::vector<GeomEntity*>& res);


    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


private:

    /* entités passées en argument à la commande. */
    std::vector<Surface*> m_surfaces_param;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMNEWVOLUMEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

