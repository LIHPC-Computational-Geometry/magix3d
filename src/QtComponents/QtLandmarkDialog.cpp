#include "Internal/ContextIfc.h"

#include "QtComponents/QtLandmarkDialog.h"
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtConfiguration.h>

#include <TkUtil/Exception.h>

#include <assert.h>

#include <QLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;


QtLandmarkDialog::QtLandmarkDialog (
			QWidget* parent, const UTF8String& title, const string& appTitle,
			bool modal, 
			const string& xTitle, const string& yTitle, const string& zTitle,
			const LandmarkAttributes& parameters, const string& helpURL,
			const string& helpTag)
	: QDialog (parent),
	  _landmarkPanel (0), _closurePanel (0)
{	
	setModal (modal);
	setWindowTitle (QString::fromUtf8 (title.utf8 ( ).c_str ( )));

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (5);	// QtConfiguration::margin);
	layout->setSpacing (5);	// QtConfiguration::spacing);
	setContentsMargins (0, 0, 0, 0);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	QtGroupBox*		frame		= 
			new QtGroupBox (QString::fromUtf8("Paramètres des axes"), this, "frame");
	QVBoxLayout*	frameLayout	= new QVBoxLayout (frame);
	layout->addWidget (frame);
	frame->setLayout (frameLayout);
	frame->setMargin (5);	// QtConfiguration::margin);
	frame->setSpacing (5);	// QtConfiguration::spacing);
	frameLayout->setContentsMargins (0, 0, 0, 0);
	layout->setMargin (5);
	layout->setSpacing (5);
	_landmarkPanel	= 
		new QtLandmarkPanel ( frame, "", xTitle, yTitle, zTitle, parameters);
	_landmarkPanel->adjustSize ( );
	frameLayout->addWidget (_landmarkPanel);

	layout->addWidget (new QLabel (" ", this));
	_closurePanel	= 
		new QtDlgClosurePanel (this, false, "Appliquer", "", "Annuler",
		                       helpURL, helpTag);
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	_closurePanel->setContentsMargins (0, 0, 0, 0);

	// Par defaut le bouton OK est artificellement clique par QDialog quand
	// l'utilisateur fait return dans un champ de texte => on inhibe ce
	// comportement par defaut :
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(reject ( )));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(apply ( )));

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
}	// QtLandmarkDialog::QtLandmarkDialog


QtLandmarkDialog::QtLandmarkDialog (const QtLandmarkDialog&)
{
	assert (0 && "QtLandmarkDialog copy constructor is forbidden.");
}	// QtLandmarkDialog::QtLandmarkDialog (const QtLandmarkDialog&)


QtLandmarkDialog& QtLandmarkDialog::operator = (const QtLandmarkDialog&)
{
	assert (0 && "QtLandmarkDialog assignment operator is forbidden.");
	return *this;
}	// QtLandmarkDialog::operator =


QtLandmarkDialog::~QtLandmarkDialog ( )
{
}	// QtLandmarkDialog::~QtLandmarkDialog


LandmarkAttributes QtLandmarkDialog::getParameters ( ) const
{
	assert ((0 != _landmarkPanel) && "QtLandmarkDialog::getParameters : null landmark panel.");
	return _landmarkPanel->getParameters ( );
}	// QtLandmarkDialog::xParameters


AxisAttributes QtLandmarkDialog::xParameters ( ) const
{
	assert ((0 != _landmarkPanel) && "QtLandmarkDialog::xParameters : null landmark panel.");
	return _landmarkPanel->xParameters ( );
}	// QtLandmarkDialog::xParameters


AxisAttributes QtLandmarkDialog::yParameters ( ) const
{
	assert ((0 != _landmarkPanel) && "QtLandmarkDialog::yParameters : null landmark panel.");
	return _landmarkPanel->yParameters ( );
}	// QtLandmarkDialog::yParameters


AxisAttributes QtLandmarkDialog::zParameters ( ) const
{
	assert ((0 != _landmarkPanel) && "QtLandmarkDialog::zParameters : null landmark panel.");
	return _landmarkPanel->zParameters ( );
}	// QtLandmarkDialog::zParameters


bool QtLandmarkDialog::autoAdjust ( ) const
{
	assert ((0 != _landmarkPanel) && "QtLandmarkDialog::autoAdjust : null landmark panel.");
	return _landmarkPanel->autoAdjust ( );
}	// QtLandmarkDialog::autoAdjust


QPushButton* QtLandmarkDialog::getApplyButton ( ) const
{
	assert ((0 != _closurePanel) && "QtLandmarkDialog::getApplyButton : null closure panel.");
	return _closurePanel->getApplyButton ( );
}	// QtLandmarkDialog::getApplyButton


QPushButton* QtLandmarkDialog::getCancelButton ( ) const
{
	assert ((0 != _closurePanel) && "QtLandmarkDialog::getCancelButton : null closure panel.");
	return _closurePanel->getCancelButton ( );
}	// QtLandmarkDialog::getCancelButton


void QtLandmarkDialog::apply ( )
{
	QDialog::accept ( );
	emit applied (this);
}	// QtLandmarkDialog::apply


void QtLandmarkDialog::reject ( )
{
	QDialog::reject ( );
	emit canceled (this);
}	// QtLandmarkDialog::reject


