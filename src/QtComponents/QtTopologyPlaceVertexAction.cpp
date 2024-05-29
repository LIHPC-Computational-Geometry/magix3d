/**
 * \file        QtTopologyPlaceVertexAction.cpp
 * \author      Charles PIGNEROL
 * \date        01/02/2017
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologyPlaceVertexAction.h"

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

const double	zoomMin		= 0.2, zoomMax	= 5;
const double	zoomComul	= 0.75;


// ===========================================================================
//                        LA CLASSE QtTopologyPlaceVertexPanel
// ===========================================================================

QtTopologyPlaceVertexPanel::QtTopologyPlaceVertexPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).placeVertexOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).placeVertexOperationTag),
	  RenderingManager::InteractorObserver ( ),
	  _vertexPanel (0), _xTextField (0), _yTextField (0), _zTextField (0),
	  _sliderLabel(0), _interactorSlider (0),
	  _constraintCheckBox (0), _entityConstraintPanel (0), _refinementFactorTextField(0)
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

	// Le sommet à positionner :
	_vertexPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Sommet :", "", &mainWindow,
			SelectionManagerIfc::D0, FilterEntity::TopoVertex);
	connect (_vertexPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertexPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_vertexPanel);

	// Les composantes :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QVBoxLayout*	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	label	= new QLabel ("X :", this);
	vlayout->addWidget (label);
	label	= new QLabel ("Y :", this);
	vlayout->addWidget (label);
	label	= new QLabel ("Z :", this);
	vlayout->addWidget (label);
	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	_xTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_xTextField->setValue (0.);
	connect (_xTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesModifiedCallback ( )));
	vlayout->addWidget (_xTextField);
	_yTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_yTextField->setValue (0.);
	connect (_yTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesModifiedCallback ( )));
	vlayout->addWidget (_yTextField);
	_zTextField	= &QtNumericFieldsFactory::createPositionTextField (this);
	_zTextField->setValue (0.);
	connect (_zTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesModifiedCallback ( )));
	vlayout->addWidget (_zTextField);

	/** L'interacteur. */
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	_sliderLabel	= new QLabel ("Taille de l'interacteur.", this);
	_sliderLabel->setToolTip (QString::fromUtf8("Modifie la taille de l'interacteur et donc l'amplitude possible de déplacement du sommet."));
	hlayout->addWidget (_sliderLabel);
	_interactorSlider	= new QSlider (Qt::Horizontal, this);
	_interactorSlider->setRange (1, 100);
	_interactorSlider->setToolTip (QString::fromUtf8("Modifie la taille de l'interacteur et donc l'amplitude possible de déplacement du sommet."));
	hlayout->addWidget (_interactorSlider);
	connect (_interactorSlider, SIGNAL(valueChanged (int)), this,
	         SLOT (interactorSizeCallback ( )));
	_sliderLabel->hide ( );
	_interactorSlider->hide ( );

	/** L'éventuelle contrainte. */
	FilterEntity::objectType	filter	= (FilterEntity::objectType)(FilterEntity::AllEdges | FilterEntity::AllSurfaces);
	_entityConstraintPanel	= new QtEntityByDimensionSelectorPanel (
		this, mainWindow, "Entité de contrainte :",
		(SelectionManagerIfc::DIM)(SelectionManagerIfc::D1 | SelectionManagerIfc::D2),
		filter, SelectionManagerIfc::D1, false);
	_entityConstraintPanel->setToolTip ("Le sommet sera placé sur l'entité sélectionnée.\nPour déplacer le point presser le bouton gauche de la souris\net déplacer la souris.");
	connect (_entityConstraintPanel, SIGNAL (entitiesAddedToSelection(QString)), this,
	         SLOT (constraintSelectedCallback (QString)));
	connect (_entityConstraintPanel, SIGNAL (entitiesRemovedFromSelection(QString)), this,
	         SLOT (constraintUnselectedCallback (QString)));
	_entityConstraintPanel->hide ( );
	layout->addWidget (_entityConstraintPanel);
	QWidget*	factorWidget	= new QWidget (this);
	hlayout	= new QHBoxLayout ( );
	factorWidget->setLayout (hlayout);
	label	= new QLabel ("Facteur de raffinement :", factorWidget);
	hlayout->addWidget (label);
	_refinementFactorTextField	= new QtIntTextField (10, factorWidget);
	hlayout->addWidget (_refinementFactorTextField);
	_refinementFactorTextField->setRange (1, 10000);
	connect (_refinementFactorTextField, SIGNAL (returnPressed( )), this,
	         SLOT (constraintCallback ( )));
	label->setToolTip ("Facteur de raffinement de la représentation graphique pour positionnement interactif\n(ex 10 pour discrétisation 10x plus fine que la représentation graphique).");
	_refinementFactorTextField->setToolTip ("Facteur de raffinement de la représentation graphique pour positionnement interactif\n(ex 10 pour discrétisation 10x plus fine que la représentation graphique).");
	factorWidget->hide ( );
	layout->addWidget (factorWidget);
	_constraintCheckBox	= new QCheckBox (": contraindre sur une entité.", this);
	_constraintCheckBox->setToolTip ("Coché le sommet sera placé sur l'entité sélectionnée.\nPour déplacer le point presser le bouton gauche de la souris\net déplacer la souris.");
	connect (_constraintCheckBox, SIGNAL (stateChanged (int)), this, SLOT (constraintCallback ( )));
	layout->addWidget (_constraintCheckBox);

	layout->addStretch (2);
	addPreviewCheckBox (true);

	CHECK_NULL_PTR_ERROR (_vertexPanel->getNameTextField ( ))
	_vertexPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);
}	// QtTopologyPlaceVertexPanel::QtTopologyPlaceVertexPanel


