/**
 * \file        QtTopologyProjectVerticesAction.cpp
 * \author      Charles PIGNEROL
 * \date        25/03/2014
 */

#include "Internal/ContextIfc.h"
#include "Internal/Context.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Entity.h"
#include "Utils/Spherical.h"
#include "Utils/Cylindrical.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologyProjectVerticesAction.h"
#include "Geom/Vertex.h"
#include "Topo/Vertex.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyProjectVerticesPanel
// ===========================================================================

QtTopologyProjectVerticesPanel::QtTopologyProjectVerticesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).projectVerticesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).projectVerticesOperationTag),
	  _verticesPanel (0), _xTextLabel(0), _yTextLabel(0), _zTextLabel(0),
	  _xTextField (0), _yTextField (0), _zTextField (0),
	  _xCheckBox (0), _yCheckBox (0), _zCheckBox (0), _coordTypeComboBox(0),
	  _sysCoordPanel(0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
			Resources::instance ( )._margin.getValue ( ),
			Resources::instance ( )._margin.getValue ( ),
			Resources::instance ( )._margin.getValue ( ),
			Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// Le nom du panneau :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

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

	// [EB] le type de coordonnées
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	QLabel* typeLabel	= new QLabel ("Type de coordonnées :", this);
	hlayout->addWidget (typeLabel);
	_coordTypeComboBox = new QComboBox (this);
	hlayout->addWidget (_coordTypeComboBox);
	_coordTypeComboBox->addItem(QString::fromUtf8("Cartésiennes"));
	_coordTypeComboBox->addItem(QString::fromUtf8("Sphériques"));
	_coordTypeComboBox->addItem(QString::fromUtf8("Cylindriques"));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// Les composantes :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QVBoxLayout*	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	_xTextLabel	= new QLabel ("X :", this);
	vlayout->addWidget (_xTextLabel);
	_yTextLabel	= new QLabel ("Y :", this);
	vlayout->addWidget (_yTextLabel);
	_zTextLabel	= new QLabel ("Z :", this);
	vlayout->addWidget (_zTextLabel);
	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	_xTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_xTextField->setValue (0.);
	vlayout->addWidget (_xTextField);
	_yTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_yTextField->setValue (0.);
	vlayout->addWidget (_yTextField);
	_zTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_zTextField->setValue (0.);
	vlayout->addWidget (_zTextField);
	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	_xCheckBox	= new QCheckBox (QString::fromUtf8("Modifier"), this);
	vlayout->addWidget (_xCheckBox);
	_yCheckBox	= new QCheckBox (QString::fromUtf8("Modifier"), this);
	vlayout->addWidget (_yCheckBox);
	_zCheckBox	= new QCheckBox (QString::fromUtf8("Modifier"), this);
	vlayout->addWidget (_zCheckBox);

	// [EB] L'ID de l'éventuel vertex dont on récupère les coordonnées :
	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);
	label	= new QLabel ("Vertex :", this);
	label->setMinimumSize (label->sizeHint ( ));
	hlayout->addWidget (label);
	_vertexIDTextField	= new QtEntityIDTextField (
			this, &mainWindow, SelectionManagerIfc::D0, FilterEntity::AllPoints);
	hlayout->addWidget (_vertexIDTextField);
	connect (_vertexIDTextField, SIGNAL (selectionModified (QString)), this,
	         SLOT (selectedVertexCallback(QString)));
	_vertexIDTextField->setToolTip(QString::fromUtf8("Sommet pour initialiser les coordonnées"));

	// [EB] repère local ou non
	_sysCoordPanel	= new QtMgx3DEntityPanel (
				this, "", true, "Repère :", "", &mainWindow,
				SelectionManagerIfc::D3, FilterEntity::SysCoord);
	_sysCoordPanel->setMultiSelectMode (false);
	layout->addWidget (_sysCoordPanel);

	layout->addStretch (2);

	// enchainement des sélections possible
	CHECK_NULL_PTR_ERROR (_verticesPanel->getNameTextField ( ))
	_vertexIDTextField->setLinkedSeizureManagers(_verticesPanel->getNameTextField ( ), 0);
	_verticesPanel->getNameTextField ( )->setLinkedSeizureManagers (0,_vertexIDTextField);

	connect (_coordTypeComboBox, SIGNAL (currentIndexChanged (int)),
			this, SLOT (coordTypeChangedCallback (int)));

}	// QtTopologyProjectVerticesPanel::QtTopologyProjectVerticesPanel


