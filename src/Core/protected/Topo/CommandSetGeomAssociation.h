/*----------------------------------------------------------------------------*/
/** \file CommandSetGeomAssociation.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/5/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSETGEOMASSOCIATION_H_
#define COMMANDSETGEOMASSOCIATION_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandCreateGeom;
class CommandEditGeom;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdge;
class TopoEntity;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class CommandSetGeomAssociation
 *  \brief Commande permettant de modifier le contour géométrique sur lequel est projeté une liste de TopoEntity
 */
/*----------------------------------------------------------------------------*/
class CommandSetGeomAssociation: public Topo::CommandEditTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste d'arêtes et une commande de création de géométrie
     *
     *  \param c le contexte
     *  \param topoentities une liste d'arêtes ou faces communes
     *  \param command_geom une commande de création d'entité géométrique
     */
    CommandSetGeomAssociation(Internal::Context& c, std::vector<TopoEntity*> &topoentities, Geom::CommandCreateGeom* command_geom);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une commande de d'édition de géométries (suppressions)
     *
     *  \param c le contexte
     *  \param command_geom une commande d'édition d'entités géométriques
     */
    CommandSetGeomAssociation(Internal::Context& c, Geom::CommandEditGeom* command_geom);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de TopoEntity et une entité géométrique
     *
     *  \param c le contexte
     *  \param topo_entities  une liste d'entités topologiques
     *  \param geom_entity  une entité géométrique (qui peut être nulle)
     *  \param move_vertices déplace ou non les sommets suivant la projection
     */
    CommandSetGeomAssociation(Internal::Context& c, std::vector<TopoEntity*> &topo_entities, Geom::GeomEntity* geom_entity, bool move_vertices);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSetGeomAssociation();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /*------------------------------------------------------------------------*/
    /** Met à jour l'association et c'est tout */
    void project(Block* bloc);

    /** Effectue la projection et propage aux entités liées à une face commune
     * dans le cas non nulle
     */
    void project(CoFace* coface);

    /** Effectue la projection et propage aux entités liées à une arête commune
     * dans le cas non nulle
     */
    void project(CoEdge* coedge);

    /** Effectue la projection  pour un sommet
     */
    void project(Vertex* vtx);

    /*------------------------------------------------------------------------*/
    /** Méthode de vérification et validation de l'entité présente */
    void validGeomEntity();

    /// vérifie qu'un même groupe n'est pas référencé depuis la géométrie et depuis la topologie
    void validGroupsName(TopoEntity* te);

    /*------------------------------------------------------------------------*/

    /** les entités à projeter */
    std::vector<TopoEntity*> m_topo_entities;

    /** ce sur quoi on projette les entités */
    Geom::GeomEntity* m_geom_entity;

    /** commande de création d'une entité géométrique */
    Geom::CommandCreateGeom* m_command_create_geom;

    /** commande d'édition d'entités géométriques (suppressions) */
    Geom::CommandEditGeom* m_command_edit_geom;

    /** déplace ou non les sommets suivant la projections */
    bool m_move_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSETGEOMASSOCIATION_H_ */
/*----------------------------------------------------------------------------*/

