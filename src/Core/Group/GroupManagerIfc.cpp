/*----------------------------------------------------------------------------*/
/*
 * \file GroupManagerIfc.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
#include "Group/GroupManagerIfc.h"
#include "Group/Group3D.h"
#include "Internal/Context.h"

#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
GroupManagerIfc::GroupManagerIfc(const std::string& name, Internal::ContextIfc* c)
:Internal::CommandCreator(name, c)
{
}
/*----------------------------------------------------------------------------*/
GroupManagerIfc::~GroupManagerIfc()
{
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::clear()
{
    throw TkUtil::Exception ("GroupManagerIfc::clear should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string GroupManagerIfc::getInfos(const std::string& name, int dim) const
{
    throw TkUtil::Exception ("GroupManagerIfc::getInfos should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string>
GroupManagerIfc::getGeomEntities(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getGeomEntities should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getGeomVolumes(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getGeomVolumes should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getGeomSurfaces(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getGeomSurfaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getGeomCurves(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getGeomCurves should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getGeomVertices(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getGeomVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getTopoBlocks(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getTopoBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getTopoFaces(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getTopoFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getTopoEdges(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getTopoEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManagerIfc::getTopoVertices(const std::vector<std::string>& vg)
{
	throw TkUtil::Exception ("GroupManagerIfc::getTopoVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
 std::vector<std::string> GroupManagerIfc::getVisibles() const
 {
	 throw TkUtil::Exception ("GroupManagerIfc::getVisibles should be overloaded.");
 }
/*----------------------------------------------------------------------------*/
Group3D* GroupManagerIfc::getGroup3D(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup3D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Group2D* GroupManagerIfc::getGroup2D(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup2D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Group1D* GroupManagerIfc::getGroup1D(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup1D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Group0D* GroupManagerIfc::getGroup0D(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup0D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getGroup3D(std::vector<Group3D*>& grp, const bool onlyLive) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup3D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getGroup2D(std::vector<Group2D*>& grp, const bool onlyLive) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup2D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getGroup1D(std::vector<Group1D*>& grp, const bool onlyLive) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup1D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getGroup0D(std::vector<Group0D*>& grp, const bool onlyLive) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroup0D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getGroups(std::vector<GroupEntity*>& grp, Utils::SelectionManagerIfc::DIM, const bool onlyLive) const
{
	throw TkUtil::Exception ("GroupManagerIfc::getGroups should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getAddedShownAndHidden(
            const std::vector<Group::GroupEntity*>& groupAddedShown,
            const std::vector<Group::GroupEntity*>& groupAddedHidden,
            std::vector<Geom::GeomEntity*>& geomAddedShown,
            std::vector<Geom::GeomEntity*>& geomAddedHidden,
            std::vector<Topo::TopoEntity*>& topoAddedShown,
            std::vector<Topo::TopoEntity*>& topoAddedHidden,
            std::vector<Mesh::MeshEntity*>& meshAddedShown,
            std::vector<Mesh::MeshEntity*>& meshAddedHidden,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden)
{
	throw TkUtil::Exception ("GroupManagerIfc::getAddedShownAndHidden should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getAddedShownAndHidden(
            const Utils::FilterEntity::objectType newVisibilityMask,
            std::vector<Geom::GeomEntity*>& geomAddedShown,
            std::vector<Geom::GeomEntity*>& geomAddedHidden,
            std::vector<Topo::TopoEntity*>& topoAddedShown,
            std::vector<Topo::TopoEntity*>& topoAddedHidden,
            std::vector<Mesh::MeshEntity*>& meshAddedShown,
            std::vector<Mesh::MeshEntity*>& meshAddedHidden,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden)
{
	throw TkUtil::Exception ("GroupManagerIfc::getAddedShownAndHidden should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::getShownEntities(
            const Utils::FilterEntity::objectType visibilityMask, std::vector<Utils::Entity*>& entities)
{
	throw TkUtil::Exception ("GroupManagerIfc::getShownEntities should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addProjectionOnPX0(const std::string& nom)

{
	throw TkUtil::Exception ("GroupManagerIfc::addProjectionOnPX0 should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addProjectionOnPY0(const std::string& nom)

{
	throw TkUtil::Exception ("GroupManagerIfc::addProjectionOnPY0 should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addProjectionOnPZ0(const std::string& nom)

{
	throw TkUtil::Exception ("GroupManagerIfc::addProjectionOnPZ0 should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addCartesianPerturbation(const std::string& nom, PyObject* py_obj)
{
	throw TkUtil::Exception ("GroupManagerIfc::addCartesianPerturbation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addPolarPerturbation(const std::string& nom, PyObject* py_obj)
{
	throw TkUtil::Exception ("GroupManagerIfc::addPolarPerturbation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addSmoothing(const std::string& nom, Mesh::SurfacicSmoothing& sm)
{
	throw TkUtil::Exception ("GroupManagerIfc::addSmoothing surfacic should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addSmoothing(const std::string& nom, Mesh::VolumicSmoothing& sm)
{
	throw TkUtil::Exception ("GroupManagerIfc::addSmoothing volumic should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addSepa(const std::string& nom, Mesh::MeshModificationBySepa& ASepa)
{
	throw TkUtil::Exception ("GroupManagerIfc::addSepa should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
	throw TkUtil::Exception ("GroupManagerIfc::addToGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
	throw TkUtil::Exception ("GroupManagerIfc::removeFromGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
	throw TkUtil::Exception ("GroupManagerIfc::setGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::setLevel(std::vector<std::string>& vg, int dim, int level)
{
	throw TkUtil::Exception ("GroupManagerIfc::setLevel should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GroupManagerIfc::clearGroup(int dim, const std::string& groupName)
{
	throw TkUtil::Exception ("GroupManagerIfc::clearGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Geom::Volume*>& v)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Geom::Surface*>& v)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Geom::Curve*>& v)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Geom::Vertex*>& v)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Topo::Block*>& blocks)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Topo::CoFace*>& cofaces)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Topo::CoEdge*>& coedges)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Topo::Vertex*>& vertices)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Mesh::Volume*>& volumes)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Mesh::Surface*>& surfaces)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Mesh::Line*>& lines)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*------------------------------------------------------------------------*/
void GroupManagerIfc::get(const std::vector<std::string>& vg, std::vector<Mesh::Cloud*>& clouds)
{
	throw TkUtil::Exception ("GroupManagerIfc::get should be overloaded.");
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
