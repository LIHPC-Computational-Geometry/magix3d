/*----------------------------------------------------------------------------*/
/*
 * CommandSplitCurve.h
 *
 *  Created on: 12/4/2018
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITCURVE_H_
#define COMMANDSPLITCURVE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

class Curve;

/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandSplitCurve
 *  \brief Commande permettant de découper une courbe en deux suivant la position d'un point
 */
/*----------------------------------------------------------------------------*/
class CommandSplitCurve: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur
     *
     *  \param c   le contexte
     *  \param crv la courbe à découper
     *  \param pt  le point
     */
    CommandSplitCurve(Internal::Context& c, Curve* crv, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitCurve();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();


    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();



protected:

//    /* courbe à découper*/
//    Curve* m_crv;
//
//    /* point pour positionner la découpe */
//    Point m_pt;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITCURVE_H_ */
/*----------------------------------------------------------------------------*/


