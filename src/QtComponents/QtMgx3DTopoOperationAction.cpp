/** 
 * \file		QtMgx3DTopoOperationAction.hcpp
 * \author		Charles PIGNEROL 
 * \date		15/01/2013 
 */ 

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "Utils/Common.h"

using namespace Mgx3D::Utils;


namespace Mgx3D 
{ 
 
namespace QtComponents 
{ 


QtMgx3DTopoOperationAction::QtMgx3DTopoOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
    : QtMgx3DOperationAction (icon, text, mainWindow, tooltip)
{
}   // QtMgx3DTopoOperationAction::QtMgx3DTopoOperationAction


QtMgx3DTopoOperationAction::QtMgx3DTopoOperationAction (const QtMgx3DTopoOperationAction&)
	: QtMgx3DOperationAction (
		QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMgx3DTopoOperationAction copy constructor is not allowed.")
}	// QtMgx3DTopoOperationAction::QtMgx3DTopoOperationAction


QtMgx3DTopoOperationAction& QtMgx3DTopoOperationAction::operator = (const QtMgx3DTopoOperationAction&)
{
	MGX_FORBIDDEN ("QtMgx3DTopoOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMgx3DTopoOperationAction::operator =


QtMgx3DTopoOperationAction::~QtMgx3DTopoOperationAction ( )
{
}	// QtMgx3DTopoOperationAction::~QtMgx3DTopoOperationAction


}	// namespace QtComponents

}	// namespace Mgx3D
