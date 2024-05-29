/*----------------------------------------------------------------------------*/
/*
 * \file CommandGlue2Topo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/12/14
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDGLUE2TOPO_H_
#define COMMANDGLUE2TOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom{
class Volume;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandGlue2Topo
 *  \brief Commande permettant de fusionner les faces de 2 topologies
 *  prises dans 2 volumes géométriques pour les unir
 *
 *  Cette commande ne fait le collage que pour les sommets topologiques avec
 *  des coordonnées identiques à epsilon près.
 */
/*----------------------------------------------------------------------------*/
class CommandGlue2Topo: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vol_A un volume géométrique dont les faces communes seront concervées
     *  \param vol_B un deuxième volumes dont les faces seront remplacées par celles de vol_A
     */
    CommandGlue2Topo(Internal::Context& c, Geom::Volume* vol_A, Geom::Volume* vol_B);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandGlue2Topo();

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
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// volume dont la topologie ne devrait pas changer
    Geom::Volume* m_vol_A;
    /// volume dont la topologie sera collée
    Geom::Volume* m_vol_B;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDGLUE2TOPO_H_ */
/*----------------------------------------------------------------------------*/
