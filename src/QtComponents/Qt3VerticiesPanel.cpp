/**
 * \file        Qt3VerticiesPanel.cpp
 * \author      Charles PIGNEROL
 * \date        08/09/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Vertex.h"
#include "Topo/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/Qt3VerticiesPanel.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QGridLayout>

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
//                      LA CLASSE Qt3VerticiesPanel
// ===========================================================================


Qt3VerticiesPanel::Qt3VerticiesPanel (
			 QWidget* parent, const string& appTitle, QtMgx3DMainWindow& window,
			Mgx3D::Utils::FilterEntity::objectType types, bool extremities)
	: QtMgx3DOperationsSubPanel (parent, window),
	  _mainWindow (&window), _vertex1Panel (0), _vertex2Panel (0), _vertex3Panel (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QGridLayout*	layout	= new QGridLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	// Les 3 vertex :
	QLabel*	label;

	label	= new QLabel (QString::fromUtf8(extremities?"Centre :":"Point 3:"), this);
	layout->addWidget (label, 0, 0);
	_vertex3Panel	= new QtMgx3DEntityPanel (
			this, "", true, "", "", &window, SelectionManagerIfc::D0, types);
	layout->addWidget (_vertex3Panel, 0, 1);
	connect (_vertex3Panel, SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (pointsAddedToSelectionCallback (QString)));
	connect (_vertex3Panel, SIGNAL (entitiesRemovedFromSelection (QString)),
			this, SLOT (pointsRemovedFromSelectionCallback (QString)));

	label	= new QLabel (QString::fromUtf8(extremities?"Départ :":"Point 1:"), this);
	layout->addWidget (label, 1, 0);
	_vertex1Panel	= new QtMgx3DEntityPanel (
			this, "", true, "", "", &window, SelectionManagerIfc::D0, types);
	layout->addWidget (_vertex1Panel, 1, 1);
	connect (_vertex1Panel, SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (pointsAddedToSelectionCallback (QString)));
	connect (_vertex1Panel, SIGNAL (entitiesRemovedFromSelection (QString)),
			this, SLOT (pointsRemovedFromSelectionCallback (QString)));

	label	= new QLabel (QString::fromUtf8(extremities?"Fin :":"Point 2:"), this);
	layout->addWidget (label, 2, 0);
	_vertex2Panel	= new QtMgx3DEntityPanel (
			this, "", true, "", "", &window, SelectionManagerIfc::D0, types);
	layout->addWidget (_vertex2Panel, 2, 1);
	connect (_vertex2Panel, SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (pointsAddedToSelectionCallback (QString)));
	connect (_vertex2Panel, SIGNAL (entitiesRemovedFromSelection (QString)),
			this, SLOT (pointsRemovedFromSelectionCallback (QString)));

	CHECK_NULL_PTR_ERROR (_vertex1Panel->getNameTextField( ))
	CHECK_NULL_PTR_ERROR (_vertex2Panel->getNameTextField( ))
	CHECK_NULL_PTR_ERROR (_vertex3Panel->getNameTextField( ))
	_vertex3Panel->getNameTextField( )->setLinkedSeizureManagers (0, _vertex1Panel->getNameTextField( ));
	_vertex1Panel->getNameTextField( )->setLinkedSeizureManagers (_vertex3Panel->getNameTextField( ), _vertex2Panel->getNameTextField( ));
	_vertex2Panel->getNameTextField( )->setLinkedSeizureManagers (_vertex3Panel->getNameTextField( ), 0);
}	// Qt3VerticiesPanel::Qt3VerticiesPanel


Qt3VerticiesPanel::Qt3VerticiesPanel (const Qt3VerticiesPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _mainWindow (0), _vertex1Panel (0), _vertex2Panel (0), _vertex3Panel (0)
{
	MGX_FORBIDDEN ("Qt3VerticiesPanel copy constructor is not allowed.");
}	// Qt3VerticiesPanel::Qt3VerticiesPanel


Qt3VerticiesPanel& Qt3VerticiesPanel::operator = (const Qt3VerticiesPanel&)
{
	 MGX_FORBIDDEN ("Qt3VerticiesPanel assignment operator is not allowed.");
	return *this;
}	// Qt3VerticiesPanel::operator =


Qt3VerticiesPanel::~Qt3VerticiesPanel ( )
{
}	// Qt3VerticiesPanel::~Qt3VerticiesPanel


void Qt3VerticiesPanel::cancel ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	CHECK_NULL_PTR_ERROR (_vertex3Panel)
	_vertex1Panel->stopSelection ( );
	_vertex2Panel->stopSelection ( );
	_vertex3Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertex1Panel->setUniqueName ("");
		_vertex2Panel->setUniqueName ("");
		_vertex3Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))

	QtMgx3DOperationsSubPanel::cancel ( );
}	// Qt3VerticiesPanel::cancel


void Qt3VerticiesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	CHECK_NULL_PTR_ERROR (_vertex3Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const FilterEntity::objectType	types	=
											_vertex1Panel->getFilteredTypes ( );
		vector<string>	selectedVertices =
			getSelectionManager ( ).getEntitiesNames (types);
		if (3 <= selectedVertices.size ( ))
		{
			_vertex1Panel->setUniqueName (selectedVertices [0]);
			if (2 <= selectedVertices.size ( ))
				_vertex2Panel->setUniqueName (selectedVertices [1]);
			if (3 <= selectedVertices.size ( ))
				_vertex3Panel->setUniqueName (selectedVertices [2]);
		}	// if (2 <= selectedVertices.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_vertex1Panel->setUniqueName ("");
	_vertex2Panel->setUniqueName ("");
	_vertex3Panel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// Qt3VerticiesPanel::autoUpdate


string Qt3VerticiesPanel::getStartVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	return _vertex1Panel->getUniqueName ( );
}	// Qt3VerticiesPanel::getStartVertexUniqueName


string Qt3VerticiesPanel::getEndVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	return _vertex2Panel->getUniqueName ( );
}	// Qt3VerticiesPanel::getEndVertexUniqueName


string Qt3VerticiesPanel::getCenterVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex3Panel)
	return _vertex3Panel->getUniqueName ( );
}	// Qt3VerticiesPanel::getCenterVertexUniqueName


void Qt3VerticiesPanel::getPoints (Math::Point& p1, Math::Point& p2, Math::Point& p3) const
{
	p1	= getPoint (getStartVertexUniqueName ( ));
	p2	= getPoint (getEndVertexUniqueName ( ));
	p3	= getPoint (getCenterVertexUniqueName ( ));
}	// Qt3VerticiesPanel::getPoints


void Qt3VerticiesPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	CHECK_NULL_PTR_ERROR (_vertex3Panel)
	_vertex1Panel->stopSelection ( );
	_vertex2Panel->stopSelection ( );
	_vertex3Panel->stopSelection ( );
}	// Qt3VerticiesPanel::stopSelection


vector<Entity*> Qt3VerticiesPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	name1	= getStartVertexUniqueName ( );
	const string	name2	= getEndVertexUniqueName ( );
	const string	name3	= getCenterVertexUniqueName ( );
	if (0 != name1.length ( ))
	{
		try
		{
			Entity&	vertex	= getContext().nameToEntity (name1);
			entities.push_back (&vertex);
		}
		catch (...)
		{
		}
	}	// if (0 != name1.length ( ))
	if (0 != name2.length ( ))
	{
		try
		{
			Entity&	vertex	= getContext().nameToEntity (name2);
			entities.push_back (&vertex);
		}
		catch (...)
		{
		}
	}	// if (0 != name2.length ( ))
	if (0 != name3.length ( ))
	{
		try
		{
			Entity&	vertex	= getContext().nameToEntity (name3);
			entities.push_back (&vertex);
		}
		catch (...)
		{
		}
	}	// if (0 != name3.length ( ))

	return entities;
}	// Qt3VerticiesPanel::getInvolvedEntities


void Qt3VerticiesPanel::pointAddedToSelectionCallback (QString pointName)
{
	emit pointAddedToSelection (pointName);
}	// Qt3VerticiesPanel::pointAddedToSelectionCallback


void Qt3VerticiesPanel::pointsAddedToSelectionCallback (QString pointName)
{
	// Seul un point peut être ajouté :
	emit pointAddedToSelection (pointName);
}	// Qt3VerticiesPanel::pointsAddedToSelectionCallback


void Qt3VerticiesPanel::pointRemovedFromSelectionCallback (QString pointName)
{
	emit pointRemovedFromSelection (pointName);
}	// Qt3VerticiesPanel::pointRemovedFromSelectionCallback


void Qt3VerticiesPanel::pointsRemovedFromSelectionCallback (QString pointName)
{
	// Seul un point peut être enlevé :
	emit pointRemovedFromSelection (pointName);
}	// Qt3VerticiesPanel::pointsRemovedFromSelectionCallback


Utils::Math::Point Qt3VerticiesPanel::getPoint (const string& name) const
{
	Entity&			entity	= getContext().nameToEntity (name);
	Geom::Vertex*	gvertex	= dynamic_cast<Geom::Vertex*>(&entity);
	Topo::Vertex*	tvertex	= dynamic_cast<Topo::Vertex*>(&entity);

    if (0 != gvertex)
		return gvertex->getCoord ( );
	else if (0 != tvertex)
		return tvertex->getCoord ( );

	throw Exception (UTF8String ("Absence de point de ce nom.", Charset::UTF_8));
}   // getPoint


const ContextIfc& Qt3VerticiesPanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// Qt3VerticiesPanel::getContext


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
