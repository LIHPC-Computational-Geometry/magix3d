/*----------------------------------------------------------------------------*/
/*
 * \file CommandScaleMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSCALEMESH_H_
#define COMMANDSCALEMESH_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandTransformMesh.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandScaleMesh
 *  \brief Commande permettant de faire une homothétie de tout le maillage
 *
 */
/*----------------------------------------------------------------------------*/
class CommandScaleMesh: public CommandTransformMesh {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param facteur le facteur d'homothétie
     *  \param pcentre centre de l'homotéthie
     */
    CommandScaleMesh(Internal::Context& c,
    		const double& facteur,
            const Utils::Math::Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    CommandScaleMesh(Internal::Context& c,
    		const double factorX,
    		const double factorY,
    		const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandScaleMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo();

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
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSCALEMESH_H_ */
/*----------------------------------------------------------------------------*/
