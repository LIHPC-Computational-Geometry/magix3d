/*----------------------------------------------------------------------------*/
/** \file CoFaceTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13/2/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COFACE_TOPO_PROPERTY_H_
#define COFACE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include <TkUtil/Exception.h>
#include <vector>
#include <list>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group2D;
}
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

     ~CoFaceTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     CoFaceTopoProperty* clone() {
         CoFaceTopoProperty* prop = new CoFaceTopoProperty();

         prop->m_vertices.clone(m_vertices);
         prop->m_edges.clone(m_edges);
         prop->m_faces.clone(m_faces);
         prop->m_groups.clone(m_groups);
         prop->m_hasHole = m_hasHole;

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     Utils::Container<Vertex>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes
     Utils::Container<Edge>& getEdgeContainer() {return m_edges;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     Utils::Container<Face>& getFaceContainer() {return m_faces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     Utils::Container<Group::Group2D>& getGroupsContainer() {return m_groups;}

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
     Utils::Container<Face>  m_faces;

     /// Lien avec les arêtes incidentes
     Utils::Container<Edge> m_edges;

     /// Lien avec les sommets extrémités
     Utils::Container<Vertex> m_vertices;

     /// Listes des groupes 2D
     Utils::Container<Group::Group2D> m_groups;

     /// vrai si les arêtes ne forment pas un unique cycle (il y a des trous)
     bool m_hasHole;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COFACE_TOPO_PROPERTY_H_ */
