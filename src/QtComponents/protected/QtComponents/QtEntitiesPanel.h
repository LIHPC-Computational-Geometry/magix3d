/**
 * \file		QtEntitiesPanel.h
 * \author		Charles PIGNEROL
 * \date		29/11/2010
 */
#ifndef QT_ENTITIES_PANEL_H
#define QT_ENTITIES_PANEL_H

#include "Internal/ContextIfc.h"

#include <QSettings>
#include <QItemDelegate>
#include <QMenu>
#include <QTreeWidget>

#include "QtComponents/QtEntitiesItemViewPanel.h"
#include "Utils/SelectionManager.h"
#include "Geom/GeomEntity.h"
#include "Topo/TopoEntity.h"
#include "Mesh/MeshEntity.h"
#include "SysCoord/SysCoord.h"
#include "Structured/StructuredMeshEntity.h"

#include <vector>


namespace Mgx3D 
{

namespace QtComponents
{
	class QtEntitiesPanel;


/**
 * \brief		Classe d'item dans un widget de type <I>QTreeWidget</I>
 *				représentant une entité Magix 3D.
 */
class QtEntityTreeWidgetItem :
	public QTreeWidgetItem, public Mgx3D::Utils::DisplayProperties::EntityView
{
	public :

	/**
	 * \param		Item parent
	 * \param		Entité représentée
	 * \param		Représentation 3D actuelle (cf.
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres)
	 */
	QtEntityTreeWidgetItem (QTreeWidgetItem* parent, Mgx3D::Utils::Entity& entity, unsigned long rep3d);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtEntityTreeWidgetItem ( );

	/**
	 * \return		L'entité représentée.
	 */
	virtual Mgx3D::Utils::Entity* getEntity ( );
	virtual const Mgx3D::Utils::Entity* getEntity ( ) const;

	/**
	 * \return		La représentation 3D proposée à la construction.
	 * \see			<I>GraphicalEntityRepresentation::CURVES</I> et autres
	 * \see			getRepresentationMask
	 */
	virtual unsigned long getNativeRepresentationMask ( ) const;

	/**
	 * \return		La représentation 3D actuelle.
	 * \see			<I>GraphicalEntityRepresentation::CURVES</I> et autres
	 * \see			setRepresentationMask
	 */
	virtual unsigned long getRepresentationMask ( ) const;

	/**
	 * \return		La représentation 3D actuelle.
	 * \see			<I>GraphicalEntityRepresentation::CURVES</I> et autres
	 * \see			getRepresentationMask
	 * \deprecated
	 */
	virtual void setRepresentationMask (unsigned long representation);

	/**
	 * Actualise les données affichées.
	 * \param		Si <I>updateRepresentation</I> vaut <I>true</I> actualise
	 *				également la représentation 3D de l'entité.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * L'entité est affichée/masquée.
	 */
	virtual void entityDisplayed (bool displayed);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */	
	QtEntityTreeWidgetItem (const QtEntityTreeWidgetItem&);
	QtEntityTreeWidgetItem& operator = (const QtEntityTreeWidgetItem&);

	/** L'entité représentée. */
	Mgx3D::Utils::Entity*				_entity;

	/** La représentation 3D actuelle. */
	unsigned long					_3dRep;
};	// class QtEntityTreeWidgetItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'entités
 * géométriques.
 */
class QtCADEntityTreeWidgetItem : public QtEntityTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Entité représentée
	 * \param		Représentation 3D actuelle (cf.
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres)
	 */
	QtCADEntityTreeWidgetItem (QTreeWidgetItem* parent, Geom::GeomEntity& entity, unsigned long rep3d);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCADEntityTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 * \param		Si <I>updateRepresentation</I> vaut <I>true</I> actualise
	 *				également la représentation 3D de l'entité.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * \return		Pointeur sur l'entité géométrique représentée, ou 0 s'il
	 *				n'y en a pas.
	 */
	virtual Geom::GeomEntity* getGeomEntity ( )
	{ return _geomEntity; }
	virtual const Geom::GeomEntity* getGeomEntity ( ) const
	{ return _geomEntity; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCADEntityTreeWidgetItem (const QtCADEntityTreeWidgetItem&);
	QtCADEntityTreeWidgetItem& operator = (const QtCADEntityTreeWidgetItem&);

	/** L'entité géométrique représentée. */
	Geom::GeomEntity*					_geomEntity;
};	// class QtCADEntityTreeWidgetItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'entités
 * topologiques.
 */
class QtTopologicEntityTreeWidgetItem : public QtEntityTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Entité représentée
	 * \param		Représentation 3D actuelle (cf.
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres)
	 */
	QtTopologicEntityTreeWidgetItem (QTreeWidgetItem* parent, Topo::TopoEntity& entity, unsigned long rep3d);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologicEntityTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 * \param		Si <I>updateRepresentation</I> vaut <I>true</I> actualise
	 *				également la représentation 3D de l'entité.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * \return		Pointeur sur l'entité topologique représentée, ou 0 s'il
	 *				n'y en a pas.
	 */
	virtual Topo::TopoEntity* getTopologicEntity ( )
	{ return _topoEntity; }
	virtual const Topo::TopoEntity* getTopologicEntity ( ) const
	{ return _topoEntity; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologicEntityTreeWidgetItem (const QtTopologicEntityTreeWidgetItem&);
	QtTopologicEntityTreeWidgetItem& operator = (const QtTopologicEntityTreeWidgetItem&);

	/** L'entité géométrique représentée. */
	Topo::TopoEntity*					_topoEntity;
};	// class QtTopologicEntityTreeWidgetItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'entités
 * maillages.
 */
class QtMeshEntityTreeWidgetItem : public QtEntityTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Entité représentée
	 * \param	Représentation 3D actuelle (cf.<I>GraphicalEntityRepresentation::CURVES</I> et autres)
	 */
	QtMeshEntityTreeWidgetItem (QTreeWidgetItem* parent, Mesh::MeshEntity& entity, unsigned long rep3d);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshEntityTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 * \param		Si <I>updateRepresentation</I> vaut <I>true</I> actualise
	 *			également la représentation 3D de l'entité.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * \return		Pointeur sur l'entité maillage représentée, ou 0 s'il n'y en a pas.
	 */
	virtual Mesh::MeshEntity* getMeshEntity ( )
	{ return _meshEntity; }
	virtual const Mesh::MeshEntity* getMeshEntity ( ) const
	{ return _meshEntity; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshEntityTreeWidgetItem (const QtMeshEntityTreeWidgetItem&);
	QtMeshEntityTreeWidgetItem& operator = (const QtMeshEntityTreeWidgetItem&);

	/** L'entité maillage représentée. */
	Mesh::MeshEntity*					_meshEntity;
};	// class QtMeshEntityTreeWidgetItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'entités
 * repère.
 */
class QtSysCoordEntityTreeWidgetItem : public QtEntityTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Entité représentée
	 * \param	Représentation 3D actuelle (cf.<I>GraphicalEntityRepresentation::CURVES</I> et autres)
	 */
	QtSysCoordEntityTreeWidgetItem (QTreeWidgetItem* parent, CoordinateSystem::SysCoord& entity, unsigned long rep3d);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSysCoordEntityTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 * \param		Si <I>updateRepresentation</I> vaut <I>true</I> actualise
	 *			également la représentation 3D de l'entité.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * \return		Pointeur sur l'entité repère représentée, ou 0 s'il
	 *			n'y en a pas.
	 */
	virtual CoordinateSystem::SysCoord* getSysCoordEntity ( )
	{ return _sysCoordEntity; }
	virtual const CoordinateSystem::SysCoord* getSysCoordEntity ( ) const
	{ return _sysCoordEntity; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSysCoordEntityTreeWidgetItem (const QtSysCoordEntityTreeWidgetItem&);
	QtSysCoordEntityTreeWidgetItem& operator = (const QtSysCoordEntityTreeWidgetItem&);

	/** L'entité repère représentée. */
	CoordinateSystem::SysCoord*			_sysCoordEntity;
};	// class QtSysCoordEntityTreeWidgetItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'entités
 * maillages structurés.
 */
class QtStructuredMeshEntityTreeWidgetItem : public QtEntityTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Entité représentée
	 * \param	Représentation 3D actuelle (cf.<I>GraphicalEntityRepresentation::CURVES</I> et autres)
	 */
	QtStructuredMeshEntityTreeWidgetItem (QTreeWidgetItem* parent, Structured::StructuredMeshEntity& entity, unsigned long rep3d);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtStructuredMeshEntityTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 * \param		Si <I>updateRepresentation</I> vaut <I>true</I> actualise
	 *			également la représentation 3D de l'entité.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * \return		Pointeur sur l'entité maillage structurée représentée, ou 0 s'il
	 *			n'y en a pas.
	 */
	virtual Structured::StructuredMeshEntity* getStructuredMeshEntity ( )
	{ return _structuredMeshEntity; }
	virtual const Structured::StructuredMeshEntity* getStructuredMeshEntity ( ) const
	{ return _structuredMeshEntity; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtStructuredMeshEntityTreeWidgetItem (const QtStructuredMeshEntityTreeWidgetItem&);
	QtStructuredMeshEntityTreeWidgetItem& operator = (const QtStructuredMeshEntityTreeWidgetItem&);

	/** L'entité maillage structuré représentée. */
	Structured::StructuredMeshEntity*		_structuredMeshEntity;
};	// class QtStructuredMeshEntityTreeWidgetItem


/**
 * \brief		Widget "arbre" Qt contenant les entités Magix 3D. Classe créée
 *				pour bénéficier du service <I>selectionChanged</I>.
 *
 *				<P>Les items contenus par ce panneau sont de type 
 *				<I>QtEntityTreeWidgetItem</I>.
 *				</P>
 */
class QtEntitiesTreeWidget : public QTreeWidget
{
	public :

