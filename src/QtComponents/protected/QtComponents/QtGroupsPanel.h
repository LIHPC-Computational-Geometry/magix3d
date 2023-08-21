/**
 * \file		QtGroupsPanel.h
 * \author		Charles PIGNEROL
 * \date		12/11/2012
 */
#ifndef QT_GROUPS_PANEL_H
#define QT_GROUPS_PANEL_H


#include "Internal/ContextIfc.h"

#include "Group/GroupEntity.h"
#include "QtComponents/QtEntitiesItemViewPanel.h"

#include <QCheckBox>
#include <QMenu>
#include <QSettings>
#include <QTreeWidget>

#include <map>
#include <vector>


namespace Mgx3D 
{

namespace QtComponents
{
	class QtGroupsPanel;


/**
 * Classe d'item représentant les types d'entités (Géométriques : Volumes/Surfaces/Courbes/Points, Topologiques : Blocs, Faces, Arêtes,
 * Sommets, Maillages : Volumes, Surfaces, Lignes, Nuages).
 */
class QtEntityTypeItem : public QTreeWidgetItem
{
	public :

	/**
	 * Constructeur.
	 * \param		Item parent.
	 * \param		Type d'entité représenté.
	 * \see			getEntityType
	 */
	QtEntityTypeItem (QTreeWidgetItem* parent, Mgx3D::Utils::FilterEntity::objectType type);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtEntityTypeItem ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Enregistre les paramètres d'affichage (coché, ...) de cet item.
	 * \see		readSettings
	 */
	virtual void writeSettings (QSettings& settings);

	/**
	 * Lit et s'applique  les paramètres d'affichage (coché, ...) de cet item.
	 * \see		writeSettings
	 */
	virtual void readSettings (QSettings& settings);

	//@}	// Divers IHM

	/**
	 * \return		Le type d'entité représenté.
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getEntityType ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtEntityTypeItem (const QtEntityTypeItem&);
	QtEntityTypeItem& operator = (const QtEntityTypeItem&);

	/** Le type d'entité représenté. */	
	Mgx3D::Utils::FilterEntity::objectType		_entityType;
};	// class QtEntityTypeItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'entités de type <I>Group</I>.
 */
class QtGroupTreeWidgetItem : public QTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Groupe représenté
	 */
	QtGroupTreeWidgetItem (QTreeWidgetItem* parent, Group::GroupEntity& entity);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGroupTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 */
	virtual void update (bool updateRepresentation);

	/**
	 * \return		Pointeur sur le groupe représenté, ou 0 s'il
	 *				n'y en a pas.
	 */
	virtual Group::GroupEntity* getGroup ( );
	virtual const Group::GroupEntity* getGroup ( ) const;


	protected :

	/**
	 * Pour le mode client/serveur : on n'a pas référence sur le groupe.
	 */
	QtGroupTreeWidgetItem (QTreeWidgetItem* parent);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGroupTreeWidgetItem (const QtGroupTreeWidgetItem&);
	QtGroupTreeWidgetItem& operator = (const QtGroupTreeWidgetItem&);

	/** Le groupe représenté. */
	Group::GroupEntity*					_group;
};	// class QtGroupTreeWidgetItem


/**
 * Classe de représentation dans une instance de <I>QTreeWidget</I> d'un niveau d'entités de type <I>Group</I>.
 */
class QtGroupLevelTreeWidgetItem : public QTreeWidgetItem
{
	public :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent
	 * \param	Niveau du groupe
	 * \param	Widget graphique où afficher les groupes
	 */
	QtGroupLevelTreeWidgetItem (QTreeWidgetItem* parent, int level, Qt3DGraphicalWidget* graphicalWidget);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGroupLevelTreeWidgetItem ( );

	/**
	 * Actualise les données affichées.
	 */
	virtual void update ( );

	/**
	 * Actualise la case à cocher selon l'état des groupes du niveau.
	 */
	virtual void updateState ( );
	

	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGroupLevelTreeWidgetItem (const QtGroupLevelTreeWidgetItem&);
	QtGroupLevelTreeWidgetItem& operator = (const QtGroupLevelTreeWidgetItem&);

	/** Le niveau des groupes contenus. */
	int						_level;
	
