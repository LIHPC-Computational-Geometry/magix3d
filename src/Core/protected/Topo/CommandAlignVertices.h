/*----------------------------------------------------------------------------*/
/*
 * \file CommandAlignVertices.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDALIGNVERTICES_H_
#define COMMANDALIGNVERTICES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandAlignVertices
 *  \brief Commande permettant de modifier la position d'un ensemble de sommets
 *
 *  soit en les replacent sur le segment entre les deux sommets
 *  aux extrémités de la sélection
 *  soit en les replacent sur la droite définie par 2 points
 */
/*----------------------------------------------------------------------------*/
class CommandAlignVertices: public Topo::CommandEditTopo {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vertices les sommets topologiques concernées
     */
    CommandAlignVertices(Internal::Context& c,
            std::vector<Vertex*> & vertices);

    /*  \param c le contexte
     * \param p1 une extrémité de la droite
     * \param p2 l'autre extrémité de la droite
     *  \param vertices les sommets topologiques concernées
     */
    CommandAlignVertices(Internal::Context& c,
    		const Utils::Math::Point& p1, const Utils::Math::Point& p2,
			std::vector<Vertex*> & vertices);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandAlignVertices();

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
    /// liste des sommets topologiques concernés (qui bougent)
    std::vector<Vertex*> m_vertices;

    /// un point à une extrémité de la droite
    Utils::Math::Point m_p1;
    /// un point à l'autre extrémité de la droite
    Utils::Math::Point m_p2;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDALIGNVERTICES_H_ */
/*----------------------------------------------------------------------------*/
