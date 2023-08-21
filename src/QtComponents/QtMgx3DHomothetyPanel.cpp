/**
 * \file        QtMgx3DHomothetyPanel.cpp
 * \author      Charles PIGNEROL
 * \date        05/05/2017
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DHomothetyPanel.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                    LA CLASSE QtMgx3DHomogeneousHomothetyPanel
// ===========================================================================

QtMgx3DHomogeneousHomothetyPanel::QtMgx3DHomogeneousHomothetyPanel (
	QWidget* parent, const string& appTitle, QtMgx3DMainWindow& mainWindow)
	: QtMgx3DOperationsSubPanel (parent, mainWindow), _mainWindow (&mainWindow),
	  _homothetyFactorTextField (0), _centerPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);

	// Le rapport d'homothétie :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);	
	QLabel*	label	= new QLabel (QString::fromUtf8("Rapport d'homothétie :"), this);
	hlayout->addWidget (label);
	_homothetyFactorTextField = &QtNumericFieldsFactory::createDistanceTextField (this);
	_homothetyFactorTextField->setValue(1.0);
	hlayout->addWidget (_homothetyFactorTextField);

	// Le centre de l'homothétie :
	_centerPanel	= new QtMgx3DPointPanel (
			this, "Centre :", true, "x :", "y :", "z :",
			0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
			&mainWindow, (FilterEntity::objectType)(
					FilterEntity::GeomVertex |  FilterEntity::TopoVertex),
			true);
	layout->addWidget (_centerPanel);
}	// QtMgx3DHomogeneousHomothetyPanel::QtMgx3DHomogeneousHomothetyPanel


QtMgx3DHomogeneousHomothetyPanel::QtMgx3DHomogeneousHomothetyPanel (
									const QtMgx3DHomogeneousHomothetyPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _mainWindow (0), _homothetyFactorTextField (0), _centerPanel (0)
{
	MGX_FORBIDDEN ("QtMgx3DHomogeneousHomothetyPanel copy constructor is not allowed.");
}	// QtMgx3DHomogeneousHomothetyPanel::QtMgx3DHomogeneousHomothetyPanel


QtMgx3DHomogeneousHomothetyPanel& QtMgx3DHomogeneousHomothetyPanel::operator = (
										const QtMgx3DHomogeneousHomothetyPanel&)
{
	 MGX_FORBIDDEN ("QtMgx3DHomogeneousHomothetyPanel assignment operator is not allowed.");
	return *this;
}	// QtMgx3DHomogeneousHomothetyPanel::operator =


QtMgx3DHomogeneousHomothetyPanel::~QtMgx3DHomogeneousHomothetyPanel ( )
{
}	// QtMgx3DHomogeneousHomothetyPanel::~QtMgx3DHomogeneousHomothetyPanel


void QtMgx3DHomogeneousHomothetyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_homothetyFactorTextField)	
	CHECK_NULL_PTR_ERROR (_centerPanel)
	_homothetyFactorTextField->setValue (1.);
	_centerPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtMgx3DHomogeneousHomothetyPanel::reset


void QtMgx3DHomogeneousHomothetyPanel::cancel ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	_centerPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_centerPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))

	QtMgx3DOperationsSubPanel::cancel ( );
}	// QtMgx3DHomogeneousHomothetyPanel::cancel


void QtMgx3DHomogeneousHomothetyPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	QtMgx3DOperationsSubPanel::autoUpdate ( );
	_centerPanel->clearSelection ( );
	_centerPanel->actualizeGui (true);
}	// QtMgx3DHomogeneousHomothetyPanel::autoUpdate


void QtMgx3DHomogeneousHomothetyPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	_centerPanel->stopSelection ( );
}	// QtMgx3DHomogeneousHomothetyPanel::stopSelection


double QtMgx3DHomogeneousHomothetyPanel::getHomothetyFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_homothetyFactorTextField)
	return _homothetyFactorTextField->getValue ( );
}	// QtMgx3DHomogeneousHomothetyPanel::getHomothetyFactor


Math::Point QtMgx3DHomogeneousHomothetyPanel::getCenter ( ) const
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	return _centerPanel->getPoint ( );
}	// QtMgx3DHomogeneousHomothetyPanel::getCenter


QtDoubleTextField& QtMgx3DHomogeneousHomothetyPanel::getFactorTextField ( )
{
	CHECK_NULL_PTR_ERROR (_homothetyFactorTextField)
	return *_homothetyFactorTextField;
}	// QtMgx3DHomogeneousHomothetyPanel::getFactorTextField


QtMgx3DPointPanel& QtMgx3DHomogeneousHomothetyPanel::getCenterPanel ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	return *_centerPanel;
}	// QtMgx3DHomogeneousHomothetyPanel::getCenterPanel


vector<Entity*> QtMgx3DHomogeneousHomothetyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	CHECK_NULL_PTR_ERROR (_centerPanel)
	const string	centerName	= _centerPanel->getUniqueName ( );
	if (0 != centerName.length ( ))
	{
		try	
		{
			entities.push_back (&getContext().nameToEntity (centerName));
		}
		catch (...)
		{
		}
	}	// if (0 != centerName.length ( ))

	return entities;
}	// QtMgx3DHomogeneousHomothetyPanel::getInvolvedEntities


const ContextIfc& QtMgx3DHomogeneousHomothetyPanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DHomogeneousHomothetyPanel::getContext


// ===========================================================================
//                  LA CLASSE QtMgx3DHeterogeneousHomothetyPanel
// ===========================================================================

QtMgx3DHeterogeneousHomothetyPanel::QtMgx3DHeterogeneousHomothetyPanel (
	QWidget* parent, const string& appTitle, QtMgx3DMainWindow& mainWindow)
	: QtMgx3DOperationsSubPanel (parent, mainWindow), _mainWindow (&mainWindow),
	  _homothetyXFactorTextField (0), _homothetyYFactorTextField (0),
	  _homothetyZFactorTextField (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);

	// Le rapport d'homothétie selon Ox :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);	
	QLabel*	label	= new QLabel (QString::fromUtf8("Rapport d'homothétie Ox :"), this);
	hlayout->addWidget (label);
	_homothetyXFactorTextField	= new QtDoubleTextField (1., this);
	hlayout->addWidget (_homothetyXFactorTextField);

	// Le rapport d'homothétie selon Oy :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);	
	label	= new QLabel (QString::fromUtf8("Rapport d'homothétie Oy :"), this);
	hlayout->addWidget (label);
	_homothetyYFactorTextField	= new QtDoubleTextField (1., this);
	hlayout->addWidget (_homothetyYFactorTextField);

	// Le rapport d'homothétie selon Oz :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);	
	label	= new QLabel (QString::fromUtf8("Rapport d'homothétie Oz :"), this);
	hlayout->addWidget (label);
	_homothetyZFactorTextField	= new QtDoubleTextField (1., this);
	hlayout->addWidget (_homothetyZFactorTextField);
}	// QtMgx3DHeterogeneousHomothetyPanel::QtMgx3DHeterogeneousHomothetyPanel


QtMgx3DHeterogeneousHomothetyPanel::QtMgx3DHeterogeneousHomothetyPanel (
									const QtMgx3DHeterogeneousHomothetyPanel& p)
	: QtMgx3DOperationsSubPanel (p), _mainWindow (0),
	  _homothetyXFactorTextField (0), _homothetyYFactorTextField (0),
	  _homothetyZFactorTextField (0)
{
	MGX_FORBIDDEN ("QtMgx3DHeterogeneousHomothetyPanel copy constructor is not allowed.");
}	// QtMgx3DHeterogeneousHomothetyPanel::QtMgx3DHeterogeneousHomothetyPanel


QtMgx3DHeterogeneousHomothetyPanel& QtMgx3DHeterogeneousHomothetyPanel::operator = (
									const QtMgx3DHeterogeneousHomothetyPanel&)
{
	 MGX_FORBIDDEN ("QtMgx3DHeterogeneousHomothetyPanel assignment operator is not allowed.");
	return *this;
}	// QtMgx3DHeterogeneousHomothetyPanel::operator =


QtMgx3DHeterogeneousHomothetyPanel::~QtMgx3DHeterogeneousHomothetyPanel ( )
{
}	// QtMgx3DHeterogeneousHomothetyPanel::~QtMgx3DHeterogeneousHomothetyPanel


void QtMgx3DHeterogeneousHomothetyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_homothetyXFactorTextField)	
	CHECK_NULL_PTR_ERROR (_homothetyYFactorTextField)	
	CHECK_NULL_PTR_ERROR (_homothetyZFactorTextField)	
	_homothetyXFactorTextField->setValue (1.);
	_homothetyYFactorTextField->setValue (1.);
	_homothetyZFactorTextField->setValue (1.);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::reset


void QtMgx3DHeterogeneousHomothetyPanel::cancel ( )
{	// A ACTUALISER SI CENTRE D'HOMOTHETIE
	QtMgx3DOperationsSubPanel::cancel ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::cancel


void QtMgx3DHeterogeneousHomothetyPanel::autoUpdate ( )
{	// A ACTUALISER SI CENTRE D'HOMOTHETIE
	QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::autoUpdate


void QtMgx3DHeterogeneousHomothetyPanel::stopSelection ( )
{	// A ACTUALISER SI CENTRE D'HOMOTHETIE
}	// QtMgx3DHeterogeneousHomothetyPanel::stopSelection


double QtMgx3DHeterogeneousHomothetyPanel::getHomothetyXFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_homothetyXFactorTextField)
	return _homothetyXFactorTextField->getValue ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::getHomothetyXFactor


double QtMgx3DHeterogeneousHomothetyPanel::getHomothetyYFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_homothetyYFactorTextField)
	return _homothetyYFactorTextField->getValue ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::getHomothetyYFactor


double QtMgx3DHeterogeneousHomothetyPanel::getHomothetyZFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_homothetyZFactorTextField)
	return _homothetyZFactorTextField->getValue ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::getHomothetyZFactor


QtDoubleTextField& QtMgx3DHeterogeneousHomothetyPanel::getXFactorTextField ( )
{
	CHECK_NULL_PTR_ERROR (_homothetyXFactorTextField)
	return *_homothetyXFactorTextField;
}	// QtMgx3DHeterogeneousHomothetyPanel::getXFactorTextField


QtDoubleTextField& QtMgx3DHeterogeneousHomothetyPanel::getYFactorTextField ( )
{
	CHECK_NULL_PTR_ERROR (_homothetyYFactorTextField)
	return *_homothetyYFactorTextField;
}	// QtMgx3DHeterogeneousHomothetyPanel::getYFactorTextField


QtDoubleTextField& QtMgx3DHeterogeneousHomothetyPanel::getZFactorTextField ( )
{
	CHECK_NULL_PTR_ERROR (_homothetyZFactorTextField)
	return *_homothetyZFactorTextField;
}	// QtMgx3DHeterogeneousHomothetyPanel::getZFactorTextField


vector<Entity*> QtMgx3DHeterogeneousHomothetyPanel::getInvolvedEntities ( )
{	// A ACTUALISER SI CENTRE D'HOMOTHETIE
	vector<Entity*>	entities;

	return entities;
}	// QtMgx3DHeterogeneousHomothetyPanel::getInvolvedEntities


const ContextIfc& QtMgx3DHeterogeneousHomothetyPanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DHeterogeneousHomothetyPanel::getContext


}	// namespace QtComponents

}	// namespace Mgx3D
