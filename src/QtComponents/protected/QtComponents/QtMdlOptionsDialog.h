/**
 * \file		QtMdlOptionsDialog.h
 * \author		Charles PIGNEROL
 * \date		26/05/2014
 */
#ifndef QT_MDL_OPTIONS_DIALOG_H
#define QT_MDL_OPTIONS_DIALOG_H


#include <TkUtil/File.h>

#include <QtUtil/QtDlgClosurePanel.h>
#include <QtUtil/QtTextField.h>

#include <QCheckBox>
#include <QDialog>
#include <QSpinBox>

#include <string>

namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Boite de dialogue pemettant de modifier les paramètres d'importation de CAO
 * au format <I>MDL</I>.
 */
class QtMdlOptionsDialog : public QDialog
{
	Q_OBJECT

	public :

	/**
	 * Constructeur 1.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Nom du fichier <I>MDL</I> importé.
	 * \see		createGui
	 */
	QtMdlOptionsDialog (
			QWidget* parent, const std::string& appTitle, const std::string& fileName);

	/**
	 * RAS.
	 */
	virtual ~QtMdlOptionsDialog ( );

	/**
	 * \return	<I>true</I> si l'application doit importer la topologie du
	 *			fichier <I>MDL</I>, <I>false</I> dans le cas contraire.
	 */
	virtual bool importTopology ( ) const;

	/**
	 * \return	<I>true</I> si l'application doit importer tous les contours du
	 *			fichier <I>MDL</I>, <I>false</I> dans le cas contraire.
	 */
	virtual bool importAllContours ( ) const;

	/**
	 * \return	<I>true</I> si l'application doit retenir le nom des zones du
	 *			fichier <I>MDL</I>, <I>false</I> dans le cas contraire.
	 */
	virtual bool useAreaNames ( ) const;

	/**
	 * \return le préfixe pour les noms des groupes
	 */
	virtual std::string getPrefix ( ) const;

	/**
	 * \return le degrés min pour les B-Splines
	 */
	virtual int getDegMinBSpline() const;

	/**
	 * \return le degrés max pour les B-Splines
	 */
	virtual int getDegMaxBSpline() const;

	/**
	 * \return	<I>true</I> s'il faut décomposer le fichier mdl en commandes
	 *			Mgx 3D élémentaires, <I>false</I> dans le cas contraire.
	 */
	virtual bool splitMgx3DCommands ( ) const;


	protected :

	/**
	 * Créé l'IHM.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Nom du fichier <I>MDL</I> impoprté.
	 */
	virtual void createGui (
			QWidget* parent, const std::string& appTitle, const std::string& fileName);


	protected slots :

	/**
	 * Appelé lorsque l'utilisateur modifie la case à cocher "Décomposer en 
	 * opérations élémentaires". Actualise le panneau.
	 */
	virtual void splitCommandsCallback ( );


	private :

	// Opérations interdites :
	QtMdlOptionsDialog (const QtMdlOptionsDialog&);
	QtMdlOptionsDialog& operator = (const QtMdlOptionsDialog&);

	/** Les cases à cocher correspondant aux différentes questions. */
	QCheckBox					*_importTopologyCheckBox;
	QCheckBox					*_importAllContoursCheckBox;
	QCheckBox					*_useAreaNamesCheckBox;
	QCheckBox					*_splitCommandsCheckBox;
	/** Préfixe pour les noms des groupes */
	QtTextField                 *_prefixTextField;
	///	Degrés min et max pour les B-Splines
	QSpinBox *_degMinBSplineSpinBox, *_degMaxBSplineSpinBox;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel*				_closurePanel;
};	// class QtMdlOptionsDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MDL_OPTIONS_DIALOG_H
