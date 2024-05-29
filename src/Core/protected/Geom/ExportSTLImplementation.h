/*----------------------------------------------------------------------------*/
/*
 * ExportSTLImplementation.h
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef EXPORTSTLIMPLEMENTATION_H_
#define EXPORTSTLIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomExport.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class ExportSTLImplementation: public GeomExport
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param geomEntity l'entité à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportSTLImplementation(Internal::Context& c,
    		GeomEntity* geomEntity,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ExportSTLImplementation();

protected:

    /*------------------------------------------------------------------------*/
    /** \brief   Ajout d'une entité géométrique à exporter
     *
     *  \param geomEntity l'entité géométrique M3D à exporter
     */
    void addGeomEntityToExport(GeomEntity* geomEntity) {}

    /*------------------------------------------------------------------------*/
    /** \brief   Ecriture effective du fichier de sortie
     */
    void write();

protected:

};

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EXPORTSTLIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



