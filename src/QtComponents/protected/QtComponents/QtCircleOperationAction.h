/**
 * \file		QtCircleOperationAction.h
 * \author		Charles PIGNEROL
 * \date		09/09/2014
 */
#ifndef QT_CIRCLE_OPERATION_ACTION_H
#define QT_CIRCLE_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/Qt3VerticiesPanel.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant un cercle / une ellipse.
 */
class QtCircleOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification du cercle / de l'ellipse.
	 * <OL>
	 * <LI>Renseignement de 3 vertex (extrémités + centre du cercle)
	 * <LI>Renseignement de 3 vertex (point 1 + point 2 + centre de l'ellipse)
	 * </OL>
	 */
	enum OPERATION_METHOD { THREE_POINTS, ELLIPSE_THREE_POINTS };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtCircleOperationPanel (
			QWidget* parent, const std::string& panelName, QtMgx3DGroupNamePanel::POLICY creationPolicy, Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtCircleOperationPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création/modification du cercle.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées des vertices définissant le cercle.
	 * (si <I>getOperationMethod ( )</I> retourne <I>EXTREMITIES_CENTER</I>).
	 */
	virtual std::string getVertex1UniqueName ( ) const;
	virtual std::string getVertex2UniqueName ( ) const;
	virtual std::string getVertex3UniqueName ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
     * En cas de paramètrage invalide cette méthode doit leve une exception de type <I>TkUtil::Exception</I> contenant un descriptif des erreurs rencontrées.
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
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des paramètres.
	 */
	virtual void operationMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtCircleOperationPanel (const QtCircleOperationPanel&);
	QtCircleOperationPanel& operator = (const QtCircleOperationPanel&);

	/** Le nom du groupe. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création/modification du cercle. */
	QComboBox*						_operationMethodComboBox;

	/**
	 * Les panneaux de saisie des paramètres de définition du cercle.
	 */
	//{@
	/** Parent du panneau courant. */
	QWidget*						_currentParentWidget;

	/** Panneau courant. */
	QtMgx3DOperationsSubPanel*		_currentPanel;

	/** Renseignement des 3 vertices. */
	Qt3VerticiesPanel*				_verticesPanel;
	//@}	// Panneaux de saisie des paramètres de définition du cercle
};	// class QtCircleOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type <I>QtCircleOperationPanel</I> de création/modification d'un cercle/d'une ellipse.
 */
class QtCircleOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtCircleOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée notamment pour récupérer le contexte et le panneau contenant les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtCircleOperationAction (
		const QIcon& icon, const QString& text, Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow, const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCircleOperationAction ( );

	/**
	 * \return		Le panneau d'édition du cercle.
	 */
	virtual QtCircleOperationPanel* getCirclePanel ( );

	/**
	 * Créé/modifie le cercle avec le paramétrage de son panneau associé.
	 * Invoque préalablement <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCircleOperationAction (const QtCircleOperationAction&);
	QtCircleOperationAction& operator = (const QtCircleOperationAction&);
};  // class QtCircleOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_CIRCLE_OPERATION_ACTION_H
