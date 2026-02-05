//
// Created by rochec on 28/01/2026.
//
/*----------------------------------------------------------------------------*/
#include "Smoothing/YaoSurfacicSmoothing.h"
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
        YaoSurfacicSmoothing::
        YaoSurfacicSmoothing(std::vector<Topo::CoFace*>& cofaces, Geom::Surface* surface, int nbIterations):
            m_cofaces(cofaces),
            m_surface(surface),
            m_nbIterations(nbIterations)
        {}

/*----------------------------------------------------------------------------*/
        YaoSurfacicSmoothing::~YaoSurfacicSmoothing()
        {}
/*----------------------------------------------------------------------------*/
        void
        YaoSurfacicSmoothing::execute()
        {
            std::cout << __FILE__ << ", line " << __LINE__ << ", function " << __FUNCTION__ << std::endl;

            gmds::Mesh& gmds_mesh = getStdContext()->getMeshManager().getMesh()->getGMDSMesh();

            gmds::IGMeshIOService ioService(&gmds_mesh);
            gmds::VTKWriter vtkWriter(&ioService);
            vtkWriter.setCellOptions(gmds::F|gmds::N);
            vtkWriter.setDataOptions(gmds::F|gmds::N);
            vtkWriter.write("YaoSurfacicSmoothing_"+std::to_string(0)+".vtk");

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
                            gmds::Node nImuJmu = gmds_mesh.get<gmds::Node>(cf->getNode(i-1,j-1));
                            gmds::Node nImuJ   = gmds_mesh.get<gmds::Node>(cf->getNode(i-1,  j));
                            gmds::Node nImuJpu = gmds_mesh.get<gmds::Node>(cf->getNode(i-1,j+1));
                            gmds::Node nIJmu   = gmds_mesh.get<gmds::Node>(cf->getNode(  i,j-1));
                            gmds::Node nIJ     = gmds_mesh.get<gmds::Node>(cf->getNode(  i,  j));
                            gmds::Node nIJpu   = gmds_mesh.get<gmds::Node>(cf->getNode(  i,j+1));
                            gmds::Node nIpuJmu = gmds_mesh.get<gmds::Node>(cf->getNode(i+1,j-1));
                            gmds::Node nIpuJ   = gmds_mesh.get<gmds::Node>(cf->getNode(i+1,  j));
                            gmds::Node nIpuJpu = gmds_mesh.get<gmds::Node>(cf->getNode(i+1,j+1));

                            gmds::math::Point pImu = computePointOnBranch(nImuJmu.point(), nImuJ.point(), nImuJpu.point());
                            gmds::math::Point pI   = computePointOnBranch(  nIJmu.point(),   nIJ.point(),   nIJpu.point());
                            gmds::math::Point pIpu = computePointOnBranch(nIpuJmu.point(), nIpuJ.point(), nIpuJpu.point());

                            gmds::math::Point pJmu = computePointOnBranch(nImuJmu.point(), nIJmu.point(), nIpuJmu.point());
                            gmds::math::Point pJ   = computePointOnBranch(  nImuJ.point(),   nIJ.point(),   nIpuJ.point());
                            gmds::math::Point pJpu = computePointOnBranch(nImuJpu.point(), nIJpu.point(), nIpuJpu.point());

                            gmds::math::Point pImid = computePointOnBranch(pImu, pI, pIpu);
                            gmds::math::Point pJmid = computePointOnBranch(pJmu, pJ, pJpu);

                            Utils::Math::Point p({0.5*(pImid+pJmid).X(), 0.5*(pImid+pJmid).Y(), 0.5*(pImid+pJmid).Z()});

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

                    gmds::IGMeshIOService ioService2(&gmds_mesh);
                    gmds::VTKWriter vtkWriter2(&ioService2);
                    vtkWriter2.setCellOptions(gmds::F|gmds::N);
                    vtkWriter2.setDataOptions(gmds::F|gmds::N);
                    vtkWriter2.write("YaoSurfacicSmoothing_"+std::to_string(iter+1)+".vtk");
                }

            }

        }
/*----------------------------------------------------------------------------*/
        Utils::Math::Point
        YaoSurfacicSmoothing::computeNextNodePosition(Topo::CoFace* coface, int i, int j)
        {


        }
/*----------------------------------------------------------------------------*/
        gmds::math::Point
        YaoSurfacicSmoothing::computePointOnBranch(gmds::math::Point const &p0,
                                                   gmds::math::Point const &p1,
                                                   gmds::math::Point const &p2)
        {
            gmds::math::Point p({0,0,0});
            double const d1 = (p1-p0).norm();
            double const d2 = (p2-p1).norm();
            double const d = d1 + d2;
            if (d/2.0 <= d1)
            {
                p = p0 + (d/2.0)*(p1-p0).normalize();
            }
            else
            {
                p = p1 + (d/2.0 - d1)*(p2-p1).normalize();
            }
            return p;
        }
        /*----------------------------------------------------------------------------*/
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/