QtTopologyProjectVerticesPanel::QtTopologyProjectVerticesPanel (
									const QtTopologyProjectVerticesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _verticesPanel (0),  _xTextLabel(0), _yTextLabel(0), _zTextLabel(0),
	  _xTextField (0), _yTextField (0), _zTextField (0),
	  _xCheckBox (0), _yCheckBox (0), _zCheckBox (0), _vertexIDTextField(0),
	  _coordTypeComboBox(0),
	  _sysCoordPanel(0)
{
	MGX_FORBIDDEN ("QtTopologyProjectVerticesPanel copy constructor is not allowed.");
}	// QtTopologyProjectVerticesPanel::QtTopologyProjectVerticesPanel (const QtTopologyProjectVerticesPanel&)


QtTopologyProjectVerticesPanel& QtTopologyProjectVerticesPanel::operator = (
										const QtTopologyProjectVerticesPanel&)
{
	MGX_FORBIDDEN ("QtTopologyProjectVerticesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyProjectVerticesPanel::QtTopologyProjectVerticesPanel (const QtTopologyProjectVerticesPanel&)


QtTopologyProjectVerticesPanel::~QtTopologyProjectVerticesPanel ( )
{
}	// QtTopologyProjectVerticesPanel::~QtTopologyProjectVerticesPanel


void QtTopologyProjectVerticesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	_verticesPanel->reset ( );
	_xTextField->setValue (0.);
	_yTextField->setValue (0.);
	_zTextField->setValue (0.);
	_vertexIDTextField->reset ( );
	_sysCoordPanel->reset ( );
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyProjectVerticesPanel::reset


void QtTopologyProjectVerticesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_verticesPanel)

	_verticesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_verticesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyProjectVerticesPanel::cancel


void QtTopologyProjectVerticesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	CHECK_NULL_PTR_ERROR (_xCheckBox)
	CHECK_NULL_PTR_ERROR (_yCheckBox)
	CHECK_NULL_PTR_ERROR (_zCheckBox)

#ifdef AUTO_UPDATE_OLD_SCHEME
	// [EB] code non maintenu
//	if (true == autoUpdateUsesSelection ( ))
//	{
//		BEGIN_QT_TRY_CATCH_BLOCK
//
//		const vector<string>	selectedVertices	=
//			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
//		_verticesPanel->setUniqueNames (selectedVertices);
//		_xTextField->setText (QString (""));
//		_yTextField->setText (QString (""));
//		_zTextField->setText (QString (""));
//
//		// Ces sommets ont ils des composantes communes ?
//		bool	sameX	= true, sameY	= true, sameZ	= true;
//		double	x		= 0.,	y		= 0.,	z		= 0.;
//		bool	first	= true;
//		const vector<Entity*>	entities	= getInvolvedEntities ( );
//
//		for (vector<Entity*>::const_iterator it = entities.begin ( );
//		     entities.end ( ) != it; it++)
//		{
//			const Topo::Vertex*	vertex	=
//									dynamic_cast<const Topo::Vertex*>(*it);
//			if (0 != vertex)
//			{
//				double	vx	= vertex->getX ( );
//				double	vy	= vertex->getY ( );
//				double	vz	= vertex->getZ ( );
//				if (true == first)
//				{
//					x	= vx;
//					y	= vy;
//					z	= vz;
//					first	= false;
//				}	// if (true == first)
//				else
//				{
//					if (true == sameX)
//						if (fabs (vx - x) > MgxNumeric::mgxTopoDoubleEpsilon)
//							sameX	= false;
//					if (true == sameY)
//						if (fabs (vy - y) > MgxNumeric::mgxTopoDoubleEpsilon)
//							sameY	= false;
//					if (true == sameZ)
//						if (fabs (vz - z) > MgxNumeric::mgxTopoDoubleEpsilon)
//							sameZ	= false;
//				}	// else if (true == first)
//			}	// if (0 != vertex)
//		}	// for (vector<Entity*>::const_iterator it = ...
//		if (true == sameX)
//		{
//			_xTextField->setValue (x);
//			_xCheckBox->setChecked (Qt::Checked);
//		}
//		else
//			_xCheckBox->setChecked (Qt::Unchecked);
//		if (true == sameY)
//		{
//			_yTextField->setValue (y);
//			_yCheckBox->setChecked (Qt::Checked);
//		}
//		else
//			_yCheckBox->setChecked (Qt::Unchecked);
//		if (true == sameZ)
//		{
//			_zTextField->setValue (z);
//			_zCheckBox->setChecked (Qt::Checked);
//		}
//		else
//			_zCheckBox->setChecked (Qt::Unchecked);
//
//
//		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
//	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_verticesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_verticesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyProjectVerticesPanel::autoUpdate


vector<string> QtTopologyProjectVerticesPanel::getVerticesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getUniqueNames ( );
}	// QtTopologyProjectVerticesPanel::getVerticesNames

string QtTopologyProjectVerticesPanel::getSysCoordName() const
{
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	return _sysCoordPanel->getUniqueName();
}

double QtTopologyProjectVerticesPanel::getX ( ) const
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	return _xTextField->getValue ( );
}	// QtTopologyProjectVerticesPanel::getX


