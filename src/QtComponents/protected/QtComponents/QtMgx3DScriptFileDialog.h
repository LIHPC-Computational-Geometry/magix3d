/*----------------------------------------------------------------------------*/
/** \file QtMgx3DScriptFileDialog.h
 *
 *  \author Charles Pignerol
 *
 *  \date 04/09/2014
 */

#ifndef QT_MGX3D_SCRIPT_FILE_DIALOG_H
#define QT_MGX3D_SCRIPT_FILE_DIALOG_H


#include <QtUtil/QtCharsetComboBox.h>
#include <QFileDialog>
#include <QCheckBox>

namespace Mgx3D
{

namespace QtComponents
{

/**
 * Widget type "File dialog" permettant de sélectionner un fichier script Magix 3D.
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class QtMgx3DScriptFileDialog : public QFileDialog 
{
	public :

	/**
	 * Constructeur
	 * @param		parent
	 * @param		titre de l'application
	 * @param		<I>true</I> s'il s'agit d'une boite de dialogue  d'ouverture de fichiers, <I>false</I> s'il s'agit d'une boite de dialogue d'enregistrement.
	 * @param		<I>true</I> si l'utilisateur peut choisir des options d'encodage du script, <I>false</I> dans le cas contraire.
	 * @param		<I>true</I> si l'utilisateur peut choisir d'enregistrer en plus des informations liées à l'environnement python, <I>false</I> dans le cas contraire.
	 * @param		Jeu de caractères proposé par défaut pour l'encodage du fichier (cas boite de dialogue d'enregistrement).
	 * @param		caractère modal de la boite de dialogue
	 * Invoque createGui.
	 */
	QtMgx3DScriptFileDialog (QWidget* parent, const char* title, bool open, bool withScriptEncodage, bool withEnvironment, TkUtil::Charset::CHARSET charset, bool modal);

	/**
	 * Destructeur.
	 */
	virtual ~QtMgx3DScriptFileDialog ( );

	/**
	 * Appelé lorsque l'utilisateur valide le choix.
	 */
	virtual void accept ( );

	/**
	 * @return      le type de références pour les entités
	 */
	virtual unsigned int getEncodageScript ( ) const;

	/**
	 * @return		<I>true</I> s'il faut enregistrer des informations complémentaires liées à l'environnement python.
	 */
	virtual bool getEnvironmentPython ( ) const;
	virtual void setEnvironmentPython (bool set);
	
	/**
	 * Par défaut proposer l'enregistrement des noms d'entités, des id ou des coordonnées
	 * Permet de retrouver la sélection d'une fois à l'autre
	 */
	static unsigned int encodageScript;

	/**
	 * @return	le jeu de caractères à utiliser pour encoder le fichier (cas boite de dialogue d'enregistrement).
	 */
	virtual TkUtil::Charset::CHARSET getCharset ( ) const;


	protected :

	/**
	 * Création des éléments complémentaires du sélecteur de fichier.
	 * @param		<I>true</I> s'il s'agit d'une boite de dialogue d'ouverture de fichiers, <I>false</I> s'il s'agit d'une boite de dialogue d'enregistrement.
	 * @param		<I>true</I> si l'utilisateur peut choisir d'enregistrer en plus des informations liées à l'environnement python, <I>false</I> dans le cas contraire.
	 * @param		<I>true</I> si l'utilisateur peut choisir des options d'encodage du script, <I>false</I> dans le cas contraire.
	 * @param		Jeu de caractères proposé par défaut pour l'encodage du fichier (cas boite de dialogue d'enregistrement).
	 */
	virtual void createGui (bool open, bool withScriptEncodage, bool withEnvironment, TkUtil::Charset::CHARSET charset);


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DScriptFileDialog (const QtMgx3DScriptFileDialog&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DScriptFileDialog& operator = (const QtMgx3DScriptFileDialog&);

	/** Le choix du type de références vers les entités dans le fichier sauvegardé. */
	QCheckBox*						_saveEntitiesNamesCheckBox;
	QCheckBox*						_saveEntitiesIdRefCheckBox;
	QCheckBox*						_saveEntitiesCoordCheckBox;
	QCheckBox*						_saveEnvironmentCheckBox;
	QtLabelisedCharsetComboBox*		_charsetComboBox;
};	// class QtMgx3DScriptFileDialog


}	// namespace QtComponents

}	// namespace Mgx3D



#endif	// QT_MGX3D_SCRIPT_FILE_DIALOG_H