	/**
	 * Constructeur. RAS.
	 */
	QtEntitiesTreeWidget (QtEntitiesPanel& parent);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtEntitiesTreeWidget ( );

	/**
	 * \return		Le panneau associé gérant les entités.
	 */
	virtual QtEntitiesPanel* getEntitiesPanel ( )	
	{ return _entitiesPanel; }
	virtual const QtEntitiesPanel* getEntitiesPanel ( )	const
	{ return _entitiesPanel; }

#ifndef REPRESENTATION_TYPE_BY_POPUP
	/**
	 * La gestion des représentations possibles de l'entité représentée.
	 */
	//@{
	/**
	 * Classe déléguée d'édition des items d'instances de la classe
	 * QtEntityTreeWidgetItem.
	 * Cette classe créé le type d'éditeur approprié selon la zone à éditer.
	 */
	class QtEntityTreeWidgetItemDelegate : public QItemDelegate
	{
		public :

		/**
		 * Panneau de gestion des entités de rattachement.
		 */
		QtEntityTreeWidgetItemDelegate (QtEntitiesPanel*);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~QtEntityTreeWidgetItemDelegate ( );

		/**
		 * \return		L'éditeur adapté selon le contexte de l'appel.
		 */
		virtual QWidget* createEditor (QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

		/**
		 * Affectation à l'entité représentée des données venant d'être
		 * renseignées par l'utilisateur.
		 */
		virtual void setModelData (QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

	
		private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		QtEntityTreeWidgetItemDelegate (const QtEntityTreeWidgetItemDelegate&);
		QtEntityTreeWidgetItemDelegate& operator = (const QtEntityTreeWidgetItemDelegate&);

		/** Le panneau associé. */
		QtEntitiesPanel*			_entitiesPanel;
	};	// class QtEntityTreeWidgetItemDelegate
	//@}
#endif	// REPRESENTATION_TYPE_BY_POPUP


	protected :

	/**
	 * Appelé lorsque la sélection change. En informe le panneau auquel ce
	 * widget est rattaché.
	 * \see		QtEntitiesPanel::selectionChanged
	 * \see		_entitiesPanel
	 */
	virtual void selectionChanged (const QItemSelection& selected, const QItemSelection & deselected);

	/**
	 * <P>Les listes Qt transmises à QTreeWidget::selectionChanged contiennent
	 * des cellules sélectionnées. Un item peut donc y apparaître autant de fois
	 * qu'il a de cellules (nombre de colonnes).</P>
	 *
	 * <P>Cette fonction réduit la liste d'entités (dé)sélectionnées tranmise en
	 * argument à une liste d'instance de QTreeWidgetItem où chaque instance est
	 * présente de manière unique.</P>
	 */
	virtual std::vector<QTreeWidgetItem*> filteredSelectionList (const QItemSelection& list) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtEntitiesTreeWidget (const QtEntitiesTreeWidget&);
	QtEntitiesTreeWidget& operator = (const QtEntitiesTreeWidget&);

	/** Le panneau auquel est rattaché ce widget. */
	QtEntitiesPanel*			_entitiesPanel;
};	// class QtEntitiesTreeWidget


/**
 * Classe contenant les actions du menu contextuel de la vue arborescente
 * de la classe <I>QtEntitiesPanel</I>.
 *
 * \see		QtEntitiesPanel
 * \see		QtEntitiesPanel::createPopupMenu
 * \see		QtEntitiesPanel::displayTreeViewPopupMenu
 */
struct QTreeViewPopupActions
{
	QTreeViewPopupActions ( );
	QTreeViewPopupActions (const QTreeViewPopupActions&);
	QTreeViewPopupActions& operator = (const QTreeViewPopupActions&);
	virtual ~QTreeViewPopupActions ( );
	virtual void setEnabled (bool enable);

