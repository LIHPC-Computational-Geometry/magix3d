/**
 * \file		QtCurvilinearAbscissaPanel.cpp
 * \author		Charles PIGNEROL
 * \date		09/09/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtCurvilinearAbscissaPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Utils/MgxNumeric.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <assert.h>
#include <memory>

#include <QBoxLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ============================================================================
//                          LA CLASSE QtCurvilinearAbscissaPanel
// ============================================================================


QtCurvilinearAbscissaPanel::QtCurvilinearAbscissaPanel (
			QWidget* parent, const string& title, QtMgx3DMainWindow* mainWindow)
	: QWidget (parent), 
	  _appName (title), _curvePanel (0), _curvilinearTextField (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	setContentsMargins (0, 0, 0, 0);
	layout->setContentsMargins (0, 0, 0, 0);

	// La courbe :
	_curvePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Courbe :", "", mainWindow, SelectionManagerIfc::D1,
			FilterEntity::GeomCurve);
	layout->addWidget (_curvePanel);

	// L'abscisse curviligne :
	QHBoxLayout*	hLayout	= new QHBoxLayout (0);
	layout->addLayout (hLayout);
	QLabel*	label	= new QLabel ("Abscisse curviligne :", this);
	hLayout->addWidget (label);
	_curvilinearTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_curvilinearTextField->setValue (0.5);
	hLayout->addWidget (_curvilinearTextField);
	hLayout->addStretch (2);
}	// QtCurvilinearAbscissaPanel::QtCurvilinearAbscissaPanel


QtCurvilinearAbscissaPanel::QtCurvilinearAbscissaPanel (
											const QtCurvilinearAbscissaPanel&)
	: QWidget (0), _appName ( ), _curvePanel (0), _curvilinearTextField (0)
{
	assert (0 && "QtCurvilinearAbscissaPanel copy constructor is forbidden.");
}	// QtCurvilinearAbscissaPanel::QtCurvilinearAbscissaPanel (const QtCurvilinearAbscissaPanel&)


QtCurvilinearAbscissaPanel& QtCurvilinearAbscissaPanel::operator = (
											const QtCurvilinearAbscissaPanel&)
{
	assert (0 && "QtCurvilinearAbscissaPanel assignment operator is forbidden.");
	return *this;
}	// QtCurvilinearAbscissaPanel::operator =


QtCurvilinearAbscissaPanel::~QtCurvilinearAbscissaPanel ( )
{
}	// QtCurvilinearAbscissaPanel::~QtCurvilinearAbscissaPanel


void QtCurvilinearAbscissaPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_curvePanel)
	_curvePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtCurvilinearAbscissaPanel::reset


string QtCurvilinearAbscissaPanel::getCurveUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return _curvePanel->getUniqueName ( );
}	// QtCurvilinearAbscissaPanel::getCurveUniqueName


void QtCurvilinearAbscissaPanel::setCurveUniqueName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	_curvePanel->setUniqueName (name);
}	// QtCurvilinearAbscissaPanel::setCurveUniqueName


double QtCurvilinearAbscissaPanel::getCurvilinearAbscissa ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvilinearTextField)
	return _curvilinearTextField->getValue ( );
}	// QtCurvilinearAbscissaPanel::getCurvilinearAbscissa


QtMgx3DEntityPanel& QtCurvilinearAbscissaPanel::getCurvePanel ( )
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return *_curvePanel;
}	// QtCurvilinearAbscissaPanel::getCurvePanel


QtDoubleTextField& QtCurvilinearAbscissaPanel::getCurvilinearAbscissaPanel ( )
{
	CHECK_NULL_PTR_ERROR (_curvilinearTextField)
	return *_curvilinearTextField;
}

void QtCurvilinearAbscissaPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	_curvePanel->stopSelection ( );
}	// QtCurvilinearAbscissaPanel::stopSelection



}	// namespace QtComponents

}	// namespace Mgx3D

