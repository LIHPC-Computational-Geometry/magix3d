#ifndef COMMANDNEWHOLLOWSPHERE_H_
#define COMMANDNEWHOLLOWSPHERE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/Constants.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewHollowSphere
 *  \brief Commande permettant de créer une sphère creuse
 */
/*----------------------------------------------------------------------------*/
class CommandNewHollowSphere: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pcentre le centre de la sphère creuse
     *  \param dr_int le rayon interne de la sphère creuse
     *  \param dr_ext le rayon externe de la sphère creuse
     *  \param da angle de la portion de sphère
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewHollowSphere(Internal::Context& c,
                       const Utils::Math::Point& pcentre,
                       const double& dr_int,
                       const double& dr_ext,
                       const double& angle,
                       const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pcentre le centre de la sphère creuse
     *  \param dr_int le rayon interne de la sphère creuse
     *  \param dr_ext le rayon externe de la sphère creuse
     *  \param dp type de portion de sphere
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewHollowSphere(Internal::Context& c,
                       const Utils::Math::Point& pcentre,
                       const double& dr_int,
                       const double& dr_ext,
                       const Utils::Portion::Type& dp,
                       const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewHollowSphere();

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
    double m_dr_int;
    double m_dr_ext;
    double m_angle;
    Utils::Portion::Type m_type;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWHOLLOWSPHERE_H_ */
/*----------------------------------------------------------------------------*/

