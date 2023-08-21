/*----------------------------------------------------------------------------*/
/*
 * CommandCommon.h
 *
 *  Created on: 14 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCOMMON2D_H_
#define COMMANDCOMMON2D_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandCommon
 *  \brief Commande permettant de faire l'intersection Booléenne de 2 géométries
 */
/*----------------------------------------------------------------------------*/
class CommandCommon2D: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param geom_cmd la commande de copie géométrique avec les modifications
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    CommandCommon2D(Internal::Context& c, GeomEntity* e1, GeomEntity* e2, std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e1 une première entité
     *  \param e12 une deuxième entité
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    CommandCommon2D(Internal::Context& c, Geom::CommandGeomCopy* geom_cmd, std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandCommon2D();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();

protected:

    void validate(GeomEntity* entity1, GeomEntity* entity2);
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCOMMON2D_H_ */
/*----------------------------------------------------------------------------*/



