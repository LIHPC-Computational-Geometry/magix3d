/*----------------------------------------------------------------------------*/
/*
 * CommandJoinEntities.h
 *
 *  Created le 23/6/2015
 *  	Par Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDJOINENTITIES_H_
#define COMMANDJOINENTITIES_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandJoinEntities
 *  \brief Commande permettant de réunir plusieurs entités en une seule
 */
/*----------------------------------------------------------------------------*/
class CommandJoinEntities: public CommandEditGeom
{
public:

    /*------------------------------------------------------------------------*/
       /** \brief  Constructeur
        *
        *  \param c le contexte
        *  \param e les entités à réunir
        */
    CommandJoinEntities(Internal::Context& c,
    		std::string titreCmd,
            std::vector<GeomEntity*>& e);

    /*------------------------------------------------------------------------*/
     /** \brief  Destructeur
      */
    virtual ~CommandJoinEntities();


    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    virtual void internalSpecificPreExecute();

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
     *          (toujours vide dans notre cas)
     */
    virtual std::vector<GeomEntity*>& getMovedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur toutes les entités  sur lesquelles
     *          l'algorithme travaille
     */
    virtual std::list<GeomEntity*>& getRefEntities(const int dim);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur une map indiquant par quelles
     *          entités une entité supprimée a été remplacée
     */
    virtual std::map<GeomEntity*,std::vector<GeomEntity*> >& getReplacedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  Initialise les attributs de cette classe dans les constructeurs
     *          des classes filles.
     */
    virtual void init(std::vector<GeomEntity*>& es);

    /// valide les paramètres
    virtual void validate()=0;

    /*------------------------------------------------------------------------*/
    /** \brief  initialise les entités de références
     *
     *  \param entity une entité
     */
    virtual void addReference(GeomEntity* entity);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute les entités filles de entity dans les entités de
     *          référence
     *
     *  \param entity une entité
     */
    virtual void addDownIncidentReference(GeomEntity* entity);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute les entités parentes de entity dans les entités de
     *          référence
     *
     *  \param entity une entité
     */
    virtual void addUpIncidentReference(GeomEntity* entity);


protected:

    /// entités à fusionner
    std::vector<GeomEntity*> m_entities;

    /// référence sur toutes les entités  sur lesquelles l'algorithme travaille
    std::list<GeomEntity*> m_ref_entities[4];

    /* entités que l'on a conservées (modifiées ou non)*/
    std::vector<Vertex*> m_toKeepVertices;
    std::vector<Curve*> m_toKeepCurves;
    std::vector<Surface*> m_toKeepSurfaces;
    std::vector<Volume*> m_toKeepVolumes;


    /** entités ajoutées */
    std::vector<GeomEntity*> m_newEntities;
    /** entités que l'on a supprimées */
    std::vector<GeomEntity*> m_removedEntities;
    /** entités que l'on a déplacées (toujours vide dans notre cas) */
    std::vector<GeomEntity*> m_movedEntities;
    /** entités que l'on a remplacées par une autre. Une entité remplacée est
     *  aussi supprimée (et donc apparait dans m_removedEntities)*/
    std::map<GeomEntity*,std::vector<GeomEntity*> > m_replacedEntities;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDJOINENTITIES_H_ */
/*----------------------------------------------------------------------------*/

