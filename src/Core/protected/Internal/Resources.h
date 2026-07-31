//
// Created by calderans on 29/07/2026.
//

#ifndef MAGIX3D_RESOURCES_H
#define MAGIX3D_RESOURCES_H

#include <PrefsCore/BoolNamedValue.h>
#include <PrefsCore/ColorNamedValue.h>
#include <PrefsCore/DoubleNamedValue.h>
#include <PrefsCore/LongNamedValue.h>
#include <PrefsCore/StringNamedValue.h>
#include <PrefsCore/UnsignedLongNamedValue.h>
#include <PrefsCore/Section.h>
#include <TkUtil/Log.h>


namespace Mgx3D
{

    namespace Internal
    {

        /**
         * Les ressources de l'application à l'exécution, pour l'essentiel des "préférences utilisateur" lues en configuration et éventuellement surchargées à la ligne de commande.
         *
         * A finaliser : mettre de beaux accesseurs, de belles fonctions load, save, ...
         */
        class Resources
        {
        public :

        static Resources& instance ( );

            /**
 * Une commande peut-elle être autorisée à être décomposée en plusieurs tâches exécutées parallèlement dans plusieurs threads ?
 */
            Preferences::BoolNamedValue					_allowThreadedCommandTasks;

            /**
 * Le prémaillage des arêtes, faces et blocs peut il être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads ?
 */
            Preferences::BoolNamedValue					_allowThreadedEdgePreMeshTasks, _allowThreadedFacePreMeshTasks, _allowThreadedBlockPreMeshTasks;

            /**
 * Le programme doit mémoriser le prémaillage des arêtes ?
 */
            Preferences::BoolNamedValue					_memorizeEdgePreMesh;

            /** Les scripts à exécuter au lancement de l'application (arguments de ligne de commande suivant -scripts. */
            std::vector<std::string>					_scripts;

        private :

        Resources ( );
        Resources (const Resources&);
        Resources& operator = (const Resources&);
        ~Resources ( );
        static Resources*	_instance;
        };	// class GUIResources

    }	// namespace Internal

}	// namespace Mgx3D

#endif //MAGIX3D_RESOURCES_H
