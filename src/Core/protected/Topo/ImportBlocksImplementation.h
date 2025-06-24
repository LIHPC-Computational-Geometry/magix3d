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
                               const std::string& n, bool withGeom);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
    */
    ~ImportBlocksImplementation() =default;

    void internalExecute();

    std::string getWarning();


private:
    void readNodes(ifstream &str, Group::Group0D* group);
    int readEdges(ifstream &str, Group::Group1D* group);
    void readFaces(ifstream &str, Group::Group2D* group);
    void readBlocks(ifstream &str, Group::Group3D* group);

    void readDiscr(ifstream &str, const int &nbEdges);

    void readAssociation(std::ifstream &str);

    bool moveStreamOntoFirst(std::ifstream& s,const std::string &AString);
    static std::string findGeom(int dim, int id);

private:
    /// helper pour la gestion des groupes dans les commandes
    Group::GroupHelperForCommand m_group_helper;

    /// Informations sur la commande: entités modifiées
    Internal::InfoCommand* m_icmd;

    Internal::Context& m_c;

    /// le nom du fichier en entier
    std::string m_filename;
    bool m_geom;

    std::string m_warning;

    std::vector<Vertex*> m_vertices;
    std::vector<std::string> m_vnames;

    std::vector<CoEdge*> m_coedges;
    std::vector<std::string> m_enames;

    std::vector<CoFace*> m_cofaces;
    std::vector<std::string > m_fnames;

    std::vector<std::string > m_bnames;
    std::vector<Block*> m_blocks;

};
}
}


#endif //MAGIX3D_IMPORTBLOCKSIMPLEMENTATION_H
