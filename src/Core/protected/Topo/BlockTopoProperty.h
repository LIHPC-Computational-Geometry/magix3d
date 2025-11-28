#ifndef BLOCK_TOPO_PROPERTY_H_
#define BLOCK_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
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
     BlockTopoProperty() = default;
     ~BlockTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     BlockTopoProperty* clone()
     {
         BlockTopoProperty* prop = new BlockTopoProperty();
         prop->m_vertices = m_vertices;
         prop->m_faces = m_faces;
         prop->m_groups = m_groups;
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
