/*----------------------------------------------------------------------------*/
/*
 * ImportIGESImplementation.h
 *
 *  Created on: 21 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTIGESIMPLEMENTATION_H_
#define IMPORTIGESIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomImport.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class ImportIGESImplementation
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format IGES
 */
class ImportIGESImplementation: public GeomImport {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportIGESImplementation(Internal::Context& c, Internal::InfoCommand* icmd, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportIGESImplementation();


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
#endif /* IMPORTIGESIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

