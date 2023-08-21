/*----------------------------------------------------------------------------*/
/*
 * ImportCATIAImplementation.h
 *
 *  Created on: 15 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTCATIAIMPLEMENTATION_H_
#define IMPORTCATIAIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomImport.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class ImportCATIAImplementation
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format CATIA
 */
class ImportCATIAImplementation: public GeomImport {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportCATIAImplementation(Internal::Context& c,
    		Internal::InfoCommand* icmd, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportCATIAImplementation();


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
#endif /* IMPORTCATIAIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


