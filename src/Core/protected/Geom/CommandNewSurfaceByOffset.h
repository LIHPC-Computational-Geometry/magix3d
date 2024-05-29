/*
 * CommandNewSurfaceByOffset.h
 *
 *  Created on: 19/09/2019
 *      Author: Eric B
 */

#ifndef COMMAND_NEW_SURFACE_BY_OFFSET_H_
#define COMMAND_NEW_SURFACE_BY_OFFSET_H_

/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewSurfaceByOffset
 *  \brief Commande permettant de créer une boite
 */
/*----------------------------------------------------------------------------*/
class CommandNewSurfaceByOffset: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param base l'entité à copier
     *  \param offset la distance de la base pour la construction
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewSurfaceByOffset(Internal::Context& c,
    		Surface* base,
    		const double& offset,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewSurfaceByOffset();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:

    /* entités à copier */
    Surface* m_entity;

    /** offset pour la copie (distance) */
    double m_offset;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMAND_NEW_SURFACE_BY_OFFSET_H_ */
/*----------------------------------------------------------------------------*/
