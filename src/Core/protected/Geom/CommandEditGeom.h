/*----------------------------------------------------------------------------*/
/*
 * CommandEditGeom.h
 *
 *  Created on: 25 janv. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEDITGEOM_H_
#define COMMANDEDITGEOM_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/MementoGeomEntity.h"
/*----------------------------------------------------------------------------*/
#include "Geom/Volume.h"
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
/** \class CommandEditGeom
 *  \brief Partie commune à toutes les commandes de création et de modification
 *         d'entités géométriques faisant disparaitre ou modifiant des entités
 *         géométriques déjà présentes dans le modèle
 */
/*----------------------------------------------------------------------------*/
class CommandEditGeom: public Geom::CommandCreateGeom {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte (permet l'accès au manager)
     * \param le nom de la commande
     */
    CommandEditGeom(Internal::Context& c, std::string name,
                        const std::string& groupName="");

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du volume
     */
    virtual ~CommandEditGeom();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande en déléguant un traitement spécifique
     *          aux classes filles
     */
    void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();

    /*------------------------------------------------------------------------*/
    /** \brief  Pré-traitement spécifique délégué aux classes filles
     */
    virtual void internalSpecificPreExecute()=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Traitement spécifique délégué aux classes filles
     */
    virtual void internalSpecificExecute()=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Traitement spécifique délégué aux classes filles
     */
    virtual void internalSpecificUndo(){}
    virtual void internalSpecificRedo(){}

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités supprimées. N'a de sens
     *          qu'une fois l'opération perform() appelée.
     */
    virtual std::vector<GeomEntity*>& getRemovedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les nouvelles entités. N'a de sens
     *          qu'une fois l'opération perform() appelée.
     */
    virtual std::vector<GeomEntity*>& getNewEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités conservées et
     *          potentiellement modifées. N'a de sens qu'une fois l'opération
     *          perform() appelée.
     */
    virtual std::vector<GeomEntity*> getKeepedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités conservées et déplacées
     *          qu'une fois l'opération perform() appelée.
     */
    virtual std::vector<GeomEntity*>& getMovedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur toutes les entités  sur lesquelles
     *          l'algorithme travaille
     */
    virtual std::list<GeomEntity*>& getRefEntities(const int dim);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur toutes les entités  adjacentes
     */
    virtual std::list<GeomEntity*>& getAdjEntities(const int dim);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur une map indiquant par quelles
     *          entités une entité supprimée a été remplacée
     */
    virtual std::map<GeomEntity*,std::vector<GeomEntity*> >& getReplacedEntities();


protected:

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne la commande de modifications ce qui permet d'accéder
     *          aux listes d'entités modifiés, détruites et aux correspondances
     *          entre les anciennes et les nouvelles
     */
    virtual GeomModificationBaseClass* getGeomModificationBaseClass()
    {return m_impl;}

    /*------------------------------------------------------------------------*/
    /** parcours les entités modifiées par l'opération géométrique et pour
     *  celles modifiées dans entities, conserve le memento associé
     */
    void saveMementos(std::map<GeomEntity*,MementoGeomEntity> & entities);

    /*------------------------------------------------------------------------*/
    /** permute les propriétés internes avec leur sauvegarde
     */
    void permMementos();

    /*------------------------------------------------------------------------*/
    /** fait le ménage pour le cas d'une commande en erreur
     */
    void cancelMementos();

    /*------------------------------------------------------------------------*/
    /** destruction des propriétés internes des entités modifiés
     */
    void deleteMementos();

    /*------------------------------------------------------------------------*/
    /** Copie les groupes d'une entité vers une autre (de même dimension) */
    void copyGroups(GeomEntity* ge1, GeomEntity* ge2);

    /** Met à jour les groupes pour les entités modifiées (création, destruction ...) */
    virtual void updateGroups();


protected:

    /** pour les entitiés géométriques modifiées lors de l'opération, on
     * stocke leur "état interne" sous forme de mémento. Ce stockage est de la
     * responsabilité de chaque entité */
    std::map<GeomEntity*,MementoGeomEntity> m_mementos;

    /* objet gérant l'opération OCC*/
    GeomModificationBaseClass* m_impl;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEDITGEOM_H_ */
/*----------------------------------------------------------------------------*/

