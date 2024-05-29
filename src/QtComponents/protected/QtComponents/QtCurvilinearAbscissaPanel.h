/**
 * \file        QtCurvilinearAbscissaPanel.h
 * \author      Charles PIGNEROL
 * \date        09/09/2013
 */

#ifndef QT_CURVILINEAR_ABSCISSA_PANEL_H
#define QT_CURVILINEAR_ABSCISSA_PANEL_H

#include <TkUtil/util_config.h>

#include <QtComponents/QtMgx3DEntityPanel.h>
#include <Internal/ContextIfc.h>

#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>

#include <string>
#include <vector>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Widget de saisie d'une abscisse curviligne sur une courbe.
 */
class QtCurvilinearAbscissaPanel : public QWidget 
{
	public :

	/** Constructeur
	 * \param		widget parent. Ne doit pas être nul.
	 * \param		Le nom de l'application, pour les éventuels messages
	 *				d'erreur.
	 * \param		Fenêtre principale associée, pour la saisie des entités.
	 */
	QtCurvilinearAbscissaPanel (
						QWidget* parent, const std::string& appTitle,
						Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow);

	/** Destructeur. */
	virtual ~QtCurvilinearAbscissaPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		La courbe sélectionnée.
	 * \see			setCurveUniqueName
	 */
	virtual std::string getCurveUniqueName ( ) const;

	/**
	 * \param		Nouvelle courbe sélectionnée.
	 * \see			getCurveUniqueName
	 */
	virtual void setCurveUniqueName (const std::string& name);

	/**
	 * \return		L'abscisse curviligne du vertex.
	 */
	virtual double getCurvilinearAbscissa ( ) const;

	/**
 	 * \return		Le champ de saisie de la courbe.
 	 */
	virtual QtMgx3DEntityPanel& getCurvePanel ( );

	/**
	 * \return		Le champ de saisie de l'abscisse curviligne.
	 */
	virtual QtDoubleTextField& getCurvilinearAbscissaPanel ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );


	protected :


	private :

	/** Constructeur de copie. Interdit. */
	QtCurvilinearAbscissaPanel (const QtCurvilinearAbscissaPanel&);

	/** Opérateur de copie. Interdit. */
	QtCurvilinearAbscissaPanel& operator = (const QtCurvilinearAbscissaPanel&);

	/** Le nom de l'appliication, pour les éventuels messages d'erreur. */
	std::string						_appName;

	/** Le champ de saisie de la courbe. */
	QtMgx3DEntityPanel*				_curvePanel;

	/** Le champ de saisie manuelle. */
	QtDoubleTextField*				_curvilinearTextField;
};	// class QtCurvilinearAbscissaPanel

}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_CURVILINEAR_ABSCISSA_PANEL_H
