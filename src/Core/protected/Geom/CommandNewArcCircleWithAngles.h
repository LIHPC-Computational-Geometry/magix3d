/*----------------------------------------------------------------------------*/
/*
 * CommandNewArcCircleWithAngles.h
 *
 *  Created on: 29/9/2020
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWARCCIRCLEWITHANGLES_H_
#define COMMANDNEWARCCIRCLEWITHANGLES_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewArcCircleWithAngles
 *  \brief Commande permettant de créer un arc de cercle à partir d'angles et
 *  d'un rayon dans un repère donné
 */
/*----------------------------------------------------------------------------*/
class CommandNewArcCircleWithAngles: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param angleDep l'angle de départ / axe Ox du repère, dans le plan xOy
     *  \param angleFin l'angle de fin (en degrés)
     *  \param rayon celui du cercle
     *  \param repere repère dans lequel se fait la construction (peut-être nul)
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewArcCircleWithAngles(Internal::Context& c,
                                  const double& angleDep,
                                  const double& angleFin,
                                  const double& rayon,
                                  CoordinateSystem::SysCoord* rep,
                                  const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewArcCircleWithAngles();

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
    /* parametres de l'operation */
    double m_angleDep;
    double m_angleFin;
    double m_rayon;
    CoordinateSystem::SysCoord* m_rep;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWARCCIRCLEWITHANGLES_H_ */
/*----------------------------------------------------------------------------*/

