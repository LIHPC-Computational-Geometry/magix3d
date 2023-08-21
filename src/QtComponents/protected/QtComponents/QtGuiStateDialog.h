/**
 * \file		QtGuiStateDialog.h
 * \author		Charles PIGNEROL
 * \date		29/11/2013
 */
#ifndef QT_GUI_STATE_DIALOG_H
#define QT_GUI_STATE_DIALOG_H


#include <TkUtil/File.h>
#include <PrefsCore/BoolNamedValue.h>

#include <QtUtil/QtDlgClosurePanel.h>

#include <QCheckBox>
#include <QDialog>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Boite de dialogue pemettant de modifier les paramètres de
 * sauvegarde/restauration de l'état d'une IHM (taille, position, tailles des
 * panneaux, ...).
 */
class QtGuiStateDialog : public QDialog
{
	Q_OBJECT

	public :

	/**
	 * Constructeur 1.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	<I>true</I> s'il faut restaurer l'état enregistré de l'IHM au
	 *			lancement de l'application, <I>false</I> dans le cas contraire.
	 * \param	<I>true</I> s'il faut sauvegarder automatiquement l'état de
	 *			l'IHM en quittant l'application, <I>false</I> dans le cas
	 *			contraire.
	 * \param	Nom du fichier de sauvegarde de l'état de l'IHM (à titre
	 *			informatif).
	 * \see		createGui
	 */
	QtGuiStateDialog (
			QWidget* parent, const std::string& appTitle, bool loadState,
			bool saveState, const std::string& fileName);

	/**
	 * Constructeur 2.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	<I>true</I> s'il faut restaurer l'état enregistré de l'IHM au
	 *			lancement de l'application, <I>false</I> dans le cas contraire.
	 * \param	<I>true</I> s'il faut sauvegarder automatiquement l'état de
	 *			l'IHM en quittant l'application, <I>false</I> dans le cas
	 *			contraire.
	 * \param	Nom du fichier de sauvegarde de l'état de l'IHM (à titre
	 *			informatif).
	 * \see		createGui
	 */
	QtGuiStateDialog (
			QWidget* parent, const std::string& appTitle,
			Preferences::BoolNamedValue* loadState,
			Preferences::BoolNamedValue* saveState,
			const std::string& fileName);

	/**
	 * RAS.
	 */
	virtual ~QtGuiStateDialog ( );

	/**
	 * Ferme la boite de dialogue et effectue (autant que possible) les actions
	 * demandées (selon les éléments transmis lors de la construction).
	 */
	virtual void accept ( );

	/**
	 * \return		<I>true</I> s'il faut restaurer l'état enregistré de l'IHM
	 *				au lancement de l'application, <I>false</I> dans le cas
	 *				contraire.
	 */
	virtual bool loadState ( ) const;

	/**
	 * \return		<I>true</I> s'il faut sauvegarder automatiquement l'état de
	 *				l'IHM en quittant l'application, <I>false</I> dans le cas
	 *				contraire.
	 */
	virtual bool saveState ( ) const;

	/**
	 * \return		<I>true</I> s'il faut créer une copie de sauvegarde du
	 * 				fichier actuel contenant l'état de l'IHM.
	 * \see			createSafeguard
	 */
	virtual bool createBackupCopy ( ) const;

	/**
	 * \return		<I>true</I> s'il faut sauvegarder immédiatement l'état
	 *				de l'IHM, <I>false</I> dans le cas contraire.
	 */
	virtual bool immediatelySaveState ( ) const;

	/**
	 * \return		<I>true</I> s'il faut détruire immédiatement l'état
	 *				de l'IHM, <I>false</I> dans le cas contraire.
	 */
	virtual bool immediatelyDestroyState ( ) const;


	protected :

	/**
	 * Créé l'IHM.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	<I>true</I> s'il faut restaurer l'état enregistré de l'IHM au
	 *			lancement de l'application, <I>false</I> dans le cas contraire.
	 * \param	<I>true</I> s'il faut sauvegarder automatiquement l'état de
	 *			l'IHM en quittant l'application, <I>false</I> dans le cas
	 *			contraire.
	 * \param	Nom du fichier de sauvegarde de l'état de l'IHM (à titre
	 *			informatif).
	 */
	virtual void createGui (
			QWidget* parent, const std::string& appTitle, bool loadState,
			bool saveState, const std::string& fileName);


	protected slots :

	/**
	 * Appelé si l'utilisateur coche une case provoquant la modification du
	 * fichier d'enregistrement de l'état de l'IHM.
	 * Propose une sauvegarde du fichier actuel.
	 */
	virtual void protectSafeguardFileCallback ( );


	protected :

	/**
	 * Effectue la sauvegarde du fichier actuel. Gère les erreurs, demande
	 * à l'utilisateur si nécessaire un nom de fichier.
	 */
	virtual void createSafeguard ( );


	private :

	// Opérations interdites :
	QtGuiStateDialog (const QtGuiStateDialog&);
	QtGuiStateDialog& operator = (const QtGuiStateDialog&);

	/** Les cases à cocher correspondant aux différentes questions. */
	QCheckBox					*_loadStateCheckBox, *_saveStateCheckBox;
	QCheckBox					*_backupCopyCheckBox,
								*_saveStateImmediatelyCheckBox,
								*_destroyStateCheckBox;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel*				_closurePanel;

	/** Le fichier enregistrant l'état de l'IHM. */
	TkUtil::File					_safeguardFile;

	/** Les préférences utilisateurs. */
	Preferences::BoolNamedValue		*_loadState, *_saveState;
};	// class QtGuiStateDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GUI_STATE_DIALOG_H
