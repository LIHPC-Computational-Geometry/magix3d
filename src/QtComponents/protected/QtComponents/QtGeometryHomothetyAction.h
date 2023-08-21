/**
 * \file		QtGeometryHomothetyAction.h
 * \author		Charles PIGNEROL
 * \date		26/05/2014
 */
#ifndef QT_GEOMETRY_HOMOTHETY_ACTION_H
#define QT_GEOMETRY_HOMOTHETY_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DHomothetyPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>
#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;

/**
 * Panneau d'édition des paramètres d'homothétie d'entités géométriques.
 */
class QtGeometryHomothetyPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeometryHomothetyPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeometryHomothetyPanel ( );

	/**
	 * \param		Nouvelle dimension de saisie des entités géométriques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		<I>true</I> s'il faut soumettre toutes les entités
	 * 				géométriques à l'homothétie, <I>false</I> s'il faut juste
	 * 				traiter la sélection désignée par l'utilisateur.
	 * \see			getInvolvedEntities
	 */
	virtual bool processAllGeomEntities ( ) const;

	/**
	 * \return		<I>true</I> s'il faut copier les entités avant la transformation
	 */
	virtual bool copyEntities ( ) const;

	/**
	 * @return		Le nom du groupe créé en cas de copie.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également copier les entités topologiques
	 */
	virtual bool copyTopoEntities ( ) const;

	/**
	 * \return		La liste des entités géométriques devant effectuer une
	 *				homothétie.
	 * \see			getHomothetyFactor
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

	/**
	 * Le type d'homothétie.
	 */
	enum HOMOTHETY_TYPE { HOMOGENEOUS, HETEROGENEOUS };

	/**
	 * \return		Le type d'homothétie.
	 */
	virtual HOMOTHETY_TYPE getHomothetyType ( ) const;

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
     * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \return		Le panneau de saisie des paramètres d'une homothétie
	 * 				homogène.
	 */
	virtual const QtMgx3DHomogeneousHomothetyPanel&
											getHomogeneousPanel ( ) const;

	/**
	 * \return		Le panneau de saisie des paramètres d'une homothétie
	 * 				hétérogène.
	 */
	virtual const QtMgx3DHeterogeneousHomothetyPanel&
											getHeterogeneousPanel ( ) const;


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 * \see			processAllGeomEntities
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );

	/**
	 * \return		Le panneau courant de paramétrage de l'homothétie.
	 */
	virtual QtMgx3DOperationsSubPanel& getParametersPanel ( ) const;


	protected slots :

	/**
	 * Appelé lorsque la case à cocher "toutes les entités" change d'état.
	 * (In)active la zone de saisie des entités soumises à la transformation.
	 */
	virtual void allEntitiesCallback ( );

	/**
	 * Appelé lorsque l'on active ou non la copie avant transformation.
	 * (In)active la zone de sélection du nom d'un groupe ainsi que
	 * l'option de copie de la topologie associée.
	 */
	virtual void copyCallback();

	/**
	 * Le changement de type d'homothétie. Actualisation du panneau.
	 */
	virtual void operationMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeometryHomothetyPanel (const QtGeometryHomothetyPanel&);
	QtGeometryHomothetyPanel& operator = (
										const QtGeometryHomothetyPanel&);

	/** Les dimensions possibles pour les entités géométriques suivant
	 * l'homothétie. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;

	/** Case à cocher <I>Toutes les entités</I>. */
	QCheckBox*									_allEntitiesCheckBox;

	/** Le type d'homothétie. */
	QComboBox*									_operationMethodComboBox;

	/** Le paramétrage de l'homothétie. */
	QtMgx3DOperationsSubPanel*					_currentPanel;

	/** Paramétrage d'une homothétie homogène. */
	QtMgx3DHomogeneousHomothetyPanel*			_homogeneousPanel;

	/** Paramétrage d'une homothétie hétérogène. */
	QtMgx3DHeterogeneousHomothetyPanel*			_heterogeneousPanel;

	/** Case à cocher copier. */
	QCheckBox*									_copyCheckBox;

	/** Le nom du groupe créé (en cas de copie). */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** Case à cocher copier avec la topologie</I>. */
	QCheckBox*									_withTopologyCheckBox;

};	// class QtGeometryHomothetyPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeometryHomothetyPanel</I> d'homothétie d'entités géométriques.
 */
class QtGeometryHomothetyAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeometryHomothetyPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeometryHomothetyAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryHomothetyAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'homothétie d'entités
	 *				géométriques.
	 */
	virtual QtGeometryHomothetyPanel* getHomothetyPanel ( );

	/**
	 * Actualise le paramétrage des entités géométriques avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeometryHomothetyAction (const QtGeometryHomothetyAction&);
	QtGeometryHomothetyAction& operator = (
									const QtGeometryHomothetyAction&);
};  // class QtGeometryHomothetyAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOMETRY_HOMOTHETY_ACTION_H