	// Affichage :
	QAction			*_displayAction, *_hideAction;
	QAction			*_representationMaskAction;
};	// struct QTreeViewPopupActions


/**
 * \brief		Panneau de l'IHM <I>Magix 3D</I> affichant les différentes
 *				entités (CAO, topologie, Maillage) existantes d'une session.
 *				L'affichage est d'une part textuelle, d'autre part dans une
 *				fenêtre graphique 3D.
 *
 *				<P>Ce panneau permet également de sélectionner des entités. La
 *				gestion de la sélection est effectuée au niveau du contexte.
 *				</P>
 */
class QtEntitiesPanel :public QtEntitiesItemViewPanel, public Utils::SelectionManagerObserver
{
	friend class QtEntitiesTreeWidget;	// beurk

	Q_OBJECT

	public :

	/**
	 * Appelle createGui et createPopupMenu.
	 * \param	Widget parent.
	 * \param	Fenêtre principale de rattachement du panneau
	 * \param	Nom du panneau.
	 * \param	Contexte Magix 3D du panneau
	 * \see		createGui
	 * \see		createPopupMenu
	 */
	QtEntitiesPanel (QWidget* parent, QtMgx3DMainWindow* mainWindow,
	                 const std::string& name, Internal::ContextIfc& context);

	/**
	 * RAS.
	 */
	virtual ~QtEntitiesPanel ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Enregistre les paramètres d'affichage (taille, position, ...) de cette
	 * fenêtre.
	 * \see		readSettings
	 */
	virtual void writeSettings (QSettings& settings);

