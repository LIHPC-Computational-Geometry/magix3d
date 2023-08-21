/*----------------------------------------------------------------------------*/
/*
 * ExportIGESImplementation.h
 *
 *  Created on: 14 févr. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef EXPORTIGESIMPLEMENTATION_H_
#define EXPORTIGESIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomExport.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
#include <IGESControl_Writer.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class ExportIGESImplementation: public GeomExport
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur (exportation de toutes entités du GeomManager)
     *
     *  \param c le contexte
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportIGESImplementation(Internal::Context& c, const std::string& n);

    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param geomEntities les entités à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportIGESImplementation(Internal::Context& c,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string& n);


    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ExportIGESImplementation();

protected:

    /*------------------------------------------------------------------------*/
    /** \brief   Ajout d'une entité géométrique à exporter
     *
     *  \param geomEntity l'entité géométrique M3D à exporter
     */
    void addGeomEntityToExport(GeomEntity* geomEntity);

    /*------------------------------------------------------------------------*/
    /** \brief   Ecriture effective du fichier de sortie
     */
    void write();

    /*------------------------------------------------------------------------*/
    /** \brief   Initialisation des unités et autres....
     */
    void init();

protected:

    IGESControl_Writer* m_writer;

};

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EXPORTIGESIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

