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
#include "Geom/MementoGeomEntity.h"
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
    virtual ~CommandCreateWithOtherGeomEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();


protected:

    /*------------------------------------------------------------------------*/
    /** sauvegarde pour une entité géométrique des connections topologiques */
    void saveMemento(GeomEntity* entity);

    /*------------------------------------------------------------------------*/
    /** permute les propriétés internes avec leur sauvegarde
     */
    void permMementos();

    /*------------------------------------------------------------------------*/
    /** destruction des propriétés internes des entités modifiés
     */
    void deleteMementos();


protected:

    /** pour les entitiés géométriques modifiées lors de l'opération, on
     * stocke leur "état interne" sous forme de mémento. Ce stockage est de la
     * responsabilité de chaque entité */
    std::map<GeomEntity*,MementoGeomEntity> m_mementos;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATEWITHOTHERGEOMENTITIES_H_ */
/*----------------------------------------------------------------------------*/

