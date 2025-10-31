#ifndef COEDGE_TOPO_PROPERTY_H_
#define COEDGE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Vertex;
class Edge;
/*----------------------------------------------------------------------------*/
/**
 * \class CoEdgeTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux arêtes dites communes (CoEdge)
 */
 
 class CoEdgeTopoProperty {
 public:
     CoEdgeTopoProperty() = default;
     ~CoEdgeTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     CoEdgeTopoProperty* clone()
     {
         CoEdgeTopoProperty* prop = new CoEdgeTopoProperty();
         prop->m_vertices = m_vertices;
         prop->m_edges = m_edges;
         prop->m_groups = m_groups;
         return prop;
     }
     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes
     std::vector<Edge*>& getEdgeContainer() {return m_edges;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     std::vector<Group::Group1D*>& getGroupsContainer() {return m_groups;}

    /*------------------------------------------------------------------------*/
     /** \brief   Suppression des dépendances (entités topologiques incidentes)
      */
     inline void clearDependancy()
     {
         m_edges.clear();
         m_vertices.clear();
         m_groups.clear();
     }

 private:
     /// Lien avec les arêtes qui utilisent cette CoEdge
     std::vector<Edge*> m_edges;

     /// Lien avec les sommets extrémités
     std::vector<Vertex*> m_vertices;

     /// Listes des groupes 1D
     std::vector<Group::Group1D*> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COEDGE_TOPO_PROPERTY_H_ */
