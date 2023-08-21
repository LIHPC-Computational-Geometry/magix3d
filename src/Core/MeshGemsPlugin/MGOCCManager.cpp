/*----------------------------------------------------------------------------*/
/** \file    MGOCCManager.cpp
 *  \author  F. LEDOUX
 *  \date    30/06/11
 */
/*----------------------------------------------------------------------------*/
// GMDS File Headers
#include "MeshGemsPlugin/MGOCCManager.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS.hxx> 
#include <TopoDS_Shape.hxx> 
#include <TopExp_Explorer.hxx> 
#include <BRep_Builder.hxx>
#include <Standard_CString.hxx>
#include <BRepTools.hxx>
/*----------------------------------------------------------------------------*/
#include "MeshGemsPlugin/MGCurve.h"
#include "MeshGemsPlugin/MGPoint.h"
#include "MeshGemsPlugin/MGSurface.h"
#include "MeshGemsPlugin/MGVolume.h"
#include "MeshGemsPlugin/MGOCCCurve.h"
#include "MeshGemsPlugin/MGOCCPoint.h"
#include "MeshGemsPlugin/MGOCCSurface.h"
#include "MeshGemsPlugin/MGOCCVolume.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems {
/*------------------------------------------------------------------------*/
MGOCCManager::MGOCCManager()
{

}
/*------------------------------------------------------------------------*/
MGOCCManager::~MGOCCManager()
{
	clear();
}
/*------------------------------------------------------------------------*/
gmds::TInt MGOCCManager::getNbPoints() const
{
	return m_points.size();
}
/*------------------------------------------------------------------------*/
gmds::TInt MGOCCManager::getNbCurves() const
{
	return m_curves.size();
}
/*------------------------------------------------------------------------*/
gmds::TInt MGOCCManager::getNbSurfaces() const
{
	return m_surfaces.size();
}
/*------------------------------------------------------------------------*/
gmds::TInt MGOCCManager::getNbVolumes() const
{
	return m_volumes.size();
}
/*------------------------------------------------------------------------*/
void MGOCCManager::getPoints(std::vector<MGPoint*>& APoints) const
{
	APoints = m_points;
}
/*------------------------------------------------------------------------*/
void MGOCCManager::getCurves(std::vector<MGCurve*>& ACurves) const
{
	ACurves = m_curves;
}
/*------------------------------------------------------------------------*/
void MGOCCManager::getSurfaces(std::vector<MGSurface*>& ASurfaces) const
{
	ASurfaces = m_surfaces;
}
/*------------------------------------------------------------------------*/
void MGOCCManager::getVolumes(std::vector<MGVolume*>& AVolumes) const
{
	AVolumes= m_volumes;
}
/*------------------------------------------------------------------------*/
void MGOCCManager::clear()
{
	//=====================================================================
	// Cleaning of OCC containers
	//=====================================================================
	m_OCC_solids.Clear();
	m_OCC_shells.Clear();
	m_OCC_faces.Clear();
	m_OCC_wires.Clear();
	m_OCC_edges.Clear();
	m_OCC_vertices.Clear();

	//=====================================================================
	// Cleaning of our shapes
	//=====================================================================
	for(unsigned int i=0;i<m_volumes.size();i++){
		delete m_volumes[i];
	}
	for(unsigned int i=0;i<m_surfaces.size();i++){
		delete m_surfaces[i];
	}
	for(unsigned int i=0;i<m_curves.size();i++){
		delete m_curves[i];
	}
	for(unsigned int i=0;i<m_points.size();i++){
		delete m_points[i];
	}
}
/*------------------------------------------------------------------------*/
void MGOCCManager::newVolume(const TopoDS_Solid& AShape)
{
	int index =  m_OCC_solids.Add(AShape);
	MGOCCVolume* v = new MGOCCVolume(index,this);
	m_volumes.push_back(v);
}
/*------------------------------------------------------------------------*/
void MGOCCManager::newSurface(const TopoDS_Face& AShape)
{
	int index =  m_OCC_faces.Add(AShape);
	MGOCCSurface* s = new MGOCCSurface(index,this);
	m_surfaces.push_back(s);
}
/*------------------------------------------------------------------------*/
void MGOCCManager::newCurve(const TopoDS_Edge& AShape)
{

	int index =  m_OCC_edges.Add(AShape);
	m_OCC_edges_vec.push_back(AShape);
	int index2 = m_OCC_edges_vec.size()-1;
	std::cout<<"\t index = "<<index<<std::endl;
	MGOCCCurve* c = new MGOCCCurve(index, index2,this);
	m_curves.push_back(c);
}
/*------------------------------------------------------------------------*/
void MGOCCManager::newPoint(const TopoDS_Vertex& AShape)
{
	int index =  m_OCC_vertices.Add(AShape);
	MGOCCPoint* p = new MGOCCPoint(index,this);
	m_points.push_back(p);
}
/*------------------------------------------------------------------------*/
bool MGOCCManager::importFace(const TopoDS_Face &AFace)
{

	importShape(AFace);
	return true;
}

/*------------------------------------------------------------------------*/
bool MGOCCManager::importBREP(const std::string& AFileName)
{
	TopoDS_Shape aShape;
	BRep_Builder aBuilder;
	Standard_CString aFileName = (Standard_CString)AFileName.c_str();

	bool result = BRepTools::Read(aShape,aFileName,aBuilder);
	if (!(result))
		return false;

	importShape(aShape);
	return true;
}

