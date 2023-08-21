/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 *  CommandExportMDL.h
 *
 *  Created on: 28/6/2013
 *
 *  Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXPORTMDL_H_
#define COMMANDEXPORTMDL_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class ExportMDLImplementation;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportMDL
 *  \brief Commande permettant d'exporter une sélection dans un fichier au format
 *         MDL (modélisation issue de Magix 2D)
 */
/*----------------------------------------------------------------------------*/
class CommandExportMDL: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param impl l'implémentation de l'exportation MDL
     */
    CommandExportMDL(Internal::Context& c, Internal::ExportMDLImplementation* impl);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportMDL();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo(){}

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo(){}


protected:

    /* objet gérant l'opération d'exportation */
    Internal::ExportMDLImplementation* m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTMDL_H_ */
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/
