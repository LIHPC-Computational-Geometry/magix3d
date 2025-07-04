#ifndef GEOMROTATIONIMPLEMENTATION_H_
#define GEOMROTATIONIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
class CommandGeomCopy;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomRotationImplementation
 * \brief Classe réalisant la translation d'une entité géométrique.
 *
 */
class GeomRotationImplementation : public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param entities les entities dont on fait la révolution
     *  \param axis1    premier point definissant l'axe de rotation
     *  \param axis2    second point definissant l'axe de rotation
     *  \param angle    angle de rotation en degré (compris entre ]0,360])
     */
    GeomRotationImplementation(Internal::Context& c,
            std::vector<GeomEntity*> entities,
            const Utils::Math::Point& axis1,
            const Utils::Math::Point& axis2,
            const double& angle);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param cmd      la commande de création des entités à faire tourner
     *  \param axis1    premier point definissant l'axe de rotation
     *  \param axis2    second point definissant l'axe de rotation
     *  \param angle    angle de rotation en degré (compris entre ]0,360])
     */
    GeomRotationImplementation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const Utils::Math::Point& axis1,
            const Utils::Math::Point& axis2,
            const double& angle);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour toutes les entités existantes
     *
     *  \param c le contexte
     *  \param axis1    premier point definissant l'axe de rotation
     *  \param axis2    second point definissant l'axe de rotation
     *  \param angle    angle de rotation en degré (compris entre ]0,360])
     */
    GeomRotationImplementation(Internal::Context& c,
            const Utils::Math::Point& axis1,
            const Utils::Math::Point& axis2,
            const double& angle);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomRotationImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la translation
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation du undo/redo pour les entités qui n'ont pas leur état stocké dans le Memento de la commande
     */
    void performUndo();
    void performRedo();

private:
    void makeRevol(GeomEntity* e);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la rotation de l'entité géométrique de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param shape l'entité géométrique
     *  \param P1    premier point definissant l'axe de rotation
     *  \param P2    second point definissant l'axe de rotation
     *  \param Angle angle de rotation en degré (compris entre ]0,360])
     */
    TopoDS_Shape rotate(const TopoDS_Shape& shape, const Utils::Math::Point& P1, const Utils::Math::Point& P2, double Angle) const;

protected:


    /* points définissant l'axe de révolution */
    Utils::Math::Point m_axis1;
    Utils::Math::Point m_axis2;

    /* angle de révolution */
    double m_angle;

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
#endif /* GEOMROTATIONIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


