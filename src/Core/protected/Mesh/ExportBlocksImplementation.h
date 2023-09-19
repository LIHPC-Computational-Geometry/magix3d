/*----------------------------------------------------------------------------*/
/*
 * ExportBlocksImplementation.h
 *
 *  Created on: 27/06/23
 *      Author: calderan
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_EXPORTBLOCKSIMPLEMENTATION_H_
#define MGX3D_GEOM_EXPORTBLOCKSIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        class ExportBlocksImplementation
        {
        public:

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur (exportation de toutes entités du GeomManager)
             *
             *  \param c le contexte
             *  \param n le nom du fichier dans lequel se fait l'exportation
             */
            ExportBlocksImplementation(Internal::Context& c, const std::string& n);

            /*------------------------------------------------------------------------*/
            /** \brief   Destructeur
             */
            virtual ~ExportBlocksImplementation();


            /*------------------------------------------------------------------------*/
            /** \brief  réalisation de l'exportation
             */
            void perform(Internal::InfoCommand* icmd);

        private:

            /// contexte d'exécution
            Internal::Context& m_context;

            /// fichier dans lequel on exporte
            std::string m_filename;
        };

/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_EXPORTBLOCKSIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