double QtTopologyProjectVerticesPanel::getY ( ) const
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	return _yTextField->getValue ( );
}	// QtTopologyProjectVerticesPanel::getY


double QtTopologyProjectVerticesPanel::getZ ( ) const
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	return _zTextField->getValue ( );
}	// QtTopologyProjectVerticesPanel::getZ


bool QtTopologyProjectVerticesPanel::projectX ( ) const
{
	CHECK_NULL_PTR_ERROR (_xCheckBox)
	return Qt::Checked == _xCheckBox->checkState ( ) ? true : false;
}	// QtTopologyProjectVerticesPanel::projectX


bool QtTopologyProjectVerticesPanel::projectY ( ) const
{
	CHECK_NULL_PTR_ERROR (_yCheckBox)
	return Qt::Checked == _yCheckBox->checkState ( ) ? true : false;
}	// QtTopologyProjectVerticesPanel::projectY


bool QtTopologyProjectVerticesPanel::projectZ ( ) const
{
	CHECK_NULL_PTR_ERROR (_zCheckBox)
	return Qt::Checked == _zCheckBox->checkState ( ) ? true : false;
}	// QtTopologyProjectVerticesPanel::projectZ


vector<Entity*> QtTopologyProjectVerticesPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names	= getVerticesNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*		vertex	=
			getContext ( ).getTopoManager ( ).getVertex (*it, false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopologyProjectVerticesPanel::getInvolvedEntities


void QtTopologyProjectVerticesPanel::operationCompleted ( )
{
//	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _verticesPanel)
	{
		_verticesPanel->stopSelection ( );
		//_verticesPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyProjectVerticesPanel::operationCompleted


void QtTopologyProjectVerticesPanel::selectedVertexCallback (const QString name)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	updateCoordinates (name.toStdString ( ));
	emit pointModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Sélection d'un sommet pour modifier les coordonnées d'un sommet")
}	// QtTopologyProjectVerticesPanel::selectedVertexCallback

void QtTopologyProjectVerticesPanel::updateCoordinates (const string& name)
{
	// [EB] identique à QtMgx3DPointPanel::updateCoordinates

	CHECK_NULL_PTR_ERROR (_vertexIDTextField)

	Utils::Math::Point pt;

	if (0 != _vertexIDTextField->getObjectsCount ( ))
	{
		switch (_vertexIDTextField->getObjectType ( ))
		{
			case Entity::GeomVertex	:
			{
				const Geom::Vertex*	vertex	=
					getContext ().getGeomManager ( ).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex);
				pt = vertex->getCoord();
			}	// case Entity::GeomVertex
			break;
			case Entity::TopoVertex	:
			{
				const Topo::Vertex*	vertex	=
					getContext ().getTopoManager ( ).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex);
				pt = vertex->getCoord();
			}	// case Entity::TopoVertex
			break;
			default					:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Entité de type "
			        << Entity::objectTypeToObjectTypeName (_vertexIDTextField->getObjectType ( ))
			        << " (" << (unsigned long)_vertexIDTextField->getObjectType ( )
			        << ") non supporté pour la saisie d'un point.";
				throw Exception (message);
			}	// default
		}	// switch (_vertexIDTextField->getObjectType ( ))
	}	// if (0 != _vertexIDTextField->getObjectsCount ( ))

	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)

	// on met les coordonnées dans le repère sélectionné si c'est le cas
	string repName = getSysCoordName();
	if (!repName.empty()){
		Internal::ContextIfc* ctxifc = &getContext();
		Internal::Context* ctx = dynamic_cast<Internal::Context*>(ctxifc);
		CHECK_NULL_PTR_ERROR(ctx);
		CoordinateSystem::SysCoord* rep = ctx->getLocalSysCoordManager().getSysCoord(repName, true);
		pt = rep->toLocal(pt);
	}

	coordinateType ct = getTypeCoordinate();

	if (ct == cartesian){
		_xTextField->setText (QString::number (pt.getX()));
		_yTextField->setText (QString::number (pt.getY()));
		_zTextField->setText (QString::number (pt.getZ()));
	}
	else if (ct == spherical){
		Utils::Math::Spherical sph(pt);
		_xTextField->setText (QString::number (sph.getRho()));
		_yTextField->setText (QString::number (sph.getTheta()));
		_zTextField->setText (QString::number (sph.getPhi()));
	}
	else if (ct == cylindrical){
		Utils::Math::Cylindrical cyl(pt);
		_xTextField->setText (QString::number (cyl.getRho()));
		_yTextField->setText (QString::number (cyl.getPhi()));
		_zTextField->setText (QString::number (cyl.getZ()));
	}
	else
		throw Exception ("Erreur interne, coordinateType de updateCoordinates en dehors des clous");

}	// QtTopologyProjectVerticesPanel::updateCoordinates


