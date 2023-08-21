/** 
 * \file		QtMgx3DMeshOperationAction.cpp
 * \author		Charles PIGNEROL 
 * \date		15/01/2013 
 */ 

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DMeshOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "Utils/Common.h"

using namespace Mgx3D::Utils;


namespace Mgx3D 
{ 
 
namespace QtComponents 
{ 


QtMgx3DMeshOperationAction::QtMgx3DMeshOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
    : QtMgx3DOperationAction (icon, text, mainWindow, tooltip)
{
}   // QtMgx3DMeshOperationAction::QtMgx3DMeshOperationAction


QtMgx3DMeshOperationAction::QtMgx3DMeshOperationAction (const QtMgx3DMeshOperationAction&)
	: QtMgx3DOperationAction (
		QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMgx3DMeshOperationAction copy constructor is not allowed.")
}	// QtMgx3DMeshOperationAction::QtMgx3DMeshOperationAction


QtMgx3DMeshOperationAction& QtMgx3DMeshOperationAction::operator = (const QtMgx3DMeshOperationAction&)
{
	MGX_FORBIDDEN ("QtMgx3DMeshOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMgx3DMeshOperationAction::operator =


QtMgx3DMeshOperationAction::~QtMgx3DMeshOperationAction ( )
{
}	// QtMgx3DMeshOperationAction::~QtMgx3DMeshOperationAction


}	// namespace QtComponents

}	// namespace Mgx3D
