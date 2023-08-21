/**
 * \file		QtGroupsSelectorDialog.cpp
 * \author		Charles PIGNEROL
 * \date		31/01/2017
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtGroupsSelectorDialog.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>

#include <assert.h>



using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{

QtGroupsSelectorDialog::QtGroupsSelectorDialog (
										QWidget* parent, const string& title)
	: QDialog (parent),
	  _dimensionsPanel (0), _levelTextField (0), _levelsLabel (0),
	  _newSelectionCheckBox (0), _closurePanel (0), _levels ( )
{
	setModal (true);
	setWindowTitle (title.c_str ( ));

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// Le panneau :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	const SelectionManagerIfc::DIM	dims	=
										SelectionManagerIfc::ALL_DIMENSIONS;
	_dimensionsPanel	= new QtDimensionsSelectorPanel(this, dims, dims, true);
	hlayout->addWidget (_dimensionsPanel);
	QVBoxLayout*	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	QHBoxLayout*	hlayout2	= new QHBoxLayout ();
	vlayout->addLayout (hlayout2);
	QLabel*	label	= new QLabel ("Niveau :", this);
	hlayout2->addWidget (label);
	_levelTextField	= new QtIntTextField (this);
	_levelTextField->setRange (0, 1000000);
	_levelTextField->setValue (1);
	hlayout2->addWidget (_levelTextField);
	QPushButton*	addButton	= new QPushButton ("Ajouter", this);
	connect (addButton, SIGNAL (clicked (bool)), this,
	         SLOT (addLevelCallback ( )));
	hlayout2->addWidget (addButton);
	QPushButton*	removeButton	= new QPushButton ("Enlever", this);
	connect (removeButton, SIGNAL (clicked (bool)), this,
	         SLOT (removeLevelCallback ( )));
	hlayout2->addWidget (removeButton);
	_levelsLabel	= new QLabel ("", this);
	vlayout->addWidget (_levelsLabel);
	vlayout->addStretch (1000);

	// Nouvelle sélection ?
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	_newSelectionCheckBox	= new QCheckBox (QString::fromUtf8("Nouvelle sélection"), this);
	_newSelectionCheckBox->setChecked (true);
	_newSelectionCheckBox->setToolTip (QString::fromUtf8("Cochée cette sélection remplacera la sélection en cours."));
	hlayout->addWidget (_newSelectionCheckBox);
	hlayout->addStretch (1000);

	// Les boutons :
	layout->addWidget (new QLabel (" ", this));
	_closurePanel	= new QtDlgClosurePanel (this, false, "OK", "", "Annuler");
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(reject ( )));

	// Par defaut le bouton OK est artificellement clique par QDialog quand
	// l'utilisateur fait return dans un champ de texte => on inhibe ce
	// comportement par defaut :
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
}	// QtGroupsSelectorDialog::QtGroupsSelectorDialog


QtGroupsSelectorDialog::QtGroupsSelectorDialog (const QtGroupsSelectorDialog&)
	: _dimensionsPanel (0), _levelTextField (0), _levelsLabel (0),
	  _newSelectionCheckBox (0), _closurePanel (0), _levels ( )
{
	MGX_FORBIDDEN ("QtGroupsSelectorDialog copy constructor is forbidden.")
}	// QtGroupsSelectorDialog::QtGroupsSelectorDialog (const QtGroupsSelectorDialog&)


QtGroupsSelectorDialog& QtGroupsSelectorDialog::operator = (
											const QtGroupsSelectorDialog&)
{
	MGX_FORBIDDEN ("QtGroupsSelectorDialog assignment operator is forbidden.")
	return *this;
}	// QtGroupsSelectorDialog::operator =


QtGroupsSelectorDialog::~QtGroupsSelectorDialog ( )
{
}	// QtGroupsSelectorDialog::~QtGroupsSelectorDialog


SelectionManagerIfc::DIM QtGroupsSelectorDialog::dimensions ( ) const
{
	CHECK_NULL_PTR_ERROR (_dimensionsPanel)
	return _dimensionsPanel->getDimensions ( );
}	// QtGroupsSelectorDialog::getCommandPanel


const set<unsigned long>& QtGroupsSelectorDialog::levels ( ) const
{
	return _levels;
}	// QtGroupsSelectorDialog::levels


bool QtGroupsSelectorDialog::newSelection ( ) const
{
	CHECK_NULL_PTR_ERROR (_newSelectionCheckBox)
	return _newSelectionCheckBox->isChecked ( );
}	// QtGroupsSelectorDialog::newSelection


void QtGroupsSelectorDialog::addLevelCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_levelTextField)
	CHECK_NULL_PTR_ERROR (_levelsLabel)

	_levels.insert (_levelTextField->getValue ( ));

	UTF8String	levels (Charset::UTF_8);
	bool			first	= true;
	for (set<unsigned long>::iterator it = _levels.begin ( );
	     _levels.end ( ) != it; it++)
	{
		if (false == first)
			levels << ", " << *it;
		else
		{
			levels << *it;
			first	= false;
		}

		_levelsLabel->setText (UTF8TOQSTRING (levels));
	}	// for (set<unsigned long>::iterator it = _levels.begin ( ); ...

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtGroupsSelectorDialog::addLevelCallback


void QtGroupsSelectorDialog::removeLevelCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_levelTextField)
	CHECK_NULL_PTR_ERROR (_levelsLabel)

	_levelsLabel->setText ("");
	_levels.erase (_levelTextField->getValue ( ));

	UTF8String	levels (Charset::UTF_8);
	bool			first	= true;
	for (set<unsigned long>::iterator it = _levels.begin ( );
	     _levels.end ( ) != it; it++)
	{
		if (false == first)
			levels << ", " << *it;
		else
		{
			levels << *it;
			first	= false;
		}

		_levelsLabel->setText (UTF8TOQSTRING (levels));
	}	// for (set<unsigned long>::iterator it = _levels.begin ( ); ...

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D")
}	// QtGroupsSelectorDialog::removeLevelCallback


void QtGroupsSelectorDialog::accept ( )
{
	if (SelectionManagerIfc::NO_DIM == dimensions ( ))
	{
		QtMessageBox::displayErrorMessage (this, windowTitle( ).toStdString( ),
											"Absence de dimension retenue.");
		return;
	}	// if (SelectionManagerIfc::NO_DIM == dimensions ( ))
	if (0 == _levels.size ( ))
	{
		QtMessageBox::displayErrorMessage (this, windowTitle( ).toStdString( ),
											"Absence de niveau retenu.");
		return;
	}	// if (0 == _levels.size ( ))

	QDialog::accept ( );
}	// QtGroupsSelectorDialog::accept


}	// namespace QtComponents

}	// namespace Mgx3D

