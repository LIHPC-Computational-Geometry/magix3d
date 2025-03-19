/*----------------------------------------------------------------------------*/
/*
 * CommandCreateWithOtherGeomEntities.h
 *
 *  Created on: 29/08/2018
 *      Author: Eric Brière de l'Isle
 *
 *      Se base sur CommandEditGeom pour la gestion des mementos,
 *      mais sans lien avec GeomModificationBaseClass
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCREATEWITHOTHERGEOMENTITIES_H_
#define COMMANDCREATEWITHOTHERGEOMENTITIES_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/MementoManager.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
class GeomEntity;
/*----------------------------------------------------------------------------*/
/** \class CommandCreateWithOtherGeomEntities
 *  \brief Partie commune à toutes les commandes de création
 *         d'entités géométriques utilisant d'autres entités existantes
 *
/*----------------------------------------------------------------------------*/
class CommandCreateWithOtherGeomEntities: public Geom::CommandCreateGeom {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte (permet l'accès au manager)
     * \param le nom de la commande
     */
	CommandCreateWithOtherGeomEntities(Internal::Context& c, std::string name,
                        const std::string& groupName="");

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction de l'entité
     */
    virtual ~CommandCreateWithOtherGeomEntities() = default;

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();

protected:
    MementoManager m_memento_manager;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATEWITHOTHERGEOMENTITIES_H_ */
/*----------------------------------------------------------------------------*/