	/**
	 * Lit et s'applique les paramètres d'affichage (taille, position, ...) de
	 * cette fenêtre.
	 * \see		writeSettings
	 */
	virtual void readSettings (QSettings& settings);

	//@}	// Divers IHM


	/**
	 * Les objets représentés.
	 */
	//@{

	/**
	 * Ajoute l'instance transmise en argument à la liste des objets C.A.O.
	 * représentés.
	 * \param		Instance C.A.O. à ajouter
	 * \param		<I>true</I> s'il faut l'afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeCADEntity
	 * \deprecated
	 */
	virtual void addCADEntity (Mgx3D::Geom::GeomEntity& entity, bool show);

	/**
	 * Ajoute les instances transmises en argument à la liste des objets C.A.O.
	 * représentés.
	 * \param		Instances C.A.O. à ajouter
	 * \param		<I>true</I> s'il faut les afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeCADEntities
	 * \see			updateEntryItems
	 */
	virtual void addCADEntities (const std::vector<Mgx3D::Geom::GeomEntity*>& entities, bool show);

	/**
	 * Enlève l'instance transmise en argument de la liste des objets C.A.O.
	 * représentés.
	 * \param		Instance C.A.O. à enlever
	 * \see			addCADEntity
	 * \deprecated
	 */
	virtual void removeCADEntity (Mgx3D::Geom::GeomEntity& entity);

