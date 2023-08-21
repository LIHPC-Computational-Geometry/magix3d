#ifndef	LANDMARK_ATTRIBUTES_H
#define LANDMARK_ATTRIBUTES_H

#include <TkUtil/PaintAttributes.h>

#include <string>



/** 
 * Structure décrivant les paramètres d'un axe. 
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
struct AxisAttributes
{
	/** Constructeur : initialisation à :
	 * - false des booléens (exceptions : visible),
	 * - "" des chaines de caractères (exception faite de format : %g),,
	 * - 0 des valeurs numériques,
	 * - Color (0., 0., 0.) des couleurs.
	 */
	AxisAttributes ( );

	/** Constructeur de copie : RAS. */
	AxisAttributes (const AxisAttributes&);

	/** Opérateur = : RAS. */
	AxisAttributes& operator = (const AxisAttributes&);

	/** visibilité (true) ou non (false) de l'axe. */
	bool							visible;

	/** valeurs extrémales (min et max). */
	double							min, max;

	/** titre de l'axe. */	
	std::string						title;

	/** couleur de l'axe. */
	TkUtil::Color					color;

	/** L'épaisseur des axes. */
	double							lineWidth;

	/** affichage (true) ou non (false) des intitulés (titres et valeurs
	 * aux graduations). */
	bool							displayLabels;

	/** format d'affichage des valeurs. */
	std::string						format;

	/** visibité (true) ou non (false) des graduations. */
	bool							displayTicks;

	/** visibité (true) ou non (false) des graduations mineurs. */
	bool							displayMinorTicks;

	/** visibité (true) ou non (false) des lignes de graduation 
	 *  parallèles à l'axe. */
	bool							displayLines;
};	// struct AxisAttributes


/**
 * Structure décrivant les paramètres d'un repère composé de 3 axes.
 */
struct LandmarkAttributes
{
	/** Constructeur. autoAdjust initialisé à false. */
	LandmarkAttributes ( )
		: axis1 ( ), axis2 ( ), axis3 ( ), autoAdjust (true)
	{ }

	/** Constructeur de copie : RAS. */
	LandmarkAttributes (const LandmarkAttributes& copied)
		: axis1 (copied.axis1), axis2 (copied.axis2), axis3 (copied.axis3),
		  autoAdjust (copied.autoAdjust)
	{ }

	/** Opérateur = : RAS. */
	LandmarkAttributes& operator = (const LandmarkAttributes& copied)
	{
		if (&copied == this)
			return *this;

		axis1		= copied.axis1;
		axis2		= copied.axis2;
		axis3		= copied.axis3;
		autoAdjust	= copied.autoAdjust;

		return *this;
	}	// operator =

	/** Le trois axes. */
	AxisAttributes		axis1, axis2, axis3;

	/** Le repère doit il être automatiquement redimensioné lorsque
	 * le théatre est modifié ? */
	bool				autoAdjust;
};	// struct LandmarkAttributes


inline AxisAttributes::AxisAttributes ( )
	: visible (true), min (0.), max (0.), title ( ), 
	  color (0, 0, 0), lineWidth (1.), displayLabels (true), format ("%g"),
	  displayTicks (false), displayMinorTicks (false), displayLines (false)
{
}	// AxisAttributes::AxisAttributes


inline AxisAttributes::AxisAttributes (const AxisAttributes& copied)
	: visible (copied.visible), min (copied.min), max (copied.max), 
	  title (copied.title), 
	  color (copied.color), lineWidth (copied.lineWidth),
	  displayLabels (copied.displayLabels),
	  format (copied.format), displayTicks (copied.displayTicks),
	  displayMinorTicks (copied.displayMinorTicks), 
	  displayLines (copied.displayLines)
{
}	// AxisAttributes::AxisAttributes (const AxisAttributes&)


inline AxisAttributes& AxisAttributes::operator = (const AxisAttributes& copied)
{
	if (&copied == this)
		return *this;

	visible				= copied.visible;
	min					= copied.min;
	max					= copied.max;
	title				= copied.title;
	color				= copied.color;
	lineWidth			= copied.lineWidth;
	displayLabels		= copied.displayLabels;
	format				= copied.format;
	displayTicks		= copied.displayTicks;
	displayMinorTicks	= copied.displayMinorTicks;
	displayLines		= copied.displayLines;

	return *this;
}	// AxisAttributes::operator =



#endif	// LANDMARK_ATTRIBUTES_H
