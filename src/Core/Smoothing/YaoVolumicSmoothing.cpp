//
// Created by rochec on 02/02/2026.
//
/*----------------------------------------------------------------------------*/
#include "Smoothing/YaoVolumicSmoothing.h"
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
        YaoVolumicSmoothing::
        YaoVolumicSmoothing(std::vector<Topo::Block*> &blocks, int nbIterations):
            m_blocks(blocks),
            m_nbIterations(nbIterations),
            m_gmds_mesh(&getStdContext()->getMeshManager().getMesh()->getGMDSMesh())
        {
        }

/*----------------------------------------------------------------------------*/
        YaoVolumicSmoothing::~YaoVolumicSmoothing()
        {}
/*----------------------------------------------------------------------------*/
        void
        YaoVolumicSmoothing::execute()
        {
            std::cout << __FILE__ << ", line " << __LINE__ << ", function " << __FUNCTION__ << std::endl;

            gmds::Mesh& gmds_mesh = getStdContext()->getMeshManager().getMesh()->getGMDSMesh();

            gmds::IGMeshIOService ioService(&gmds_mesh);
            gmds::VTKWriter vtkWriter(&ioService);
            vtkWriter.setCellOptions(gmds::R|gmds::N);
            vtkWriter.setDataOptions(gmds::R|gmds::N);
            vtkWriter.write("YaoVolumicSmoothing_"+std::to_string(0)+".vtk");

            for (Topo::Block* b:m_blocks)
            {
                uint nbI;
                uint nbJ;
                uint nbK;
                b->getNbMeshingEdges(nbI, nbJ, nbK);
                nbI++;
                nbJ++;
                nbK++;
                std::vector<gmds::TCellID> bNodes = b->nodes();
                // save new positions
                std::map<gmds::TCellID, gmds::math::Point> newPositions;

                // loop over iterations
                for (int iter=0; iter<m_nbIterations; iter++)
                {
                    // compute next positions
                    for (uint k=1; k<nbK-1; k++)
                    {
                        for (uint j=1; j<nbJ-1; j++)
                        {
                            for (uint i=1; i<nbI-1; i++)
                            {
                                newPositions[b->getNode(i,j,k)] = computeNextNodePosition(b, i, j, k);
                            }
                        }
                    }

                    // update mesh with new positions
                    for (uint k=1; k<nbK-1; k++)
                    {
                        for (uint j=1; j<nbJ-1; j++)
                        {
                            for (uint i=1; i<nbI-1; i++)
                            {
                                gmds::Node nIJK = m_gmds_mesh->get<gmds::Node>(b->getNode(i, j, k));
                                nIJK.setPoint(newPositions[nIJK.id()]);
                            }
                        }
                    }

                    gmds::IGMeshIOService ioService2(&gmds_mesh);
                    gmds::VTKWriter vtkWriter2(&ioService2);
                    vtkWriter2.setCellOptions(gmds::R|gmds::N);
                    vtkWriter2.setDataOptions(gmds::R|gmds::N);
                    vtkWriter2.write("YaoVolumicSmoothing_"+std::to_string(iter+1)+".vtk");
                }

            }

        }