QtTopologyPlaceVertexPanel::QtTopologyPlaceVertexPanel (const QtTopologyPlaceVertexPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  RenderingManager::InteractorObserver ( ),
	  _vertexPanel (0), _xTextField (0), _yTextField (0), _zTextField (0),
	  _sliderLabel(0), _interactorSlider (0),
	  _constraintCheckBox (0), _entityConstraintPanel (0), _refinementFactorTextField (0)
{
	MGX_FORBIDDEN ("QtTopologyPlaceVertexPanel copy constructor is not allowed.");
}	// QtTopologyPlaceVertexPanel::QtTopologyPlaceVertexPanel (const QtTopologyPlaceVertexPanel&)


QtTopologyPlaceVertexPanel& QtTopologyPlaceVertexPanel::operator = (
										const QtTopologyPlaceVertexPanel&)
{
	MGX_FORBIDDEN ("QtTopologyPlaceVertexPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyPlaceVertexPanel::QtTopologyPlaceVertexPanel (const QtTopologyPlaceVertexPanel&)


QtTopologyPlaceVertexPanel::~QtTopologyPlaceVertexPanel ( )
{
}	// QtTopologyPlaceVertexPanel::~QtTopologyPlaceVertexPanel


void QtTopologyPlaceVertexPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vertexPanel)
	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	CHECK_NULL_PTR_ERROR (_entityConstraintPanel)
	_vertexPanel->reset ( );
	_entityConstraintPanel->reset ( );
	_xTextField->setValue (0.);
	_yTextField->setValue (0.);
	_zTextField->setValue (0.);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyPlaceVertexPanel::reset


void QtTopologyPlaceVertexPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_vertexPanel)
	CHECK_NULL_PTR_ERROR (_entityConstraintPanel)

	_vertexPanel->stopSelection ( );
	_entityConstraintPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertexPanel->setUniqueName ("");
		_entityConstraintPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyPlaceVertexPanel::cancel


void QtTopologyPlaceVertexPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	CHECK_NULL_PTR_ERROR (_entityConstraintPanel)

	_vertexPanel->clearSelection ( );
	_entityConstraintPanel->clearSelection ( );

	_vertexPanel->actualizeGui (true);
	_entityConstraintPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyPlaceVertexPanel::autoUpdate


void QtTopologyPlaceVertexPanel::pointModifiedCallback (Math::Point point)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	_xTextField->setValue (point.getX ( ));
	_yTextField->setValue (point.getY ( ));
	_zTextField->setValue (point.getZ ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::pointModifiedCallback


void QtTopologyPlaceVertexPanel::constraintCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_sliderLabel)
	CHECK_NULL_PTR_ERROR (_interactorSlider)
	CHECK_NULL_PTR_ERROR (_entityConstraintPanel)
	CHECK_NULL_PTR_ERROR (_refinementFactorTextField->parentWidget ( ))

	const bool	constrained	= isContrained ( );
	_sliderLabel->setVisible (!constrained);
	_interactorSlider->setVisible (!constrained);
	_entityConstraintPanel->setVisible (constrained);
	_refinementFactorTextField->parentWidget ( )->setVisible (constrained);
	preview (false, true);	// Destruction interacteur
	preview (true, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::constraintCallback


void QtTopologyPlaceVertexPanel::constraintSelectedCallback (QString name)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_refinementFactorTextField)
	const size_t					factor		= _refinementFactorTextField->getValue ( );
	Entity*						entity		= &getContext().nameToEntity (name.toStdString ( ));
	RenderingManager::ConstrainedPointInteractor*	interactor	= getConstrainedInteractor ( );
	if (0 != interactor)
		interactor->setConstraint (entity, factor);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::constraintSelectedCallback


void QtTopologyPlaceVertexPanel::constraintUnselectedCallback (QString name)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Entity*	entity	= false == name.isEmpty ( ) ? &getContext().nameToEntity (name.toStdString ( )) : 0;
	if (0 != entity)
	{
		GraphicalEntityRepresentation*	rep	= getRenderingManager ( ).getRepresentation (*entity);
		if (0 != rep)
			rep->destroyRefinedRepresentation ( );
	}	// if (0 != entity)

	RenderingManager::ConstrainedPointInteractor*	interactor	= getConstrainedInteractor ( );
	if (0 != interactor)
		interactor->setConstraint (0, 1);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::constraintUnselectedCallback


void QtTopologyPlaceVertexPanel::preview (bool show, bool destroyInteractor)
{
	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	QtMgx3DOperationPanel::preview (show, destroyInteractor);
	if ((false == show) || (false == previewResult ( )))
	{
		if (0 != _sliderLabel)
			_sliderLabel->hide ( );
		if (0 != _interactorSlider)
			_interactorSlider->hide ( );
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	const bool	constrained	= isContrained ( );

	if (false == constrained)	// => Box interactor
	{
		double  dx = 0.1, dy = 0.1, dz = -0.1;
		getInteractorBoxDims (dx, dy, dz);
		Math::Point	p (getPoint ( ));
		RenderingManager::PointInteractor*	interactor	= getInteractor ( );
		if (0 == interactor)
		{
			if (0 != getVertex ( ))
			{
				interactor	= getRenderingManager ( ).createPointInteractor (p, dx, dy, dz, this);
				if (0 != _sliderLabel)
					_sliderLabel->show ( );
				if (0 != _interactorSlider)
				{
					const int	min		= _interactorSlider->minimum ( );
					const int	max		= _interactorSlider->maximum ( );
					const int	val		= (max - min) / 2;
					double		scale	=
						min == max ?  1. : fabs(val - min) / fabs(max - min);
					const double	zoomMin	= 0.2, zoomMax	= 5;
					scale	= zoomComul * scale * (zoomMax - zoomMin);
					interactor->setBoxDimensions (scale*dx, scale*dy, scale*dz);
					_interactorSlider->setValue (val);
					_interactorSlider->show ( );
				}
				registerPreviewedInteractor (interactor);
			}	// if (0 != getVertex ( ))
		}	// if (0 == interactor)
		else
			interactor->setPoint (p);
	}	// if (false == constrained)
	else	// => Curve/surface constrained interactor
	{
		CHECK_NULL_PTR_ERROR (_entityConstraintPanel)
		CHECK_NULL_PTR_ERROR (_entityConstraintPanel->getEntitiesPanel ( ))
		CHECK_NULL_PTR_ERROR (_refinementFactorTextField)
		const Math::Point	p (getPoint ( ));
		const size_t		factor	= _refinementFactorTextField->getValue ( );
		RenderingManager::ConstrainedPointInteractor*	interactor	= getConstrainedInteractor ( );
		if (0 == interactor)
		{
			const string	constraintName	= _entityConstraintPanel->getEntitiesPanel ( )->getUniqueName ( );
			Entity*		constraint	= false == constraintName.empty ( ) ?
							&getContext().nameToEntity (constraintName) : 0;
			// Cas tordu : constraint est une entité détruite (ex : undo) ;
			constraint	= 0 == constraint ? 0 : (true == constraint->isDestroyed ( ) ? 0 : constraint);
			interactor	= getRenderingManager ( ).createConstrainedPointInteractor (p, constraint, factor, this);
			registerPreviewedInteractor (interactor);
		}
	}	// else if (false == constrained)

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::preview


string QtTopologyPlaceVertexPanel::getVertexName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	return _vertexPanel->getUniqueName ( );
}	// QtTopologyPlaceVertexPanel::getVertexName


Math::Point QtTopologyPlaceVertexPanel::getPoint ( ) const
{
	return Math::Point (getX ( ), getY ( ), getZ ( ));
}	// QtTopologyPlaceVertexPanel::getPoint


double QtTopologyPlaceVertexPanel::getX ( ) const
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	return _xTextField->getValue ( );
}	// QtTopologyPlaceVertexPanel::getX


double QtTopologyPlaceVertexPanel::getY ( ) const
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	return _yTextField->getValue ( );
}	// QtTopologyPlaceVertexPanel::getY


