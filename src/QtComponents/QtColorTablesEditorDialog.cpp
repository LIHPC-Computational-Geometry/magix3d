/**
 * \file        QtColorTablesEditorDialog.cpp
 * \author      Charles PIGNEROL
 * \date        23/01/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtColorTablesEditorDialog.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtColorTablesEditorDialog
// ===========================================================================

QtColorTablesEditorDialog::QtColorTablesEditorDialog (
			QWidget* parent, const string& appTitle, const string& title, RenderingManager& renderingManager,
			const vector< RenderingManager::ColorTable* >& tables)
	: QDialog (parent), _appTitle (appTitle), _renderingManager (&renderingManager), _tables (tables),
	  _tablesComboBox (0), _colorNumTextField (0), _minTextField (0),
	  _maxTextField (0), _closurePanel (0)
{
	setModal (true);
	setWindowModality (Qt::WindowModal);
	setWindowTitle (title.c_str ( ));

	QBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);

	// La modifications des tables de couleurs :
	QFormLayout*	formLayout	= new QFormLayout ( );
	layout->addLayout (formLayout);
	// Les tables de couleurs :
	_tablesComboBox	= new QComboBox (this);
	for (vector<RenderingManager::ColorTable*>::const_iterator itct = 
			_tables.begin ( ); _tables.end ( ) != itct; itct++)
		_tablesComboBox->addItem ((*itct)->getValueName ( ).c_str ( ));
	formLayout->addRow ("Tables de couleurs", _tablesComboBox);
	connect (_tablesComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (colorTableCallback ( )));
	// Le nombre de couleurs :
	_colorNumTextField	= new QtIntTextField (this, true);
	_colorNumTextField->setValue (16);
	_colorNumTextField->setRange (16, 1.048576E6);//NumericServices::longMachMax);
	formLayout->addRow ("Nombre de couleurs", _colorNumTextField);
	// Min et max affichés :
	_minTextField	= new QtDoubleTextField (this, true);
	_minTextField->setValue (0.);
	formLayout->addRow ("Minimum", _minTextField);
	_maxTextField	= new QtDoubleTextField (this, true);
	_maxTextField->setValue (0.);
	formLayout->addRow ("Maximum", _maxTextField);

	// Appliquer/Fermer :
	_closurePanel	=
				new QtDlgClosurePanel (this, true, "Appliquer", "Fermer", "");
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(applyCallback ( )));
	connect (_closurePanel->getCloseButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(reject ( )));
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCloseButton ( )->setAutoDefault (false);
	_closurePanel->getCloseButton ( )->setDefault (false);

	colorTableCallback ( );

	layout->activate ( );
}	// QtColorTablesEditorDialog::QtColorTablesEditorDialog


QtColorTablesEditorDialog::QtColorTablesEditorDialog (const QtColorTablesEditorDialog&)
	: QDialog(0), _appTitle ("invalid title"), _renderingManager (0), _tables ( ), _tablesComboBox (0),
	  _colorNumTextField (0), _minTextField (0), _maxTextField (0),
	  _closurePanel (0)
{
	MGX_FORBIDDEN ("QtColorTablesEditorDialog copy constructor is not allowed.");
}	// QtColorTablesEditorDialog::QtColorTablesEditorDialog (const QtColorTablesEditorDialog&)


QtColorTablesEditorDialog& QtColorTablesEditorDialog::operator = (const QtColorTablesEditorDialog&)
{
	MGX_FORBIDDEN ("QtColorTablesEditorDialog assignment operator is not allowed.");
	return *this;
}	// QtColorTablesEditorDialog::QtColorTablesEditorDialog (const QtColorTablesEditorDialog&)


QtColorTablesEditorDialog::~QtColorTablesEditorDialog ( )
{
}	// QtColorTablesEditorDialog::~QtColorTablesEditorDialog


string QtColorTablesEditorDialog::getAppTitle ( ) const
{
	return _appTitle;
}	// QtColorTablesEditorDialog::getAppTitle


RenderingManager::ColorTable& QtColorTablesEditorDialog::getColorTable ( )
{
	CHECK_NULL_PTR_ERROR (_tablesComboBox)
	const size_t	index	= _tablesComboBox->currentIndex ( );
	if (index >= _tables.size ( ))
	{
		INTERNAL_ERROR (exc, "Index de table de couleur invalide.", "QtColorTablesEditorDialog::getColorTable")
		throw exc;
	}	// if (index >= _tables.size ( ))
	RenderingManager::ColorTable*	table	= _tables [index];
	CHECK_NULL_PTR_ERROR (table)

	return *table;
}	// QtColorTablesEditorDialog::getColorTable


RenderingManager::ColorTableDefinition QtColorTablesEditorDialog::getColorTableDefinition ( ) const
{
	return RenderingManager::ColorTableDefinition (
				getValueName ( ), getColorNum ( ), getMinimumValue ( ),
				getMaximumValue ( ));
}	// QtColorTablesEditorDialog::getColorTableDefinition


string QtColorTablesEditorDialog::getValueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_tablesComboBox)

	return _tablesComboBox->currentText ( ).toStdString ( );
}	// QtColorTablesEditorDialog::getValueName


size_t QtColorTablesEditorDialog::getColorNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_colorNumTextField)

	return _colorNumTextField->getValue ( );
}	// QtColorTablesEditorDialog::getColorNum


double QtColorTablesEditorDialog::getMinimumValue ( ) const
{
	CHECK_NULL_PTR_ERROR (_minTextField)

	return _minTextField->getValue ( );
}	// QtColorTablesEditorDialog::getMinimumValue


double QtColorTablesEditorDialog::getMaximumValue ( ) const
{
	CHECK_NULL_PTR_ERROR (_maxTextField)

	return _maxTextField->getValue ( );
}	// QtColorTablesEditorDialog::getMaximumValue


void QtColorTablesEditorDialog::colorTableCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_tablesComboBox)
	CHECK_NULL_PTR_ERROR (_colorNumTextField)
	CHECK_NULL_PTR_ERROR (_minTextField)
	CHECK_NULL_PTR_ERROR (_maxTextField)
	const size_t	index	= _tablesComboBox->currentIndex ( );
	if (index >= _tables.size ( ))
	{
		INTERNAL_ERROR (exc, "Index de table de couleur invalide.", "QtColorTablesEditorDialog::colorTableCallback")
		throw exc;
	}	// if (index >= _tables.size ( ))
	RenderingManager::ColorTable*	table	= _tables [index];
	CHECK_NULL_PTR_ERROR (table)
	_colorNumTextField->setValue (table->getColorNum ( ));
	_minTextField->setValue (table->getMinValue ( ));
	_maxTextField->setValue (table->getMaxValue ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtColorTablesEditorDialog::colorTableCallback


void QtColorTablesEditorDialog::applyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_renderingManager)
	RenderingManager::ColorTableDefinition	definition (getColorTableDefinition ( ));
//	getColorTable ( ).setDefinition (definition);
	_renderingManager->setTableDefinition (definition);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtColorTablesEditorDialog::applyCallback


}	// namespace QtComponents

}	// namespace Mgx3D
