/*----------------------------------------------------------------------------*/
/*
 *  CommandExportBlocksForCGNS.h
 *
 *  Created on: 17/04/2014
 *
 *  Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_COMMANDEXPORTBLOCKSFORCGNS_H_
#define MGX3D_COMMANDEXPORTBLOCKSFORCGNS_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"
#include "Internal/Context.h"
#include "ExportBlocksCGNSImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportBlocksForCGNS
 *  \brief Commande permettant d'exporter le maillage au format CGNS
 */
/*----------------------------------------------------------------------------*/
class CommandExportBlocksForCGNS: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportBlocksForCGNS(Internal::Context& context, const std::string &fileName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportBlocksForCGNS();

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
    ExportBlocksCGNSImplementation m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_COMMANDEXPORTBLOCKSFORCGNS_H_ */
/*----------------------------------------------------------------------------*/
