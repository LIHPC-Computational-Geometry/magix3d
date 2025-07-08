/*----------------------------------------------------------------------------*/
/** \file MeshVolumeTopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/11/13
 */
/*----------------------------------------------------------------------------*/
#ifndef MESH_VOLUME_TOPO_PROPERTY_H_
#define MESH_VOLUME_TOPO_PROPERTY_H_
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
class Block;
/*----------------------------------------------------------------------------*/
/**
 * \class MeshVolumeTopoProperty
 * 
 * Rassemble les propriétés topologiques communes aux Mesh::Volume.
 */
 
 class MeshVolumeTopoProperty {
 public:
     MeshVolumeTopoProperty()
     {}

     ~MeshVolumeTopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     MeshVolumeTopoProperty* clone() {
         MeshVolumeTopoProperty* prop = new MeshVolumeTopoProperty();

         prop->m_blocks.insert(prop->m_blocks.end(), m_blocks.begin(), m_blocks.end());

         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// accesseur sur le conteneur des blocs
     std::vector<Block*>& getBlocks() {return m_blocks;}

     /*------------------------------------------------------------------------*/
     /** \brief   Suppression des relations
      */
     inline void clear()
     {
         m_blocks.clear();
     }

 private:
     /// lien sur les Topo::Block qui ont participés à la constitution des mailles
     std::vector<Block*> m_blocks;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_VOLUME_TOPO_PROPERTY_H_ */
