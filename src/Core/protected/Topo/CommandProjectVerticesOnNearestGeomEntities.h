/*----------------------------------------------------------------------------*/
/** \file CommandProjectVerticesOnNearestGeomEntities.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/11/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDPROJECTVERTICESONNEARESTGEOMENTITIES_H_
#define COMMANDPROJECTVERTICESONNEARESTGEOMENTITIES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdge;
class TopoEntity;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class CommandProjectVerticesOnNearestGeomEntities
 *  \brief Commande permettant de projetter ces sommets topologiques vers l'entité
 *   géométrique la plus proche parmis celles passées en paramêtre
 */
/*----------------------------------------------------------------------------*/
class CommandProjectVerticesOnNearestGeomEntities: public Topo::CommandEditTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de sommets et d'entités géométriques
     *
     *  \param c le contexte
     *  \param vertices une liste de sommets topologiques
     *  \param geom_entities une liste d'entités géométriques
     *  \param move_vertices déplace ou non les sommets suivant la projection
     */
    CommandProjectVerticesOnNearestGeomEntities(Internal::Context& c,
    		std::vector<Vertex*> &vertices,
    		std::vector<Geom::GeomEntity*> & geom_entities,
    		bool move_vertices);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandProjectVerticesOnNearestGeomEntities();

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
    /** projection pour un sommet */
    void project(Vertex* vtx, Geom::GeomEntity* ge);

    /** les sommets à projeter */
    std::vector<Vertex*> m_vertices;

    /** ce sur quoi on projette les entités */
    std::vector<Geom::GeomEntity*> m_geom_entities;

    /** déplace ou non les sommets suivant la projections */
    bool m_move_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDPROJECTVERTICESONNEARESTGEOMENTITIES_H_ */
/*----------------------------------------------------------------------------*/