	/**
	 * Enlève les instances transmises en argument de la liste des objets C.A.O.
	 * représentés.
	 * \param		Instances C.A.O. à enlever
	 * \see			addCADEntities
	 * \see			updateEntryItems
	 */
	virtual void removeCADEntities (const std::vector<Mgx3D::Geom::GeomEntity*>& entities);


	/**
	 * Ajoute l'instance transmise en argument à la liste des objets
	 * topologiques représentés.
	 * \param		Instance topologique à ajouter
	 * \param		<I>true</I> s'il faut l'afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeTopologicEntity
	 * \deprecated
	 */
	virtual void addTopologicEntity(Mgx3D::Topo::TopoEntity& entity, bool show);

	/**
	 * Ajoute les instances transmises en arguments à la liste des objets
	 * topologiques représentés.
	 * \param		Instances topologiques à ajouter
	 * \param		<I>true</I> s'il faut les afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeTopologicEntities
	 * \see			updateEntryItems
	 */
	virtual void addTopologicEntities (const std::vector<Mgx3D::Topo::TopoEntity*>& entities, bool show);

	/**
	 * Enlève l'instance transmise en argument de la liste des objets 
	 * topologiques représentés.
	 * \param		Instance topologique à enlever
	 * \see			addTopologicEntity
	 * \deprecated
	 */
	virtual void removeTopologicEntity (Mgx3D::Topo::TopoEntity& entity);

	/**
	 * Enlève les instances transmises en arguments de la liste des objets
	 * topologiques représentés.
	 * \param		Instances topologiques à enlever
	 * \see			addTopologicEntities
	 * \see			updateEntryItems
	 */
	virtual void removeTopologicEntities (const std::vector<Mgx3D::Topo::TopoEntity*>& entities);

	/**
	 * Ajoute l'instance transmise en argument à la liste des objets
	 * maillages représentés.
	 * \param		Instance maillage à ajouter
	 * \param		<I>true</I> s'il faut l'afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeMeshEntity
	 * \deprecated
	 */
	virtual void addMeshEntity(Mgx3D::Mesh::MeshEntity& entity, bool show);

	/**
	 * Ajoute les instances transmises en arguments à la liste des objets
	 * maillages représentés.
	 * \param		Instances maillage à ajouter
	 * \param		<I>true</I> s'il faut les afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeMeshEntities
	 * \see			updateEntryItems
	 */
	virtual void addMeshEntities (const std::vector<Mgx3D::Mesh::MeshEntity*>& entities, bool show);

	/**
	 * Enlève l'instance transmise en argument de la liste des objets 
	 * maillages représentés.
	 * \param		Instance maillage à enlever
	 * \see			addMeshEntity
	 * \deprecated
	 */
	virtual void removeMeshEntity (Mgx3D::Mesh::MeshEntity& entity);

	/**
	 * Enlève les instances transmises en arguments de la liste des objets
	 * maillages représentés.
	 * \param		Instances maillage à enlever
	 * \see			addMeshEntities
	 * \see			updateEntryItems
	 */
	virtual void removeMeshEntities (const std::vector<Mgx3D::Mesh::MeshEntity*>& entities);


	/**
	 * Ajoute l'instance transmise en argument à la liste des objets repère
	 * représentés.
	 * \param		Instance repère à ajouter
	 * \param		<I>true</I> s'il faut l'afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeSysCoordEntity
	 * \deprecated
	 */
	virtual void addSysCoordEntity (Mgx3D::CoordinateSystem::SysCoord& entity, bool show);


	/**
	 * Enlève l'instance transmise en argument de la liste des objets repère
	 * représentés.
	 * \param		Instance repère à enlever
	 * \see			addSysCoordEntity
	 * \deprecated
	 */
	virtual void removeSysCoordEntity (Mgx3D::CoordinateSystem::SysCoord& entity);


	/**
	 * Ajoute l'instance transmise en argument à la liste des objets
	 * maillages structurés représentés.
	 * \param		Instance maillage à ajouter
	 * \param		<I>true</I> s'il faut l'afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeMeshEntity
	 * \deprecated
	 */
	virtual void addStructuredMeshEntity(Mgx3D::Structured::StructuredMeshEntity& entity, bool show);

