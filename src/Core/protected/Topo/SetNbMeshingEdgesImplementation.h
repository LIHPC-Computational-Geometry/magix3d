/*----------------------------------------------------------------------------*/
/*
 * \file SetNbMeshingEdgesImplementation.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11/1/2019
 */
/*----------------------------------------------------------------------------*/
#ifndef SETNBMESHINGEDGESIMPLEMENTATION_H_
#define SETNBMESHINGEDGESIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include <map>
#include <vector>
#include <stdlib.h>	// CP : => uint
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class InfoCommand;
}
namespace Topo {
class CoEdge;
class CoFace;
/*----------------------------------------------------------------------------*/
/** \class SetNbMeshingEdgesImplementation
 *  \brief Implémentation pour la commande qui change la discrétisation (en nombre de bras) pour une arête
 *  et propage aux arêtes parallèles
 *  Certaines arêtes peuvent être figées
 *
 *  Le nombre de bras demandé peut être celui déjà présent, ce qui aura pour effet
 *  de propager une variation nulle mais qui pourra imposer un nombre de bras
 *  aux arêtes d'une face invalide
 */
/*----------------------------------------------------------------------------*/
class SetNbMeshingEdgesImplementation {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param icmd le conteneur pour les entités modifiées, créées...
     */
    SetNbMeshingEdgesImplementation(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief interdit de toucher à des arêtes
     *
     *  \param frozed_coedges les arêtes dont on ne doit pas modifier le nombre de bras
     */
    void addFrozen(std::vector<CoEdge*>& frozed_coedges);

    /*------------------------------------------------------------------------*/
    /** \brief spécifie le nombre de bras à ajouter pour une arête
     *
     *  \param ed l'arête dont on va changer la discrétisation
     *  \param nb le nombre de bras pour cette arête
      */
    void addDelta(CoEdge* ed, int nb);

    /*------------------------------------------------------------------------*/
    /** \brief marque la coface comme vue
     */
    void addTaggedCoFace(CoFace* coface);


    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~SetNbMeshingEdgesImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void execute();

protected:

    // structure pour le couple CoFace et CoEdge avec déséquilibre à propager
    struct CoFace_CoEdge {
        CoFace* m_coface;
        CoEdge* m_coedge;
    };

    /** Pour une arête donnée, parcours les arêtes voisines de proche en proche
     *  Pour les arêtes opposées qui font tout le côté de la coface entre, boucle avec
     *  Pour les autres arêtes, met de côté le couple coface coedge dans  m_coface_invalide
     *  pour un traitement ultérieur
     */
    void propagateWholeCoedges(CoEdge* coedge);

    /** Ajoute à la liste les arêtes parallèles "entières",
     * et met de côté celles correspondant à une arête composée (que l'on traitera après)
     */
    void addParalelCoEdges(CoEdge* coedge_dep, std::vector<CoEdge*>& whole_coedges);

    /** Recherche une arête non vue en face d'une arête donnée, sur une face commune donnée
     *  Elle est en face si du fait de la structuration les noeuds de cette arêtes en face
     *  correspondent à ceux de l'arête ou sont compris dedans
     */
    CoEdge* findOppositeCoEdge(CoEdge* coedge_dep, uint cote_dep, CoFace* coface);

    /** Calcul du déséquilibre sur un côté d'une face commune
     *  Les faces dégénérées n'ont pas de déséquilibre
     */
    int computeDelta(CoFace* coface, uint cote);

    /// retourne uniquement les cofaces structurées associées à une arête
    void getStructuredCoFaces(CoEdge* coedge, std::vector<CoFace* >& cofaces);

    /// modifie les nombres de bras des coedges en fonction du delta
    void applyDeltas();

private:

    Internal::InfoCommand* m_icmd;

    /// les arêtes pour démarrer les recherches
    std::vector<CoEdge*> m_coedges;

    /// la variation en nombre de bras par arête
    std::map<CoEdge*, int> m_coedge_delta;

    /// liste des faces communes avec déséquilibre entre 2 côtés
    std::vector<CoFace_CoEdge> m_coface_invalide;

    /// marque pour les arêtes vues
    std::map<CoEdge*, bool> m_coedges_vue;

    /// marque pour les cofaces vues
    std::map<CoFace*, bool> m_cofaces_vue;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* SETNBMESHINGEDGESIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
