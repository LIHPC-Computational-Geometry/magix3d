/**
 * \file        QtGroupsPanel.cpp
 * \author      Charles PIGNEROL
 * \date        12/11/2012
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/SelectionManagerDimFilter.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Mesh/Surface.h"
#include "Mesh/Volume.h"
#include "Mesh/Line.h"
#include "Mesh/Cloud.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Mesh/MeshEntity.h"
#include "QtComponents/QtGroupsPanel.h"
#include "QtComponents/QtGroupsSelectorDialog.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtExpansionTreeRestorer.h"
#include "Group/GroupManagerIfc.h"
#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ThreeStates.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QComboBox>
#include <QInputDialog>
#include <QSplitter>
#include <QThread>
#include <QString>

#include <assert.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


static QAction* createAction (
			QMenu& parent, const QString& label, const QString& whatsThis,
			const char* method, QObject* receiver,
			bool checkable, THREESTATES state)
{
	QAction*	action	= new QAction (label, &parent);
	action->setWhatsThis (whatsThis);
	if (true == checkable)
	{
		action->setCheckable (true);
		switch (state)
		{
			case ON		: action->setChecked (true);	break;
			// Pb : Qt 4.6.2 ne prévoit plus les checkbox dans les menus avec
			// le setTristate (true) et le setCheckState (Qt::PartiallyChecked).
			case OFF	:
			default		: action->setChecked (false);
		}	// switch (state)
	}	// if (true == checkable)
	if ((0 != method) && (0 != receiver))
		QObject::connect (action, SIGNAL (triggered ( )), receiver, method);
	parent.addAction (action);

	return action;
}	// createAction


static QAction* createCheckableAction (
			QMenu& parent, const QString& label, const QString& whatsThis,
			const char* method, QObject* receiver,
			bool checked, THREESTATES state)
{
	QAction*	action	= new QAction (label, &parent);
	action->setWhatsThis (whatsThis);
	action->setCheckable (true);
	action->setChecked (checked);
	if ((0 != method) && (0 != receiver))
		QObject::connect (action, SIGNAL (triggered (bool)), receiver, method);
	parent.addAction (action);

	return action;
}	// createAction


// ===========================================================================
//                      LA CLASSE QtEntityTypeItem
// ===========================================================================

QtEntityTypeItem::QtEntityTypeItem (QTreeWidgetItem* parent, FilterEntity::objectType type)
	: QTreeWidgetItem (parent), _entityType (type)
{
	setText (0, FilterEntity::filterToObjectTypeName (type).c_str ( ));
	setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
	          Qt::ItemIsUserCheckable);
}	// QtEntityTypeItem::QtEntityTypeItem


QtEntityTypeItem::QtEntityTypeItem (const QtEntityTypeItem&)
	: QTreeWidgetItem (0), _entityType ((FilterEntity::objectType)0)
{
	MGX_FORBIDDEN ("QtEntityTypeItem copy constructor is not allowed.");
}	// QtEntityTypeItem::QtEntityTypeItem


QtEntityTypeItem& QtEntityTypeItem::operator = (const QtEntityTypeItem&)
{
	MGX_FORBIDDEN ("QtEntityTypeItem assignment operator is not allowed.");
	return *this;
}	// QtEntityTypeItem::operator =


QtEntityTypeItem::~QtEntityTypeItem ( )
{
}	// QtEntityTypeItem::~QtEntityTypeItem


void QtEntityTypeItem::writeSettings (QSettings& settings)
{
	settings.beginGroup (
		FilterEntity::filterToFullObjectTypeName (getEntityType ( )).c_str ( ));
	settings.setValue ("checked", Qt::Checked == checkState (0) ? true : false);
	settings.endGroup ( );
}	// QtEntityTypeItem::writeSettings


void QtEntityTypeItem::readSettings (QSettings& settings)
{
	settings.beginGroup (
		FilterEntity::filterToFullObjectTypeName (getEntityType ( )).c_str ( ));
	setCheckState (0, true == settings.value ("checked").toBool ( ) ?
	                  Qt::Checked : Qt::Unchecked);
	settings.endGroup ( );
}	// QtEntityTypeItem::readSettings


FilterEntity::objectType QtEntityTypeItem::getEntityType ( ) const
{
	return _entityType;
}	// QtEntityTypeItem::getEntityType



// ===========================================================================
//                      LA CLASSE QtGroupTreeWidgetItem
// ===========================================================================

QtGroupTreeWidgetItem::QtGroupTreeWidgetItem (QTreeWidgetItem* parent, GroupEntity& group)
	: QTreeWidgetItem (parent), _group (&group)
{
	setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	update (false);
}	// QtGroupTreeWidgetItem::QtGroupTreeWidgetItem


QtGroupTreeWidgetItem::QtGroupTreeWidgetItem (QTreeWidgetItem* parent)
	: QTreeWidgetItem (parent), _group (0)
{	// Pour mode client/serveur
	setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
	          Qt::ItemIsUserCheckable);
	update (false);
}	// QtGroupTreeWidgetItem::QtGroupTreeWidgetItem


QtGroupTreeWidgetItem::QtGroupTreeWidgetItem (const QtGroupTreeWidgetItem&)
	: QTreeWidgetItem (0), _group (0)
{
	MGX_FORBIDDEN ("QtGroupTreeWidgetItem copy constructor is not allowed.");
}	// QtGroupTreeWidgetItem::QtGroupTreeWidgetItem


QtGroupTreeWidgetItem& QtGroupTreeWidgetItem::operator = (const QtGroupTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtGroupTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtGroupTreeWidgetItem::operator =


QtGroupTreeWidgetItem::~QtGroupTreeWidgetItem ( )
{
}	// QtGroupTreeWidgetItem::~QtGroupTreeWidgetItem


GroupEntity* QtGroupTreeWidgetItem::getGroup ( )
{
	return _group;
}	// QtGroupTreeWidgetItem::getGroup


const GroupEntity* QtGroupTreeWidgetItem::getGroup ( ) const
{
	return _group;
}	// QtGroupTreeWidgetItem::getGroup


void QtGroupTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	setText (0, 0 == getGroup ( ) ? "" : getGroup ( )->getName ( ).c_str ( ));

	// La vue 3D :
/*
	if ((0 != getGroup ( )) &&
	    (true == getGroup ( )->getDisplayProperties ( ).isDisplayed ( )))
	{
		QtEntitiesTreeWidget*			parent	=
				dynamic_cast<QtEntitiesTreeWidget*>(treeWidget ( ));
		QtGroupsPanel*		panel	=
							0 == parent ? 0 : parent->getEntitiesPanel ( );
		Qt3DGraphicalWidget*	graphicalWidget	=
							0 == panel ? 0 : panel->getGraphicalWidget ( );
		GraphicalGroupRepresentation*	entity3DRep	=
							0 == graphicalWidget ?
				0 : graphicalWidget->getRenderingManager ( ).getRepresentation (
																*getGroup ( ));
		if (0 != entity3DRep)
		{
			entity3DRep->updateRepresentation (mask, true);
			if ((0 != graphicalWidget) && (true == updateRepresentation))
				graphicalWidget->getRenderingManager ( ).forceRender ( );
		}	// if (0 != entity3DRep)
	}	// if (0 != getGroup ( ))
*/

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtGroupTreeWidgetItem::update


// ===========================================================================
//                    LA CLASSE QtGroupLevelTreeWidgetItem
// ===========================================================================

string	QtGroupLevelTreeWidgetItem::_levelEntryPrefix ("Niveau_");


