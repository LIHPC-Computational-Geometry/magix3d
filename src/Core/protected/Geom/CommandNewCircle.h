/*----------------------------------------------------------------------------*/
/*
 * CommandNewCircle.h
 *
 *  Created on: 28 oct. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWCIRCLE_H_
#define COMMANDNEWCIRCLE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewCircle
 *  \brief Commande permettant de créer un cercle
 */
/*----------------------------------------------------------------------------*/
class CommandNewCircle: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur. Le cercleh construit passera par les trois sommets
     *          p1, p2 et p3
     *
     *  \param p1 premier point du cercle
     *  \param p2 second point du cercle
     *  \param p3 dernier point du cercle
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewCircle(Internal::Context& c,
            Geom::Vertex* p1,
            Geom::Vertex* p2,
            Geom::Vertex* p3,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewCircle();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /* parametres de l'operation */
    Geom::Vertex* m_p1;
    Geom::Vertex* m_p2;
    Geom::Vertex* m_p3;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWCIRCLE_H_ */
/*----------------------------------------------------------------------------*/

