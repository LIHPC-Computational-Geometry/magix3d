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

        void SelectionHelper::selectSheet(Topo::TopoEntity *entity, double* point, SelectionManager* selectionManager) {
            std::cout<<"selection helper"<<std::endl;
            std::vector<Topo::TopoEntity *> topoEntities = Topo::TopoHelper::getSheet(entity, point);

            std::vector<Utils::Entity *> entities;
            for (auto topoEntity : topoEntities) {
                entities.push_back(topoEntity);
            }

            selectionManager->addToSelection(entities);
        }

        /*------------------------------------------------------------------------*/

        void SelectionHelper::selectChord(Topo::TopoEntity *entity, double* point, SelectionManager* selectionManager) {
            std::vector<Topo::TopoEntity *> topoEntities = Topo::TopoHelper::getChord(entity, point);

            std::vector<Utils::Entity *> entities;
            for (auto topoEntity : topoEntities) {
                entities.push_back(topoEntity);
            }

            selectionManager->addToSelection(entities);
        }
    }
}


