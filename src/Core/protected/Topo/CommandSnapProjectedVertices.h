/*----------------------------------------------------------------------------*/
/*
 * \file CommandSnapProjectedVertices.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSNAPPROJECTEDVERTICES_H_
#define COMMANDSNAPPROJECTEDVERTICES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSnapProjectedVertices
 *  \brief Commande permettant de modifier la position d'un ensemble de sommets
 *  en les replaceant suivant leur projection sur la géométrie
 */
/*----------------------------------------------------------------------------*/
class CommandSnapProjectedVertices: public Topo::CommandEditTopo {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vertices les sommets topologiques concernées
     */
    CommandSnapProjectedVertices(Internal::Context& c,
            std::vector<Vertex*> & vertices);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour tous les sommets
     *
     *  \param c le contexte
     */
    CommandSnapProjectedVertices(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSnapProjectedVertices();

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
    /// liste des sommets topologiques concernés
    std::vector<Vertex*> m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSNAPPROJECTEDVERTICES_H_ */
/*----------------------------------------------------------------------------*/
