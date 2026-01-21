//
// Created by calderans on 21/01/2026.
//

#include "Topo/CommandAero.h"

#include "gmds/aero/AeroPipeline_3D.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandAero::CommandAero(Internal::Context &c) : CommandEditTopo(c, "Génération d'une structure de blocs via la méthode Aero") {

}
/*----------------------------------------------------------------------------*/
CommandAero::~CommandAero() {

}
/*----------------------------------------------------------------------------*/
void CommandAero::internalExecute() {
    std::string dir = "";
    gmds::AeroPipeline_3D pipeline("",dir);

    pipeline.execute();
    pipeline.getBlocking();
}
}
}