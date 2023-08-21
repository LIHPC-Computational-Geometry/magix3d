/*----------------------------------------------------------------------------*/
/** \file    MGOCCPoint.cpp
 *  \author  F. LEDOUX
 *  \date    03/12/15
 */
/*----------------------------------------------------------------------------*/
// GMDS File Headers
#include "MeshGemsPlugin/MGOCCPoint.h"
#include "MeshGemsPlugin/MGOCCManager.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/
MGOCCPoint::MGOCCPoint(const int AIndex, MGOCCManager* AMng)
:m_index(AIndex), m_manager(AMng)
{
	m_pnt = BRep_Tool::Pnt(TopoDS::Vertex(m_manager->m_OCC_vertices(m_index)));
}
/*------------------------------------------------------------------------*/
gmds::TInt MGOCCPoint::getNbCurves() const
{
	std::vector<MGCurve*> adj_curves;
	get(adj_curves);
	return adj_curves.size();
}
/*------------------------------------------------------------------------*/
gmds::TCoord MGOCCPoint::X() const
{
	return m_pnt.X();
}
/*------------------------------------------------------------------------*/
gmds::TCoord MGOCCPoint::Y() const
{
	return m_pnt.Y();
}
/*------------------------------------------------------------------------*/
gmds::TCoord MGOCCPoint::Z() const
{
	return  m_pnt.Z();
}
/*------------------------------------------------------------------------*/
void MGOCCPoint::XYZ(gmds::TCoord ACoordinates[3]) const
{
	ACoordinates[0] = m_pnt.X();
	ACoordinates[1] = m_pnt.Y();
	ACoordinates[2] = m_pnt.Z();

}
/*------------------------------------------------------------------------*/
gmds::math::Point MGOCCPoint::getPoint() const
{
	return gmds::math::Point(X(),Y(),Z());
}
/*------------------------------------------------------------------------*/
void MGOCCPoint::get(std::vector<MGPoint*>& APnt) const
{
	APnt.clear();
	std::vector<MGCurve*> adj_curves;
	get(adj_curves);
	for(unsigned int i=0;i<adj_curves.size();i++)
	{
		MGCurve* ci = adj_curves[i];
		if(ci->firstPoint()==this)
			APnt.push_back(ci->secondPoint());
		else if(ci->secondPoint()==this)
			APnt.push_back(ci->firstPoint());

	}
}
/*------------------------------------------------------------------------*/
void MGOCCPoint::get(std::vector<MGCurve*>& ACurv) const
{
	ACurv.clear();
	for(unsigned int i=1; i<=m_manager->m_OCC_edges.Extent();i++)
	{
		TopoDS_Edge ei = TopoDS::Edge(m_manager->m_OCC_edges(i));
		TopExp_Explorer exp;
		bool found_edge = false;
		for(exp.Init(ei, TopAbs_VERTEX); exp.More() && !found_edge; exp.Next())
		{
			TopoDS_Vertex vertex = TopoDS::Vertex(exp.Current());
			int current_index = m_manager->m_OCC_vertices.FindIndex(vertex);
			if(m_index==current_index)
			{
				int curve_index = m_manager->m_OCC_edges.FindIndex(ei);
				ACurv.push_back( m_manager->m_curves[curve_index-1]);
				found_edge = true;
			}
		}
	}
}
/*------------------------------------------------------------------------*/
void MGOCCPoint::get(std::vector<MGSurface*>& ASurf) const
{
	ASurf.clear();
	for(unsigned int i=1; i<=m_manager->m_OCC_faces.Extent();i++)
	{
		TopoDS_Face fi = TopoDS::Face(m_manager->m_OCC_faces(i));
		TopExp_Explorer exp;
		bool found_face = false;
		for(exp.Init(fi, TopAbs_VERTEX); exp.More() && !found_face; exp.Next())
		{
			TopoDS_Vertex vertex = TopoDS::Vertex(exp.Current());
			int current_index = m_manager->m_OCC_vertices.FindIndex(vertex);
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
void MGOCCPoint::get(std::vector<MGVolume*>& AVol) const
{
	AVol.clear();
	for(unsigned int i=1; i<=m_manager->m_OCC_solids.Extent();i++)
	{
		TopoDS_Solid si = TopoDS::Solid(m_manager->m_OCC_solids(i));
		TopExp_Explorer exp;
		bool found_vol = false;
		for(exp.Init(si, TopAbs_VERTEX); exp.More() && !found_vol; exp.Next())
		{
			TopoDS_Vertex vertex = TopoDS::Vertex(exp.Current());
			int current_index = m_manager->m_OCC_vertices.FindIndex(vertex);
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
void MGOCCPoint::computeBoundingBox(gmds::TCoord AMinXYZ[3], gmds::TCoord AMaxXYZ[3]) const
{
	AMinXYZ[0]=X();
	AMinXYZ[1]=Y();
	AMinXYZ[2]=Z();

	AMaxXYZ[0]=X();
	AMaxXYZ[1]=Y();
	AMaxXYZ[2]=Z();
}
/*------------------------------------------------------------------------*/
int MGOCCPoint::getId() const
{
	return m_index;
}
/*------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
