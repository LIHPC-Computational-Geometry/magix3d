//
// Created by calderans on 3/17/25.
//
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXPORTBLOCKS_H
#define COMMANDEXPORTBLOCKS_H
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Internal/Context.h"
#include "Topo/ExportBlocksImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
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
}
}

#endif //COMMANDEXPORTBLOCKS_H
