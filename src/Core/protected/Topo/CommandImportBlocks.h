//
// Created by calderans on 4/3/25.
//

#ifndef MAGIX3D_COMMANDIMPORTBLOCKS_H
#define MAGIX3D_COMMANDIMPORTBLOCKS_H
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"
#include "Topo/ImportBlocksImplementation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CommandImportBlocks : public CommandCreateTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param impl l'implémentation de l'importation de Blocs
     */
    CommandImportBlocks(Internal::Context& c, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportBlocks();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();


protected:

    /* fichier à importer */
    std::string  m_filename;

    /* objet gérant l'opération d'importation */
    ImportBlocksImplementation* m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif //MAGIX3D_COMMANDIMPORTBLOCKS_H
/*----------------------------------------------------------------------------*/