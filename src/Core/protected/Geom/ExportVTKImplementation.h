/*----------------------------------------------------------------------------*/
/*
 * ExportVTKImplementation.h
 *
 *  Created on: 14 avr. 2014
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_EXPORTVTKIMPLEMENTATION_H_
#define MGX3D_GEOM_EXPORTVTKIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class ExportVTKImplementation
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur (exportation de toutes entités du GeomManager)
     *
     *  \param c le contexte
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportVTKImplementation(Internal::Context& c, const std::string& n);

    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param geomEntities les entités à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportVTKImplementation(Internal::Context& c,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ExportVTKImplementation();


    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'exportation
     */
    void perform(Internal::InfoCommand* icmd);

private:

    /// contexte d'exécution
    Internal::Context& m_context;

    /// fichier dans lequel on exporte
    std::string m_filename;
};

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_EXPORTVTKIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



