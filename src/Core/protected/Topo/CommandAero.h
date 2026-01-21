#ifndef MAGIX3D_COMMANDAERO_H
#define MAGIX3D_COMMANDAERO_H
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CommandAero: public CommandEditTopo{

public:
    CommandAero(Internal::Context& c);
    virtual ~CommandAero();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    void getPreviewRepresentation(Utils::DisplayRepresentation &dr);

    private:

};
}
}

#endif //MAGIX3D_COMMANDAERO_H