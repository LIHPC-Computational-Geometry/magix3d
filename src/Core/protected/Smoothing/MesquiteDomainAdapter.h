/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteDomainAdapter.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 févr. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MESQUITEDOMAINADAPTER_H_
#define MESQUITEDOMAINADAPTER_H_
/*----------------------------------------------------------------------------*/
// Mesquite
#include <MeshInterface.hpp>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Surface;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** @brief Classe pont entre une surface Magix3D et Mesquite
*/
class MesquiteDomainAdapter : public Mesquite2::MeshDomain
{
public:
	MesquiteDomainAdapter(Geom::Surface *surface);
  ~MesquiteDomainAdapter();

  /** Modifies "coordinate" so that it lies on the
   domain to which "entity_handle" is constrained.
   The handle determines the domain.  The coordinate
   is the proposed new position on that domain.
   */
  virtual void snap_to(Mesquite2::Mesh::EntityHandle,
		  Mesquite2::Vector3D &coordinate) const ;

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
  virtual void vertex_normal_at(Mesquite2::Mesh::EntityHandle,
		  Mesquite2::Vector3D &coordinate) const ;

  virtual void element_normal_at(Mesquite2::Mesh::ElementHandle entity_handle,
		  Mesquite2::Vector3D &coordinate) const;

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
  virtual void vertex_normal_at( const Mesquite2::Mesh::EntityHandle  * handles,
		  Mesquite2::Vector3D coordinates[],
			  unsigned int count,
			  Mesquite2::MsqError& err ) const;

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
  virtual void closest_point( Mesquite2::Mesh::EntityHandle handle,
			      const Mesquite2::Vector3D& position,
			      Mesquite2::Vector3D& closest,
			      Mesquite2::Vector3D& normal,
			      Mesquite2::MsqError& err ) const;

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
  virtual void domain_DoF( const Mesquite2::Mesh::EntityHandle* handle_array,
			   unsigned short* dof_array,
			   size_t num_handles,
			   Mesquite2::MsqError& err ) const;

private:
  /// surface sur laquelle se fait le lissage
  Geom::Surface *m_surface;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESQUITEDOMAINADAPTER_H_ */
