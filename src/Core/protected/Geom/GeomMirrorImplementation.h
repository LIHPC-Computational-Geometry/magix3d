/*
 * GeomMirrorImplementation.h
 *
 *  Created on: 12/4/2016
 *      Author: Eric B
 */

#ifndef GEOMMIRRORIMPLEMENTATION_H_
#define GEOMMIRRORIMPLEMENTATION_H_

/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
/*----------------------------------------------------------------------------*/
#include "Utils/Plane.h"
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
 * \class GeomMirrorImplementation
 * \brief Classe réalisant la symétrie d'entités géométriques
 *
 */
class GeomMirrorImplementation : public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à symétriser
     *  \param plane le plan de symétrie
     */
    GeomMirrorImplementation(Internal::Context& c,
    		std::vector<GeomEntity*>& e,
    		const Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param cmd la commande de création des entités à symétriser
     *  \param plane le plan de symétrie
     */
    GeomMirrorImplementation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
    		const Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param plane le plan de symétrie
     */
    GeomMirrorImplementation(Internal::Context& c,
    		const Utils::Math::Plane* plane);
    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomMirrorImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la symétrie
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
    /** \brief  symétrie des entités
     *
     *  \param e l'entité à symétriser
     */
    void mirrorSingle(GeomEntity* e);

protected:

    /** Plan de symétrie */
    Utils::Math::Plane m_plane;

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
#endif /* GEOMMIRRORIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

