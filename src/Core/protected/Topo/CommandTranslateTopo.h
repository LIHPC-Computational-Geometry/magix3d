/*----------------------------------------------------------------------------*/
/*
 * \file CommandTranslateTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/9/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDTRANSLATETOPO_H_
#define COMMANDTRANSLATETOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandTransformTopo.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandTranslateTopo
 *  \brief Commande permettant de déplacer la topologie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandTranslateTopo: public Topo::CommandTransformTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ve la liste des entités topologiques sélectionnnées pour la translation
     *  \param dp le vecteur de translation
     */
    CommandTranslateTopo(Internal::Context& c, std::vector<TopoEntity*>& ve, const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd le constructeur des entités topologiques pour la translation
     *  \param dp le vecteur de translation
     * \param all_topo vrai si on est dans un cas de copie + transformation de toute la topologie
     */
    CommandTranslateTopo(Internal::Context& c,
    		CommandDuplicateTopo* cmd,
    		const Vector& dp,
			bool all_topo);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param dp le vecteur de translation
     */
    CommandTranslateTopo(Internal::Context& c, const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandTranslateTopo();

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
    /// le vecteur de translation
    Utils::Math::Vector m_vector;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDTRANSLATETOPO_H_ */
/*----------------------------------------------------------------------------*/
