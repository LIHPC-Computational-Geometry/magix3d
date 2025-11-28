#ifndef VERTEX_TOPO_PROPERTY_H_
#define VERTEX_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdge;
/*----------------------------------------------------------------------------*/
/**
 * \class VertexTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux sommets.
 */
 
 class VertexTopoProperty {
 public:
     VertexTopoProperty() = default;
     ~VertexTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     VertexTopoProperty* clone()
     {
        VertexTopoProperty* prop = new VertexTopoProperty();
        prop->m_coedges = m_coedges;
        prop->m_groups = m_groups;
        return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     std::vector<CoEdge*>& getCoEdgeContainer() {return m_coedges;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     std::vector<Group::Group0D*>& getGroupsContainer() {return m_groups;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des dépendances (entités topologiques incidentes)
      */
     inline void clearDependancy()
     {
         m_coedges.clear();
         m_groups.clear();
     }

 private:
     /// accès aux arêtes incidentes communes
     std::vector<CoEdge*> m_coedges;

     /// Listes des groupes 0D
     std::vector<Group::Group0D*> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* VERTEX_TOPO_PROPERTY_H_ */
