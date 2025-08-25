//
// Created by calderans on 4/3/25.
//

#ifndef MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
#define MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Group/GroupHelperForCommand.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/

class ImportBlocksImplementation {
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportBlocksImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
                               const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
    */
    ~ImportBlocksImplementation() =default;

    void internalExecute();

private:
    bool moveStreamOntoFirst(std::ifstream& s,const std::string &AString);

private:
    /// helper pour la gestion des groupes dans les commandes
    Group::GroupHelperForCommand m_group_helper;

    /// Informations sur la commande: entités modifiées
    Internal::InfoCommand* m_icmd;

    Internal::Context& m_c;

    /// le nom du fichier en entier
    std::string m_filename;
};
}
}


#endif //MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