	/**
	 * Ajoute les instances transmises en arguments à la liste des objets
	 * maillages structurés représentés.
	 * \param		Instances maillage à ajouter
	 * \param		<I>true</I> s'il faut les afficher, <I>false</I> dans le cas
	 *				contraire.
	 * \see			removeMeshEntities
	 * \see			updateEntryItems
	 */
	virtual void addStructuredMeshEntities (const std::vector<Mgx3D::Structured::StructuredMeshEntity*>& entities, bool show);

	/**
	 * Enlève l'instance transmise en argument de la liste des objets 
	 * maillages structurés représentés.
	 * \param		Instance maillage à enlever
	 * \see			addMeshEntity
	 * \deprecated
	 */
	virtual void removeStructuredMeshEntity (Mgx3D::Structured::StructuredMeshEntity& entity);

	/**
	 * Enlève les instances transmises en arguments de la liste des objets
	 * maillages structurés représentés.
	 * \param		Instances maillage à enlever
	 * \see			addMeshEntities
	 * \see			updateEntryItems
	 */
	virtual void removeStructuredMeshEntities (const std::vector<Mgx3D::Structured::StructuredMeshEntity*>& entities);

	/**
	 * Actualise l'item "entrée" en fonction de ses enfants.
	 * \param		Types d'entrées à actualiser
	 */
	virtual void updateEntryItems (Mgx3D::Utils::DisplayRepresentation::display_type type);

	//@}

	/**
	 * La gestion de la sélection.
	 */
	//@{

	/**
	 * Ajoute les entités tranmises en arguments à la sélection.
	 */
	virtual void entitiesAddedToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * Enlève les entités tranmises en arguments à la sélection.
	 * @param	Entités enlevées de la sélection
	 * @param	<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des fins
	 * 			d'optimisation.
	 */
	virtual void entitiesRemovedFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);

	/**
	 * Annule la sélection en cours.
	 */
	virtual void selectionCleared ( );

	/**
	 * \return	Un masque sur les types d'entités filtrés, celles sélectionnées.
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getSelectedEntitiesTypes ( ) const;

	//@}

	/**
	 * La vue "arborescente".
	 */
	//@{

	/**
	 * \return		La fenêtre <I>QtEntityTreeWidget</I> utilisée.
	 * \warning		Méthode avant tout à usage interne.
	 */
	virtual QtEntitiesTreeWidget* getTreeWidget ( );
	virtual const QtEntitiesTreeWidget* getTreeWidget ( ) const;

	//@}	// La vue "arborescente".

	/**
	 * Les affichages graphiques 3D.
	 */
	//@{

	/**
	 * \param		La fenêtre graphique à utiliser pour les affichages 3D,
	 *				ou 0.
	 * \exception	Une exception est levée si la fenêtre transmise en
	 *				argument est non nulle et qu'une autre fenêtre est déjà
	 *				affectée.
	 * \see			getGraphicalWidget
	 */
	virtual void setGraphicalWidget (Qt3DGraphicalWidget* widget3D);

	//@}	// Les affichages graphiques 3D.


	protected :

	/**
	 * Création de l'IHM (aspects purements Qt).
	 * \see		createPopupMenu
	 */
	virtual void createGui ( );

	/**
	 * Création du menu contextuel.
	 * \see		displayTreeViewPopupMenu
	 */
	virtual void createPopupMenu ( );

	/**
	 * Appelé lorsque la sélection est modifiée au nineau du panneau "arbre".
	 * Passe en argument les entités nouvellement (dé)sélectionnées.
	 * Propage la modification de la sélection au gestionnaire de sélection.
	 * \see			QtEntitiesTreeWidget
	 */
	virtual void selectionChanged (std::vector<Mgx3D::Utils::Entity*>& selectedEntities, std::vector<Mgx3D::Utils::Entity*>& deselectedEntities);

	/**
	 * \return		L'item représentant l'entité transmise en argument.
	 */
	virtual QtEntityTreeWidgetItem* getItem(const Mgx3D::Utils::Entity& entity);

