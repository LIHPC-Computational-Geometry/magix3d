/**
 * \file        QtSelectionIndividualPropertiesPanel.cpp
 * \author      Charles PIGNEROL
 * \date        12/09/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtSelectionIndividualPropertiesPanel.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QHeaderView>
#include <QScrollBar>

#include <assert.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;
/*static bool isInfinite (double value)
{
	if ((DBL_MAX == value) || (-DBL_MAX == value))
		return true;	// En fait non mais utilisé ici comme infini.
	if ((0 != isfinite (value)) && (0 == isnan (value)) && (0 == isinf (value)))
		return false;

	return true;
}	// isInfinite (double value)*/


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                         LA CLASSE QtEntityPropertiesItem
// ===========================================================================

QtEntityPropertiesItem::QtEntityPropertiesItem (
					QTreeWidget* parent, Entity& entity, bool alsoComputed,
					RenderingManager* rm)
	: QtSelectionPropertiesTreeItem (parent, 0, alsoComputed, rm),
	  _entity (&entity)
{
	setText (0, entity.getName ( ).c_str ( ));
	setText (1, entity.getTypeName ( ).c_str ( ));
}	// QtEntityPropertiesItem::QtEntityPropertiesItem


QtEntityPropertiesItem::QtEntityPropertiesItem (
				QTreeWidgetItem* parent, Entity& entity, bool alsoComputed,
				RenderingManager* rm)
	: QtSelectionPropertiesTreeItem (parent, 0, alsoComputed, rm),
	  _entity (&entity)
{
//	setFlags (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	setText (0, entity.getName ( ).c_str ( ));
	setText (1, entity.getTypeName ( ).c_str ( ));
}	// QtEntityPropertiesItem::QtEntityPropertiesItem


QtEntityPropertiesItem::QtEntityPropertiesItem (const QtEntityPropertiesItem&)
	: QtSelectionPropertiesTreeItem (
				(QTreeWidget*)0, new SerializedRepresentation ( ), false, 0),
	  _entity (0)
{
	MGX_FORBIDDEN ("QtEntityPropertiesItem copy constructor is not allowed.");
}	// QtEntityPropertiesItem::QtEntityPropertiesItem


QtEntityPropertiesItem& QtEntityPropertiesItem::operator = (
												const QtEntityPropertiesItem&)
{
	MGX_FORBIDDEN ("QtEntityPropertiesItem assignment operator is not allowed.");
	return *this;
}	// QtEntityPropertiesItem::operator =


QtEntityPropertiesItem::~QtEntityPropertiesItem ()
{
}	// QtEntityPropertiesItem::~QtEntityPropertiesItem


void QtEntityPropertiesItem::initialize ( )
{
	try
	{
		QtSelectionPropertiesTreeItem::initialize ( );
	}
	catch (...)
	{
	}

	int				i	= 0;
	QBrush			brush (Qt::lightGray);
	try
	{
	    if (getEntity ( ).isDestroyed())
	        brush.setColor(Qt::red);
    }
	catch (...)
	{
	}

	for (i = 0; i < columnCount ( ); i++)
		setBackground (i, brush);	

	try
	{
		const string	summary	= getEntity ( ).getSummary ( );
		for (i = 0; i < columnCount ( ); i++)
			setToolTip (i, summary.c_str ( ));	
	}
	catch (...)
	{
	}
}	// QtEntityPropertiesItem::initialize


Entity& QtEntityPropertiesItem::getEntity ( )
{
	CHECK_NULL_PTR_ERROR (_entity)
	return *_entity;
}	// QtEntityPropertiesItem::getEntity


const Entity& QtEntityPropertiesItem::getEntity ( ) const
{
	CHECK_NULL_PTR_ERROR (_entity)
	return *_entity;
}	// QtEntityPropertiesItem::getEntity