	/** L'état en cours (cache). */
	Qt::CheckState			_checkState;
	
	/** Le widget d'affichage. */
	Qt3DGraphicalWidget*	_graphicalWidget;
	
	/** Le préfixe des entrées de type "niveau". */
	static std::string		_levelEntryPrefix;
};	// class QtGroupLevelTreeWidgetItem


/**
 * \brief		Panneau de l'IHM <I>Magix 3D</I> affichant les différents groupes existants d'une session et les différents types de
 *				groupes <I>Magix 3D</I>.
 *				L'affichage est d'une part textuelle, d'autre part dans une fenêtre graphique 3D.
 *
 *				<P>Ce panneau permet également de sélectionner des groupes et de provoquer un affichage 3D filtré, selon des types
 *				de groupes sélectionnés.
 *				</P>
 */
class QtGroupsPanel : public QtEntitiesItemViewPanel, public Utils::SelectionManagerObserver
{
	Q_OBJECT

	public :

	/**
	 * Appelle createGui et createPopupMenus.
	 * \param	Widget parent.
	 * \param	Fenêtre principale de rattachement du panneau
	 * \param	Nom du panneau.
	 * \param	Contexte Magix 3D du panneau
	 * \see		createGui
	 * \see		createPopupMenus
	 */
	QtGroupsPanel (QWidget* parent, QtMgx3DMainWindow* mainWindow,
	               const std::string& name, Internal::ContextIfc& context);

	/**
	 * RAS.
	 */
	virtual ~QtGroupsPanel ( );

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

//	/**
//	 * \return		<I>true</I> si l'affichage d'entités d'un groupe doit
//	 * 				provoquer l'affichage des entités de dimension inférieures
//	 * 				du même groupe.
//	 */
//	virtual bool groupsRepresentationPropagation ( ) const;

	//@}	// Divers IHM

	virtual void setGraphicalWidget (Qt3DGraphicalWidget* widget3D);

	/**
	 * Les types d'entités filtrés.
	 */
	//@{

	/**
	 * \return		Un masque sur les types d'entités filtrés, celles visibles (case cochée).
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getCheckedEntitiesTypes ( ) const;

	/**
	 * \return      Un masque sur les types d'entités filtrés, celles sélectionnées.
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getSelectedEntitiesTypes ( ) const;

	/**
	 * \return		L'item correspondant au type d'entité transmis en argument.
	 */
	virtual QtEntityTypeItem* getEntityTypeItem (Mgx3D::Utils::FilterEntity::objectType type) const;

	//@}	// Les types d'entités filtrés.

	/**
	 * Les groupes représentés.
	 */
	//@{

	/**
	 * Ajoute les instances transmises en argument à la liste des groupes
	 * représentés.
	 * \param		Instances de groupes à ajouter
	 * \see			removeGroups
	 */
	virtual void addGroups (const std::vector<Mgx3D::Group::GroupEntity*>& groups);

	/**
	 * Enlève les instances transmises en argument de la liste des groupes
	 * représentés.
	 * \param		Instances de groupes à enlever
	 * \see			addGroups
	 */
	virtual void removeGroups (const std::vector<Mgx3D::Group::GroupEntity*>& groups);

	/**
	 * \return		Les groupes sélectionnés.
	 */
	virtual std::vector<Mgx3D::Group::GroupEntity*> getSelectedGroups ( ) const;

	//@}	// Les groupes représentés.

	/**
	 * La vue "arborescente".
	 */
	//@{


	//@}	// La vue "arborescente".

	/**
	 * La sélection.
	 */
	//@{

	/**
 	 * Actualise le panneau de manière à faciliter la sélection interactive
 	 * d'entités conformément à la politique de sélection courrante du 
 	 * gestionnaire de sélection.
 	 * \see		updateEntityItemState
 	 * \see		enableSelectionPolicyAdaptation
 	 */
	virtual void selectionPolicyModified (void* smp);

	/**
	 * Affiche ou masque les types d'entités sélectionnés.
	 */
	virtual void displaySelectedTypes (bool display);

	/**
	 * Affiche ou masque les groupes d'entités sélectionnés.
	 */
	virtual void displaySelectedGroups (bool display);

