/*
 * CommandScaling.h
 *
 *  Created on: 19 juin 2013
 *      Author: ledouxf
 */

#ifndef COMMANDSCALING_H_
#define COMMANDSCALING_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandScaling
 *  \brief Commande permettant de créer une boite
 */
/*----------------------------------------------------------------------------*/
class CommandScaling: public Geom::CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités à scaler
     *  \param factor le facteur de scaling
     *  \param pcentre centre de l'homotéthie
     */
    CommandScaling(Internal::Context& c,
            std::vector<GeomEntity*>& e,
            const double factor,
            const Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd la commande de construction des entités à scaler
     *  \param factor le facteur de scaling
     *  \param pcentre centre de l'homotéthie
     */
    CommandScaling(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const double factor,
            const Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param factor le facteur de scaling
     *  \param pcentre centre de l'homotéthie
     */
    CommandScaling(Internal::Context& c,
            const double factor,
            const Point& pcentre);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités à scaler
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    CommandScaling(Internal::Context& c,
            std::vector<GeomEntity*>& e,
            const double factorX,
            const double factorY,
            const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd la commande de construction des entités à scaler
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    CommandScaling(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const double factorX,
            const double factorY,
            const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param factorX le facteur de scaling suivant X
     *  \param factorY le facteur de scaling suivant Y
     *  \param factorZ le facteur de scaling suivant Z
     */
    CommandScaling(Internal::Context& c,
            const double factorX,
            const double factorY,
            const double factorZ);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandScaling();

    /*------------------------------------------------------------------------*/
     /** \brief  exécute la commande
      */
     void internalSpecificExecute();


     /*------------------------------------------------------------------------*/
     /** \brief  initialisation de la commande
      */
     void internalSpecificPreExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  Traitement spécifique délégué aux classes filles
     */
    void internalSpecificUndo();
    void internalSpecificRedo();

protected:
     /// valide les paramètres
     void validate();

     /*facteur d'homothétie */
     double m_factor;

     /// vrai si l'on utilise un facteur unique pour toutes les directions
     bool m_isHomogene;
     /** facteur d'homothétie suivant X */
     double m_factorX;
     /** facteur d'homothétie suivant Y */
     double m_factorY;
     /** facteur d'homothétie suivant Z */
     double m_factorZ;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSCALING_H_ */
/*----------------------------------------------------------------------------*/

