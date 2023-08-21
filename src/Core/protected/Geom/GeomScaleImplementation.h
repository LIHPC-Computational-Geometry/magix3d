/*
 * GeomScaleImplementation.h
 *
 *  Created on: 19 juin 2013
 *      Author: ledouxf
 */

#ifndef GEOMSCALEIMPLEMENTATION_H_
#define GEOMSCALEIMPLEMENTATION_H_

/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Internal/Context.h"
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
 * \class GeomScaleImplementation
 * \brief Classe réalisant l'homothétie d'entités géométriques
 *
 */
class GeomScaleImplementation : public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à scaler
     *  \param factor le facteur de scaling
     *  \param pcentre centre du scaling
     */
    GeomScaleImplementation(Internal::Context& c,
    		std::vector<GeomEntity*>& e,
            const double factor, const Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *  \param cmd la commande de création des entités à scaler
     *  \param factor le facteur de scaling
     *  \param pcentre centre du scaling
     */
    GeomScaleImplementation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const double factor, const Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param factor le facteur de scaling
     *  \param pcentre centre du scaling
     */
    GeomScaleImplementation(Internal::Context& c,
            const double factor, const Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à scaler
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
    */
    GeomScaleImplementation(Internal::Context& c,
    		std::vector<GeomEntity*>& e,
    		const double factorX,
    		const double factorY,
    		const double factorZ);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param cmd la commande de création des entités à scaler
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
    */
    GeomScaleImplementation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
    		const double factorX,
    		const double factorY,
    		const double factorZ);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    GeomScaleImplementation(Internal::Context& c,
    		const double factorX,
    		const double factorY,
    		const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomScaleImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'homothétie
     */
    void perform(std::vector<GeomEntity*>& res);

    void prePerform();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation du undo/redo pour les entités qui n'ont pas leur état stocké dans le Memento de la commande
     */
    void performUndo();
    void performRedo();

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  homothétie des entités descendantes
     *
     *  \param entities les entités à translater
     */
    //void scaleDownEntities(std::list<GeomEntity*>& entities);
    void scaleSingle(GeomEntity* e);


protected:

    /** facteur d'homothétie*/
    double m_factor;

    /// vrai si l'on utilise un facteur unique pour toutes les directions
    bool m_isHomogene;
    /** facteur d'homothétie suivant X */
    double m_factorX;
    /** facteur d'homothétie suivant Y */
    double m_factorY;
    /** facteur d'homothétie suivant Z */
    double m_factorZ;

    /** centre d'homothétie*/
    Point m_center;

    /** entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_modifiedEntities;

    /** entités nécessitant d'être modifiées lors du Undo/Redo (FacetedSurface) */
    std::vector<GeomEntity*> m_undoableEntities;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMSCALEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

