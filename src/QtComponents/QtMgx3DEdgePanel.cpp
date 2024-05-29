/**
 * \file		QtMgx3DEdgePanel.cpp
 * \author		Charles PIGNEROL
 * \date		30/01/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEdgePanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"

#include "Utils/Common.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Topo/Vertex.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"

#include <QtUtil/QtConfiguration.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>

#include <QLayout>
#include <QLabel>

#include <limits>
#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


QtMgx3DEdgePanel::QtMgx3DEdgePanel (
	QWidget* parent, const string& title, bool editable, const string& name,
	QtMgx3DMainWindow* mainWindow, FilterEntity::objectType types)
	: QWidget (parent),
	  _mainWindow (mainWindow), _entityTextField (0), _initialUniqueName (name), _title (title)
{
	QVBoxLayout*	layout	= new QVBoxLayout ( );
	layout->setContentsMargins (0, 0, 0, 0);
	layout->setSpacing (0);
	layout->setSizeConstraint (QLayout::SetFixedSize);
	setLayout (layout);
	QHBoxLayout*	hlayout	= 0;

	// Titre :
	if (0 != title.length ( ))
	{
		hlayout	= new QHBoxLayout (0);
		hlayout->setContentsMargins (0, 0, 0, 0);
		layout->addLayout (hlayout);
		QLabel*	titleLabel	= new QLabel (title.c_str ( ), this);
		QFont	font (titleLabel->font ( ));
		font.setBold (true);
		titleLabel->setFont (font);
		hlayout->addWidget (titleLabel);
	}	// if (0 != title.length ( ))

	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);
	_entityTextField	= new QtMgx3DEntityPanel (
				this, "", editable, "Arête : ", name, mainWindow,
				SelectionManagerIfc::D1, types);
	connect (_entityTextField, SIGNAL (selectionModified (QString)), this,
	         SLOT (selectionModifiedCallback (QString)));
	connect (_entityTextField, SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (edgesAddedToSelectionCallback (QString)));
	connect (_entityTextField, SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (edgesRemovedFromSelectionCallback (QString)));
	hlayout->addWidget (_entityTextField);
	hlayout->addStretch (2);
	setUniqueName (name);

	adjustSize ( );
}	// QtMgx3DEdgePanel::QtMgx3DEdgePanel


QtMgx3DEdgePanel::QtMgx3DEdgePanel (const QtMgx3DEdgePanel&)
	: QWidget (0),
	  _mainWindow (0), _entityTextField (0), _initialUniqueName (0),
	  _title ("Invalid QtMgx3DEdgePanel")
{
	MGX_FORBIDDEN ("QtMgx3DEdgePanel copy constructor is forbidden.")
}	// QtMgx3DEdgePanel::QtMgx3DEdgePanel (const QtMgx3DEdgePanel&)


QtMgx3DEdgePanel& QtMgx3DEdgePanel::operator = (const QtMgx3DEdgePanel&)
{
	MGX_FORBIDDEN ("QtMgx3DEdgePanel assignment operator is forbidden.")
	return *this;
}	// QtMgx3DEdgePanel::operator =


QtMgx3DEdgePanel::~QtMgx3DEdgePanel ( )
{
}	// QtMgx3DEdgePanel::~QtMgx3DEdgePanel


bool QtMgx3DEdgePanel::isModified ( ) const
{
	return getUniqueName ( ) == _initialUniqueName ? false : true;
}	// QtMgx3DEdgePanel::isModified


bool QtMgx3DEdgePanel::isMultiSelectMode ( ) const
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	return _entityTextField->isMultiSelectMode ( );
}	// QtMgx3DEdgePanel::isMultiSelectMode


void QtMgx3DEdgePanel::setMultiSelectMode (bool multi)
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	_entityTextField->setMultiSelectMode (multi);
}	// QtMgx3DEdgePanel::setMultiSelectMode


void QtMgx3DEdgePanel::setUniqueName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	_entityTextField->setUniqueName (name);
}	// QtMgx3DEdgePanel::setUniqueName


string QtMgx3DEdgePanel::getUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	return _entityTextField->getUniqueName ( );
}	// QtMgx3DEdgePanel::getUniqueName


void QtMgx3DEdgePanel::setUniqueNames (const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	_entityTextField->setUniqueNames (names);
}	// QtMgx3DEdgePanel::setUniqueNames


vector<string> QtMgx3DEdgePanel::getUniqueNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	return _entityTextField->getUniqueNames ( );
}	// QtMgx3DEdgePanel::getUniqueNames


QtMgx3DEntityPanel& QtMgx3DEdgePanel::getEntityPanel ( )
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	return *_entityTextField;
}	// QtMgx3DEdgePanel::getEntityPanel


const QtMgx3DEntityPanel& QtMgx3DEdgePanel::getEntityPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	return *_entityTextField;
}	// QtMgx3DEdgePanel::getEntityPanel


QtEntityIDTextField& QtMgx3DEdgePanel::getEntityTextField ( )
{
	CHECK_NULL_PTR_ERROR (getEntityPanel ( ).getNameTextField ( ))
	return *getEntityPanel ( ).getNameTextField ( );
}	// QtMgx3DEdgePanel::getEntityTextField


const QtEntityIDTextField& QtMgx3DEdgePanel::getEntityTextField ( ) const
{
	CHECK_NULL_PTR_ERROR (getEntityPanel ( ).getNameTextField ( ))
	return *getEntityPanel ( ).getNameTextField ( );
}	// QtMgx3DEdgePanel::getEntityTextField


void QtMgx3DEdgePanel::getPoints (Math::Point& p1, Math::Point& p2) const
{
	const string	edgeName	= getUniqueName ( );
	Entity&	entity	= getContext().nameToEntity (edgeName);
	Geom::Curve*	curve	= dynamic_cast<Geom::Curve*>(&entity);
	Topo::Edge*		edge	= dynamic_cast<Topo::Edge*>(&entity);
	Topo::CoEdge*	coEdge	= dynamic_cast<Topo::CoEdge*>(&entity);
	if (0 != curve)
	{
		vector<Geom::Vertex*>	vertices;
		curve->get (vertices);
		if (2 != vertices.size ( ))
			throw Exception (UTF8String ("Arête géométrique non composée de 2 points.", Charset::UTF_8));
		p1	= vertices [0]->getCoord ( );
		p2	= vertices [1]->getCoord ( );
	}	// if (0 != curve)
	else if (0 != edge)
	{
		vector<Topo::Vertex*>	vertices;
		edge->getVertices (vertices);
		if (2 != vertices.size ( ))
			throw Exception (UTF8String ("Arête topologique non composée de 2 points.", Charset::UTF_8));
		p1	= vertices [0]->getCoord ( );
		p2	= vertices [1]->getCoord ( );
	}	// else if (0 != edge)
	else if (0 != coEdge)
	{
		vector<Topo::Vertex*>	vertices;
		coEdge->getVertices (vertices);
		if (2 != vertices.size ( ))
			throw Exception (UTF8String ("Arête topologique non composée de 2 points.", Charset::UTF_8));
		p1	= vertices [0]->getCoord ( );
		p2	= vertices [1]->getCoord ( );
	}	// else if (0 != coEdge)
	else
	{
		INTERNAL_ERROR (exc, "Arête de type indéfini.", "QtMgx3DEdgePanel::getPoints")
		throw exc;
	}	// else ...
}	// QtMgx3DEdgePanel::getPoints


void QtMgx3DEdgePanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_entityTextField)
		_entityTextField->reset ( );
	}
	catch (...)
	{
	}
}	// QtMgx3DEdgePanel::reset


void QtMgx3DEdgePanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	_entityTextField->stopSelection ( );
}	// QtMgx3DEdgePanel::stopSelection


bool QtMgx3DEdgePanel::actualizeGui (bool removeDestroyed)
{
	CHECK_NULL_PTR_ERROR (_entityTextField)
	return _entityTextField->actualizeGui (removeDestroyed);
}	// QtMgx3DEdgePanel::actualizeGui


void QtMgx3DEdgePanel::selectionModifiedCallback (QString selection)
{
	emit selectionModified (selection);
}	// QtMgx3DEdgePanel::selectionModifiedCallback


void QtMgx3DEdgePanel::edgeAddedToSelectionCallback (QString edgeName)
{
	emit edgeAddedToSelection (edgeName);
}	// QtMgx3DEdgePanel::edgeAddedToSelectionCallback


void QtMgx3DEdgePanel::edgeRemovedFromSelectionCallback (QString edgeName)
{
	emit edgeRemovedFromSelection (edgeName);
}	// QtMgx3DEdgePanel::edgeRemovedFromSelectionCallback


void QtMgx3DEdgePanel::edgesAddedToSelectionCallback (QString edgesNames)
{
	emit edgesAddedToSelection (edgesNames);
}	// QtMgx3DEdgePanel::edgesAddedToSelectionCallback


void QtMgx3DEdgePanel::edgesRemovedFromSelectionCallback (QString edgesNames)
{
	emit edgesRemovedFromSelection (edgesNames);
}	// QtMgx3DEdgePanel::edgesRemovedFromSelectionCallback

const ContextIfc& QtMgx3DEdgePanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DEdgePanel::getContext


}	// namespace QtComponents

}	// namespace Mgx3D
