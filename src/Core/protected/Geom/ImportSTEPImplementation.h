/*----------------------------------------------------------------------------*/
/*
 * ImportSTEPImplementation.h
 *
 *  Created on: 15 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTSTEPIMPLEMENTATION_H_
#define IMPORTSTEPIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomImport.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class ImportSTEPImplementation
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format STEP
 */
class ImportSTEPImplementation: public GeomImport {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportSTEPImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportSTEPImplementation();

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Réalise l'ouverture du fichier géométrique au format
     *          souhaité
     */
    virtual void readFile();

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* IMPORTSTEPIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


