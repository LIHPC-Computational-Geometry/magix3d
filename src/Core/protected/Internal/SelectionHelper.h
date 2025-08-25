//
// Created by calderans on 06/08/25.
//

/*----------------------------------------------------------------------------*/
#ifndef SELECTIONHELPER_H
#define SELECTIONHELPER_H
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
#include "Topo/TopoEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
    namespace Internal {
        class SelectionHelper {

        public:
            static void selectSheet(Topo::TopoEntity* entity);

        private:
            SelectionHelper ( );
            SelectionHelper (const SelectionHelper&);
            SelectionHelper& operator = (const SelectionHelper&);
            ~SelectionHelper ( );
        };
    }
}
#endif //SELECTIONHELPER_H
