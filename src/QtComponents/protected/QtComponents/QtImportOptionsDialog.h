/**
 * \file		QtImportOptionsDialog.h
 * \author		Eric B
 * \date		17/09/2019
 */
#ifndef QT_IMPORT_OPTIONS_DIALOG_H
#define QT_IMPORT_OPTIONS_DIALOG_H


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
 * au format <I>STEP</I>, <I>IGES</I> ou <I>Catia</I>.
 */
class QtImportOptionsDialog : public QDialog
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
	QtImportOptionsDialog (QWidget* parent, const std::string& appTitle, const std::string& fileName);

	/**
	 * RAS.
	 */
	virtual ~QtImportOptionsDialog ( );

	/**
	 * \return	<I>true</I> si l'application doit décomposer les courbes composées en courbes élémentaires,
	 * <I>false</I> dans le cas contraire.
	 */
	virtual bool splitCompoundCurves ( ) const;

	/**
	 * \return	<I>true</I> si cette réponse s'applique aux choix à venir, <I>false</I> dans le cas contraire.
	 */
	virtual bool forAll ( ) const;

	protected :

	/**
	 * Créé l'IHM.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Nom du fichier <I>MDL</I> impoprté.
	 */
	virtual void createGui (QWidget* parent, const std::string& appTitle, const std::string& fileName);


	private :

	// Opérations interdites :
	QtImportOptionsDialog (const QtImportOptionsDialog&);
	QtImportOptionsDialog& operator = (const QtImportOptionsDialog&);

	/** Les cases à cocher correspondant aux différentes questions. */
	QCheckBox					*_splitCompoundCurvesCheckBox, *_forAllCheckBox;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel*			_closurePanel;
};	// class QtImportOptionsDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_IMPORT_OPTIONS_DIALOG_H
