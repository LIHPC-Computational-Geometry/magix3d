/**
 * \file        QtMdlOptionsDialog.cpp
 * \author      Charles PIGNEROL
 * \date        26/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtMdlOptionsDialog.h"
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
//                        LA CLASSE QtMdlOptionsDialog
// ===========================================================================

QtMdlOptionsDialog::QtMdlOptionsDialog (
			QWidget* parent, const string& appTitle, const string& fileName)
	: QDialog (parent),
	  _importTopologyCheckBox (0), _importAllContoursCheckBox (0),
	  _useAreaNamesCheckBox (0), _splitCommandsCheckBox (0),
	  _prefixTextField (0), _closurePanel (0)
{
	createGui (parent, appTitle, fileName);
}	// QtMdlOptionsDialog::QtMdlOptionsDialog




QtMdlOptionsDialog::QtMdlOptionsDialog (const QtMdlOptionsDialog&)
	: QDialog (0), _importTopologyCheckBox (0), _importAllContoursCheckBox (0),
	  _useAreaNamesCheckBox (0), _splitCommandsCheckBox (0),
	  _degMinBSplineSpinBox(0), _degMaxBSplineSpinBox(0),
	  _closurePanel (0)
{
	MGX_FORBIDDEN ("QtMdlOptionsDialog copy constructor is not allowed.");
}	// QtMdlOptionsDialog::QtMdlOptionsDialog (const QtMdlOptionsDialog&)


QtMdlOptionsDialog& QtMdlOptionsDialog::operator = (const QtMdlOptionsDialog&)
{
	MGX_FORBIDDEN ("QtMdlOptionsDialog assignment operator is not allowed.");
	return *this;
}	// QtMdlOptionsDialog::QtMdlOptionsDialog (const QtMdlOptionsDialog&)


QtMdlOptionsDialog::~QtMdlOptionsDialog ( )
{
}	// QtMdlOptionsDialog::~QtMdlOptionsDialog


void QtMdlOptionsDialog::createGui (
			QWidget* parent, const string& appTitle, const string& fileName)
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
	name << "Fichier MDL importé : " << fileName;
	QLabel*	label	= new QLabel (UTF8TOQSTRING (name), this);
	layout->addWidget (label);
	layout->addSpacing (20);

	// Importer la topologie ? :
	_importTopologyCheckBox	=
		new QCheckBox ("Importer la topologie", this);
	_importTopologyCheckBox->setCheckState (Qt::Checked);
	layout->addWidget (_importTopologyCheckBox);

	// Importer tous les contours ? :
	_importAllContoursCheckBox	=
		new QCheckBox (QString::fromUtf8("Importation de tous les contours (y compris ceux non reliés à une zone)."), this);
	_importAllContoursCheckBox->setCheckState (Qt::Unchecked);
	layout->addWidget (_importAllContoursCheckBox);

	// Utiliser les noms des zones ? :
	_useAreaNamesCheckBox	=
		new QCheckBox ("Conservation du nom des zones.", this);
	_useAreaNamesCheckBox->setCheckState (Qt::Unchecked);
	layout->addWidget (_useAreaNamesCheckBox);

	// Décomposer en commandes Magix 3D élémentaires :
	_splitCommandsCheckBox	=
				new QCheckBox (QString::fromUtf8("Décomposer en opérations élémentaires"), this);
	_splitCommandsCheckBox->setCheckState (Qt::Unchecked);
	layout->addWidget (_splitCommandsCheckBox);
	connect (_splitCommandsCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (splitCommandsCallback ( )));
	_splitCommandsCheckBox->setToolTip (QString::fromUtf8("Coché, chaque élément du fichier Mdl est converti en commande élémentaire Magix 3D,\net tout lien avec le fichier Mdl est oublié (et ne figure donc pas dans les scripts générés)."));
	layout->addSpacing (20);

	/// le préfixe aux noms des groupes
	QHBoxLayout*	hlayout	= new QHBoxLayout ();
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Préfixe :"), this);
	hlayout->addWidget (label);
	_prefixTextField = new QtTextField (this);
	hlayout->addWidget (_prefixTextField);
	label->setToolTip (QString::fromUtf8("Préfixe pour les noms des groupes."));
	_prefixTextField->setToolTip (QString::fromUtf8("Préfixe pour les noms des groupes."));

	/// les degrés pour les B-Splines
	hlayout	= new QHBoxLayout ();
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Degrés pour les B-Splines Min :"), this);
	hlayout->addWidget (label);
	_degMinBSplineSpinBox = new QSpinBox(this);
	hlayout->addWidget (_degMinBSplineSpinBox);
	_degMinBSplineSpinBox->setMinimum(1);
	label	= new QLabel (QString::fromUtf8(" et Max :"), this);
	hlayout->addWidget (label);
	_degMaxBSplineSpinBox = new QSpinBox(this);
	hlayout->addWidget (_degMaxBSplineSpinBox);
	_degMaxBSplineSpinBox->setMinimum(1);
	_degMaxBSplineSpinBox->setValue(2);

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
}	// QtMdlOptionsDialog::createGui


bool QtMdlOptionsDialog::importTopology ( ) const
{
	CHECK_NULL_PTR_ERROR (_importTopologyCheckBox)
	return Qt::Checked == _importTopologyCheckBox->checkState ( ) ?
	       true : false;
}	// QtMdlOptionsDialog::importTopology


bool QtMdlOptionsDialog::importAllContours ( ) const
{
	CHECK_NULL_PTR_ERROR (_importAllContoursCheckBox)
	return Qt::Checked == _importAllContoursCheckBox->checkState ( ) ?
	       true : false;
}	// QtMdlOptionsDialog::importAllContours


bool QtMdlOptionsDialog::useAreaNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_useAreaNamesCheckBox)
	return Qt::Checked == _useAreaNamesCheckBox->checkState ( ) ?
	       true : false;
}	// QtMdlOptionsDialog::useAreaNames

string QtMdlOptionsDialog::getPrefix ( ) const
{
	CHECK_NULL_PTR_ERROR (_prefixTextField)
	return _prefixTextField->getText ( );
} // QtMdlOptionsDialog::getPrefix


int QtMdlOptionsDialog::getDegMinBSpline() const
{
	CHECK_NULL_PTR_ERROR(_degMinBSplineSpinBox);
	return _degMinBSplineSpinBox->value();
}

int QtMdlOptionsDialog::getDegMaxBSpline() const
{
	CHECK_NULL_PTR_ERROR(_degMaxBSplineSpinBox);
	return _degMaxBSplineSpinBox->value();
}

bool QtMdlOptionsDialog::splitMgx3DCommands ( ) const
{
	CHECK_NULL_PTR_ERROR (_splitCommandsCheckBox)
	return Qt::Checked == _splitCommandsCheckBox->checkState ( ) ?
	       true : false;
}	// QtMdlOptionsDialog::splitMgx3DCommands


void QtMdlOptionsDialog::splitCommandsCallback ( )
{
	CHECK_NULL_PTR_ERROR (_importAllContoursCheckBox)
	CHECK_NULL_PTR_ERROR (_splitCommandsCheckBox)
	QtObjectSignalBlocker	blocker (_importAllContoursCheckBox);
	if (true == splitMgx3DCommands ( ))
		_importAllContoursCheckBox->setChecked (true);
	_importAllContoursCheckBox->setEnabled (!splitMgx3DCommands ( ));
}	// QtMdlOptionsDialog::splitCommandsCallback


}	// namespace QtComponents

}	// namespace Mgx3D
