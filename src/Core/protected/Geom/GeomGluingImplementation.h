/*----------------------------------------------------------------------------*/
/*
 * GeomGluingImplementation.h
 *
 *  Created on: 1 déc. 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMSEWINGIMPLEMENTATION_H_
#define GEOMSEWINGIMPLEMENTATION_H_
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
 * \class GeomGluingImplementation
 * \brief Classe réalisant le collage de deux entités géométriques
 *
 */
class GeomGluingImplementation: public GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param es les entités sur lesquelles appliquer le splitting

     */
    GeomGluingImplementation(Internal::Context& c, std::vector<GeomEntity*>& es);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomGluingImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation du collage
     *          Les entités créées sont stockées dans res
     */
    void perform(std::vector<GeomEntity*>& res);


    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'union de volumes
     */
    void sewVolumes(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'union de courbes
     */
    void sewCurves(std::vector<GeomEntity*>& res);

private:

    /** Entités qui vont être modifiées par l'algorithme. Purement technique pour
     * faire le lien entre initialize() et finalize()
     *
     */
    std::vector<GeomEntity*> entities_to_update;
    std::vector<GeomEntity*> entities_new;


    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMSEWINGIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


