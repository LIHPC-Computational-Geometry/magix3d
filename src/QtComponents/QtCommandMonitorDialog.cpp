/**
 * \file		QtCommandMonitorDialog.cpp
 * \author		Charles PIGNEROL
 * \date		18/01/2011
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtCommandMonitorDialog.h"
#include <QtUtil/QtErrorManagement.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtMessageBox.h>

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>

#include <assert.h>



using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{

QtCommandMonitorDialog::QtCommandMonitorDialog (
QWidget* parent, const string& title, CommandManager* manager, bool modal)
	: QDialog (parent, Qt::Window),
	  _commandMonitorPanel (0), _closurePanel (0), _position ( )
{
	setModal (modal);
	setWindowTitle (title.c_str ( ));

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// Le panneau :
	QtGroupBox*	groupBox	= new QtGroupBox ("Commandes", this);
	groupBox->setSpacing (QtConfiguration::spacing);
	groupBox->setMargin (QtConfiguration::margin);
	layout->addWidget (groupBox);
	QVBoxLayout*	commandLayout	= new QVBoxLayout (groupBox);
	groupBox->setLayout (commandLayout);
	_commandMonitorPanel	= new QtCommandMonitorPanel (groupBox, manager);
	commandLayout->addWidget (_commandMonitorPanel);

	// Les boutons :
	layout->addWidget (new QLabel (" ", this));
	_closurePanel	= 
			new QtDlgClosurePanel (this, false, "Fermer", "", "");
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));

	// Par defaut le bouton OK est artificellement clique par QDialog quand
	// l'utilisateur fait return dans un champ de texte => on inhibe ce
	// comportement par defaut :
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
	if (0 != parent)
	{
		QPoint	parentPos	= parent->pos ( );
		QSize	parentSize	= parent->size ( );
		QSize	dlgSize		= size ( );
		QSize	margins		= parentSize - dlgSize;
		_position.setX (parentPos.x ( ) + (margins.width ( ) / 2));
		_position.setY (parentPos.y ( ) + (margins.height ( ) / 2));
	}	// if (0 != parent)
}	// QtCommandMonitorDialog::QtCommandMonitorDialog


QtCommandMonitorDialog::QtCommandMonitorDialog (
											const QtCommandMonitorDialog&)
	: _commandMonitorPanel (0), _closurePanel (0), _position ( )
{
	MGX_FORBIDDEN ("QtCommandMonitorDialog copy constructor is forbidden.")
}	// QtCommandMonitorDialog::QtCommandMonitorDialog (const QtCommandMonitorDialog&)


QtCommandMonitorDialog& QtCommandMonitorDialog::operator = (
											const QtCommandMonitorDialog&)
{
	MGX_FORBIDDEN ("QtCommandMonitorDialog assignment operator is forbidden.")
	return *this;
}	// QtCommandMonitorDialog::operator =


QtCommandMonitorDialog::~QtCommandMonitorDialog ( )
{
}	// QtCommandMonitorDialog::~QtCommandMonitorDialog


QtCommandMonitorPanel& QtCommandMonitorDialog::getCommandPanel ( )
{
	CHECK_NULL_PTR_ERROR (_commandMonitorPanel)
	return *_commandMonitorPanel;
}	// QtCommandMonitorDialog::getCommandPanel


void QtCommandMonitorDialog::show ( )
{
	move (_position);
	QDialog::show ( );
}	// QtCommandMonitorDialog::


void QtCommandMonitorDialog::done (int r)
{
	// Rem : contrairement à la doc Qt 4.6.2, ce code ne marche pas dans
	// QtCommandMonitorDialog::closeEvent car méthode non appelée.
	_position	= pos ( );
	QDialog::done (r);
}	// QtCommandMonitorDialog::done


}	// namespace QtComponents

}	// namespace Mgx3D

