/**
 * \file		QtGroupsSelectorDialog.h
 * \author		Charles PIGNEROL
 * \date		31/01/2017
 */

#ifndef QT_GROUP_SELECTOR_DIALOG_H
#define QT_GROUP_SELECTOR_DIALOG_H

#include <TkUtil/util_config.h>
#include "Utils/SelectionManagerIfc.h"
#include "QtComponents/QtDimensionsSelectorPanel.h"

#include <QtUtil/QtDlgClosurePanel.h>
#include <QtUtil/QtIntTextField.h>

#include <QDialog>
#include <QLabel>

#include <set>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * \brief	Classe de boite de dialogue permettant de renseigner des critères
 * 			pour établir une sélection de groupes d'entités.
 */
class QtGroupsSelectorDialog : public QDialog
{
	Q_OBJECT

	public	:

	/**
	 * Constructeur. Délègue à la classe <I>QtDlgClosurePanel</I> l'éventuelle
	 * aide contextuelle.
	 * @param		Widget parent
	 * @param		Titre de la fenêtre
	 * @param		Gestionnaire de commande représenté. Ne doit pas être nul.
	 * @param		caractère modal de la boite de dialogue.
	 */
	QtGroupsSelectorDialog (
					QWidget* parent, const std::string& title);

	/**
	 * Destructeur : RAS.
	 */
	virtual ~QtGroupsSelectorDialog ( );

	/**
	 * \return		Les dimensions des groupes à sélectionner.
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM dimensions ( ) const;

	/**
	 * \return		Les niveaux des groupes à sélectionner.
	 */
	virtual const std::set<unsigned long>& levels ( ) const;

	/**
	 * \return		<I>true</I> s'il s'agit d'une nouvelle sélection,
	 * 				<I>false</I> s'il faut ajouter la sélection à celle
	 * 				existante.
	 */
	virtual bool newSelection ( ) const;


	protected slots :

	/**
	 * Ajoute/Enlève le niveau des niveaux sélectionnés.
	 */
	virtual void addLevelCallback ( );
	virtual void removeLevelCallback ( );

	/**
	 * Affiche un message d'erreur si les paramètres ne sont pas corrects,
	 * sinon ferme la boite de dialogue.
	 */
	virtual void accept ( );


	private	:

	/** 
	 * Constructeur de copie : interdit.
	 */
	QtGroupsSelectorDialog (const QtGroupsSelectorDialog&);

	/** 
	 * Opérateur = : interdit.
	 */
	QtGroupsSelectorDialog& operator = (const QtGroupsSelectorDialog&);

	/** Les dimensions retenues. */
	QtDimensionsSelectorPanel*		_dimensionsPanel;

	/** Les niveaux. */
	QtIntTextField*					_levelTextField;
	QLabel*							_levelsLabel;

	/** Nouvelle sélection ? */
	QCheckBox*						_newSelectionCheckBox;

	/**
	 * Le panneau contenant les boutons "OK" / "Annuler".
	 */
	QtDlgClosurePanel*				_closurePanel;

	/** Les niveaux déjà sélectionnés. */
	std::set<unsigned long>			_levels;
};	// class QtGroupsSelectorDialog

}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GROUP_SELECTOR_DIALOG_H
