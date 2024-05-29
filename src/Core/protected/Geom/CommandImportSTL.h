/*----------------------------------------------------------------------------*/
/*
 * CommandImportSTL.h
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
 /*----------------------------------------------------------------------------*/

#ifndef COMMANDIMPORTSTL_H_
#define COMMANDIMPORTSTL_H_

/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
class ImportSTLImplementation;
/*----------------------------------------------------------------------------*/
/** \class CommandImportSTL
 *  \brief Commande permettant d'importer le contenu d'un fichier au format
 *         STL
 */
/*----------------------------------------------------------------------------*/
class CommandImportSTL: public CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    CommandImportSTL(Internal::Context& c, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportSTL();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();


protected:

    /* fichier à importer*/
    std::string  m_filename;

    /* objet gérant l'opération de fusion*/
    ImportSTLImplementation* m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTSTL_H_ */
/*----------------------------------------------------------------------------*/

