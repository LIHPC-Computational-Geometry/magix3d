/*----------------------------------------------------------------------------*/
/*
 * CommandRemove.h
 *
 *  Created on: 10 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDREMOVE_H_
#define COMMANDREMOVE_H_
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomRemoveImplementation.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class CommandRemove: public Geom::CommandEditGeom {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param entities les entities à supprimer
     *  \param propageDown si true, on supprime les entités incidentes de dim
     *                     inférieure
     */
    CommandRemove(Internal::Context& c,
            std::vector<GeomEntity*>& es,
            const bool propagateDown);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandRemove();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();


protected:

    /* entités à détruire */
    std::vector<GeomEntity*> m_entities;
    /* destruction ou non des entités de dimension inférieur */
    bool m_propagate;


};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* COMMANDREMOVE_H_ */
/*----------------------------------------------------------------------------*/

