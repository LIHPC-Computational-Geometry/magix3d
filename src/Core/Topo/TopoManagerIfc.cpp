/*----------------------------------------------------------------------------*/
/** \file TopoManagerIfc.cpp
 *
 *  \author Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 01/02/2012
 *
 *  Modified on: 23/02/2022
 *      Author: Simon C
 *      ajout de la fonction de création de blocs par extrusion de faces topologiques
 */
/*----------------------------------------------------------------------------*/
#include "Topo/TopoManagerIfc.h"
#include "Utils/Common.h"
#include "Utils/Point.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
TopoManagerIfc::TopoManagerIfc(const std::string& name, Internal::ContextIfc* c)
	: Internal::CommandCreator(name, c)
{
}
/*----------------------------------------------------------------------------*/
TopoManagerIfc::TopoManagerIfc(const TopoManagerIfc& tm)
	: Internal::CommandCreator(tm)
{
	MGX_FORBIDDEN ("TopoManagerIfc copy constructor is not allowed.");
}
/*----------------------------------------------------------------------------*/
TopoManagerIfc& TopoManagerIfc::operator = (const TopoManagerIfc& tm)
{
	MGX_FORBIDDEN ("TopoManagerIfc assignment operator is not allowed.");
	return *this;
}
/*----------------------------------------------------------------------------*/
TopoManagerIfc::~TopoManagerIfc ( )
{
}
/*----------------------------------------------------------------------------*/
void TopoManagerIfc::clear ( )
{
	throw TkUtil::Exception ("TopoManagerIfc::clear should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newUnstructuredTopoOnGeometry(std::string ne)
{
	throw TkUtil::Exception ("TopoManagerIfc::newUnstructuredTopoOnGeometry should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newTopoOnGeometry(std::string ne)
{
	throw TkUtil::Exception ("TopoManagerIfc::newTopoOnGeometry should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newStructuredTopoOnGeometry(std::string ne)
{
	throw TkUtil::Exception ("TopoManagerIfc::newStructuredTopoOnGeometry should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newStructuredTopoOnSurface(std::string ne, std::vector<std::string>& ve)
{
	throw TkUtil::Exception ("TopoManagerIfc::newStructuredTopoOnSurface should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newFreeTopoOnGeometry(std::string ne)
{
	throw TkUtil::Exception ("TopoManagerIfc::newFreeTopoOnGeometry should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newFreeTopoInGroup(std::string ne, int dim)
{
	throw TkUtil::Exception ("TopoManagerIfc::newFreeTopoInGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newInsertionTopoOnGeometry(std::string ne)
{
	throw TkUtil::Exception ("TopoManagerIfc::newInsertionTopoOnGeometry should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newBoxWithTopo(
				const Utils::Math::Point& pmin, const Utils::Math::Point& pmax, bool meshStructured, std::string groupName)
{
	throw TkUtil::Exception ("TopoManagerIfc::newBoxWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newBoxWithTopo(
				const Utils::Math::Point& pmin, const Utils::Math::Point& pmax, const int ni, const int nj, const int nk, std::string groupName)
{
	throw TkUtil::Exception ("TopoManagerIfc::newBoxWithTopo (with ni nj nk) should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newCylinderWithTopo(const Point& pcentre, const double& dr,
                                 const Vector& dv, const double& da,
                                 bool meshStructured, const double& rat,
                                 const int naxe, const int ni, const int nr,
                                 std::string groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::newCylinderWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newHollowCylinderWithTopo(const Point& pcentre,
		                         const double& dr_int, const double& dr_ext,
                                 const Vector& dv, const double& da,
                                 bool meshStructured,
                                 const int naxe, const int ni, const int nr,
                                 std::string groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::newHollowCylinderWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newConeWithTopo(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da,
			bool meshStructured,  const double& rat,
            const int naxe, const int ni, const int nr,
			std::string groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::newConeWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newSphereWithTopo(const Point& pcentre,  const double& radius,
            const  Utils::Portion::Type& dt,
            bool meshStructured, const double& rat,
            const int ni, const int nr,
            std::string groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::newSphereWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newHollowSphereWithTopo(const Point& pcentre,  const double& r_int, const double& r_ext,
            const  Utils::Portion::Type& dt,
            bool meshStructured,
            const int ni, const int nr,
            std::string groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::newHollowSphereWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newSpherePartWithTopo(const double& radius,
		const double& angleY,
		const double& angleZ,
		const int ni,
		const int nj,
		const int nr,
		std::string groupName)
{
	throw TkUtil::Exception ("TopoManagerIfc::newSpherePartWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newHollowSpherePartWithTopo(const double& dr_int,
		const double& dr_ext,
		const double& angleY,
		const double& angleZ,
		const int ni,
		const int nj,
		const int nr,
		std::string groupName)
{
	throw TkUtil::Exception ("TopoManagerIfc::newHollowSpherePartWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newIJBoxesWithTopo(int ni, int nj, bool alternateStruture)
{
	throw TkUtil::Exception ("TopoManagerIfc::newIJBoxesWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newIJKBoxesWithTopo(int ni, int nj, int nk, bool alternateStruture)
{
    throw TkUtil::Exception ("TopoManagerIfc::newIJKBoxesWithTopo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::newTopoOGridOnGeometry(std::string ne, const double& rat)
{
	throw TkUtil::Exception ("TopoManagerIfc::newTopoOGridOnGeometry should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void TopoManagerIfc::destroy(std::vector<std::string>& ve, bool propagate)
{
    throw TkUtil::Exception ("TopoManagerIfc::destroy should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::copy(std::vector<std::string>& vb, std::string vo)
{
    throw TkUtil::Exception ("TopoManagerIfc::copy should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::extract(std::vector<std::string>& vb, const std::string ng)
{
	throw TkUtil::Exception ("TopoManagerIfc::extract should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::insertHole(std::vector<std::string>& faces)
{
    throw TkUtil::Exception ("TopoManagerIfc::insertHole should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuse2Edges(std::string na, std::string nb)
{
	throw TkUtil::Exception ("TopoManagerIfc::fuse2Edges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuse2EdgeList(std::vector<std::string>& coedge_names1, std::vector<std::string>& coedge_names2)
{
	throw TkUtil::Exception ("TopoManagerIfc::fuse2EdgeList should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuse2Faces(std::string na, std::string nb)
{
	throw TkUtil::Exception ("TopoManagerIfc::fuse2Faces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuse2FaceList(std::vector<std::string>& coface_names1, std::vector<std::string>& coface_names2)
{
	throw TkUtil::Exception ("TopoManagerIfc::fuse2FaceList should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::glue2Blocks(std::string na, std::string nb)
{
	throw TkUtil::Exception ("TopoManagerIfc::glue2Blocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc*
TopoManagerIfc::glue2Topo(std::string volName1, std::string volName2)
{
    throw TkUtil::Exception ("TopoManagerIfc::glue2Topo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuse2Vertices(std::string na, std::string nb)
{
	throw TkUtil::Exception ("TopoManagerIfc::fuse2Vertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitBlock(std::string nbloc, std::string narete, const double& ratio)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitBlock should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const double& ratio)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const Point& pt)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitAllBlocks(std::string narete, const double& ratio)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitAllBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitAllBlocks(std::string narete, const Point& pt)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitAllBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::extendSplitBlock(std::string nbloc, std::string narete)
{
    throw TkUtil::Exception ("TopoManagerIfc::extendSplitBlock should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitBlocksWithOgrid(std::vector<std::string> &blocs_names,
        std::vector<std::string> &cofaces_names,
        const double& ratio, int nb_bras)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitBlocksWithOgrid should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitBlocksWithOgridV2(std::vector<std::string> &blocs_names,
        std::vector<std::string> &cofaces_names,
        const double& ratio, int nb_bras)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitBlocksWithOgridV2 should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitBlocksWithOgrid_old(std::vector<std::string> &blocs_names,
        std::vector<std::string> &cofaces_names,
        const double& ratio, int nb_bras)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitBlocksWithOgrid_old should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitFacesWithOgrid(std::vector<std::string> &faces_names,
        std::vector<std::string> &coedges_names,
        const double& ratio, int nb_bras)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitFacesWithOgrid should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const double& ratio_dec, const double& ratio_ogrid)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const Point& pt, const double& ratio_ogrid)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitAllFaces(std::string narete, const double& ratio_dec, const double& ratio_ogrid)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitAllFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitAllFaces(std::string narete, const Point& pt, const double& ratio_ogrid)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitAllFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitFace(std::string coface_name, std::string narete, const double& ratio_dec, bool project_on_meshing_edges)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitFace should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitFace(std::string coface_name, std::string narete, const Point& pt, bool project_on_meshing_edges)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitFace should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::extendSplitFace(std::string coface_name, std::string nsommet)
{
    throw TkUtil::Exception ("TopoManagerIfc::extendSplitFace should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitEdge(std::string coedge_name, const double& ratio_dec)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitEdge should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::splitEdge(std::string coedge_name, const Point& pt)
{
    throw TkUtil::Exception ("TopoManagerIfc::splitEdge should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::snapVertices(std::string nom1, std::string nom2, bool project_on_first)
{
    throw TkUtil::Exception ("TopoManagerIfc::snapVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::makeBlocksByRevol(std::vector<std::string> &coedges_names,  const  Utils::Portion::Type& dt)
{
    throw TkUtil::Exception ("TopoManagerIfc::makeBlocksByRevol should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::makeBlocksByRevolWithRatioOgrid(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt, const double& ratio_ogrid)
{
    throw TkUtil::Exception ("TopoManagerIfc::makeBlocksByRevolWithRatioOgrid should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::makeBlocksByExtrude(std::vector<std::string> &cofaces_names,  const  Utils::Math::Vector& dv)
{
    throw TkUtil::Exception ("TopoManagerIfc::makeBlocksByExtrude should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<std::string> &edge_names)
{
	throw TkUtil::Exception ("TopoManagerIfc::setMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
    TopoManagerIfc::setAllMeshingProperty(CoEdgeMeshingProperty& emp)
{
	throw TkUtil::Exception ("TopoManagerIfc::setAllMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
    TopoManagerIfc::setParallelMeshingProperty(CoEdgeMeshingProperty& emp, std::string coedge_name)
{
	throw TkUtil::Exception ("TopoManagerIfc::setParallelMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
    TopoManagerIfc::reverseDirection(std::vector<std::string> &edge_names)
{
	throw TkUtil::Exception ("TopoManagerIfc::reverseDirection should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<std::string> &face_names)
{
	throw TkUtil::Exception ("TopoManagerIfc::setMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
    TopoManagerIfc::setAllMeshingProperty(CoFaceMeshingProperty& emp)
{
	throw TkUtil::Exception ("TopoManagerIfc::setAllMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setMeshingProperty(BlockMeshingProperty& emp, std::vector<std::string> &bloc_names)
{
	throw TkUtil::Exception ("TopoManagerIfc::setMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
    TopoManagerIfc::setAllMeshingProperty(BlockMeshingProperty& emp)
{
	throw TkUtil::Exception ("TopoManagerIfc::setAllMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, std::string ed)
{
	throw TkUtil::Exception ("TopoManagerIfc::setEdgeMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty&
TopoManagerIfc::getEdgeMeshingProperty(std::string ed)
{
	throw TkUtil::Exception ("TopoManagerIfc::getEdgeMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setFaceMeshingProperty(CoFaceMeshingProperty& emp, std::string cf)
{
    throw TkUtil::Exception ("TopoManagerIfc::setFaceMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setBlockMeshingProperty(BlockMeshingProperty& emp, std::string bl)
{
    throw TkUtil::Exception ("TopoManagerIfc::setBlockMeshingProperty should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::replaceTransfiniteByDirectionalMeshMethodAsPossible()
{
    throw TkUtil::Exception ("TopoManagerIfc::replaceTransfiniteByDirectionalMeshMethodAsPossible should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setNbMeshingEdges(std::string edge, int nb, std::vector<std::string>& frozed_edges_names)
{
    throw TkUtil::Exception ("TopoManagerIfc::setNbMeshingEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::unrefine(std::string nbloc, std::string narete, int ratio)
{
    throw TkUtil::Exception ("TopoManagerIfc::unrefine should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::refine(int ratio)
{
	throw TkUtil::Exception ("TopoManagerIfc::refine should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::importMDL(std::string n, const bool all, const bool useAreaName, std::string prefixName, int deg_min, int deg_max)
{
    throw TkUtil::Exception ("TopoManagerIfc::importMDL should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::vector<std::string> > TopoManagerIfc::getFusableEdges()
{
    throw TkUtil::Exception ("TopoManagerIfc::getFusableEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getInvalidEdges() const
{
	throw TkUtil::Exception ("TopoManagerIfc::getInvalidEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setGeomAssociation(std::vector<std::string> & topo_entities_names,
		std::string geom_entity_name,
		bool move_vertices)
{
    throw TkUtil::Exception ("TopoManagerIfc::setGeomAssociation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::projectVerticesOnNearestGeomEntities(std::vector<std::string> & vertices_names, std::vector<std::string> & geom_entities_names, bool move_vertices)
{
	 throw TkUtil::Exception ("TopoManagerIfc::projectVerticesOnNearestGeomEntities should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::projectEdgesOnCurves(std::vector<std::string> & coedges_names)
{
	throw TkUtil::Exception ("TopoManagerIfc::projectEdgesOnCurves should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::projectAllEdgesOnCurves()
{
	throw TkUtil::Exception ("TopoManagerIfc::projectAllEdgesOnCurves should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::projectFacesOnSurfaces(std::vector<std::string> & cofaces_names)
{
	throw TkUtil::Exception ("TopoManagerIfc::projectFacesOnSurfaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::projectAllFacesOnSurfaces()
{
	throw TkUtil::Exception ("TopoManagerIfc::projectAllFacesOnSurfaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::translate(std::vector<std::string>& ve, const Vector& dp,
        const bool withGeom)
{
    throw TkUtil::Exception ("TopoManagerIfc::translate should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::rotate(std::vector<std::string>& ve,
		const Utils::Math::Rotation& rot,
        const bool withGeom)
{
    throw TkUtil::Exception ("TopoManagerIfc::rotate should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::scale(std::vector<std::string>& ve,
            const double& facteur,
            const bool withGeom)
{
    throw TkUtil::Exception ("TopoManagerIfc::scale should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::scale(std::vector<std::string>& ve,
            const double& facteur,
            const Point& pcentre,
            const bool withGeom)
{
    throw TkUtil::Exception ("TopoManagerIfc::scale should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::scale(std::vector<std::string>& ve,
		const double factorX,
		const double factorY,
		const double factorZ,
        const bool withGeom)
{
    throw TkUtil::Exception ("TopoManagerIfc::scale should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::mirror(std::vector<std::string>& ve,
        		Utils::Math::Plane* plane,
        		const bool withGeom)
{
	throw TkUtil::Exception ("TopoManagerIfc::mirror should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setVertexLocation(std::vector<std::string>& vertices_names,
            const bool changeX,
            const double& xPos,
            const bool changeY,
            const double& yPos,
            const bool changeZ,
            const double& zPos,
			std::string sysCoordName)
{
    throw TkUtil::Exception ("TopoManagerIfc::setVertexLocation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setVertexSphericalLocation(std::vector<std::string>& vertices_names,
            const bool changeRho,
            const double& rhoPos,
            const bool changeTheta,
            const double& thetaPos,
            const bool changePhi,
            const double& phiPos,
			std::string sysCoordName)
{
    throw TkUtil::Exception ("TopoManagerIfc::setVertexSphericalLocation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setVertexCylindricalLocation(std::vector<std::string>& vertices_names,
            const bool changeRho,
            const double& rhoPos,
            const bool changePhi,
            const double& phiPos,
			const bool changeZ,
			const double& zPos,
			std::string sysCoordName)
{
    throw TkUtil::Exception ("TopoManagerIfc::setVertexCylindricalLocation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setVertexSameLocation(std::string vertex_name,
			std::string target_name)
{
    throw TkUtil::Exception ("TopoManagerIfc::setVertexSameLocation should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::snapProjectedVertices(std::vector<std::string> &vertices_names)
{
    throw TkUtil::Exception ("TopoManagerIfc::snapProjectedVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::snapAllProjectedVertices()
{
    throw TkUtil::Exception ("TopoManagerIfc::snapAllProjectedVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::alignVertices(std::vector<std::string> &vertices_names)
{
    throw TkUtil::Exception ("TopoManagerIfc::alignVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::alignVertices(const Point& p1, const Point& p2, std::vector<std::string> &vertices_names)
{
    throw TkUtil::Exception ("TopoManagerIfc::alignVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuseEdges(std::vector<std::string> &coedges_names)
{
    throw TkUtil::Exception ("TopoManagerIfc::fuseEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::fuse2Blocks(std::string na, std::string nb)
{
	throw TkUtil::Exception ("TopoManagerIfc::fuse2Blocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int TopoManagerIfc::getNbBlocks () const
{
	throw TkUtil::Exception ("TopoManagerIfc::getNbBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int TopoManagerIfc::getNbFaces () const
{
	throw TkUtil::Exception ("TopoManagerIfc::getNbFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo TopoManagerIfc::getInfos(const std::string& name, int dim) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getInfos should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::Math::Point TopoManagerIfc::getCoord(const std::string& name) const
{
	 throw TkUtil::Exception ("TopoManagerIfc::getCoord should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string TopoManagerIfc::getDirOnBlock(const std::string& aname, const std::string& bname) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getDirOnBlock should be overloaded.");
}
/*------------------------------------------------------------------------*/
int TopoManagerIfc::getNbMeshingEdges(const std::string& name) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getNbMeshingEdges should be overloaded.");
}
/*------------------------------------------------------------------------*/
int TopoManagerIfc::getDefaultNbMeshingEdges()
{
    throw TkUtil::Exception ("TopoManagerIfc::getDefaultNbMeshingEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManagerIfc::setDefaultNbMeshingEdges(int nb)
{
    throw TkUtil::Exception ("TopoManagerIfc::setDefaultNbMeshingEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getBorderFaces() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getBorderFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getFacesWithoutBlock() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getFacesWithoutBlock should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getSemiConformalFaces() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getSemiConformalFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getInvalidFaces() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getInvalidFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getUnstructuredFaces() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getUnstructuredFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getTransfiniteMeshLawFaces() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getTransfiniteMeshLawFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getInvalidBlocks() const
{
	throw TkUtil::Exception ("TopoManagerIfc::getInvalidBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getVisibleBlocks() const
{
	throw TkUtil::Exception ("TopoManagerIfc::getVisibleBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getUnstructuredBlocks() const
{
	throw TkUtil::Exception ("TopoManagerIfc::getUnstructuredBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getTransfiniteMeshLawBlocks() const
{
	throw TkUtil::Exception ("TopoManagerIfc::getTransfiniteMeshLawBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManagerIfc::addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::addToGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManagerIfc::removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::removeFromGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManagerIfc::setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    throw TkUtil::Exception ("TopoManagerIfc::setGroup should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string>
TopoManagerIfc::getBlocks() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void
TopoManagerIfc::getBlocks(std::vector<Topo::Block* >& blocks, bool sort) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string>
TopoManagerIfc::getCoFaces() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getCoFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void
TopoManagerIfc::getCoFaces(std::vector<Topo::CoFace* >& faces) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getCoFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string>
TopoManagerIfc::getCoEdges() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getCoEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void
TopoManagerIfc::getCoEdges(std::vector<Topo::CoEdge* >& edges) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getCoEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string>
TopoManagerIfc::getVertices() const
{
    throw TkUtil::Exception ("TopoManagerIfc::getVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void
TopoManagerIfc::getVertices(std::vector<Topo::Vertex* >& vertices) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getVertices should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Block*
TopoManagerIfc::getBlock(const std::string& name, const bool exceptionIfNotFound) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getBlocks should be overloaded.");
}
/*----------------------------------------------------------------------------*/
CoFace*
TopoManagerIfc::getCoFace(const std::string& name, const bool exceptionIfNotFound) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getCoFace should be overloaded.");
}
/*----------------------------------------------------------------------------*/
CoEdge*
TopoManagerIfc::getCoEdge(const std::string& name, const bool exceptionIfNotFound) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getCoEdge should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Vertex*
TopoManagerIfc::getVertex(const std::string& name, const bool exceptionIfNotFound) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getVertex should be overloaded.");
}
/*----------------------------------------------------------------------------*/
TopoEntity*
TopoManagerIfc::getEntity(const std::string& name, const bool exceptionIfNotFound) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getEntity should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string TopoManagerIfc::getVertexAt(const Point& pt1) const
{
    throw TkUtil::Exception ("TopoManagerIfc::getVertexAt should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string TopoManagerIfc::getEdgeAt(const Point& pt1, const Point& pt2) const
{
	throw TkUtil::Exception ("TopoManagerIfc::getEdgeAt should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string TopoManagerIfc::getFaceAt(std::vector<Point>& pts) const
{
	throw TkUtil::Exception ("TopoManagerIfc::getFaceAt should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string TopoManagerIfc::getBlockAt(std::vector<Point>& pts) const
{
	throw TkUtil::Exception ("TopoManagerIfc::getBlockAt should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManagerIfc::getCommonEdges(const std::string& face1, const std::string& face2, int dim) const
{
	throw TkUtil::Exception ("TopoManagerIfc::getCommonEdges should be overloaded.");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
