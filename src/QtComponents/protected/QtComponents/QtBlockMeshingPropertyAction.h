/**
 * \file		QtBlockMeshingPropertyAction.h
 * \author		Charles PIGNEROL
 * \date		14/11/2013
 */
#ifndef QT_BLOCK_MESHING_PROPERTY_ACTION_H
#define QT_BLOCK_MESHING_PROPERTY_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/RenderingManager.h"
#include "Topo/BlockMeshingPropertyTransfinite.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Topo/BlockMeshingPropertyOrthogonal.h"
#include "Topo/BlockMeshingPropertyRotational.h"
#include "Topo/BlockMeshingPropertyDelaunayTetgen.h"
#include "Topo/BlockMeshingPropertyInsertion.h"

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
 * Le paramétrage d'un maillage directionnel.
 */
class QtBlockDirectionalPanel : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtBlockDirectionalPanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockDirectionalPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * cf. spécifications de la classe de base.
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * cf. spécifications de la classe de base.
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour prévisualiser l'opération, <I>false</I>
	 * 				pour arrêter la prévisualisation.
	 * \param		non utilisé
	 */
	virtual void preview (bool on, bool destroyInteractor);

	/**
	 * Affiche/masque la fenêtre, et les éventuelles informations de
	 * prévisualisation.
	 */
	virtual void setVisible (bool visible);

	/**
	 * \return		Le point de départ définissant le vecteur directionnel.
	 * \see			getPoint2
	 */
	virtual Mgx3D::Utils::Math::Point getPoint1 ( ) const;

	/**
	 * \return		Le point d'arrivée définissant le vecteur directionnel.
	 * \see			getPoint1
	 */
	virtual Mgx3D::Utils::Math::Point getPoint2 ( ) const;

	/**
	 * \return		La propriété de maillage de du bloc conforme au panneau.
	 */
	virtual Mgx3D::Topo::BlockMeshingPropertyDirectional*
											getMeshingProperty ( ) const; 

	/**
	 * Faut-il inverser le vecteur donné par l'arête ?
	 */
	virtual bool isEdgeInverted ( ) const;

	/**
	 * \return	<I>true</I> s'il faut utiliser le nom de l'arête, <I>false</I>
	 * 			s'il faut utiliser le vecteur associé.
	 * \see		getEdgeName
	 */
	 virtual bool useEdgeName ( ) const;

	/**
	 * \return	Le nom de l'arête directionnelle
	 */
	virtual std::string getEdgeName ( ) const;

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * \return		Le champ de saisie de l'arête directionnelle.
	 */
	virtual QtEntityIDTextField* getEdgeTextField ( );


	protected slots :

	/**
	 * Appelé lorsqu'un paramètre à changé : actualise les informations
	 * affichées.
	 */
	virtual void directionModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockDirectionalPanel (const QtBlockDirectionalPanel&);
	QtBlockDirectionalPanel& operator = (const QtBlockDirectionalPanel&);

	/** La saisie du vecteur directeur. */
	QtMgx3DEdgePanel*						_edgeTextField;

	/** Le vecteur directionnel est-il à inverser ? */
	QCheckBox*								_invertCheckBox;

	/** Le vecteur donnant la direction. */
	QLabel*									_directionLabel;

	/** Utiliser le nom de l'arête ou le vecteur associé ? */
	QCheckBox*								_useEdgeNameCheckBox;
};	// class QtBlockDirectionalPanel


/**
 * Le paramétrage d'un maillage <I>Orthogonal</I>.
 */
class QtBlockOrthogonalPanel : public QtBlockDirectionalPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtBlockOrthogonalPanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockOrthogonalPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		La propriété de maillage du bloc conforme au panneau.
	 */
	virtual Mgx3D::Topo::BlockMeshingPropertyOrthogonal*
											getMeshingProperty ( ) const;

	/**
	 * \return		Le nombre de couches.
	 */
	virtual size_t getLayersNum ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockOrthogonalPanel (const QtBlockOrthogonalPanel&);
	QtBlockOrthogonalPanel& operator = (const QtBlockOrthogonalPanel&);

	/** Le nombre de couches. */
	QtIntTextField*		_layersNumTextField;
};	// class QtBlockOrthogonalPanel


/**
 * Le paramétrage d'un maillage rotationel. La rotation est définie par
 * l'axe de rotation et la direction selon laquelle s'effectue le maillage.
 */
class QtBlockRotationalPanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtBlockRotationalPanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockRotationalPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * cf. spécifications de la classe de base.
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * cf. spécifications de la classe de base.
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour prévisualiser l'opération, <I>false</I>
	 * 				pour arrêter la prévisualisation.
	 * \param		non utilisé
	 */
	virtual void preview (bool on, bool destroyInteractor);

	/**
	 * Affiche/masque la fenêtre, et les éventuelles informations de
	 * prévisualisation.
	 */
	virtual void setVisible (bool visible);

	/**
	 * \return		Le point de départ définissant la direction de la rotation.
	 * \see			getDirPoint2
	 * \see			getAxePoint1
	 */
	virtual Mgx3D::Utils::Math::Point getDirPoint1 ( ) const;

	/**
	 * \return		Le point d'arrivée définissant la direction de la rotation.
	 * \see			getDirPoint1
	 */
	virtual Mgx3D::Utils::Math::Point getDirPoint2 ( ) const;

	/**
	 * \return		Le point de départ définissant l'axe de rotation.
	 * \see			getAxePoint2
	 * \see			getDirPoint1
	 */
	virtual Mgx3D::Utils::Math::Point getAxePoint1 ( ) const;

	/**
	 * \return		Le point d'arrivée définissant l'axe de rotation.
	 * \see			getAxePoint1
	 */
	virtual Mgx3D::Utils::Math::Point getAxePoint2 ( ) const;

	/**
	 * \return		La propriété de maillage de du bloc conforme au panneau.
	 */
	virtual Mgx3D::Topo::BlockMeshingPropertyRotational* getMeshingProperty ( ) const;

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * \return		Le champ de saisie de l'arête directionnelle.
	 */
	virtual QtEntityIDTextField* getEdgeTextField ( );

	/**
	 * \return		Le champ de saisie du premier point de l'axe de rotation.
	 */
	virtual QtEntityIDTextField* getAxePoint1TextField ( );

	/**
	 * \return		Le champ de saisie du second point de l'axe de rotation.
	 */
	virtual QtEntityIDTextField* getAxePoint2TextField ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockRotationalPanel (const QtBlockRotationalPanel&);
	QtBlockRotationalPanel& operator = (const QtBlockRotationalPanel&);

	/** La saisie de l'arête donnant la direction du maillage. */
	QtMgx3DEdgePanel*						_directionTextField;

	/** La saisie de 2 points permettant de définir l'axe de rotation. */
	QtMgx3DPointPanel						*_axePoint1Panel, *_axePoint2Panel;

	/** Le vecteur donnant la direction. */
	QLabel*									_directionLabel;

	/** Le vecteur donnant l'axe de rotation. */
	QLabel*									_axeLabel;
};	// class QtBlockRotationalPanel


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
 * Le paramétrage d'un maillage par insertion de maillage.
 */
class QtBlockMeshInsertionPanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtBlockMeshInsertionPanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockMeshInsertionPanel ( );

	/**
	 * \return		La propriété de maillage de du bloc conforme au panneau.
	 */
	virtual Mgx3D::Topo::BlockMeshingPropertyInsertion*
											getMeshingProperty ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockMeshInsertionPanel (const QtBlockMeshInsertionPanel&);
	QtBlockMeshInsertionPanel& operator = (const QtBlockMeshInsertionPanel&);
};	// class QtBlockMeshInsertionPanel


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
	 * <LI>Maillage structuré directionnel
	 * <LI>Maillage structuré directionnel orthogonal
	 * <LI>Maillage structuré rotationnel
	 * <LI>Maillage non structuré Delaunay généré par <I>Tetgen</I>
	 * </OL>
	 */
	enum OPERATION_METHOD { TRANSFINITE, DIRECTIONAL, ORTHOGONAL, ROTATIONAL, DELAUNAY_TETGEN, MESH_INSERTION };

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

	/** Maillage structuré directionnel. */
	QtBlockDirectionalPanel*					_directionalPanel;

	/** Maillage structuré directionnel orthogonal. */
	QtBlockOrthogonalPanel*						_orthogonalPanel;

	/** Maillage structuré rotationnel. */
	QtBlockRotationalPanel*						_rotationalPanel;

	/** Maillage non structuré Delaunay généré avec <I>Tetgen</I>. */
	QtBlockDelaunayTetgenPanel*					_delaunayTetgenPanel;

	/** Maillage non structuré obtenu par insertion de maillage. */
	QtBlockMeshInsertionPanel*					_meshInsertionPanel;

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
