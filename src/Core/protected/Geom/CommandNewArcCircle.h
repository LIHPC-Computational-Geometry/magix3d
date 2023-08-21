/*----------------------------------------------------------------------------*/
/*
 * CommandNewArcCircle.h
 *
 *  Created on: 2 avr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWARCCIRCLE_H_
#define COMMANDNEWARCCIRCLE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewArcCircle
 *  \brief Commande permettant de créer un arc de cercle
 */
/*----------------------------------------------------------------------------*/
class CommandNewArcCircle: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param pc centre de l'arc de cercle
     *  \param pd point de départ de l'arc de cercle
     *  \param pe point de fin de l'arc de cercle
     *  \param direct indique si l'on tourne dans le sens direct ou indirect
     *  \param normal vecteur normal au plan pour lever l'ambiguïté dans le cas des 3 points alignés
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewArcCircle(Internal::Context& c,
            Geom::Vertex* pc,
            Geom::Vertex* pd,
            Geom::Vertex* pe,
            const bool direct,
            const Vector& normal,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewArcCircle();

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
    Geom::Vertex* m_center;
    Geom::Vertex* m_start;
    Geom::Vertex* m_end;
    /// vecteur normal au plan pour lever l'ambiguïté dans le cas des 3 points alignés
    Utils::Math::Vector m_normal;
    bool m_direction;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWARCCIRCLE_H_ */
/*----------------------------------------------------------------------------*/

