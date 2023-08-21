/*----------------------------------------------------------------------------*/
/** \file GeomTranslateImplementation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 10/12/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMTRANSLATEIMPLEMENTATION_H_
#define GEOMTRANSLATEIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
#include <set>
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
class CommandGeomCopy;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomTranslateImplementation
 * \brief Classe réalisant la translation d'une entité géométrique.
 *
 */
class GeomTranslateImplementation : public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à translater
     *  \param dv le vecteur de translation
     */
    GeomTranslateImplementation(Internal::Context& c,
    		std::vector<GeomEntity*>& e,
            const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param cmd la commande de création des entités à translater
     *  \param dv le vecteur de translation
     */
    GeomTranslateImplementation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param dv le vecteur de translation
     */
    GeomTranslateImplementation(Internal::Context& c,
            const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomTranslateImplementation();

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

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  translation des entités descendantes
     *
     *  \param e l'entité à translater
     */
    void translateSingle(GeomEntity* e);

protected:

    /* vecteur de translation */
    Utils::Math::Vector m_dv;

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
#endif /* GEOMTRANSLATEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

