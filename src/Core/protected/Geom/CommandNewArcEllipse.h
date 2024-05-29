/*----------------------------------------------------------------------------*/
/*
 * CommandArcEllipse.h
 *
 *  Created on: 2 avr. 2012
 *      Author: ledouxf
 *
 *  EB:
 *     Cette ellipse est créée dans le plan XY et a pour axe 0X
 *     Si on veut pouvoir créer cette ellipse n'importe où,
 *     il est alors nécessaire de donner l'axe de l'ellipse
 *
 *     Les 3 points ne peuvent pas être alignés, sinon cela ne forme pas une unique ellipse
 *
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWARCELLIPSE_H_
#define COMMANDNEWARCELLIPSE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewArcEllipse
 *  \brief Commande permettant de créer un arc d'ellipse
 */
/*----------------------------------------------------------------------------*/
class CommandNewArcEllipse: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param pc centre de l'arc d'ellipse
     *  \param pd point de départ de l'arc d'ellipse
     *  \param pe point de fin de l'arc d'ellipse
     *  \param direct indique si l'on tourne dans le sens direct ou indirect
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewArcEllipse(Internal::Context& c,
            Geom::Vertex* pc,
            Geom::Vertex* pd,
            Geom::Vertex* pe,
            const bool direct,
           const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewArcEllipse();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /* parametres de l'operation */
    Geom::Vertex* m_center;
    Geom::Vertex* m_start;
    Geom::Vertex* m_end;
    bool m_direction;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWARCELLIPSE_H_ */
/*----------------------------------------------------------------------------*/