QtGroupLevelTreeWidgetItem::QtGroupLevelTreeWidgetItem (QTreeWidgetItem* parent, int level, Qt3DGraphicalWidget* graphicalWidget)
	: QTreeWidgetItem (parent), _level (level), _checkState (Qt::Unchecked), _graphicalWidget (graphicalWidget)
{
	setFlags (Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	UTF8String	levelName (Charset::UTF_8);
	levelName << _levelEntryPrefix << (unsigned long)level;
	setCheckState (0, Qt::Unchecked);
	setText (0, UTF8TOQSTRING (levelName));
	setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
}	// QtGroupLevelTreeWidgetItem::QtGroupLevelTreeWidgetItem


QtGroupLevelTreeWidgetItem::QtGroupLevelTreeWidgetItem (const QtGroupLevelTreeWidgetItem&)
	: QTreeWidgetItem (0), _level (0), _checkState (Qt::Unchecked), _graphicalWidget (0)
{
	MGX_FORBIDDEN ("QtGroupLevelTreeWidgetItem copy constructor is not allowed.");
}	// QtGroupLevelTreeWidgetItem::QtGroupLevelTreeWidgetItem


QtGroupLevelTreeWidgetItem& QtGroupLevelTreeWidgetItem::operator = (const QtGroupLevelTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtGroupLevelTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtGroupLevelTreeWidgetItem::operator =


QtGroupLevelTreeWidgetItem::~QtGroupLevelTreeWidgetItem ( )
{
}	// QtGroupLevelTreeWidgetItem::~QtGroupLevelTreeWidgetItem


void QtGroupLevelTreeWidgetItem::update ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (checkState (0) == _checkState)
		return;
	
	_checkState	= checkState (0);
	const int				count	= childCount ( );
	vector<GroupEntity*>	groups;
	for (int i = 0; i < count; i++)
	{
		QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(child (i));
		if ((0 != groupItem) && (groupItem->checkState (0) != _checkState))
		{
			groupItem->setCheckState (0, _checkState);
			groups.push_back (groupItem->getGroup ( ));
		}	// if ((0 != groupItem) && (groupItem->checkState (0) != _checkState))
	}	// for (int i = 0; i < count; i++)
	
	if ((0 != groups.size ( )) && (0 != _graphicalWidget))
		_graphicalWidget->getRenderingManager ( ).displayRepresentations (groups, Qt::Checked == _checkState ? true : false);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtGroupLevelTreeWidgetItem::update


void QtGroupLevelTreeWidgetItem::updateState ( )
{
	const int		count		= childCount ( );
	bool			first		= true;
	Qt::CheckState	checkState	= Qt::Unchecked;
	for (int i = 0; i < count; i++)
	{
		QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(child (i));
		if (0 == groupItem)
			continue;
			
		if (true == first)
		{
			checkState	= groupItem->checkState (0);
			first	= false;
		}	// if (true == first)
		checkState	= groupItem->checkState (0) != checkState ? Qt::PartiallyChecked : checkState;
		if (Qt::PartiallyChecked == checkState)
			break;
	}	// for (int i = 0; i < count; i++)

	if (checkState != _checkState)
	{
		_checkState	= checkState;
		setCheckState (0, _checkState);
	}	// if (checkState != _checkState)
}	// QtGroupLevelTreeWidgetItem::updateState


// ===========================================================================
//                        LA CLASSE QtGroupsPanel
// ===========================================================================


QtGroupsPanel::QtGroupsPanel (
	QWidget* parent, QtMgx3DMainWindow* mainWindow, const string& name,
	ContextIfc& context)
	: QtEntitiesItemViewPanel (parent, mainWindow, name, context),
	  SelectionManagerObserver (&context.getSelectionManager ( )),
	  _entitiesTypesWidget (0), _entitiesGroupsWidget (0),
	  _typesItems ( ), _groupsEntriesItems ( ), _levelsEntriesItems ( ),
	  //_groupsPropagationCheckBox (0),
	  _typesPopupMenu (0), _groupsPopupMenu (0),
	  _uncheckedCheckboxes (FilterEntity::NoneEntity)
{
	createGui ( );
	createPopupMenus ( );

	// Par défaut le fait de démarrer une sélection interactive provoque
	// l'affichage des entités des types sélectionnables des groupes affichés.
	enableSelectionPolicyAdaptation (true);

	context.getSelectionManager ( ).addSelectionObserver (*this);
}	// QtGroupsPanel::QtGroupsPanel


QtGroupsPanel::QtGroupsPanel (const QtGroupsPanel& gp)
	: QtEntitiesItemViewPanel (gp), SelectionManagerObserver (0),
	  _entitiesTypesWidget (0), _entitiesGroupsWidget (0),
	  _typesItems ( ), _groupsEntriesItems ( ), _levelsEntriesItems ( ),
	  //_groupsPropagationCheckBox (0),
	  _typesPopupMenu (0), _groupsPopupMenu (0),
	  _uncheckedCheckboxes (FilterEntity::NoneEntity)
{
	MGX_FORBIDDEN ("QtGroupsPanel copy constructor is not allowed.");
}	// QtGroupsPanel::QtGroupsPanel (const QtGroupsPanel&)


QtGroupsPanel& QtGroupsPanel::operator = (const QtGroupsPanel&)
{
	MGX_FORBIDDEN ("QtGroupsPanel assignment operator is not allowed.");
	return *this;
}	// QtGroupsPanel::QtGroupsPanel (const QtGroupsPanel&)


QtGroupsPanel::~QtGroupsPanel ( )
{
}	// QtGroupsPanel::~QtGroupsPanel


void QtGroupsPanel::writeSettings (QSettings& settings)
{
	//CHECK_NULL_PTR_ERROR (_groupsPropagationCheckBox)

	settings.beginGroup ("GroupsPanel");

	settings.setValue ("size", size ( ));

	for (vector<QtEntityTypeItem*>::const_iterator iteti = _typesItems.begin( );
	     _typesItems.end ( ) != iteti; iteti++)
		(*iteti)->writeSettings (settings);
	//settings.setValue ("groupsPropagation", Qt::Checked == _groupsPropagationCheckBox->checkState ( ) ? true : false);

	CHECK_NULL_PTR_ERROR (_entitiesTypesWidget)
	CHECK_NULL_PTR_ERROR (_entitiesGroupsWidget)
	settings.beginGroup ("EntitiesTypesPanel");
	settings.setValue ("size", _entitiesTypesWidget->size ( ));
	settings.endGroup ( );
	settings.beginGroup ("EntitiesGroupsPanel");
	settings.setValue ("size", _entitiesGroupsWidget->size ( ));
	settings.endGroup ( );

	settings.endGroup ( );
}	// QtGroupsPanel::writeSettings


void QtGroupsPanel::readSettings (QSettings& settings)
{
	//CHECK_NULL_PTR_ERROR (_groupsPropagationCheckBox)

	settings.beginGroup ("GroupsPanel");

	resize (settings.value ("size", size ( )).toSize ( ));

	for (vector<QtEntityTypeItem*>::const_iterator iteti = _typesItems.begin( );
	     _typesItems.end ( ) != iteti; iteti++)
	{
		const Qt::CheckState	oldState	= (*iteti)->checkState (0);
		(*iteti)->readSettings (settings);
		if (oldState != (*iteti)->checkState (0))
			entitiesTypesStateChangeCallback (*iteti, 0);
	}	// for (vector<QtEntityTypeItem*>::const_iterator iteti = ...
//	const Qt::CheckState	state	=
//			true == settings.value ("groupsPropagation", true).toBool ( ) ?
//			Qt::Checked : Qt::Unchecked;
	//_groupsPropagationCheckBox->setCheckState (state);

	CHECK_NULL_PTR_ERROR (_entitiesTypesWidget)
	CHECK_NULL_PTR_ERROR (_entitiesGroupsWidget)
	settings.beginGroup ("EntitiesTypesPanel");
	_entitiesTypesWidget->resize (
			settings.value ("size", _entitiesTypesWidget->size ( )).toSize ( ));
	settings.endGroup ( );
	settings.beginGroup ("EntitiesGroupsPanel");
	_entitiesGroupsWidget->resize (
			settings.value ("size", _entitiesGroupsWidget->size ( )).toSize( ));
	settings.endGroup ( );

	settings.endGroup ( );

	//groupsRepresentationPropagationCallback ( );
}	// QtGroupsPanel::readSettings


//bool QtGroupsPanel::groupsRepresentationPropagation ( ) const
//{
//	CHECK_NULL_PTR_ERROR (_groupsPropagationCheckBox)
//	return Qt::Checked == _groupsPropagationCheckBox->checkState ( ) ?
//	       true : false;
//}	// QtGroupsPanel::groupsRepresentationPropagation


void QtGroupsPanel::setGraphicalWidget (Qt3DGraphicalWidget* widget3D)
{
	QtEntitiesItemViewPanel::setGraphicalWidget (widget3D);
	entitiesTypesStateChangeCallback (0, 0);
}	// QtGroupsPanel::setGraphicalWidget


FilterEntity::objectType QtGroupsPanel::getCheckedEntitiesTypes ( ) const
{
	FilterEntity::objectType	types	= FilterEntity::NoneEntity;

	for (vector<QtEntityTypeItem*>::const_iterator it = _typesItems.begin ( );
	     _typesItems.end ( ) != it; it++)
		if (Qt::Checked == (*it)->checkState (0))
			types = (FilterEntity::objectType)(types | (*it)->getEntityType( ));

	return types;
}	// QtGroupsPanel::getCheckedEntitiesTypes


FilterEntity::objectType QtGroupsPanel::getSelectedEntitiesTypes ( ) const
{
    FilterEntity::objectType    types   = FilterEntity::NoneEntity;

    for (vector<QtEntityTypeItem*>::const_iterator it = _typesItems.begin ( );
         _typesItems.end ( ) != it; it++)
        if ((*it)->isSelected())
            types = (FilterEntity::objectType)(types | (*it)->getEntityType( ));

    return types;
}   // QtGroupsPanel::getSelectedEntitiesTypes


QtEntityTypeItem* QtGroupsPanel::getEntityTypeItem (FilterEntity::objectType type) const
{
	for (vector<QtEntityTypeItem*>::const_iterator it = _typesItems.begin ( );
	     _typesItems.end ( ) != it; it++)
		if (type == (*it)->getEntityType ( ))
			return *it;

	UTF8String	error (Charset::UTF_8);
	error << "Absence d'item correspondant au type "
	      << FilterEntity::filterToObjectTypeName (type)
	      << " dans l'arbre Types d'entités.";

	INTERNAL_ERROR (exc, error, "QtGroupsPanel::getEntityTypeItem")
	throw exc;
}	// QtGroupsPanel::getEntityTypeItem


void QtGroupsPanel::addGroups (const vector<Group::GroupEntity*>& groups)
{
	for (vector<Group::GroupEntity*>::const_iterator it = groups.begin ( );
	     groups.end ( ) != it; it++)
	{
		QTreeWidgetItem*	item	= createGroupItem (**it);
		item->setCheckState (0, (**it).isVisible()?Qt::Checked:Qt::Unchecked);
	}	// for (vector<Group::GroupEntity*>::const_iterator it = groups.begin ( ); ...

	sort ( );
}	// QtGroupsPanel::addGroups


void QtGroupsPanel::removeGroups (const vector<Group::GroupEntity*>& groups)
{
	vector<Utils::Entity*>	list;

	for (vector<Group::GroupEntity*>::const_iterator it = groups.begin ( );
	     groups.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	parent	= getGroupEntryItem ((*it)->getDim ( ), (*it)->getLevel ( ));
		CHECK_NULL_PTR_ERROR (parent)
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QtGroupTreeWidgetItem*	item	= dynamic_cast<QtGroupTreeWidgetItem*>(parent->child (i));
			if ((0 != item) && (*it == item->getGroup ( )))
			{
				parent->takeChild (i);
				delete item;
				break;
			}	// if ((0 != item) && (&entity == item->getGroup ( )))
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
		QtGroupLevelTreeWidgetItem*	levelItem	= dynamic_cast<QtGroupLevelTreeWidgetItem*>(parent);
		if ((0 != levelItem) && (Qt::PartiallyChecked == levelItem->checkState (0)))
			levelItem->updateState ( );
	}	// for (vector<Group::GroupEntity*>::const_iterator it = groups.begin ( );

	// sort a priori inutile, ordre conservé.
}	// QtGroupsPanel::removeGroups


vector<Group::GroupEntity*> QtGroupsPanel::getSelectedGroups ( ) const
{
	vector<Group::GroupEntity*>	groups;
	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );

	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( ); items.end ( ) != it; it++)
		if (true == (*it)->isSelected ( ))
			groups.push_back ((*it)->getGroup ( ));

	return groups;
}	// QtGroupsPanel::getSelectedGroups