void QtEntityPropertiesItem::updateDescription ( )
{
	SerializedRepresentation*	description	= 0 ;

	try {
	description	=
			getEntity ( ).getDescription (displayComputedProperties ( ));
	}
	catch (TkUtil::Exception &e){
		std::cerr<<"ERREUR "<<e.getFullMessage()<<std::endl;
	}

	setDescription (description);
}	// QtEntityPropertiesItem::updateDescription


// ===========================================================================
//            LA CLASSE QtSelectionIndividualPropertiesTreeWidget
// ===========================================================================

QtSelectionIndividualPropertiesTreeWidget::QtSelectionIndividualPropertiesTreeWidget (
										QWidget* parent, RenderingManager* rm)
	: QTreeWidget (parent), _renderingManager (0)
{
	setSelectionBehavior (QTreeWidget::SelectRows);
	setSelectionMode (QTreeWidget::ExtendedSelection);
	setRootIsDecorated (true);
	setSortingEnabled (false);
	setColumnCount (2);
	connect (this, SIGNAL (itemExpanded(QTreeWidgetItem*)), this,
	         SLOT (itemExpandedCallback(QTreeWidgetItem*)));
}	// QtSelectionIndividualPropertiesTreeWidget::QtSelectionIndividualPropertiesTreeWidget


QtSelectionIndividualPropertiesTreeWidget::QtSelectionIndividualPropertiesTreeWidget (
							const QtSelectionIndividualPropertiesTreeWidget&)
	: QTreeWidget (0), _renderingManager (0)
{
	MGX_FORBIDDEN ("QtSelectionIndividualPropertiesTreeWidget copy constructor is not allowed.");
}	// QtSelectionIndividualPropertiesTreeWidget::QtSelectionIndividualPropertiesTreeWidget


QtSelectionIndividualPropertiesTreeWidget&
			QtSelectionIndividualPropertiesTreeWidget::operator = (
							const QtSelectionIndividualPropertiesTreeWidget&)
{
	MGX_FORBIDDEN ("QtSelectionIndividualPropertiesTreeWidget assignment operator is not allowed.");
	return *this;
}	// QtSelectionIndividualPropertiesTreeWidget::operator =


QtSelectionIndividualPropertiesTreeWidget::~QtSelectionIndividualPropertiesTreeWidget ( )
{
}	// QtSelectionIndividualPropertiesTreeWidget::~QtSelectionIndividualPropertiesTreeWidget


void QtSelectionIndividualPropertiesTreeWidget::itemExpandedCallback (QTreeWidgetItem* item)
{
	QtSelectionPropertiesTreeItem*	pti	=
						dynamic_cast<QtSelectionPropertiesTreeItem*>(item);
	try
	{	// Si la description n'est pas calculée on le fait :
		if (0 != pti)
			pti->getDescription ( );	// throw ...
	}
	catch (...)
	{
		QtEntityPropertiesItem*	pi	=
									dynamic_cast<QtEntityPropertiesItem*>(item);
		if (0 != pi)
		{
			pi->takeChildren ( );
			pi->updateDescription ( );
		}	// if (0 != pi)
	}

	try
	{
		if (0 != pti)
			pti->displayContents ( );

		for (int i = 0; i < columnCount ( ); i++)
			resizeColumnToContents (i);
	}
	catch (...)
	{
	}
}	// QtSelectionIndividualPropertiesTreeWidget::itemExpandedCallback


void QtSelectionIndividualPropertiesTreeWidget::selectionChanged (
			const QItemSelection& selected, const QItemSelection& deselected)
{
	try
	{
		QTreeWidget::selectionChanged (selected, deselected);
		highlightSelection (filteredSelectionList (selected), true);
		highlightSelection (filteredSelectionList (deselected), false);
	}
	catch (...)
	{
	}
}	// QtSelectionIndividualPropertiesTreeWidget::selectionChanged


