/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetNbMeshingEdges.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/11/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSETNBMESHINGEDGES_H_
#define COMMANDSETNBMESHINGEDGES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdge;
class CoFace;
/*----------------------------------------------------------------------------*/
/** \class CommandSetNbMeshingEdges
 *  \brief Commande qui change la discrétisation (en nombre de bras) pour une arête
 *  et propage aux arêtes parallèles
 *  Certaines arêtes peuvent être figées
 *
 *  Le nombre de bras demandé peut être celui déjà présent, ce qui aura pour effet
 *  de propager une variation nulle mais qui pourra imposer un nombre de bras
 *  aux arêtes d'une face invalide
 */
/*----------------------------------------------------------------------------*/
class CommandSetNbMeshingEdges: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ed l'arête dont on va changer la discrétisation
     *  \param nb le nombre de bras pour cette arête
     *  \param frozed_coedges les arêtes dont on ne doit pas modifier le nombre de bras
     */
    CommandSetNbMeshingEdges(Internal::Context& c, CoEdge* ed, int nb, std::vector<CoEdge*>& frozed_coedges);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSetNbMeshingEdges();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);


private:
    /// l'arête concernée
    CoEdge* m_coedge;

    /// le nombre de bras demandé
    int m_nb_meshing_edges;

    /// la liste des arêtes dites figées
    std::vector<CoEdge*> m_frozed_coedges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSETNBMESHINGEDGES_H_ */
/*----------------------------------------------------------------------------*/
