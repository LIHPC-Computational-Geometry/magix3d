/*----------------------------------------------------------------------------*/
/*
 * CommandNewVertexByCurveParameterization.h
 *
 *  Created on: 18 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWVERTEXBYCURVEPARAMETERIZATION_H_
#define COMMANDNEWVERTEXBYCURVEPARAMETERIZATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewVertexByCurveParameterization
 *  \brief Commande permettant de créer une sommet comme le point d'une courve
 *         d'un certain paramètre
 */
/*----------------------------------------------------------------------------*/
class CommandNewVertexByCurveParameterization: public Geom::CommandCreateGeom{

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param curv la courbe
     *  \param p le paramètre compris entre 0 et 1 pour creer un point sur
     *         curv
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewVertexByCurveParameterization(Internal::Context& c,
            Geom::Curve* curv,
            const double param,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */

    virtual ~CommandNewVertexByCurveParameterization();

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

protected:
    /* parametres de l'operation */
    Geom::Curve* m_c;
    double m_param;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWVERTEXBYCURVEPARAMETERIZATION_H_ */
/*----------------------------------------------------------------------------*/


