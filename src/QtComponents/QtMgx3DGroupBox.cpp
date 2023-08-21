/**
 * \file        QtMgx3DGroupBox.cpp
 * \author      Charles PIGNEROL
 * \date        12/06/2018
 */

#include "QtComponents/QtMgx3DGroupBox.h"
#include "Utils/Common.h"

#include <QLabel>
#include <QBoxLayout>

#include <TkUtil/MemoryError.h>

using namespace std;


namespace Mgx3D
{

namespace QtComponents
{

QtMgx3DGroupBox::QtMgx3DGroupBox (const QString& title, QWidget* parent, int margin, int spacing)
	: QWidget (parent), _widget (0)
{	
	QVBoxLayout*	mainLayout	= new QVBoxLayout ( );
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);
	mainLayout->setMargin (margin);
	mainLayout->setSpacing (spacing);
	mainLayout->setContentsMargins (0, 0, 0, 0);
	QLabel*	label	= new QLabel (title, this);
	label->setAlignment (Qt::AlignVCenter | Qt::AlignLeft);
	QFont	font	= label->font ( );
	font.setBold (true);
	label->setFont (font);
	mainLayout->addWidget (label, 1);
	_widget	= new QWidget (this);
	mainLayout->addWidget (_widget, 1000);
	setLayout (mainLayout);
}	// QtMgx3DGroupBox::QtMgx3DGroupBox


QtMgx3DGroupBox::QtMgx3DGroupBox (const QtMgx3DGroupBox&)
	: QWidget (0), _widget (0)
{
	MGX_FORBIDDEN ("QtMgx3DGroupBox copy constructor is not allowed.");
}	// QtMgx3DGroupBox::QtMgx3DGroupBox


QtMgx3DGroupBox& QtMgx3DGroupBox::operator = (const QtMgx3DGroupBox&)
{
	MGX_FORBIDDEN ("QtMgx3DGroupBox copy constructor is not allowed.");
	return *this;
}	// QtMgx3DGroupBox::operator =


QtMgx3DGroupBox::~QtMgx3DGroupBox ( )
{
}	// QtMgx3DGroupBox::~QtMgx3DGroupBox


QWidget& QtMgx3DGroupBox::widget ( )
{
	CHECK_NULL_PTR_ERROR (_widget)
	return *_widget;
}	// QtMgx3DGroupBox::widget


const QWidget& QtMgx3DGroupBox::widget ( ) const
{
	CHECK_NULL_PTR_ERROR (_widget)
	return *_widget;
}	// QtMgx3DGroupBox::widget


void QtMgx3DGroupBox::setAreaLayout (QLayout* layout)
{
	CHECK_NULL_PTR_ERROR (_widget)
	_widget->setLayout (layout);
}	// QtMgx3DGroupBox::setAreaLayout


void QtMgx3DGroupBox::setAreaMargin (int margin)
{
	CHECK_NULL_PTR_ERROR (_widget)
	CHECK_NULL_PTR_ERROR (_widget->layout ( ))
	_widget->layout ( )->setMargin (margin);
}	// QtMgx3DGroupBox::setAreaMargin


void QtMgx3DGroupBox::setAreaSpacing (int spacing)
{
	CHECK_NULL_PTR_ERROR (_widget)
	CHECK_NULL_PTR_ERROR (_widget->layout ( ))
	_widget->layout ( )->setSpacing (spacing);
}	// QtMgx3DGroupBox::setAreaSpacing

}	// namespace QtComponents

}	// namespace Magix3D

