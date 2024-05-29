/**
 * \file		QtSegmentOperationAction.h
 * \author		Charles PIGNEROL
 * \date		10/09/2013
 */
#ifndef QT_SEGMENT_OPERATION_ACTION_H
#define QT_SEGMENT_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtCurvilinearAbscissaPanel.h"
#include "QtComponents/Qt2VerticiesPanel.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * un segment.
 */
class QtSegmentOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification du segment.
	 * <OL>
	 * <LI>Renseignement des vertex aux extrémités,
	 * </OL>
	 */
	enum OPERATION_METHOD {VERTEX_VERTEX};

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtSegmentOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtSegmentOperationPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création/modification du segment..
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées des vertices aux extrémités du segment
	 * (si <I>getOperationMethod ( )</I> retourne <I>COORDINATES</I>).
	 */
	virtual std::string getVertex1Name ( ) const;
	virtual std::string getVertex2Name ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
     * En cas de paramètrage invalide cette méthode doit leve une exception de
     * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
     * rencontrées.
     */
    virtual void validate ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );


	protected :

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des
	 * paramètres.
	 */
	virtual void operationMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtSegmentOperationPanel (const QtSegmentOperationPanel&);
	QtSegmentOperationPanel& operator = (const QtSegmentOperationPanel&);

	/** Le nom du groupe. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création/modification du segment. */
	QComboBox*						_operationMethodComboBox;

	/**
	 * Les panneaux de saisie des paramètres de définition du segment
	 */
	//{@
	/** Parent du panneau courant. */
	QWidget*						_currentParentWidget;

	/** Panneau courant. */
	QtMgx3DOperationsSubPanel*		_currentPanel;

	/** Renseignement des vertex aux extrémités. */
	Qt2VerticiesPanel*				_verticesPanel;
	//@}	// Panneaux de saisie des paramètres de définition du segment
};	// class QtSegmentOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSegmentOperationPanel</I> de création/modification d'un segment.
 */
class QtSegmentOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSegmentOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtSegmentOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSegmentOperationAction ( );

	/**
	 * \return		Le panneau d'édition du segment associé.
	 */
	virtual QtSegmentOperationPanel* getSegmentPanel ( );

	/**
	 * Créé/modifie le segment avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSegmentOperationAction (const QtSegmentOperationAction&);
	QtSegmentOperationAction& operator = (
									const QtSegmentOperationAction&);
};  // class QtSegmentOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SEGMENT_OPERATION_ACTION_H
