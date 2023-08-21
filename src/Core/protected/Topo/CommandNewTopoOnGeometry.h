/*----------------------------------------------------------------------------*/
/** \file CommandNewTopoOnGeometry.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 07/03/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWTOPOONGEOMETRY_H_
#define COMMANDNEWTOPOONGEOMETRY_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandCreateGeom;
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandNewTopoOnGeometry
 *  \brief Commande permettant de créer une topologie pour une géométrie quelconque
 *  avec réutilisation de la topologie existante
 *  Fonctionne également sans géométrie, mais avec nom de groupe
 */
/*----------------------------------------------------------------------------*/
class CommandNewTopoOnGeometry: public Topo::CommandCreateTopo{

public:

	/** type de topologie demandée, non structuré, structuré avec projection,
	 * un bloc sans projection, ou un bloc pour l'insertion
	 */
	enum eTopoType { UNSTRUCTURED_BLOCK, STRUCTURED_BLOCK, FREE_BLOCK, INSERTION_BLOCK, ASSOCIATED_TOPO };

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param entity un Volume ou une Surface
     *  \param topoType un énuméré sur le type de topologie souhaitée
     */
    CommandNewTopoOnGeometry(Internal::Context& c, Geom::GeomEntity* entity, eTopoType topoType);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ng le nom du groupe
     *  \param dim la dimension 2 pour face, 3 pour bloc
     */
    CommandNewTopoOnGeometry(Internal::Context& c, const std::string& ng, const short& dim);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param command_geom une commande de création d'entité géométrique
     *  \param topoType un énuméré sur le type de topologie souhaitée
     */
    CommandNewTopoOnGeometry(Internal::Context& c, Geom::CommandCreateGeom* command_geom, eTopoType topoType);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour cas structuré avec ni, nj et nk donnés
     *
     *  \param c le contexte
     *  \param command_geom une commande de création d'entité géométrique
     *  \param ni le nombre de bras suivant la 1ère direction
     *  \param nj idem 2ème
     *  \param nk idem 3ème
     */
    CommandNewTopoOnGeometry(Internal::Context& c, Geom::CommandCreateGeom* command_geom, const int ni, const int nj, const int nk);

    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param entity une Surface
     *  \param vertices la liste des sommets qui délimitent la structuration.
     */
    CommandNewTopoOnGeometry(Internal::Context& c, Geom::Surface* entity, std::vector<Geom::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewTopoOnGeometry();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// Création du bloc topologique pour une aiguille pleine
    void createSpherePartBlock();

    /// Création du bloc topologique pour une aiguille creuse
    void createHollowSpherePartBlock();

    /** Méthode de vérification et validation de l'entité présente */
    virtual void validGeomEntity();

    /** Création d'un sommet topologique */
    virtual void createVertex();

    /** Création d'une arête topologique */
    virtual void createCoEdge();

private:

    /// cas d'un bloc seul (sans projection)
    bool m_freeTopo;

    /// dimension de l'entité topo libre
    short m_dim;

    /// cas d'un bloc pour l'insertion
    bool m_insertionBlock;

    /// nom du groupe pour cas d'un bloc seul (sans projection)
    std::string m_groupName;

    /// nombre de bras pour la première direction dans le cas structuré
    uint m_ni;
    /// nombre de bras pour la deuxième direction dans le cas structuré
    uint m_nj;
    /// nombre de bras pour la troisième direction dans le cas structuré
    uint m_nk;

    /// Les sommets extrémités qui déterminent la structuration pour un face d'une surface
    std::vector<Geom::Vertex*> m_extrem_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWTOPOONGEOMETRY_H_ */
/*----------------------------------------------------------------------------*/

