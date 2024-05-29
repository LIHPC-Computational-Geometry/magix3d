/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeVerticesLocation.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCHANGEVERTICESLOCATION_H_
#define COMMANDCHANGEVERTICESLOCATION_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandChangeVerticesLocation
 *  \brief Commande permettant de modifier la position d'un ensemble de sommets
 */
/*----------------------------------------------------------------------------*/
class CommandChangeVerticesLocation: public Topo::CommandEditTopo {

public:

	enum coordinateType {cartesian, spherical, cylindrical};

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vertices les sommets topologiques concernées
     * \param changeX vrai si la coordonnée en X est à modifier
     * \param xPos la nouvelle valeur pour la coordonnées en X
     * \param changeY vrai si la coordonnée en Y est à modifier
     * \param yPos la nouvelle valeur pour la coordonnées en Y
     * \param changeZ vrai si la coordonnée en Z est à modifier
     * \param zPos la nouvelle valeur pour la coordonnées en Z
     * \param typeCoord type de coordonnées (cartésiennes, sphérique ...)
     * \param rep le repère dans lequel se fait la modification (peut-être nul)
     */
    CommandChangeVerticesLocation(Internal::Context& c,
            std::vector<Vertex*> & vertices,
            const bool changeX,
            const double& xPos,
            const bool changeY,
            const double& yPos,
            const bool changeZ,
            const double& zPos,
			const coordinateType typeCoord,
			CoordinateSystem::SysCoord* rep);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandChangeVerticesLocation();

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
    /// liste des sommets topologiques concernés
    std::vector<Vertex*> m_vertices;

    /// type de coordonnées utilisées
    coordinateType m_typeCoord;

    /// vrai si l'on doit modifier les X
    bool m_changeX;
    /// la nouvelle valeur pour X
    double m_xPos;

    /// vrai si l'on doit modifier les Y
    bool m_changeY;
    /// la nouvelle valeur pour Y
    double m_yPos;

    /// vrai si l'on doit modifier les Z
    bool m_changeZ;
    /// la nouvelle valeur pour Z
    double m_zPos;

    /// repère dans lequel on fait les opérations
    CoordinateSystem::SysCoord* m_rep;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCHANGEVERTICESLOCATION_H_ */
/*----------------------------------------------------------------------------*/
