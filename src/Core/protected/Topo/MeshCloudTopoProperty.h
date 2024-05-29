/*----------------------------------------------------------------------------*/
/** \file MeshCloudTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/11/13
 */
/*----------------------------------------------------------------------------*/
#ifndef MESH_CLOUD_TOPO_PROPERTY_H_
#define MESH_CLOUD_TOPO_PROPERTY_H_
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
class CoEdge;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class MeshCloudTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux arêtes et sommets.
 */
 
 class MeshCloudTopoProperty {
 public:
     MeshCloudTopoProperty()
     {
     }

     ~MeshCloudTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     MeshCloudTopoProperty* clone() {
         MeshCloudTopoProperty* prop = new MeshCloudTopoProperty();
         prop->m_coedges.clone(m_coedges);
         prop->m_vertices.clone(m_vertices);

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     Utils::Container<CoEdge>& getCoEdgeContainer() {return m_coedges;}

     /// accesseur sur le conteneur des sommets
     Utils::Container<Vertex>& getVertexContainer() {return m_vertices;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des relations
      */
     inline void clear()
     {
         m_coedges.clear();
         m_vertices.clear();
     }

 private:
     /// Lien avec les arêtes communes sur lesquelles se base ce nuage
     Utils::Container<CoEdge> m_coedges;

     /// Lien avec les sommets sur lesquels se base ce nuage
     Utils::Container<Vertex> m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_CLOUD_TOPO_PROPERTY_H_ */
