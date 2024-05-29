/**
 * \file		QtColorTablesEditorDialog.h
 * \author		Charles PIGNEROL
 * \date		23/01/2015
 */
#ifndef QT_COLOR_TABLES_EDITOR_DIALOG_H
#define QT_COLOR_TABLES_EDITOR_DIALOG_H

#include "Internal/ContextIfc.h"

#include "QtComponents/RenderingManager.h"

#include <QtUtil/QtDlgClosurePanel.h>
#include <QtUtil/QtDoubleTextField.h>
#include <QtUtil/QtIntTextField.h>

#include <QComboBox>
#include <QDialog>

#include <string>
#include <vector>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Boite de dialogue pemettant de modifier les paramètres de tables de couleurs
 * utilisées pour représenter des valeurs aux noeuds/mailles dans une vue
 * graphique.
 */
class QtColorTablesEditorDialog : public QDialog
{
	Q_OBJECT

	public :

	/**
	 * Constructeur 1.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Nom de la boite de dialogue.
	 * \param	Tables de couleurs éditables.
	 */
	QtColorTablesEditorDialog (
			QWidget* parent, const std::string& appTitle, const std::string& title,
			Mgx3D::QtComponents::RenderingManager& renderingManager,
			const std::vector< Mgx3D::QtComponents::RenderingManager::ColorTable* >& tables);

	/**
	 * RAS.
	 */
	virtual ~QtColorTablesEditorDialog ( );

	/**
	 * \return	Le titre de l'application.
	 */
	virtual std::string getAppTitle ( ) const;

	/**
	 * \return	La table de couleurs courante.
	 */
	virtual Mgx3D::QtComponents::RenderingManager::ColorTable& getColorTable( );

	/**
	 * \return	La définition de la table de couleurs saisi.
	 */
	virtual Mgx3D::QtComponents::RenderingManager::ColorTableDefinition
											getColorTableDefinition ( ) const;

	/**
	 * \return	Le nom de la table de couleurs saisi.
	 */
	virtual std::string getValueName ( ) const;

	/**
	 * \return	Le nombre de couleurs de la table de couleurs saisi.
	 */
	virtual size_t getColorNum ( ) const;

	/**
	 * \return	La valeur minimale affichée de la table de couleurs saisi.
	 */
	virtual double getMinimumValue ( ) const;

	/**
	 * \return	La valeur maximale affichée de la table de couleurs saisi.
	 */
	virtual double getMaximumValue ( ) const;


	protected slots :

	/**
	 * Appelé lorsque l'utilisateur change de table de couleurs. Actualise le
	 * panneau.
	 */
	virtual void colorTableCallback ( );

	/**
	 * Appelé lorsque l'utilisateur clique sur le bouton "Appliquer".
	 * Applique les modifications à la table de couleurs en cours.
	 */
	virtual void applyCallback ( );


	private :

	// Opérations interdites :
	QtColorTablesEditorDialog (const QtColorTablesEditorDialog&);
	QtColorTablesEditorDialog& operator = (const QtColorTablesEditorDialog&);

	/** Le titre de l'application. */
	const std::string			                                        _appTitle;

	/** Le gestionnaire de rendu associé. */
	Mgx3D::QtComponents::RenderingManager*                              _renderingManager;
	/** Les tables de couleurs gérées. */
	std::vector< Mgx3D::QtComponents::RenderingManager::ColorTable* >	_tables;
	
	/** Les cases à cocher correspondant aux différentes questions. */
	QComboBox*					                                        _tablesComboBox;
	QtIntTextField*				                                        _colorNumTextField;
	QtDoubleTextField*			                                        _minTextField;
	QtDoubleTextField*			                                        _maxTextField;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel*			                                        _closurePanel;
};	// class QtColorTablesEditorDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_COLOR_TABLES_EDITOR_DIALOG_H
