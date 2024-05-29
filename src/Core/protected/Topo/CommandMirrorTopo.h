/*----------------------------------------------------------------------------*/
/*
 * \file CommandMirrorTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 12/4/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDMIRRORTOPO_H_
#define COMMANDMIRRORTOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandTransformTopo.h"
#include "Utils/Plane.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandMirrorTopo
 *  \brief Commande permettant de faire une symétrie de la topologie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandMirrorTopo: public Topo::CommandTransformTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ve la liste des entités topologiques sélectionnnées pour la symétrie
      *  \param plane le plan de symétrie
     */
    CommandMirrorTopo(Internal::Context& c,
    		std::vector<TopoEntity*>& ve,
			Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd le constructeur des entités topologiques pour la symétrie
     *  \param plane le plan de symétrie
     * \param all_topo vrai si on est dans un cas de copie + transformation de toute la topologie
     */
    CommandMirrorTopo(Internal::Context& c,
    		CommandDuplicateTopo* cmd,
			Utils::Math::Plane* plane,
			bool all_topo);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
      *  \param plane le plan de symétrie
     */
    CommandMirrorTopo(Internal::Context& c,
			Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandMirrorTopo();

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
    /** Plan de symétrie */
    Utils::Math::Plane* m_plane;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDMIRRORTOPO_H_ */
/*----------------------------------------------------------------------------*/
