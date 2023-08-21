/**
 * \file		QtArcCircleOperationAction.h
 * \author		Charles PIGNEROL
 * \date		08/09/2014
 */
#ifndef QT_ARC_CIRCLE_OPERATION_ACTION_H
#define QT_ARC_CIRCLE_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"
#include "QtComponents/QtCurvilinearAbscissaPanel.h"
#include "QtComponents/Qt3VerticiesPanel.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <SysCoord/SysCoord.h>

#include <QCheckBox>
#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Le paramétrage d'un arc de cercle à partir du centre et des extrémités.
 */
class QtCenterExtremitiesArcPanel : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtCenterExtremitiesArcPanel (QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw, QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCenterExtremitiesArcPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Restitue l'environnement dans son état initial.
	 */
	virtual void cancel ( );

	/**
	 * Les coordonnées des vertices définissant l'arc de cercle.
	 */
	virtual std::string getStartVertexUniqueName ( ) const;
	virtual std::string getEndVertexUniqueName ( ) const;
	virtual std::string getCenterVertexUniqueName ( ) const;

	/**
	 * \return	<I>true</I> si l'utilisateur défini la normale au plan, <I>false</I> dans le cas contraire.
	 * \see		getNormal
	 */
	virtual bool defineNormal ( ) const;

	/**
	 * \return	La normale au plan contenant l'arc de cercle.
	 * \see		defineNormal
	 */
	virtual Mgx3D::Utils::Math::Vector getNormal ( ) const;

	/**
	 * \return	<I>true</I> si le sens de rotation est direct, <I>false</I> dans le cas contraire.
	 */
	virtual bool directOrientation ( ) const;

	signals :

	/**
	 * Signal émis lorsque un paramètre du panneau est modifié.
	 */
	void parametersModified ( );


	protected slots :

	/**
	 * Emet le signal parametersModified pour prise en charge par le panneau maître.
	 */
	 virtual void parametersModifiedCallback ( );

	/**
	 * Appelé lorsque le caractère "définir la normale au plan" est modifié. Actualise l'IHM.
	 */
	virtual void useNormalModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCenterExtremitiesArcPanel (const QtCenterExtremitiesArcPanel&);
	QtCenterExtremitiesArcPanel& operator = (const QtCenterExtremitiesArcPanel&);

	/** Les 3 vertices. */
	Qt3VerticiesPanel*              _verticesPanel;

	/** Faut-il définir la normale au plan contenant les points ? */
	QCheckBox*						_normalCheckBox;

	/** Vecteur normal au plan pour les cas ambigues (3 points alignés). */
	QtMgx3DVectorPanel*				_normalPanel;

	/** Le sens de rotation. */
	QCheckBox*						_directionCheckBox;

};	// class QtCenterExtremitiesArcPanel


/**
 * Le paramétrage d'un arc de cercle à partir de 2 angles, un rayon, un repère.
 */
class QtAnglesSysCoordsArcPanel : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtAnglesSysCoordsArcPanel (QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw, QtMgx3DOperationPanel* mainPanel = 0);

	/**
		 * Destructeur. RAS.
		 */
	virtual ~QtAnglesSysCoordsArcPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Restitue l'environnement dans son état initial.
	 */
	virtual void cancel ( );

	/**
	 * Les angles, rayon, repère définissant l'arc de cercle.
	 */
	virtual double getStartAngle ( ) const;
	virtual double getEndAngle ( ) const;
	virtual double getRayLength ( ) const;
	virtual Mgx3D::CoordinateSystem::SysCoord* getSysCoord ( ) const;
	virtual std::string getSysCoordName ( ) const;

	signals :

	/**
	 * Signal émis lorsque un paramètre du panneau est modifié.
	 */
	void parametersModified ( );


	protected slots :

	/**
	 * Emet le signal parametersModified pour prise en charge par le panneau maître.
	 */
	virtual void parametersModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtAnglesSysCoordsArcPanel (const QtAnglesSysCoordsArcPanel&);
	QtAnglesSysCoordsArcPanel& operator = (const QtAnglesSysCoordsArcPanel&);

	/** Les valeurs des angles. */
	QtDoubleTextField		    *_startTextField, *_endTextField;

	/** Le rayon */
	QtDoubleTextField			*_rayLengthTextField;

	/** Le repère. */
	QtMgx3DEntityPanel          *_sysCoordPanel;
};	// class QtAnglesSysCoordsArcPanel
	
	
/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * un arc de cercle.
 */
class QtArcCircleOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification de l'arc de cercle.
	 * <OL>
	 * <LI>Renseignement de 3 vertex (extrémités + centre de l'arc).
	 * <LI>Renseignement de 2 angles, d'un rayon et d'un repère.
	 * </OL>
	 */
	enum OPERATION_METHOD { EXTREMITIES_CENTER, SYSCOORD_2_ANGLES };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtArcCircleOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtArcCircleOperationPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création/modification de l'arc de cercle.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées des vertices définissant l'arc de cercle.
	 * (si <I>getOperationMethod ( )</I> retourne <I>EXTREMITIES_CENTER</I>).
	 */
	virtual std::string getStartVertexUniqueName ( ) const;
	virtual std::string getEndVertexUniqueName ( ) const;
	virtual std::string getCenterVertexUniqueName ( ) const;

	/**
	 * \return	<I>true</I> si l'utilisateur défini la normale au plan, <I>false</I> dans le cas contraire.
	 * (si <I>getOperationMethod ( )</I> retourne <I>EXTREMITIES_CENTER</I>).
	 * \see		getNormal
	 */
	virtual bool defineNormal ( ) const;

	/**
	 * \return	La normale au plan contenant l'arc de cercle.
	 * (si <I>getOperationMethod ( )</I> retourne <I>EXTREMITIES_CENTER</I>).
	 * \see		defineNormal
	 */
	virtual Mgx3D::Utils::Math::Vector getNormal ( ) const;

	/**
	 * \return	<I>true</I> si le sens de rotation est direct, <I>false</I> dans le cas contraire.
	 * (si <I>getOperationMethod ( )</I> retourne <I>EXTREMITIES_CENTER</I>).
	 */
	virtual bool directOrientation ( ) const;

	/**
	 * Les angles, rayon, repère définissant l'arc de cercle.
	 * (si <I>getOperationMethod ( )</I> retourne <I>SYSCOORD_2_ANGLES</I>).
	 */
	 virtual double getStartAngle ( ) const;
	 virtual double getEndAngle ( ) const;
	 virtual double getRayLength ( ) const;
	 virtual Mgx3D::CoordinateSystem::SysCoord* getSysCoord ( ) const;
	 virtual std::string getSysCoordName ( ) const;

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

	/**
	 * \param	<I>true</I> pour prévisualiser l'opération, <I>false</I>
	 *			pour arrêter la prévisualisation.
	 * \param	non utilisé
	 */
	virtual void preview (bool on, bool destroyInteractor);


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
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des
	 * paramètres.
	 */
	virtual void operationMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtArcCircleOperationPanel (const QtArcCircleOperationPanel&);
	QtArcCircleOperationPanel& operator = (const QtArcCircleOperationPanel&);

	/** Le nom du groupe. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création/modification de l'arc de cercle. */
	QComboBox*						_operationMethodComboBox;

	/**
	 * Les panneaux de saisie des paramètres de définition de l'arc de cercle.
	 */
	//{@
	/** Parent du panneau courant. */
	QWidget*						_currentParentWidget;

	/** Panneau courant. */
	QtMgx3DOperationsSubPanel*		_currentPanel;

	/** Renseignement des 3 vertices. */
	QtCenterExtremitiesArcPanel*	_centerExtremitiesPanel;

	/** Renseignement de 2 angles / 1 rayon / 1 repère */
	QtAnglesSysCoordsArcPanel*      _anglesSysCoordsPanel;
	//@}	// Panneaux de saisie des paramètres de définition de l'arc de cercle
};	// class QtArcCircleOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtArcCircleOperationPanel</I> de création/modification d'un arc de cercle.
 */
class QtArcCircleOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtArcCircleOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtArcCircleOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtArcCircleOperationAction ( );

	/**
	 * \return		Le panneau d'édition de l'arc de cercle.
	 */
	virtual QtArcCircleOperationPanel* getArcCirclePanel ( );

	/**
	 * Créé/modifie l'arc de cercle avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtArcCircleOperationAction (const QtArcCircleOperationAction&);
	QtArcCircleOperationAction& operator = (const QtArcCircleOperationAction&);
};  // class QtArcCircleOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_ARC_CIRCLE_OPERATION_ACTION_H
