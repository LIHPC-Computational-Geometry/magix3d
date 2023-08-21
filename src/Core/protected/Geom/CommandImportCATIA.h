/*----------------------------------------------------------------------------*/
/*
 * CommandImportCATIA.h
 *
 *  Created on: 12 nov. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDIMPORTCATIA_H_
#define COMMANDIMPORTCATIA_H_
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
class ImportCATIAImplementation;
/*----------------------------------------------------------------------------*/
/** \class CommandImportCATIA
 *  \brief Commande permettant d'importer le contenu d'un fichier au format
 *         CATIA
 */
/*----------------------------------------------------------------------------*/
class CommandImportCATIA: public CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param testVolumicProperties s'il est nécessaire que les volumes sont fermés
     *  \param splitCompoundCurves décompose les courbes composites en de multiples courbes
     */
    CommandImportCATIA(Internal::Context& c, const std::string& n,  const bool testVolumicProperties, const bool splitCompoundCurves);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportCATIA();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();


protected:

    /* fichier à importer*/
    std::string  m_filename;

    /* objet gérant l'opération de fusion*/
    ImportCATIAImplementation* m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTCATIA_H_ */
/*----------------------------------------------------------------------------*/

