/*----------------------------------------------------------------------------*/
/**
 * CommandRotation.h
 *
 *  Created on: 27 sept. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDROTATION_H_
#define COMMANDROTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Rotation.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandRotation
 *  \brief Commande permettant d'effectuer la rotation d'une entité géométrique
 */
/*----------------------------------------------------------------------------*/
class CommandRotation: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param entities les entities dont on fait la révolution
     *  \param rot      la rotation
     */
    CommandRotation(Internal::Context& c,
            std::vector<GeomEntity*>& entities,
            const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd la commande de construction des entités dont on fait la révolution
     *  \param rot      la rotation
     */
    CommandRotation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
     /** \brief  Constructeur pour la rotation de toutes les entités existantes
      *
      *  \param c le contexte
      *  \param rot      la rotation
      */
     CommandRotation(Internal::Context& c,
             const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandRotation();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  Traitement spécifique délégué aux classes filles
     */
    void internalSpecificUndo();
    void internalSpecificRedo();

protected:

    /// valide les paramètres
    void validate();

    /// la rotation
    Utils::Math::Rotation m_rotation;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDROTATION_H_ */
