/**
 * \file		QtMgx3DGeomOperationAction.h
 * \author      Charles PIGNEROL
 * \date        11/12/2012
 */
#ifndef QT_GEOM_OPERATION_ACTION_H
#define QT_GEOM_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DOperationsPanel.h"

namespace Mgx3D
{

namespace QtComponents
{

/**
 * Classe de base d'action provoquant l'affichage d'un panneau de "Opération
 * géométrique Magix 3D".
 * A spécialiser.
 * \see     QtMgx3DOperationAction
 */
class QtMgx3DGeomOperationAction : public QtMgx3DOperationAction
{
	public :

	/**
	 * Destructeur : RAS.
	 */
	virtual ~QtMgx3DGeomOperationAction ( );


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
	QtMgx3DGeomOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DGeomOperationAction (const QtMgx3DGeomOperationAction&);
	QtMgx3DGeomOperationAction& operator = (const QtMgx3DGeomOperationAction&);
};  // class QtMgx3DOperationAction


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_OPERATION_ACTION_H
