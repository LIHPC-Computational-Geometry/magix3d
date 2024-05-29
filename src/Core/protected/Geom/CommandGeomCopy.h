/*
 * CommandGeomCopy.h
 *
 *  Created on: 19 juin 2013
 *      Author: ledouxf
 */

#ifndef COMMANDGEOMCOPY_H_
#define COMMANDGEOMCOPY_H_

/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandGeomCopy
 *  \brief Commande permettant de créer une boite
 */
/*----------------------------------------------------------------------------*/
class CommandGeomCopy: public Geom::CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param entities les entités à copier
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandGeomCopy(Internal::Context& c,
            std::vector<GeomEntity*>& entities,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour la copie de toutes les entités géométriques
     *
     *  \param c le contexte
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandGeomCopy(Internal::Context& c,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandGeomCopy();


    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();


    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();

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

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur une map indiquant par quelle
     *          entité une entité de référence a été copiée
     */
    virtual std::map<GeomEntity*, GeomEntity* >& getRef2NewEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités à copier
     */
    virtual const std::vector<GeomEntity*>& getEntities() const;

    /*------------------------------------------------------------------------*/
    /** Met à jour les groupes pour les entités modifiées (création, destruction ...) */
    virtual void updateGroups();


protected:

    void validate();
    /* entités à copier */
    std::vector<GeomEntity*> m_entities;
    std::vector<std::list  <GeomEntity*> > m_ref_entities;
    std::vector<GeomEntity*> m_new_entities;
    std::vector<GeomEntity*> m_removed_entities;
    std::vector<GeomEntity*> m_moved_entities;
    std::map<GeomEntity*, GeomEntity*> m_ref_to_new_entities;
    std::map<GeomEntity*, std::vector<GeomEntity*> > m_replaced_entities;
    std::list<GeomEntity*> m_adj_entities;
    std::map<GeomEntity*,GeomEntity*> m_correspondance[4];
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDGEOMCOPY_H_ */
/*----------------------------------------------------------------------------*/
