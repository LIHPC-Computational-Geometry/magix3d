/**
 * \file        QtImportOptionsDialog.cpp
 * \author      Eric Brière de l'Isle
 * \date        17/09/2019
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtImportOptionsDialog.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/Process.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtObjectSignalBlocker.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtImportOptionsDialog
// ===========================================================================

QtImportOptionsDialog::QtImportOptionsDialog (
			QWidget* parent, const string& appTitle, const string& fileName)
	: QDialog (parent),
	  _splitCompoundCurvesCheckBox (0), _forAllCheckBox (0), _closurePanel (0)
{
	createGui (parent, appTitle, fileName);
}	// QtImportOptionsDialog::QtImportOptionsDialog




QtImportOptionsDialog::QtImportOptionsDialog (const QtImportOptionsDialog&)
	: QDialog (0), _splitCompoundCurvesCheckBox (0), _forAllCheckBox (0), _closurePanel (0)
{
	MGX_FORBIDDEN ("QtImportOptionsDialog copy constructor is not allowed.");
}	// QtImportOptionsDialog::QtImportOptionsDialog (const QtImportOptionsDialog&)


QtImportOptionsDialog& QtImportOptionsDialog::operator = (const QtImportOptionsDialog&)
{
	MGX_FORBIDDEN ("QtImportOptionsDialog assignment operator is not allowed.");
	return *this;
}	// QtImportOptionsDialog::QtImportOptionsDialog (const QtImportOptionsDialog&)


QtImportOptionsDialog::~QtImportOptionsDialog ( )
{
}	// QtImportOptionsDialog::~QtImportOptionsDialog


void QtImportOptionsDialog::createGui (QWidget* parent, const string& appTitle, const string& fileName)
{
	setModal (true);
	setWindowModality (Qt::WindowModal);
	setWindowTitle (appTitle.c_str ( ));

	QBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);

	// Nom du fichier de sauvegarde de l'état de l'IHM :
	UTF8String	name (Charset::UTF_8);
	name << "Fichier importé : " << fileName;
	QLabel*	label	= new QLabel (UTF8TOQSTRING (name), this);
	layout->addWidget (label);
	layout->addSpacing (20);

	// Importer la topologie ? :
	_splitCompoundCurvesCheckBox	= new QCheckBox ("Décomposer les courbes composites", this);
	_splitCompoundCurvesCheckBox->setCheckState (Qt::Unchecked);
	layout->addWidget (_splitCompoundCurvesCheckBox);
	_forAllCheckBox	= new QCheckBox ("Ne plus poser cette question", this);
	_forAllCheckBox->setCheckState (Qt::Unchecked);
	layout->addWidget (_forAllCheckBox);

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
}	// QtImportOptionsDialog::createGui


bool QtImportOptionsDialog::splitCompoundCurves ( ) const
{
	CHECK_NULL_PTR_ERROR (_splitCompoundCurvesCheckBox)
	return Qt::Checked == _splitCompoundCurvesCheckBox->checkState ( ) ? true : false;
}	// QtImportOptionsDialog::splitCompoundCurves


bool QtImportOptionsDialog::forAll ( ) const
{
	CHECK_NULL_PTR_ERROR (_forAllCheckBox)
	return Qt::Checked == _forAllCheckBox->checkState ( ) ? true : false;
}	// QtImportOptionsDialog::forAll

}	// namespace QtComponents

}	// namespace Mgx3D