void QtGroupsPanel::displaySelectedTypes (bool display)
{
	// MAJ IHM (pas de callback de déclenché) :
	QtEntityTypeItem*	item	= 0;
	for (vector<QtEntityTypeItem*>::iterator it = _typesItems.begin ( ); _typesItems.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			(*it)->setCheckState (0, true == display ? Qt::Checked : Qt::Unchecked);
			item	= *it;
		}
	}	// for (vector<QtEntityTypeItem*>::iterator it = _typesItems.begin ( )

	// Traitement :
	if (0 != item)
		entitiesTypesStateChangeCallback (item, 0);
}	// QtGroupsPanel::displaySelectedTypes


void QtGroupsPanel::displaySelectedGroups (bool display)
{
	// MAJ IHM (pas de callback de déclenché) + réalisation traitement :
	vector<GroupEntity*>			groups;
	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
//		if (true == (*it)->isSelected ( )) [EB car getSelectedGroupsItems a déjà fait une sélection]
//		{
			(*it)->setCheckState (0, true == display ? Qt::Checked : Qt::Unchecked);
			groups.push_back ((*it)->getGroup ( ));
//		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...
	if ((0 != groups.size ( )) && (0 != getGraphicalWidget ( )))
		getGraphicalWidget ( )->getRenderingManager ( ).displayRepresentations (groups, display);
}	// QtGroupsPanel::displaySelectedGroups


void QtGroupsPanel::displaySelectedGroupsNames (bool display)
{
	// MAJ IHM (pas de callback de déclenché) + réalisation traitement :
	vector<GroupEntity*>			groups;
	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			(*it)->setCheckState (0, true == display ? Qt::Checked : Qt::Unchecked);

			// TODO améliorer displaySelectedGroupsNames

			/* EB: il serait mieux de sélectionner les items
			 * Actuellement [26/3/2018] j'arrive juste à rendre visible/insible les entités dans la vue graphique
			 * mais findItems ne me retourne pas la liste des items avec le même nom
			 */


//			std::cout<<" it text(0) : "<<(*it)->text(0).toStdString()<<std::endl;
//
//			groups.push_back ((*it)->getGroup ( ));
//
//			QString nom((*it)->getGroup ( )->getName().c_str());
//
//			QList<QTreeWidgetItem*>	sameNamesItems	=
//					_entitiesGroupsWidget->findItems ((*it)->text(0), Qt::MatchFixedString | Qt::MatchCaseSensitive, 1);
//
//			std::cout<<"sameNamesItems.size() = "<<sameNamesItems.size()<<std::endl;
//
//			for (QList<QTreeWidgetItem*>::iterator it2 = sameNamesItems.begin ( );
//					sameNamesItems.end ( ) != it2; it2++){
//				if (false == (*it2)->isSelected ( )){
//					(*it2)->setCheckState(0, true == display ? Qt::Checked : Qt::Unchecked);
//
//					QtGroupTreeWidgetItem*	groupItem	=
//								dynamic_cast<QtGroupTreeWidgetItem*>(*it2);
//					if (0 != groupItem)
//						groups.push_back (groupItem->getGroup ( ));
//				}
//			}

			std::string nom = (*it)->getGroup ( )->getName();

			GroupEntity* gr0 = getStdContext()->getGroupManager().getGroup0D(nom, false);
			if (gr0)
				groups.push_back (gr0);

			GroupEntity* gr1 = getStdContext()->getGroupManager().getGroup1D(nom, false);
			if (gr1)
				groups.push_back (gr1);

			GroupEntity* gr2 = getStdContext()->getGroupManager().getGroup2D(nom, false);
			if (gr2)
				groups.push_back (gr2);

			GroupEntity* gr3 = getStdContext()->getGroupManager().getGroup3D(nom, false);
			if (gr3)
				groups.push_back (gr3);

		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...
	if ((0 != groups.size ( )) && (0 != getGraphicalWidget ( )))
		getGraphicalWidget ( )->getRenderingManager (
							).displayRepresentations (groups, display);
}	// QtGroupsPanel::displaySelectedGroupsNames


void QtGroupsPanel::clearGroupCallback ()
{
	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			GroupEntity* gr = (*it)->getGroup ( );
			getContext().getGroupManager().clearGroup(gr->getDim(), gr->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...
}	// QtGroupsPanel::displaySelectedGroups



void QtGroupsPanel::selectionPolicyModified (void* smp)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesTypesWidget)

	if (false == isSelectionPolicyAdaptationEnabled ( ))
		return;

	// Réunion CP/EBL du 14/10/13.
	// Appelé lorsque la politique de sélection change, par exemple lorsque
	// l'utilisateur clique sur un champ de texte de type QtEntityIDTextField
	// pour saisir de manière interactive l'id d'une entité.
	// L'objectif est ici de provoquer l'affichage d'entités selectionnables en
	// cochant dans l'arbre "Types d'entités" les cases à cocher
	// correspondantes. Les entités correspondantes ne seront visibles dans la
	// fenêtre graphique qu'à condition que leur groupe ne soit lui même coché
	// dans l'arbre "Groupes".
	// Dans cet appel on :
	// - Annule l'effet de l'appel précédent
	// - Actualise l'IHM à l'état courant du gestionnaire de sélection.
	SelectionManagerDimFilter*	smdv	=
			dynamic_cast<SelectionManagerDimFilter*>(getSelectionManager ( ));
	FilterEntity::objectType		toRestore	= FilterEntity::NoneEntity;
	if (0 != smdv)
	{
		FilterEntity::objectType	types	= smdv->getFilteredTypes ( );

		for (vector<QtEntityTypeItem*>::iterator it = _typesItems.begin ( );
		     _typesItems.end ( ) != it; it++)
			updateEntityItemState (
				(*it)->getEntityType ( ), types,_uncheckedCheckboxes,toRestore);

		_uncheckedCheckboxes	= toRestore;
	}	// if (0 != smdv)

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::selectionPolicyModified


void QtGroupsPanel::updateEntityItemState (
		FilterEntity::objectType type, FilterEntity::objectType newState,
		FilterEntity::objectType oldState, FilterEntity::objectType& toRestore)
{
	QtEntityTypeItem*	item	= getEntityTypeItem (type);
	CHECK_NULL_PTR_ERROR (item)

	// Rem : FilterEntity::All n'est pas issu d'un | sur des valeurs mais de
	// (None) - 1 ... Pas très beau mais convient.
	if ((FilterEntity::All != newState) && (0 != (type & newState)))
	{
		if (Qt::Checked != item->checkState (0))
		{
			toRestore	= (FilterEntity::objectType)(toRestore | type);
			item->setCheckState (0, Qt::Checked);
			entitiesTypesStateChangeCallback (item, 0);
		}	// if (Qt::Checked != item->checkState (0))
	}
	else	// Le décocher ???
	{	
		if ((Qt::Unchecked != item->checkState (0)) && (0 != (type & oldState)))
		{
			item->setCheckState (0, Qt::Unchecked);
			entitiesTypesStateChangeCallback (item, 0);
		}
	}	// else if (0 != (type & newState))
}	// QtGroupsPanel::updateEntityItemState


void QtGroupsPanel::sort ( )
{
	if (false == Resources::instance ( )._automaticSort.getValue ( ))
		return;

	typeNameSort ( );
}	// QtGroupsPanel::sort


