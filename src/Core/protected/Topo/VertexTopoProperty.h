/*----------------------------------------------------------------------------*/
/** \file VertexTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef VERTEX_TOPO_PROPERTY_H_
#define VERTEX_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include "TkUtil/Exception.h"
#include <vector>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group0D;
}
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
     VertexTopoProperty()
     {}

     ~VertexTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     VertexTopoProperty* clone() {
         VertexTopoProperty* prop = new VertexTopoProperty();

         prop->m_coedges.clone(m_coedges);
         prop->m_groups.clone(m_groups);

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     Utils::Container<CoEdge>& getCoEdgeContainer() {return m_coedges;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     Utils::Container<Group::Group0D>& getGroupsContainer() {return m_groups;}

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
     Utils::Container<CoEdge> m_coedges;

     /// Listes des groupes 0D
     Utils::Container<Group::Group0D> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* VERTEX_TOPO_PROPERTY_H_ */