void QtTopologyProjectVerticesPanel::coordTypeChangedCallback (int index)
{
	CHECK_NULL_PTR_ERROR(_xTextLabel)
	CHECK_NULL_PTR_ERROR(_yTextLabel)
	CHECK_NULL_PTR_ERROR(_zTextLabel)

	coordinateType ct = getTypeCoordinate();
	if (ct == cartesian){
		_xTextLabel->setText("X :");
		_yTextLabel->setText("Y :");
		_zTextLabel->setText("Z :");
	}
	else if (ct == spherical){
		_xTextLabel->setText("Rhô :");
		_yTextLabel->setText("Thêta :");
		_zTextLabel->setText("Phi :");
	}
	else if (ct == cylindrical){
		_xTextLabel->setText("Rhô :");
		_yTextLabel->setText("Phi :");
		_zTextLabel->setText("Z :");
	}
	else
		throw Exception ("Erreur interne, coordinateType de coordTypeChangedCallback en dehors des clous");

	if (_vertexIDTextField->getObjectsCount ( ) != 0)
		updateCoordinates(_vertexIDTextField->getText());

} // QtTopologyProjectVerticesPanel::coordTypeChangedCallback


QtTopologyProjectVerticesPanel::coordinateType QtTopologyProjectVerticesPanel::getTypeCoordinate()
{
	CHECK_NULL_PTR_ERROR(_coordTypeComboBox);
	if (_coordTypeComboBox->currentIndex() == 0)
		return cartesian;
	else if (_coordTypeComboBox->currentIndex() == 1)
		return spherical;
	else if (_coordTypeComboBox->currentIndex() == 2)
		return cylindrical;
	else
		throw Exception ("Erreur interne, index de _coordTypeComboBox en dehors des clous");
}

