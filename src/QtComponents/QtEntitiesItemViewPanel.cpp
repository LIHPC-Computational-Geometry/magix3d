/**
 * \file        QtEntitiesItemViewPanel.cpp
 * \author      Charles PIGNEROL
 * \date        07/02/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Internal/EntitiesHelper.h"
#include "QtComponents/QtEntitiesItemViewPanel.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtRepresentationTypesDialog.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ThreeStates.h>
#include <QtUtil/QtConfiguration.h>

#include <QComboBox>
#include <QThread>

#include <assert.h>
#include <algorithm>
#include <memory>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================

// ===========================================================================
//                        LA CLASSE QtEntitiesItemViewPanel
// ===========================================================================

QtEntitiesItemViewPanel::QtEntitiesItemViewPanel (
	QWidget* parent, QtMgx3DMainWindow* mainWindow, const string& name,
	ContextIfc& context)
	: QtGroupBox (name.c_str ( ), parent),
	  _mainWindow (mainWindow), _context (&context), _graphicalWidget (0),
	  _logStream (0)
{
}	// QtEntitiesItemViewPanel::QtEntitiesItemViewPanel


QtEntitiesItemViewPanel::QtEntitiesItemViewPanel (const QtEntitiesItemViewPanel&)
	: QtGroupBox (0),
	  _context(0), _mainWindow (0), _graphicalWidget (0), _logStream (0)
{
	MGX_FORBIDDEN ("QtEntitiesItemViewPanel copy constructor is not allowed.");
}	// QtEntitiesItemViewPanel::QtEntitiesItemViewPanel (const QtEntitiesItemViewPanel&)


QtEntitiesItemViewPanel& QtEntitiesItemViewPanel::operator = (const QtEntitiesItemViewPanel&)
{
	MGX_FORBIDDEN ("QtEntitiesItemViewPanel assignment operator is not allowed.");
	return *this;
}	// QtEntitiesItemViewPanel::QtEntitiesItemViewPanel (const QtEntitiesItemViewPanel&)


QtEntitiesItemViewPanel::~QtEntitiesItemViewPanel ( )
{
}	// QtEntitiesItemViewPanel::~QtEntitiesItemViewPanel


const string& QtEntitiesItemViewPanel::getAppTitle ( ) const
{
	return getMainWindow ( ).getAppTitle ( );
}	// QtEntitiesItemViewPanel::getAppTitle


const QtMgx3DMainWindow& QtEntitiesItemViewPanel::getMainWindow ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtEntitiesItemViewPanel::getMainWindow


QtMgx3DMainWindow& QtEntitiesItemViewPanel::getMainWindow ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtEntitiesItemViewPanel::getMainWindow


void QtEntitiesItemViewPanel::setMainWindow (QtMgx3DMainWindow* mainWindow)
{
	_mainWindow	= mainWindow;
}	// QtEntitiesItemViewPanel::setMainWindow


Mgx3D::Internal::ContextIfc& QtEntitiesItemViewPanel::getContext ( )
{
	if (0 == _context)
		throw Exception (UTF8String ("QtEntitiesItemViewPanel::getContext : absence de contexte.", Charset::UTF_8));

	return *_context;
}	// QtEntitiesItemViewPanel::getContext


void QtEntitiesItemViewPanel::setContext (Mgx3D::Internal::ContextIfc* context)
{
	if (_context != context)
	{
		_context	= context;
	}	// if (_context != context)
}	// QtEntitiesItemViewPanel::setContext


void QtEntitiesItemViewPanel::setGraphicalWidget (Qt3DGraphicalWidget* widget3D)
{	
	if ((0 != widget3D) && (widget3D != _graphicalWidget) &&
	    (0 != _graphicalWidget))
		throw Exception (UTF8String ("QtEntitiesItemViewPanel::setGraphicalWidget : widget déjà affecté.", Charset::UTF_8));

	_graphicalWidget	= widget3D;
}	// QtEntitiesItemViewPanel::setGraphicalWidget


Qt3DGraphicalWidget* QtEntitiesItemViewPanel::getGraphicalWidget ( )
{
	return _graphicalWidget;
}	// QtEntitiesItemViewPanel::getGraphicalWidget


const Qt3DGraphicalWidget* QtEntitiesItemViewPanel::getGraphicalWidget ( ) const
{
	return _graphicalWidget;
}	// QtEntitiesItemViewPanel::getGraphicalWidget


void QtEntitiesItemViewPanel::setLogStream (LogOutputStream* stream)
{
	_logStream	= stream;
}	// QtEntitiesItemViewPanel::setLogStream


void QtEntitiesItemViewPanel::log (const Log& l)
{
	if (0 != _logStream)
		_logStream->log (l);
}	// QtEntitiesItemViewPanel::log


void QtEntitiesItemViewPanel::changeRepresentationTypes (const vector<Entity*>& entities)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	getMainWindow ( ).changeRepresentationTypes (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesItemViewPanel::changeRepresentationTypes


}	// namespace QtComponents

}	// namespace Mgx3D
