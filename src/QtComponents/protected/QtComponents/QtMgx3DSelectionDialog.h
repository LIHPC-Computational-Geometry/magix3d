/**
 * \file		QtMgx3DSelectionDialog.h
 * \author		Charles PIGNEROL
 * \date		12/05/2015
 */
#ifndef QT_MGX3D_SELECTION_DIALOG_H
#define QT_MGX3D_SELECTION_DIALOG_H


#include "QtComponents/QtMgx3DEntityPanel.h"

#include <QtUtil/QtDlgClosurePanel.h>

#include <QCheckBox>
#include <QDialog>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DMainWindow;


/**
 * Boite de dialogue pemettant de sélectionner des entités <I>Magix 3D</I> en
 * renseignant leurs identifiants.
 */
class QtMgx3DSelectionDialog : public QDialog
{
	public :

	/**
	 * Constructeur 1.
	 * \param	Fenêtre principale associée,
	 */
	QtMgx3DSelectionDialog (QtMgx3DMainWindow& mainWindow);

	/**
	 * RAS.
	 */
	virtual ~QtMgx3DSelectionDialog ( );

	/**
	 * Les entités sélectionnées.
	 */
	virtual std::vector<std::string> getUniqueNames ( ) const;

	/**
	 * \return	<I>true</I> si les entités sélectionnées doivent être affichées,
	 *			<I>false</I> dans le cas contraire.
	 */
	virtual bool displaySelectedEntities ( ) const;

	/**
	 * \return	<I>true</I> si les entités sélectionnées doivent remplacer la
	 *			sélection en cours, <I>false</I> si elles doivent y être ajouté.
	 */
	virtual bool replaceSelection ( ) const;

	/**
	 * Fermeture de la boite de dialogue. Réactive les menus et actions de la
	 * fenêtre principale.
	 */
	virtual void done (int r);


	private :

	// Opérations interdites :
	QtMgx3DSelectionDialog (const QtMgx3DSelectionDialog&);
	QtMgx3DSelectionDialog& operator = (const QtMgx3DSelectionDialog&);

	/** La fenêtre principale <I>Magix 3D</I> à laquelle est rattachée cette
	 * boite de dialogue. */
	QtMgx3DMainWindow*			_mainWindow;

	/** Le panneau de saisie interactive/textuelle. */
	QtMgx3DEntityPanel*			_entitiesPanel;

	/** Les cases à cocher correspondant aux différentes questions. */
	QCheckBox					*_displaySelectionCheckBox,
								*_replaceSelectionCheckBox;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel*			_closurePanel;

	bool						_actionsStatus;

	std::vector<std::string>	_entitiesNames;
};	// class QtMgx3DSelectionDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_SELECTION_DIALOG_H