	/**
	 * Affiche ou masque les groupes d'entités dont les noms sont sélectionnés.
	 */
	virtual void displaySelectedGroupsNames (bool display);

	//@}	// La sélection.


	protected :

	/**
	 * Création de l'IHM (aspects purements Qt).
	 * \see		createPopupMenus
	 */
	virtual void createGui ( );

	/**
	 * Création des menus contextuels (Types d'entités et Groupes).
	 * \see		displayTypesPopupMenu
	 * \see		displayGroupsPopupMenu
	 */
	virtual void createPopupMenus ( );

	/**
	 * \return		L'item représentant le groupe transmise en argument.
	 */
	virtual QtGroupTreeWidgetItem* getGroupItem (const Mgx3D::Group::GroupEntity& group);

	/**
	 * \param		Item parent
	 * \param		Libellé de l'item
	 * \return		Une entrée principale dans l'arborescence des groupes.
	 */
	virtual QTreeWidgetItem* createEntryItem (QTreeWidgetItem& parent, const std::string& title);

	/**
	 * \return		Un item représentant le groupe transmis en argument.
	 */
	virtual QtGroupTreeWidgetItem* createGroupItem (Mgx3D::Group::GroupEntity&);

	/**
	 * \return		L'entrée principale dans l'arborescence des groupes dont la
	 *				dimension et le niveaux sont transmis en arguments.
	 * \exception	Une exception est levée en cas de dimension invalide.
	 * \see			createEntryItem
	 */
	virtual QTreeWidgetItem* getGroupEntryItem (unsigned char dim, unsigned long level);

	/**
	 * \return		Les items des groupes sélectionnés.
	 */
	virtual std::vector<QtGroupTreeWidgetItem*> getSelectedGroupsItems ( ) const;

	/**
	 * Appelé par <I>selectionPolicyModified</I>, factorisation de code.
	 * Actualise l'état (coché) de l'item correspondant au type d'entité
	 * transmis en premier argument selon l'état courant du gestionnaire de
	 * sélection et l'ancien état du gestion de sélection.
	 * En cas d'activation d'un item, <I>toRestore</I> reçoit le type
	 * correspondant par <I>opérateur |</I>.
	 * \see		selectionPolicyModified
	 */
	virtual void updateEntityItemState (
					Mgx3D::Utils::FilterEntity::objectType type, 
					Mgx3D::Utils::FilterEntity::objectType newState,
					Mgx3D::Utils::FilterEntity::objectType oldState,
					Mgx3D::Utils::FilterEntity::objectType& toRestore);

	/**
	 * Trier les items.
	 */
	virtual void sort ( );
	virtual void typeNameSort ( );
	virtual void typeNameSort (QTreeWidgetItem& parent);


	public :

	/**
	 * \return		<I>true</I> lorsque les modifications graphiques sur
	 *				les entités doivent être groupées si possible.
	 *				Optimisation pour le mode client/serveur ...
	 *				Retourne <I>false</I> par défaut.
	 */
	virtual bool groupEntitiesDisplayModifications ( ) const;


	public slots :

	/**
	 * Appellé lorsque l'utilisateur clique sur un item de type
	 * "type d'entités".
	 * Actualise la représentation des groupes dans la fenêtre
	 * graphique 3D.
	 * \see		groupStateChangeCallback
	 */
	virtual void entitiesTypesStateChangeCallback (QTreeWidgetItem*, int);


	protected slots :

	/**
	 * Appellé lorsque l'utilisateur clique sur un item de type groupe.
	 * Affiche/Masque la représentation des entités du groupe dans la fenêtre
	 * graphique 3D.
	 * \see		entitiesTypesStateChangeCallback
	 */
	virtual void groupStateChangeCallback (QTreeWidgetItem*, int);

	/**
	 * Affiche les types d'entités sélectionnés.
	 */
	virtual void displaySelectedTypesCallback ( );

	/**
	 * Masque les types d'entités sélectionnés.
	 */
	virtual void hideSelectedTypesCallback ( );

	/**
	 * Affiche les groupes d'entités sélectionnés.
	 */
	virtual void displaySelectedGroupsCallback ( );

