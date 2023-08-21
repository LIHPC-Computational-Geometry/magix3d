/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteMeshImplAdapter.h
 *
 *  \author Nicolas Le Goff
 *
 *  \date 21 mar. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_MESQUITEMESHIMPLADAPTER_H_
#define MGX3D_MESH_MESQUITEMESHIMPLADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshManager.h"
#include "Mesh/MeshItf.h"
#include "Geom/GeomManager.h"
/*----------------------------------------------------------------------------*/
//#include <vector>
#include <map>
//#include <sys/types.h> // pour uint
/*----------------------------------------------------------------------------*/
//namespace gmds{
//class Node;
////class Face;
//}
#include "GMDS/IG/IGMesh.h"
/*----------------------------------------------------------------------------*/
// Mesquite
#include <MeshImpl.hpp>
//#include <ParallelMeshImpl.hpp>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
  /** @brief Classe pont entre structure de maillage Gmds dans Magix3D et structure Mesquite.
  */
class MesquiteMeshImplAdapter : public MESQUITE_NS::MeshImpl
{
public:
	/// passage de Gmds vers Mesquite
	MesquiteMeshImplAdapter(
			Mgx3D::Mesh::MeshManager& AMeshManager,
			Mgx3D::Geom::GeomManager& AGeomManager);

	///
	virtual ~MesquiteMeshImplAdapter();

	std::map<MESQUITE_NS::Mesh::VertexHandle,gmds::Node>* getMesquite2GMDSNodes();

	std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Face>* getMesquite2GMDSCells2D();
	std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Region>* getMesquite2GMDSCells3D();



private:

	Mgx3D::Mesh::MeshManager& m_meshManager;
	Mgx3D::Geom::GeomManager& m_geomManager;

	std::map<MESQUITE_NS::Mesh::VertexHandle,gmds::Node> m_mesquite2GMDSNodes;
	std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Face> m_mesquite2GMDSCells2D;
	std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Region> m_mesquite2GMDSCells3D;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_MESQUITEMESHIMPLADAPTER_H_ */
