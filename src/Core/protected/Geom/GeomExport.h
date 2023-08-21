/*----------------------------------------------------------------------------*/
/*
 * GeomExport.h
 *
 *  Created on: 4 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMEXPORT_H_
#define GEOMEXPORT_H_
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomExport
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour l'exportation de toute la modélisation du GeomManager
     *
     *  \param c le contexte
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    GeomExport(Internal::Context& c, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour l'exportation d'un ensemble d'entités géométriques
     *
     *  \param c le contexte
     *  \param ge les entités à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    GeomExport(Internal::Context& c,
    		const std::vector<GeomEntity*>& ge,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour l'exportation d'une entité géométrique
     *
     *  \param c le contexte
     *  \param ge l'entité à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    GeomExport(Internal::Context& c,
    		GeomEntity* ge,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomExport();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'exportation
     */
    void perform(Internal::InfoCommand* icmd);


protected:

    /*------------------------------------------------------------------------*/
    /** \brief   Ajout d'une entité géométrique à exporter
     *
     *  \param geomEntity l'entité géométrique M3D à exporter
     */
    virtual void addGeomEntityToExport(GeomEntity* geomEntity)=0;

    /*------------------------------------------------------------------------*/
    /** \brief   Ecriture effective du fichier de sortie
     */
    virtual void write()=0;

protected:

    /// contexte d'exécution
    Internal::Context& m_context;

    /// entités à exporter
    std::vector<GeomEntity*> m_geomEntities;

    /// fichier dans lequel on exporte
    std::string m_filename;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMEXPORT_H_ */
/*----------------------------------------------------------------------------*/
