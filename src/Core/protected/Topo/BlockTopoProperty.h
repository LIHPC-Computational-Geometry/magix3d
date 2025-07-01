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

         prop->m_vertices.insert(prop->m_vertices.end(), m_vertices.begin(), m_vertices.end());
         prop->m_faces.insert(prop->m_faces.end(), m_faces.begin(), m_faces.end());
         prop->m_groups.insert(prop->m_groups.end(), m_groups.begin(), m_groups.end());

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     std::vector<Face*>& getFaceContainer() {return m_faces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     std::vector<Group::Group3D*>& getGroupsContainer() {return m_groups;}

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
     std::vector<Face*>  m_faces;

     /// Lien avec les sommets extrémités
     std::vector<Vertex*> m_vertices;

     /// Listes des groupes 3D
     std::vector<Group::Group3D*> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* BLOCK_TOPO_PROPERTY_H_ */
