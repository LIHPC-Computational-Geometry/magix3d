/*----------------------------------------------------------------------------*/
/*
 * CommandNewCurveByCurveProjectionOnSurface.h
 *
 *  Created on: 9/3/2015
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWCURVEBYCURVEPROJECTIONONSURFACE_H_
#define COMMANDNEWCURVEBYCURVEPROJECTIONONSURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewCurveByCurveProjectionOnSurface
 *  \brief Commande permettant de créer un cercle
 */
/*----------------------------------------------------------------------------*/
class CommandNewCurveByCurveProjectionOnSurface: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param curve la courbe projetée
     *  \param surface celle sur laquelle se fait la projection
     *  \param groupName un nom de groupe (qui peut être vide)
     */
	CommandNewCurveByCurveProjectionOnSurface(Internal::Context& c,
            Geom::Curve* curve,
            Geom::Surface* surface,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewCurveByCurveProjectionOnSurface();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /* parametres de l'operation */
    Geom::Curve*   m_curve;
    Geom::Surface* m_surface;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWCURVEBYCURVEPROJECTIONONSURFACE_H_ */
/*----------------------------------------------------------------------------*/

