/**
 * \file		QtBlockMeshingPropertyAction.h
 * \author		Charles PIGNEROL
 * \date		14/11/2013
 */
#ifndef QT_BLOCK_MESHING_PROPERTY_ACTION_H
#define QT_BLOCK_MESHING_PROPERTY_ACTION_H

#include "Internal/Context.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/RenderingManager.h"
#include "Topo/BlockMeshingPropertyTransfinite.h"
#include "Topo/BlockMeshingPropertyDelaunayTetgen.h"

#include <QtUtil/QtIntTextField.h>

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Le paramétrage d'un maillage transfini.
 */
class QtBlockTransfinitePanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtBlockTransfinitePanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockTransfinitePanel ( );

	/**
	 * \return		La propriété de maillage de du bloc conforme au panneau.
	 */
	virtual Mgx3D::Topo::BlockMeshingPropertyTransfinite*
											getMeshingProperty ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockTransfinitePanel (const QtBlockTransfinitePanel&);
	QtBlockTransfinitePanel& operator = (const QtBlockTransfinitePanel&);
};	// class QtBlockTransfinitePanel


/**
 * Le paramétrage d'un maillage Delaunay via <I>Tetgen</I>.
 */
class QtBlockDelaunayTetgenPanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtBlockDelaunayTetgenPanel (QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw, QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockDelaunayTetgenPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );
	
	/**
	 * \return		La propriété de maillage de du bloc conforme au panneau.
	 */
	virtual Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen* getMeshingProperty ( ) const;

	/**
	 * \return		La taille maximale des mailles pour <I>Tetgen</I>.
	 * \see			getQualityFactor
	 */
	virtual double getVolMax ( ) const;

	/**
	 * \return		Le <I>facteur de qualité</I> pour <I>Tetgen</I>.
	 * \see			getVolMax
	 */
	virtual double getQualityFactor ( ) const;

	/**
	 * \return		La ratio pour la taille des pyramides pour <I>Tetgen</I>.
	 */
	virtual double getRatioPyramidSize ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockDelaunayTetgenPanel (const QtBlockDelaunayTetgenPanel&);
	QtBlockDelaunayTetgenPanel& operator = (const QtBlockDelaunayTetgenPanel&);

	/** Les valeurs min et max pour <I>Tetgen</I>. */
	QtDoubleTextField			*_volMaxTextField, *_qualityFactorTextField, *_ratioPyramiSizeTextField;
};	// class QtBlockDelaunayTetgenPanel


/**
 * Panneau d'édition des paramètres de maillage de blocs topologiques.
 */
class QtBlockMeshingPropertyPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de maillage de du bloc.
	 * <OL>
	 * <LI>Maillage structuré transfini,
	 * <LI>Maillage non structuré Delaunay généré par <I>Tetgen</I>
	 * </OL>
	 */
	enum OPERATION_METHOD { TRANSFINITE, DELAUNAY_TETGEN };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtBlockMeshingPropertyPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtBlockMeshingPropertyPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		La méthode de maillage du bloc.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * \return		La propriété de maillage du bloc.
	 * \warning		C'est à l'appelant de détruire l'instance retournée.
	 */
	virtual Mgx3D::Topo::BlockMeshingProperty* getMeshingProperty ( ) const;

	/**
	 * \return		La liste des blocs sélectionnés.
	 * \see			setBlocksNames
	 */
	virtual std::vector<std::string> getBlocksNames ( ) const;

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
     * Invoque <I>preview (false)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour prévisualiser le paramétrage surfacique du
	 *				maillage, <I>false</I> pour arrêter la prévisualisation.
	 * \param		inutilisé
	 */
	virtual void preview (bool show, bool destroyInteractor);


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
 	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
 	 * cf. spécifications dans la classe de base.
 	 */
	virtual void operationCompleted ( );

	/**
	 * \param		La liste des blocs sélectionnés.
	 * \see			getBlocksNames
	 */
	virtual void setBlocksNames (const std::vector<std::string>& names);


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
	QtBlockMeshingPropertyPanel (const QtBlockMeshingPropertyPanel&);
	QtBlockMeshingPropertyPanel& operator = (const QtBlockMeshingPropertyPanel&);

	/** La méthode de maillage du bloc. */
	QComboBox*									_operationMethodComboBox;

	/**
	 * Les panneaux de définition de la discrétisation des blocs.
	 */
	//{@

	/** Parent du panneau courant. */
	QWidget*									_currentParentWidget;

	/** Panneau courant. */
	QtMgx3DOperationsSubPanel*					_currentPanel;

	/** Maillage transfini. */
	QtBlockTransfinitePanel*					_transfinitePanel;

	/** Maillage non structuré Delaunay généré avec <I>Tetgen</I>. */
	QtBlockDelaunayTetgenPanel*					_delaunayTetgenPanel;

	//@}	// Panneaux de saisie des paramètres de définition du vertex

	/** Les blocs à discrétiser. */
	QtMgx3DEntityPanel*							_blocksPanel;
};	// class QtBlockMeshingPropertyPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtBlockMeshingPropertyPanel</I> de modification des paramètres de
 * maillage d'un bloc.
 */
class QtBlockMeshingPropertyAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtBlockMeshingPropertyPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtBlockMeshingPropertyAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockMeshingPropertyAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de maillage du bloc
	 *				associé.
	 */
	virtual QtBlockMeshingPropertyPanel* getMeshingPropertyPanel ( );

	/**
	 * Actualise le paramétrage des blocs avec le paramétrage de son
	 * panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockMeshingPropertyAction (const QtBlockMeshingPropertyAction&);
	QtBlockMeshingPropertyAction& operator = (
									const QtBlockMeshingPropertyAction&);
};  // class QtBlockMeshingPropertyAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_BLOCK_MESHING_PROPERTY_ACTION_H
