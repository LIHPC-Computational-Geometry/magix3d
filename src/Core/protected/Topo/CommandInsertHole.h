/*----------------------------------------------------------------------------*/
/*
 * \file CommandInsertHole.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 16/1/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDINSERTHOLE_H_
#define COMMANDINSERTHOLE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandInsertHole
 *  \brief Commande qui duplique les faces topologie pour créer un trou dans la toplogie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandInsertHole: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cofaces le vecteur de faces topologiques concernés
     */
    CommandInsertHole(Internal::Context& c,
            std::vector<CoFace*>& cofaces);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandInsertHole();

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

private:
    /// duplique certaines entités topologiques
    void duplicate();

    /// duplique une face commune
    CoFace* duplicate(CoFace* cf);

    /// duplique une arête
    Edge* duplicate(Edge* ed);

    /// duplique une arête sans dupliquer les entités de niveau inférieur
    void separate(Edge* ed);

    /// duplique une arête commune
    CoEdge* duplicate(CoEdge* co);

    /// duplique un sommet
    Vertex* duplicate(Vertex* ve);

    /// destruction des données temporaires
    void clearMemory();

    /// recherche le bloc (de plus petit id) dont la marque vaut mark, return 0 s'il n'y en a pas
    Block* getBlock(uint mark);
    Block* getBlock(std::vector<Block*>& blocs, uint mark);

    /// recherche d'un groupe de blocs connectés par une coface non marquée
    std::vector<Block*> getConnectedBlocks();

    /// retourne vrai s'il n'y a une liaison via les blocs et cofaces de l'arête en s'interdisant la coface
    bool findCoFace_Block_relation(CoEdge* coedge, CoFace* coface);

    /// translation du sommet en direction du barycentre moyen des blocs en relation
    void translate(Vertex* vtx);

private:

    /// les entités topologiques concernées
    std::vector<CoFace*> m_cofaces;

    // les maps de correspondance entre les entités pour éviter de dupliquer plusieurs fois
    std::map<Vertex*, Vertex*> m_corr_vertex;
    std::map<CoEdge*, CoEdge*> m_corr_coedge;
    std::map<Edge*, Edge*> m_corr_edge;
    std::map<CoFace*, CoFace*> m_corr_coface;
    std::map<Face*, Face*> m_corr_face;
    std::map<Block*, Block*> m_corr_block;

    // les filtres sur les entités
    std::map<Block*, uint> m_filtre_block;
    std::map<CoFace*, uint> m_filtre_coface;
    std::map<CoEdge*, uint> m_filtre_coedge;
    std::map<Vertex*, uint> m_filtre_vertex;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDINSERTHOLE_H_ */
/*----------------------------------------------------------------------------*/
