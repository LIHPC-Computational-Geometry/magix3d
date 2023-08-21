/*----------------------------------------------------------------------------*/
/*
 * CommandNewHollowCylinder.h
 *
 *  Created on: 12 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWHOLLOWCYLINDER_H_
#define COMMANDNEWHOLLOWCYLINDER_H_
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
/** \class CommandNewHollowCylinder
 *  \brief Commande permettant de créer un cylindre creux
 */
/*----------------------------------------------------------------------------*/
class CommandNewHollowCylinder: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pcentre le centre du cercle de base
     *  \param dr_int le rayon du cercle interne
     *  \param dr_ext le rayon du cercle externe
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param angle définit la partie du cylindre que l'on conserve(rad.)
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewHollowCylinder(
            Internal::Context& c,
            const Utils::Math::Point& pcentre,
            const double& dr_int,
            const double& dr_ext,
            const Utils::Math::Vector& dv,
            const double& angle,
            const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pcentre le centre du cercle de base
     *  \param dr_int le rayon du cercle interne
     *  \param dr_ext le rayon du cercle externe
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param dp description de la portion de cylindre à générer
     */
    CommandNewHollowCylinder(
            Internal::Context& c,
            const Utils::Math::Point& pcentre,
            const double& dr_int,
            const double& dr_ext,
            const Utils::Math::Vector& dv,
            const Utils::Portion::Type& dp,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du volume
     */
    virtual ~CommandNewHollowCylinder();

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
    Utils::Math::Point m_centre;
    double m_dr_int;
    double m_dr_ext;
    Utils::Math::Vector m_dv;
    /** angle en degre*/
    double m_angle;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWHOLLOWCYLINDER_H_ */
/*----------------------------------------------------------------------------*/
