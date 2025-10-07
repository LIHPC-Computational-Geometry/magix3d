/*----------------------------------------------------------------------------*/
/*
 * ExportBlocksCGNSImplementation.h
 *
 *  Created on: 27/06/23
 *      Author: calderan
 */
/*----------------------------------------------------------------------------*/
#ifndef EXPORTBLOCKSCGNSIMPLEMENTATION_H_
#define EXPORTBLOCKSCGNSIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        class ExportBlocksCGNSImplementation
        {
        public:

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur (exportation de toutes entités du GeomManager)
             *
             *  \param c le contexte
             *  \param n le nom du fichier dans lequel se fait l'exportation
             */
            ExportBlocksCGNSImplementation(Internal::Context& c, const std::string& filename);

            /*------------------------------------------------------------------------*/
            /** \brief   Destructeur
             */
            virtual ~ExportBlocksCGNSImplementation();


            /*------------------------------------------------------------------------*/
            /** \brief  réalisation de l'exportation
             */
            void perform(Internal::InfoCommand* icmd);

        private:

            /// contexte d'exécution
            Internal::Context& m_context;

            std::string m_path;

            /// fichier dans lequel on exporte
            std::string m_filename;

            std::string m_basename;
        };

/*----------------------------------------------------------------------------*/
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EXPORTBLOCKCGNSSIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



