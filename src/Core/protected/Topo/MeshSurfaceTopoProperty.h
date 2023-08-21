/*----------------------------------------------------------------------------*/
/** \file MeshSurfaceTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/11/13
 */
/*----------------------------------------------------------------------------*/
#ifndef MESH_SURFACE_TOPO_PROPERTY_H_
#define MESH_SURFACE_TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Container.h"
#include "TkUtil/Exception.h"
#include <sys/types.h>
#include <vector>
#include <list>
#include <algorithm>
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
 
 class MeshSurfaceTopoProperty {
 public:
     MeshSurfaceTopoProperty()
     {}

     ~MeshSurfaceTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     MeshSurfaceTopoProperty* clone() {
         MeshSurfaceTopoProperty* prop = new MeshSurfaceTopoProperty();

         prop->m_cofaces.clone(m_cofaces);

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des faces
     Utils::Container<CoFace>& getCoFaceContainer() {return m_cofaces;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des relations
      */
     inline void clear()
     {
         m_cofaces.clear();
     }

 private:
     /// lien sur les Topo::CoFace qui ont participées à la constitution des mailles
     Utils::Container<CoFace>  m_cofaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_SURFACE_TOPO_PROPERTY_H_ */
