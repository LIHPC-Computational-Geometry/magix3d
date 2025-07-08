/*----------------------------------------------------------------------------*/
/** \file MeshLineTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/08/16
 */
/*----------------------------------------------------------------------------*/
#ifndef MESH_LINE_TOPO_PROPERTY_H_
#define MESH_LINE_TOPO_PROPERTY_H_
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
class CoEdge;
/*----------------------------------------------------------------------------*/
/**
 * \class MeshLineTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux arêtes et sommets.
 */
 
 class MeshLineTopoProperty {
 public:
     MeshLineTopoProperty()
     {
     }

     ~MeshLineTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     MeshLineTopoProperty* clone() {
         MeshLineTopoProperty* prop = new MeshLineTopoProperty();
         prop->m_coedges.insert(prop->m_coedges.end(), m_coedges.begin(), m_coedges.end());

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     std::vector<CoEdge*>& getCoEdges() {return m_coedges;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des relations
      */
     inline void clear()
     {
         m_coedges.clear();
     }

 private:
     /// Lien avec les arêtes communes sur lesquelles se base ce nuage
     std::vector<CoEdge*> m_coedges;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_LINE_TOPO_PROPERTY_H_ */