// ===========================================================================
//                  LA CLASSE QtTopologyProjectVerticesAction
// ===========================================================================

QtTopologyProjectVerticesAction::QtTopologyProjectVerticesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyProjectVerticesPanel*	operationPanel	= 
			new QtTopologyProjectVerticesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyProjectVerticesAction::QtTopologyProjectVerticesAction


QtTopologyProjectVerticesAction::QtTopologyProjectVerticesAction (
										const QtTopologyProjectVerticesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyProjectVerticesAction copy constructor is not allowed.")
}	// QtTopologyProjectVerticesAction::QtTopologyProjectVerticesAction


QtTopologyProjectVerticesAction& QtTopologyProjectVerticesAction::operator = (
										const QtTopologyProjectVerticesAction&)
{
	MGX_FORBIDDEN ("QtTopologyProjectVerticesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyProjectVerticesAction::operator =


QtTopologyProjectVerticesAction::~QtTopologyProjectVerticesAction ( )
{
}	// QtTopologyProjectVerticesAction::~QtTopologyProjectVerticesAction


QtTopologyProjectVerticesPanel*
					QtTopologyProjectVerticesAction::getTopologyProjectVerticesPanel ( )
{
	return dynamic_cast<QtTopologyProjectVerticesPanel*>(getOperationPanel ( ));
}	// QtTopologyProjectVerticesAction::getTopologyProjectVerticesPanel


void QtTopologyProjectVerticesAction::executeOperation ( )
{
	QtTopologyProjectVerticesPanel*	panel	= dynamic_cast<QtTopologyProjectVerticesPanel*>(getTopologyProjectVerticesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de projection des sommets topologiques :
	vector<string>	vertices	= panel->getVerticesNames ( );
	bool	changeX	= panel->projectX ( );
	bool	changeY	= panel->projectY ( );
	bool	changeZ	= panel->projectZ ( );
	double	x		= true == changeX ? panel->getX ( ) : 0.;
	double	y		= true == changeY ? panel->getY ( ) : 0.;
	double	z		= true == changeZ ? panel->getZ ( ) : 0.;


	QtTopologyProjectVerticesPanel::coordinateType ct = panel->getTypeCoordinate();
	string sysCoordName = panel->getSysCoordName();

	if (ct == QtTopologyProjectVerticesPanel::cartesian){
		cmdResult	= getContext ( ).getTopoManager ( ).setVertexLocation (vertices, changeX, x, changeY, y, changeZ, z, sysCoordName);
	}
	else if (ct == QtTopologyProjectVerticesPanel::spherical){
		cmdResult	= getContext ( ).getTopoManager ( ).setVertexSphericalLocation (vertices, changeX, x, changeY, y, changeZ, z, sysCoordName);
	}
	else if (ct == QtTopologyProjectVerticesPanel::cylindrical){
		cmdResult	= getContext ( ).getTopoManager ( ).setVertexCylindricalLocation (vertices, changeX, x, changeY, y, changeZ, z, sysCoordName);
	}
	else
		throw Exception ("Erreur interne, coordinateType de executeOperation en dehors des clous");

	setCommandResult (cmdResult);
}	// QtTopologyProjectVerticesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
