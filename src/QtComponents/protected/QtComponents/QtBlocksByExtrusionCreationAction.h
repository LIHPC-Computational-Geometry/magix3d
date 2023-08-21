/**
 * \file		QtBlocksByExtrusionCreationAction.h
 * \author		Simon CALDERAN
 * \date		23/02/2022
 */

#ifndef QT_BLOCKS_BY_EXTRUSION_CREATION_ACTION_H
#define QT_BLOCKS_BY_EXTRUSION_CREATION_ACTION_H

#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"


#include "Utils/Vector.h"

namespace Mgx3D {

    namespace QtComponents {

        /**
        * Panneau de création de bloc topologique par extrusion de faces.
        */
        class QtBlocksByExtrusionCreationPanel : public QtMgx3DOperationPanel
        {
        public:

            /**
	        * Créé l'ihm.
	        * \param	Widget parent.
	        * \param	Nom du panneau.
	        * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	        *			notamment pour récupérer le contexte.
	        * \param	Eventuelle action associée à ce panneau.
	        */
            QtBlocksByExtrusionCreationPanel (
                    QWidget* parent, const std::string& panelName,
                    Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
                    Mgx3D::QtComponents::QtMgx3DOperationAction* action);

            /**
	        * RAS.
	        */
            virtual ~QtBlocksByExtrusionCreationPanel ( );

            /**
            * Réinitialise le panneau.
            */
            virtual void reset ( );

            /**
            * \return		La liste des entités impliquées dans l'opération.
            */
            virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

            /**
	        * \return		Les arêtes à partir desquelles est effectuée la création de
	        * 				bloc par rotation.
	        */
            virtual std::vector<std::string> getFaces ( ) const;

            /**
             * \return      Le vecteur d'extrusion
             */
            virtual Utils::Math::Vector getDirection () const;

            /**
            * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
            * En cas de paramètrage invalide cette méthode doit leve une exception de
            * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
            * rencontrées.
            */
            virtual void validate ( );

            /**
             * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
             * Restitue l'environnement dans son état initial.
             * \see		autoUpdate
             */
            virtual void cancel ( );

            /**
             * Actualise le panneau en fonction du contexte.
             * \see		cancel
             */
            virtual void autoUpdate ( );


        protected :

            /**
             * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
             * Invoque <I>preview (false, true).</I>
             * \see		preview
             */
            virtual void operationCompleted ( );

        private :

            /**
             * Constructeur de copie et opérateur = opérations interdites.
             */
            QtBlocksByExtrusionCreationPanel (
                    const QtBlocksByExtrusionCreationPanel&);
            QtBlocksByExtrusionCreationPanel& operator = (
                    const QtBlocksByExtrusionCreationPanel&);

            /** Les faces utilisées pour construire le bloc. */
            QtMgx3DEntityPanel*				_facesPanel;

            /** Le panneau définissant la direction dans laquelle est effectuée l'extrusion */
            QtMgx3DVectorPanel*							_directionPanel;

        };	// class QtBlocksByRevolutionCreationPanel

        /**
        * Classe d'action type <I>check box</I> associée à un panneau type
        * <I>QtBlocksByExtrusionCreationPanel</I> de création d'un bloc par rotation
        * d'arêtes.
        */
        class QtBlocksByExtrusionCreationAction : public QtMgx3DTopoOperationAction
        {
        public :

            /**
             * Créé et s'associe une instance de la classe
             * <I>QtBlocksByExtrusionCreationAction</I>.
             * \param		Icône représentant l'action.
             * \param		Texte représentant l'action.
             * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
             *				notamment pour récupérer le contexte et le panneau contenant
             *				les icônes.
             * \param		Tooltip décrivant l'action.
             * \param		Politique de création/modification du groupe
             */
            QtBlocksByExtrusionCreationAction (
                    const QIcon& icon, const QString& text,
                    Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
                    const QString& tooltip);

            /**
             * Destructeur. RAS.
             */
            virtual ~QtBlocksByExtrusionCreationAction ( );

            /**
             * \return		Le panneau d'édition du bloc associé.
             */
            virtual QtBlocksByExtrusionCreationPanel* getBlocksByExtrusionPanel ( );

            /**
             * Créé/modifie le bloc avec le paramétrage de son panneau associé.
             * Invoque préalablement
             * <I>QtMgx3DtopoOperationAction::executeOperation</I>.
             */
            virtual void executeOperation ( );


        private :

            /**
             * Constructeur de copie et opérateur = : interdits.
             */
            QtBlocksByExtrusionCreationAction (
                    const QtBlocksByExtrusionCreationAction&);
            QtBlocksByExtrusionCreationAction& operator = (
                    const QtBlocksByExtrusionCreationAction&);
        };  // class QtBlocksByRevolutionCreationAction

    }
}

#endif //QT_BLOCKS_BY_EXTRUSION_CREATION_ACTION_H

