/**
 * \file		QtMgx3DTopoOperationAction.h
 * \author      Charles PIGNEROL
 * \date        15/01/2013
 */
#ifndef QT_TOPO_OPERATION_ACTION_H
#define QT_TOPO_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DOperationsPanel.h"

namespace Mgx3D
{

namespace QtComponents
{

/**
 * Classe de base d'action provoquant l'affichage d'un panneau de "Opération
 * topologique Magix 3D".
 * A spécialiser.
 * \see     QtMgx3DOperationAction
 */
class QtMgx3DTopoOperationAction : public QtMgx3DOperationAction
{
	public :

	/**
	 * Destructeur : RAS.
	 */
	virtual ~QtMgx3DTopoOperationAction ( );


	protected :

	/**
	 * Constructeur. RAS.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMgx3DTopoOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DTopoOperationAction (const QtMgx3DTopoOperationAction&);
	QtMgx3DTopoOperationAction& operator = (const QtMgx3DTopoOperationAction&);
};  // class QtMgx3DOperationAction


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPO_OPERATION_ACTION_H
