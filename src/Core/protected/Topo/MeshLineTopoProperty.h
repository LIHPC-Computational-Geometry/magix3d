#ifndef MESH_LINE_TOPO_PROPERTY_H_
#define MESH_LINE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
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
 
 class MeshLineTopoProperty
 {
 public:
     MeshLineTopoProperty() = default;
     ~MeshLineTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     MeshLineTopoProperty* clone()
     {
         MeshLineTopoProperty* prop = new MeshLineTopoProperty();
         prop->m_coedges = m_coedges;
         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des arêtes communes
     std::vector<CoEdge*>& getCoEdgeContainer() {return m_coedges;}

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
