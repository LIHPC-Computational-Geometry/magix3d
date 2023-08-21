#ifndef QT_LANDMARK_DIALOG_H
#define QT_LANDMARK_DIALOG_H

#include "QtComponents/QtLandmarkPanel.h"
#include <QtUtil/QtDlgClosurePanel.h>
#include <TkUtil/UTF8String.h>
#include <QDialog> 

#include <string>


/**
 * Boite de dialogue Qt permettant la saisie des paramètres d'affichage d'un
 * repère composé de 3 axes.
 * \author	Charles Pignerol
 */
class QtLandmarkDialog : public QDialog 
{
	Q_OBJECT

	public :

	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de la boite de dialogue.
	 * @param		titre de l'application.
	 * @param		caractère modal de la boite de dialogue.
	 * @param		nom du premier axe.
	 * @param		nom du second axe.
	 * @param		nom du troisième axe.
	 * @param		paramètres du repère.
	 * @param		URL de l'aide
	 * @param		Balise de l'aide
	 */
	QtLandmarkDialog (QWidget* parent, const TkUtil::UTF8String& title, 
	                  const std::string& appTitle, bool modal,
	                  const std::string& xTitle,
	                  const std::string& yTitle,
	                  const std::string& zTitle,
	                  const LandmarkAttributes& parameters,
	                  const std::string& helpURL = "",
	                  const std::string& helpTag = "");


	/** Destructeur. */
	virtual ~QtLandmarkDialog ( );

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

	/** @return			le bouton "Appliquer" de la boite de dialogue. */
	virtual QPushButton* getApplyButton ( ) const;

	/** @return			le bouton "Annuler" de la boite de dialogue. */
	virtual QPushButton* getCancelButton ( ) const;


	public slots :

	/** Appelé quand l'utilisateur appuie sur le bouton "Appliquer".
	 * Invoque Dialog::accept.
	 * Envoie le signal "applied" avec en argument cette instance. 
	 */
	virtual void apply ( );

	/** Appelé quand l'utilisateur appuie sur le bouton "Annuler".
	 * Invoque Dialog::reject.
	 * Envoie le signal "canceled" avec en argument cette instance. 
	 */
	virtual void reject ( );


	signals :

	/** Envoyé à lorsque le bouton "Appliquer" est cliqué.
	 * @param		l'instance siège de l'évènement.
	 */
	void applied (QtLandmarkDialog* dialog);

	/** Envoyé à lorsque le bouton "Annuler" est cliqué.
	 * @param		l'instance siège de l'évènement.
	 */
	void canceled (QtLandmarkDialog* dialog);


	protected :


	private :

	/** Constructeur de copie. Interdit. */
	QtLandmarkDialog (const QtLandmarkDialog&);

	/** Opérateur de copie. Interdit. */
	QtLandmarkDialog& operator = (const QtLandmarkDialog&);

	/** Le panneau de paramétrage d'affichage des axes. */
	QtLandmarkPanel*					_landmarkPanel;

	/** Le panneau proposant "Appliquer" et "Annuler". */
	QtDlgClosurePanel*					_closurePanel;
};	// class QtLandmarkDialog



#endif	// QT_LANDMARK_DIALOG_H
