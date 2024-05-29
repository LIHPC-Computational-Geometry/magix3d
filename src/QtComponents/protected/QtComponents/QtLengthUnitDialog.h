/**
 * \file		QtLengthUnitDialog.h
 * \author		Charles PIGNEROL
 * \date		30/09/2020
 */
#ifndef QT_LENGTH_UNIT_DIALOG_H
#define QT_LENGTH_UNIT_DIALOG_H

#include "Utils/Unit.h"
#include <QtUtil/QtDlgClosurePanel.h>

#include <QButtonGroup>
#include <QRadioButton>
#include <QDialog>

namespace Mgx3D
{

namespace QtComponents
{

/**
 * Boite de dialogue pemettant de sélectionner l'utité de longueur à utiliser.
 */
class QtLengthUnitDialog : public QDialog
{
	public :

	/**
	 * Constructeur 1.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Unité de longueur courante.
	 */
	QtLengthUnitDialog (QWidget* parent, const std::string& appTitle, Mgx3D::Utils::Unit::lengthUnit current);

	/**
	 * RAS.
	 */
	virtual ~QtLengthUnitDialog ( );

	/**
	 * \return	<I>true</I> L'unité de longueur courante.
	 */
	virtual Mgx3D::Utils::Unit::lengthUnit lengthUnit ( ) const;


	protected :


	private :

	// Opérations interdites :
	QtLengthUnitDialog (const QtLengthUnitDialog&);
	QtLengthUnitDialog& operator = (const QtLengthUnitDialog&);

	/** Les cases à cocher correspondant aux différentes questions. */
	QButtonGroup                *_buttonGroup;
	QRadioButton				*_umRadioButton, *_mmRadioButton, *_cmRadioButton, *_mRadioButton, *_undefinedRadioButton;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel           *_closurePanel;
};	// class QtLengthUnitDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_LENGTH_UNIT_DIALOG_H
