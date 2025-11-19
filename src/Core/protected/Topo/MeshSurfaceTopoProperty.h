#ifndef MESH_SURFACE_TOPO_PROPERTY_H_
#define MESH_SURFACE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoFace;
/*----------------------------------------------------------------------------*/
/**
 * \class MeshSurfaceTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux blocs.
 */
 
class MeshSurfaceTopoProperty
{
 public:
     MeshSurfaceTopoProperty() = default;
     ~MeshSurfaceTopoProperty() = default;

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     MeshSurfaceTopoProperty* clone()
     {
         MeshSurfaceTopoProperty* prop = new MeshSurfaceTopoProperty();
         prop->m_cofaces = m_cofaces;
         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     std::vector<CoFace*>& getCoFaceContainer() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des relations
      */
     inline void clear()
     {
         m_cofaces.clear();
     }

 private:
     /// lien sur les Topo::CoFace qui ont participées à la constitution des mailles
     std::vector<CoFace*>  m_cofaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_SURFACE_TOPO_PROPERTY_H_ */
