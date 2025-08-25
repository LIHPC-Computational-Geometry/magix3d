//
// Created by calderans on 06/08/25.
//
#include "Internal/Context.h"
#include "Internal/SelectionHelper.h"
#include "Topo/TopoEntity.h"
#include "Topo/TopoHelper.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Internal {
/*------------------------------------------------------------------------*/
        SelectionHelper::SelectionHelper() {
            MGX_FORBIDDEN("SelectionHelper::SelectionHelper is not allowed.");
        }   // SelectionHelper::SelectionHelper

/*----------------------------------------------------------------------------*/
        SelectionHelper::SelectionHelper(const SelectionHelper &) {
            MGX_FORBIDDEN("SelectionHelper::SelectionHelper is not allowed.");
        }   // SelectionHelper::SelectionHelper(const SelectionHelper&)

/*----------------------------------------------------------------------------*/
        SelectionHelper &SelectionHelper::operator=(const SelectionHelper &) {
            MGX_FORBIDDEN("SelectionHelper::operator = is not allowed.");
            return *this;
        }   // SelectionHelper::operator =

/*----------------------------------------------------------------------------*/
        SelectionHelper::~SelectionHelper() {
            MGX_FORBIDDEN("SelectionHelper::~SelectionHelper is not allowed.");
        }   // SelectionHelper::~SelectionHelper

/*------------------------------------------------------------------------*/

        void SelectionHelper::selectSheet(Topo::TopoEntity *entity) {
            std::vector<Topo::TopoEntity*> topoEntities = Topo::TopoHelper::getSheet(entity);

            std::vector<std::string> names;
            names.reserve(topoEntities.size());
            for (auto topoEntity : topoEntities) {
                names.push_back(topoEntity->getName());
            }

            getContext(getContextAlias())->addToSelection(names);
        }
    }
}