#ifndef COMMANDSECTIONBYPLANE_H_
#define COMMANDSECTIONBYPLANE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Plane.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandSectionByPlane
 *  \brief Commande permettant de découper des géométries à l'aide d'un plan
 */
/*----------------------------------------------------------------------------*/
class CommandSectionByPlane: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur
     *
     *  \param c    le contexte
     *  \param e   l'entité à découper
     *  \param p    le plan de coupe
     */
    CommandSectionByPlane(Internal::Context& c, GeomEntity* e,
                            Utils::Math::Plane* p,
                            std::string planeGroupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSectionByPlane();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();

protected:
    bool isIntersected();

    /// valide les paramètres
    void validate();

    /* entité à découper*/
    GeomEntity* m_entity;

    /* outil de découpe */
    Utils::Math::Plane* m_tool;

    /* outil de découpe */
    std::string m_planeName;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSECTIONBYPLANE_H_ */
/*----------------------------------------------------------------------------*/



