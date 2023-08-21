/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteDomainAdapter.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 févr. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Smoothing/MesquiteDomainAdapter.h"
#include "Geom/Surface.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"

#include <Vector3D.hpp>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
//#define _DEBUG2
/*----------------------------------------------------------------------------*/
MesquiteDomainAdapter::MesquiteDomainAdapter(Geom::Surface *surface)
: m_surface(surface)
{
}
/*----------------------------------------------------------------------------*/
MesquiteDomainAdapter::~MesquiteDomainAdapter()
{
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainAdapter::snap_to (Mesquite2::Mesh::EntityHandle, Mesquite2::Vector3D & coordinate) const
{
  Utils::Math::Point pt (coordinate.x(),coordinate.y(),coordinate.z());
  Utils::Math::Point proj;
  m_surface->project(pt, proj);
#ifdef _DEBUG2
  std::cout << "snap_to : "<< pt << " => " << proj << " distance "<<(pt-proj).norme()<<std::endl;
#endif
  coordinate.set(proj.getX(), proj.getY(), proj.getZ());
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainAdapter::vertex_normal_at(Mesquite2::Mesh::EntityHandle,
		Mesquite2::Vector3D &coordinate) const
{
  Utils::Math::Point pt (coordinate.x(),coordinate.y(),coordinate.z());
  Utils::Math::Vector proj;
  m_surface->normal(pt, proj);
#ifdef _DEBUG2
  std::cout << "normal_at : "<< pt << " => " << proj << std::endl;
#endif
  coordinate.set(proj.getX(), proj.getY(), proj.getZ());
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainAdapter::element_normal_at(Mesquite2::Mesh::ElementHandle entity_handle,
		Mesquite2::Vector3D &coordinate) const
{
	MGX_NOT_YET_IMPLEMENTED("element_normal_at non prévue")
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainAdapter::vertex_normal_at( const Mesquite2::Mesh::EntityHandle * m,
		Mesquite2::Vector3D coords[],
		unsigned count,
		Mesquite2::MsqError& ) const
{
#ifdef _DEBUG2
  std::cout << "normal_at pour count = " << count << " BEGIN\n";
#endif
  for (unsigned i = 0; i < count; ++i) {
	  vertex_normal_at(*m, coords[i]);
  }
#ifdef _DEBUG2
  std::cout << "normal_at pour count = " << count << " END\n";
#endif
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainAdapter::closest_point( Mesquite2::Mesh::EntityHandle mesh_entity,
				       const Mesquite2::Vector3D& position,
				       Mesquite2::Vector3D& closest,
				       Mesquite2::Vector3D& normal,
				       Mesquite2::MsqError& ) const
{
#ifdef _DEBUG2
  std::cout << "closest_point\n";
#endif
  closest = position;
  normal = position;

  snap_to(mesh_entity, closest);
  vertex_normal_at(mesh_entity, normal);
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainAdapter::domain_DoF( const Mesquite2::Mesh::EntityHandle* handle_array,
		unsigned short* dof_array,
		size_t num_vertices,
		Mesquite2::MsqError& err ) const
{
#ifdef _DEBUG2
  std::cout << "domain_DoF\n";
#endif
  std::fill( dof_array, dof_array + num_vertices, 2 );
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
