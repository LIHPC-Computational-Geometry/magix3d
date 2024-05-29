/*----------------------------------------------------------------------------*/
/** \file EdgeTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGE_TOPO_PROPERTY_H_
#define EDGE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include "TkUtil/Exception.h"
#include <vector>
#include <sys/types.h>
#include <algorithm>
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
     EdgeTopoProperty()
     {
     }

     ~EdgeTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     EdgeTopoProperty* clone() {
         EdgeTopoProperty* prop = new EdgeTopoProperty();
         prop->m_vertices.clone(m_vertices);
         prop->m_coedges.clone(m_coedges);
         prop->m_cofaces.clone(m_cofaces);

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des sommets
     Utils::Container<Vertex>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces communes
     Utils::Container<CoFace>& getCoFaceContainer() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     Utils::Container<CoEdge>& getCoEdgeContainer() {return m_coedges;}

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
     Utils::Container<CoFace> m_cofaces;

     /// Lien avec les arêtes communes sur lesquelles se base cette arête
     Utils::Container<CoEdge> m_coedges;

     /// Lien avec les sommets extrémités
     Utils::Container<Vertex> m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EDGE_TOPO_PROPERTY_H_ */
