/*----------------------------------------------------------------------------*/
/** \file CommandNewCone.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/5/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWCONE_H_
#define COMMANDNEWCONE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Vector.h"
#include "Internal/Context.h"
#include "Geom/Volume.h"
#include "Utils/Constants.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewCone
 *  \brief Commande permettant de créer un cone
 */
/*----------------------------------------------------------------------------*/
class CommandNewCone: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param dr1 le premier rayon du cone (à l'origine)
     *  \param dr2 le deuxième rayon du cone
     *  \param dv le vecteur pour l'axe
     *  \param da l'angle de la portion de cone
     *  \param groupName optionnellement un nom de groupe
     */
	CommandNewCone(Internal::Context& c,
			const double& dr1,
			const double& dr2,
			const Utils::Math::Vector& dv,
			const double& da,
			const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param dr1 le premier rayon du cone (à l'origine)
     *  \param dr2 le deuxième rayon du cone
     *  \param dv le vecteur pour l'axe
     *  \param dp description de la portion de cylindre à générer
     */
	CommandNewCone(Internal::Context& c,
			const double& dr1,
			const double& dr2,
			const Utils::Math::Vector& dv,
			const Utils::Portion::Type& dp,
			const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du volume
     */
    virtual ~CommandNewCone();

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
    double m_dr1;
    double m_dr2;
    Utils::Math::Vector m_dv;
    /** angle en degre*/
    double m_angle;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWCONE_H_ */
/*----------------------------------------------------------------------------*/