/*----------------------------------------------------------------------------*/
        gmds::math::Point
        YaoVolumicSmoothing::computeNextNodePosition(Topo::Block* b, uint i, uint j, uint k)
        {

            gmds::Node nImuJmuKmu = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,j-1,k-1));
            gmds::Node nImuJKmu   = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,  j,k-1));
            gmds::Node nImuJpuKmu = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,j+1,k-1));
            gmds::Node nIJmuKmu   = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,j-1,k-1));
            gmds::Node nIJKmu     = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,  j,k-1));
            gmds::Node nIJpuKmu   = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,j+1,k-1));
            gmds::Node nIpuJmuKmu = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,j-1,k-1));
            gmds::Node nIpuJKmu   = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,  j,k-1));
            gmds::Node nIpuJpuKmu = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,j+1,k-1));

            gmds::Node nImuJmuK = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,j-1,k));
            gmds::Node nImuJK   = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,  j,k));
            gmds::Node nImuJpuK = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,j+1,k));
            gmds::Node nIJmuK   = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,j-1,k));
            gmds::Node nIJK     = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,  j,k));
            gmds::Node nIJpuK   = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,j+1,k));
            gmds::Node nIpuJmuK = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,j-1,k));
            gmds::Node nIpuJK   = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,  j,k));
            gmds::Node nIpuJpuK = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,j+1,k));

            gmds::Node nImuJmuKpu = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,j-1,k+1));
            gmds::Node nImuJKpu   = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,  j,k+1));
            gmds::Node nImuJpuKpu = m_gmds_mesh->get<gmds::Node>(b->getNode(i-1,j+1,k+1));
            gmds::Node nIJmuKpu   = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,j-1,k+1));
            gmds::Node nIJKpu     = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,  j,k+1));
            gmds::Node nIJpuKpu   = m_gmds_mesh->get<gmds::Node>(b->getNode(  i,j+1,k+1));
            gmds::Node nIpuJmuKpu = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,j-1,k+1));
            gmds::Node nIpuJKpu   = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,  j,k+1));
            gmds::Node nIpuJpuKpu = m_gmds_mesh->get<gmds::Node>(b->getNode(i+1,j+1,k+1));

            // For faces i-1, i, and i+1
            gmds::math::Point pImu = computePointOnFace(nImuJmuKmu.point(), nImuJKmu.point(), nImuJpuKmu.point(),
                                                          nImuJmuK.point(),   nImuJK.point(),   nImuJpuK.point(),
                                                        nImuJmuKpu.point(), nImuJKpu.point(), nImuJpuKpu.point());
            gmds::math::Point pI = computePointOnFace(nIJmuKmu.point(), nIJKmu.point(), nIJpuKmu.point(),
                                                        nIJmuK.point(),   nIJK.point(),   nIJpuK.point(),
                                                      nIJmuKpu.point(), nIJKpu.point(), nIJpuKpu.point());
            gmds::math::Point pIpu = computePointOnFace(nImuJmuKmu.point(), nImuJKmu.point(), nImuJpuKmu.point(),
                                                          nImuJmuK.point(),   nImuJK.point(),   nImuJpuK.point(),
                                                        nImuJmuKpu.point(), nImuJKpu.point(), nImuJpuKpu.point());

            // For faces j-1, j, and j+1
            gmds::math::Point pJmu = computePointOnFace(nImuJmuKmu.point(), nImuJmuK.point(), nImuJmuK.point(),
                                                          nIJmuKmu.point(),   nIJmuK.point(),   nIJmuK.point(),
                                                        nIpuJmuKmu.point(), nIpuJmuK.point(), nIpuJmuK.point());
            gmds::math::Point pJ = computePointOnFace(nImuJKmu.point(), nImuJK.point(), nImuJK.point(),
                                                        nIJKmu.point(),   nIJK.point(),   nIJK.point(),
                                                      nIpuJKmu.point(), nIpuJK.point(), nIpuJK.point());
            gmds::math::Point pJpu = computePointOnFace(nImuJpuKmu.point(), nImuJpuK.point(), nImuJpuK.point(),
                                                          nIJpuKmu.point(),   nIJpuK.point(),   nIJpuK.point(),
                                                        nIpuJpuKmu.point(), nIpuJpuK.point(), nIpuJpuK.point());

            // For faces k-1, k, and k+1
            gmds::math::Point pKmu = computePointOnFace(nImuJmuKmu.point(), nImuJKmu.point(), nImuJpuKmu.point(),
                                                          nIJmuKmu.point(),   nIJKmu.point(),   nIJpuKmu.point(),
                                                        nIpuJmuKmu.point(), nIpuJKmu.point(), nIpuJpuKmu.point());
            gmds::math::Point pK = computePointOnFace(nImuJmuK.point(), nImuJK.point(), nImuJpuK.point(),
                                                        nIJmuK.point(),   nIJK.point(),   nIJpuK.point(),
                                                      nIpuJmuK.point(), nIpuJK.point(), nIpuJpuK.point());
            gmds::math::Point pKpu = computePointOnFace(nImuJmuKpu.point(), nImuJKpu.point(), nImuJpuKpu.point(),
                                                          nIJmuKpu.point(),   nIJKpu.point(),   nIJpuKpu.point(),
                                                        nIpuJmuKpu.point(), nIpuJKpu.point(), nIpuJpuKpu.point());

            gmds::math::Point pImid = computePointOnBranch(pImu, pI, pIpu);
            gmds::math::Point pJmid = computePointOnBranch(pJmu, pJ, pJpu);
            gmds::math::Point pKmid = computePointOnBranch(pKmu, pK, pKpu);

            gmds::math::Point p = (1.0/3.0)*(pImid + pJmid + pKmid);
            return p;

        }
/*----------------------------------------------------------------------------*/
        gmds::math::Point
        YaoVolumicSmoothing::computePointOnBranch(gmds::math::Point const &p0,
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
        gmds::math::Point
        YaoVolumicSmoothing::computePointOnFace(gmds::math::Point const &p0,
                                                gmds::math::Point const &p1,
                                                gmds::math::Point const &p2,
                                                gmds::math::Point const &p3,
                                                gmds::math::Point const &p4,
                                                gmds::math::Point const &p5,
                                                gmds::math::Point const &p6,
                                                gmds::math::Point const &p7,
                                                gmds::math::Point const &p8)
        {
            gmds::math::Point pV1 = computePointOnBranch(p0, p3, p6);
            gmds::math::Point pV2 = computePointOnBranch(p1, p4, p7);
            gmds::math::Point pV3 = computePointOnBranch(p2, p5, p8);

            gmds::math::Point pH1 = computePointOnBranch(p0, p1, p2);
            gmds::math::Point pH2 = computePointOnBranch(p3, p4, p5);
            gmds::math::Point pH3 = computePointOnBranch(p6, p7, p8);

            gmds::math::Point pV = computePointOnBranch(pV1, pV2, pV3);
            gmds::math::Point pH = computePointOnBranch(pH1, pH2, pH3);

            gmds::math::Point p({0.5*(pV+pH).X(), 0.5*(pV+pH).Y(), 0.5*(pV+pH).Z()});
            return p;

        }
/*----------------------------------------------------------------------------*/
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/