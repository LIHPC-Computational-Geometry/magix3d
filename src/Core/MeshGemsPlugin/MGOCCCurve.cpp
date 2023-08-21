/*----------------------------------------------------------------------------*/
/** \file    OCCCurve.cpp
 *  \author  F. LEDOUX
 *  \date    03/12/15
 */
/*----------------------------------------------------------------------------*/
// GMDS File Headers
#include "MeshGemsPlugin/MGOCCCurve.h"
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
#include <Geom2d_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/
MGOCCCurve::MGOCCCurve(const int AIndex, const int AI2, MGOCCManager* AMng)
:m_index(AIndex), m_index2(AI2),m_manager(AMng), m_first_pnt(0), m_second_pnt(0)
{
	//m_edge = TopoDS::Edge(m_manager->m_OCC_edges(m_index));
	m_edge = TopoDS::Edge(m_manager->m_OCC_edges_vec[m_index2]);
}
/*------------------------------------------------------------------------*/
void MGOCCCurve::project(gmds::math::Point& AP) const
{
	gp_Pnt pnt(AP.X(),AP.Y(),AP.Z());
	TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
	BRepExtrema_DistShapeShape extrema(V, m_edge);
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
MGPoint* MGOCCCurve::firstPoint() const
{
	TopoDS_Vertex v = TopExp::FirstVertex(m_edge);
	int index = m_manager->m_OCC_vertices.FindIndex(v);
	if(index<1){
		throw gmds::GMDSException("An end point of an edge is unknown for the geom. manager");
	}
	return m_manager->m_points[index-1];
}
/*------------------------------------------------------------------------*/
MGPoint* MGOCCCurve::secondPoint() const
{
	TopoDS_Vertex v = TopExp::LastVertex(m_edge);
	int index = m_manager->m_OCC_vertices.FindIndex(v);
	if(index<1){
		throw gmds::GMDSException("An end point of an edge is unknown for the geom. manager");
	}
	return m_manager->m_points[index-1];
}
/*------------------------------------------------------------------------*/
bool MGOCCCurve::isALoop() const
{
	std::vector<MGPoint*> pnts;
	get(pnts);

	if(pnts.size()==1)
		return true;

	return (pnts[0]==pnts[1]);

}
/*------------------------------------------------------------------------*/
void MGOCCCurve::bounds(MGSurface* ASurf, gmds::TCoord& ATMin, gmds::TCoord& ATMax) const
{
	TopoDS_Face occ_face;
	if(getOCCFace(ASurf,occ_face))
	{
		BRep_Tool::CurveOnSurface(m_edge,occ_face,ATMin,ATMax);
	}
	else
		throw gmds::GMDSException("Only OCC-based geometric surfaces can be used to get parameters");
}
/*------------------------------------------------------------------------*/
void MGOCCCurve::parametricData(MGSurface* ASurf, gmds::TCoord& AT,
		gmds::TCoord AUV[2],
		gmds::TCoord ADT[2],
		gmds::TCoord ADTT[2]) const
{
	TopoDS_Face occ_face;
	if(getOCCFace(ASurf,occ_face))
	{
		double t_min, t_max;
		Handle_Geom2d_Curve parametric_curve =
				BRep_Tool::CurveOnSurface(m_edge,occ_face,t_min,t_max);

		// We get the 2D coordinate of the point with parameter AT
		gp_Pnt2d pnt = parametric_curve->Value(AT);
		AUV[0] = pnt.X();
		AUV[1] = pnt.Y();
		// We get the first derivative in the point with parameter AT
		gp_Vec2d dt = parametric_curve->DN(AT,1);
		ADT[0] = dt.X();
		ADT[1] = dt.Y();
		// We get the second derivative in the point with parameter AT
		gp_Vec2d dtt = parametric_curve->DN(AT,2);
		ADTT[0] = dtt.X();
		ADTT[1] = dtt.Y();

	}
	else
		throw gmds::GMDSException("Only OCC-based geometric surfaces can be used to get parameters");
}
/*------------------------------------------------------------------------*/
MGOrientation MGOCCCurve::orientation() const
{
	MGOrientation value;
	switch(m_edge.Orientation())
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
bool MGOCCCurve::isInternal() const
{
	return (m_edge.Orientation()==TopAbs_INTERNAL);
}

/*------------------------------------------------------------------------*/
gmds::TCoord MGOCCCurve::length() const
{
	BRepCheck_Analyzer anaAna(m_edge, Standard_False);

	GProp_GProps pb;
	BRepGProp::LinearProperties(m_edge,pb);
	return  pb.Mass();
}
/*------------------------------------------------------------------------*/
void MGOCCCurve::get(std::vector<MGPoint*>& APnt) const
{
	APnt.clear();
	TopExp_Explorer exp;
	for(exp.Init(m_edge, TopAbs_VERTEX); exp.More(); exp.Next())
	{
		TopoDS_Vertex vertex = TopoDS::Vertex(exp.Current());
		int pnt_index = m_manager->m_OCC_vertices.FindIndex(vertex);
		APnt.push_back(m_manager->m_points[pnt_index-1]);
	}	  
}
/*------------------------------------------------------------------------*/
void MGOCCCurve::get(std::vector<MGCurve*>& ACurve) const
{
	throw gmds::GMDSException("Curve->Curve connectivity is not available");
}
/*------------------------------------------------------------------------*/
void MGOCCCurve::get(std::vector<MGSurface*>& ASurf) const
{
	ASurf.clear();
	for(unsigned int i=1; i<=m_manager->m_OCC_faces.Extent();i++)
	{
		TopoDS_Face fi = TopoDS::Face(m_manager->m_OCC_faces(i));
		TopExp_Explorer exp;
		bool found_face = false;
		for(exp.Init(fi, TopAbs_EDGE); exp.More() && !found_face; exp.Next())
		{
			TopoDS_Edge current_edge = TopoDS::Edge(exp.Current());
			int current_index = m_manager->m_OCC_edges.FindIndex(current_edge);
			if(m_index==current_index)
			{
				int face_index = m_manager->m_OCC_faces.FindIndex(fi);
				ASurf.push_back( m_manager->m_surfaces[face_index-1]);
				found_face = true;
			}
		}
	}
}
/*------------------------------------------------------------------------*/
void MGOCCCurve::get(std::vector<MGVolume*>& AVol) const
{
	AVol.clear();
	for(unsigned int i=1; i<=m_manager->m_OCC_solids.Extent();i++)
	{
		TopoDS_Solid si = TopoDS::Solid(m_manager->m_OCC_solids(i));
		TopExp_Explorer exp;
		bool found_vol = false;
		for(exp.Init(si, TopAbs_EDGE); exp.More() && !found_vol; exp.Next())
		{
			TopoDS_Edge current_edge = TopoDS::Edge(exp.Current());
			int current_index = m_manager->m_OCC_edges.FindIndex(current_edge);
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
void MGOCCCurve::computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const
{
	Bnd_Box box;
	//box.SetGap(tol);
	BRepBndLib::Add(m_edge,box);
	box.Get(minXYZ[0], minXYZ[1], minXYZ[2],
			maxXYZ[0], maxXYZ[1], maxXYZ[2]);
}
/*------------------------------------------------------------------------*/
int MGOCCCurve::getId() const
{
	return m_index;
}
/*------------------------------------------------------------------------*/
bool MGOCCCurve:: getOCCFace(MGSurface* ASurf, TopoDS_Face& AFace) const
{
	bool isOCC = false;
	MGOCCSurface* occ_surf = dynamic_cast<MGOCCSurface*>(ASurf);
	if(occ_surf!=NULL)
	{
		isOCC=true;
		AFace = occ_surf->getOCCFace();
	}

	return isOCC;
}
/*------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
