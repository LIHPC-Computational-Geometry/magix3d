//
// Created by rochec on 28/01/2026.
//
// This class proposed a simplified version of the Line-Sweeping smoother [1,2].
// Here, it is designed to perform smoothing of 2D meshes on 3D surfaces.
// To do so, we add two main modifications.
//     1. We perform the method on a 2D mesh, which lies on a 3D surface.
//        This resulting point computed by the method is projected onto
//        the geometric surface at each step.
//     2. Instead of computing the intersection of the two mid-lines, we
//        compute the mean between the mid-points of the two mid-lines.
//        this is necessary because we work on 3D surfaces, and there is
//        no reason the two mid-lines intersect.
//
// [1] YAO, Jin. A mesh relaxation study and other topics.
//     Lawrence Livermore National Laboratory (LLNL), Livermore, CA (United States), 2013.
// [2] YAO, Jin et STILLMAN, Douglas. An equal-space algorithm for block-mesh improvement.
//     Procedia Engineering, 2016, vol. 163, p. 199-211.
//
//
/*----------------------------------------------------------------------------*/
#ifndef MAGIX3D_YAOSURFACICSMOOTHING_H
#define MAGIX3D_YAOSURFACICSMOOTHING_H
/*----------------------------------------------------------------------------*/
#include "Geom/Surface.h"
#include "Topo/CoFace.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        class YaoSurfacicSmoothing
        {
        public:

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur
             *
             *  \param
             */
            YaoSurfacicSmoothing(std::vector<Topo::CoFace*>& cofaces, Geom::Surface* surface, int nbIterations);

            /*------------------------------------------------------------------------*/
            /** \brief   Destructeur
             */
            virtual ~YaoSurfacicSmoothing();


            /*------------------------------------------------------------------------*/
            /** \brief
             */
            void execute();

        private:
            /*------------------------------------------------------------------------*/
            /** \brief
             */
            Utils::Math::Point computeNextNodePosition(Topo::CoFace* coface, int i, int j);


            /*------------------------------------------------------------------------*/
            /** \brief
             */
            gmds::math::Point computePointOnBranch(gmds::math::Point const &p0,
                                                    gmds::math::Point const &p1,
                                                    gmds::math::Point const &p2);


        private:
            /** geometric surface to smooth */
            Geom::Surface* m_surface;
            /** co-faces classified on the geometric surface to smooth */
            std::vector<Topo::CoFace*>& m_cofaces;
            /** smoothing number of iterations */
            int m_nbIterations;

        };

/*----------------------------------------------------------------------------*/
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif //MAGIX3D_YAOSURFACICSMOOTHING_H