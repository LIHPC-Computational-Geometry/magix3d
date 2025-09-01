//
// Created by calderans on 06/08/25.
//

/*----------------------------------------------------------------------------*/
#ifndef SELECTIONHELPER_H
#define SELECTIONHELPER_H
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
#include "Internal/SelectionManager.h"
#include "Topo/TopoEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
    namespace Internal {
        class SelectionHelper {

        public:
            static void selectSheet(Topo::TopoEntity* entity, double* point, SelectionManager* selectionManager);

            static void selectChord(Topo::TopoEntity* entity, double* point, SelectionManager* selectionManager);

        private:
            SelectionHelper ( );
            SelectionHelper (const SelectionHelper&);
            SelectionHelper& operator = (const SelectionHelper&);
            ~SelectionHelper ( );
        };
    }
}
#endif //SELECTIONHELPER_H
