/*----------------------------------------------------------------------------*/
/*
 * ExportMLIImplementation.cpp
 *
 *  Created on: 30 sep. 2016
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#include <Geom/ExportMLIImplementation.h>
#include <Geom/Volume.h>
#include <Geom/Surface.h>
#include <Geom/Curve.h>
#include <Geom/Vertex.h>
#include <Mesh/MeshItf.h>
#include <Mesh/Surface.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IGMesh.h>
#include "GMDSCEA/GMDSCEAWriter.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <iostream>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ExportMLIImplementation::
ExportMLIImplementation(Internal::Context& c, const std::string& n)
: m_context(c), m_filename(n)
{}
/*----------------------------------------------------------------------------*/
ExportMLIImplementation::
ExportMLIImplementation(Internal::Context& c,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string& n)
: m_context(c), m_filename(n)
{
	MGX_NOT_YET_IMPLEMENTED("Cas Mli avec sélection a implémenter");
}
/*----------------------------------------------------------------------------*/
ExportMLIImplementation::~ExportMLIImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportMLIImplementation::perform(Internal::InfoCommand* icmd)
{

	gmds::MeshModel mod = gmds::DIM3|gmds::N|gmds::E|gmds::F|gmds::E2N|gmds::F2N;
	gmds::IGMesh mesh(mod);

	try {
		std::vector<std::string> surfacesNames = m_context.getLocalGeomManager().getSurfaces();
		for(int iSurf=0; iSurf<surfacesNames.size(); iSurf++) {
			Geom::Surface* surf = m_context.getLocalGeomManager().getSurface(surfacesNames[iSurf]);
			std::vector<gmds::math::Triangle> triangles;
			surf->getFacetedRepresentation(triangles);

			gmds::IGMesh::surface& surf_group = mesh.newSurface(surfacesNames[iSurf]);
			for(int iTri=0; iTri<triangles.size(); iTri++) {
				gmds::Node n0 = mesh.newNode(triangles[iTri].getPoint(0));
				gmds::Node n1 = mesh.newNode(triangles[iTri].getPoint(1));
				gmds::Node n2 = mesh.newNode(triangles[iTri].getPoint(2));
				gmds::Face f = mesh.newTriangle(n0,n1,n2);
				surf_group.add(f);
			}
		}

		std::vector<std::string> curvesNames = m_context.getLocalGeomManager().getCurves();
		for(int iCurv=0; iCurv<curvesNames.size(); iCurv++) {
			Geom::Curve* curv = m_context.getLocalGeomManager().getCurve(curvesNames[iCurv]);
			std::vector<gmds::math::Triangle> triangles;
			curv->getFacetedRepresentation(triangles);

			gmds::IGMesh::line& line_group = mesh.newLine(curvesNames[iCurv]);
			for(int iTri=0; iTri<triangles.size(); iTri++) {
				gmds::Node n0 = mesh.newNode(triangles[iTri].getPoint(0));
				gmds::Node n1 = mesh.newNode(triangles[iTri].getPoint(1));
				gmds::Edge e = mesh.newEdge(n0,n1);
				line_group.add(e);
			}
		}

		std::vector<std::string> verticesNames = m_context.getLocalGeomManager().getVertices();
		for(int iVert=0; iVert<verticesNames.size(); iVert++) {
			Geom::Vertex* vert = m_context.getLocalGeomManager().getVertex(verticesNames[iVert]);
			std::vector<gmds::math::Triangle> triangles;
			vert->getFacetedRepresentation(triangles);

			gmds::IGMesh::cloud& cloud_group = mesh.newCloud(verticesNames[iVert]);
			for(int iTri=0; iTri<triangles.size(); iTri++) {
				gmds::Node n = mesh.newNode(triangles[iTri].getPoint(0));
				cloud_group.add(n);
			}
		}
	}
	catch(TkUtil::Exception &e) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "ExportMLIImplementation::perform "
				<< e.getMessage();
		throw TkUtil::Exception (message);
	}

	try {
		Utils::Unit::lengthUnit lu = m_context.getLengthUnit();
		double convertFactorToMeter = 1.;
		if(lu != Utils::Unit::undefined) {
			convertFactorToMeter = Utils::Unit::computeMeterFactor(lu);
		}

		bool	implemented	= false;
		gmds::GMDSCEAWriter lw(mesh);
		lw.setLengthUnit(convertFactorToMeter);
		lw.write(m_filename, gmds::R|gmds::F|gmds::E|gmds::N);
		implemented	= true;
		if (false == implemented)
			throw TkUtil::Exception (TkUtil::UTF8String ("Absence d'écrivain GMDS pour le format mli.", TkUtil::Charset::UTF_8));
	}
	catch(gmds::GMDSException& e) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "ExportMLIImplementation::perform "
				<< e.what();
		throw TkUtil::Exception (message);
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
