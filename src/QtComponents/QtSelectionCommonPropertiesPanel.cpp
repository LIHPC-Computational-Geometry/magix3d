/**
 * \file        QtSelectionCommonPropertiesPanel.cpp
 * \author      Charles PIGNEROL
 * \date        07/10/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"
#include "Utils/MgxNumeric.h"
#include "QtComponents/QtSelectionCommonPropertiesPanel.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/NumericConversions.h>
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


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                         LA CLASSE QtCommonPropertiesItem
// ===========================================================================

QtCommonPropertiesItem::QtCommonPropertiesItem (
		QTreeWidget* parent, const vector<Entity*>& entities, bool alsoComputed)
	: QtSelectionPropertiesTreeItem (parent, 0, alsoComputed, 0),
	  _entities (entities)
{
	setText (0, "Propriétés communes");
}	// QtCommonPropertiesItem::QtCommonPropertiesItem


QtCommonPropertiesItem::QtCommonPropertiesItem (const QtCommonPropertiesItem&)
	: QtSelectionPropertiesTreeItem (
				(QTreeWidget*)0, new SerializedRepresentation ( ), false, 0),
	  _entities ( )
{
	MGX_FORBIDDEN ("QtCommonPropertiesItem copy constructor is not allowed.");
}	// QtCommonPropertiesItem::QtCommonPropertiesItem


QtCommonPropertiesItem& QtCommonPropertiesItem::operator = (
												const QtCommonPropertiesItem&)
{
	MGX_FORBIDDEN ("QtCommonPropertiesItem assignment operator is not allowed.");
	return *this;
}	// QtCommonPropertiesItem::operator =


QtCommonPropertiesItem::~QtCommonPropertiesItem ()
{
}	// QtCommonPropertiesItem::~QtCommonPropertiesItem


void QtCommonPropertiesItem::initialize ( )
{
	try
	{
		QtSelectionPropertiesTreeItem::initialize ( );
	}
	catch (...)
	{
	}
}	// QtCommonPropertiesItem::initialize


const vector<Entity*>& QtCommonPropertiesItem::getEntities ( ) const
{
	return _entities;
}	// QtCommonPropertiesItem::getEntities


void QtCommonPropertiesItem::updateDescription ( )
{
	const vector<Entity*>&					entities	= getEntities ( );
	vector<const SerializedRepresentation*>	representations;
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
		representations.push_back ((*it)->getDescription (
												displayComputedProperties ( )));
	unique_ptr<SerializedRepresentation>	commonRep (
			SerializedRepresentation::merge (
							"Propriétés communes", representations, false));
	setDescription (commonRep.release ( ));

	for (vector<const SerializedRepresentation*>::iterator it =
		representations.begin ( ); representations.end ( ) != it; it++)
		delete *it;
}	// QtCommonPropertiesItem::updateDescription


// ===========================================================================
//                     LA CLASSE QtSelectionCommonPropertiesTreeWidget
// ===========================================================================

QtSelectionCommonPropertiesTreeWidget::QtSelectionCommonPropertiesTreeWidget (
															QWidget* parent)
	: QTreeWidget (parent)
{
	setSelectionBehavior (QTreeWidget::SelectRows);
	setSelectionMode (QTreeWidget::ExtendedSelection);
	setRootIsDecorated (true);
	setSortingEnabled (false);
	setColumnCount (2);
	connect (this, SIGNAL (itemExpanded(QTreeWidgetItem*)), this,
	         SLOT (itemExpandedCallback(QTreeWidgetItem*)));
}	// QtSelectionCommonPropertiesTreeWidget::QtSelectionCommonPropertiesTreeWidget


QtSelectionCommonPropertiesTreeWidget::QtSelectionCommonPropertiesTreeWidget (
								const QtSelectionCommonPropertiesTreeWidget&)
	: QTreeWidget (0)
{
	MGX_FORBIDDEN ("QtSelectionCommonPropertiesTreeWidget copy constructor is not allowed.");
}	// QtSelectionCommonPropertiesTreeWidget::QtSelectionCommonPropertiesTreeWidget


QtSelectionCommonPropertiesTreeWidget&
			QtSelectionCommonPropertiesTreeWidget::operator = (
								const QtSelectionCommonPropertiesTreeWidget&)
{
	MGX_FORBIDDEN ("QtSelectionCommonPropertiesTreeWidget assignment operator is not allowed.");
	return *this;
}	// QtSelectionCommonPropertiesTreeWidget::QtSelectionCommonPropertiesTreeWidget


QtSelectionCommonPropertiesTreeWidget::~QtSelectionCommonPropertiesTreeWidget ( )
{
}	// QtSelectionCommonPropertiesTreeWidget::~QtSelectionCommonPropertiesTreeWidget


void QtSelectionCommonPropertiesTreeWidget::itemExpandedCallback (
													QTreeWidgetItem* item)
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
		QtCommonPropertiesItem*	cpi	=
									dynamic_cast<QtCommonPropertiesItem*>(item);
		if (0 != cpi)
		{
			cpi->takeChildren ( );
			cpi->updateDescription ( );
		}	// if (0 != cpi)
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
}	// QtSelectionCommonPropertiesTreeWidget::itemExpandedCallback


vector<QTreeWidgetItem*>
		QtSelectionCommonPropertiesTreeWidget::filteredSelectionList(
													const QItemSelection& list)
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
//                        LA CLASSE QtSelectionCommonPropertiesPanel
// ===========================================================================

QtSelectionCommonPropertiesPanel::QtSelectionCommonPropertiesPanel (
			QWidget* parent, const string& name, SelectionManagerIfc* manager)
	: QtGroupBox (name.c_str ( ), parent), SelectionManagerObserver (manager),
	  _boundingBoxWidget (0), _entitiesWidget (0),
	  _displayProperties (true), _displayComputedProperties (true)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);


	// La boite englobante.
	_boundingBoxWidget	= new QTableWidget (this);
	layout->addWidget (_boundingBoxWidget);
	_boundingBoxWidget->setRowCount (1);
	_boundingBoxWidget->setColumnCount (6);
	for (size_t i = 0; i < 6; i++)
		_boundingBoxWidget->setItem (0, i, new QTableWidgetItem ( ));
	QStringList	boundingBoxLabels;
	boundingBoxLabels
				<< "Xmin" << "Xmax" << "Ymin" << "Ymax" << "Zmin" << "Zmax";
	_boundingBoxWidget->setHorizontalHeaderLabels (boundingBoxLabels);
	_boundingBoxWidget->resizeColumnsToContents ( );
	_boundingBoxWidget->setFixedHeight (
					_boundingBoxWidget->rowHeight (0) +
					_boundingBoxWidget->horizontalHeader ( )->height ( ) +
					_boundingBoxWidget->horizontalScrollBar ( )->height ( ));

	// Les entités sélectionnées.
	_entitiesWidget	= new QtSelectionCommonPropertiesTreeWidget (this);
	layout->addWidget (_entitiesWidget);
}	// QtSelectionCommonPropertiesPanel::QtSelectionCommonPropertiesPanel


QtSelectionCommonPropertiesPanel::QtSelectionCommonPropertiesPanel (
									const QtSelectionCommonPropertiesPanel& gp)
	: QtGroupBox ("Invalid panel", 0), SelectionManagerObserver (0),
	  _boundingBoxWidget (0), _entitiesWidget (0),
	  _displayProperties (true), _displayComputedProperties (true)
{
	MGX_FORBIDDEN ("QtSelectionCommonPropertiesPanel copy constructor is not allowed.");
}	// QtSelectionCommonPropertiesPanel::QtSelectionCommonPropertiesPanel (const QtSelectionCommonPropertiesPanel&)


QtSelectionCommonPropertiesPanel& QtSelectionCommonPropertiesPanel::operator = (
										const QtSelectionCommonPropertiesPanel&)
{
	MGX_FORBIDDEN ("QtSelectionCommonPropertiesPanel assignment operator is not allowed.");
	return *this;
}	// QtSelectionCommonPropertiesPanel::QtSelectionCommonPropertiesPanel (const QtSelectionCommonPropertiesPanel&)


QtSelectionCommonPropertiesPanel::~QtSelectionCommonPropertiesPanel ( )
{
}	// QtSelectionCommonPropertiesPanel::~QtSelectionCommonPropertiesPanel


void QtSelectionCommonPropertiesPanel::writeSettings (QSettings& settings)
{
	settings.beginGroup ("SelectionCommonPropertiesPanel");
	settings.setValue ("size", size ( ));
	settings.endGroup ( );
}	// QtSelectionCommonPropertiesPanel::writeSettings


void QtSelectionCommonPropertiesPanel::readSettings (QSettings& settings)
{
	settings.beginGroup ("SelectionCommonPropertiesPanel");
	resize (settings.value ("size", size ( )).toSize ( ));
	settings.endGroup ( );
}	// QtSelectionCommonPropertiesPanel::readSettings


bool QtSelectionCommonPropertiesPanel::displayProperties ( ) const
{
	return _displayProperties;
}	// QtSelectionCommonPropertiesPanel::displayProperties


void QtSelectionCommonPropertiesPanel::displayProperties (bool display)
{
	if (display == _displayProperties)
		return;

	BEGIN_QT_TRY_CATCH_BLOCK

	_displayProperties	= display;

	if ((false == _displayProperties) || (false == _displayComputedProperties))
	{
		CHECK_NULL_PTR_ERROR (_boundingBoxWidget)
		for (int i = 0; i < 6; i++)
			_boundingBoxWidget->item (0, i)->setText ("");
		_boundingBoxWidget->resizeColumnsToContents ( );

		CHECK_NULL_PTR_ERROR (_entitiesWidget)
		_entitiesWidget->clear ( );
		_entitiesWidget->setColumnCount (2);
	}	// if ((false == _displayProperties) || ...
	else
		selectionModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtSelectionCommonPropertiesPanel::displayProperties


bool QtSelectionCommonPropertiesPanel::displayComputedProperties ( ) const
{
	return _displayComputedProperties;
}	// QtSelectionCommonPropertiesPanel::displayComputedProperties


void QtSelectionCommonPropertiesPanel::displayComputedProperties (bool display)
{
	if (display == _displayComputedProperties)
		return;

	BEGIN_QT_TRY_CATCH_BLOCK

	_displayComputedProperties	= display;

	if ((false == _displayProperties) || (false == _displayComputedProperties))
	{
		CHECK_NULL_PTR_ERROR (_boundingBoxWidget)
		for (int i = 0; i < 6; i++)
			_boundingBoxWidget->item (0, i)->setText ("");
		_boundingBoxWidget->resizeColumnsToContents ( );

		CHECK_NULL_PTR_ERROR (_entitiesWidget)
		_entitiesWidget->clear ( );
		_entitiesWidget->setColumnCount (2);
	}	// if ((false == _displayProperties) || ...
	else
		selectionModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtSelectionCommonPropertiesPanel::displayComputedProperties


void QtSelectionCommonPropertiesPanel::setSelectionManager (
										SelectionManagerIfc* selectionManager)
{
	if (selectionManager == getSelectionManager ( ))
		return;

	SelectionManagerObserver::setSelectionManager (selectionManager);
	selectionModified ( );
}	// QtSelectionCommonPropertiesPanel::setSelectionManager


void QtSelectionCommonPropertiesPanel::selectionModified ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if ((0 == getSelectionManager ( )) || (false == displayProperties ( )))
		return;

	// La boite englobante :
	if (true == displayComputedProperties ( ))
	{
		size_t	i	= 0;
		CHECK_NULL_PTR_ERROR (_boundingBoxWidget)
		double	bounds [] = {
			NumericServices::doubleMachMax(),-NumericServices::doubleMachMax(),
			NumericServices::doubleMachMax(),-NumericServices::doubleMachMax(),
			NumericServices::doubleMachMax(),-NumericServices::doubleMachMax()};
		getSelectionManager ( )->getBounds (bounds);
		for (i = 0; i < 6; i++)
		{
			string	num ("NaN");
			if (true == NumericServices::isValidAndNotMax (bounds [i]))
				num	= MgxNumeric::userRepresentation (bounds [i]);
			_boundingBoxWidget->item (0, i)->setText (num.c_str ( ));
		}	// for (i = 0; i < 6; i++)
		_boundingBoxWidget->resizeColumnsToContents ( );
	}	// if (true == displayComputedProperties ( ))

	// Les entités sélectionnées :
	CHECK_NULL_PTR_ERROR (_entitiesWidget)
	_entitiesWidget->clear ( );
	_entitiesWidget->setColumnCount (2);
	
	const vector<Entity*>	entities = getSelectionManager ( )->getEntities ( );
	if (0 == entities.size ( ))
		return;
	if (Resources::instance ( )._maxCommonProperties.getValue( ) < entities.size( ))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Sélection courante : " << entities.size ( ) << " entités.";
		QTreeWidgetItem*	item	= new QTreeWidgetItem (_entitiesWidget);
		item->setText (0, UTF8TOQSTRING (message));
		_entitiesWidget->addTopLevelItem (item);
		return;
	}	// if (Resources::instance ( )._maxCommonProperties.getValue ( ) < ...

	QtCommonPropertiesItem*	item	=
		new QtCommonPropertiesItem (
					_entitiesWidget, entities, displayComputedProperties ( ));
	_entitiesWidget->addTopLevelItem (item);

	_entitiesWidget->resizeColumnToContents (0);
	_entitiesWidget->resizeColumnToContents (1);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtSelectionCommonPropertiesPanel::selectionModified


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
