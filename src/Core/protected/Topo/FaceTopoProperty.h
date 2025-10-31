#ifndef FACE_TOPO_PROPERTY_H_
#define FACE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Vertex;
class Edge;
class CoFace;
class Block;
/*----------------------------------------------------------------------------*/
/**
 * \class FaceTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux faces.
 */
 
 class FaceTopoProperty {
 public:
     FaceTopoProperty() = default;
     ~FaceTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     FaceTopoProperty* clone()
     {
        FaceTopoProperty* prop = new FaceTopoProperty();
        prop->m_vertices = m_vertices;
        prop->m_cofaces = m_cofaces;
        prop->m_blocks = m_blocks;
        return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces communes
     std::vector<CoFace*>& getCoFaceContainer() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des blocs
     std::vector<Block*>& getBlockContainer() {return m_blocks;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des dépendances (entités topologiques incidentes)
      */
     inline void clearDependancy()
     {
         m_blocks.clear();
         m_vertices.clear();
         m_cofaces.clear();
     }

 private:
     /// Lien avec les blocs incidents
     std::vector<Block*> m_blocks;

     /// Lien avec les faces communes
     std::vector<CoFace*> m_cofaces;

      /// Lien avec les sommets extrémités
     std::vector<Vertex*> m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* FACE_TOPO_PROPERTY_H_ */