void QtGroupsPanel::typeNameSort ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesGroupsWidget)

	for (vector<QTreeWidgetItem*>::iterator itgi = _groupsEntriesItems.begin( );
	     _groupsEntriesItems.end ( ) != itgi; itgi++)
		typeNameSort (**itgi);

	if (true == Resources::instance ( )._multiLevelGroupsView.getValue ( ))
	{
		for (int i = 0; i < 4; i++)
		{
			map <unsigned long, QTreeWidgetItem*>&	entries	= _levelsEntriesItems [i];
			for (map <unsigned long, QTreeWidgetItem*>::iterator ite	= entries.begin ( ); entries.end ( ) != ite; ite++)
				typeNameSort (*(*ite).second);
		}	// for (int i = 0; i < 4; i++)
	}	// if (true == Resources::instance ( )._multiLevelGroupsView ...


	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, getAppTitle ( ))
}	// QtGroupsPanel::typeNameSort


void QtGroupsPanel::typeNameSort (QTreeWidgetItem& parent)
{
	QTreeWidgetItem*			item	= 0;
	vector<QTreeWidgetItem*>	items;
	QtExpansionTreeRestorer	expansionTreeRestorer (parent);

	while (0 != parent.childCount ( ))
	{
		item	= parent.takeChild (0);
		items.push_back (item);
	}	// while (0 != parent.childCount ( ))
	if (0 != Resources::instance ( )._sortType.getValue ( ).ascii ( ).compare ("typeAscendingName"))
		std::sort (&items.front ( ), &(items [items.size ( )]),
							TypeNameSortGreater<QTreeWidgetItem*> ( ));
	else
		std::sort (&items.front ( ), &(items [items.size ( )]),
							TypeNameSortLesser<QTreeWidgetItem*> ( ));

	for (vector<QTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
		parent.addChild (*it);

	expansionTreeRestorer.restore ( );
}	// QtGroupsPanel::typeNameSort


void QtGroupsPanel::createGui ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if ((0 != _entitiesTypesWidget) || (0 != _entitiesGroupsWidget))
	{
		INTERNAL_ERROR (exc,
				"QtGroupsPanel::createGui", "IHM déjà initialisée.")
		throw exc;
	}	// if ((0 != _entitiesTypesWidget) || ...
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSpacing (QtConfiguration::spacing);
	setLayout (layout);
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	QSplitter*	splitter	= new QSplitter (Qt::Vertical, this);
	layout->addWidget (splitter);

	_entitiesTypesWidget	= new QTreeWidget (splitter);
	splitter->addWidget (_entitiesTypesWidget);
	connect (_entitiesTypesWidget, SIGNAL (itemClicked (QTreeWidgetItem*,int)),
         this, SLOT (entitiesTypesStateChangeCallback (QTreeWidgetItem*, int)));
	// On ne peut pas forcer la fermeture d'une branche :
	_entitiesTypesWidget->setItemsExpandable (false);	// Voir expandAll + loin
	_entitiesTypesWidget->setRootIsDecorated (false);	// Pas de croix
	_entitiesTypesWidget->setSelectionBehavior (QTreeWidget::SelectItems);
/*	// Sélection indépendante des items :
	_entitiesTypesWidget->setSelectionMode (QTreeWidget::MultiSelection); */
	_entitiesTypesWidget->setSelectionMode (QTreeWidget::ExtendedSelection);
	_entitiesTypesWidget->setSortingEnabled (false);
	QAbstractItemModel*	treeModel	= _entitiesTypesWidget->model ( );
	_entitiesTypesWidget->setColumnCount (1);
	_entitiesTypesWidget->setColumnWidth (0, 200);
	CHECK_NULL_PTR_ERROR (treeModel)
	treeModel->setHeaderData (0, Qt::Horizontal, QVariant ("Types d'entités"));
	// Géométrie :
	QTreeWidgetItem*	item	= new  	QTreeWidgetItem (_entitiesTypesWidget);
	item->setText (0, "Géométrie");
	_entitiesTypesWidget->addTopLevelItem (item);
	QtEntityTypeItem*	typeItem	=
						new QtEntityTypeItem (item, FilterEntity::GeomVolume);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::GeomSurface);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::GeomCurve);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::GeomVertex);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	// Topologie :
	item	= new  	QTreeWidgetItem (_entitiesTypesWidget);
	item->setText (0, "Topologie");
	_entitiesTypesWidget->addTopLevelItem (item);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::TopoBlock);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::TopoCoFace);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::TopoCoEdge);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::TopoVertex);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	// Maillage :
	item	= new  	QTreeWidgetItem (_entitiesTypesWidget);
	item->setText (0, "Maillage");
	_entitiesTypesWidget->addTopLevelItem (item);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::MeshVolume);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::MeshSurface);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::MeshLine);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::MeshCloud);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	//Repères
	item	= new  	QTreeWidgetItem (_entitiesTypesWidget);
	item->setText (0, "Repère");
	_entitiesTypesWidget->addTopLevelItem (item);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::SysCoord);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	// Maillages structurés :
	item	= new  	QTreeWidgetItem (_entitiesTypesWidget);
	item->setText (0, "Maillage structuré");
	_entitiesTypesWidget->addTopLevelItem (item);
	typeItem	= new QtEntityTypeItem (item, FilterEntity::StructuredMesh);
	typeItem->setCheckState (0, Qt::Unchecked);
	_typesItems.push_back (typeItem);
	item->addChild (typeItem);
	// On force la visibilité de l'ensemble des items :
	_entitiesTypesWidget->expandAll ( );

	_entitiesGroupsWidget	= new QTreeWidget (splitter);
	splitter->addWidget (_entitiesGroupsWidget);
	connect (_entitiesGroupsWidget, SIGNAL (itemClicked (QTreeWidgetItem*,int)),
	         this, SLOT (groupStateChangeCallback (QTreeWidgetItem*, int)));
	connect (_entitiesGroupsWidget, SIGNAL (itemClicked (QTreeWidgetItem*,int)),
	         this, SLOT (showLevelGroupsCallback (QTreeWidgetItem*, int)));
	_entitiesGroupsWidget->setSelectionBehavior (QTreeWidget::SelectItems);
/*/	// Sélection indépendante des items :
	_entitiesGroupsWidget->setSelectionMode (QTreeWidget::MultiSelection);*/
	_entitiesGroupsWidget->setSelectionMode (QTreeWidget::ExtendedSelection);
	_entitiesGroupsWidget->setSortingEnabled (false);
	treeModel	= _entitiesGroupsWidget->model ( );
	_entitiesGroupsWidget->setColumnCount (1);
	_entitiesGroupsWidget->setColumnWidth (0, 200);
	CHECK_NULL_PTR_ERROR (treeModel)
	treeModel->setHeaderData (0, Qt::Horizontal, QVariant ("Groupes"));

	// Les entrées de l'arbre : les groupes sont regroupés par dimension :
	const Qt::ItemFlags	entryFlags	= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	for (int i = 0; i < 4; i++)
	{
		UTF8String	name (Charset::UTF_8);
		name << "Dimension " << (unsigned long)i;
		QTreeWidgetItem* groupEntryItem	=
							new QTreeWidgetItem (_entitiesGroupsWidget);
		groupEntryItem->setFlags (entryFlags);
		groupEntryItem->setText (0, UTF8TOQSTRING (name));
		groupEntryItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		_entitiesGroupsWidget->addTopLevelItem (groupEntryItem);
		_groupsEntriesItems.push_back (groupEntryItem);
		_levelsEntriesItems.push_back (map <unsigned long,QTreeWidgetItem*>( ));
	}	// for (int i = 0; i < 4; i++)

	// On force la visibilité de l'ensemble des items :
	_entitiesGroupsWidget->expandAll ( );

	// Propagation de l'affichage des groupes de dimension N - 1 :
//	_groupsPropagationCheckBox	=
//		new QCheckBox ("Propager l'affichage", this);
//	_groupsPropagationCheckBox->setChecked (Qt::Checked);
//	_groupsPropagationCheckBox->setToolTip (QString::fromUtf8("Cochée, la demande d'affichage d'un groupe provoquera celui des entités du même groupe et de dimensions inférieures."));
//	layout->addWidget (_groupsPropagationCheckBox);
//	connect (_groupsPropagationCheckBox, SIGNAL (stateChanged (int)), this,
//	         SLOT (groupsRepresentationPropagationCallback ( )));

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::createGui


