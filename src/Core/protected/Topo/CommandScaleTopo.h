/*----------------------------------------------------------------------------*/
/*
 * \file CommandScaleTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/9/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSCALETOPO_H_
#define COMMANDSCALETOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandTransformTopo.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandScaleTopo
 *  \brief Commande permettant de faire une homothétie de la topologie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandScaleTopo: public Topo::CommandTransformTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ve la liste des entités topologiques sélectionnnées pour l'homotéthie
     *  \param facteur le facteur d'homothétie
     *  \param pcentre centre de l'homotéthie
     */
    CommandScaleTopo(Internal::Context& c,
    		std::vector<TopoEntity*>& ve,
    		const double& facteur,
            const Utils::Math::Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd le constructeur des entités topologiques pour l'homotéthie
     *  \param facteur le facteur d'homothétie
     *  \param pcentre centre de l'homotéthie
     * \param all_topo vrai si on est dans un cas de copie + transformation de toute la topologie
     */
    CommandScaleTopo(Internal::Context& c,
    		CommandDuplicateTopo* cmd,
    		const double& facteur,
            const Utils::Math::Point& pcentre,
			bool all_topo);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param facteur le facteur d'homothétie
     *  \param pcentre centre de l'homotéthie
     */
    CommandScaleTopo(Internal::Context& c,
    		const double& facteur,
            const Utils::Math::Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ve la liste des entités topologiques sélectionnnées pour l'homotéthie
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    CommandScaleTopo(Internal::Context& c,
    		std::vector<TopoEntity*>& ve,
            const double factorX,
            const double factorY,
            const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd le constructeur des entités topologiques pour l'homotéthie
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     *  \param all_topo vrai si on est dans un cas de copie + transformation de toute la topologie
     */
    CommandScaleTopo(Internal::Context& c,
    		CommandDuplicateTopo* cmd,
            const double factorX,
            const double factorY,
            const double factorZ,
			bool all_topo);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    CommandScaleTopo(Internal::Context& c,
            const double factorX,
            const double factorY,
            const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandScaleTopo();

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
    /// le facteur d'homotéthie
    double m_factor;

    /// vrai si l'on utilise un facteur unique pour toutes les directions
    bool m_isHomogene;
    /** facteur d'homothétie suivant X */
    double m_factorX;
    /** facteur d'homothétie suivant Y */
    double m_factorY;
    /** facteur d'homothétie suivant Z */
    double m_factorZ;

    /// le centre de l'homotéthie
    Utils::Math::Point m_center;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSCALETOPO_H_ */
/*----------------------------------------------------------------------------*/
