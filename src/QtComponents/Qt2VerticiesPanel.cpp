/**
 * \file        Qt2VerticiesPanel.cpp
 * \author      Charles PIGNEROL
 * \date        23/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Vertex.h"
#include "Topo/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/Qt2VerticiesPanel.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

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
//                      LA CLASSE Qt2VerticiesPanel
// ===========================================================================


Qt2VerticiesPanel::Qt2VerticiesPanel (
			 QWidget* parent, const string& appTitle, QtMgx3DMainWindow& window,
			Mgx3D::Utils::FilterEntity::objectType types)
	: QtMgx3DOperationsSubPanel (parent, window),
	  _mainWindow (&window), _vertex1Panel (0), _vertex2Panel (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	// Les 2 vertex :
	_vertex1Panel	= new QtMgx3DEntityPanel (
				this, "", true, "Point 1 :", "", &window,
				SelectionManagerIfc::D0, types);
	layout->addWidget (_vertex1Panel);
	connect (_vertex1Panel, SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (pointsAddedToSelectionCallback (QString)));
	connect (_vertex1Panel, SIGNAL (entitiesRemovedFromSelection (QString)),
			this, SLOT (pointsRemovedFromSelectionCallback (QString)));
	_vertex2Panel	= new QtMgx3DEntityPanel (
				this, "", true, "Point 2 :", "", &window,
				SelectionManagerIfc::D0, types);
	layout->addWidget (_vertex2Panel);
	connect (_vertex2Panel, SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (pointsAddedToSelectionCallback (QString)));
	connect (_vertex2Panel, SIGNAL (entitiesRemovedFromSelection (QString)),
			this, SLOT (pointsRemovedFromSelectionCallback (QString)));

	CHECK_NULL_PTR_ERROR (_vertex1Panel->getNameTextField( ))
	CHECK_NULL_PTR_ERROR (_vertex2Panel->getNameTextField( ))
	_vertex1Panel->getNameTextField( )->setLinkedSeizureManagers (
										0, _vertex2Panel->getNameTextField( ));
	_vertex2Panel->getNameTextField( )->setLinkedSeizureManagers (
										_vertex1Panel->getNameTextField( ), 0);
}	// Qt2VerticiesPanel::Qt2VerticiesPanel


Qt2VerticiesPanel::Qt2VerticiesPanel (const Qt2VerticiesPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _mainWindow (0), _vertex1Panel (0), _vertex2Panel (0)
{
	MGX_FORBIDDEN ("Qt2VerticiesPanel copy constructor is not allowed.");
}	// Qt2VerticiesPanel::Qt2VerticiesPanel


Qt2VerticiesPanel& Qt2VerticiesPanel::operator = (const Qt2VerticiesPanel&)
{
	 MGX_FORBIDDEN ("Qt2VerticiesPanel assignment operator is not allowed.");
	return *this;
}	// Qt2VerticiesPanel::operator =


Qt2VerticiesPanel::~Qt2VerticiesPanel ( )
{
}	// Qt2VerticiesPanel::~Qt2VerticiesPanel


void Qt2VerticiesPanel::cancel ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	_vertex1Panel->stopSelection ( );
	_vertex2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertex1Panel->setUniqueName ("");
		_vertex2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))

	QtMgx3DOperationsSubPanel::cancel ( );
}	// Qt2VerticiesPanel::cancel


void Qt2VerticiesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const FilterEntity::objectType	types	=
											_vertex1Panel->getFilteredTypes ( );
		vector<string>	selectedVertices =
			getSelectionManager ( ).getEntitiesNames (types);
		if (2 <= selectedVertices.size ( ))
		{
			_vertex1Panel->setUniqueName (selectedVertices [0]);
			if (2 == selectedVertices.size ( ))
				_vertex2Panel->setUniqueName (selectedVertices [1]);
		}	// if (2 <= selectedVertices.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_vertex1Panel->setUniqueName ("");
	_vertex2Panel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// Qt2VerticiesPanel::autoUpdate


string Qt2VerticiesPanel::getVertex1UniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	return _vertex1Panel->getUniqueName ( );
}	// Qt2VerticiesPanel::getVertex1UniqueName


string Qt2VerticiesPanel::getVertex2UniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	return _vertex2Panel->getUniqueName ( );
}	// Qt2VerticiesPanel::getVertex2UniqueName


void Qt2VerticiesPanel::getPoints (Math::Point& p1, Math::Point& p2) const
{
	p1	= getPoint (getVertex1UniqueName ( ));
	p2	= getPoint (getVertex2UniqueName ( ));
}	// Qt2VerticiesPanel::getPoints


void Qt2VerticiesPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	_vertex1Panel->stopSelection ( );
	_vertex2Panel->stopSelection ( );
}	// Qt2VerticiesPanel::stopSelection


void Qt2VerticiesPanel::reset ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	_vertex1Panel->reset ( );
	_vertex2Panel->reset ( );
}	// Qt2VerticiesPanel::reset


bool Qt2VerticiesPanel::actualizeGui (bool removeDestroyed)
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	bool	modified	= false, tmp	= false;
	modified	= _vertex1Panel->actualizeGui (removeDestroyed);
	tmp			= _vertex2Panel->actualizeGui (removeDestroyed);

	if (true == tmp)
		modified	= true;

	return modified;
}	// Qt2VerticiesPanel::stopSelection


vector<Entity*> Qt2VerticiesPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	name1	= getVertex1UniqueName ( );
	const string	name2	= getVertex2UniqueName ( );
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

	return entities;
}	// Qt2VerticiesPanel::getInvolvedEntities


void Qt2VerticiesPanel::pointAddedToSelectionCallback (QString point)
{
	emit pointAddedToSelection (point);
}	// Qt2VerticiesPanel::pointAddedToSelectionCallback


void Qt2VerticiesPanel::pointsAddedToSelectionCallback (QString point)
{
	// Seul un point peut être ajouté :
	emit pointAddedToSelection (point);
}	// Qt2VerticiesPanel::pointsAddedToSelectionCallback


void Qt2VerticiesPanel::pointRemovedFromSelectionCallback (QString point)
{
	emit pointRemovedFromSelection (point);
}	// Qt2VerticiesPanel::pointRemovedFromSelectionCallback


void Qt2VerticiesPanel::pointsRemovedFromSelectionCallback (QString point)
{
	// Seul un point peut être supprimé :
	emit pointRemovedFromSelection (point);
}	// Qt2VerticiesPanel::pointsRemovedFromSelectionCallback


Utils::Math::Point Qt2VerticiesPanel::getPoint (const string& name) const
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


const ContextIfc& Qt2VerticiesPanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// Qt2VerticiesPanel::getContext


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