	/**
	 * Affiche les groupes d'entités dont les noms sont sélectionnés (toutes dimensions).
	 */
	virtual void displaySelectedGroupsNamesCallback ( );

	/**
	 * Masque les groupes d'entités sélectionnés.
	 */
	virtual void hideSelectedGroupsCallback ( );

	/**
	 * Masque les groupes d'entités dont les noms sont sélectionnés (toutes dimensions).
	 */
	virtual void hideSelectedGroupsNamesCallback ( );

	/**
	 * Tri automatiquement les groupes d'entités.
	 */
	virtual void automaticSortCallback (bool automatic);

	/**
	 * Tri les groupes d'entités par ordre alphabétique ascendant.
	 */
	virtual void ascendingSortCallback (bool ascending);

	/**
	 * Affichage multi-niveaux des groupes.
	 */
	virtual void multiLevelViewCallback (bool enabled);
	
	/**
	 * Affichage/masquage du niveau (dé)coché.
	 */
	virtual void showLevelGroupsCallback (QTreeWidgetItem*, int);
	
	/**
	 * Change de niveau les groupes sélectionnés.
	 */
	virtual void changeGroupLevelCallback ( );

	/**
	 * Permet de sélectionner les groupes d'un niveau renseigné par
	 * l'utilisateur.
	 */
	virtual void levelSelectionCallback ( );

	/**
	 * Appelé pour modifier les paramètres d'affichage
	 * (points/filaire/surfacique) des entités de types sélectionnés.
	 * Affiche une boite de dialogue de choix des types de représentation à
	 * utiliser, initialisé selon les entités sélectionnées dans l'arborescence
	 * de types d'entités, et effectue les modifications demandées.
	 * \see		groupsRepresentationTypesCallback ( );
	 */
	virtual void typesRepresentationTypesCallback ( );

	/**
	 * Appelé pour modifier les paramètres d'affichage
	 * (points/filaire/surfacique) des entités des groupes sélectionnés.
	 * Affiche une boite de dialogue de choix des types de représentation à
	 * utiliser, initialisé selon les entités sélectionnées dans l'arborescence
	 * de groupes d'entités, et effectue les modifications demandées.
	 * \see		typesRepresentationTypesCallback ( );
	 */
	virtual void groupsRepresentationTypesCallback ( );

	/**
	 * Désélectionne tout
	 */
	virtual void unselectCallback ( );

	/**
	 * Sélectionne les entités visibles pour les types sélectionnés
	 */
	virtual void selectVisibleCallback ( );

	/**
	 * Sélectionne toutes les entités pour les types sélectionnés
	 */
	virtual void selectAllCallback ( );


	/**
	 * Sélectionne les volumes géométriques correspondants aux groupes sélectionnés
	 */
	virtual void selectGeomVolumesCallback ( );

	/**
	 * Sélectionne les surfaces géométriques correspondants aux groupes sélectionnés
	 */
	virtual void selectGeomSurfacesCallback ( );

	/**
	 * Sélectionne les courbes géométriques correspondants aux groupes sélectionnés
	 */
	virtual void selectGeomCurvesCallback ( );

	/**
	 * Sélectionne les sommets géométriques correspondants aux groupes sélectionnés
	 */
	virtual void selectGeomVerticesCallback ( );

	/**
	 * Sélectionne les blocs topologiques correspondants aux groupes sélectionnés
	 */
	virtual void selectTopoBlocksCallback ( );

	/**
	 * Sélectionne les faces communes topologiques correspondants aux groupes sélectionnés
	 */
	virtual void selectTopoCoFacesCallback ( );

	/**
	 * Sélectionne les arêtes communes topologiques correspondants aux groupes sélectionnés
	 */
	virtual void selectTopoCoEdgesCallback ( );

	/**
	 * Sélectionne les sommets topologiques correspondants aux groupes sélectionnés
	 */
	virtual void selectTopoVerticesCallback ( );

	/**
	 * Sélectionne les volumes de mailles correspondants aux groupes sélectionnés
	 */
	virtual void selectMeshVolumesCallback ( );

	/**
	 * Sélectionne les surfaces de mailles correspondants aux groupes sélectionnés
	 */
	virtual void selectMeshSurfacesCallback ( );

