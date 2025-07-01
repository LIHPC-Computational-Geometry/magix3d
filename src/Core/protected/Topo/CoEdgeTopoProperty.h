/*----------------------------------------------------------------------------*/
/** \file CoEdgeTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COEDGE_TOPO_PROPERTY_H_
#define COEDGE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include <TkUtil/Exception.h>
#include <vector>
#include <sys/types.h>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group1D;
}
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
     CoEdgeTopoProperty()
     {
     }

     ~CoEdgeTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     CoEdgeTopoProperty* clone() {
         CoEdgeTopoProperty* prop = new CoEdgeTopoProperty();

         prop->m_vertices.insert(prop->m_vertices.end(), m_vertices.begin(), m_vertices.end());
         prop->m_edges.insert(prop->m_edges.end(), m_edges.begin(), m_edges.end());
         prop->m_groups.insert(prop->m_groups.end(), m_groups.begin(), m_groups.end());

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
