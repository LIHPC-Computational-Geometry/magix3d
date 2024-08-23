/*----------------------------------------------------------------------------*/
/*
 * ExportBREPImplementation.h
 *
 *  Created on: 23 aout 2024
 *      Author: lelandaisb
 */
/*----------------------------------------------------------------------------*/
#ifndef EXPORTBREPIMPLEMENTATION_H_
#define EXPORTBREPIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomExport.h"
/*----------------------------------------------------------------------------*/
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class ExportBREPImplementation: public GeomExport
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur (exportation de toutes entités du GeomManager)
     *
     *  \param c le contexte
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportBREPImplementation(Internal::Context& c, const std::string& n);

    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param geomEntities les entités à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportBREPImplementation(Internal::Context& c,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ExportBREPImplementation();

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

private:
    BRep_Builder m_builder;
    TopoDS_Compound m_compound;    
};

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EXPORTBREPIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