	/**
	 * \param		Item parent
	 * \param		Libellé de l'item
	 * \param		Masque d'affichage par défaut
	 * \param		Type d'entités de l'entrée
	 * \return		Une entrée principale dans l'arborescence des entités.
	 * \see			getCADEntryItem	
	 * \see			getTopologicEntryItem	
	 * \see			createCADItem
	 * \see			createTopologicItem
	 * \see			GraphicalEntityRepresentation::CLOUDS
	 * \see			GraphicalEntityRepresentation::CURVES
	 * \see			updateEntryItems
	 */
	virtual QTreeWidgetItem* createEntryItem (
		QTreeWidgetItem& parent, const std::string& title, unsigned long mask,
		Mgx3D::Utils::DisplayRepresentation::display_type entitiesType);

	/**
	 * \return		Un item représentant l'entité C.A.O. transmise en argument.
	 */
	virtual QtCADEntityTreeWidgetItem* createCADItem (Mgx3D::Geom::GeomEntity&);

	/**
	 * \return		L'entrée principale dans l'arborescence C.A.O. dont la
	 *				dimension est transmise en argument.
	 * \exception	Une exception est levée en cas de dimension invalide.
	 * \see			createEntryItem
	 */
	virtual QTreeWidgetItem* getCADEntryItem (unsigned char dim) const;

	/**
	 * \return		Un item représentant l'entité topologique transmise en
	 *				argument.
	 */
	virtual QtTopologicEntityTreeWidgetItem* createTopologicItem (Mgx3D::Topo::TopoEntity&);

	/**
	 * \return		L'entrée principale dans l'arborescence topologique dont la
	 *				dimension est transmise en argument.
	 * \exception	Une exception est levée en cas de dimension invalide.
	 * \see			createEntryItem
	 */
	virtual QTreeWidgetItem* getTopologicEntryItem (unsigned char dim) const;

	/**
	 * \return		Un item représentant l'entité maillage transmise en
	 *				argument.
	 */
	virtual QtMeshEntityTreeWidgetItem* createMeshItem (Mgx3D::Mesh::MeshEntity&);

	/**
	 * \return		L'entrée principale dans l'arborescence maillage dont la
	 *				dimension est transmise en argument.
	 * \exception	Une exception est levée en cas de dimension invalide.
	 * \see			createEntryItem
	 */
	virtual QTreeWidgetItem* getMeshEntryItem (unsigned char dim) const;

	/**
	 * \return		Un item représentant l'entité repère transmise en argument.
	 */
	virtual QtSysCoordEntityTreeWidgetItem* createSysCoordItem (Mgx3D::CoordinateSystem::SysCoord&);

	/**
	 * \return		L'entrée principale dans l'arborescence repère
	 * \see			createEntryItem
	 */
	virtual QTreeWidgetItem* getSysCoordEntryItem () const;

	/**
	 * \return		Un item représentant l'entité maillage transmise en
	 *				argument.
	 */
	virtual QtStructuredMeshEntityTreeWidgetItem* createStructuredMeshItem (Mgx3D::Structured::StructuredMeshEntity&);

	/**
	 * \return		L'entrée principale dans l'arborescence maillage structuré dont la
	 *			dimension est transmise en argument.
	 * \exception		Une exception est levée en cas de dimension invalide.
	 * \see			createEntryItem
	 */
	virtual QTreeWidgetItem* getStructuredMeshEntryItem (unsigned char dim) const;

	/**
	 * Actualise l'item "entrée" transmis en fonction de ses enfants.
	 * \param		Item entrée à actualiser
	 */
	virtual void updateEntryItem (QTreeWidgetItem& entryItem);

	/**
	 * Le type de d'entités correspondant à l'item (supposé "entrée") transmis
	 * en argument.
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getFilteredType (QTreeWidgetItem* item) const;


	public :

	/**
	 * \return		<I>true</I> si lors de la création d'un item le
	 *				callback <I>stateChangeCallback</I> doit être
	 *				invoqué si nécessaire. Optimisation pour le mode
	 *				client/serveur ... Retourne <I>true</I> par défaut.
	 */
	virtual bool itemCreationTriggersDisplayCallback ( ) const;

	/**
	 * \return		<I>true</I> lorsque les modifications graphiques sur
	 *				les entités doivent être groupées si possible.
	 *				Optimisation pour le mode client/serveur ...
	 *				Retourne <I>false</I> par défaut.
	 */
	virtual bool groupEntitiesDisplayModifications ( ) const;

