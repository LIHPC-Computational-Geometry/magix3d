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

         prop->m_vertices.insert(prop->m_vertices.end(), m_vertices.begin(), m_vertices.end());
         prop->m_edges.insert(prop->m_edges.end(), m_edges.begin(), m_edges.end());
         prop->m_faces.insert(prop->m_faces.end(), m_faces.begin(), m_faces.end());
         prop->m_groups.insert(prop->m_groups.end(), m_groups.begin(), m_groups.end());
         prop->m_hasHole = m_hasHole;

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertices() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes
     std::vector<Edge*>& getEdges() {return m_edges;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     std::vector<Face*>& getFaces() {return m_faces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des groupes
     std::vector<Group::Group2D*>& getGroups() {return m_groups;}

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
