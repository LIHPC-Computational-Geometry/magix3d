/*----------------------------------------------------------------------------*/
/*
 * ImportSTLImplementation.h
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTSTLIMPLEMENTATION_H_
#define IMPORTSTLIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomImport.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class ImportSTLImplementation
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format STL
 */
class ImportSTLImplementation: public GeomImport {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportSTLImplementation(Internal::Context& c, Internal::InfoCommand* icmd, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportSTLImplementation();


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
#endif /* IMPORTSTLIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

