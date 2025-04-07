//
// Created by calderans on 4/3/25.
//

#ifndef MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
#define MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/Context.h"
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
    virtual ~ImportBlocksImplementation();

    void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

protected:
    bool moveStreamOntoFirst(std::ifstream& s,const std::string &AString);

protected:
    /** Informations sur la commande: entités modifiées */
    Internal::InfoCommand* m_icmd;

    Internal::Context& m_c;

    /// le nom du fichier en entier
    std::string m_filename;


};
}
}


#endif //MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
