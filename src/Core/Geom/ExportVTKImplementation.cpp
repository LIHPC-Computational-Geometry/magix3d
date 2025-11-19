/*----------------------------------------------------------------------------*/
#include "Geom/ExportVTKImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Mesh/MeshItf.h"
#include "Mesh/Surface.h"
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
#include <gmds/io/IGMeshIOService.h>
#include <gmds/io/VTKWriter.h>
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ExportVTKImplementation::
ExportVTKImplementation(Internal::Context& c, const std::string& n)
: m_context(c), m_filename(n)
{}
/*----------------------------------------------------------------------------*/
ExportVTKImplementation::
ExportVTKImplementation(Internal::Context& c,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string& n)
: m_context(c), m_filename(n)
{
	MGX_NOT_YET_IMPLEMENTED("Cas VTK avec sélection a implémenter");
}
/*----------------------------------------------------------------------------*/
ExportVTKImplementation::~ExportVTKImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportVTKImplementation::perform(Internal::InfoCommand* icmd)
{


	std::vector<std::string> volumesNames = m_context.getGeomManager().getVolumes();

	if(1 != volumesNames.size()) {
		std::cerr<<"ExportVTKImplementation::perform can only be called when there is "
				"one and only one volume"<<std::endl;
		throw TkUtil::Exception (TkUtil::UTF8String ("ExportVTKImplementation::perform can only be called when there is "
				"one and only one volume", TkUtil::Charset::UTF_8));
	}

	Geom::Volume* volume = m_context.getGeomManager().getVolume(volumesNames[0]);

	unsigned int nbBlocks = m_context.getTopoManager().getNbBlocks();
	if(1 != nbBlocks) {
		std::cerr<<"ExportVTKImplementation::perform can only be called when there is "
				"one and only one block"<<std::endl;
		throw TkUtil::Exception (TkUtil::UTF8String ("ExportVTKImplementation::perform can only be called when there is "
				"one and only one block", TkUtil::Charset::UTF_8));
	}

	std::vector<Topo::Block*> topo_blocs = m_context.getTopoManager().getBlocksObj();

	gmds::Mesh& mesh = m_context.getMeshManager().getMesh()->getGMDSMesh();
	{
		// surfaces
		for(Topo::CoFace* coface : topo_blocs[0]->getCoFaces()) {

			Geom::Surface* surf_tmp = dynamic_cast<Geom::Surface*> (coface->getGeomAssociation());
			CHECK_NULL_PTR_ERROR(surf_tmp);
			auto surf = mesh.newGroup<gmds::Face>(surf_tmp->getName());

			std::vector<gmds::TCellID>& elems = coface->faces();
			std::vector<gmds::Face> face_elems;
			face_elems.resize(elems.size());
			for(unsigned int iGMDSFace=0; iGMDSFace<elems.size(); iGMDSFace++) {
			    face_elems[iGMDSFace] = mesh.get<gmds::Face>(elems[iGMDSFace]);
			}
			for(unsigned int iGMDSFace=0; iGMDSFace<elems.size(); iGMDSFace++) {
				surf->add(face_elems[iGMDSFace]);
			}
		}

		// curves
		for(Topo::CoEdge* coedge : topo_blocs[0]->getCoEdges()) {

		    Geom::Curve* curv_tmp = dynamic_cast<Geom::Curve*> (coedge->getGeomAssociation());
		    CHECK_NULL_PTR_ERROR(curv_tmp);
		    auto cl = mesh.newGroup<gmds::Node>(curv_tmp->getName());

		    std::vector<gmds::TCellID>& elems = coedge->nodes();

		    std::vector<gmds::Node> node_elems;
		    node_elems.resize(elems.size());
		    for(unsigned int iGMDSFace=0; iGMDSFace<elems.size(); iGMDSFace++) {
		        node_elems[iGMDSFace] = mesh.get<gmds::Node>(elems[iGMDSFace]);
		    }
		    for(unsigned int iGMDSNode=0; iGMDSNode<elems.size(); iGMDSNode++) {
		        cl->add(node_elems[iGMDSNode]);
		    }
		}

		// vertices
		for(Topo::Vertex* vertex : topo_blocs[0]->getVertices()) {

			Geom::Vertex* vert_tmp = dynamic_cast<Geom::Vertex*> (vertex->getGeomAssociation());
			CHECK_NULL_PTR_ERROR(vert_tmp);
			auto cl = mesh.newGroup<gmds::Node>(vert_tmp->getName());
			gmds::Node elem = mesh.get<gmds::Node>(vertex->getNode());
			cl->add(elem);
		}
	}

	gmds::IGMeshIOService ioService(&mesh);
	gmds::VTKWriter writer(&ioService);
	writer.setCellOptions(gmds::N|gmds::F);
	writer.setDataOptions(gmds::N|gmds::F);
	writer.write(m_filename);

    //std::cerr<<"ExportVTKImplementation::perform --->PAS IMPLEMENTE"<<std::endl;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