	/**
	 * Appelé pour afficher/masquer les entités sélectionnées. Actualise
	 * également l'IHM (cases à cocher) sans envoyer de signaux.
	 */
	virtual void showSelectedEntities (bool show);


	protected slots :

	/**
	 * Appellé lorsque l'utilisateur clique sur un item. Affiche/Masque
	 * la représentation de l'entité dans la fenêtre graphique 3D.
	 * Invoque <I>itemStateChange</I> en demandant l'actualisation de l'item
	 * parent.
	 * \see			itemStateChange
	 */
	virtual void stateChangeCallback (QTreeWidgetItem*, int);

	/**
	 * Appelé pour afficher/masquer les entités sélectionnées.
	 * \see	showSelectedEntities
	 */
	virtual void displayEntitiesCallback ( );
	virtual void hideEntitiesCallback ( );

	/**
	 * Appelé pour modifier les paramètres d'affichage
	 * (points/filaire/surfacique) des entités . Affiche une boite de dialogue
	 * de choix des types de représentation à utiliser, initialisé selon les
	 * entités sélectionnées dans l'arborescence d'entités, et effectue les
	 * modifications demandées.
	 */
	virtual void representationTypesCallback ( );

	/**
	 * Appelé lorsque le menu popup est demandé au niveau de l'arborescence des
	 * entités. Affiche ce menu popup à la position transmise en argument.
	 */
	virtual void displayTreeViewPopupMenu (const QPoint&);


	protected :

	/**
	 * Actualise la représentation de l'entité associée à l'item selon la valeur
	 * de la case à cocher en colonne 0 et supposée avoir été modifiée et à
	 * l'origine de cet appel.
	 * Si <I>updateParent</I> vaut true actualise la case à cocher en colonne 0
	 * du parent.
	 */
	virtual void itemStateChange (QTreeWidgetItem*, bool updateParent);

	/**
	 * Actualise la représentation des entités associées aux items transmis en
	 * argument selon la valeur de leurs cases à cocher en colonne 0 et
	 * supposées avoir été modifiées et à l'origine de cet appel.
	 * Si <I>updateParent</I> vaut true actualise la case à cocher en colonne 0
	 * du parent.
	 */
	virtual void itemsStateChange (const std::vector<QTreeWidgetItem*>&, bool updateParent);

	/**
	 * Appelé pour afficher/masquer les entités sélectionnées. Actualise
	 * également l'IHM (cases à cocher) sans envoyer de signaux.
	 */
//	virtual void showSelectedEntities (bool show);


	// Opérations interdites :
	QtEntitiesPanel (const QtEntitiesPanel&);


	private :

	QtEntitiesPanel& operator = (const QtEntitiesPanel&);

	/** Le widget représentant les entités. */
	QtEntitiesTreeWidget*			_entitiesWidget;

	/** Les entrées principales pour la CAO, la topologie, le maillage, le maillage, les repères, les maillages structurés. */
	QTreeWidgetItem				*_cadItem, *_topologyItem, *_meshingItem, *_coordinateSystemItem, *_structuredMeshingItem;

	/** Les entrées par dimension pour la CAO. */
	QTreeWidgetItem				*_cadVolumesItem, *_cadSurfacesItem, *_cadCurvesItem, *_cadCloudsItem;

	/** Les entrées par dimension pour la topologie. */
	QTreeWidgetItem				*_topoBlocksItem, *_topoFacesItem, *_topoEdgesItem, *_topoVerticesItem;

	/** Les entrées par dimension pour le maillage. */
	QTreeWidgetItem				*_meshVolumesItem, *_meshSurfacesItem, *_meshLinesItem, *_meshCloudsItem;

	/** une seule dimension pour les repères */
	QTreeWidgetItem                 	*_sysCoordItem;

	/** Une seule dimension - 3D - (pour l'instant) pour les maillages structurés. */
	QTreeWidgetItem				*_structuredMeshVolumesItem;

	/** Le menu contextuel de l'arborescence d'items. */
	QMenu*					_treeViewPopupMenu;
	QTreeViewPopupActions			_treeViewActions;
};	// class QtEntitiesPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_ENTITIES_PANEL_H
