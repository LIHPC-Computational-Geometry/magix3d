/*----------------------------------------------------------------------------*/
/** \file FaceTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7/12/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef FACE_TOPO_PROPERTY_H_
#define FACE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include <TkUtil/Exception.h>
#include <vector>
#include <list>
#include <algorithm>
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
     FaceTopoProperty()
     {}

     ~FaceTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     FaceTopoProperty* clone() {
         FaceTopoProperty* prop = new FaceTopoProperty();

         prop->m_vertices.insert(prop->m_vertices.end(), m_vertices.begin(), m_vertices.end());
         prop->m_cofaces.insert(prop->m_cofaces.end(), m_cofaces.begin(), m_cofaces.end());
         prop->m_blocks.insert(prop->m_blocks.end(), m_blocks.begin(), m_blocks.end());

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     std::vector<Vertex*>& getVertices() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces communes
     std::vector<CoFace*>& getCoFaces() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des blocs
     std::vector<Block*>& getBlocks() {return m_blocks;}

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