void QtSelectionIndividualPropertiesTreeWidget::highlightSelection (
					const vector<QTreeWidgetItem*>& selection, bool highlight)
{
	try
	{
		for (vector<QTreeWidgetItem*>::const_iterator its = selection.begin ( );
		     selection.end ( ) != its; its++)
		{
			QtEntityPropertiesItem*	pi	=
							dynamic_cast<QtEntityPropertiesItem*>(*its);
			if (0 != pi)
			{
				Entity&		entity	= pi->getEntity ( );
				DisplayProperties::GraphicalRepresentation*	gr	=
					entity.getDisplayProperties( ).getGraphicalRepresentation();
				if (0 != gr)
				{
					RenderedEntityRepresentation*	rep	=
								dynamic_cast<RenderedEntityRepresentation*>(gr);
					if (0 != rep)
						rep->setRenderingManager (getRenderingManager ( ));
					gr->setHighlighted (highlight);
				}
			}	// if (0 != pi)
		}	// for (vector<QTreeWidgetItem*>::iterator its = ...
	}
	catch (...)
	{
	}
}	// QtSelectionIndividualPropertiesTreeWidget::highlightSelection


RenderingManager* QtSelectionIndividualPropertiesTreeWidget::getRenderingManager ( )
{
	return _renderingManager;
}	// QtSelectionIndividualPropertiesTreeWidget::getRenderingManager


void QtSelectionIndividualPropertiesTreeWidget::setRenderingManager (RenderingManager* rm)
{
	_renderingManager	= rm;
}	// QtSelectionIndividualPropertiesTreeWidget:sgetRenderingManager


vector<QTreeWidgetItem*>
		QtSelectionIndividualPropertiesTreeWidget::filteredSelectionList (const QItemSelection& list)
{	// From Lem 3.3.1 :
	vector<QTreeWidgetItem*>    items;

	QModelIndexList indexes = list.indexes ( );
	for (size_t i = 0; i < indexes.count ( ); i++)
	{   // On est tenté de faire i += n, où n est le nombre de colonne -1, mais
		// on ne préjugera pas de l'ordre ...
		QTreeWidgetItem*    item    =
				dynamic_cast<QTreeWidgetItem*>(itemFromIndex (indexes [i]));
		if (0 == item)
		continue;

		// Cet item est il déjà recensé :
		bool    founded = false;
		for (vector<QTreeWidgetItem*>::const_iterator it = items.begin ( );
		items.end ( ) != it; it++)
		{
			if (*it == item)
			{
				founded = true;
				break;
			}   // if (*it == item)
		}   // for (vector<QTreeWidgetItem*>::const_iterator it = ..

		if (false == founded)
			items.push_back (item);
	}	// for (i = 0; i < indexes.count ( ); i++)

	return items;
}	// filteredSelectionList


// ===========================================================================
//                        LA CLASSE QtSelectionIndividualPropertiesPanel
// ===========================================================================

QtSelectionIndividualPropertiesPanel::QtSelectionIndividualPropertiesPanel (
			QWidget* parent, const string& name, SelectionManagerIfc* manager)
	: QtGroupBox (name.c_str ( ), parent), SelectionManagerObserver (manager),
	  _entitiesWidget (0), _graphicalWidget (0),
	  _displayProperties (true), _displayComputedProperties (true)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);

	// Les entités sélectionnées.
	_entitiesWidget	= new QtSelectionIndividualPropertiesTreeWidget (this, 0);
	layout->addWidget (_entitiesWidget);
}	// QtSelectionIndividualPropertiesPanel::QtSelectionIndividualPropertiesPanel


QtSelectionIndividualPropertiesPanel::QtSelectionIndividualPropertiesPanel (
								const QtSelectionIndividualPropertiesPanel& gp)
	: QtGroupBox ("Invalid panel", 0), SelectionManagerObserver (0),
	  _entitiesWidget (0), _graphicalWidget (0),
	  _displayProperties (true), _displayComputedProperties (true)
{
	MGX_FORBIDDEN ("QtSelectionIndividualPropertiesPanel copy constructor is not allowed.");
}	// QtSelectionIndividualPropertiesPanel::QtSelectionIndividualPropertiesPanel (const QtSelectionIndividualPropertiesPanel&)