double QtTopologyPlaceVertexPanel::getZ ( ) const
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	return _zTextField->getValue ( );
}	// QtTopologyPlaceVertexPanel::getZ


bool QtTopologyPlaceVertexPanel::isContrained ( ) const
{
	CHECK_NULL_PTR_ERROR (_constraintCheckBox)

	return _constraintCheckBox->isChecked ( );
}	// QtTopologyPlaceVertexPanel::isContrained


vector<Entity*> QtTopologyPlaceVertexPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	TopoEntity*		vertex	= getVertex ( );
	if (0 != vertex)
			entities.push_back (vertex);

	return entities;
}	// QtTopologyPlaceVertexPanel::getInvolvedEntities


Vertex* QtTopologyPlaceVertexPanel::getVertex ( ) const
{
	const string	name	= getVertexName ( );
	return getContext ( ).getTopoManager ( ).getVertex (name, false);
}	// QtTopologyPlaceVertexPanel::getVertex


RenderingManager::PointInteractor* QtTopologyPlaceVertexPanel::getInteractor ( )
{
	vector <RenderingManager::Interactor*>	interactors	= getPreviewedInteractors ( );
	if (0 != interactors.size ( ))
		return dynamic_cast<RenderingManager::PointInteractor*>(interactors[0]);

	return 0;
}	// QtTopologyPlaceVertexPanel::getInteractor


