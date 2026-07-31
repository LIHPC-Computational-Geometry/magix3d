#ifndef MAGIX3D_CONTROLLER_H
#define MAGIX3D_CONTROLLER_H

#include "Internal/Context.h"

namespace Mgx3D
{
    class Controller
    {
    public:
        Controller();
        ~Controller();

        void init(Internal::Context *context);

        // Les groupes
        void getGroupsNames();

        // Geom+Topo
        // Par exemple getPoint()

        // La géométrie

        // La topologie

        // Le maillage

        // La Selection

        // Le scripting

        // Les ressources
        // Rien a faire si on met les resources liées a l'IHM dans la partie IHM ?

        // Les commandes

        // Les entités

        // Les syscoords

    private:

        void setContext(Internal::Context *context);

        Internal::Context* _context;
    };
}

#endif //MAGIX3D_CONTROLLER_H
