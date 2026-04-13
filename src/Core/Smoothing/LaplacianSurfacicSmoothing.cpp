//
// Created by rochec on 28/01/2026.
//
#include "Smoothing/LaplacianSurfacicSmoothing.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <gmds/ig/Node.h>
#include <gmds/io/IGMeshIOService.h>
#include <gmds/io/VTKWriter.h>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomProjectImplementation.h"
#include "Internal/Context.h"
#include "Mesh/MeshItf.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
LaplacianSurfacicSmoothing::
LaplacianSurfacicSmoothing(std::vector<gmds::Node>& gmdsNodes, Geom::Surface* surface, int nbIterations):
    m_gmdsNodes(gmdsNodes),
    m_surface(surface),
    m_nbIterations(nbIterations)
{}

/*----------------------------------------------------------------------------*/
LaplacianSurfacicSmoothing::~LaplacianSurfacicSmoothing()
{}
/*----------------------------------------------------------------------------*/
void
LaplacianSurfacicSmoothing::execute()
{
    std::cout << __FILE__ << ", line " << __LINE__ << ", function " << __FUNCTION__ << std::endl;

    gmds::Mesh& gmds_mesh = getStdContext()->getMeshManager().getMesh()->getGMDSMesh();
    /*gmds_mesh.changeModel(gmds::MeshModel(gmds::DIM3 | gmds::R | gmds::N | gmds::E | gmds::F |
                                            gmds::R2F | gmds::R2N |gmds::F2N | gmds::N2F | gmds::F2E |
                                            gmds::E2F | gmds::E2N | gmds::N2E), true);*/

    std::map<gmds::TCellID, std::vector<gmds::TCellID>> N2E = buildN2E();

    gmds::IGMeshIOService ioService(&gmds_mesh);
    gmds::VTKWriter vtkWriter(&ioService);
    vtkWriter.setCellOptions(gmds::F|gmds::N);
    vtkWriter.setDataOptions(gmds::F|gmds::N);
    vtkWriter.write("mesh_BeforeSmoothing.vtk");

    // save new positions
    std::map<gmds::TCellID, gmds::math::Point> newPositions;

    for (int iter=0; iter<m_nbIterations; iter++)
    {
        for (auto const& n:m_gmdsNodes)
        {
            std::cout << "Node " << n.id() << ", edges: " << N2E[n.id()].size() << std::endl;
            if (!N2E[n.id()].empty())
            {
                gmds::math::Point p({0.0, 0.0, 0.0});
                for (auto const& eId:N2E[n.id()])
                {
                    gmds::Edge e = gmds_mesh.get<gmds::Edge>(eId);
                    p = p + e.getOppositeNode(n).point();
                }
                p = (1.0/static_cast<double>(N2E[n.id()].size()))*p;
                newPositions[n.id()] = p;
            }
        }

        // update new positions
        for (auto n:m_gmdsNodes)
        {
            n.setPoint(newPositions[n.id()]);
        }
    }

    //gmds_mesh.changeModel(gmds::MeshModel(gmds::DIM3|gmds::R|gmds::F|gmds::E|gmds::N|gmds::R2N|gmds::F2N|gmds::E2N|gmds::R2F|gmds::F2E), true);

    /*
    for (Topo::CoFace* cf:m_cofaces)
    {
        uint nbI;
        uint nbJ;
        cf->getNbMeshingNodes(nbI, nbJ);
        std::vector<gmds::TCellID> cfNodes = cf->nodes();
        // save new positions
        std::map<gmds::TCellID, Utils::Math::Point> newPositions;

        // loop over iterations
        for (int iter=0; iter<m_nbIterations; iter++)
        {
            // compute next positions
            for (uint j=1; j<nbJ-1; j++)
            {
                for (uint i=1; i<nbI-1; i++)
                {
                    gmds::Node nIJ = gmds_mesh.get<gmds::Node>(cf->getNode(i,j));
                    gmds::Node nImuJ = gmds_mesh.get<gmds::Node>(cf->getNode(i-1,j));
                    gmds::Node nIpuJ = gmds_mesh.get<gmds::Node>(cf->getNode(i+1,j));
                    gmds::Node nIJmu = gmds_mesh.get<gmds::Node>(cf->getNode(i,j-1));
                    gmds::Node nIJpu = gmds_mesh.get<gmds::Node>(cf->getNode(i,j+1));

                    Utils::Math::Point p = {0.0, 0.0, 0.0};

                    p.setX((nImuJ.X() + nIpuJ.X() + nIJmu.X() + nIJpu.X())/4.0 );
                    p.setY((nImuJ.Y() + nIpuJ.Y() + nIJmu.Y() + nIJpu.Y())/4.0 );
                    p.setZ((nImuJ.Z() + nIpuJ.Z() + nIJmu.Z() + nIJpu.Z())/4.0 );

                    // project position onto the surface
                    Geom::GeomProjectVisitor gpv(p);
                    m_surface->accept(gpv);
                    p = gpv.getProjectedPoint();

                    newPositions[nIJ.id()] = p;
                }
            }

            // update mesh with new positions
            for (uint j=1; j<nbJ-1; j++)
            {
                for (uint i=1; i<nbI-1; i++)
                {
                    gmds::Node nIJ = gmds_mesh.get<gmds::Node>(cf->getNode(i,j));
                    nIJ.setXYZ(newPositions[nIJ.id()].getX(), newPositions[nIJ.id()].getY(), newPositions[nIJ.id()].getZ());
                }
            }
        }

    }
    */

    gmds::IGMeshIOService ioServiceAfter(&gmds_mesh);
    gmds::VTKWriter vtkWriterAfter(&ioServiceAfter);
    vtkWriterAfter.setCellOptions(gmds::F|gmds::N);
    vtkWriterAfter.setDataOptions(gmds::F|gmds::N);
    vtkWriterAfter.write("mesh_AfterSmoothing.vtk");

}
/*----------------------------------------------------------------------------*/
std::map<gmds::TCellID, std::vector<gmds::TCellID>>
LaplacianSurfacicSmoothing::buildN2E() const
{
    gmds::Mesh& gmds_mesh = getStdContext()->getMeshManager().getMesh()->getGMDSMesh();
    std::map<gmds::TCellID, std::vector<gmds::TCellID>> N2E;
    for (auto e:gmds_mesh.edges())
    {
        std::vector<gmds::Node> nodes = gmds_mesh.get<gmds::Edge>(e).get<gmds::Node>();
        if (std::find(m_gmdsNodes.begin(), m_gmdsNodes.end(), nodes[0]) != m_gmdsNodes.end())
        {
            N2E[nodes[0].id()].push_back(e);
        }
        if (std::find(m_gmdsNodes.begin(), m_gmdsNodes.end(), nodes[1]) != m_gmdsNodes.end())
        {
            N2E[nodes[1].id()].push_back(e);
        }
    }
    return N2E;
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
// end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/