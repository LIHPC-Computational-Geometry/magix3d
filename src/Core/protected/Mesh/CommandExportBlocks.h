/*----------------------------------------------------------------------------*/
/*
 *  CommandExportBlocks.h
 *
 *  Created on: 17/04/2014
 *
 *  Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_COMMANDEXPORTBLOCKS_H_
#define MGX3D_GEOM_COMMANDEXPORTBLOCKS_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "ExportBlocksImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportBlocks
 *  \brief Commande permettant d'exporter l'ensemble des entités géométriques
 *         dans un fichier au format VTK
 */
/*----------------------------------------------------------------------------*/
class CommandExportBlocks: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportBlocks(Internal::Context& context, const std::string &fileName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportBlocks();

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
    ExportBlocksImplementation m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_COMMANDEXPORTBLOCKS_H_ */
/*----------------------------------------------------------------------------*/
