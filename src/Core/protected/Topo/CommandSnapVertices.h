/*----------------------------------------------------------------------------*/
/*
 * \file CommandSnapVertices.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/9/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSNAPVERTICES_H_
#define COMMANDSNAPVERTICES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSnapVertices
 *  \brief Fusionne 2 à 2 des sommets
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSnapVertices: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vertices1 le premier groupe de sommets
     *  \param vertices2 un deuxième groupe de sommets
     *  \param project_on_first vrai si l'on projette les sommets du deuxième groupe sur ceux du premier,
     *  au milieu sinon
     */
    CommandSnapVertices(Internal::Context& c,
            std::vector<Topo::Vertex* > &vertices1,
            std::vector<Topo::Vertex* > &vertices2,
            bool project_on_first);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSnapVertices();

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
    /** return true si le 2ème sommet est associé à une entité identique à celle du 1er
     * ou si elle est reliée à cette entité
     */
    bool areAssociationOK(Vertex* som1, Vertex* som2);

    /// les premiers sommets (conservés)
    std::vector<Vertex* > m_vertices1;

    /// le deuxième groupe de sommets
    std::vector<Vertex* > m_vertices2;

    /// paramètre pour spécifier où seront placés les sommets après la fusion
    bool m_project_on_first;


    /// un bloc dans lequel apparaissent les sommets concernés
    Block* m_common_block;

    /// une face dans laquelle apparaissent les sommets concernés
    CoFace* m_common_coface;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSNAPVERTICES_H_ */
/*----------------------------------------------------------------------------*/
