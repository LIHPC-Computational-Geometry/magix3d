/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeVertexSameLocation.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/11/2017
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCHANGEVERTEXSAMELOCATION_H_
#define COMMANDCHANGEVERTEXSAMELOCATION_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandChangeVertexSameLocation
 *  \brief Commande permettant de modifier la position d'un sommet pour la rendre identique à celle d'un autre
 */
/*----------------------------------------------------------------------------*/
class CommandChangeVertexSameLocation: public Topo::CommandEditTopo {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vertex le sommet modifié
     *  \param target le sommet cible
     */
    CommandChangeVertexSameLocation(Internal::Context& c,
    		Vertex* vtx,
			Vertex* target);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandChangeVertexSameLocation();

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
    /// le sommet topologique concerné à modifier
    Vertex* m_vertex;

    /// le sommet topologique cible
    Vertex* m_target;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCHANGEVERTEXSAMELOCATION_H_ */
/*----------------------------------------------------------------------------*/
