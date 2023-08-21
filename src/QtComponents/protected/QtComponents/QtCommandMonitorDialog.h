/**
 * \file		QtCommandMonitorDialog.h
 * \author		Charles PIGNEROL
 * \date		18/01/2011
 */

#ifndef QT_COMMAND_MONITOR_DIALOG_H
#define QT_COMMAND_MONITOR_DIALOG_H

#include <TkUtil/util_config.h>
#include "QtComponents/QtCommandMonitorPanel.h"

#include <QtUtil/QtDlgClosurePanel.h>

#include <QDialog>

namespace Mgx3D
{

namespace QtComponents
{


/**
 * \brief	Classe de boite de dialogue représentant un gestionnaire de commande
 *			de Magix 3D.
 */
class QtCommandMonitorDialog : public QDialog
{
	public	:

	/**
	 * Constructeur. Délègue à la classe <I>QtDlgClosurePanel</I> l'éventuelle
	 * aide contextuelle.
	 * @param		Widget parent
	 * @param		Titre de la fenêtre
	 * @param		Gestionnaire de commande représenté. Ne doit pas être nul.
	 * @param		caractère modal de la boite de dialogue.
	 */
	QtCommandMonitorDialog (
					QWidget* parent, const std::string& title, 
					Mgx3D::Utils::CommandManager* manager, bool modal);

	/**
	 * Destructeur : RAS.
	 */
	virtual ~QtCommandMonitorDialog ( );

	/**
	 * @return		Le panneau associé.
	 */
	QtCommandMonitorPanel& getCommandPanel ( );

	/**
	 * Affiche la boite de dialogue à la dernière position où elle a été.
	 */
	virtual void show ( );


	/**
	 * Mémorise la position courrante pour la réutiliser lors d'un appel
	 * ultérieur à <I>show</I>.
	 */
	virtual void done (int r);


	private	:

	/** 
	 * Constructeur de copie : interdit.
	 */
	QtCommandMonitorDialog (const QtCommandMonitorDialog&);

	/** 
	 * Opérateur = : interdit.
	 */
	QtCommandMonitorDialog& operator = (const QtCommandMonitorDialog&);

	/**
	 * Le panneau de représentation du gestionnaire de commandes. 
	 */
	QtCommandMonitorPanel			*_commandMonitorPanel;

	/**
	 * Le panneau contenant les boutons "OK" / "Annuler".
	 */
	QtDlgClosurePanel				*_closurePanel;

	/** La dernière position de la boite de dialogue avant d'être fermée.  */
	QPoint							_position;
};	// class QtCommandMonitorDialog

}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_COMMAND_MONITOR_DIALOG_H
