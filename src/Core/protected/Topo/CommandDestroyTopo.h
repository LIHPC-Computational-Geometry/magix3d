/*----------------------------------------------------------------------------*/
/** \file CommandDestroyTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 31/5/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMAND_DESTROY_TOPO_H_
#define COMMAND_DESTROY_TOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandEditGeom;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class TopoEntity;
class Block;
class Face;
class CoFace;
class Edge;
class CoEdge;
class Vertex;
/*----------------------------------------------------------------------------*/
/** \class CommandDestroyTopo
 *  \brief Commande permettant de detruire une entité
 *  et optionnellement les entités qui en dépendent
 */
/*----------------------------------------------------------------------------*/
class CommandDestroyTopo: public Topo::CommandEditTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param topos les entités topologiques à détruire
     *  \param propagate l'option de destruction ou non des entités dépendantes
     */
    CommandDestroyTopo(Internal::Context& c, std::vector<Topo::TopoEntity*> topos, bool propagate);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param topos les entités topologiques à détruire
     *  \param propagate l'option de destruction ou non des entités dépendantes
     */
    CommandDestroyTopo(Internal::Context& c, Geom::CommandEditGeom* command_geom, bool propagate);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandDestroyTopo();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités détruites
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// Détruit un bloc, propage si nécessaire
    void destroy(Block* );
    void destroy(CoFace* );
    void destroy(CoEdge* );
    void destroy(Vertex* );

    /// marque le bloc est ses faces, propage si nécessaire
    void mark(Block* bloc);
    void mark(Face* face);
    void mark(CoFace* coface);
    void mark(Edge* edge);
    void mark(CoEdge* coedge);
    void mark(Vertex* vertex);

    /// retourne vrai si la face est reliée à un bloc non marqué
    bool dependUnmarqued(Face* face);

    /// retourne vrai si la coface est reliée à une face non marquée
    bool dependUnmarqued(CoFace* coface);

    /// retourne vrai si la edge est reliée à une coface non marquée
    bool dependUnmarqued(Edge* edge);

    /// retourne vrai si la coedge est reliée à une edge non marquée
    bool dependUnmarqued(CoEdge* coedge);

    /// retourne vrai si le vertex est relié à une coedge non marquée
    bool dependUnmarqued(Vertex* vertex);

    /// détruit les entités qui ne dépendent pas d'une entité non marquée
    void destroyAllFreeEntities();

    /// vérification des liaisons pour les arêtes avec méthode interpolée
    void checkInterpolates();

    /// récupération si nécessaire de la liste des entités topologiques à partir de la liste des entités géométriques détruites
    void findTopoEntities();

private:
    /** Les entités topologiques à détruire */
    std::vector<Topo::TopoEntity*> m_topo_entities;

    /** option de destruction ou non des entités dépendantes */
    bool m_propagate;

    /** commande d'édition d'entités géométriques (suppressions) */
    Geom::CommandEditGeom* m_command_edit_geom;

    /// filtres pour identifier les entités touchées par la destruction
	std::set<Block*> m_filtre_blocks;
	std::set<Face*> m_filtre_faces;
	std::set<CoFace*> m_filtre_cofaces;
	std::set<Edge*> m_filtre_edges;
	std::set<CoEdge*> m_filtre_coedges;
	std::set<Vertex*> m_filtre_vertices;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMAND_DESTROY_TOPO_H_ */
/*----------------------------------------------------------------------------*/

