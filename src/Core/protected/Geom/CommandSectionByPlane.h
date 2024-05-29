/*----------------------------------------------------------------------------*/
/*
 * CommandSectionByPlane.h
 *
 *  Created on: 24 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
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
     *  \param es   les entités à découper
     *  \param p    le plan de coupe
     */
    CommandSectionByPlane(Internal::Context& c, std::vector<GeomEntity*>& es,
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


    void removeNonIntersectedEntities();

protected:

    /// valide les paramètres
    void validate();

    /* entités à découper*/
    std::vector<GeomEntity*> m_entities;

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



