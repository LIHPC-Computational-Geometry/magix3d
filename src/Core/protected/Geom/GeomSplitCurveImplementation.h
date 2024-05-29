/*----------------------------------------------------------------------------*/
/*
 * GeomSplitCurveImplementation.h
 *
 *  Created on: 12/4/2018
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMSPLITCURVEIMPLEMENTATION_H_
#define GEOMSPLITCURVEIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/Curve.h"
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
 * \class GeomSplitCurveImplementation
 * \brief Classe réalisant le découpage d'une courbe
 *
 */
class GeomSplitCurveImplementation: public GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param crv la courbe sur laquelle est appliqué le découpage

     */
    GeomSplitCurveImplementation(Internal::Context& c, Curve* crv, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomSplitCurveImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation du découpage
     *          Les entités créées sont stockées dans res
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:

private:

    /** Entités qui vont être modifiées par l'algorithme. Purement technique pour
     * faire le lien entre initialize() et finalize()
     *
     */
    std::vector<GeomEntity*> entities_to_update;
    std::vector<GeomEntity*> entities_new;

    /* entités passées en argument à la commande. */
    Curve* m_entity_param;

    /* position de la découpe */
    Point m_pt;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMSPLITCURVEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
