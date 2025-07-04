/*----------------------------------------------------------------------------*/
#include "Geom/ExportMLIImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Mesh/MeshItf.h"
#include "Mesh/Surface.h"
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
#include <gmds/io/LimaWriter.h>
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
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
	gmds::Mesh mesh(mod);

	
	try {
		std::vector<std::string> surfacesNames = m_context.getGeomManager().getSurfaces();
		for(int iSurf=0; iSurf<surfacesNames.size(); iSurf++) {
			Geom::Surface* surf = m_context.getGeomManager().getSurface(surfacesNames[iSurf]);
			std::vector<gmds::math::Triangle> triangles;
			auto add = [&](const TopoDS_Shape& sh) {
				OCCFacetedRepresentationBuilder builder(sh);
				builder.execute(triangles);
			};
			surf->apply(add);

			auto surf_group = mesh.getGroup<gmds::Face>(iSurf);
			for(int iTri=0; iTri<triangles.size(); iTri++) {
				gmds::Node n0 = mesh.newNode(triangles[iTri].getPoint(0));
				gmds::Node n1 = mesh.newNode(triangles[iTri].getPoint(1));
				gmds::Node n2 = mesh.newNode(triangles[iTri].getPoint(2));
				gmds::Face f = mesh.newTriangle(n0,n1,n2);
				surf_group->add(f);
			}
		}

		std::vector<std::string> curvesNames = m_context.getGeomManager().getCurves();
		for(int iCurv=0; iCurv<curvesNames.size(); iCurv++) {
			Geom::Curve* curv = m_context.getGeomManager().getCurve(curvesNames[iCurv]);
			std::vector<gmds::math::Triangle> triangles;
			auto add = [&](const TopoDS_Shape& sh) {
				OCCFacetedRepresentationBuilder builder(sh);
				builder.execute(triangles);
			};
			curv->apply(add);

			auto line_group = mesh.getGroup<gmds::Edge>(curvesNames[iCurv]);
			for(int iTri=0; iTri<triangles.size(); iTri++) {
				gmds::Node n0 = mesh.newNode(triangles[iTri].getPoint(0));
				gmds::Node n1 = mesh.newNode(triangles[iTri].getPoint(1));
				gmds::Edge e = mesh.newEdge(n0,n1);
				line_group->add(e);
			}
		}

		std::vector<std::string> verticesNames = m_context.getGeomManager().getVertices();
		for(int iVert=0; iVert<verticesNames.size(); iVert++) {
			Geom::Vertex* vert = m_context.getGeomManager().getVertex(verticesNames[iVert]);
			std::vector<gmds::math::Triangle> triangles;
			auto add = [&](const TopoDS_Shape& sh) {
				OCCFacetedRepresentationBuilder builder(sh);
				builder.execute(triangles);
			};
			vert->apply(add);

			auto cloud_group = mesh.newGroup<gmds::Node>(verticesNames[iVert]);
			for(int iTri=0; iTri<triangles.size(); iTri++) {
				gmds::Node n = mesh.newNode(triangles[iTri].getPoint(0));
				cloud_group->add(n);
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
		gmds::LimaWriter lw(mesh);
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
