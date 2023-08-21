/**
 * \file		QtEntityByDimensionSelectorPanel.cpp
 * \author		Charles PIGNEROL
 * \date		05/12/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/MemoryError.h>

#include <QHBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{


QtEntityByDimensionSelectorPanel::QtEntityByDimensionSelectorPanel (
	QWidget* parent, QtMgx3DMainWindow& mainWindow, const string& label,
	SelectionManagerIfc::DIM allowedDimensions, FilterEntity::objectType types,
	SelectionManagerIfc::DIM defaultDimensions, bool allowMultipleDimensions)
	: QWidget (parent),
	  _dimensionsPanel (0), _entitiesPanel (0), _mainWindow (&mainWindow)
{
	setContentsMargins (0, 0, 0, 0);
	QHBoxLayout*	layout	= new QHBoxLayout (this);
	layout->setMargin (0);

	// La dimension filtrante :
	_dimensionsPanel	= new QtDimensionsSelectorPanel (
		this, allowedDimensions, defaultDimensions, allowMultipleDimensions);
	layout->addWidget (_dimensionsPanel);
	connect (_dimensionsPanel,  SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));

	// La saisie des entités :
	_entitiesPanel	= new QtMgx3DEntityPanel (
			this, "", true, label, "", &mainWindow, allowedDimensions, types);
	_entitiesPanel->setFilteredDimensions (_dimensionsPanel->getDimensions ( ));
	layout->addWidget (_entitiesPanel);
	connect (_entitiesPanel->getNameTextField ( ), 
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (selectionModifiedCallback (QString)));
	connect (_entitiesPanel->getNameTextField ( ), 
	         SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_entitiesPanel->getNameTextField ( ), 
	         SIGNAL (entitiesRemovedFromSelection (QString)), this,
	         SLOT (entitiesRemovedFromSelectionCallback (QString)));

	layout->addStretch (2);	// L'espace en trop est ajouté à la fin.

	setLayout (layout);
}	// QtEntityByDimensionSelectorPanel::QtEntityByDimensionSelectorPanel


QtEntityByDimensionSelectorPanel::QtEntityByDimensionSelectorPanel (
										const QtEntityByDimensionSelectorPanel&)
	: QWidget (0),
	  _dimensionsPanel (0), _entitiesPanel (0), _mainWindow (0)
{
	MGX_FORBIDDEN ("QtEntityByDimensionSelectorPanel copy constructor is not allowed.");
}	// QtEntityByDimensionSelectorPanel::QtEntityByDimensionSelectorPanel (const QtEntityByDimensionSelectorPanel&)


QtEntityByDimensionSelectorPanel& QtEntityByDimensionSelectorPanel::operator = (
										const QtEntityByDimensionSelectorPanel&)
{
	MGX_FORBIDDEN ("QtEntityByDimensionSelectorPanel assignment operator is not allowed.");
	return *this;
}	// QtEntityByDimensionSelectorPanel::QtEntityByDimensionSelectorPanel (const QtEntityByDimensionSelectorPanel&)


QtEntityByDimensionSelectorPanel::~QtEntityByDimensionSelectorPanel ( )
{
}	// QtEntityByDimensionSelectorPanel::~QtEntityByDimensionSelectorPanel


vector<string> QtEntityByDimensionSelectorPanel::getEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	return _entitiesPanel->getUniqueNames ( );
}	// QtEntityByDimensionSelectorPanel::getEntitiesNames


void QtEntityByDimensionSelectorPanel::setEntitiesNames (
											const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_entitiesPanel->setUniqueNames (names);
}	// QtEntityByDimensionSelectorPanel::setEntitiesNames


QtMgx3DEntityPanel* QtEntityByDimensionSelectorPanel::getEntitiesPanel ( )
{
	return _entitiesPanel;
}	// QtEntityByDimensionSelectorPanel::getEntitiesPanel


void QtEntityByDimensionSelectorPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_entitiesPanel)
		_entitiesPanel->reset ( );
	}
	catch (...)
	{
	}
}	// QtEntityByDimensionSelectorPanel::reset


void QtEntityByDimensionSelectorPanel::clearSelection ( )
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_entitiesPanel->clearSelection ( );
}	// QtEntityByDimensionSelectorPanel::clearSelection


void QtEntityByDimensionSelectorPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_entitiesPanel->stopSelection ( );
}	// QtEntityByDimensionSelectorPanel::stopSelection


bool QtEntityByDimensionSelectorPanel::actualizeGui (bool removeDestroyed)
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	return _entitiesPanel->actualizeGui (removeDestroyed);
}	// QtEntityByDimensionSelectorPanel::actualizeGui


SelectionManagerIfc::DIM
					QtEntityByDimensionSelectorPanel::getDimensions ( ) const
{
	CHECK_NULL_PTR_ERROR (_dimensionsPanel)
	return _dimensionsPanel->getDimensions ( );
}	// QtEntityByDimensionSelectorPanel::getDimensions


void QtEntityByDimensionSelectorPanel::setDimensions (
												SelectionManagerIfc::DIM dims)
{
	CHECK_NULL_PTR_ERROR (_dimensionsPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	if (_dimensionsPanel->getDimensions ( ) != dims)
	{
		_dimensionsPanel->setDimensions (dims);
		if (false == _dimensionsPanel->allowMultipleDimensions ( ))
			clearSelection ( );
		_entitiesPanel->setFilteredDimensions (
										_dimensionsPanel->getDimensions ( ));
		dimensionsModifiedCallback ( );
	}	// if (_dimensionsPanel->getDimensions ( ) != dims)
}	// QtEntityByDimensionSelectorPanel::setDimensions


SelectionManagerIfc::DIM
				QtEntityByDimensionSelectorPanel::getAllowedDimensions ( ) const
{
	CHECK_NULL_PTR_ERROR (_dimensionsPanel)
	return _dimensionsPanel->getAllowedDimensions ( );
}	// QtEntityByDimensionSelectorPanel::getAllowedDimensions


void QtEntityByDimensionSelectorPanel::setAllowedDimensions (
												SelectionManagerIfc::DIM dims)
{
	CHECK_NULL_PTR_ERROR (_dimensionsPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_dimensionsPanel->setAllowedDimensions (dims);
}	// QtEntityByDimensionSelectorPanel::setAllowedDimensions


FilterEntity::objectType QtEntityByDimensionSelectorPanel::getAllowedTypes ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	return _entitiesPanel->getNameTextField ( )->getFilteredTypes ( );
}	// QtEntityByDimensionSelectorPanel::getAllowedTypes


bool QtEntityByDimensionSelectorPanel::isMultiSelectMode ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	return _entitiesPanel->isMultiSelectMode ( );
}	// QtEntityByDimensionSelectorPanel::isMultiSelectMode


void QtEntityByDimensionSelectorPanel::setMultiSelectMode (bool multi)
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_entitiesPanel->setMultiSelectMode (multi);
}	// QtEntityByDimensionSelectorPanel::setMultiSelectMode


void QtEntityByDimensionSelectorPanel::dimensionsModifiedCallback ( )
{
	try
	{
		getMainWindow ( );
		getMainWindow ( ).getGraphicalWidget ( ).getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_dimensionsPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	if (false == _dimensionsPanel->allowMultipleDimensions ( ))
		clearSelection ( );
	_entitiesPanel->setFilteredDimensions (_dimensionsPanel->getDimensions ( ));
	emit dimensionsModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtEntityByDimensionSelectorPanel::dimensionsModifiedCallback


void QtEntityByDimensionSelectorPanel::selectionModifiedCallback (
															QString selection)
{
	emit selectionModified (selection);
}	// QtEntityByDimensionSelectorPanel::selectionModifiedCallback


void QtEntityByDimensionSelectorPanel::entitiesAddedToSelectionCallback (
														QString entitiesNames)
{
	emit entitiesAddedToSelection (entitiesNames);
}	// QtEntityByDimensionSelectorPanel::entitiesAddedToSelectionCallback


void QtEntityByDimensionSelectorPanel::entitiesRemovedFromSelectionCallback (
														QString entitiesNames)
{
	emit entitiesRemovedFromSelection (entitiesNames);
}	// QtEntityByDimensionSelectorPanel::entitiesRemovedFromSelectionCallback


QtMgx3DMainWindow& QtEntityByDimensionSelectorPanel::getMainWindow ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtEntityByDimensionSelectorPanel::getMainWindow


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================

}	// namespace QtComponents

}	// namespace Mgx3D
