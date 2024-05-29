#ifndef QT_AXE_PANEL_H
#define QT_AXE_PANEL_H

#include <TkUtil/PaintAttributes.h>
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtTextField.h>

#include "QtComponents/LandmarkAttributes.h"

#include <QCheckBox>

#include <string>


/**
 * Panneau Qt permettant de définir un axe. Un axe est composé de
 * son titre, des valeurs minimales et maximales, et de graduations.
 * Certaines de ces propriétés son visibles ou non, paramétrables
 * ou non.
 * \author	Charles Pignerol
 */
class QtAxisPanel : public QtGroupBox 
{
	Q_OBJECT

	public :

	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		titre du panneau.
	 * @param		paramètres de l'axe.
	 */
	QtAxisPanel (QWidget* parent, const std::string& appTitle, 
				const std::string& title, 
	            const AxisAttributes& parameters);

	/** Destructeur. */
	virtual ~QtAxisPanel ( );

	/**
	 * @return		les paramètres de l'axe.
	 */
	AxisAttributes getParameters ( ) const;

	/**
	 * @return		true si l'axe est visible, sinon false.
	 */
	virtual bool isVisible ( ) const;

	/**
	 * @return		la valeur minimale.
	 */
	virtual double getMin ( ) const;

	/**
	 * @return		la valeur maximale.
	 */
	virtual double getMax ( ) const;

	/**
	 * @return		le titre de l'axe.
	 */
	virtual std::string getTitle ( ) const;

	/**
	 * @return		la couleur.
	 */
	virtual TkUtil::Color getColor ( ) const
	{ return _color; }

	/**
	 * @return		true si des intitulés (titre, valeurs aux graduations)
	 * 				doivent être affichées, sinon false.
	 */
	virtual bool displayLabels ( ) const;

	/**
	 * @return		le format d'affichage des valeurs.
	 */
	std::string getDisplayFormat ( ) const;

	/**
	 * @return		true si des graduations doivent être affichées, sinon
	 *				false.
	 */
	virtual bool displayTicks ( ) const;

	/**
	 * @return		true si des graduations mineurs doivent être 
	 *				affichées, sinon false.
	 */
	virtual bool displayMinorTicks ( ) const;

	/**
	 * @return		true si des lignes parallèles doivent être 
	 *				affichées, sinon false.
	 */
	virtual bool displayLines ( ) const;


	protected slots :

	/**
	 * Appelé quand l'utilisateur clique sur le bouton "Couleur ...".
	 */
	virtual void setColorCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtAxisPanel (const QtAxisPanel&);

	/** Opérateur de copie. Interdit. */
	QtAxisPanel& operator = (const QtAxisPanel&);

	/** Le titre de l'application. */
	std::string							_appTitle;

	/** Le titre de l'axe. */
	QtTextField*						_titleTextField;

	/** Les valeurs minimales et maximales. */
	QtTextField							*_minTextField, *_maxTextField;

	/** Le format d'affichage des valeurs. */
	QtTextField*						_formatTextField;

	/** L'affichage de l'axe, des intitulés, des graduations, des
	 * graduations mineures et des lignes. */
	QCheckBox							*_visibleCheckBox, 
										*_labelsCheckBox, *_ticksCheckBox, 
										*_minorTicksCheckBox, *_linesCheckBox;

	/** La couleur de l'axe. */
	TkUtil::Color						_color;
};	// class QtAxisPanel



#endif	// QT_AXE_PANEL_H