QtSelectionIndividualPropertiesPanel&
					QtSelectionIndividualPropertiesPanel::operator = (
									const QtSelectionIndividualPropertiesPanel&)
{
	MGX_FORBIDDEN ("QtSelectionIndividualPropertiesPanel assignment operator is not allowed.");
	return *this;
}	// QtSelectionIndividualPropertiesPanel::QtSelectionIndividualPropertiesPanel (const QtSelectionIndividualPropertiesPanel&)


QtSelectionIndividualPropertiesPanel::~QtSelectionIndividualPropertiesPanel ( )
{
}	// QtSelectionIndividualPropertiesPanel::~QtSelectionIndividualPropertiesPanel


void QtSelectionIndividualPropertiesPanel::writeSettings (QSettings& settings)
{
	settings.beginGroup ("SelectionIndividualPropertiesPanel");
	settings.setValue ("size", size ( ));
	settings.endGroup ( );
}	// QtSelectionIndividualPropertiesPanel::writeSettings


void QtSelectionIndividualPropertiesPanel::readSettings (QSettings& settings)
{
	settings.beginGroup ("SelectionIndividualPropertiesPanel");
	resize (settings.value ("size", size ( )).toSize ( ));
	settings.endGroup ( );
}	// QtSelectionIndividualPropertiesPanel::readSettings


bool QtSelectionIndividualPropertiesPanel::displayProperties ( ) const
{
	return _displayProperties;
}	// QtSelectionIndividualPropertiesPanel::displayProperties


