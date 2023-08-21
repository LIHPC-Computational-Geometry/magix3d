/**
 * \file		QtEdgeMeshingPropertyAction.h
 * \author		Charles PIGNEROL
 * \date		04/11/2013
 */
#ifndef QT_EDGE_MESHING_PROPERTY_ACTION_H
#define QT_EDGE_MESHING_PROPERTY_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/RenderingManager.h"
#include "QtComponents/QtMgx3DPointPanel.h"

#include "Topo/EdgeMeshingPropertyBeta.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Topo/EdgeMeshingPropertySpecificSize.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
#include "Topo/EdgeMeshingPropertyBigeometric.h"
#include "Topo/EdgeMeshingPropertyHyperbolic.h"
#include "Topo/EdgeMeshingPropertyUniform.h"

#include <QtUtil/QtDoubleTextField.h>
#include <QtUtil/QtIntTextField.h>

#include <QCheckBox>
#include <QComboBox>

#include <vector>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau de base de définition des paramètres de discrétisation des
 * arêtes. Offre une interface de réinitialisation du panneau.
 */
class QtDiscretisationPanelIfc : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtDiscretisationPanelIfc ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * @return		<I>true</I> si la discrétisation est orthogonale à la
	 * 				paroi, <I>false</I> dans le cas contraire.
	 */
	virtual bool isOrthogonalToSide ( ) const;

	/**
	 * @return		Le nombre de couches si la discrétisation est orthogonale à
	 * 				la paroi.
	 * @see			isOrthogonalToSide
	 */
	virtual size_t getNbLayers ( ) const;

	/**
	 * @return		Le sens de la discrétisation (cas d'une discrétisation
	 * 				orthogonale).
	 * @see			isOrthogonalToSide
	 */
	virtual bool getDirection ( ) const;

	/**
	 * @return		<I>true</I> si la discrétisation est en mode polaire,
	 * 				<I>false</I> dans le cas contraire.
	 */
	virtual bool isPolarCut ( ) const;

	/**
	 * @return		Le point au centre de la discrétisation en mode polaire
	 * @see			isPolarCut
	 */
	virtual Utils::Math::Point getPolarCenter ( ) const;

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 * A surcharger.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * Actualise la propriété transmise en argument selon les paramètres
	 * définis par l'utilisateur dans ce panneau.
	 * A surcharger.
	 */
	virtual void updateProperty (Mgx3D::Topo::CoEdgeMeshingProperty&) const;


	protected :

	/**
	 * \param		Widget parent
	 * \param		Nombre d'arêtes proposé par défaut
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtDiscretisationPanelIfc (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw);


	protected :

	/**
	 * Créé une zone de saisie "discrétisation orthogonale à la paroi" et
	 * l'ajoute au gestionnaire de mise en forme transmis en argument via
	 * <I>addWidget</I>.
	 */
	virtual void createOrthogonalityArea (QLayout& layout);

	/**
	 * Créé une zone de saisie "discrétisation polaire" et
	 * l'ajoute au gestionnaire de mise en forme transmis en argument via
	 * <I>addWidget</I>.
	 */
	virtual void createPolarArea (QLayout& layout);


	protected slots :

	/**
	 * Invoqué lorsque l'utilisateur modifie le paramètre de discrétisation
	 * orthogonale à la paroi. Actualise l'IHM et invoque
	 * discretisationModifiedCallback.
	 * \see		discretisationModifiedCallback
	 */
	virtual void orthogonalityModifiedCallback ( );

	/**
	 * Invoqué lorsque l'utilisateur modifie le paramètre de discrétisation
	 * polaire. Actualise l'IHM et invoque
	 * discretisationModifiedCallback.
	 * \see		discretisationModifiedCallback
	 */
	virtual void polarModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtDiscretisationPanelIfc (const QtDiscretisationPanelIfc&);
	QtDiscretisationPanelIfc& operator = (const QtDiscretisationPanelIfc&);

	/** L'option discrétisation orthogonale à la paroi. */
	QWidget*					_orthogonalOptionsPanel;
	QCheckBox					*_orthogonalCheckBox, *_directionCheckBox;
	QtIntTextField*				_layersNumTextField;

	/** L'option discrétisation polaire. */
	QWidget*					_polarOptionsPanel;
	QCheckBox*					_polarCheckBox;
	QtMgx3DPointPanel*			_polarCenterPanel;


	protected:



};	// class QtDiscretisationPanelIfc


