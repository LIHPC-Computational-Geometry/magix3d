/*----------------------------------------------------------------------------*/
/*
 * \file CommandRotateTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/9/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDROTATETOPO_H_
#define COMMANDROTATETOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandTransformTopo.h"
#include "Utils/Rotation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandRotateTopo
 *  \brief Commande permettant de faire une rotation de la topologie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandRotateTopo: public Topo::CommandTransformTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ve la liste des entités topologiques sélectionnnées pour la rotation
     *  \param rot la rotation
     */
    CommandRotateTopo(Internal::Context& c,
            std::vector<TopoEntity*>& ve,
            const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd le constructeur des entités topologiques pour la rotation
     *  \param rot la rotation
     * \param all_topo vrai si on est dans un cas de copie + transformation de toute la topologie
     */
    CommandRotateTopo(Internal::Context& c,
    		CommandDuplicateTopo* cmd,
            const Utils::Math::Rotation& rot,
			bool all_topo);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour rotation de tout
     *
     *  \param c le contexte
     *  \param rot la rotation
     */
    CommandRotateTopo(Internal::Context& c,
			const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandRotateTopo();

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
    /// la rotation
    Utils::Math::Rotation m_rotation;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDROTATETOPO_H_ */
/*----------------------------------------------------------------------------*/
