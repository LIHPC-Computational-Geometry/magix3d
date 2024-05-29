/*----------------------------------------------------------------------------*/
/*
 * \file CommandDuplicateTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/6/2013
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDDUPLICATETOPO_H_
#define COMMANDDUPLICATETOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom{
class CommandGeomCopy;
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandDuplicateTopo
 *  \brief Commande qui duplique la topologie avec une mise à jour de la géométrie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandDuplicateTopo: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param geom_cmd la commande de copie géométrique avec les modifications
     */
    CommandDuplicateTopo(Internal::Context& c,
            Geom::CommandGeomCopy* geom_cmd);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vb le vecteur de blocs topologiques concernés
     *  \param ng le nom du groupe dans lequel sont mis les blocs
     */
    CommandDuplicateTopo(Internal::Context& c,
            std::vector<Block*>& vb,
    		const std::string& ng);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vb le vecteur de blocs topologiques concernés
     *  \param vol le volume géométrique sur lequel on reporte l'association des blocs
     */
    CommandDuplicateTopo(Internal::Context& c,
            std::vector<Block*>& vb,
            Geom::Volume* vol);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandDuplicateTopo();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

    /// retourne la liste des blocs créés
    virtual void get(std::vector<Topo::Block*>& blocs);

    /// retourne la liste des faces communes créées
    virtual void get(std::vector<Topo::CoFace*>& cofaces);

private:
    /// duplique toutes les entités topologiques sélectionnées et les entités de niveau inférieur
    void duplicate();

    /// duplique un bloc
    Block* duplicate(Block* bl);

    /// duplique une face
    Face* duplicate(Face* fa);

    /// duplique une face commune
    CoFace* duplicate(CoFace* cf);

    /// duplique une arête
    Edge* duplicate(Edge* ed);

    /// duplique une arête commune
    CoEdge* duplicate(CoEdge* co);

    /// duplique un sommet
    Vertex* duplicate(Vertex* ve);

    /// met à jour la relation vers la géométrie en fonction de la correspondance obtenue dans m_geomModif
    void MAJRelation(Geom::GeomEntity* ge, TopoEntity* te);

    /// met à jour les interpolations
    void updateInterpolate();

    /// copie des groupes propres à la topologie
    void updateGroups();

private:

    /// les entités topologiques concernées
    std::vector<TopoEntity*> m_entities;

    /// la commande de copie qui permet d'accéder aux copies géométriques
    Geom::CommandGeomCopy* m_geomCmd;

    /// le volume géométrique (pour associer les blocs)
    Geom::Volume* m_volume;

    /// nom du groupe dans lequel sont mis les blocs (cas sans volume)
    std::string m_groupName;

    // les maps de correspondance entre les entités pour éviter de dupliquer plusieurs fois
    std::map<Vertex*, Vertex*> m_corr_vertex;
    std::map<CoEdge*, CoEdge*> m_corr_coedge;
    std::map<Edge*, Edge*> m_corr_edge;
    std::map<CoFace*, CoFace*> m_corr_coface;
    std::map<Face*, Face*> m_corr_face;
    std::map<Block*, Block*> m_corr_block;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDDUPLICATETOPO_H_ */
/*----------------------------------------------------------------------------*/
