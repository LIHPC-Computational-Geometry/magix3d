/*----------------------------------------------------------------------------*/
/** \file CommandCreateGeom.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCREATEGEOM_H_
#define COMMANDCREATEGEOM_H_
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/CommandInternal.h"
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
/** \class CommandCreateGeom
 *  \brief Partie commune à toutes les commandes de création et de modification
 *         d'entités géométriques
 */
/*----------------------------------------------------------------------------*/
class CommandCreateGeom: public Internal::CommandInternal {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte (permet l'accès au manager)
     * \param le nom de la commande
     */
    CommandCreateGeom(Internal::Context& c, std::string name, const std::string& groupName="");

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du volume
     */
    virtual ~CommandCreateGeom();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute()=0;

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    virtual void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    virtual void internalRedo();

    /*------------------------------------------------------------------------*/
    /** \brief  récupère le résultat dans res
     */
    virtual void getResult(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  stockage dans le manager géométrique
     *
     *          Attention pour tous les stockages, on ne vérifie pas que l'
     *          élément n'est pas déjà présent et stocké dans le manager
     */
    virtual void store(Vertex* e);
    virtual void store(Curve* e);
    virtual void store(Surface* e);
    virtual void store(Volume* e);
    virtual void store(GeomEntity* e);
    virtual void store(std::vector<GeomEntity*>& e);

    /*------------------------------------------------------------------------*/
    /** \brief  découpage du volume en entité de dimension inférieure
     *  et stockage dans le manager géométrique
     */
    virtual void split(Volume* v);

    /*------------------------------------------------------------------------*/
    /** \brief  découpage de la surface en entité de dimension inférieure
     *  et stockage dans le manager géométrique
     */
    virtual void split(Surface* s);

    /*------------------------------------------------------------------------*/
    /** \brief  découpage de la courbe en entité de dimension inférieure
     *  et stockage dans le manager géométrique
     */
    virtual void split(Curve* c);

    /*------------------------------------------------------------------------*/
    /** \brief  stockage dans le manager géométrique
     */
    virtual void split(Vertex* v);
    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le manager géométrique
     */
    virtual GeomManager& getGeomManager();

    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le nom du groupe
     */
    virtual std::string getGroupName() const {return m_group_name;}

    /*------------------------------------------------------------------------*/
    /** \brief  ajoute à un groupe un volume
     */
    virtual void addToGroup(Volume* v, bool use_default_name = false);

    /** \brief  ajoute à un groupe une surface
     */
    virtual void addToGroup(Surface* s, bool use_default_name = false);

    /** \brief  ajoute à un groupe une courbe
     */
    virtual void addToGroup(Curve* c, bool use_default_name = false);

    /** \brief  ajoute à un groupe un sommet
     */
    virtual void addToGroup(Vertex* v, bool use_default_name = false);

    /** \brief  ajoute à un groupe d'une entité géométrique
     */
    virtual void addToGroup(GeomEntity* e, bool use_default_name = false);

    /// change la dimension des entités sur lesquels on travaille
    virtual void setDimensionGroup(uint dim)
    {m_dim_new_group = dim;}

    /// détermine la dimension des entités sur lesquelles on travaille (celle de dim la plus grande)
    virtual void updateDimensionGroup(std::vector<GeomEntity*>& entities);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des nouveaux sommets géométriques
     *
     *  /see getPreviewRepresentation
     */
    virtual void getPreviewRepresentationNewVertices(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation filaire des nouveaux objets géométriques
     *
     *  /see getPreviewRepresentation
     */
    virtual void getPreviewRepresentationNewObjects(Utils::DisplayRepresentation& dr);

    /** Ce qui est fait avant la commande
     */
    virtual void preExecute();

   /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);


protected:

    /** resultat de l'opération */
    std::vector<GeomEntity*> m_createdEntities;

    /// le nom d'un groupe dans lequel on met les nouvelles entités (peut être vide)
    std::string m_group_name;

    /// dimension des nouvelles entités à mettre dans le groupe
    uint m_dim_new_group;

    /// mode preview activé
    bool m_isPreview;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATEGEOM_H_ */
/*----------------------------------------------------------------------------*/