void QtSelectionIndividualPropertiesPanel::displayProperties (bool display)
{
	if (display == _displayProperties)
		return;

	BEGIN_QT_TRY_CATCH_BLOCK

	_displayProperties	= display;

	if (false == _displayProperties)
	{
		CHECK_NULL_PTR_ERROR (_entitiesWidget)
		_entitiesWidget->clear ( );
		_entitiesWidget->setColumnCount (2);
	}	// if (false == _displayProperties)
	else
		selectionModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtSelectionIndividualPropertiesPanel::displayProperties


bool QtSelectionIndividualPropertiesPanel::displayComputedProperties ( ) const
{
	return _displayComputedProperties;
}	// QtSelectionIndividualPropertiesPanel::displayComputedProperties


void QtSelectionIndividualPropertiesPanel::displayComputedProperties (bool display)
{
	if (display == _displayComputedProperties)
		return;

	BEGIN_QT_TRY_CATCH_BLOCK

	_displayComputedProperties	= display;

	if (false == _displayProperties)
	{
		CHECK_NULL_PTR_ERROR (_entitiesWidget)
		_entitiesWidget->clear ( );
		_entitiesWidget->setColumnCount (2);
	}	// if (false == _displayProperties)
	else
		selectionModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtSelectionIndividualPropertiesPanel::displayComputedProperties


void QtSelectionIndividualPropertiesPanel::setSelectionManager (SelectionManagerIfc* selectionManager)
{
	if (selectionManager == getSelectionManager ( ))
		return;

	SelectionManagerObserver::setSelectionManager (selectionManager);
	selectionModified ( );
}	// QtSelectionIndividualPropertiesPanel::setSelectionManager


QtEntityPropertiesItem* QtSelectionIndividualPropertiesPanel::findEntityItem (
									const Entity& entity) const
{
	CHECK_NULL_PTR_ERROR (_entitiesWidget)
	const size_t	count	= _entitiesWidget->topLevelItemCount ( );
	for (size_t i = 0; i < count; i++)
	{
		QtEntityPropertiesItem*	item	=
				dynamic_cast<QtEntityPropertiesItem*>(_entitiesWidget->topLevelItem (i));
		if ((0 != item) && (&entity == &(item->getEntity ( ))))
			return item;
	}	// for (size_t i = 0; i < count; i++)

	return 0;
}	// QtSelectionIndividualPropertiesPanel::findEntityItem


size_t QtSelectionIndividualPropertiesPanel::findEntityItemIndex (const Entity& entity) const
{
	CHECK_NULL_PTR_ERROR (_entitiesWidget)
	const size_t	count	= _entitiesWidget->topLevelItemCount ( );
	for (size_t i = 0; i < count; i++)
	{
		QtEntityPropertiesItem*	item	=
				dynamic_cast<QtEntityPropertiesItem*>(_entitiesWidget->topLevelItem (i));
		if ((0 != item) && (&entity == &(item->getEntity ( ))))
			return i;
	}	// for (size_t i = 0; i < count; i++)

	return (size_t)-1;
}	// QtSelectionIndividualPropertiesPanel::findEntityItemIndex


void QtSelectionIndividualPropertiesPanel::entitiesAddedToSelection (
						const vector<Entity*>& entities)
{
	BEGIN_QT_TRY_CATCH_BLOCK

		if ((0 == getSelectionManager ( )) || (false == displayProperties ( )))
		return;

	CHECK_NULL_PTR_ERROR (_entitiesWidget)
	if (Resources::instance ( )._maxIndividualProperties.getValue ( ) <
															entities.size ( ))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Sélection courante : " << entities.size ( ) << " entités.";
		QTreeWidgetItem*	item	= new QTreeWidgetItem (_entitiesWidget);
		item->setText (0, UTF8TOQSTRING (message));
		_entitiesWidget->addTopLevelItem (item);
		return;
	}	// if (Resources::instance ( )._maxIndividualProperties.getValue ( ) < ...

	QtSelectionPropertiesTreeItem*	item	= 0;
	RenderingManager*		rm	= 0 == getGraphicalWidget ( ) ?
					0 : &getGraphicalWidget ( )->getRenderingManager ( );
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		CHECK_NULL_PTR_ERROR (*it)
		item	= new QtEntityPropertiesItem (
				_entitiesWidget, **it, displayComputedProperties ( ), rm);
		item->initialize ( );
		_entitiesWidget->addTopLevelItem (item);
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")
}	// QtSelectionIndividualPropertiesPanel::entitiesAddedToSelection


void QtSelectionIndividualPropertiesPanel::entitiesRemovedFromSelection (
						const vector<Entity*>& entities, bool clear)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 == getSelectionManager ( ))
		return;

	CHECK_NULL_PTR_ERROR (_entitiesWidget)
	if (false == clear)	
	{
		for (vector<Entity*>::const_iterator it = entities.begin ( );
		     entities.end ( ) != it; it++)
		{
			CHECK_NULL_PTR_ERROR (*it)
			const size_t index	= findEntityItemIndex (**it);
			if ((size_t)-1 != index)
				_entitiesWidget->takeTopLevelItem (index);
		}	// for (vector<Entity*>::const_iterator it = entities.begin ( );
	}	// if (false == clear)
	else	// => optimisation
	{
		_entitiesWidget->clear ( );
		_entitiesWidget->setColumnCount (2);
	}	// else 

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")
}	// QtSelectionIndividualPropertiesPanel::entitiesRemovedFromSelection


Qt3DGraphicalWidget* QtSelectionIndividualPropertiesPanel::getGraphicalWidget( )
{
	return _graphicalWidget;
}	// QtSelectionIndividualPropertiesPanel::getGraphicalWidget


void QtSelectionIndividualPropertiesPanel::setGraphicalWidget (Qt3DGraphicalWidget* widget3D)
{
	_graphicalWidget	= widget3D;
	if (0 != _entitiesWidget)
		_entitiesWidget->setRenderingManager (
				0 == _graphicalWidget ? 0 : &_graphicalWidget->getRenderingManager ( ));
}	// QtSelectionIndividualPropertiesPanel::setGraphicalWidget


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
