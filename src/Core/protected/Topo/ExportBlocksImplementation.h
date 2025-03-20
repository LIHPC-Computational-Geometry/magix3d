//
// Created by calderans on 3/17/25.
//

/*----------------------------------------------------------------------------*/
#ifndef EXPORTBLOCKSIMPLEMENTATION_H
#define EXPORTBLOCKSIMPLEMENTATION_H
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class ExportBlocksCGNSImplementation
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour l'exportation de tous les blocs
     *
     *  \param c le contexte
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportBlocksCGNSImplementation(Internal::Context& c, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ExportBlocksCGNSImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'exportation
     */
    void perform(Internal::InfoCommand* icmd);

protected:

    void init();

    /// contexte d'exécution
    Internal::Context& m_context;

    /// fichier dans lequel on exporte
    std::string m_filename;

    std::ofstream* m_stream;

    std::map<std::string, int> m_node_ids_mapping;
    std::map<std::string, int> m_edge_ids_mapping;
    std::map<std::string, int> m_face_ids_mapping;
};
}
}
#endif //EXPORTBLOCKSIMPLEMENTATION_H
