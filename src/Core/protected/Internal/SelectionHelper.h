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
            /**
             * This method allows bridge between @class selectionManager and TopoHelper by passing argument
             * @param entity the entity of which the sheet will be build
             * @param point a mathematical point used to get the direction of the sheet
             * @return the list containing all topoEntities that make up the sheet
             */
            static std::vector<Topo::TopoEntity*> selectSheet(Topo::TopoEntity* entity, double* point);

            /**
             * This method allows bridge between @class selectionManager and TopoHelper by passing argument
             * @param entity the entity of which the chord will be build
             * @param point a mathematical point used to get the direction of the chord
             * @returnthe list containing all topoEntities that make up the chord
             */
            static std::vector<Topo::TopoEntity*> selectChord(Topo::TopoEntity* entity, double* point);

        private:
            SelectionHelper ( );
            SelectionHelper (const SelectionHelper&);
            SelectionHelper& operator = (const SelectionHelper&);
            ~SelectionHelper ( );
        };
    }
}
#endif //SELECTIONHELPER_H