/*------------------------------------------------------------------------*/
void MGOCCManager::importShape(const TopoDS_Shape& AShape)
{
	TopoDS_Shape aShape = AShape;

	clear();

	TopExp_Explorer exp0, exp1, exp2, exp3, exp4, exp5;
	m_OCC_solids.Clear();
	m_OCC_shells.Clear();
	m_OCC_faces.Clear();
	m_OCC_wires.Clear();
	m_OCC_edges.Clear();
	m_OCC_vertices.Clear();

	for(exp0.Init(aShape, TopAbs_SOLID); exp0.More(); exp0.Next()){
		TopoDS_Solid solid = TopoDS::Solid(exp0.Current());
		if(m_OCC_solids.FindIndex(TopoDS::Solid(exp0.Current())) < 1){
			newVolume(TopoDS::Solid(exp0.Current()));

			for(exp1.Init(exp0.Current(), TopAbs_SHELL); exp1.More(); exp1.Next()){
				TopoDS_Shell shell = TopoDS::Shell(exp1.Current().Composed(exp0.Current().Orientation()));
				if(m_OCC_shells.FindIndex(shell) < 1)
				{
					m_OCC_shells.Add(shell);

					for(exp2.Init(shell, TopAbs_FACE); exp2.More(); exp2.Next()){
						TopoDS_Face face = TopoDS::Face(exp2.Current().Composed(shell.Orientation()));
						if(m_OCC_faces.FindIndex(face) < 1){
							newSurface(face);

							for(exp3.Init(exp2.Current(), TopAbs_WIRE); exp3.More(); exp3.Next()){
								TopoDS_Wire wire = TopoDS::Wire(exp3.Current().Composed(face.Orientation()));
								if(m_OCC_wires.FindIndex(wire) < 1){
									m_OCC_wires.Add(wire);

									for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
										TopoDS_Edge edge = TopoDS::Edge(exp4.Current().Composed(wire.Orientation()));
										if(m_OCC_edges.FindIndex(edge) < 1) {
											newCurve(edge);

											for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
												TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
												if(m_OCC_vertices.FindIndex(vertex) < 1)
													newPoint(vertex);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Free Shells
	for(exp1.Init(exp0.Current(), TopAbs_SHELL, TopAbs_SOLID); exp1.More(); exp1.Next()){
		TopoDS_Shape shell = exp1.Current().Composed(exp0.Current().Orientation());
		if(m_OCC_shells.FindIndex(shell) < 1){
			m_OCC_shells.Add(shell);

			for(exp2.Init(shell, TopAbs_FACE); exp2.More(); exp2.Next()){
				TopoDS_Face face = TopoDS::Face(exp2.Current().Composed(shell.Orientation()));
				if(m_OCC_faces.FindIndex(face) < 1){
					newSurface(face);

					for(exp3.Init(exp2.Current(), TopAbs_WIRE); exp3.More(); exp3.Next()){
						TopoDS_Wire wire = TopoDS::Wire(exp3.Current());
						if(m_OCC_wires.FindIndex(wire) < 1){
							m_OCC_wires.Add(wire);

							for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
								TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
								if(m_OCC_edges.FindIndex(edge) < 1){
									newCurve(edge);

									for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
										TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
										if(m_OCC_vertices.FindIndex(vertex) < 1)
											newPoint(vertex);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Free Faces
	for(exp2.Init(aShape, TopAbs_FACE, TopAbs_SHELL); exp2.More(); exp2.Next()){
		TopoDS_Face face = TopoDS::Face(exp2.Current());
		if(m_OCC_faces.FindIndex(face) < 1){
			newSurface(face);

			for(exp3.Init(exp2.Current(), TopAbs_WIRE); exp3.More(); exp3.Next()){
				TopoDS_Wire wire = TopoDS::Wire(exp3.Current());
				if(m_OCC_wires.FindIndex(wire) < 1){
					m_OCC_wires.Add(wire);

					for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
						TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
						if(m_OCC_edges.FindIndex(edge) < 1){
							newCurve(edge);

							for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
								TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
								if(m_OCC_vertices.FindIndex(vertex) < 1)
									newPoint(vertex);
							}
						}
					}
				}
			}
		}
	}

	// Free Wires
	for(exp3.Init(aShape, TopAbs_WIRE, TopAbs_FACE); exp3.More(); exp3.Next()){
		TopoDS_Wire wire = TopoDS::Wire(exp3.Current());
		if(m_OCC_wires.FindIndex(wire) < 1){
			m_OCC_wires.Add(wire);

			for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
				TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
				if(m_OCC_edges.FindIndex(edge) < 1){
					newCurve(edge);

					for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
						TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
						if(m_OCC_vertices.FindIndex(vertex) < 1)
							newPoint(vertex);
					}
				}
			}
		}
	}

	// Free Edges
	for(exp4.Init(aShape, TopAbs_EDGE, TopAbs_WIRE); exp4.More(); exp4.Next()){
		TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
		if(m_OCC_edges.FindIndex(edge) < 1){
			newCurve(edge);

			for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
				TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
				if(m_OCC_vertices.FindIndex(vertex) < 1)
					newPoint(vertex);
			}
		}
	}

	// Free Vertices
	for(exp5.Init(aShape, TopAbs_VERTEX, TopAbs_EDGE); exp5.More(); exp5.Next()){
		TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
		if(m_OCC_vertices.FindIndex(vertex) < 1)
			m_OCC_vertices.Add(vertex);
	}
}
/*--------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
