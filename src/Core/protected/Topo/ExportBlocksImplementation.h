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
class ExportBlocksImplementation
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour l'exportation de tous les blocs
     *
     *  \param c le contexte
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportBlocksImplementation(Internal::Context& c, const std::string& n, const bool& withGeom);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    ~ExportBlocksImplementation()=default;

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'exportation
     */
    void perform(Internal::InfoCommand* icmd);

private:

    void writeNodes(std::ofstream& str, std::vector<Topo::Vertex*> vertices);
    void writeEdges(std::ofstream& str, std::vector<Topo::CoEdge*> edges);
    void writeFaces(std::ofstream& str, std::vector<Topo::CoFace*> faces);
    void writeBlocks(std::ofstream& str, std::vector<Topo::Block*> blocks);

    void writeAssociationNodes(ofstream &str, const std::vector<Topo::Vertex*>& vs);
    void writeAssociationEdges(ofstream &str, const std::vector<Topo::CoEdge*>& es);
    void writeAssociationFaces(ofstream &str, const std::vector<Topo::CoFace*>& fs);
    void writeAssociationBlocks(ofstream &str, const std::vector<Topo::Block*>& bs);


    /// contexte d'exécution
    Internal::Context& m_context;

    /// fichier dans lequel on exporte
    std::string m_filename;

    /// si oui ou non l'association géometrique est activée
    bool with_geom;

    std::map<std::string, int> m_node_ids_mapping;
    std::map<std::string, int> m_edge_ids_mapping;
    std::map<std::string, int> m_face_ids_mapping;
    std::map<std::string, int> m_block_ids_mapping;

};
}
}
#endif //EXPORTBLOCKSIMPLEMENTATION_H
