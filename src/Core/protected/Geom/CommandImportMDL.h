/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 * CommandImportMDL.h
 *
 *  Created on: 10 mai 2012
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDIMPORTMDL_H_
#define COMMANDIMPORTMDL_H_
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
namespace Internal {
class ImportMDLImplementation;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandImportMDL
 *  \brief Commande permettant d'importer le contenu d'un fichier au format
 *         MDL (modélisation issue de Magix 2D)
 */
/*----------------------------------------------------------------------------*/
class CommandImportMDL: public CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param impl l'implémentation de l'importation MDL
     */
    CommandImportMDL(Internal::Context& c, Internal::ImportMDLImplementation* impl);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportMDL();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();


protected:

    /* objet gérant l'opération d'importation */
    Internal::ImportMDLImplementation* m_impl;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTMDL_H_ */
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/
