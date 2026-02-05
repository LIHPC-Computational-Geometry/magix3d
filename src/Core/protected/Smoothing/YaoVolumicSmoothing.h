//
// Created by rochec on 02/02/2026.
//
// This class proposed a 3D extension of the Line-Sweeping smoother [1,2].
// The smoothing is performed block by block. The co-faces between the
// blocks are not changed by the smoother. To smooth a node in a block,
// we take the stencil made of 27 nodes, and we apply 9 times the
// 2D line-sweeping, 3 times in each direction, to get one branch of
// 3 points per direction. We then take the mean of the three mid
// points.
//
// [1] YAO, Jin. A mesh relaxation study and other topics.
//     Lawrence Livermore National Laboratory (LLNL), Livermore, CA (United States), 2013.
// [2] YAO, Jin et STILLMAN, Douglas. An equal-space algorithm for block-mesh improvement.
//     Procedia Engineering, 2016, vol. 163, p. 199-211.
#ifndef MAGIX3D_YAOVOLUMICSMOOTHING_H
#define MAGIX3D_YAOVOLUMICSMOOTHING_H
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
#include "Topo/CoFace.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        class YaoVolumicSmoothing
        {
        public:

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur
             *
             *  \param
             */
            YaoVolumicSmoothing(std::vector<Topo::Block*> &blocks, int nbIterations);

            /*------------------------------------------------------------------------*/
            /** \brief   Destructeur
             */
            virtual ~YaoVolumicSmoothing();


            /*------------------------------------------------------------------------*/
            /** \brief
             */
            void execute();

        private:
            /*------------------------------------------------------------------------*/
            /** \brief
             */
            gmds::math::Point computeNextNodePosition(Topo::Block* b, uint i, uint j, uint k);


            /*------------------------------------------------------------------------*/
            /** \brief
             */
            gmds::math::Point computePointOnBranch(gmds::math::Point const &p0,
                                                    gmds::math::Point const &p1,
                                                    gmds::math::Point const &p2);


            /*------------------------------------------------------------------------*/
            /** \brief
             *         p0           p1           p2
             *         0------------0------------0
             *         |            |            |
             *         |            |            |
             *      p3 0------------0------------0 p5
             *         |            | p4         |
             *         |            |            |
             *         0------------0------------0
             *         p6           p7           p8
             */
            gmds::math::Point computePointOnFace(gmds::math::Point const &p0,
                                                 gmds::math::Point const &p1,
                                                 gmds::math::Point const &p2,
                                                 gmds::math::Point const &p3,
                                                 gmds::math::Point const &p4,
                                                 gmds::math::Point const &p5,
                                                 gmds::math::Point const &p6,
                                                 gmds::math::Point const &p7,
                                                 gmds::math::Point const &p8);

        private:
            /** gmds mesh */
            gmds::Mesh* m_gmds_mesh;
            /** co-faces classified on the geometric surface to smooth */
            std::vector<Topo::Block*>& m_blocks;
            /** smoothing number of iterations */
            int m_nbIterations;
        };

/*----------------------------------------------------------------------------*/
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif //MAGIX3D_YAOVOLUMICSMOOTHING_H