/**
 * \file        QtAnglePanel.h
 * \author      Charles PIGNEROL
 * \date        17/12/2012
 */

#ifndef QT_ANGLE_PANEL_H
#define QT_ANGLE_PANEL_H

#include <TkUtil/util_config.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>
#include <QLabel>

#include "Utils/Constants.h"

#include <string>
#include <vector>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Widget de saisie d'un angle et proposant des valeurs prédéfinies.
 */
class QtAnglePanel : public QWidget 
{
	Q_OBJECT

	public :

	/** Constructeur
	 * \param		widget parent. Ne doit pas être nul.
	 * \param		Le nom de l'application, pour les éventuels messages
	 *				d'erreur.
	 * \param		Nombre de colonnes visibles
	 * \param		Nombre de décimales
	 * \param		angle par défaut.
	 * \param		unité de mesure de l'angle.
	 * \param		<I>true</I> si les valeurs personnalisées sont autorisées,
	 *				</I>false</I> dans le cas contraire.
	 */
	QtAnglePanel (QWidget* parent, const std::string& appTitle,
	              unsigned short visibleColumns, unsigned short decimals,
	              double angle, const std::string& unity,
	              bool allowCustomizedValues = true);

	/** Destructeur. */
	virtual ~QtAnglePanel ( );

	/**
	 * \return		L'angle saisi.
	 * \exception	Lève une exception si l'angle est hors domaine.
	 * \see			setRange
	 * \see			getCircleFraction
	 * \see			getPortion
	 */
	virtual double getAngle ( ) const;

//	/**
//	 * \return		La fraction d'angle par rapport à 360 degrés (ex : 4 pour
//	 * 				90 degrés, 2 pour 180 degrés, ...).
//	 * \see			getAngle
//	 * \see			getPortion
//	 * \exception	Une exception est levée si l'angle ne correspond pas à une
//	 *				fraction entière.
//	 */
//	virtual unsigned short getCircleFraction ( ) const;

	/**
	 * \return		La portion d'angle, si elle existe, ou lève une exception.
	 * \see			getAngle
	 * \see			getCircleFraction
	 */
	virtual Mgx3D::Utils::Portion::Type getPortion ( ) const;

	/**
	 * \param		Valeur minimale acceptée pour l'angle.
	 * \param		Valeur maximale acceptée pour l'angle.
	 * \exception	Une exception est levée si min > max.
	 */
	virtual void setRange (double min, double max);

	/**
	 * Ajoute la valeur d'angle prédéfinie à la liste des valeurs prédéfinies
	 * proposées à l'utilisateur.
	 * \param		Nom de la valeur prédéfinie (ex : <I>Quart</I>).
	 * \param		Valeur associée (ex : <I>90<I>).
	 * \see			setPredefinedValue
	 */
	virtual void addPredefinedValue (const std::string& name, double value);

	/**
	 * \param		Nouvelle valeur prédéfinie courante.
	 * \exception	Une exception est levée s'il n'y a pas de valeur
	 *				prédéfinie de ce nom.
	 */
	virtual void setPredefinedValue (const std::string& name);

	/**
	 * Liste de valeurs prédéfinies : Entier/Demi/Quart/Sixième/Huitième.
	 */
	static const std::vector< std::pair<std::string, double> >&	eightthToWhole ( );

	/**
	 * Liste de valeurs prédéfinies : Demi/Quart/Sixième/Huitième.
	 */
	static const std::vector< std::pair<std::string, double> >&	eightthToHalf ( );


	/**
	 * Masque la comboBox avec sélection d'une portion
	 */
	void hidePartComboBox();


	signals :

	/**
	 * Signal émis lorsque l'angle est modifié.
	 * \param		Nouvelle valeur de l'angle.
	 */
	void angleModified (double);


	protected slots :

	/**
	 * Appelé lorsque l'utilisateur sélectionne une valeur prédéfinie au
	 * sélecteur <I>combo box</I>. Actualise le champ de texte de saisie.
	 * Emet le signal <I>angleModified</I>.
	 */
	virtual void predefinedValueCallback (int);

	/**
	 * Appelé lorsque l'utilisateur modifie manuellement la valeur de l'angle.
	 * Emet le signal <I>angleModified</I>.
	 */
	virtual void angleModifiedCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtAnglePanel (const QtAnglePanel&);

	/** Opérateur de copie. Interdit. */
	QtAnglePanel& operator = (const QtAnglePanel&);

	/** Le nom de l'appliication, pour les éventuels messages d'erreur. */
	std::string				_appName;

	/** Le label Portion */
	QLabel*	_partLabel;

	/** Le sélecteur de valeurs prédéfinies. */
	QComboBox*				_predefinedValuesComboBox;

	/** Le champ de saisie manuelle. */
	QtDoubleTextField*		_angleTextField;

	/** Les valeurs prédéfinies. */
	std::vector < std::pair < std::string, double > >		_predefinedValues;

	bool					_allowCustomizedValues;
};	// class QtAnglePanel

}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_ANGLE_PANEL_H
