/*----------------------------------------------------------------------------*/
/** \file    MGOCCVolume.cpp
 *  \author  F. LEDOUX
 *  \date    03/12/15
 */
/*----------------------------------------------------------------------------*/
#include "MeshGemsPlugin/MGOCCVolume.h"
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
/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/
MGOCCVolume::MGOCCVolume(const int AIndex, MGOCCManager* AMng)
:m_index(AIndex), m_manager(AMng)
{
	m_solid = TopoDS::Solid(m_manager->m_OCC_solids(m_index));
}
/*------------------------------------------------------------------------*/
gmds::TCoord MGOCCVolume::volume() const
{
	Standard_Boolean onlyClosed = Standard_True;
	Standard_Boolean isUseSpan = Standard_True;
	Standard_Real aDefaultTol = 1.e-7;
	Standard_Boolean CGFlag = Standard_False;
	Standard_Boolean IFlag = Standard_False;

	BRepCheck_Analyzer anaAna(m_solid, Standard_False);

	GProp_GProps pb;
	Standard_Real local_eps =BRepGProp::VolumePropertiesGK (m_solid,
			pb,
			aDefaultTol,
			onlyClosed,
			isUseSpan,
			CGFlag,
			IFlag);

	return pb.Mass();
}
/*------------------------------------------------------------------------*/
void MGOCCVolume::get(std::vector<MGPoint*>& APnt) const
{
	APnt.clear();
	TopTools_IndexedMapOfShape  mapV;
	TopExp::MapShapes(m_solid,TopAbs_VERTEX, mapV);
	for(unsigned int i=1 ; i <=mapV.Extent();i++)
	{
		TopoDS_Vertex vertex = TopoDS::Vertex(mapV.FindKey(i));
		int pnt_index = m_manager->m_OCC_vertices.FindIndex(vertex);
		APnt.push_back(m_manager->m_points[pnt_index-1]);
	}
}
/*------------------------------------------------------------------------*/
void MGOCCVolume::get(std::vector<MGCurve*>& ACurves) const
{
	ACurves.clear();
	TopTools_IndexedMapOfShape  mapC;
	TopExp::MapShapes(m_solid,TopAbs_EDGE, mapC);
	for(unsigned int i=1 ; i <=mapC.Extent();i++)
	{
		TopoDS_Edge edge = TopoDS::Edge(mapC.FindKey(i));
		int edge_index = m_manager->m_OCC_edges.FindIndex(edge);
		ACurves.push_back(m_manager->m_curves[edge_index-1]);
	}
}
/*------------------------------------------------------------------------*/
void MGOCCVolume::get(std::vector<MGSurface*>& ASurfs) const
{
	ASurfs.clear();
	TopTools_IndexedMapOfShape  mapF;
	TopExp::MapShapes(m_solid,TopAbs_FACE, mapF);
	for(unsigned int i=1 ; i <=mapF.Extent();i++)
	{
		TopoDS_Face current_face = TopoDS::Face(mapF.FindKey(i));
		int current_index = m_manager->m_OCC_faces.FindIndex(current_face);
		ASurfs.push_back(m_manager->m_surfaces[current_index-1]);
	}
}
/*------------------------------------------------------------------------*/
void MGOCCVolume::get(std::vector<MGVolume*>& AVols) const
{
	AVols.clear();
	std::vector<MGSurface*> adj_surfaces;
	get(adj_surfaces);
	for(unsigned int i=0;i<adj_surfaces.size();i++)
	{
		MGSurface* si = adj_surfaces[i];
		std::vector<MGVolume*> adj_vols_i;
		si->get(adj_vols_i);
		for(unsigned int j=0;j<adj_vols_i.size();j++)
		{
			if(adj_vols_i[j]!=this)
				AVols.push_back(adj_vols_i[j]);
		}
	}
}
/*------------------------------------------------------------------------*/
void MGOCCVolume::computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const
{
	Bnd_Box box;
	//box.SetGap(tol);
	BRepBndLib::Add(m_solid,box);
	box.Get(minXYZ[0], minXYZ[1], minXYZ[2],
			maxXYZ[0], maxXYZ[1], maxXYZ[2]);
}
/*------------------------------------------------------------------------*/
int MGOCCVolume::getId() const
{
	return m_index;
}
/*--------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
