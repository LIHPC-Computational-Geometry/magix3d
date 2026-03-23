#ifndef COMMANDMOVETO_H
#define COMMANDMOVETO_H
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandMoveTo
 *  \brief Commande permettant de déplacer une entité géométrique vers une
 *  coordonnée donnée
 */
/*----------------------------------------------------------------------------*/
class CommandMoveTo: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités à translater
     *  \param dv le vecteur de translation
     */
    CommandMoveTo(Internal::Context& c,
            std::vector<GeomEntity*>& e,
            const Utils::Math::Point& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd la commande de construction des entités à déplacer
     *  \param dv le vecteur de translation
     */
    CommandMoveTo(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const Utils::Math::Point& dv);

   /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandMoveTo();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();


protected:

    Utils::Math::Vector computeVector(const std::vector<GeomEntity*>& entities);

    /// valide les paramètres
    void validate();

    /* vecteur de translation */
    Utils::Math::Point m_point;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif //COMMANDMOVETO_H