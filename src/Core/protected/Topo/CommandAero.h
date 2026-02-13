#ifndef MAGIX3D_COMMANDAERO_H
#define MAGIX3D_COMMANDAERO_H
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CommandAero: public CommandCreateTopo{

public:
    CommandAero(Internal::Context& c, std::string filename);
    virtual ~CommandAero();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    void getPreviewRepresentation(Utils::DisplayRepresentation &dr);

    private:
    Internal::Context& m_c;

    std::string m_filename;


    std::map<int, Topo::Vertex*> m_gmdsNode2TopoNode;
    std::map<int, Topo::CoEdge*> m_gmdsEdge2TopoEdge;
    std::map<int, Topo::CoFace*> m_gmdsFace2TopoFace;
    std::map<int, Topo::Block*> m_gmdsBlock2TopoBlock;
};
}
}

#endif //MAGIX3D_COMMANDAERO_H