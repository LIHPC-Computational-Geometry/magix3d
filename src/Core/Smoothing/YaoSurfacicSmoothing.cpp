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
#include <omp.h>
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

/*
#pragma omp parallel
    {
        std::cout << "Test" << std::endl;
    }
  */
    // perturbation to test the mesh smoothing
    for (Topo::CoFace* cf:m_cofaces)
    {
        uint nbI;
        uint nbJ;
        cf->getNbMeshingNodes(nbI, nbJ);
        gmds::Node n00 = gmds_mesh.get<gmds::Node>(cf->getNode(0,0));
        gmds::Node nImJm = gmds_mesh.get<gmds::Node>(cf->getNode(nbI-1,nbJ-1));
        // update mesh with new positions
        for (uint j=1; j<nbJ-1; j++)
        {
            for (uint i=1; i<nbI-1; i++)
            {
                gmds::Node nIJ = gmds_mesh.get<gmds::Node>(cf->getNode(i,j));
                nIJ.setXYZ((n00.X()+nImJm.X())/2.0, (n00.Y()+nImJm.Y())/2.0, (n00.Z()+nImJm.Z())/2.0);
            }
        }
    }

    gmds::IGMeshIOService ioService(&gmds_mesh);
    gmds::VTKWriter vtkWriter(&ioService);
    vtkWriter.setCellOptions(gmds::F|gmds::N);
    vtkWriter.setDataOptions(gmds::F|gmds::N);
    vtkWriter.write("YaoSurfacicSmoothing_"+std::to_string(0)+".vtk");
    std::cout << "Nbr cofaces: " << m_cofaces.size() << std::endl;

    for (Topo::CoFace* cf:m_cofaces)
    {
        uint nbI;
        uint nbJ;
        cf->getNbMeshingNodes(nbI, nbJ);
        std::vector<gmds::TCellID> cfNodes = cf->nodes();

        // compute ratios
        std::vector<double> r_imin;
        std::vector<double> r_imax;
        std::vector<double> r_jmin;
        std::vector<double> r_jmax;
        r_imin.reserve(nbJ);
        r_imax.reserve(nbJ);
        r_jmin.reserve(nbI);
        r_jmax.reserve(nbI);
        computeRatios(cf, r_imin, r_imax, r_jmin, r_jmax);

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

                    double r_I   =     i*r_imax[j] + (nbI-1-i)*r_imin[j];
                    double r_Imu = (i-1)*r_imax[j] + (nbI-1-(i-1))*r_imin[j];
                    double r_Ipu = (i+1)*r_imax[j] + (nbI-1-(i+1))*r_imin[j];
                    r_I   =   r_I/(nbI-1);
                    r_Imu = r_Imu/(nbI-1);
                    r_Ipu = r_Ipu/(nbI-1);
                    double r_J   =     j*r_jmax[i] + (nbJ-1-j)*r_jmin[i];
                    double r_Jmu = (j-1)*r_jmax[i] + (nbJ-1-(j-1))*r_jmin[i];
                    double r_Jpu = (j+1)*r_jmax[i] + (nbJ-1-(j+1))*r_jmin[i];
                    r_J   =   r_J/(nbJ-1);
                    r_Jmu = r_Jmu/(nbJ-1);
                    r_Jpu = r_Jpu/(nbJ-1);

                    gmds::math::Point pImu = computePointOnBranch(nImuJmu.point(), nImuJ.point(), nImuJpu.point(), r_Imu);
                    gmds::math::Point pI   = computePointOnBranch(  nIJmu.point(),   nIJ.point(),   nIJpu.point(), r_I);
                    gmds::math::Point pIpu = computePointOnBranch(nIpuJmu.point(), nIpuJ.point(), nIpuJpu.point(), r_Ipu);

                    gmds::math::Point pJmu = computePointOnBranch(nImuJmu.point(), nIJmu.point(), nIpuJmu.point(), r_Jmu);
                    gmds::math::Point pJ   = computePointOnBranch(  nImuJ.point(),   nIJ.point(),   nIpuJ.point(), r_J);
                    gmds::math::Point pJpu = computePointOnBranch(nImuJpu.point(), nIJpu.point(), nIpuJpu.point(), r_Jpu);

                    gmds::math::Point pImid = computePointOnBranch(pImu, pI, pIpu, r_J);
                    gmds::math::Point pJmid = computePointOnBranch(pJmu, pJ, pJpu, r_I);

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
void
YaoSurfacicSmoothing::computeRatios(Topo::CoFace* coface,
                                    std::vector<double>& r_imin,
                                    std::vector<double>& r_imax,
                                    std::vector<double>& r_jmin,
                                    std::vector<double>& r_jmax)
{
    uint nbI;
    uint nbJ;
    coface->getNbMeshingNodes(nbI, nbJ);
    gmds::Mesh& gmds_mesh = getStdContext()->getMeshManager().getMesh()->getGMDSMesh();
    r_jmin.push_back(0.0);
    r_jmax.push_back(0.0);
    for (uint i=1; i<nbI-1; i++)
    {
        gmds::Node nImu = gmds_mesh.get<gmds::Node>(coface->getNode(i-1,0));
        gmds::Node nI   = gmds_mesh.get<gmds::Node>(coface->getNode(i,0));
        gmds::Node nIpu = gmds_mesh.get<gmds::Node>(coface->getNode(i+1,0));
        double lImu = (nI.point()-nImu.point()).norm();
        double lIpu = (nIpu.point()-nI.point()).norm();
        r_jmin.push_back(lImu/(lImu+lIpu));

        nImu = gmds_mesh.get<gmds::Node>(coface->getNode(i-1, nbJ-1));
        nI   = gmds_mesh.get<gmds::Node>(coface->getNode(i, nbJ-1));
        nIpu = gmds_mesh.get<gmds::Node>(coface->getNode(i+1, nbJ-1));
        lImu = (nI.point()-nImu.point()).norm();
        lIpu = (nIpu.point()-nI.point()).norm();
        r_jmax.push_back(lImu/(lImu+lIpu));
    }
    r_jmin.push_back(0.0);
    r_jmax.push_back(0.0);

    r_imin.push_back(0.0);
    r_imax.push_back(0.0);
    for (uint j=1; j<nbJ-1; j++)
    {
        gmds::Node nJmu = gmds_mesh.get<gmds::Node>(coface->getNode(0, j-1));
        gmds::Node nJ   = gmds_mesh.get<gmds::Node>(coface->getNode(0, j));
        gmds::Node nJpu = gmds_mesh.get<gmds::Node>(coface->getNode(0, j+1));
        double lJmu = (nJ.point()-nJmu.point()).norm();
        double lJpu = (nJpu.point()-nJ.point()).norm();
        r_imin.push_back(lJmu/(lJmu+lJpu));

        nJmu = gmds_mesh.get<gmds::Node>(coface->getNode(nbI-1, j-1));
        nJ   = gmds_mesh.get<gmds::Node>(coface->getNode(nbI-1, j));
        nJpu = gmds_mesh.get<gmds::Node>(coface->getNode(nbI-1, j+1));
        lJmu = (nJ.point()-nJmu.point()).norm();
        lJpu = (nJpu.point()-nJ.point()).norm();
        r_imax.push_back(lJmu/(lJmu+lJpu));
    }
    r_jmin.push_back(0.0);
    r_jmax.push_back(0.0);

}
/*----------------------------------------------------------------------------*/
gmds::math::Point
YaoSurfacicSmoothing::computePointOnBranch(gmds::math::Point const &p0,
                                           gmds::math::Point const &p1,
                                           gmds::math::Point const &p2,
                                           double const r)
{
    gmds::math::Point p({0,0,0});
    double const d1 = (p1-p0).norm();
    double const d2 = (p2-p1).norm();
    double const d = d1 + d2;
    if (r*d <= d1)
    {
        p = p0 + r*d*(p1-p0).normalize();
    }
    else
    {
        p = p1 + (r*d - d1)*(p2-p1).normalize();
    }
    return p;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/