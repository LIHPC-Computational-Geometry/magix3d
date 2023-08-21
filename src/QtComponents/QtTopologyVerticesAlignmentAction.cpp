/**
 * \file        QtTopologyVerticesAlignmentAction.cpp
 * \author      Charles PIGNEROL
 * \date        16/12/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Geom/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyVerticesAlignmentAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyVerticesAlignmentPanel
// ===========================================================================

QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).alignVerticesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).alignVerticesOperationTag),
	  _point1TextField (0), _point2TextField (0), _verticesPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	// Le nom du panneau :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// La droite de projection :
//	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	QGridLayout*	hlayout	= new QGridLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);
	const FilterEntity::objectType	pointTypes	= (FilterEntity::objectType)
						(FilterEntity::GeomVertex | FilterEntity::TopoVertex);
	label	= new QLabel ("Point 1 :", this);
	hlayout->addWidget (label, 0, 0);
	_point1TextField	= new QtEntityIDTextField (
						this, &mainWindow, SelectionManagerIfc::D0, pointTypes);
	connect (_point1TextField, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_point1TextField, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	label->setToolTip (QString::fromUtf8("Premier point définissant la droite de projection des sommets."));
	_point1TextField->setToolTip (QString::fromUtf8("Premier point définissant la droite de projection des sommets."));
	hlayout->addWidget (_point1TextField, 0, 1);
	label	= new QLabel ("Point 2 :", this);
	hlayout->addWidget (label, 1, 0);
	_point2TextField	= new QtEntityIDTextField (
						this, &mainWindow, SelectionManagerIfc::D0, pointTypes);
	connect (_point2TextField, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_point2TextField, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	label->setToolTip (QString::fromUtf8("Second point définissant la droite de projection des sommets."));
	_point2TextField->setToolTip (QString::fromUtf8("Second point définissant la droite de projection des sommets."));
	hlayout->addWidget (_point2TextField, 1, 1);

	// Les sommets à projeter :
	_verticesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Sommets :", "", &mainWindow,
			SelectionManagerIfc::D0, FilterEntity::TopoVertex);
	_verticesPanel->setMultiSelectMode (true);
	connect (_verticesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_verticesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_verticesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_verticesPanel->getNameTextField ( ))
	_point1TextField->setLinkedSeizureManagers (0, _point2TextField);
	_point2TextField->setLinkedSeizureManagers (
						_point1TextField, _verticesPanel->getNameTextField ( ));
	_verticesPanel->getNameTextField ( )->setLinkedSeizureManagers (
														_point2TextField, 0);
}	// QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel


QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (
									const QtTopologyVerticesAlignmentPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
			_point1TextField (0), _point2TextField (0), _verticesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentPanel copy constructor is not allowed.");
}	// QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (const QtTopologyVerticesAlignmentPanel&)


QtTopologyVerticesAlignmentPanel& QtTopologyVerticesAlignmentPanel::operator = (
										const QtTopologyVerticesAlignmentPanel&)
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (const QtTopologyVerticesAlignmentPanel&)


QtTopologyVerticesAlignmentPanel::~QtTopologyVerticesAlignmentPanel ( )
{
}	// QtTopologyVerticesAlignmentPanel::~QtTopologyVerticesAlignmentPanel


void QtTopologyVerticesAlignmentPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_point1TextField)
	CHECK_NULL_PTR_ERROR (_point2TextField)
	_point1TextField->reset ( );
	_point2TextField->reset ( );
	_verticesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyVerticesAlignmentPanel::reset


void QtTopologyVerticesAlignmentPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_point1TextField)
	CHECK_NULL_PTR_ERROR (_point2TextField)
	CHECK_NULL_PTR_ERROR (_verticesPanel)

	_point1TextField->setInteractiveMode (false);
	_point2TextField->setInteractiveMode (false);
	_verticesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	names;
		_point1TextField->setUniqueNames (names);
		_point2TextField->setUniqueNames (names);
		_verticesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyVerticesAlignmentPanel::cancel


void QtTopologyVerticesAlignmentPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_point1TextField)
	CHECK_NULL_PTR_ERROR (_point2TextField)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
		if (0 != selectedVertices.size ( ))
			_verticesPanel->setUniqueNames (selectedVertices);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_point1TextField->clearSelection ( );
	_point2TextField->clearSelection ( );
	_verticesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_verticesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyVerticesAlignmentPanel::autoUpdate


Math::Point QtTopologyVerticesAlignmentPanel::getPoint1 ( ) const
{
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	CHECK_NULL_PTR_ERROR (_point1TextField)
	Math::Point	point;

	if (1 == _point1TextField->getObjectsCount ( ))
	{
		const vector<string>	names	= _point1TextField->getUniqueNames ( );
		const string			name	= names [0];
		switch (_point1TextField->getObjectType ( ))
		{
			case Entity::GeomVertex	:
			{

				const Geom::Vertex*	vertex	=
					getMainWindow ( )->getContext ( ).getGeomManager (
													).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex)
				point	= vertex->getPoint ( );
			}	// case Entity::GeomVertex
			case Entity::TopoVertex	:
			{
				const Topo::Vertex*	vertex	=
					getMainWindow ( )->getContext ( ).getTopoManager (
													).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex)
				point	= vertex->getCoord ( );
			}	// case Entity::TopoVertex
			break;
			default	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Entité de type "
					<< Entity::objectTypeToObjectTypeName (_point1TextField->getObjectType ( ))
					<< " (" << (unsigned long)_point1TextField->getObjectType ( )
					<< ") non supporté pour la saisie d'un point.";
				throw Exception (message);
			}	// default
		}	// switch (_point1TextField->getObjectType ( ))
	}	// if (1 == _point1TextField->getObjectsCount ( ))
	else
		 throw Exception (UTF8String ("Absence de premier point de la droite saisi.", Charset::UTF_8));

	return point;
}	// QtTopologyVerticesAlignmentPanel::getPoint1


Math::Point QtTopologyVerticesAlignmentPanel::getPoint2 ( ) const
{
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	CHECK_NULL_PTR_ERROR (_point2TextField)
	Math::Point	point;

	if (1 == _point2TextField->getObjectsCount ( ))
	{
		const vector<string>	names	= _point2TextField->getUniqueNames ( );
		const string			name	= names [0];
		switch (_point2TextField->getObjectType ( ))
		{
			case Entity::GeomVertex	:
			{

				const Geom::Vertex*	vertex	=
					getMainWindow ( )->getContext ( ).getGeomManager (
													).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex)
				point	= vertex->getPoint ( );
			}	// case Entity::GeomVertex
			case Entity::TopoVertex	:
			{
				const Topo::Vertex*	vertex	=
					getMainWindow ( )->getContext ( ).getTopoManager (
													).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex)
				point	= vertex->getCoord ( );
			}	// case Entity::TopoVertex
			break;
			default	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Entité de type "
					<< Entity::objectTypeToObjectTypeName (_point2TextField->getObjectType ( ))
					<< " (" << (unsigned long)_point2TextField->getObjectType ( )
					<< ") non supporté pour la saisie d'un point.";
				throw Exception (message);
			}	// default
		}	// switch (_point2TextField->getObjectType ( ))
	}	// if (1 == _point2TextField->getObjectsCount ( ))
	else
		 throw Exception (UTF8String ("Absence de second point de la droite saisi.", Charset::UTF_8));

	return point;
}	// QtTopologyVerticesAlignmentPanel::getPoint2


vector<string> QtTopologyVerticesAlignmentPanel::getVerticesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getUniqueNames ( );
}	// QtTopologyVerticesAlignmentPanel::getVerticesNames


vector<Entity*> QtTopologyVerticesAlignmentPanel::getInvolvedEntities ( )
{
	const vector<string>	pt1Names	= _point1TextField->getUniqueNames ( );
	const vector<string>	pt2Names	= _point2TextField->getUniqueNames ( );
	const vector<string>	names		= getVerticesNames ( );

	vector<Entity*>			entities;
	if (1 == pt1Names.size ( ))
	{
		try
		{
			Entity*	entity	= &getContext ( ).nameToEntity (pt1Names [0]);
			entities.push_back (entity);
		}
		catch (...)
		{
		}
	}	// if (1 == pt1Names.size ( ))
	if (1 == pt2Names.size ( ))
	{
		try
		{
			Entity*	entity	= &getContext ( ).nameToEntity (pt2Names [0]);
			entities.push_back (entity);
		}
		catch (...)
		{
		}
	}	// if (1 == pt2Names.size ( ))
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*		vertex	=
			getContext ( ).getTopoManager ( ).getVertex (*it, false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopologyVerticesAlignmentPanel::getInvolvedEntities


void QtTopologyVerticesAlignmentPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _point1TextField)
		_point1TextField->setInteractiveMode (false);
	if (0 != _point2TextField)
		_point2TextField->setInteractiveMode (false);
	if (0 != _verticesPanel)
		_verticesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyVerticesAlignmentPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyVerticesAlignmentAction
// ===========================================================================

QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyVerticesAlignmentPanel*	operationPanel	= 
			new QtTopologyVerticesAlignmentPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction


QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction (
										const QtTopologyVerticesAlignmentAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentAction copy constructor is not allowed.")
}	// QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction


QtTopologyVerticesAlignmentAction& QtTopologyVerticesAlignmentAction::operator = (
										const QtTopologyVerticesAlignmentAction&)
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyVerticesAlignmentAction::operator =


QtTopologyVerticesAlignmentAction::~QtTopologyVerticesAlignmentAction ( )
{
}	// QtTopologyVerticesAlignmentAction::~QtTopologyVerticesAlignmentAction


QtTopologyVerticesAlignmentPanel*
					QtTopologyVerticesAlignmentAction::getTopologyVerticesAlignmentPanel ( )
{
	return dynamic_cast<QtTopologyVerticesAlignmentPanel*>(getOperationPanel ( ));
}	// QtTopologyVerticesAlignmentAction::getTopologyVerticesAlignmentPanel


void QtTopologyVerticesAlignmentAction::executeOperation ( )
{
	QtTopologyVerticesAlignmentPanel*	panel	= dynamic_cast<QtTopologyVerticesAlignmentPanel*>(getTopologyVerticesAlignmentPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres d'alignement des sommets topologiques :
	vector<string>	vertices	= panel->getVerticesNames ( );
	Math::Point		p1			= panel->getPoint1 ( );
	Math::Point		p2			= panel->getPoint2 ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= getContext ( ).getTopoManager ( ).alignVertices (p1, p2, vertices);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyVerticesAlignmentAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
