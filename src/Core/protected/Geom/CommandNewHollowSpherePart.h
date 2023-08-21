/*----------------------------------------------------------------------------*/
/*
 * CommandNewHollowSpherePart.h
 *
 *  Created on: 29 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWHOLLOWSPHEREPART_H_
#define COMMANDNEWHOLLOWSPHEREPART_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Internal/Context.h"
#include "Geom/Volume.h"
#include "Utils/Constants.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewHollowSpherePart
 *  \brief Commande permettant de créer une partie de sphère du type aiguille
 */
/*----------------------------------------------------------------------------*/
class CommandNewHollowSpherePart: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param  c le contexte
     *  \param dr_int le rayon interne de l'aiguille creuse
     *  \param dr_ext le rayon externe de l'aiguille creuse
     *  \param  angleY   ouverture en angle dans le plan Oxy
     *  \param  angleZ   ouverture en angle dans le plan Oxz
     *  \param  groupName le nom du groupe
     */
	CommandNewHollowSpherePart(Internal::Context& c,
			const double& dr_int,
			const double& dr_ext,
			const double& angleY,
			const double& angleZ,
			const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewHollowSpherePart();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

protected:

    /// valide les paramètres
    void validate();

    /* parametres de l'operation */
    double m_dr_int;
    double m_dr_ext;
    double m_angleY;
    double m_angleZ;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWHOLLOWSPHEREPART_H_ */
/*----------------------------------------------------------------------------*/

