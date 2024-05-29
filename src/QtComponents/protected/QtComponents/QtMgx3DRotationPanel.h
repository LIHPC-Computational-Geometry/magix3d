/**
 * \file		QtMgx3DRotationPanel.h
 * \author		Charles PIGNEROL
 * \date		22/05/2014
 */

#ifndef QT_MGX3D_ROTATION_PANEL_H
#define QT_MGX3D_ROTATION_PANEL_H

#include "Internal/ContextIfc.h"

#include <TkUtil/util_config.h>
#include "Utils/Rotation.h"
#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <QButtonGroup>
#include <QCheckBox>

#include <string>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Panneau Qt permettant la définition d'une rotation dans Magix 3D.
 */
class QtMgx3DRotationPanel : public QWidget
{
	Q_OBJECT

	public :

	/** Constructeur
	 * \param		widget parent. Ne doit pas être nul.
	 * \param		titre du panneau
	 * \param		true si le panneau est éditable, sinon false.
	 * \param		Liste des couples de valeurs prédéfinies (nom/angle)
	 * \param		Valeur prédéfinie par défaut (ou -1)
	 * \param		Fenêtre principale associée, pour les saisies interactives 
	 *				(axe de rotation, ...)
	 */
	QtMgx3DRotationPanel (
		QWidget* parent, const std::string& title, bool editable,
		const std::vector< std::pair<std::string, double> >& predefinedValues,
		unsigned short defaultValue,
		Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow);

	/** Destructeur. */
	virtual ~QtMgx3DRotationPanel ( );


	/**
 	 * \return	La rotation définie par le panneau.
 	 */
	virtual Utils::Math::Rotation getRotation ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);


	signals :

	/**
	 * Signal émis lorsque l'utilisateur vient de finir une action de
	 * modification de la rotation.
	 */
	void rotationModified ( );


	protected slots :

	/**
	 * Callback appelé lorsque l'utilisateur modifie de manière interactive
	 * la rotation.
	 * Emet le signal <I>rotationModified</I>.
	 */
	virtual void rotationModifiedCallback ( );

	/**
	 * Callback appelé lorsque l'utilisateur modifie de manière interactive
	 * l'axe de rotation. Actualise l'IHM.
	 */
	virtual void axeModifiedCallback ( );

	/**
	 * Callback appelé lorsque l'utilisateur modifie de manière interactive
	 * le mode de saisie de l'axe de rotation. Actualise l'IHM.
	 */
	virtual void axeModeCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DRotationPanel (const QtMgx3DRotationPanel&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DRotationPanel& operator = (const QtMgx3DRotationPanel&);

	/**
	 * L'angle de rotation.
	 */
	QtAnglePanel*								_anglePanel;

	/**
	 * L'axe de rotation.
	 */
	QButtonGroup*								_axisButtonGroup;
	QCheckBox*									_xAxisCheckBox;
	QCheckBox*									_yAxisCheckBox;
	QCheckBox*									_zAxisCheckBox;
	QCheckBox*									_anyAxisCheckBox;

	/* Le mode de définition de l'axe de rotation dans le cas d'un axe
	 * quelconque. */
	QComboBox*									_anyAxisDefinitionModeComboBox;

	/** Le widget parent des panneaux de saisie interactive des axes de
	 * rotation quelconques. */
	QWidget*									_anyAxisDefinitionParentWidget;

	/** Les panneaux de définition de l'axe de rotation quelconque. */
	std::vector<QWidget*>						_anyAxisDefinitionWidgets;
};	// class QtMgx3DRotationPanel


}	// namespace QtComponents

}	// namespace Mgx3D



#endif	// QT_MGX3D_ROTATION_PANEL_H
