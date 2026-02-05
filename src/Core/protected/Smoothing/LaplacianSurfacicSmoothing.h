//
// Created by rochec on 28/01/2026.
//
/*----------------------------------------------------------------------------*/
#ifndef MAGIX3D_LAPLACIANSURFACICSMOOTHING_H
#define MAGIX3D_LAPLACIANSURFACICSMOOTHING_H
/*----------------------------------------------------------------------------*/
#include "Geom/Surface.h"
#include "Topo/CoFace.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
    /*----------------------------------------------------------------------------*/
    namespace Mesh {
        /*----------------------------------------------------------------------------*/
        class LaplacianSurfacicSmoothing
        {
        public:

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur
             *
             *  \param
             */
            LaplacianSurfacicSmoothing(std::vector<Topo::CoFace*>& cofaces, Geom::Surface* surface, int nbIterations);

            /*------------------------------------------------------------------------*/
            /** \brief   Destructeur
             */
            virtual ~LaplacianSurfacicSmoothing();


            /*------------------------------------------------------------------------*/
            /** \brief Execute the smoothing algorithm.
             */
            void execute();



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
#endif //MAGIX3D_LAPLACIANSURFACICSMOOTHING_H