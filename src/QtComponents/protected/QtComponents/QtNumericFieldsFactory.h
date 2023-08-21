/**
 * \file		QtNumericFieldsFactory.h
 * \author		Charles PIGNEROL
 * \date		09/01/2015
 */


#ifndef QT_NUMERIC_FIELDS_FACTORY_H
#define QT_NUMERIC_FIELDS_FACTORY_H

#include <QtUtil/QtDoubleTextField.h>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Fabrique de champs de texte spécialisés pour <I>Magix 3D</I>.
 * Objectif : homogénéiser leurs aspects et comportements, pouvoir faire du
 * copier/coller d'un champ à l'autre de même type.
 * Mais attention, certains champs sont génériques, par exemple ceux des
 * propriétés individuelles ou communes de la sélection.
 */
class QtNumericFieldsFactory
{
	public :

	/**
	 * \param	Widget parent
	 * \param	<I>true</I> s'il faut procéder à une validation automatique
	 *			lors de la perte de focus ou d'une frappe clavier. 
	 * \param	Nom du widget
	 * \return	Un champ de saisie d'un réel double précision pour une
	 * 			composante de position.
	 */
	static QtDoubleTextField& createPositionTextField (
			QWidget *parent, bool autoValidation = false, const char* name = 0);

	/**
	 * \param	Widget parent
	 * \param	<I>true</I> s'il faut procéder à une validation automatique
	 *			lors de la perte de focus ou d'une frappe clavier. 
	 * \param	Nom du widget
	 * \return	Un champ de saisie d'un réel double précision pour une
	 * 			distance.
	 */
	static QtDoubleTextField& createDistanceTextField (
			QWidget *parent, bool autoValidation = false, const char* name = 0);

	/**
	 * \param	Widget parent
	 * \param	<I>true</I> s'il faut procéder à une validation automatique
	 *			lors de la perte de focus ou d'une frappe clavier. 
	 * \param	Nom du widget
	 * \return	Un champ de saisie d'un réel double précision pour un ratio.
	 */
	static QtDoubleTextField& createRatioTextField (
			QWidget *parent, bool autoValidation = false, const char* name = 0);

	/**
	 * \param	Widget parent
	 * \param	<I>true</I> s'il faut procéder à une validation automatique
	 *			lors de la perte de focus ou d'une frappe clavier. 
	 * \param	Nom du widget
	 * \return	Un champ de saisie d'un réel double précision pour un angle.
	 */
	static QtDoubleTextField& createAngleTextField (
			QWidget *parent, bool autoValidation = false, const char* name = 0);

	/** La taille la plus grande d'un réel double précision
	 * (<I>-NumericServices::doubleMachMax</I>)(en fait sa représentation). */
	const static std::string	doubleMaxLength;

	/** Le nombre de caractères de la partie entière signée (ex : -1). */
	const static unsigned char	doubleIntegerPartMaxLength;

	/** Le nombre de caractères de la partie exponentielle signée avec son 
	 * indicateur (ex : E-308). */
	const static unsigned char	doubleExponentPartMaxLength;


	private :

	/**
	 * Constructeurs et destructeurs : interdits.
	 */
	QtNumericFieldsFactory ( );
	QtNumericFieldsFactory (const QtNumericFieldsFactory&);
	QtNumericFieldsFactory& operator = (const QtNumericFieldsFactory&);
	~QtNumericFieldsFactory ( );
};	// class QtNumericFieldsFactory

}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_NUMERIC_FIELDS_FACTORY_H
