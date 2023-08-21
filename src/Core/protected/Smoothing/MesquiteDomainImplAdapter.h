/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteDomainImplAdapter.h
 *
 *  \author Nicolas Le Goff
 *
 *  \date 21 mar. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_MESQUITEDOMAINIMPLADAPTER_H_
#define MGX3D_MESH_MESQUITEDOMAINIMPLADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshManager.h"
#include "Mesh/MeshItf.h"
#include "Topo/TopoManager.h"
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
// Mesquite
#include <MeshInterface.hpp>
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/IGMesh.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** @brief Classe pont entre une surface Magix3D et Mesquite
*/
class MesquiteDomainImplAdapter : public MESQUITE_NS::MeshDomain
{
public:
	MesquiteDomainImplAdapter(
			Mgx3D::Mesh::MeshManager& AMeshManager,
			Mgx3D::Topo::TopoManager& ATopoManager,
			std::map<Mesquite::Mesh::VertexHandle, gmds::Node>& AMesquite2GMDSNodes,
			std::map<Mesquite::Mesh::ElementHandle,gmds::Face>& AMesquite2GMDSCells2D,
			std::map<Mesquite::Mesh::ElementHandle,gmds::Region>& AMesquite2GMDSCells3D
			);
  ~MesquiteDomainImplAdapter();

  /** Modifies "coordinate" so that it lies on the
   domain to which "entity_handle" is constrained.
   The handle determines the domain.  The coordinate
   is the proposed new position on that domain.
   */
  virtual void snap_to(Mesquite::Mesh::EntityHandle,
		  MESQUITE_NS::Vector3D &coordinate) const ;

  /** Returns the normal of the domain to which
   "entity_handle" is constrained.  For non-planar surfaces,
   the normal is calculated at the point on the domain that
   is closest to the passed in value of "coordinate".  If the
   domain does not have a normal, or the normal cannot
   be determined, "coordinate" is set to (0,0,0).  Otherwise,
   "coordinate" is set to the domain's normal at the
   appropriate point.
   In summary, the handle determines the domain.  The coordinate
   determines the point of interest on that domain.

   User should see also PatchData::get_domain_normal_at_vertex and
   PatchData::get_domain_normal_at_element .
   */
  virtual void vertex_normal_at(Mesquite::Mesh::EntityHandle,
		  MESQUITE_NS::Vector3D &coordinate) const ;

  virtual void element_normal_at(Mesquite::Mesh::ElementHandle entity_handle,
		  MESQUITE_NS::Vector3D &coordinate) const;

  /**\brief evaluate surface normals
   *
   * Returns normals for a domain.
   *
   *\param handles       The domain evaluated is the one in which
   *                     this mesh entity is constrained.
   *\param coordinates   As input, a list of positions at which to
   *                     evaluate the domain.  As output, the resulting
   *                     domain normals.
   *\param count         The length of the coordinates array.
   *\param err           Erreur en retour
   */
  virtual void vertex_normal_at( const MESQUITE_NS::Mesh::EntityHandle  * handles,
		  MESQUITE_NS::Vector3D coordinates[],
			  unsigned int count,
			  MESQUITE_NS::MsqError& err ) const;

  /**\brief evaluate closest point and normal
   *
   * Given a position in space, return the closest
   * position in the domain and the domain normal
   * at that point.
   *
   *\param handle        Evaluate the subset of the domain contianing
   *                     this entity
   *\param position      Input position for which to evaluate
   *\param closest       Closest position in the domain.
   *\param normal        Domain normal at the location of 'closest'
   *\param err           Erreur en retour
   */
  virtual void closest_point( MESQUITE_NS::Mesh::EntityHandle handle,
			      const MESQUITE_NS::Vector3D& position,
			      MESQUITE_NS::Vector3D& closest,
			      MESQUITE_NS::Vector3D& normal,
			      MESQUITE_NS::MsqError& err ) const;

  /**\brief Get degrees of freedom in vertex movement.
   *
   * Given a vertex, return how the domain constrains the
   * location of that vertex as the number of degrees of
   * freedom in the motion of the vertex.  If the domain
   * is a geometric domain, the degrees of freedom for a
   * vertex is the dimension of the geometric entity the
   * vertex is constrained to lie on (e.g. point = 0, curve = 1,
   * surface = 2, volume = 3.)
   */
  virtual void domain_DoF( const MESQUITE_NS::Mesh::EntityHandle* handle_array,
			   unsigned short* dof_array,
			   size_t num_handles,
			   MESQUITE_NS::MsqError& err ) const;

private:

	Mgx3D::Mesh::MeshManager& m_meshManager;
	Mgx3D::Topo::TopoManager& m_topoManager;


	std::map<Mesquite::Mesh::VertexHandle, gmds::Node>& m_mesquite2GMDSNodes;
    std::map<Mesquite::Mesh::ElementHandle,gmds::Face>& m_mesquite2GMDSCells2D;
    std::map<Mesquite::Mesh::ElementHandle,gmds::Region>& m_mesquite2GMDSCells3D;

	std::map<gmds::TCellID, Mgx3D::Geom::GeomEntity*> m_nodes2GeomEntity;
	std::map<gmds::TCellID, bool> m_nodesIsOnSurface;
	std::map<gmds::TCellID, bool> m_nodesIsOnCurve;
	std::map<gmds::TCellID, bool> m_nodesIsOnVertex;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_MESQUITEDOMAINIMPLADAPTER_H_ */
