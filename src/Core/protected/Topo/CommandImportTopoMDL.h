/*----------------------------------------------------------------------------*/
/*
 * CommandImportMDL.h
 *
 *  Created on: 21 mai 2012
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDIMPORTTOPOMDL_H_
#define COMMANDIMPORTTOPOMDL_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"
#include "Utils/Vector.h"
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
namespace Topo {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandImportTopoMDL
 *  \brief Commande permettant d'importer la topologie depuis un fichier au format
 *         MDL (modélisation issue de Magix 2D)
 */
/*----------------------------------------------------------------------------*/
class CommandImportTopoMDL: public CommandCreateTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param impl l'implémentation de l'importation MDL
     */
    CommandImportTopoMDL(Internal::Context& c, Internal::ImportMDLImplementation* impl);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportTopoMDL();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

protected:

    /* objet gérant l'opération d'importation */
    Internal::ImportMDLImplementation* m_impl;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTTOPOMDL_H_ */
/*----------------------------------------------------------------------------*/
