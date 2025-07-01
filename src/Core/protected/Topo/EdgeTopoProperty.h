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
#include <TkUtil/Exception.h>
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
         prop->m_vertices.insert(prop->m_vertices.end(), m_vertices.begin(), m_vertices.end());
         prop->m_coedges.insert(prop->m_coedges.end(), m_coedges.begin(), m_coedges.end());
         prop->m_cofaces.insert(prop->m_cofaces.end(), m_cofaces.begin(), m_cofaces.end());

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