/**
 * Le paramétrage d'un découpage uniforme.
 */
class QtUniformDiscretisationPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtUniformDiscretisationPanel (
							QWidget* parent,
							Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtUniformDiscretisationPanel ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyUniform*
											getMeshingProperty ( size_t edgeNum ) const;

	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtUniformDiscretisationPanel (const QtUniformDiscretisationPanel&);
	QtUniformDiscretisationPanel& operator = (
										const QtUniformDiscretisationPanel&);

};	// class QtUniformDiscretisationPanel


/**
 * Le paramétrage d'un découpage avec progression géométrique.
 */
class QtGeometricProgressionPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Raison de la progression géométrique proposée par défaut
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtGeometricProgressionPanel (
							QWidget* parent,
							double factor,
							Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometricProgressionPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		La raison de la progression
	 */
	virtual double getFactor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut inverser l'orientation du découpage,
	 * 				<I>false</I> dans le sens contraire.
	 */
	virtual bool invertOrientation ( ) const;

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyGeometric*
											getMeshingProperty ( size_t edgeNum ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeometricProgressionPanel (const QtGeometricProgressionPanel&);
	QtGeometricProgressionPanel& operator = (
										const QtGeometricProgressionPanel&);

	/** La saisie de la raison de la progression géométrique. */
	QtDoubleTextField*			_factorTextField;

	/** Faut il inverser le sens de discrétisation ? */
	QCheckBox*					_orientationCheckBox;
};	// class QtGeometricProgressionPanel


/**
 * Le paramétrage d'un découpage en spécifiant la taille des segments.
 */
class QtSpecificSizePanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Taille des arêtes proposée par défaut
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtSpecificSizePanel (QWidget* parent, double size,
						Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSpecificSizePanel ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		La taille des segments
	 */
	virtual double getSize ( ) const;

	/**
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertySpecificSize*
											getMeshingProperty ( ) const; 


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSpecificSizePanel (const QtSpecificSizePanel&);
	QtSpecificSizePanel& operator = (const QtSpecificSizePanel&);

	/** La saisie du nombre de segments. */
	QtDoubleTextField*			_sizeTextField;
};	// class QtSpecificSizePanel


/**
 * Le paramétrage d'un découpage reposant sur les sommets d'un ensemble
 * d'arêtes.
 */
class QtInterpolatedEdgeDiscretisationPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Arête de référence proposée par défaut
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtInterpolatedEdgeDiscretisationPanel (
				QWidget* parent, const std::string& edgeName,
				Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtInterpolatedEdgeDiscretisationPanel ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		Les noms des entités de référence.
	 */
	virtual std::vector<std::string> getRefEntitiesNames ( ) const;
	virtual QtEntityIDTextField* getRefEntitiesTextField ( )
	{ return _refEntitiesTextField; }

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyInterpolate*
									getMeshingProperty ( size_t edgeNum ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Quitte le mode de sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Efface les entités sélectionnées.
	 */
	virtual void cancel ( );

	/**
	 * \return		La liste des entités du panneau impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getPanelEntities ( );


	protected slots :

	/**
	 * Changement de type d'entités de référence => RAZ de la liste.
	 */
	virtual void refEntitiesCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtInterpolatedEdgeDiscretisationPanel (
								const QtInterpolatedEdgeDiscretisationPanel&);
	QtInterpolatedEdgeDiscretisationPanel& operator = (
								const QtInterpolatedEdgeDiscretisationPanel&);

	/** Que prend t'on en référence ? */
	QComboBox*					_referenceComboBox;

	/** Les entités de référence. */
	QtEntityIDTextField*		_refEntitiesTextField;

};	// class QtInterpolatedEdgeDiscretisationPanel


/**
 * Le paramétrage d'un découpage reposant sur les sommets entre deux ensembles
 * d'arêtes.
 */
class QtGlobalInterpolatedEdgeDiscretisationPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Arête de référence proposée par défaut
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtGlobalInterpolatedEdgeDiscretisationPanel (
				QWidget* parent, const std::string& edgeName,
				Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGlobalInterpolatedEdgeDiscretisationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		Les noms de la 1ère liste d'arêtes de référence.
	 */
	virtual std::vector<std::string> getFirstEdgeNames ( ) const;
	virtual QtEntityIDTextField* getFirstEdgeTextField ( )
	{ return _firstEdgesPanel; }

	/**
	 * \return		Les noms de la 2nde liste d'arêtes de référence.
	 */
	virtual std::vector<std::string> getSecondEdgeNames ( ) const;
	virtual QtEntityIDTextField* getSecondEdgeTextField ( )
	{ return _secondEdgesPanel; }

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyGlobalInterpolate*
	     getMeshingProperty ( size_t edgeNum ) const;

	/**
	 * Quitte le mode de sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Efface les entités sélectionnées.
	 */
	virtual void cancel ( );

	/**
	 * \return		La liste des entités du panneau impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getPanelEntities ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGlobalInterpolatedEdgeDiscretisationPanel (
								const QtGlobalInterpolatedEdgeDiscretisationPanel&);
	QtGlobalInterpolatedEdgeDiscretisationPanel& operator = (
								const QtGlobalInterpolatedEdgeDiscretisationPanel&);

	/** La 1ère liste d'arêtes de référence. */
	QtEntityIDTextField*		_firstEdgesPanel;

	/** La 2nde liste d'arêtes de référence. */
	QtEntityIDTextField*		_secondEdgesPanel;

};	// class QtGlobalInterpolatedEdgeDiscretisationPanel


/**
 * Le paramétrage d'un découpage avec progression bi-géométrique.
 */
class QtBiGeometricProgressionPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Raison et longueur du premier bras par le côté 1 proposés
	 * 				par défaut
	 * \param		Raison et longueur du premier bras par le côté 2 proposés
	 * 				par défaut
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtBiGeometricProgressionPanel (
							QWidget* parent,
							double ratio1, double length1,
							double ratio2, double length2,
							Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBiGeometricProgressionPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		La raison de la progression côté 1.
	 */
	virtual double getRatio1 ( ) const;

	/**
	 * @return		La longueur du premier segment côté 1.
	 */
	virtual double getLength1 ( ) const;

	/**
	 * \return		La raison de la progression côté 2.
	 */
	virtual double getRatio2 ( ) const;

	/**
	 * @return		La longueur du premier segment côté 2.
	 */
	virtual double getLength2 ( ) const;

	/**
	 * \return		<I>true</I> s'il faut inverser l'orientation du découpage,
	 * 				<I>false</I> dans le sens contraire.
	 */
	virtual bool invertOrientation ( ) const;

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyBigeometric*
											getMeshingProperty ( size_t edgeNum ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBiGeometricProgressionPanel (const QtBiGeometricProgressionPanel&);
	QtBiGeometricProgressionPanel& operator = (
										const QtBiGeometricProgressionPanel&);

	/** La saisie des raisons et longueurs des 2 progressions géométriques. */
	QtDoubleTextField			*_ratio1TextField, *_length1TextField,
								*_ratio2TextField, *_length2TextField;

	/** Faut il inverser le sens de discrétisation ? */
	QCheckBox*					_orientationCheckBox;
};	// class QtBiGeometricProgressionPanel


/**
 * Le paramétrage d'un découpage hyperbolique.
 */
class QtHyperbolicDiscretisationPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Longueur des premiers bras aux extrémités.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtHyperbolicDiscretisationPanel (QWidget* parent,
					   double length1, double length2,
					   Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtHyperbolicDiscretisationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		La longueur du bras à l'extrémité du côté 1.
	 */
	virtual double getLength1 ( ) const;

	/**
	 * \return		La longueur du bras à l'extrémité du côté 2.
	 */
	virtual double getLength2 ( ) const;

	/**
	 * \return		<I>true</I> s'il faut inverser l'orientation du découpage,
	 * 				<I>false</I> dans le sens contraire.
	 */
	virtual bool invertOrientation ( ) const;

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyHyperbolic*
											getMeshingProperty ( size_t edgeNum ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtHyperbolicDiscretisationPanel (const QtHyperbolicDiscretisationPanel&);
	QtHyperbolicDiscretisationPanel& operator = (
										const QtHyperbolicDiscretisationPanel&);

	/** La saisie des longueurs des 2 bras aux extrémités. */
	QtDoubleTextField			*_length1TextField, *_length2TextField;

	/** Faut il inverser le sens de discrétisation ? */
	QCheckBox*					_orientationCheckBox;
};	// class QtHyperbolicDiscretisationPanel


/**
 * Le paramétrage d'un découpage avec loi beta de resserrement sur paroi.
 */
class QtBetaDiscretisationPanel : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Facteur beta de resserrement (1+epsilon)
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 */
	QtBetaDiscretisationPanel (
							QWidget* parent, double beta,
							Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBetaDiscretisationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * \return		Le facteur de resserrement
	 */
	virtual double getBeta ( ) const;

	/**
	 * \return		<I>true</I> s'il faut inverser l'orientation du découpage,
	 * 				<I>false</I> dans le sens contraire.
	 */
	virtual bool invertOrientation ( ) const;

	/**
	 * \param       edgeNum le nombre de bras
	 * \return		La propriété de maillage de l'arête conforme au panneau.
	 */
	virtual Mgx3D::Topo::EdgeMeshingPropertyBeta*
											getMeshingProperty ( size_t edgeNum ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBetaDiscretisationPanel (const QtBetaDiscretisationPanel&);
	QtBetaDiscretisationPanel& operator = (
										const QtBetaDiscretisationPanel&);

	/** La saisie du facteur beta de resserrement. */
	QtDoubleTextField*			_betaTextField;

	/** Faut il inverser le sens de discrétisation ? */
	QCheckBox*					_orientationCheckBox;
};	// class QtBetaDiscretisationPanel


/**
 * Panneau générique d'édition des paramètres de maillage d'une liste d'arêtes
 * topologiques.
 */
class QtEdgeMeshingPropertyPanelIfc : public QtDiscretisationPanelIfc
{
	Q_OBJECT

	public :

	/**
	 * \return		La propriété de maillage des arêtes.
	 * \warning		C'est à l'appelant de détruire l'instance retournée.
	 * \warning		<B>Méthode à surcharger, lève par défaut une exception.</B>
	 * \see			setMeshingProperty
	 */
	virtual Mgx3D::Topo::CoEdgeMeshingProperty* getMeshingProperty ( ) const;

	/**
	 * Initialise le panneau avec la propriété de maillage transmise en
	 * argument.
	 * \param		La propriété de maillage des arêtes.
	 * \warning		<B>Méthode à surcharger, lève par défaut une exception.</B>
	 * \see			getMeshingProperty
	 */
	virtual void setMeshingProperty (
						const Mgx3D::Topo::CoEdgeMeshingProperty& properties);

	/**
	 * Pour la saisie interactive "fluidifié".
	 * \warning		A surcharger, retournent 0 par défaut.
	 */
	Mgx3D::QtComponents::EntitySeizureManager* getFirstSeizureManager ( ) const;
	Mgx3D::QtComponents::EntitySeizureManager* getLastSeizureManager ( ) const;


	signals :

	/**
	 * Emis lorsque l'utilisateur sélectionne un autre algorithme de 
	 * discrétisation.
	 */
	void algorithmModified ( );


	protected slots :

	/**
	 * Invoqué lorsque des entités sont ajoutées à la sélection.
	 * Emet le signal <I>entitiesAddedToSelection</I>.
	 */
	virtual void entitiesAddedToSelectionCallback (QString entitiesNames);

	/**
	 * Invoqué lorsque des entités sont enlevées de la sélection.
	 * Emet le signal <I>entitiesRemovedFromSelection</I>.
	 */
	virtual void entitiesRemovedFromSelectionCallback (QString entitiesNames);


	protected :

	/**
	 * \param	Widget parent.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 */
	QtEdgeMeshingPropertyPanelIfc (
		QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow);

	/**
	 * RAS.
	 */
	virtual ~QtEdgeMeshingPropertyPanelIfc ( );
	

	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtEdgeMeshingPropertyPanelIfc (const QtEdgeMeshingPropertyPanelIfc&);
	QtEdgeMeshingPropertyPanelIfc& operator = (
									const QtEdgeMeshingPropertyPanelIfc&);
};	// class QtEdgeMeshingPropertyPanelIfc


/**
 * <P>Panneau d'édition des paramètres de maillage d'une liste d'arêtes
 * topologiques.</P>
 * </P>
 */
class QtEdgeListMeshingPropertyPanel : public QtEdgeMeshingPropertyPanelIfc
{
	Q_OBJECT

	public :

	/** L'algorithme de maillage des arêtes.
	 * <OL>
	 * <LI>Discrétisation uniforme,
	 * <LI>Discrétisation avec progression géométrique.
	 * <LI>Discrétisation avec des segments de taille spécifique.
	 * <LI>Interpolation reposant les sommets d'un ensemble d'arêtes.
	 * <LI>Interpolation avec une progression à chaques extrémités.
	 * <LI>Interpolation avec une progression dite hyperbolique.
	 * <LI>Interpolation avec loi de resserrement beta.
	 * </OL>
	 */
	enum ALGORITHM { UNIFORM, GEOMETRIC_PROGRESSION, SPECIFIC_SIZE,
	                 INTERPOLATION, GLOBAL_INTERPOLATION, BI_GEOMETRIC, HYPERBOLIC, BETA };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 */
	QtEdgeListMeshingPropertyPanel (
		QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow, QtIntTextField* edgesNumTextField);

	/**
	 * RAS.
	 */
	virtual ~QtEdgeListMeshingPropertyPanel ( );

	/**
	 * \return		La méthode de discrétisation de l'arête.
	 */
	virtual ALGORITHM getAlgorithm ( ) const;

	/**
	 * \return		La propriété de maillage des arêtes.
	 * \warning		C'est à l'appelant de détruire l'instance retournée.
	 */
	virtual Mgx3D::Topo::CoEdgeMeshingProperty* getMeshingProperty ( ) const;

	/**
	 * Initialise le panneau avec la propriété de maillage transmise en
	 * argument.
	 * \param		La propriété de maillage des arêtes.
	 * \warning		<B>Méthode à surcharger, lève par défaut une exception.</B>
	 * \see			getMeshingProperty
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

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
	 * Pour la saisie interactive "fluidifié".
	 * \warning		A surcharger, retournent 0 par défaut.
	 */
	Mgx3D::QtComponents::EntitySeizureManager* getFirstSeizureManager ( ) const;
	Mgx3D::QtComponents::EntitySeizureManager* getLastSeizureManager ( ) const;

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getPanelEntities ( );


	protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des
	 * paramètres.
	 */
	virtual void algorithmCallback ( );


	protected :

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications classe de base.
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtEdgeListMeshingPropertyPanel (const QtEdgeListMeshingPropertyPanel&);
	QtEdgeListMeshingPropertyPanel& operator = (
									const QtEdgeListMeshingPropertyPanel&);

	/** La méthode de discrétisation des arêtes. */
	QComboBox*								_algorithmComboBox;

	/**
	 * Les panneaux de définition de la discrétisation des arêtes.
	 */
	//{@

	/** Parent du panneau courant. */
	QWidget*								_currentParentWidget;

	/** Panneau courant. */
	QtDiscretisationPanelIfc*				_currentPanel;

	/** Discrétisation uniforme. */
	QtUniformDiscretisationPanel*			_uniformDiscretisationPanel;

	/** Discrétisation par progression géométrique. */
	QtGeometricProgressionPanel*			_geometricProgressionPanel;

	/** Discrétisation par spécification de la taille des segments. */
	QtSpecificSizePanel*					_specificSizePanel;

	/** Discrétisation par interpolation à partir d'un ensemble d'arêtes. */
	QtInterpolatedEdgeDiscretisationPanel*	_interpolatedPanel;

	/** Discrétisation par interpolation à partir de 2 ensembles d'arêtes. */
	QtGlobalInterpolatedEdgeDiscretisationPanel* _globalInterpolatedPanel;

	/** Discrétisation par progression bi-géométrique. */
	QtBiGeometricProgressionPanel*			_bigeometricProgressionPanel;

	/** Discrétisation hyperbolique. */
	QtHyperbolicDiscretisationPanel*		_hyperbolicPanel;

	/** Discrétisation avec beta resserrement. */
	QtBetaDiscretisationPanel*              _betaPanel;

	/** Lien sur le widget de sélection du nombre de bras */
	QtIntTextField* _edgesNumTextField;

	//@}	// Panneaux de saisie des paramètres de définition du vertex
};	// class QtEdgeListMeshingPropertyPanel



/**
 * Panneau d'édition des paramètres de discrétisation d'arêtes topologiques
 * parallèles.
 */
class QtTopologyEdgesRefinementPanel : public QtEdgeMeshingPropertyPanelIfc
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 */
	QtTopologyEdgesRefinementPanel (
		QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgesRefinementPanel ( );

	/**
	 * Réinitialise le panneau à l'aide des propriétés de discrétisation
	 * transmises en  argument.
	 */
	virtual void setMeshingProperty (const Mgx3D::Topo::CoEdgeMeshingProperty&);

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getPanelEntities ( );

	/**
	 * Pour la saisie interactive "fluidifié".
	 */
	Mgx3D::QtComponents::EntitySeizureManager* getFirstSeizureManager ( ) const;
	Mgx3D::QtComponents::EntitySeizureManager* getLastSeizureManager ( ) const;

	/**
	 * \return	Le nom de l'arête de référence à discrétiser.
	 * \see		getFixedEdgeNames
	 */
	virtual std::string getRefinedReferenceEdgeName ( ) const;

	/**
	 * \return	Le nom des arêtes à ne pas discrétiser.
	 * \see		getRefinedReferenceEdgeName
	 */
	virtual std::vector<std::string> getFixedEdgeNames ( ) const;


	protected :

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyEdgesRefinementPanel (const QtTopologyEdgesRefinementPanel&);
	QtTopologyEdgesRefinementPanel& operator = (
										const QtTopologyEdgesRefinementPanel&);

	/** La saisie de l'arête de référence. */
	QtMgx3DEdgePanel*			_edgePanel;

	/** La saisie des arêtes à ne pas discrétiser. */
	QtMgx3DEdgePanel*			_fixedEdgesPanel;

};	// class QtTopologyEdgesRefinementPanel


/**
 * Panneau d'édition des paramètres de maillage d'arêtes topologiques.
 */
class QtEdgeMeshingPropertyPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de maillage de l'arête.
	 * <OL>
	 * <LI>Discrétisation d'une liste d'arêtes,
	 * <LI>Discrétisation d'arêtes parallèles,
	 * <LI>Discrétisation d'arêtes parallèles à une arête,
	 * </OL>
	 */
	enum OPERATION_METHOD { EDGE_LIST, PARALLEL_EDGES, CONSTRAINED_EDGES };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtEdgeMeshingPropertyPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtEdgeMeshingPropertyPanel ( );

	/**
	 * \return		La méthode de discrétisation de l'arête.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * \return		La propriété de maillage de l'arête.
	 * \warning		C'est à l'appelant de détruire l'instance retournée.
	 */
	virtual Mgx3D::Topo::CoEdgeMeshingProperty* getMeshingProperty ( ) const;

	/**
	 * \return		La liste des arêtes sélectionnées.
	 */
	virtual std::vector<std::string> getEdgesNames ( ) const;

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
	 * \return	Le sous-panneau courant.
	 */
	virtual QtEdgeMeshingPropertyPanelIfc* getCurrentPanel ( );

	/**
	 * \return		Le nombre d'arêtes.
	 */
	virtual size_t getEdgesNum ( ) const;

	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );


	protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des
	 * paramètres.
	 */
	virtual void operationMethodCallback ( );

	/**
	 * Appelé lorsque l'algorithme de la méthode change. Actualise le panneau
	 * de saisie des paramètres.
	 */
	virtual void algorithmModifiedCallback ( );

	/**
	 * Réinitialise, si possible, le panneau en reprenant les paramètres de
	 * discrétisation des arêtes sélectionnées.
	 */
	virtual void reinitializeCallback ( );


	protected :

	/**
	 * \param		<I>true</I> pour prévisualiser la discrétisation,
	 * 				<I>false</I> pour arrêter la prévisualisation.
	 * \param		inutilisé
	 */
	virtual void preview (bool show, bool destroyInteractor);

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications classe de base.
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtEdgeMeshingPropertyPanel (const QtEdgeMeshingPropertyPanel&);
	QtEdgeMeshingPropertyPanel& operator = (const QtEdgeMeshingPropertyPanel&);

	/** La méthode de discrétisation de l'arête. */
	QComboBox*								_operationMethodComboBox;

	/**
	 * Les panneaux de définition de la discrétisation des arêtes.
	 */
	//{@

	/** Parent du panneau courant. */
	QWidget*								_currentParentWidget;

	/** Panneau courant. */
	QtEdgeMeshingPropertyPanelIfc*			_currentPanel;

	/** Discrétisation d'une liste d'arêtes. */
	QtEdgeListMeshingPropertyPanel*			_edgeListPanel;

	/** Discrétisation d'arêtes parallèles. */
	QtTopologyEdgesRefinementPanel*			_edgesRefinementPanel;

	//@}	// Panneaux de saisie des paramètres de définition du vertex

	/** Les arêtes à discrétiser. */
	QtMgx3DEntityPanel*						_edgesPanel;

	/** Faut-il projeter l'aperçu pour les arêtes ? */
	QCheckBox*								_previewProjectedCheckBox;

	/** La saisie du nombre d'arêtes (mutualisé pour toutes les méthodes). */
	QtIntTextField*							_edgesNumTextField;

};	// class QtEdgeMeshingPropertyPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtEdgeMeshingPropertyPanel</I> de modification des paramètres de
 * maillage d'une arête.
 */
class QtEdgeMeshingPropertyAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtEdgeMeshingPropertyPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtEdgeMeshingPropertyAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtEdgeMeshingPropertyAction ( );

	/**
	 * \return		Le panneau d'édition de discrétisation d'arêtes associé.
	 */
	virtual QtEdgeMeshingPropertyPanel* getMeshingPropertyPanel ( );

	/**
	 * Actualise la discrétisation des arêtes avec le paramétrage de son
	 * panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );

	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtEdgeMeshingPropertyAction (const QtEdgeMeshingPropertyAction&);
	QtEdgeMeshingPropertyAction& operator = (
									const QtEdgeMeshingPropertyAction&);
};  // class QtEdgeMeshingPropertyAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_EDGE_MESHING_PROPERTY_ACTION_H
