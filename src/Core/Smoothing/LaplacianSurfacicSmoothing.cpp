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
        LaplacianSurfacicSmoothing(std::vector<Topo::CoFace*>& cofaces, Geom::Surface* surface, int nbIterations):
            m_cofaces(cofaces),
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

            gmds::IGMeshIOService ioService(&gmds_mesh);
            gmds::VTKWriter vtkWriter(&ioService);
            vtkWriter.setCellOptions(gmds::F|gmds::N);
            vtkWriter.setDataOptions(gmds::F|gmds::N);
            vtkWriter.write("mesh_BeforeSmoothing.vtk");

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

            gmds::IGMeshIOService ioServiceAfter(&gmds_mesh);
            gmds::VTKWriter vtkWriterAfter(&ioServiceAfter);
            vtkWriterAfter.setCellOptions(gmds::F|gmds::N);
            vtkWriterAfter.setDataOptions(gmds::F|gmds::N);
            vtkWriterAfter.write("mesh_AfterSmoothing.vtk");

        }
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/