	/**
	 * Sélectionne les lignes de mailles correspondants aux groupes sélectionnés
	 */
	virtual void selectMeshLinesCallback ( );

	/**
	 * Sélectionne les nuages de mailles correspondants aux groupes sélectionnés
	 */
	virtual void selectMeshCloudsCallback ( );

	/**
	 * Désélectionne les volumes géométriques correspondants aux groupes sélectionnés
	 */
	virtual void unselectGeomVolumesCallback ( );

	/**
	 * Désélectionne les surfaces géométriques correspondants aux groupes sélectionnés
	 */
	virtual void unselectGeomSurfacesCallback ( );

	/**
	 * Désélectionne les courbes géométriques correspondants aux groupes sélectionnés
	 */
	virtual void unselectGeomCurvesCallback ( );

	/**
	 * Désélectionne les sommets géométriques correspondants aux groupes sélectionnés
	 */
	virtual void unselectGeomVerticesCallback ( );

	/**
	 * Désélectionne les blocs topologiques correspondants aux groupes sélectionnés
	 */
	virtual void unselectTopoBlocksCallback ( );

	/**
	 * Désélectionne les faces communes topologiques correspondants aux groupes sélectionnés
	 */
	virtual void unselectTopoCoFacesCallback ( );

	/**
	 * Désélectionne les arêtes communes topologiques correspondants aux groupes sélectionnés
	 */
	virtual void unselectTopoCoEdgesCallback ( );

	/**
	 * Désélectionne les sommets topologiques correspondants aux groupes sélectionnés
	 */
	virtual void unselectTopoVerticesCallback ( );

	/**
	 * Désélectionne les volumes de mailles correspondants aux groupes sélectionnés
	 */
	virtual void unselectMeshVolumesCallback ( );

	/**
	 * Désélectionne les surfaces de mailles correspondants aux groupes sélectionnés
	 */
	virtual void unselectMeshSurfacesCallback ( );

	/**
	 * Désélectionne les lignes de mailles correspondants aux groupes sélectionnés
	 */
	virtual void unselectMeshLinesCallback ( );

	/**
	 * Désélectionne les nuages de mailles correspondants aux groupes sélectionnés
	 */
	virtual void unselectMeshCloudsCallback ( );

	/**
	 * Destruction d'un groupe, on lui retire toute les entités qu'il contient
	 */
	virtual void clearGroupCallback ( );


	/**
	 * Appelé lorsque le menu popup est demandé au niveau de l'arborescence des
	 * types d'entités à afficher. Affiche ce menu popup à la position transmise
	 * en argument.
	 */
	virtual void displayTypesPopupMenu (const QPoint&);

	/**
	 * Appelé lorsque le menu popup est demandé au niveau de l'arborescence des
	 * groupes d'entités. Affiche ce menu popup à la position transmise
	 * en argument.
	 */
	virtual void displayGroupsPopupMenu (const QPoint&);


	protected :

	// Opérations interdites :
	QtGroupsPanel (const QtGroupsPanel&);


	private :

	QtGroupsPanel& operator = (const QtGroupsPanel&);

	/** Le widget représentant les types d'entités. */
	QTreeWidget*							_entitiesTypesWidget;

	/** Le widget représentant les groupes d'entités. */
	QTreeWidget*							_entitiesGroupsWidget;

	/** Les types d'entités filtrés. */
	std::vector<QtEntityTypeItem*>			_typesItems;

	/** Les entrées principales pour chaque dimensions. */
	std::vector<QTreeWidgetItem*>			_groupsEntriesItems;

	/** Les entrées principales pour chaque niveau. */
	std::vector<std::map <unsigned long, QTreeWidgetItem*> >	_levelsEntriesItems;

	/** Le menu contextuel de l'arborescence "Types d'entités". */
	QMenu*									_typesPopupMenu;

	/** Le menu contextuel de l'arborescence "Groupes". */
	QMenu*									_groupsPopupMenu;

	/** Cases à cocher à décocher au prochain appel de <I>selectionPolicyModified</I>. cf. selectionPolicyModified. */
	Mgx3D::Utils::FilterEntity::objectType	_uncheckedCheckboxes;
};	// class QtGroupsPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GROUPS_PANEL_H
