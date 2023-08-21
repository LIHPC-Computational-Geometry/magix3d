#ifndef QT_LANDMARK_PANEL_H
#define QT_LANDMARK_PANEL_H

#include <QtUtil/QtGroupBox.h>
#include <QtComponents/QtAxisPanel.h>

#include <QCheckBox>

#include <string>


/**
 * Panneau Qt permettant le paramétrage d'un repère composé de 3 axes.
 * \author	Charles PIGNEROL
 * \date	16/03/2016
 */
class QtLandmarkPanel : public QtGroupBox 
{
	public :

	/** Constructeur
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre du panneau.
	 * @param		nom du premier axe.
	 * @param		nom du second axe.
	 * @param		nom du troisième axe.
	 * @param		paramètres du repère.
	 */
	QtLandmarkPanel (QWidget* parent, const std::string& title,
	                 const std::string& xTitle,
	                 const std::string& yTitle,
	                 const std::string& zTitle,
	                 const LandmarkAttributes& parameters);

	/** Destructeur. */
	virtual ~QtLandmarkPanel ( );

	/**
	 * @return			les paramètres du repère.
	 */
	virtual LandmarkAttributes getParameters ( ) const;

	/**
	 * @return			les paramètres du premier axe.
	 */
	virtual AxisAttributes xParameters ( ) const;

	/**
	 * @return			les paramètres du second axe.
	 */
	virtual AxisAttributes yParameters ( ) const;

	/**
	 * @return			les paramètres du troisième axe.
	 */
	virtual AxisAttributes zParameters ( ) const;

	/**
	 * @return			true si les axes doivent être redimensionnés
	 *					automatiquement lorsque le théatre est modifié, 
	 *					ou false.
	 */
	virtual bool autoAdjust ( ) const;


	protected :


	private :

	/** Constructeur de copie. Interdit. */
	QtLandmarkPanel (const QtLandmarkPanel&);

	/** Opérateur de copie. Interdit. */
	QtLandmarkPanel& operator = (const QtLandmarkPanel&);

	/** Les panneau de paramétrage des 3 axes. */
	QtAxisPanel*			_xPanel;
	QtAxisPanel*			_yPanel;
	QtAxisPanel*			_zPanel;

	/** Redimensionnement automatique des axes. */
	QCheckBox*			_autoAdjustCheckBox;
};	// class QtLandmarkPanel



#endif	// QT_LANDMARK_PANEL_H