RenderingManager::ConstrainedPointInteractor* QtTopologyPlaceVertexPanel::getConstrainedInteractor ( )
{
	vector <RenderingManager::Interactor*>	interactors	= getPreviewedInteractors ( );
	if (0 != interactors.size ( ))
		return dynamic_cast<RenderingManager::ConstrainedPointInteractor*>(interactors[0]);

	return 0;
}	// QtTopologyPlaceVertexPanel::getConstrainedInteractor


void QtTopologyPlaceVertexPanel::operationCompleted ( )
{
//	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _vertexPanel)
	{
		_vertexPanel->stopSelection ( );
		//_vertexPanel->setUniqueName ("");
	}
	if (0 != _entityConstraintPanel)
		_entityConstraintPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyPlaceVertexPanel::operationCompleted


void QtTopologyPlaceVertexPanel::entitiesAddedToSelectionCallback (QString name)
{
	try
	{
		CHECK_NULL_PTR_ERROR (_xTextField)
		CHECK_NULL_PTR_ERROR (_yTextField)
		CHECK_NULL_PTR_ERROR (_zTextField)
		Vertex*		vertex	= getVertex ( );
		CHECK_NULL_PTR_ERROR (vertex)
		Math::Point	coords	= vertex->getCoord ( );
		_xTextField->setValue (coords.getX ( ));
		_yTextField->setValue (coords.getY ( ));
		_zTextField->setValue (coords.getZ ( ));
		coordinatesModifiedCallback ( );	// MAJ interacteur
	}
	catch (...)
	{
	}

	QtMgx3DOperationPanel::entitiesAddedToSelectionCallback (name);
}	// QtTopologyPlaceVertexPanel::entitiesAddedToSelectionCallback


void QtTopologyPlaceVertexPanel::entitiesRemovedFromSelectionCallback(QString n)
{
	try
	{
		CHECK_NULL_PTR_ERROR (_xTextField)
		CHECK_NULL_PTR_ERROR (_yTextField)
		CHECK_NULL_PTR_ERROR (_zTextField)
		_xTextField->setValue (0.);
		_yTextField->setValue (0.);
		_zTextField->setValue (0.);
		coordinatesModifiedCallback ( );	// MAJ interacteur
	}
	catch (...)
	{
	}

	QtMgx3DOperationPanel::entitiesRemovedFromSelectionCallback (n);
}	// QtTopologyPlaceVertexPanel::entitiesRemovedFromSelectionCallback


void QtTopologyPlaceVertexPanel::getInteractorBoxDims (double& dx, double& dy, double& dz)
{
	double		length	= 0.;
	Vertex*		vertex	= getVertex ( );
	if (0 != vertex)
	{
		const Math::Point		p1		= vertex->getCoord ( );
		const vector<CoEdge*>	edges	= vertex->getCoEdges ( );
		for (vector<CoEdge*>::const_iterator ite = edges.begin ( );
		     edges.end ( ) != ite; ite++)
		{
			Vertex*	opposite	= (*ite)->getOppositeVertex (vertex);
			if (0 != opposite)
			{
				const Math::Point	p2	= opposite->getCoord ( );
				const double		l	= p1.length (p2);
				length	= l > length ? l : length;
			}	// if (0 != opposite)
		}	// for (vector<CoEdge*>::const_iterator ite = ...

		if (0. < length)
		{
			dx	= dy	= dz	= length / 2.;
			return;
		}	// if (0. < length)
	}	// if (0 != vertex)

	double	box [6]	= { 0., 0., 0., 0., 0., 0. };
	getRenderingManager ( ).getBoundingBox (
						box [0], box [1], box [2], box [3], box [4], box [5]);
	dx	= box [1] - box [0];
	dy	= box [3] - box [2];
	dz	= box [5] - box [4];
	dx	= max (dx, dy);
	dx	= max (dx, dz);
	dx	= dy	= dz	= dx / 20.;
}	// QtTopologyPlaceVertexPanel::getInteractorBoxDims


void QtTopologyPlaceVertexPanel::coordinatesModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	RenderingManager::PointInteractor*		interactor	= getInteractor ( );
	RenderingManager::ConstrainedPointInteractor*	cinteractor	= getConstrainedInteractor ( );
	if (0 != interactor)
	{
		interactor->setPoint (getPoint ( ));
		getRenderingManager ( ).forceRender ( );
	}	// if (0 != interactor)
	if (0 != cinteractor)
	{
		cinteractor->setPoint (getPoint ( ));
		getRenderingManager ( ).forceRender ( );
	}	// if (0 != interactor)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::coordinatesModifiedCallback


void QtTopologyPlaceVertexPanel::interactorSizeCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	RenderingManager::PointInteractor*	interactor	= getInteractor ( );
	if (0 == interactor)
		return;

	CHECK_NULL_PTR_ERROR (_interactorSlider)
	const int	val		= _interactorSlider->value ( );
	const int	min		= _interactorSlider->minimum ( );
	const int	max		= _interactorSlider->maximum ( );
	double		scale	= min == max ?  1. : fabs(val - min) / fabs(max - min);
	scale	= zoomComul * scale;
	const double	zoomMin	= 0.2, zoomMax	= 5;

	scale	= scale * (zoomMax - zoomMin);
	interactor->setPoint (getPoint ( ));
	getRenderingManager ( ).forceRender ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getInteractorBoxDims (dx, dy, dz);
	interactor->setBoxDimensions (scale * dx, scale * dy, scale * dz);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyPlaceVertexPanel::interactorSizeCallback


// ===========================================================================
//                  LA CLASSE QtTopologyPlaceVertexAction
// ===========================================================================

QtTopologyPlaceVertexAction::QtTopologyPlaceVertexAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyPlaceVertexPanel*	operationPanel	= 
			new QtTopologyPlaceVertexPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyPlaceVertexAction::QtTopologyPlaceVertexAction


QtTopologyPlaceVertexAction::QtTopologyPlaceVertexAction (
										const QtTopologyPlaceVertexAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyPlaceVertexAction copy constructor is not allowed.")
}	// QtTopologyPlaceVertexAction::QtTopologyPlaceVertexAction


QtTopologyPlaceVertexAction& QtTopologyPlaceVertexAction::operator = (
										const QtTopologyPlaceVertexAction&)
{
	MGX_FORBIDDEN ("QtTopologyPlaceVertexAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyPlaceVertexAction::operator =


QtTopologyPlaceVertexAction::~QtTopologyPlaceVertexAction ( )
{
}	// QtTopologyPlaceVertexAction::~QtTopologyPlaceVertexAction


QtTopologyPlaceVertexPanel*
				QtTopologyPlaceVertexAction::getTopologyProjectVerticesPanel ( )
{
	return dynamic_cast<QtTopologyPlaceVertexPanel*>(getOperationPanel ( ));
}	// QtTopologyPlaceVertexAction::getTopologyProjectVerticesPanel


void QtTopologyPlaceVertexAction::executeOperation ( )
{
	QtTopologyPlaceVertexPanel*	panel	= dynamic_cast<QtTopologyPlaceVertexPanel*>(getTopologyProjectVerticesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de projection des sommets topologiques :
	const string	vertex	= panel->getVertexName ( );
	vector<string>	vertices;
	vertices.push_back (vertex);
	const double	x		= panel->getX ( );
	const double	y		= panel->getY ( );
	const double	z		= panel->getZ ( );

	cmdResult	= getContext ( ).getTopoManager ( ).setVertexLocation (vertices, true, x, true, y, true, z);

	setCommandResult (cmdResult);
}	// QtTopologyPlaceVertexAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
