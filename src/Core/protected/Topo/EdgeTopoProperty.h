#ifndef EDGE_TOPO_PROPERTY_H_
#define EDGE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Vertex;
class CoFace;
class CoEdge;
/*----------------------------------------------------------------------------*/
/**
 * \class EdgeTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux arêtes.
 */
 
 class EdgeTopoProperty {
 public:
     EdgeTopoProperty() = default;
     ~EdgeTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     EdgeTopoProperty* clone()
     {
        EdgeTopoProperty* prop = new EdgeTopoProperty();
        prop->m_vertices = m_vertices;
        prop->m_coedges = m_coedges;
        prop->m_cofaces = m_cofaces;
        return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces communes
     std::vector<CoFace*>& getCoFaceContainer() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     std::vector<CoEdge*>& getCoEdgeContainer() {return m_coedges;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des dépendances (entités topologiques incidentes)
      */
     inline void clearDependancy()
     {
         m_cofaces.clear();
         m_vertices.clear();
         m_coedges.clear();
     }

 private:
     /// Lien avec les faces incidentes communes
     std::vector<CoFace*> m_cofaces;

     /// Lien avec les arêtes communes sur lesquelles se base cette arête
     std::vector<CoEdge*> m_coedges;

     /// Lien avec les sommets extrémités
     std::vector<Vertex*> m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EDGE_TOPO_PROPERTY_H_ */