void QtGroupsPanel::createPopupMenus ( )
{
	// Forcer l'affichage d'un menu contextuel :
	assert (0 != _entitiesTypesWidget);
	assert (0 != _entitiesGroupsWidget);

	// Menu popup 1 : Types d'entités :
	// Affichage :
	_typesPopupMenu	= new QMenu (_entitiesTypesWidget);
	QAction*	action	=
			createAction(*_typesPopupMenu, "Afficher", QString::fromUtf8("Provoque l'affichage des entités dont les types sont sélectionnés et les groupes affichés."),
			          SLOT (displaySelectedTypesCallback( )), this, false, OFF);
	_typesPopupMenu->addAction (action);
	action	= createAction (*_typesPopupMenu, "Masquer", QString::fromUtf8("Arrête l'affichage des entités dont les types sont sélectionnés et les groupes affichés."),
			          SLOT (hideSelectedTypesCallback( )), this, false, OFF);
	_typesPopupMenu->addAction (action);
	_typesPopupMenu->addSeparator ( );
	action	= createAction (*_typesPopupMenu, QString::fromUtf8("Représentations ..."), QString::fromUtf8("Affiche une boite de dialogue de sélection des types de représentations (points, filaire, surfacique) à appliquer."),
				SLOT (typesRepresentationTypesCallback ( )), this, false, OFF);
	_typesPopupMenu->addAction (action);
	_typesPopupMenu->addSeparator ( );
	QAction* unselectAllAction1 = createAction (*_typesPopupMenu, QString::fromUtf8("Tout désélectionner"), QString::fromUtf8("Désélectionne tout"),
			SLOT (unselectCallback ( )), this, false, OFF);

	QAction* selectVisibleAction = createAction (*_typesPopupMenu, QString::fromUtf8("Sélectionner les entités visibles"),
			QString::fromUtf8("Sélectionne les entités visibles pour les types sélectionnés"),
			SLOT (selectVisibleCallback ( )), this, false, OFF);

	QAction* selectAllAction = createAction (*_typesPopupMenu, QString::fromUtf8("Sélectionner toutes les entités"),
			QString::fromUtf8("Sélectionne toutes les entités pour les types sélectionnés"),
			SLOT (selectAllCallback ( )), this, false, OFF);


	_entitiesTypesWidget->setContextMenuPolicy (Qt::CustomContextMenu);
	connect (_entitiesTypesWidget,
	         SIGNAL(customContextMenuRequested(const QPoint&)),
	         this, SLOT (displayTypesPopupMenu (const QPoint&)));

	// Menu popup 2 : Groupes d'entités :
	// Affichage :
	_groupsPopupMenu	= new QMenu (_entitiesGroupsWidget);
	action	= createAction (*_groupsPopupMenu, "Afficher", QString::fromUtf8("Provoque l'affichage des entités dont les groupes sont sélectionnés et les types affichés."),
			         SLOT (displaySelectedGroupsCallback( )), this, false, OFF);
	_groupsPopupMenu->addAction (action);
	action	= createAction (*_groupsPopupMenu, "Afficher tous ceux du même nom", QString::fromUtf8("Provoque l'affichage des entités dont les noms des groupes sont sélectionnés et les types affichés."),
			         SLOT (displaySelectedGroupsNamesCallback( )), this, false, OFF);
	_groupsPopupMenu->addAction (action);
	action	= createAction (*_groupsPopupMenu, "Masquer", QString::fromUtf8("Arrête l'affichage des entités dont les groupes sont sélectionnés et les types affichés."),
			          SLOT (hideSelectedGroupsCallback( )), this, false, OFF);
	_groupsPopupMenu->addAction (action);
	action	= createAction (*_groupsPopupMenu, "Masquer tous ceux du même nom", QString::fromUtf8("Arrête l'affichage des entités dont les noms des groupes sont sélectionnés et les types affichés."),
			          SLOT (hideSelectedGroupsNamesCallback( )), this, false, OFF);
	_groupsPopupMenu->addAction (action);
	_groupsPopupMenu->addSeparator ( );
	QMenu*	entry	= new QMenu ("Tri ...", _groupsPopupMenu);
	_groupsPopupMenu->addMenu (entry);
	action	= createCheckableAction (*entry, "Automatique",
					"Tri automatiquement les groupes",
					SLOT (automaticSortCallback (bool)), this,
					Resources::instance ( )._automaticSort.getValue ( ), OFF);
	const bool ascending	=
		0 == Resources::instance ( )._sortType.getValue ( ).ascii ( ).compare (
											"typeAscendingName") ? true : false;
	action	= createCheckableAction (*entry, "Ordre ascendant",
					"Tri les groupes par ordre alphabétique ascendant",
					SLOT (ascendingSortCallback (bool)), this, ascending, OFF);

	_groupsPopupMenu->addSeparator ( );
	action	= createCheckableAction (*_groupsPopupMenu, "Vue multi-niveaux",
					"(Dé)classer les groupes par niveaux.",
					SLOT (multiLevelViewCallback (bool)), this,
					Resources::instance ( )._multiLevelGroupsView.getValue( ), OFF);
	_groupsPopupMenu->addAction (action);
	action	= createAction (*_groupsPopupMenu, "Changer de niveau ...", QString::fromUtf8("Change de niveau les groupes sélectionnés"),
			         SLOT (changeGroupLevelCallback( )), this, false, OFF);
	action	= createAction (*_groupsPopupMenu, QString::fromUtf8("Sélectionner les groupes d'un niveau"), QString::fromUtf8("Sélectionner des groupes selon leurs dimensions et niveaux"),
					SLOT (levelSelectionCallback( )), this, false, OFF);
	_groupsPopupMenu->addAction (action);

	_groupsPopupMenu->addSeparator ( );
	action	= createAction (*_groupsPopupMenu, QString::fromUtf8("Représentations ..."), QString::fromUtf8("Affiche une boite de dialogue de sélection des types de représentations (points, filaire, surfacique) à appliquer."),
				SLOT (groupsRepresentationTypesCallback ( )), this, false, OFF);
	_groupsPopupMenu->addAction (action);
	_groupsPopupMenu->addSeparator ( );
	QAction* unselectAllAction2 = createAction (*_groupsPopupMenu, QString::fromUtf8("Tout désélectionner"), QString::fromUtf8("Désélectionne tout"),
			SLOT (unselectCallback ( )), this, false, OFF);
	entry	= new QMenu (QString::fromUtf8("Sélectionner ..."), _groupsPopupMenu);
	_groupsPopupMenu->addMenu (entry);

	QAction* selectGeomVolumesAction = createAction (*entry, QString::fromUtf8("Volumes Géométriques"),
			QString::fromUtf8("Sélectionne les volumes géométriques associés aux groupes sélectionnés"),
			SLOT (selectGeomVolumesCallback ( )), this, false, OFF);

	QAction* selectGeomSurfacesAction = createAction (*entry, QString::fromUtf8("Surfaces Géométriques"),
			QString::fromUtf8("Sélectionne les surfaces géométriques associés aux groupes sélectionnés"),
			SLOT (selectGeomSurfacesCallback ( )), this, false, OFF);

	QAction* selectGeomCurvesAction = createAction (*entry, QString::fromUtf8("Courbes Géométriques"),
			QString::fromUtf8("Sélectionne les courbes géométriques associés aux groupes sélectionnés"),
			SLOT (selectGeomCurvesCallback ( )), this, false, OFF);

	QAction* selectGeomVerticesAction = createAction (*entry, QString::fromUtf8("Sommets Géométriques"),
			QString::fromUtf8("Sélectionne les sommets géométriques associés aux groupes sélectionnés"),
			SLOT (selectGeomVerticesCallback ( )), this, false, OFF);

	QAction* selectTopoBlocksAction = createAction (*entry, QString::fromUtf8("Blocs Topologiques"),
			QString::fromUtf8("Sélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (selectTopoBlocksCallback ( )), this, false, OFF);

	QAction* selectTopoCoFacesAction = createAction (*entry, QString::fromUtf8("Faces Topologiques"),
			QString::fromUtf8("Sélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (selectTopoCoFacesCallback ( )), this, false, OFF);

	QAction* selectTopoCoEdgesAction = createAction (*entry, QString::fromUtf8("Arêtes Topologiques"),
			QString::fromUtf8("Sélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (selectTopoCoEdgesCallback ( )), this, false, OFF);

	QAction* selectTopoVerticesAction = createAction (*entry, QString::fromUtf8("Sommets Topologiques"),
			QString::fromUtf8("Sélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (selectTopoVerticesCallback ( )), this, false, OFF);

	QAction* selectMeshVolumesAction = createAction (*entry, QString::fromUtf8("Volumes de mailles"),
			QString::fromUtf8("Sélectionne les volumes de mailles associés aux groupes sélectionnés"),
			SLOT (selectMeshVolumesCallback ( )), this, false, OFF);

	QAction* selectMeshSurfacesAction = createAction (*entry, QString::fromUtf8("Surfaces de mailles"),
			QString::fromUtf8("Sélectionne les surfaces de mailles associées aux groupes sélectionnés"),
			SLOT (selectMeshSurfacesCallback ( )), this, false, OFF);

	QAction* selectMeshLinesAction = createAction (*entry, QString::fromUtf8("Lignes de mailles"),
			QString::fromUtf8("Sélectionne les lignes de mailles associées aux groupes sélectionnés"),
			SLOT (selectMeshLinesCallback ( )), this, false, OFF);

	QAction* selectMeshCloudsAction = createAction (*entry, QString::fromUtf8("Nuages de mailles"),
			QString::fromUtf8("Sélectionne les nuages de mailles associés aux groupes sélectionnés"),
			SLOT (selectMeshCloudsCallback ( )), this, false, OFF);


	entry	= new QMenu ("Désélectionner ...", _groupsPopupMenu);
	_groupsPopupMenu->addMenu (entry);

	QAction* unselectGeomVolumesAction = createAction (*entry, QString::fromUtf8("Volumes Géométriques"),
			QString::fromUtf8("Désélectionne les volumes géométriques associés aux groupes sélectionnés"),
			SLOT (unselectGeomVolumesCallback ( )), this, false, OFF);

	QAction* unselectGeomSurfacesAction = createAction (*entry, QString::fromUtf8("Surfaces Géométriques"),
			QString::fromUtf8("Désélectionne les surfaces géométriques associés aux groupes sélectionnés"),
			SLOT (unselectGeomSurfacesCallback ( )), this, false, OFF);

	QAction* unselectGeomCurvesAction = createAction (*entry, QString::fromUtf8("Courbes Géométriques"),
			QString::fromUtf8("Désélectionne les courbes géométriques associés aux groupes sélectionnés"),
			SLOT (unselectGeomCurvesCallback ( )), this, false, OFF);

	QAction* unselectGeomVerticesAction = createAction (*entry, QString::fromUtf8("Sommets Géométriques"),
			QString::fromUtf8("Désélectionne les sommets géométriques associés aux groupes sélectionnés"),
			SLOT (unselectGeomVerticesCallback ( )), this, false, OFF);

	QAction* unselectTopoBlocksAction = createAction (*entry, QString::fromUtf8("Blocs Topologiques"),
			QString::fromUtf8("Désélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (unselectTopoBlocksCallback ( )), this, false, OFF);

	QAction* unselectTopoCoFacesAction = createAction (*entry, QString::fromUtf8("Faces Topologiques"),
			QString::fromUtf8("Désélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (unselectTopoCoFacesCallback ( )), this, false, OFF);

	QAction* unselectTopoCoEdgesAction = createAction (*entry, QString::fromUtf8("Arêtes Topologiques"),
			QString::fromUtf8("Désélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (unselectTopoCoEdgesCallback ( )), this, false, OFF);

	QAction* unselectTopoVerticesAction = createAction (*entry, QString::fromUtf8("Sommets Topologiques"),
			QString::fromUtf8("Désélectionne les blocs topologiques associés aux groupes sélectionnés"),
			SLOT (unselectTopoVerticesCallback ( )), this, false, OFF);

	QAction* unselectMeshVolumesAction = createAction (*entry, QString::fromUtf8("Volumes de mailles"),
			QString::fromUtf8("Désélectionne les volumes de mailles associés aux groupes sélectionnés"),
			SLOT (unselectMeshVolumesCallback ( )), this, false, OFF);

	QAction* unselectMeshSurfacesAction = createAction (*entry, QString::fromUtf8("Surfaces de mailles"),
			QString::fromUtf8("Désélectionne les surfaces de mailles associées aux groupes sélectionnés"),
			SLOT (unselectMeshSurfacesCallback ( )), this, false, OFF);

	QAction* unselectMeshLinesAction = createAction (*entry, QString::fromUtf8("Lignes de mailles"),
			QString::fromUtf8("Désélectionne les lignes de mailles associées aux groupes sélectionnés"),
			SLOT (unselectMeshLinesCallback ( )), this, false, OFF);

	QAction* unselectMeshCloudsAction = createAction (*entry, QString::fromUtf8("Nuages de mailles"),
			QString::fromUtf8("Désélectionne les nuages de mailles associés aux groupes sélectionnés"),
			SLOT (unselectMeshCloudsCallback ( )), this, false, OFF);

	_groupsPopupMenu->addSeparator ( );

	QAction* clearGroupAction = createAction (*_groupsPopupMenu, QString::fromUtf8("Destruction d'un groupe"),
			QString::fromUtf8("Un groupe perd toute les entités qu'il contenait et disparait"),
			SLOT (clearGroupCallback ( )), this, false, OFF);

	_entitiesGroupsWidget->setContextMenuPolicy (Qt::CustomContextMenu);
	connect (_entitiesGroupsWidget,
	         SIGNAL(customContextMenuRequested(const QPoint&)),
	         this, SLOT (displayGroupsPopupMenu (const QPoint&)));
}	// QtGroupsPanel::createPopupMenus


QtGroupTreeWidgetItem* QtGroupsPanel::getGroupItem (const Mgx3D::Group::GroupEntity& group)
{
	if ((0 > group.getDim ( )) || (4 <= group.getDim ( )))
	{
		UTF8String	message (Charset::UTF_8);
		message << "QtGroupsPanel::getItem. Dimension invalide ("
		        << (unsigned long)group.getDim ( ) << ") pour le groupe "
		        << group.getName ( ) << ".";
		throw Exception (message);
	}	// if ((0 > group.getDim ( )) || (4 <= group.getDim ( )))

	if (false == Resources::instance ( )._multiLevelGroupsView.getValue ( ))
	{
		QTreeWidgetItem* parent		= _groupsEntriesItems [group.getDim ( )];
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(parent->child (i));
			if ((0 != groupItem) && (&group == groupItem->getGroup ( )))
				return groupItem;
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// if (false == Resources::instance ( )._multiLevelGroupsView.getValue ( ))
	else
	{
		const map<unsigned long, QTreeWidgetItem*>&	entries	= _levelsEntriesItems [group.getDim ( )];
		map<unsigned long, QTreeWidgetItem*>::const_iterator	it	= entries.find (group.getLevel ( ));
		if (entries.end ( ) != it)
		{
			QTreeWidgetItem*	parent	= (*it).second;
			for (size_t i = 0; i < parent->childCount ( ); i++)
			{
				QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(parent->child (i));
				if ((0 != groupItem) && (&group == groupItem->getGroup ( )))
					return groupItem;
			}	// for (size_t i = 0; i < parent->childCount ( ); i++)
		}	// if (entries.end ( ) == it)
	}	// else if (false == QtMgx3DApplication:: ...

	return 0;
}	// QtGroupsPanel::getGroupItem


QTreeWidgetItem* QtGroupsPanel::createEntryItem (QTreeWidgetItem& parent, const string& title)
{
	QTreeWidgetItem*	item	= new QTreeWidgetItem (&parent);
	item->setText (0, title.c_str ( ));
	parent.addChild (item);

	return item;
}	// QtGroupsPanel::createEntryItem


QtGroupTreeWidgetItem* QtGroupsPanel::createGroupItem (Group::GroupEntity& group)
{
	QTreeWidgetItem*		parent	= getGroupEntryItem (group.getDim ( ), group.getLevel ( ));
	CHECK_NULL_PTR_ERROR (parent)
	QtGroupTreeWidgetItem*	item	= new QtGroupTreeWidgetItem (parent, group);
//	item->setCheckState (0, Qt::Unchecked);
	parent->addChild (item);
	item->setHidden (false);
	QtGroupLevelTreeWidgetItem*	levelItem	= dynamic_cast<QtGroupLevelTreeWidgetItem*>(parent);
	if ((0 != levelItem) && (Qt::Checked == levelItem->checkState (0)))
		levelItem->updateState ( );

	return item;
}	// QtGroupsPanel::createGroupItem


QTreeWidgetItem* QtGroupsPanel::getGroupEntryItem (unsigned char dim, unsigned long level)
{
	if (4 <= dim)
	{
		UTF8String	message (Charset::UTF_8);
		message << "QtGroupsPanel::getGroupEntryItem. Dimension invalide (" << (unsigned long)dim << ").";
		throw Exception (message);
	}	// if (4 <= dim)

	if (true != Resources::instance ( )._multiLevelGroupsView.getValue ( ))
		return _groupsEntriesItems [dim];

	map<unsigned long, QTreeWidgetItem*>&	entries	= _levelsEntriesItems [dim];
	map<unsigned long, QTreeWidgetItem*>::iterator ite	= entries.find (level);
	if (entries.end ( ) != ite)
		return (*ite).second;

	QtGroupLevelTreeWidgetItem*	entry	= new QtGroupLevelTreeWidgetItem (_groupsEntriesItems[dim], level, getGraphicalWidget ( ));
	_groupsEntriesItems [dim]->addChild (entry);
	entries.insert (pair <unsigned long, QTreeWidgetItem*>(level, entry));
	sort ( );

	return entry;
}	// QtGroupsPanel::getGroupEntryItem


bool QtGroupsPanel::groupEntitiesDisplayModifications ( ) const
{
	return false;
}	// QtGroupsPanel::groupEntitiesDisplayModifications


void QtGroupsPanel::entitiesTypesStateChangeCallback (QTreeWidgetItem* item, int col)
{
	BEGIN_QT_TRY_CATCH_BLOCK

//	if ((0 == item) || (0 == item->parent ( )))
//		return;

	const FilterEntity::objectType	typesMask	= getCheckedEntitiesTypes ( );
	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).displayTypes (typesMask);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::entitiesTypesStateChangeCallback


void QtGroupsPanel::groupStateChangeCallback (QTreeWidgetItem* item, int col)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Pb : le seul moyen d'inhiber la sélection des items est de les rendre disabled via setFlags (Qt 4.7.4).
	// Or on ne souhaite pas ici actualiser la vue si un item racine change d'état sélectionné.
	// => On va tester le callback sur le type d'item
	// => on n'utilise pas le signal itemSelectionChanged ()
	if (0 == item->parent ( ))
		return;

	QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(item);
	Group::GroupEntity*		group		= 0 == groupItem ? 0 : groupItem->getGroup ( );
	if (0 != group)
	{
		if (0 != getGraphicalWidget ( ))
		{
			const bool	show	= groupItem->checkState (0) ? true : false;
			vector<GroupEntity*>	groups;
			groups.push_back (group);
			getGraphicalWidget ( )->getRenderingManager ( ).displayRepresentations (groups, show);
		}	// if (0 != getGraphicalWidget ( ))

		QtGroupLevelTreeWidgetItem*	levelItem	= dynamic_cast<QtGroupLevelTreeWidgetItem*>(groupItem->parent ( ));
		if (0 != levelItem)
			levelItem->updateState ( );
	}	// if (0 != group)

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::groupStateChangeCallback


void QtGroupsPanel::displaySelectedTypesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	displaySelectedTypes (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::displaySelectedTypesCallback


void QtGroupsPanel::hideSelectedTypesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	displaySelectedTypes (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::hideSelectedTypesCallback


void QtGroupsPanel::displaySelectedGroupsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	displaySelectedGroups (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::displaySelectedGroupsCallback


void QtGroupsPanel::displaySelectedGroupsNamesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	displaySelectedGroupsNames (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::displaySelectedGroupsNamesCallback


void QtGroupsPanel::hideSelectedGroupsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	displaySelectedGroups (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::hideSelectedGroupsCallback


void QtGroupsPanel::hideSelectedGroupsNamesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	displaySelectedGroupsNames (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::hideSelectedGroupsNamesCallback


void QtGroupsPanel::automaticSortCallback (bool automatic)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Resources::instance ( )._automaticSort.setValue (automatic);
	if (true == automatic)
		sort ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::automaticSortCallback


void QtGroupsPanel::ascendingSortCallback (bool ascending)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (true == ascending)
		Resources::instance ( )._sortType.setValue ("typeAscendingName");
	else
		Resources::instance ( )._sortType.setValue ("typeDescendingName");
	typeNameSort ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::ascendingSortCallback


void QtGroupsPanel::multiLevelViewCallback (bool enabled)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Resources::instance ( )._multiLevelGroupsView.setValue (enabled);

	if (true == enabled)	// Vue multi-niveaux
	{
		for (vector<QTreeWidgetItem*>::iterator it = _groupsEntriesItems.begin ( ); it != _groupsEntriesItems.end ( ); it++)
		{
			QList<QTreeWidgetItem*>	children	= (*it)->takeChildren ( );
			for (QList<QTreeWidgetItem*>::iterator itc = children.begin ( ); itc != children.end ( ); itc++)
			{
				QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(*itc);
				if (0 == groupItem)	// => entrée "Niveau"
				{
					(*it)->addChild (*itc);
					if (0 == (*itc)->childCount ( ))
						(*itc)->setHidden (true);
					continue;
				}	// if (0 == groupItem)

				CHECK_NULL_PTR_ERROR (groupItem)
				CHECK_NULL_PTR_ERROR (groupItem->getGroup ( ))
				unsigned char		dim		= groupItem->getGroup ( )->getDim ( );
				unsigned long		level	= groupItem->getGroup ( )->getLevel ( );
				QtGroupLevelTreeWidgetItem*	entry	= dynamic_cast<QtGroupLevelTreeWidgetItem*>(getGroupEntryItem (dim, level));
				CHECK_NULL_PTR_ERROR (entry)
				entry->addChild (*itc);
				entry->setHidden (false);
				entry->updateState ( );
			}	// for (QList<QTreeWidgetItem*>::iterator itc = ...
		}	// for (vector<QTreeWidgetItem*>::iterator it = ...
	}	// if (true == enabled)
	else
	{	// Mode mono-niveau
		for (int i = 0; i < 4; i++)	
		{
			map<unsigned long, QTreeWidgetItem*>&	entries	= _levelsEntriesItems [i];
			for (map<unsigned long, QTreeWidgetItem*>::iterator it = entries.begin ( ); entries.end ( ) != it; it++)
			{
				QtExpansionTreeRestorer	restorer (*(*it).second);
				QTreeWidgetItem*		entry		= (*it).second;
				QList<QTreeWidgetItem*>	children	= entry->takeChildren ( );
				for (QList<QTreeWidgetItem*>::iterator itc = children.begin ( );
				     itc != children.end ( ); itc++)
				{
					QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(*itc);
					CHECK_NULL_PTR_ERROR (groupItem)
					CHECK_NULL_PTR_ERROR (groupItem->getGroup ( ))
					unsigned char		dim	 	= groupItem->getGroup ( )->getDim ( );
					unsigned long		level	= groupItem->getGroup ( )->getLevel( );
					QTreeWidgetItem*	e		= getGroupEntryItem (dim, level);
					CHECK_NULL_PTR_ERROR (e)
					e->addChild (*itc);
				}	// for (QList<QTreeWidgetItem*>::iterator itc = ...
				restorer.restore ( );
				entry->setHidden (true);
			}	// for (map<unsigned long, QTreeWidgetItem*>::iterator it = ...
		}	// for (int i = 0; i < 4; i++)
	}	// else if (true == enabled)

	sort ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::multiLevelViewCallback


void QtGroupsPanel::showLevelGroupsCallback (QTreeWidgetItem* item, int column)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtGroupLevelTreeWidgetItem*	levelItem	= dynamic_cast<QtGroupLevelTreeWidgetItem*>(item);
	if (0 != levelItem)
		levelItem->update ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::showLevelGroupsCallback


void QtGroupsPanel::changeGroupLevelCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	vector<QtGroupTreeWidgetItem*>	selection	= getSelectedGroupsItems ( );
	if (0 == selection.size ( ))
		return;
	unsigned long	level	= selection [0]->getGroup ( )->getLevel ( );
	bool			ok		= true;
	level	= QInputDialog::getInt (this, "Magix 3D", "Niveau des groupes :", level, 0, 100, 1, &ok);

	if (false == ok)
		return;

	bool	doSort	= false;
	for (vector<QtGroupTreeWidgetItem*>::iterator itg = selection.begin ( ); selection.end ( ) != itg; itg++)
	{
		CHECK_NULL_PTR_ERROR ((*itg)->getGroup ( ))
		if ((*itg)->getGroup ( )->getLevel ( ) != level)
		{
			(*itg)->getGroup ( )->setLevel (level);
			doSort	= true;
			if (true == Resources::instance ( )._multiLevelGroupsView.getValue ( ))
			{
				QtExpansionTreeRestorer	restorer (**itg);
				QTreeWidgetItem*	parent	= (*itg)->parent ( );
				CHECK_NULL_PTR_ERROR (parent)
				(*itg)->parent ( )->removeChild (*itg);
				QtGroupLevelTreeWidgetItem*	entry	= dynamic_cast<QtGroupLevelTreeWidgetItem*>(getGroupEntryItem ((*itg)->getGroup ( )->getDim ( ), level));
				CHECK_NULL_PTR_ERROR (entry)
				const size_t	count		= entry->childCount ( );
				const bool		expanded	= entry->isExpanded ( );
				entry->addChild (*itg);
				restorer.restore ( );
				if (0 == count)	// before addChild
					entry->setHidden (false);
				if (false == expanded)			// before addChild
					entry->setExpanded (true);	// Voir la sélection
				if (0 == parent->childCount ( ))
					parent->setHidden (true);
				entry->updateState ( );
			}	// if (true == QtMgx3DApplication
		}	// if ((*itg)->getGroup ( )->getLevel ( ) != level)
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator itg = ...
	
	if (true == doSort)
		sort ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::changeGroupLevelCallback


void QtGroupsPanel::levelSelectionCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesGroupsWidget)
	QtGroupsSelectorDialog	dialog (this, "Magix 3D - Sélection de groupes");
	if (QDialog::Accepted != dialog.exec ( ))
		return;

	if (true == dialog.newSelection ( ))
		_entitiesGroupsWidget->clearSelection ( );
//		getContext ( ).getSelectionManager ( ).clearSelection();

	const SelectionManagerIfc::DIM	dimensions	= dialog.dimensions ( );
	const set<unsigned long>		levels		= dialog.levels ( );
	vector<GroupEntity*>			groups;
//	vector<Entity*>					selection;
	getContext ( ).getGroupManager( ).getGroups (groups, dimensions, true);
	for (vector<GroupEntity*>::iterator itg = groups.begin ( );
	     groups.end ( ) != itg; itg++)
	{
		for (set<unsigned long>::const_iterator itl = levels.begin ( );
		     levels.end ( ) != itl; itl++)
			if (*itl == (*itg)->getLevel ( ))
			{
				QtGroupTreeWidgetItem*	item	= getGroupItem (**itg);
				if (0 != item)
					item->setSelected (true);
			}	// for (set<unsigned long>::const_iterator itl = ...
	}	// for (vector<GroupEntity*>::iterator itg = groups.begin ( ); ...
// Non, Magix3D ne fonctionne pas (pour le moment) comme ça
//	getContext ( ).getSelectionManager ( ).addToSelection (selection);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::levelSelectionCallback


void QtGroupsPanel::typesRepresentationTypesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker displayLocker(
	getGraphicalWidget ( )->getRenderingManager ( ));
    
	// On s'intéresse aux types sélectionnés :
	FilterEntity::objectType    types   = getSelectedEntitiesTypes ( );
	vector<Entity*>             entities;
	getContext ( ).getGroupManager ( ).getShownEntities (types, entities);
	changeRepresentationTypes (entities);
    
	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}   // QtGroupsPanel::typesRepresentationTypesCallback


void QtGroupsPanel::groupsRepresentationTypesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<GroupEntity*>			groups;
	vector<Entity*>					entities;
	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
			// Quelles entités du groupe sont affichées ?
			vector<Entity*>	groupEntities	= (*it)->getGroup ( )->getEntities ( );
			for (vector<Entity*>::iterator ite = groupEntities.begin ( );
			     groupEntities.end ( ) != ite; ite++)
			{
				CHECK_NULL_PTR_ERROR (*ite)
				if (0 != (*ite)->getDisplayProperties ( ).getGraphicalRepresentation ( ))
					entities.push_back (*ite);
			}	// for (vector<Entity*>::iterator ite = ...
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	changeRepresentationTypes (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtGroupsPanel::groupsRepresentationTypesCallback


void QtGroupsPanel::unselectCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	getContext ( ).getSelectionManager ( ).clearSelection();

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

} // QtGroupsPanel::unselectCallback


void QtGroupsPanel::selectVisibleCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux types sélectionnés :
	FilterEntity::objectType	types	= getSelectedEntitiesTypes ( );
	vector<Entity*>				entities;
	getContext ( ).getGroupManager ( ).getShownEntities (types, entities);

	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectVisibleCallback


void QtGroupsPanel::selectAllCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux types sélectionnés :
	FilterEntity::objectType	types	= getSelectedEntitiesTypes ( );
	vector<Entity*>				entities;

	if (types & FilterEntity::GeomVolume){
		std::vector<Geom::Volume*> ge = getContext ( ).getGeomManager().getVolumesObj();
		entities.insert(entities.end(), ge.begin(), ge.end());
	}
	if (types & FilterEntity::GeomSurface){
		std::vector<Geom::Surface*> ge = getContext ( ).getGeomManager().getSurfacesObj();
		entities.insert(entities.end(), ge.begin(), ge.end());
	}
	if (types & FilterEntity::GeomCurve){
		std::vector<Geom::Curve*> ge = getContext ( ).getGeomManager().getCurvesObj();
		entities.insert(entities.end(), ge.begin(), ge.end());
	}
	if (types & FilterEntity::GeomVertex){
		std::vector<Geom::Vertex*> ge = getContext ( ).getGeomManager().getVerticesObj();
		entities.insert(entities.end(), ge.begin(), ge.end());
	}
	if (types & FilterEntity::TopoBlock){
		std::vector<Topo::Block*> te;
		getContext ( ).getTopoManager().getBlocks(te);
		entities.insert(entities.end(), te.begin(), te.end());
	}
	if (types & FilterEntity::TopoCoFace){
		std::vector<Topo::CoFace*> te;
		getContext ( ).getTopoManager().getCoFaces(te);
		entities.insert(entities.end(), te.begin(), te.end());
	}
	if (types & FilterEntity::TopoCoEdge){
		std::vector<Topo::CoEdge*> te;
		getContext ( ).getTopoManager().getCoEdges(te);
		entities.insert(entities.end(), te.begin(), te.end());
	}
	if (types & FilterEntity::TopoVertex){
		std::vector<Topo::Vertex*> te;
		getContext ( ).getTopoManager().getVertices(te);
		entities.insert(entities.end(), te.begin(), te.end());
	}
	if (types & FilterEntity::MeshSurface){
		std::vector<Mesh::Surface*> ms;
		getContext ( ).getMeshManager().getSurfaces(ms);
		entities.insert(entities.end(), ms.begin(), ms.end());
	}
	if (types & FilterEntity::MeshVolume){
		std::vector<Mesh::Volume*> me;
		getContext ( ).getMeshManager().getVolumes(me);
		entities.insert(entities.end(), me.begin(), me.end());
	}

	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

} // QtGroupsPanel::selectAllCallback


void QtGroupsPanel::selectGeomVolumesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Volume*> volumes;
	getContext ( ).getGroupManager ( ).get(groupsNames, volumes);

	vector<Entity*>	entities;
	entities.insert(entities.end(), volumes.begin(), volumes.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectGeomVolumesCallback

void QtGroupsPanel::selectGeomSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Surface*> ge;
	getContext ( ).getGroupManager ( ).get(groupsNames, ge);

	vector<Entity*>	entities;
	entities.insert(entities.end(), ge.begin(), ge.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectGeomSurfacesCallback


void QtGroupsPanel::selectGeomCurvesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Curve*> ge;
	getContext ( ).getGroupManager ( ).get(groupsNames, ge);

	vector<Entity*>	entities;
	entities.insert(entities.end(), ge.begin(), ge.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectGeomCurvesCallback


void QtGroupsPanel::selectGeomVerticesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Vertex*> ge;
	getContext ( ).getGroupManager ( ).get(groupsNames, ge);

	vector<Entity*>	entities;
	entities.insert(entities.end(), ge.begin(), ge.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectGeomVerticesCallback


void QtGroupsPanel::selectTopoBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::Block*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectTopoBlocksCallback


void QtGroupsPanel::selectTopoCoFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::CoFace*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectTopoCoFacesCallback


void QtGroupsPanel::selectTopoCoEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::CoEdge*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectTopoCoEdgesCallback


void QtGroupsPanel::selectTopoVerticesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::Vertex*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectTopoVerticesCallback


void QtGroupsPanel::selectMeshVolumesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Volume*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectMeshVolumesCallback


void QtGroupsPanel::selectMeshSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Surface*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectMeshSurfacesCallback


void QtGroupsPanel::selectMeshLinesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Line*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectMeshLinesCallback


void QtGroupsPanel::selectMeshCloudsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Cloud*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::selectMeshCloudsCallback


void QtGroupsPanel::unselectGeomVolumesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Volume*> volumes;
	getContext ( ).getGroupManager ( ).get(groupsNames, volumes);

	vector<Entity*>	entities;
	entities.insert(entities.end(), volumes.begin(), volumes.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectGeomVolumesCallback


void QtGroupsPanel::unselectGeomSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Surface*> ge;
	getContext ( ).getGroupManager ( ).get(groupsNames, ge);

	vector<Entity*>	entities;
	entities.insert(entities.end(), ge.begin(), ge.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectGeomSurfacesCallback


void QtGroupsPanel::unselectGeomCurvesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Curve*> ge;
	getContext ( ).getGroupManager ( ).get(groupsNames, ge);

	vector<Entity*>	entities;
	entities.insert(entities.end(), ge.begin(), ge.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectGeomCurvesCallback


void QtGroupsPanel::unselectGeomVerticesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Geom::Vertex*> ge;
	getContext ( ).getGroupManager ( ).get(groupsNames, ge);

	vector<Entity*>	entities;
	entities.insert(entities.end(), ge.begin(), ge.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectGeomVerticesCallback


void QtGroupsPanel::unselectTopoBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::Block*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectTopoBlocksCallback


void QtGroupsPanel::unselectTopoCoFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::CoFace*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectTopoCoFacesCallback


void QtGroupsPanel::unselectTopoCoEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::CoEdge*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectTopoCoEdgesCallback


void QtGroupsPanel::unselectTopoVerticesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Topo::Vertex*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectTopoVerticesCallback


void QtGroupsPanel::unselectMeshVolumesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Volume*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectMeshVolumesCallback


void QtGroupsPanel::unselectMeshSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Surface*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectMeshSurfacesCallback


void QtGroupsPanel::unselectMeshLinesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Line*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectMeshLinesCallback


void QtGroupsPanel::unselectMeshCloudsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker(getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux groupes sélectionnés :
	vector<string>			groupsNames;

	vector<QtGroupTreeWidgetItem*>	items	= getSelectedGroupsItems ( );
	for (vector<QtGroupTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		if (true == (*it)->isSelected ( ))
		{
			CHECK_NULL_PTR_ERROR ((*it)->getGroup ( ))
		    groupsNames.push_back((*it)->getGroup ( )->getName());
		}
	}	// for (vector<QtGroupTreeWidgetItem*>::iterator it = ...

	std::vector<Mesh::Cloud*> te;
	getContext ( ).getGroupManager ( ).get(groupsNames, te);

	vector<Entity*>	entities;
	entities.insert(entities.end(), te.begin(), te.end());
	getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

}  // QtGroupsPanel::unselectMeshCloudsCallback


void QtGroupsPanel::displayTypesPopupMenu (const QPoint& point)
{
	if ((0 != _typesPopupMenu) && (0 != _entitiesTypesWidget))
		_typesPopupMenu->popup (_entitiesTypesWidget->mapToGlobal (point));
}	// QtGroupsPanel::displayTypesPopupMenu


void QtGroupsPanel::displayGroupsPopupMenu (const QPoint& point)
{
	if ((0 != _groupsPopupMenu) && (0 != _entitiesGroupsWidget))
		_groupsPopupMenu->popup (_entitiesGroupsWidget->mapToGlobal (point));
}	// QtGroupsPanel::displayGroupsPopupMenu


vector<QtGroupTreeWidgetItem*> QtGroupsPanel::getSelectedGroupsItems ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesGroupsWidget)
	vector<QtGroupTreeWidgetItem*>	selectedItems;
	QList<QTreeWidgetItem*>			items	= _entitiesGroupsWidget->selectedItems ( );
	for (QList<QTreeWidgetItem*>::iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		QtGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtGroupTreeWidgetItem*>(*it);
		if (0 != groupItem)
			selectedItems.push_back (groupItem);
		else {
			// cas d'un item pour une dimension
			for (uint i=0; i<(*it)->childCount(); i++){
				QtGroupTreeWidgetItem*	groupItem2	= dynamic_cast<QtGroupTreeWidgetItem*>((*it)->child(i));
				if (0 != groupItem2)
					selectedItems.push_back (groupItem2);
			}
		}
	}	// for (QList<QTreeWidgetItem*>::iterator it = items.begin ( ); ...

	return selectedItems;
}	// QtGroupsPanel::getSelectedGroupsItems


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
