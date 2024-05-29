/*----------------------------------------------------------------------------*/
/** \file    MGOCCSurface.cpp
 *  \author  F. LEDOUX
 *  \date    03/12/15
 */
/*----------------------------------------------------------------------------*/
#include "MeshGemsPlugin/MGOCCSurface.h"
#include "MeshGemsPlugin/MGOCCManager.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/
MGOCCSurface::MGOCCSurface(const int AIndex, MGOCCManager* AMng)
:m_index(AIndex), m_manager(AMng)
{
	m_face = TopoDS::Face(m_manager->m_OCC_faces(m_index));
}

/*------------------------------------------------------------------------*/
void MGOCCSurface::project(gmds::math::Point& AP) const
{
	gp_Pnt pnt(AP.X(),AP.Y(),AP.Z());
	TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
	BRepExtrema_DistShapeShape extrema(V, m_face);
	bool isDone = extrema.IsDone();
	if(!isDone) {
		isDone = extrema.Perform();
	}

	if(!isDone){
		throw gmds::GMDSException("Fail to project a point onto a curve");
	}

	gp_Pnt pnt2 = extrema.PointOnShape2(1);
	AP.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::normal(const gmds::math::Point& AP, gmds::math::Vector& AV) const
{
	// projection pour en déduire les paramètres
	gp_Pnt pnt(AP.X(),AP.Y(),AP.Z());
	TopoDS_Vertex Vtx = BRepBuilderAPI_MakeVertex(pnt);
	BRepExtrema_DistShapeShape extrema(Vtx, m_face);
	bool isDone = extrema.IsDone();
	if(!isDone) {
		isDone = extrema.Perform();
	}

	if(!isDone){
		throw gmds::GMDSException("Fail to project a point onto a curve (to compute a normal)");
	}


	Standard_Real U, V;
	extrema.ParOnFaceS1(1, U, V);

	Handle(Geom_Surface) brepSurface = BRep_Tool::Surface(m_face);
	gp_Dir dir;
	BOPTools_AlgoTools3D::GetNormalToSurface (brepSurface, U, V, dir);

	AV[0] = dir.X();
	AV[1] = dir.Y();
	AV[2] = dir.Z();
}
/*------------------------------------------------------------------------*/
gmds::math::Point MGOCCSurface::closestPoint(const gmds::math::Point& AP) const
{
	gp_Pnt pnt(AP.X(),AP.Y(),AP.Z());
	TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
	BRepExtrema_DistShapeShape extrema(V, m_face);
	bool isDone = extrema.IsDone();
	if(!isDone) {
		isDone = extrema.Perform();
	}

	if(!isDone){
		throw gmds::GMDSException("Fail to project a point onto a curve");
	}

	gp_Pnt pnt2 = extrema.PointOnShape2(1);
	return gmds::math::Point(pnt2.X(), pnt2.Y(), pnt2.Z());

}
/*------------------------------------------------------------------------*/
void MGOCCSurface::bounds(gmds::TCoord& AUMin, gmds::TCoord& AUMax,
		gmds::TCoord& AVMin, gmds::TCoord& AVMax) const
{
	Handle_Geom_Surface surf = BRep_Tool::Surface(m_face);
	surf->Bounds(AUMin,AUMax,AVMin,AVMax);
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::d2(const gmds::TCoord& AU, const gmds::TCoord& AV,
		gmds::math::Point& AP,
		gmds::math::Vector& ADU,
		gmds::math::Vector& ADV,
		gmds::math::Vector& ADUU,
		gmds::math::Vector& ADUV,
		gmds::math::Vector& ADVV) const
{
	Handle_Geom_Surface surf = BRep_Tool::Surface(m_face);
	gp_Pnt pnt;
	gp_Vec du, dv, duu, dvv, duv;
	surf->D2(AU,AV,pnt,du,dv,duu,dvv,duv);

	//We get the location
	AP.setXYZ(pnt.X(),pnt.Y(),pnt.Z());

	//We get the first derivatives
	ADU[0] = du.X();
	ADU[1] = du.Y();
	ADU[2] = du.Z();

	ADV[0] = dv.X();
	ADV[1] = dv.Y();
	ADV[2] = dv.Z();

	//We get the second derivatives
	ADUU[0] = duu.X();
	ADUU[1] = duu.Y();
	ADUU[2] = duu.Z();

	ADVV[0] = dvv.X();
	ADVV[1] = dvv.Y();
	ADVV[2] = dvv.Z();

	ADUV[0] = duv.X();
	ADUV[1] = duv.Y();
	ADUV[2] = duv.Z();
}
/*------------------------------------------------------------------------*/
gmds::TCoord MGOCCSurface::area() const
{
	BRepCheck_Analyzer anaAna(m_face, Standard_False);

	GProp_GProps pb;
	BRepGProp::SurfaceProperties(m_face,pb);
	return  pb.Mass();
}
/*------------------------------------------------------------------------*/
MGOrientation MGOCCSurface::orientation() const
{
	MGOrientation value;
	switch(m_face.Orientation())
	{
	case(TopAbs_FORWARD):
			  value = MG_FORWARD;
	break;
	case(TopAbs_REVERSED):
			  value = MG_REVERSED;
	break;
	case(TopAbs_INTERNAL):
			  value = MG_INTERNAL;
	break;
	case(TopAbs_EXTERNAL):
			  value = MG_EXTERNAL;
	break;
	};
	return value;
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::get(std::vector<MGPoint*>& APnt) const
{
	APnt.clear();
	TopTools_IndexedMapOfShape  mapV;
	TopExp::MapShapes(m_face,TopAbs_VERTEX, mapV);
	for(unsigned int i=1 ; i <=mapV.Extent();i++)
	{
		TopoDS_Vertex vertex = TopoDS::Vertex(mapV.FindKey(i));
		int pnt_index = m_manager->m_OCC_vertices.FindIndex(vertex);
		APnt.push_back(m_manager->m_points[pnt_index-1]);
	}
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::get(std::vector<MGCurve*>& ACurves) const
{
	ACurves.clear();
	TopTools_IndexedMapOfShape  mapC;
	TopExp::MapShapes(m_face,TopAbs_EDGE, mapC);

	for(unsigned int i=1 ; i <=mapC.Extent();i++)
	{
		TopoDS_Edge edge = TopoDS::Edge(mapC.FindKey(i));
		int edge_index = m_manager->m_OCC_edges.FindIndex(edge);
		ACurves.push_back(m_manager->m_curves[edge_index-1]);
	}
//	TopExp_Explorer exp0;
//	for(exp0.Init(m_face, TopAbs_EDGE); exp0.More(); exp0.Next()){
//		TopoDS_Edge edge = TopoDS::Edge(exp0.Current());
//		int edge_index = m_manager->m_OCC_edges.FindIndex(edge);
//		m_manager->newCurve(edge);
//		std::cout<<"CREATE A CURVE "<<m_manager->m_curves.size()-1<<std::endl;
//		ACurves.push_back(m_manager->m_curves.back());
//	}
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::get(std::vector<MGSurface*>& APnt) const
{
	throw gmds::GMDSException("Surface->Surface connectivity is not available");
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::get(std::vector<MGVolume*>& AVol) const
{
	AVol.clear();
	for(unsigned int i=1; i<=m_manager->m_OCC_solids.Extent();i++)
	{
		TopoDS_Solid si = TopoDS::Solid(m_manager->m_OCC_solids(i));
		TopExp_Explorer exp;
		bool found_vol = false;
		for(exp.Init(si, TopAbs_FACE); exp.More() && !found_vol; exp.Next())
		{
			TopoDS_Face current_face = TopoDS::Face(exp.Current());
			int current_index = m_manager->m_OCC_faces.FindIndex(current_face);
			if(m_index==current_index)
			{
				int vol_index = m_manager->m_OCC_solids.FindIndex(si);
				AVol.push_back( m_manager->m_volumes[vol_index-1]);
				found_vol = true;
			}
		}
	}
}
/*------------------------------------------------------------------------*/
void MGOCCSurface::computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const
{
	Bnd_Box box;
	//box.SetGap(tol);
	BRepBndLib::Add(m_face,box);
	box.Get(minXYZ[0], minXYZ[1], minXYZ[2],
			maxXYZ[0], maxXYZ[1], maxXYZ[2]);
}
/*------------------------------------------------------------------------*/
int MGOCCSurface::getId() const
{
	return m_index;
}
/*------------------------------------------------------------------------*/
TopoDS_Face MGOCCSurface::getOCCFace() const
{
	return m_face;
}
/*--------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
