/*----------------------------------------------------------------------------*/
/** \file BlockTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef BLOCK_TOPO_PROPERTY_H_
#define BLOCK_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include <TkUtil/Exception.h>
#include <sys/types.h>
#include <vector>
#include <list>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group3D;
}
namespace Topo {
class Vertex;
class Face;
/*----------------------------------------------------------------------------*/
/**
 * \class BlockTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux blocs.
 */
 
 class BlockTopoProperty {
 public:
     BlockTopoProperty()
     {}

     ~BlockTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     BlockTopoProperty* clone() {
         BlockTopoProperty* prop = new BlockTopoProperty();

         prop->m_vertices.clone(m_vertices);
         prop->m_faces.clone(m_faces);
         prop->m_groups.clone(m_groups);

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     Utils::Container<Vertex>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     Utils::Container<Face>& getFaceContainer() {return m_faces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     Utils::Container<Group::Group3D>& getGroupsContainer() {return m_groups;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des dépendances (entités topologiques incidentes)
      */
     inline void clearDependancy()
     {
         m_faces.clear();
         m_vertices.clear();
         m_groups.clear();
     }

 private:
     /// Lien avec les faces qui composent le bloc
     Utils::Container<Face>  m_faces;

     /// Lien avec les sommets extrémités
     Utils::Container<Vertex> m_vertices;

     /// Listes des groupes 3D
     Utils::Container<Group::Group3D> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* BLOCK_TOPO_PROPERTY_H_ */
