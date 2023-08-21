/*----------------------------------------------------------------------------*/
/*
 * CommandNewSphere.h
 *
 *  Created on: 23 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWSPHERE_H_
#define COMMANDNEWSPHERE_H_
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
/** \class CommandNewSphere
 *  \brief Commande permettant de créer une sphère
 */
/*----------------------------------------------------------------------------*/
class CommandNewSphere: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pcentre le centre de la sphère
     *  \param dr le rayon de la sphère
     *  \param da angle de la portion de sphère
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewSphere(Internal::Context& c,
                       const Utils::Math::Point& pcentre,
                       const double& dr,
                       const double& angle,
                       const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pcentre le centre de la sphère
     *  \param dr le rayon de la sphère
     *  \param dp type de portion de sphere
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewSphere(Internal::Context& c,
                       const Utils::Math::Point& pcentre,
                       const double& dr,
                       const Utils::Portion::Type& dp,
                       const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewSphere();

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
    Utils::Math::Point m_center;
    double m_dr;
    double m_angle;
    Utils::Portion::Type m_type;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWSPHERE_H_ */
/*----------------------------------------------------------------------------*/

