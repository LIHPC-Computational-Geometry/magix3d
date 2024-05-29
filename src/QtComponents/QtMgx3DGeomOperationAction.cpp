/** 
 * \file		QtMgx3DGeomOperationAction.hcpp
 * \author		Charles PIGNEROL 
 * \date		11/12/2012 
 */ 

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "Utils/Common.h"

using namespace Mgx3D::Utils;


namespace Mgx3D 
{ 
 
namespace QtComponents 
{ 


QtMgx3DGeomOperationAction::QtMgx3DGeomOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
    : QtMgx3DOperationAction (icon, text, mainWindow, tooltip)
{
}   // QtMgx3DGeomOperationAction::QtMgx3DGeomOperationAction


QtMgx3DGeomOperationAction::QtMgx3DGeomOperationAction (const QtMgx3DGeomOperationAction&)
	: QtMgx3DOperationAction (
		QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMgx3DGeomOperationAction copy constructor is not allowed.")
}	// QtMgx3DGeomOperationAction::QtMgx3DGeomOperationAction


QtMgx3DGeomOperationAction& QtMgx3DGeomOperationAction::operator = (const QtMgx3DGeomOperationAction&)
{
	MGX_FORBIDDEN ("QtMgx3DGeomOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMgx3DGeomOperationAction::operator =


QtMgx3DGeomOperationAction::~QtMgx3DGeomOperationAction ( )
{
}	// QtMgx3DGeomOperationAction::~QtMgx3DGeomOperationAction


}	// namespace QtComponents

}	// namespace Mgx3D
