/*----------------------------------------------------------------------------*/
/** \file GeomDisplayRepresentation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 30/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMDISPLAYREPRESENTATION_H_
#define GEOMDISPLAYREPRESENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class GeomDisplayRepresentation
 *  \brief La classe GeomDisplayRepresention décrit une représentation
 *         de ce quoi doit être affichée pour représenter graphiquement une
 *         entité géométrique.
 */
/*----------------------------------------------------------------------------*/
class GeomDisplayRepresentation: public Utils::DisplayRepresentation{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GeomDisplayRepresentation(const type t=Utils::DisplayRepresentation::WIRE,
            const double d=0.001);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GeomDisplayRepresentation();

    /*------------------------------------------------------------------------*/
    /** \brief  mise à jour de la déflection de la représentation
     */
    virtual void setDeflection(const double d);

    /*------------------------------------------------------------------------*/
    /** \brief  récupération de la déflection de la représentation
     */
    double getDeflection() const;

    /*------------------------------------------------------------------------*/
    /** \brief  mise à jour du nombre de points de la représentation
     */
    int getNbPts() const;

    /*------------------------------------------------------------------------*/
    /** \brief  récupération du nombre de points de la représentation
     */
    void setNbPts(const int nb);


protected:

    /** déflection associée à la représentation */
    double m_deflection;

    /** nombre de points (pour les courbes) */
    int m_nb_pts;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMDISPLAYREPRESENTATION_H_ */
/*----------------------------------------------------------------------------*/

