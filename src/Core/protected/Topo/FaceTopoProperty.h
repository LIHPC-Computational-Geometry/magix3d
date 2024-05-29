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
#include "TkUtil/Exception.h"
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

         prop->m_vertices.clone(m_vertices);
         prop->m_cofaces.clone(m_cofaces);
         prop->m_blocks.clone(m_blocks);

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     Utils::Container<Vertex>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces communes
     Utils::Container<CoFace>& getCoFaceContainer() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des blocs
     Utils::Container<Block>& getBlockContainer() {return m_blocks;}

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
     Utils::Container<Block> m_blocks;

     /// Lien avec les faces communes
     Utils::Container<CoFace> m_cofaces;

      /// Lien avec les sommets extrémités
     Utils::Container<Vertex> m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* FACE_TOPO_PROPERTY_H_ */
