#ifndef COFACE_TOPO_PROPERTY_H_
#define COFACE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Vertex;
class Edge;
class Face;
class Block;
/*----------------------------------------------------------------------------*/
/**
 * \class CoFaceTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux faces communes
 * (partie commune entre 2 blocs).
 */
 
 class CoFaceTopoProperty {
 public:
     CoFaceTopoProperty()
     :m_hasHole(false)
     {}

     ~CoFaceTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     CoFaceTopoProperty* clone() {
        CoFaceTopoProperty* prop = new CoFaceTopoProperty();
        prop->m_vertices = m_vertices;
        prop->m_edges = m_edges;
        prop->m_faces = m_faces;
        prop->m_groups = m_groups;
        prop->m_hasHole = m_hasHole;
        return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes
     std::vector<Edge*>& getEdgeContainer() {return m_edges;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     std::vector<Face*>& getFaceContainer() {return m_faces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     std::vector<Group::Group2D*>& getGroupsContainer() {return m_groups;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des dépendances (entités topologiques incidentes)
      */
     inline void clearDependancy()
     {
         m_faces.clear();
         m_edges.clear();
         m_vertices.clear();
         m_groups.clear();
    }

     /*------------------------------------------------------------------------*/
     /// accesseur sur la présence ou non de trous
     bool hasHole() {return m_hasHole;}

     /// modifie l'attribut sur la présence ou non de trous
     void setHoled(bool b) {m_hasHole = b;}

 private:
     /// Lien avec les faces incidentes
     std::vector<Face*>  m_faces;

     /// Lien avec les arêtes incidentes
     std::vector<Edge*> m_edges;

     /// Lien avec les sommets extrémités
     std::vector<Vertex*> m_vertices;

     /// Listes des groupes 2D
     std::vector<Group::Group2D*> m_groups;

     /// vrai si les arêtes ne forment pas un unique cycle (il y a des trous)
     bool m_hasHole;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COFACE_TOPO_PROPERTY_H_ */
