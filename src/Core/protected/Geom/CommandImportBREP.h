#ifndef COMMANDIMPORTBREP_H_
#define COMMANDIMPORTBREP_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
class ImportBREPImplementation;
/*----------------------------------------------------------------------------*/
/** \class CommandImportBREP
 *  \brief Commande permettant d'importer le contenu d'un fichier au format
 *         BREP
 */
/*----------------------------------------------------------------------------*/
class CommandImportBREP: public CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param testVolumicProperties s'il est nécessaire que les volumes sont fermés
     */
    CommandImportBREP(Internal::Context& c, const std::string& n, const bool testVolumicProperties);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportBREP();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();


protected:

    /* fichier à importer */
    std::string  m_filename;

    /* objet gérant l'opération de fusion */
    ImportBREPImplementation* m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTBREP_H_ */
/*----------------------------------------------------------------------------*/
