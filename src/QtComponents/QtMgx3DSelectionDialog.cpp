/**
 * \file        QtMgx3DSelectionDialog.cpp
 * \author      Charles PIGNEROL
 * \date        12/05/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtMgx3DSelectionDialog.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>

#include <QBoxLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtMgx3DSelectionDialog
// ===========================================================================

QtMgx3DSelectionDialog::QtMgx3DSelectionDialog (QtMgx3DMainWindow& mainWindow)
	: QDialog (0),
	  _mainWindow (&mainWindow),
	  _entitiesPanel (0), _displaySelectionCheckBox (0),
	  _replaceSelectionCheckBox (0), _closurePanel (0),
	  _actionsStatus (false), _entitiesNames ( )
{
	setModal (true);
	setWindowModality (Qt::WindowModal);
	const string	title	= 0 == _mainWindow ?
			string ("Magix 3D") : _mainWindow->windowTitle ( ).toStdString ( );
	setWindowTitle (title.c_str ( ));

	QBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);

	// La saisie des entités :
	_entitiesPanel	= new QtMgx3DEntityPanel (
				this, title, true, "Identifiants :", "", _mainWindow,
				SelectionManagerIfc::ALL_DIMENSIONS, FilterEntity::All);
	_entitiesPanel->setMultiSelectMode (true);
	CHECK_NULL_PTR_ERROR (_entitiesPanel->getNameTextField ( ))
	_entitiesPanel->getNameTextField ( )->setVisibleColumns (30);
	layout->addWidget (_entitiesPanel);

	// Afficher la sélection ? :
	_displaySelectionCheckBox	=
		new QCheckBox (QString::fromUtf8("Afficher la sélection"), this);
	_displaySelectionCheckBox->setCheckState (Qt::Checked);
	layout->addWidget (_displaySelectionCheckBox);

	// Remplacer la sélection ? :
	_replaceSelectionCheckBox	=
		new QCheckBox (QString::fromUtf8("Remplacer la sélection"), this);
	_replaceSelectionCheckBox->setCheckState (Qt::Checked);
	layout->addWidget (_replaceSelectionCheckBox);

	_closurePanel	= new QtDlgClosurePanel (this, false, "OK", "", "Annuler");
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(reject ( )));
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);

	layout->activate ( );

	if (0 != _mainWindow)
	{
		_actionsStatus	= _mainWindow->disableActions (true);
		_mainWindow->getGroupsPanel ( ).enableSelectionPolicyAdaptation (false);
	}
}	// QtMgx3DSelectionDialog::QtMgx3DSelectionDialog


QtMgx3DSelectionDialog::QtMgx3DSelectionDialog (const QtMgx3DSelectionDialog&)
	: QDialog (0),
	  _mainWindow (0), _entitiesPanel (0),
	  _displaySelectionCheckBox (0), _replaceSelectionCheckBox (0),
	  _closurePanel (0), _actionsStatus (false), _entitiesNames ( )
{
	MGX_FORBIDDEN ("QtMgx3DSelectionDialog copy constructor is not allowed.");
}	// QtMgx3DSelectionDialog::QtMgx3DSelectionDialog (const QtMgx3DSelectionDialog&)


QtMgx3DSelectionDialog& QtMgx3DSelectionDialog::operator = (
												const QtMgx3DSelectionDialog&)
{
	MGX_FORBIDDEN ("QtMgx3DSelectionDialog assignment operator is not allowed.");
	return *this;
}	// QtMgx3DSelectionDialog::QtMgx3DSelectionDialog (const QtMgx3DSelectionDialog&)


QtMgx3DSelectionDialog::~QtMgx3DSelectionDialog ( )
{
}	// QtMgx3DSelectionDialog::~QtMgx3DSelectionDialog


vector<string> QtMgx3DSelectionDialog::getUniqueNames ( ) const
{
	return _entitiesNames;
}	// QtMgx3DSelectionDialog::getUniqueNames


bool QtMgx3DSelectionDialog::displaySelectedEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_displaySelectionCheckBox)
	return Qt::Checked == _displaySelectionCheckBox->checkState ( ) ?
	       true : false;
}	// QtMgx3DSelectionDialog::displaySelectedEntities


bool QtMgx3DSelectionDialog::replaceSelection ( ) const
{
	CHECK_NULL_PTR_ERROR (_replaceSelectionCheckBox)
	return Qt::Checked == _replaceSelectionCheckBox->checkState ( ) ?
	       true : false;
}	// QtMgx3DSelectionDialog::replaceSelection


void QtMgx3DSelectionDialog::done (int r)
{
	QDialog::done (r);

	try
	{

	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel->getNameTextField ( ))
	_entitiesNames	= _entitiesPanel->getUniqueNames ( );

	_entitiesPanel->getNameTextField ( )->setInteractiveMode (false);

	if (0 != _mainWindow)
	{
		_mainWindow->disableActions (_actionsStatus);
		_mainWindow->getGroupsPanel ( ).enableSelectionPolicyAdaptation (true);
	}
	_actionsStatus	= false;

	}
	catch (...)
	{
	}
}	// QtMgx3DSelectionDialog::done


}	// namespace QtComponents

}	// namespace Mgx3D
