/*----------------------------------------------------------------------------*/
/*
 * \file CommandFacetedSurfaceOffset.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11 Décembre 2020
 *
 *  Commande qui ne permet pas cette implémentation de faire un undo
 *  Il faudrait pour cela mémoriser la position des noeuds
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_COMMANDFACETEDSURFACEOFFSET_H_
#define MGX3D_MESH_COMMANDFACETEDSURFACEOFFSET_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class CommandFacetedSurfaceOffset: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *  \param c le contexte
     *  \param surface l'entité à modifier
     *  \param offset la distance de la surface pour la translation des noeuds
     */
    CommandFacetedSurfaceOffset(Internal::Context& c,
                               Surface* surface,
                               const double& offset);
//public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFacetedSurfaceOffset();

	/*------------------------------------------------------------------------*/
	/** \brief  exécute la commande
	 */
	virtual void internalExecute();

	/*------------------------------------------------------------------------*/
	/** \brief  annule la commande
	 */
	void internalUndo();

	/** \brief  rejoue la commande
	 */
	void internalRedo();

protected:

private:

    /** surface (facétisée) à modifier */
    Surface* m_surface;

    /** offset pour le déplacement des noeuds (distance) */
    double m_offset;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_COMMANDFACETEDSURFACEOFFSET_H_ */
