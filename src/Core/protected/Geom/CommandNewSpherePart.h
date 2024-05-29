/*----------------------------------------------------------------------------*/
/*
 * CommandNewSpherePart.h
 *
 *  Created on: 21 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWSPHEREPART_H_
#define COMMANDNEWSPHEREPART_H_
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
/** \class CommandNewSpherePart
 *  \brief Commande permettant de créer une partie de sphère du type aiguille
 */
/*----------------------------------------------------------------------------*/
class CommandNewSpherePart: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param  c le contexte
     *  \param  dr  Le rayon de la sphère
     *  \param  angleY   ouverture en angle dans le plan Oxy
     *  \param  angleZ   ouverture en angle dans le plan Oxz
     *  \param  groupName le nom du groupe
     */
    CommandNewSpherePart(Internal::Context& c,
                       const double& dr,
                       const double& angleY,
                       const double& angleZ,
                       const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewSpherePart();

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
    double m_dr;
    double m_angleY;
    double m_angleZ;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWSPHEREPART_H_ */
/*----------------------------------------------------------------------------*/

