/**
 * \file        VTKRenderingManager.cpp
 * \author      Charles PIGNEROL
 * \date        04/06/2012
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/VTKRenderingManager.h"
#include "QtVtkComponents/vtkCustomizableInteractorStyleTrackball.h"
#include "QtVtkComponents/VTKMgx3DPickerCommand.h"
#include "QtVtkComponents/VTKMgx3DGeomEntityRepresentation.h"
#include "QtVtkComponents/VTKGraphicalRepresentationFactory.h"
#include "QtVtkComponents/VTKMgx3DMeshEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DTopoEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DSysCoordEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DStructuredMeshEntityRepresentation.h"
#include "QtVtkComponents/VTKGMDSEntityRepresentation.h"
#include "QtVtkComponents/VTKSelectionManagerDimFilter.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "VtkComponents/vtkECMFactory.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Topo/TopoEntity.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Mesh/MeshEntity.h"
#include "Mesh/SubSurface.h"
#include "Mesh/SubVolume.h"
#include "Utils/DisplayProperties.h"
#include "Utils/ErrorManagement.h"
#include "Utils/Vector.h"
#include "SysCoord/SysCoord.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NumericServices.h>

#include <iostream>
#include <memory>

#include <QThread>
#include <QBoxLayout>


#include <vtkArcSource.h>
#include <vtkArrowSource.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkOutlineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkAssembly.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellType.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkLODActor.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVector.h>
#include <vtkVertex.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkDebugLeaks.h>

#include <set>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Structured;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::CoordinateSystem;


namespace Mgx3D
{

namespace QtVtkComponents
{

// ===========================================================================
//    LA CLASSE VTKRenderingManager::VTKRenderingManagerInteractionUpdater
// ===========================================================================

VTKRenderingManager::VTKRenderingManagerInteractionUpdater::VTKRenderingManagerInteractionUpdater (VTKRenderingManager& renderingManager)
	: vtkCommand ( ), _renderingManager (&renderingManager)
{
}	// VTKRenderingManagerInteractionUpdater::VTKRenderingManagerInteractionUpdater


VTKRenderingManager::VTKRenderingManagerInteractionUpdater::VTKRenderingManagerInteractionUpdater (
			const VTKRenderingManager::VTKRenderingManagerInteractionUpdater&)
	: vtkCommand ( ), _renderingManager (0)
{
	MGX_FORBIDDEN ("VTKRenderingManagerInteractionUpdater copy constructor is not allowed.");
}	// VTKRenderingManagerInteractionUpdater::VTKRenderingManagerInteractionUpdater


VTKRenderingManager::VTKRenderingManagerInteractionUpdater&
	VTKRenderingManager::VTKRenderingManagerInteractionUpdater::operator = (
			const VTKRenderingManager::VTKRenderingManagerInteractionUpdater&)
{
	MGX_FORBIDDEN ("VTKRenderingManagerInteractionUpdater assignment operator is not allowed.");
	return *this;
}	// VTKRenderingManagerInteractionUpdater::operator =


VTKRenderingManager::VTKRenderingManagerInteractionUpdater::~VTKRenderingManagerInteractionUpdater ( )
{
}	// VTKRenderingManagerInteractionUpdater::~VTKRenderingManagerInteractionUpdater


void VTKRenderingManager::VTKRenderingManagerInteractionUpdater::Execute (vtkObject*, unsigned long eventId, void*)
{
	if (0 != _renderingManager)
	{
		if (vtkMgx3DInteractorStyle::ViewRedefinedEvent == eventId)
			_renderingManager->focalPointModifiedCallback ( );
	}	// if (0 != _renderingManager)
}	// VTKRenderingManagerInteractionUpdater::Execute



// ===========================================================================
//              LA CLASSE VTKRenderingManager::VTKColorTable
// ===========================================================================

VTKRenderingManager::VTKColorTable::VTKColorTable (VTKRenderingManager& rm,  const RenderingManager::ColorTableDefinition& def)
	: RenderingManager::ColorTable (rm, RenderingManager::ColorTableDefinition ( )),
	  _scalarBarWidget (0), _renderingManager (&rm)
{
	_scalarBarWidget		= vtkScalarBarWidget::New ( );
	vtkLookupTable*	table	= vtkLookupTable::New ( );
	CHECK_NULL_PTR_ERROR (_scalarBarWidget)
	CHECK_NULL_PTR_ERROR (_scalarBarWidget->GetScalarBarActor ( ))
	CHECK_NULL_PTR_ERROR (table)
	_scalarBarWidget->GetScalarBarActor ( )->SetLookupTable (table);
	setValueName (def.valueName);
	setColorNum (def.colorNum);
	setExtrema (def.domainMin, def.domainMax);
//	table->SetTableRange (min, max);
//	table->SetNumberOfTableValues (colorNum);
//	table->Build ( );
	_scalarBarWidget->GetScalarBarActor ( )->SetTitle (def.valueName.c_str ( ));
	table->Delete ( );	table	= 0;

	vtkScalarBarRepresentation*	representation	= _scalarBarWidget->GetScalarBarRepresentation ( );
	CHECK_NULL_PTR_ERROR (representation)
	// On se situe à 2pc de la bordure droite :
	representation->SetPosition (0.93, 0.02);
	representation->SetPosition2 (0.05, 0.96);	// Largeur, hauteur en fait
}	// VTKColorTable::VTKColorTable


VTKRenderingManager::VTKColorTable::VTKColorTable (const VTKRenderingManager::VTKColorTable&)
	: RenderingManager::ColorTable (
		*new RenderingManager ( ), RenderingManager::ColorTableDefinition ( )),
	  _scalarBarWidget (0), _renderingManager (0)
{
	MGX_FORBIDDEN ("VTKRenderingManager::ColorTable copy constructor is not allowed.");
}	// VTKColorTable::VTKColorTable


VTKRenderingManager::VTKColorTable& VTKRenderingManager::VTKColorTable::operator = (const VTKRenderingManager::VTKColorTable&)
{
	MGX_FORBIDDEN ("VTKRenderingManager::ColorTable assignment operator is not allowed.");
	return *this;
}	// VTKColorTable::operator =


VTKRenderingManager::VTKColorTable::~VTKColorTable ( )
{
	display (false);
	if (0 != _scalarBarWidget)
		_scalarBarWidget->Delete ( );
	_scalarBarWidget	= 0;
}	// VTKColorTable::~VTKColorTable


void VTKRenderingManager::VTKColorTable::display (bool show)
{
	getScalarBarWidget ( ).SetEnabled (show);
	getRenderingManager ( ).forceRender ( );
}	// VTKColorTable::display


void VTKRenderingManager::VTKColorTable::setColorNum (size_t colorNum)
{
	if (colorNum != getColorNum ( ))
	{
		RenderingManager::ColorTable::setColorNum (colorNum);

		getLookupTable ( ).SetNumberOfTableValues (colorNum);
		getLookupTable ( ).Build ( );
		getScalarBarWidget ( ).Render ( );
	}	// if (colorNum != getColorNum ( ))
}	// VTKColorTable::setColorNum


void VTKRenderingManager::VTKColorTable::setExtrema (double min, double max)
{
	RenderingManager::ColorTable::setExtrema (min, max);

	getLookupTable ( ).SetTableRange (min, max);
	getLookupTable ( ).Build ( );
	getScalarBarWidget ( ).Render ( );
}	// VTKColorTable::setExtrema


void VTKRenderingManager::VTKColorTable::setDefinition (const RenderingManager::ColorTableDefinition& definition)
{
	if (definition != getDefinition ( ))
	{	// Eviter de multiples render :
		CHECK_NULL_PTR_ERROR (getScalarBarWidget ( ).GetScalarBarActor ( ))
		ColorTable::setDefinition (definition);
		getScalarBarWidget ( ).GetScalarBarActor ( )->SetTitle (definition.valueName.c_str ( ));
		getLookupTable ( ).SetNumberOfTableValues (definition.colorNum);
		getLookupTable ( ).SetTableRange (definition.domainMin, definition.domainMax);
		getLookupTable ( ).Build ( );
		getScalarBarWidget ( ).Render ( );

		// Actualiser les acteurs concernés :
		vector<Entity*> entities    = getRenderingManager ( ).getDisplayedEntities ( );
		for (vector<Entity*>::iterator ite = entities.begin ( ); entities.end ( ) != ite; ite++)
		{
			DisplayProperties&  properties  = (*ite)->getDisplayProperties ( );
			if (properties.getValueName ( ) != definition.valueName)
				continue;

			DisplayProperties::GraphicalRepresentation* rep = properties.getGraphicalRepresentation ( );
			if (0 != rep)
				rep->updateRepresentationProperties ( );
		}   // for (vector<Entity*>::iterator ite = entities.begin ( ); entities.end ( ) != ite; ite++)

		getRenderingManager ( ).forceRender ( );
	}	// if (definition != getDefinition)
}	// VTKColorTable::setDefinition


const vtkScalarBarWidget& VTKRenderingManager::VTKColorTable::getScalarBarWidget ( ) const
{
	CHECK_NULL_PTR_ERROR (_scalarBarWidget)
	return *_scalarBarWidget;
}	// VTKColorTable::getScalarBarWidget


vtkScalarBarWidget& VTKRenderingManager::VTKColorTable::getScalarBarWidget ( )
{
	CHECK_NULL_PTR_ERROR (_scalarBarWidget)
	return *_scalarBarWidget;
}	// VTKColorTable::getScalarBarWidget


const vtkLookupTable& VTKRenderingManager::VTKColorTable::getLookupTable ( ) const
{
	vtkScalarBarWidget&	w	= (vtkScalarBarWidget&)getScalarBarWidget ( );
	vtkScalarBarActor*	a	= w.GetScalarBarActor ( );
	CHECK_NULL_PTR_ERROR (a)
	vtkLookupTable*	lut	= dynamic_cast<vtkLookupTable*>(a->GetLookupTable ( ));
	CHECK_NULL_PTR_ERROR (lut)

	return *lut;
}	// VTKColorTable::getLookupTable


vtkLookupTable& VTKRenderingManager::VTKColorTable::getLookupTable ( )
{
	CHECK_NULL_PTR_ERROR (getScalarBarWidget ( ).GetScalarBarActor ( ))
	CHECK_NULL_PTR_ERROR (getScalarBarWidget ( ).GetScalarBarActor ( )->GetLookupTable ( ))
	vtkLookupTable*	lut	= dynamic_cast<vtkLookupTable*>(
			getScalarBarWidget ( ).GetScalarBarActor ( )->GetLookupTable ( ));
	CHECK_NULL_PTR_ERROR (lut)

	return *lut;
}	// VTKColorTable::getLookupTable


const VTKRenderingManager& VTKRenderingManager::VTKColorTable::getRenderingManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_renderingManager)
	return *_renderingManager;
}	// VTKColorTable::getRenderingManager


VTKRenderingManager& VTKRenderingManager::VTKColorTable::getRenderingManager ( )
{
	CHECK_NULL_PTR_ERROR (_renderingManager)
	return *_renderingManager;
}	// VTKColorTable::getRenderingManager


// ===========================================================================
//              LA CLASSE VTKRenderingManager::VTKPointInteractor
// ===========================================================================


VTKRenderingManager::VTKPointInteractor::VTKPointInteractor (
	Math::Point point, double dx, double dy, double dz,
	VTKRenderingManager& renderingManager,
	RenderingManager::InteractorObserver* observer)
	: RenderingManager::PointInteractor (observer),
	  _pointWidget (0), _callback (0),
	  _dx (fabs (dx)), _dy (fabs (dy)), _dz (fabs (dz)),
	  _x (point.getX ( )), _y (point.getY ( )), _z (point.getZ ( ))
{
	_boundingBox [0]	= _x - _dx / 2.;
	_boundingBox [1]	= _x + _dx / 2.;
	_boundingBox [2]	= _y - _dy / 2.;
	_boundingBox [3]	= _y + _dy / 2.;
	_boundingBox [4]	= _z - _dz / 2.;
	_boundingBox [5]	= _z + _dz / 2.;
	_pointWidget	= vtkPointWidget::New ( );
	_pointWidget->SetInteractor (renderingManager.getRenderWindow ( ).GetInteractor ( ));
	// On zoom le widget pour éviter sa juxtaposition avec la bounding box :
	_pointWidget->SetPlaceFactor (1.5);
	setPoint (point);
	_pointWidget->On ( );
	_callback	= new VTKInteractorCallback<VTKPointInteractor> (this, _pointWidget);
}	// VTKPointInteractor::VTKPointInteractor


VTKRenderingManager::VTKPointInteractor::VTKPointInteractor (const VTKRenderingManager::VTKPointInteractor&)
	: RenderingManager::PointInteractor (0),
	  _pointWidget (0), _callback (0), _dx (0.), _dy (0.), _dz (0.),
	  _x (0.), _y (0.), _z (0.)
{
}	// VTKPointInteractor::VTKPointInteractor


VTKRenderingManager::VTKPointInteractor& VTKRenderingManager::VTKPointInteractor::operator = (const VTKRenderingManager::VTKPointInteractor&)
{
	return *this;
}	// VTKPointInteractor::operator =


VTKRenderingManager::VTKPointInteractor::~VTKPointInteractor ( )
{
	if (0 != _callback)
		_callback->Delete ( );
	_callback	= 0;

	if (0 != _pointWidget)
	{
		_pointWidget->Off ( );			// Mandatory (core dump ...)
		_pointWidget->SetInteractor (0);	// Mandatory (core dump ...)
		_pointWidget->Delete ( );
	}
	_pointWidget	= 0;
}	// VTKPointInteractor::~VTKPointInteractor


Math::Point VTKRenderingManager::VTKPointInteractor::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointWidget)
	double	point [3]	= { 0., 0., 0. };
	_pointWidget->GetPosition (point);

	return Math::Point (point [0], point [1], point [2]);
}	// VTKPointInteractor::getPoint


void VTKRenderingManager::VTKPointInteractor::setPoint (Math::Point point)
{
	CHECK_NULL_PTR_ERROR (_pointWidget)
	_x	= point.getX ( );
	_y	= point.getY ( );
	_z	= point.getZ ( );
	vtkFloatingPointType	box [6]	= {
						_x - _dx / 2., _x + _dx / 2.,
						_y - _dy / 2., _y + _dy / 2.,
						_z - _dz / 2., _z + _dz / 2. };
	_pointWidget->PlaceWidget (box);
	_pointWidget->SetPosition (_x, _y, _z);
}	// VTKPointInteractor::setPoint


void VTKRenderingManager::VTKPointInteractor::getBoxDimensions (double& dx, double& dy, double& dz) const
{
	dx	= _dx;
	dy	= _dy;
	dz	= _dz;
}	// VTKPointInteractor::getBoxDimensions


void VTKRenderingManager::VTKPointInteractor::setBoxDimensions (double dx, double dy, double dz)
{
	_dx	= dx;
	_dy	= dy;
	_dz	= dz;
	_boundingBox [0]	= _x - _dx / 2.;
	_boundingBox [1]	= _x + _dx / 2.;
	_boundingBox [2]	= _y - _dy / 2.;
	_boundingBox [3]	= _y + _dy / 2.;
	_boundingBox [4]	= _z - _dz / 2.;
	_boundingBox [5]	= _z + _dz / 2.;
	_pointWidget->PlaceWidget (_boundingBox);
}	// VTKPointInteractor::setBoxDimensions


void VTKRenderingManager::VTKPointInteractor::vtkInteractorModified ( )
{
	notifyObserverForModifications ( );
}	// VTKPointInteractor::vtkInteractorModified


// ===========================================================================
//              LA CLASSE VTKRenderingManager::VTKConstrainedPointInteractor
// ===========================================================================


VTKRenderingManager::VTKConstrainedPointInteractor::VTKConstrainedPointInteractor (
	Math::Point point, Entity* constraint, size_t factor,
	VTKRenderingManager& renderingManager,
	RenderingManager::InteractorObserver* observer)
	: RenderingManager::ConstrainedPointInteractor (observer),
	  _pointWidget (0), _constraintActor (0), _callback (0),
	  _x (point.getX ( )), _y (point.getY ( )), _z (point.getZ ( ))
{
	_pointWidget		= vtkConstrainedPointWidget::New ( );
	CHECK_NULL_PTR_ERROR (_pointWidget->GetRepresentation ( ))
	_pointWidget->GetRepresentation ( );	// => CreateDefaultRepresentation ( )
	_pointWidget->SetInteractor (renderingManager.getRenderWindow ( ).GetInteractor ( ));
	// On zoom le widget pour éviter sa juxtaposition avec la bounding box :
	_pointWidget->GetRepresentation ( )->SetPlaceFactor (2.5);
	_pointWidget->GetRepresentation ( )->SetHandleSize (30);
	_pointWidget->On ( );
	_callback	= new VTKInteractorCallback<VTKConstrainedPointInteractor> (this, _pointWidget);
	// CP ATTENTION : setPoint doit être fait après setConstraint car setConstraint choisi le
	// point sous le curseur de la souris à l'emplacement du dernier évènement.
	setConstraint (constraint, factor);
	setPoint (point);
}	// VTKConstrainedPointInteractor::VTKConstrainedPointInteractor


VTKRenderingManager::VTKConstrainedPointInteractor::VTKConstrainedPointInteractor (
					const VTKRenderingManager::VTKConstrainedPointInteractor&)
	: RenderingManager::ConstrainedPointInteractor (0),
	  _pointWidget (0), _constraintActor (0), _callback (0), _x (0.), _y (0.), _z (0.)
{
}	// VTKConstrainedPointInteractor::VTKConstrainedPointInteractor


VTKRenderingManager::VTKConstrainedPointInteractor&
			VTKRenderingManager::VTKConstrainedPointInteractor::operator = (
					const VTKRenderingManager::VTKConstrainedPointInteractor&)
{
	return *this;
}	// VTKConstrainedPointInteractor::operator =


VTKRenderingManager::VTKConstrainedPointInteractor::~VTKConstrainedPointInteractor ( )
{
	if (0 != _callback)
		_callback->Delete ( );
	_callback	= 0;

	if (0 != _pointWidget)
	{
		if ((0 != _constraintActor) && (0 != _pointWidget->GetCurrentRenderer ( )) &&
		    (false != _pointWidget->GetCurrentRenderer ( )->HasViewProp (_constraintActor)))
			_pointWidget->GetCurrentRenderer ( )->RemoveViewProp (_constraintActor);
		_pointWidget->Off ( );			// Mandatory (core dump ...)
		_pointWidget->SetInteractor (0);	// Mandatory (core dump ...)
		_pointWidget->Delete ( );
	}
	_pointWidget	= 0;
}	// VTKConstrainedPointInteractor::~VTKConstrainedPointInteractor


Math::Point VTKRenderingManager::VTKConstrainedPointInteractor::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointWidget)
	CHECK_NULL_PTR_ERROR (_pointWidget->GetConstrainedPointRepresentation ( ))
	double	point [3]	= { 0., 0., 0. };
	_pointWidget->GetConstrainedPointRepresentation ( )->GetWorldPosition (point);

	return Math::Point (point [0], point [1], point [2]);
}	// VTKConstrainedPointInteractor::getPoint


void VTKRenderingManager::VTKConstrainedPointInteractor::setPoint (Math::Point point)
{
	CHECK_NULL_PTR_ERROR (_pointWidget)
	CHECK_NULL_PTR_ERROR (_pointWidget->GetConstrainedPointRepresentation ( ))
	_x	= point.getX ( );
	_y	= point.getY ( );
	_z	= point.getZ ( );
	double	bounds [6]	= { _x - 10, _y - 10, _z - 10, _x + 10, _y + 10, _z + 10 };
	_pointWidget->GetRepresentation ( )->PlaceWidget (bounds);
	double	pos [3]	= { _x, _y, _z };

	_pointWidget->GetConstrainedPointRepresentation ( )->SetWorldPosition (pos);
	_pointWidget->On ( );
}	// VTKConstrainedPointInteractor::setPoint


void VTKRenderingManager::VTKConstrainedPointInteractor::setConstraint (Entity* constraint, size_t factor)
{
	CHECK_NULL_PTR_ERROR (_pointWidget)
	if ((0 != _constraintActor) && (0 != _pointWidget->GetCurrentRenderer ( )) &&
	    (false != _pointWidget->GetCurrentRenderer ( )->HasViewProp (_constraintActor)))
		_pointWidget->GetCurrentRenderer ( )->RemoveViewProp (_constraintActor);
	_constraintActor	= 0;
	_pointWidget->GetConstrainedPointRepresentation ( )->GetUGridPointPlacer ( )->SetGrid (0);
	_pointWidget->GetConstrainedPointRepresentation ( )->GetUGridPointPlacer ( )->RemoveAllProps ( );
	_pointWidget->Off ( );

	vtkUnstructuredGrid*	grid	= 0;
	if (0 != _pointWidget)
	{
		DisplayProperties::GraphicalRepresentation*	rep	= 0 == constraint ?
				0 : constraint->getDisplayProperties ( ).getGraphicalRepresentation ( );
		VTKEntityRepresentation*	vtkRep	= dynamic_cast<VTKEntityRepresentation*>(rep);
		if (0 != rep)
			rep->createRefinedRepresentation (factor);
		grid			= 0 == vtkRep ? 0 : vtkRep->getRefinedGrid ( );
		_constraintActor	= 0 == vtkRep ? 0 : vtkRep->getRefinedActor ( );
	}	// if (0 != _pointWidget)
	_pointWidget->GetConstrainedPointRepresentation ( )->GetUGridPointPlacer ( )->SetGrid (grid);
	if (0 != _constraintActor)
	{
		_pointWidget->GetConstrainedPointRepresentation ( )->GetUGridPointPlacer ( )->AddProp (_constraintActor);
		_pointWidget->On ( );
		if ((0 != _pointWidget->GetCurrentRenderer ( )) &&
		    (false == _pointWidget->GetCurrentRenderer ( )->HasViewProp (_constraintActor)))
			_pointWidget->GetCurrentRenderer ( )->AddViewProp (_constraintActor);
	}	// if (0 != _constraintActor)
}	// VTKConstrainedPointInteractor::setConstraint


void VTKRenderingManager::VTKConstrainedPointInteractor::vtkInteractorModified ( )
{
	notifyObserverForModifications ( );
}	// VTKConstrainedPointInteractor::vtkInteractorModified


// ===========================================================================
//              LA CLASSE VTKRenderingManager::VTKPlaneInteractor
// ===========================================================================


VTKRenderingManager::VTKPlaneInteractor::VTKPlaneInteractor (
	Math::Point point, Math::Vector normal,
	double xMin, double xMax, double yMin, double yMax,
	double zMin, double zMax,
	VTKRenderingManager& renderingManager,
	RenderingManager::InteractorObserver* observer)
	: RenderingManager::PlaneInteractor (observer),
	  _planeWidget (0), _plane (0), _callback (0)
{
	_plane			= vtkPlane::New ( );
	_planeWidget	= vtkImplicitPlaneWidget::New ( );
	_planeWidget->SetInteractor (renderingManager.getRenderWindow ( ).GetInteractor ( ));
	// On zoom le widget pour éviter sa juxtaposition avec la bounding box :
	_planeWidget->SetPlaceFactor (1.5);
	vtkFloatingPointType	box [6]	= { xMin, xMax, yMin, yMax, zMin, zMax };
	_planeWidget->PlaceWidget (box);
	_planeWidget->GetOutlineProperty ( )->SetColor (255, 127, 255);
	_planeWidget->SetOrigin (point.getX ( ), point.getY ( ), point.getZ ( ));
	_planeWidget->SetNormal (normal.getX ( ), normal.getY ( ), normal.getZ ( ));
	_planeWidget->DrawPlaneOn ( );
//	_planeWidget->SetEnabled (1);
	_planeWidget->On ( );
	_callback	= new VTKInteractorCallback<VTKPlaneInteractor> (this, _planeWidget);
}	// VTKPlaneInteractor::VTKPlaneInteractor


VTKRenderingManager::VTKPlaneInteractor::VTKPlaneInteractor (const VTKRenderingManager::VTKPlaneInteractor&)
	: RenderingManager::PlaneInteractor (0),
	  _planeWidget (0), _plane (0), _callback (0)
{
}	// VTKPlaneInteractor::VTKPlaneInteractor


VTKRenderingManager::VTKPlaneInteractor& VTKRenderingManager::VTKPlaneInteractor::operator = (const VTKRenderingManager::VTKPlaneInteractor&)
{
	return *this;
}	// VTKPlaneInteractor::operator =


VTKRenderingManager::VTKPlaneInteractor::~VTKPlaneInteractor ( )
{
	if (0 != _callback)
		_callback->Delete ( );
	_callback	= 0;

	if (0 != _planeWidget)
	{
		_planeWidget->Off ( );				// Mandatory (core dump ...)
		_planeWidget->SetInteractor (0);	// Mandatory (core dump ...)
		_planeWidget->Delete ( );
	}
	_planeWidget	= 0;
	if (0 != _plane)
		_plane->Delete ( );
	_plane	= 0;
}	// VTKPlaneInteractor::~VTKPlaneInteractor


Math::Point VTKRenderingManager::VTKPlaneInteractor::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_plane)
	CHECK_NULL_PTR_ERROR (_planeWidget)
	_planeWidget->GetPlane (_plane);
	double	origin [3]	= { 0., 0., 0. };
	_plane->GetOrigin (origin);

	return Math::Point (origin [0], origin [1], origin [2]);
}	// VTKPlaneInteractor::getPoint


Math::Vector VTKRenderingManager::VTKPlaneInteractor::getNormal ( ) const
{
	CHECK_NULL_PTR_ERROR (_plane)
	CHECK_NULL_PTR_ERROR (_planeWidget)
	_planeWidget->GetPlane (_plane);
	double	normal [3]	= { 0., 0., 0. };
	_plane->GetNormal (normal);

	return Math::Vector (normal [0], normal [1], normal [2]);
}	// VTKPlaneInteractor::getNormal


void VTKRenderingManager::VTKPlaneInteractor::setPlane (Math::Point point, Math::Vector normal)
{
	CHECK_NULL_PTR_ERROR (_planeWidget)
	_planeWidget->SetOrigin (point.getX ( ), point.getY ( ), point.getZ ( ));
	_planeWidget->SetNormal (normal.getX ( ), normal.getY ( ), normal.getZ ( ));
}	// VTKPlaneInteractor::setPlane


void VTKRenderingManager::VTKPlaneInteractor::vtkInteractorModified ( )
{
	notifyObserverForModifications ( );
}	// VTKPlaneInteractor::vtkInteractorModified


// ===========================================================================
//                        LA CLASSE VTKRenderingManager
// ===========================================================================

//VTKRenderingManager::VTKRenderingManager (QtVtkWidget* vtkWidget, bool offScreen)
VTKRenderingManager::VTKRenderingManager (QtVtkGraphicWidget* vtkWidget, bool offScreen)
	: QtComponents::RenderingManager ( ),
	  _vtkWidget (vtkWidget), _renderer (0), _renderWindow (0),
	  _layerNum (2), _theaterLayer (0), _trihedronLayer (1),
	  _camera (0), _lightKit (0),
	  _magix3dPicker (0), _pickerCommand (0), _pickerCommandTag (0),
	  _focalPointAxesActor (0), _focalPointAxesTag ((unsigned long)-1),
	  _trihedron (0), _trihedronCommandTag (0), _trihedronRenderer (0),
	  _axisActor (0), _axisProperties ( ),
	  _colorTables ( )
{
	// ==========================================================================================================
	// IMPORTANT : Si backend OpenGL2 pas de Render prématuré car plante dans la phase d'initialisation 
	// d'OpenGL du render window
	// ==========================================================================================================
	_renderWindow	= ((0 != vtkWidget) && (0 != vtkWidget->getRenderWindow ( ))) ?	// C/S mode
#if VTK_MAJOR_VERSION < 9
	vtkWidget->getRenderWindow ( ) : vtkRenderWindow::New ( );
#else	// VTK_MAJOR_VERSION < 9
	vtkWidget->getRenderWindow ( ) : vtkGenericOpenGLRenderWindow::New ( );
#endif	// #if VTK_MAJOR_VERSION < 9
cout << "RENDER WINDOW IS A " << _renderWindow->GetClassName ( ) << endl;
_renderWindow->PrintSelf (cout, *vtkIndent::New ( ));
	_axisProperties.axis1.color.setDRGB (vtkTrihedron::xColor [0], vtkTrihedron::xColor [1], vtkTrihedron::xColor [2]);
	_axisProperties.axis2.color.setDRGB (vtkTrihedron::yColor [0], vtkTrihedron::yColor [1], vtkTrihedron::yColor [2]);
	_axisProperties.axis3.color.setDRGB (vtkTrihedron::zColor [0], vtkTrihedron::zColor [1], vtkTrihedron::zColor [2]);
	_renderWindow->SetNumberOfLayers (_layerNum);
	_renderWindow->SetOffScreenRendering (true == offScreen ? 1 : 0);
#ifndef USE_OPENGL2_BACKEND
	_renderWindow->Render ( );	// Requis avant window->SetAlphaBitPlanes (1);
#endif	// USE_OPENGL2_BACKEND
	_renderWindow->SetAlphaBitPlanes (1);	// Bonne gestion de la transparence
	_renderWindow->SetMultiSamples (0);	// Bonne gestion de la transparence
	_renderer	= vtkRenderer::New ( );
	_renderer->SetUseDepthPeeling (true);	// Bonne gestion de la transparence
	_renderer->SetLayer (_theaterLayer);
	_renderer->SetBackground (Resources::instance ( )._background.getRed ( ), Resources::instance ( )._background.getGreen ( ), Resources::instance ( )._background.getBlue ( ));
#ifndef USE_OPENGL2_BACKEND						
	enableMultipleLighting (Resources::instance ( )._multipleLighting.getValue ( ));
#endif	// USE_OPENGL2_BACKEND
	_renderWindow->AddRenderer (_renderer);

	if (0 != _vtkWidget)
	{
		if (_vtkWidget->getRenderWindow ( ) != _renderWindow)
			_vtkWidget->setRenderWindow (_renderWindow);
		vtkRenderWindowInteractor*	interactor	= _vtkWidget->getInteractor ( );
/*		if (0 == interactor)
		{	// => VTK 9 ?
			interactor	= QVTKInteractor::New ( );
			_renderWindow->SetInteractor (interactor);
			interactor->SetRenderWindow (_renderWindow);
			interactor->Initialize ( );
			interactor->Delete ( );	// Still alive, Delete decreases reference count.
		}	// if (0 == interactor)*/
		CHECK_NULL_PTR_ERROR (interactor)
		vtkCustomizableInteractorStyleTrackball*	interactorStyle	= vtkCustomizableInteractorStyleTrackball::New ( );
		CHECK_NULL_PTR_ERROR (interactorStyle)
		interactorStyle->EnableActorHighlighting (false);
		interactor->SetInteractorStyle (interactorStyle);
		interactor->SetRenderWindow (_renderWindow);
		VTKRenderingManagerInteractionUpdater*	interactionUpdater	= new VTKRenderingManagerInteractionUpdater (*this);
		interactorStyle->AddObserver (vtkMgx3DInteractorStyle::ViewRedefinedEvent, interactionUpdater);

		// La sélection interactive :
		_magix3dPicker	= VTKMgx3DPicker::New ( );
		CHECK_NULL_PTR_ERROR (_magix3dPicker)
		_pickerCommand		= new VTKMgx3DPickerCommand (_renderer, interactor);
		CHECK_NULL_PTR_ERROR (_pickerCommand)
		_pickerCommandTag	= _magix3dPicker->AddObserver (vtkCommand::EndPickEvent, _pickerCommand);
		interactor->SetPicker (_magix3dPicker);
		interactorStyle->SetPickingTools (_magix3dPicker, _pickerCommand);
		interactorStyle->SetSelectionManager (getSelectionManager ( ));

		interactorStyle->Delete ( );	interactorStyle	= 0;
	}	// if (0 != _vtkWidget)

//	_renderWindow->LineSmoothingOn ( );
// PolygonSmoothingOn : ne pas mettre avec les QWidget, peut donner des rendus
// pas très beau (triangularisation visible des faces ("arêtes" blanches)).
//	_renderWindow->PolygonSmoothingOn ( );
#ifndef USE_OPENGL2_BACKEND
	_renderWindow->Render ( );
#endif	// USE_OPENGL2_BACKEND
#if VTK_MAJOR_VERSION < 9
	_renderWindow->SetMapped (true == offScreen ? 0 : 1);
#else 	// VTK_MAJOR_VERSION < 9
	_renderWindow->SetShowWindow (true == offScreen ? 0 : 1);
#endif	// VTK_MAJOR_VERSION < 9
	if (true == offScreen)
		restoreMapState (*_renderWindow);
	_camera	= _renderer->GetActiveCamera ( );
//cout << "======================================================" << endl;
//_renderer->PrintSelf (cout,*vtkIndent::New( ));
}	// VTKRenderingManager::VTKRenderingManager


VTKRenderingManager::VTKRenderingManager ( )
	: QtComponents::RenderingManager ( ),
	  _vtkWidget (0), _renderer (0), _renderWindow (0),
	  _layerNum (0), _theaterLayer (0), _trihedronLayer (0),
	  _camera (0), _lightKit (0),
	  _magix3dPicker (0), _pickerCommand (0), _pickerCommandTag (0),
	  _focalPointAxesActor (0), _focalPointAxesTag ((unsigned long)-1),
	  _trihedron (0), _trihedronCommandTag (0), _trihedronRenderer (0),
	  _axisActor (0), _axisProperties ( ),
	  _colorTables ( )
{
}	// VTKRenderingManager::VTKRenderingManager


VTKRenderingManager::VTKRenderingManager (const VTKRenderingManager&)
	: QtComponents::RenderingManager ( ),
	  _vtkWidget (0), _renderer (0), _renderWindow (0),
	  _layerNum (2), _theaterLayer (0), _trihedronLayer (1),
	  _camera (0), _lightKit (0),
	  _magix3dPicker (0), _pickerCommand (0), _pickerCommandTag (0),
	  _focalPointAxesActor (0), _focalPointAxesTag ((unsigned long)-1),
	  _trihedron (0), _trihedronCommandTag (0), _trihedronRenderer (0),
	  _axisActor (0), _axisProperties ( ),
	  _colorTables ( )
{
	MGX_FORBIDDEN ("VTKRenderingManager copy constructor is not allowed.");
}	// VTKRenderingManager::VTKRenderingManager (const VTKRenderingManager&)


VTKRenderingManager& VTKRenderingManager::operator = (const VTKRenderingManager&)
{
	MGX_FORBIDDEN ("VTKRenderingManager assignment operator is not allowed.");
	return *this;
}	// VTKRenderingManager::VTKRenderingManager (const VTKRenderingManager&)


VTKRenderingManager::~VTKRenderingManager ( )
{
	enableMultipleLighting (false);

	while (false == _colorTables.empty ( ))
		removeColorTable (_colorTables [0]);

	if (0 != _magix3dPicker)
	{
		if (0 != _pickerCommandTag)
			_magix3dPicker->RemoveObserver (_pickerCommandTag);
		_magix3dPicker->Delete ( );		_magix3dPicker	= 0;
	}	// if (0 != _magix3dPicker)
	if (0 != _pickerCommand)
	{
		_pickerCommand->Delete ( ); _pickerCommand		= 0;
		_pickerCommandTag	= 0;
	}	// if (0 != _pickerCommand)

	if (((unsigned long)-1 != _focalPointAxesTag) &&
	    (0 != getRenderWindow ( ).GetInteractor ( )))
		getRenderWindow ( ).GetInteractor ( )->RemoveObserver (_focalPointAxesTag);
	_focalPointAxesTag	= (unsigned long)-1;
	if (0 != _focalPointAxesActor)
	{
		getRenderer ( ).RemoveViewProp (_focalPointAxesActor);
		_focalPointAxesActor->Delete ( );
	}
	_focalPointAxesActor	= 0;

	setDisplayLandmark (false);

	if (0 != _trihedron)
	{
		if ((0 != _trihedronCommandTag) && (true == hasRenderWindow ( )))
		{
			vtkRenderWindowInteractor*	interactor		= getRenderWindow ( ).GetInteractor ( );
			vtkInteractorObserver*		interactorStyle	= 0 == interactor ? 0 : interactor->GetInteractorStyle ( );
			if (0 != interactorStyle)
				interactorStyle->RemoveObserver (_trihedronCommandTag);
			_trihedronCommandTag	= 0;
		}	// if (0 != _trihedronCommandTag)
		if (0 != _trihedronRenderer)
		{
			_trihedronRenderer->RemoveActor (_trihedron);
			_trihedronRenderer->Delete ( );			_trihedronRenderer	= 0;
		}	//  if (0 != _trihedronRenderer)
		_trihedron->Delete ( );			_trihedron	= 0;
	}	// if (0 != _trihedron)
}	// VTKRenderingManager::~VTKRenderingManager


//QtVtkWidget& VTKRenderingManager::getVTKWidget ( )
QtVtkGraphicWidget& VTKRenderingManager::getVTKWidget ( )
{
	CHECK_NULL_PTR_ERROR (_vtkWidget)
	return *_vtkWidget;
}	// VTKRenderingManager::getVTKWidget


bool VTKRenderingManager::hasDisplayedRepresentation ( ) const
{
	bool    has = false;

	if (true == hasRenderer ( ))
	{
		vtkPropCollection*	props	= getRenderer ( ).GetViewProps ( );
		CHECK_NULL_PTR_ERROR (props)
		has = 0 == props->GetNumberOfItems ( ) ? false : true;
	}	// if (true == hasRenderer ( ))

	return has;
}	// VTKRenderingManager::hasDisplayedRepresentation


vector<Entity*> VTKRenderingManager::getDisplayedEntities ( )
{
	if (false == hasRenderer ( ))
		throw Exception (UTF8String ("VTKRenderingManager::getDisplayedEntities : absence de gestionnaire de rendu.", Charset::UTF_8));
	vtkActorCollection*	actors	= getRenderer ( ).GetActors ( );
	CHECK_NULL_PTR_ERROR (actors)
	set<Entity*>	entities;
	vtkActor*		actor	= 0;
	for (actors->InitTraversal ( ); 0 != (actor = actors->GetNextItem ( )); )
	{
		VTKMgx3DActor*	mgxActor	= dynamic_cast<VTKMgx3DActor*>(actor);
		// On peut avoir des VTKMgx3DActor sans entité : cas par exemple des
		// points représentants des noeuds sur une arête
		// (VTKMgx3DEntityRepresentation::createPointsCloudRepresentation 
		// appelé par VTKRenderingManager::createCloudRepresentation avec
		// entity == 0 lors du paramétrage de la discrétisation des arêtes).
		if ((0 != mgxActor) && (0 != mgxActor->GetEntity ( )))
			entities.insert (mgxActor->GetEntity ( ));
	}	// for (actors->InitTraversal ( ); ...

	vector<Entity*>	uniqueEntities;
	for (set<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
		uniqueEntities.push_back (*it);

	return uniqueEntities;
}	// VTKRenderingManager::getDisplayedEntities


void VTKRenderingManager::getDisplayedValues (vector<RenderingManager::ColorTableDefinition>& definitions)
{
	if (false == hasRenderer ( ))
		throw Exception (UTF8String ("VTKRenderingManager::getDisplayedValues : absence de gestionnaire de rendu.", Charset::UTF_8));

	vtkActorCollection*	actors	= getRenderer ( ).GetActors ( );
	CHECK_NULL_PTR_ERROR (actors)
	vtkActor*		actor	= 0;
	for (actors->InitTraversal ( ); 0 != (actor = actors->GetNextItem ( )); )
	{
		vtkMapper*	mapper	= actor->GetMapper ( );
		if ((0 == mapper) || (0 == mapper->GetScalarVisibility ( )))
			continue;

		double	range [2]	= {
			NumericServices::doubleMachMax(), -NumericServices::doubleMachMax() };
		mapper->GetScalarRange (range);
		size_t	colorNum	= 0 == mapper->GetLookupTable ( ) ?
			0 : mapper->GetLookupTable ( )->GetNumberOfAvailableColors ( );
		string	name;
		vtkDataSet*		dataset		= mapper->GetInput ( );
		vtkPointData*	pointdata	= 0==dataset ? 0 : dataset->GetPointData( );
		vtkCellData*	celldata	= 0==dataset ? 0 : dataset->GetCellData( );
		switch (mapper->GetScalarMode ( ))
		{
			case VTK_SCALAR_MODE_USE_POINT_DATA	:
				if ((0 != pointdata) && (0 != pointdata->GetScalars ( )))
					name	= string (pointdata->GetScalars ( )->GetName ( ));
				break;
			case VTK_SCALAR_MODE_USE_CELL_DATA	:
				if ((0 != celldata) && (0 != celldata->GetScalars ( )))
					name	= string (celldata->GetScalars ( )->GetName ( ));
				break;
			case VTK_SCALAR_MODE_DEFAULT				:
				if ((0 != pointdata) && (0 != pointdata->GetScalars ( )))
					name	= string (pointdata->GetScalars ( )->GetName ( ));
				else if ((0 != celldata) && (0 != celldata->GetScalars ( )))
					name	= string (celldata->GetScalars ( )->GetName ( ));
				break;
			default							: continue;
		}	// switch (mapper->GetScalarMode ( ))
		RenderingManager::ColorTableDefinition	def (name, colorNum, range [0], range [1]);
		if (0 == definitions.size ( ))
			definitions.push_back (def);
		else
		{
			for (vector<RenderingManager::ColorTableDefinition>::iterator
					it = definitions.begin ( ); definitions.end ( ) != it; it++)
			{
				if (true == def.comparable (*it))
				{
					(*it).extent (def);
					continue;
				}	// if (true == def.comparable (*it))
			}	// if (true == def.comparable (*it))
			definitions.push_back (def);
		}	// else if (0 == definitions.size ( ))
	}	// for (actors->InitTraversal ( ); ...
}	// VTKRenderingManager::getDisplayedValues


void VTKRenderingManager::useGlobalDisplayProperties (bool global)
{
	if (global == RenderingManager::useGlobalDisplayProperties ( ))
		return;

	RenderingManager::useGlobalDisplayProperties (global);

	vector<Entity*>	entities	= getDisplayedEntities ( );
	for (vector<Entity*>::iterator it = entities.begin ( );
	     entities.end ( ) !=it; it++)
	{
		DisplayProperties::GraphicalRepresentation*	rep	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
		if (0 != rep)
		{
			unsigned long	mask	= rep->getRepresentationMask ( );
			rep->updateRepresentation (mask, true);
		}
	}	// for (vector<Entity*>::iterator it = entities.begin ( ); ...
}	// VTKRenderingManager::useGlobalDisplayProperties


void VTKRenderingManager::displayRepresentation (RepresentationID id, bool show)
{
	vtkActor*	actor	= (vtkActor*)id;
	if (0 != actor)
	{
		if (true == show)
			getRenderer().AddViewProp(actor);
		else
			getRenderer ( ).RemoveViewProp (actor);
	}	// if (0 != actor)
}	// VTKRenderingManager::displayRepresentation


void VTKRenderingManager::getOrthogonalVector (const double coords [3], double orth [3])
{
	orth [0]	= orth [1]	= orth [2]	= 0.;
	// On cherche un vecteur orthogonal à celui transmis en argument. Il y a une
	// infinité de possibilité, alors pourquoi pas celle retournée par cette
	// fonction ?
	const Math::Vector	i (1., 0., 0.), j (0., 1., 0.), k (0., 0., 1.);
	const Math::Vector	v (coords [0], coords [1], coords [2]);
	if (Math::Vector (v * i).abs2 ( ) < NumericServices::doubleMachEpsilon())
	{
		orth [1]	= orth [2]	= 1.;
		return;
	}
	if (Math::Vector (v * j).abs2 ( ) < NumericServices::doubleMachEpsilon())
	{
		orth [0]	= orth [2]	= 1.;
		return;
	}
	if (Math::Vector (v * k).abs2 ( ) < NumericServices::doubleMachEpsilon())
	{
		orth [0]	= orth [1]	= 1.;
		return;
	}

	const Math::Vector	nulv (0., 0., 0.);

	if (fabs (coords [0]) > NumericServices::doubleMachEpsilon())
	{
		if (fabs (coords [1]) > NumericServices::doubleMachEpsilon())
		{	// On prend dz = 0
			orth [0]	= 1.;
			orth [1]	= - coords [0] / coords [1];
		}
		else if (fabs (coords [2]) > NumericServices::doubleMachEpsilon())
		{	// On prend dx = 0
			orth [1]	= 1.;
			orth [2]	= - coords [1] / coords [2];
		}
		else
		{
			orth [2]	= 1.;
		}
	} 	// if (fabs (coords [0]) > NumericServices.doubleMachEpsilon())
	else if (fabs (coords [1]) > NumericServices::doubleMachEpsilon())
	{	// dx est nul
		orth [0]	= 1.;
//		orth [2]	= - coords [2] / coords [1];
	}
	else 
	{	// dx et dy sont nul
//		orth [0]	= 1.;
		orth [1]	= 1.;
	}
}	// VTKRenderingManager::getOrthogonalVector


RenderingManager::RepresentationID VTKRenderingManager::createSegment (
		const Math::Point& p1, const Math::Point& p2,
		const DisplayProperties& properties, bool display)
{
	if (p1 == p2)
		return 0;

	vtkLineSource*		line	= vtkLineSource::New ( );
	vtkPolyDataMapper*	mapper	= vtkPolyDataMapper::New ( );
	vtkActor*			actor	= vtkActor::New ( );

	line->SetPoint1 (p1.getX ( ), p1.getY ( ), p1.getZ ( ));
	line->SetPoint2 (p2.getX ( ), p2.getY ( ), p2.getZ ( ));

	const Color&	color	= properties.getWireColor ( );
	actor->GetProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
	actor->GetProperty ( )->SetLineWidth (properties.getLineWidth ( ));
#ifndef VTK_5
	mapper->SetInputConnection (line->GetOutputPort ( ));
#else	// VTK_5
	mapper->SetInput (line->GetOutput ( ));
#endif	// VTK_5
	mapper->ScalarVisibilityOff ( );
	actor->SetMapper (mapper);
	actor->PickableOff ( );
	actor->DragableOff ( );

	if (true == display)
		getRenderer ( ).AddViewProp (actor);

	line->Delete ( );		line	= 0;
	mapper->Delete ( );		mapper	= 0;

	return actor;
}	// VTKRenderingManager::createSegment


RenderingManager::RepresentationID VTKRenderingManager::createVector (
						const Math::Point& p1, const Math::Point& p2,
						const DisplayProperties& properties, bool display)
{
	if (p1 == p2)
		return 0;

	vtkArrowSource*		arrow	= vtkArrowSource::New ( );
	vtkPolyDataMapper*	mapper	= vtkPolyDataMapper::New ( );
	vtkActor*			actor	= vtkActor::New ( );

	// Pour VTK, on s'assure que les deux points de la flèche sont bien ordonnés
	Math::Point p11 (p1), p22 (p2);
	Vector	uu (1., 0., 0.), vv (p11, p22);
	bool	inverted	= false;
	if (scaMul (uu, vv) < 0)
	{
		p11	= p2;
		p22	= p1;
		inverted	= true;
	}

	// Soit les vecteurs u, flèche VTK initiale, et v, flèche demandée.
	// Soit w un vecteur ortogonal à u et v, autour duquel est effectuée la
	// rotation (=> rotation autour d'un angle quelconque).
	// Aspects redimensionnement : On passe d'un vecteur de norme 1 sur X à
	// un vecteur de norme v.abs ( ) => multiplication de mat (0, 0) par
	// v.abs ( ).
	const Vector	u (1., 0., 0.), v (p11, p22);
	const Vector	w   = u * v;	// produit vectoriel
	// Rem : u et v sont non nuls, u car connu, v car testé ci-dessus.
	const double	cosinus	= scaMul (u, v) / (u.abs ( ) * v.abs ( ));
	const double	sinus	= std::sqrt (1 - cosinus * cosinus);
	// On laisse à VTK de calculer la matrice de transformation :
	vtkTransform*	transform	= vtkTransform::New ( ); 	// Identité
	transform->Scale (v.abs ( ), v.abs ( ), v.abs ( ));
	const double	angle	= acos (cosinus) * 180. / M_PI;
	transform->RotateWXYZ (angle, w.getX ( ), w.getY ( ), w.getZ ( ));
	transform->PostMultiply ( );
	transform->Translate (p11.getX ( ), p11.getY ( ), p11.getZ ( ));
	actor->SetUserMatrix (transform->GetMatrix ( ));
	// L'aspect de la flèche :
	// Tip		: tête de la flèche (partie conique)
	// Shaft	: base de la flèche
	// La flèche va être redimensionnée (cf. transformation ci-dessus).
	// Sa longueur avant transformation est 1 ((0, 0, 0) -> (1, 0, 0)).
	// => à prendre en compte dans les longueurs des rayons.
	arrow->SetShaftResolution (24);
	arrow->SetTipResolution (24);
	arrow->SetTipRadius (0.10 / 2.);		// Longueur du rayon du cône
	arrow->SetShaftRadius (0.03 / 2.);		// Longueur du rayon de la base
	if (true == inverted)
		arrow->InvertOn ( );
	const Color&	color	= properties.getWireColor ( );
	actor->GetProperty ( )->SetColor (
						color.getRed ( ) / 255., color.getGreen ( ) / 255.,
						color.getBlue ( ) / 255.);
#ifndef VTK_5
	mapper->SetInputConnection (arrow->GetOutputPort ( ));
#else	// VTK_5
	mapper->SetInput (arrow->GetOutput ( ));
#endif	// VTK_5
	mapper->ScalarVisibilityOff ( );
	actor->SetMapper (mapper);
	actor->PickableOff ( );
	actor->DragableOff ( );

	if (true == display)
		getRenderer ( ).AddViewProp (actor);

	transform->Delete ( );	transform	= 0;
	arrow->Delete ( );		arrow	= 0;
	mapper->Delete ( );		mapper	= 0;

	return actor;
}	// VTKRenderingManager::createVector


VTKRenderingManager::RepresentationID VTKRenderingManager::createRotationAxe (
						const Math::Point& p1, const Math::Point& p2,
						const DisplayProperties& properties, bool display)
{
	if (p1 == p2)
		return 0;

	const double	x1	= p1.getX ( ), y1	= p1.getY ( ), z1	= p1.getZ ( );
	const double	x2	= p2.getX ( ), y2	= p2.getY ( ), z2	= p2.getZ ( );
	const double	dx	= x2 - x1, dy	= y2 - y1, dz	= z2 - z1;
	double	coords [3]	= { dx, dy, dz };
	vtkVector<double, 3>	v (coords);
	vtkVector<double, 3>	vn	= v.Normalized ( );
	// On cherche un premier vecteur (u, v, w) orthogonal à P1P2 :
	// u*dx + v*dy + w*dz = 0
	double	coordsV1 [3]	= { 0., 0., 0. };
	getOrthogonalVector (coords, coordsV1);
	vtkVector<double, 3>	v1 (coordsV1);
	vtkVector<double, 3>	v1n	= v1.Normalized ( );
	// On cherche un second vecteur orthogonal au 2 premiers :
	double	coordsV2 [3]	= {
		coords [1] * coordsV1 [2] - coordsV1 [1] * coords [2],
		coordsV1 [0] * coords [2] - coords [0] * coordsV1 [2],
		coords [0] * coordsV1 [1] - coordsV1 [0] * coords [1]
		};
	vtkVector<double, 3>	v2 (coordsV2);
	vtkVector<double, 3>	v2n	= v2.Normalized ( );

	// => P1, v1n et v2n forment une base du plan perpendiculaire à P1P2.
	// On va faire un arc de M1 à M2, deux points de ce plan, distants de
	// d à P1 :
	double			d	= v.Norm ( ) / 10.;
	double	coordsV1Sized [3]	= {v1n[0] * d, v1n[1] * d, v1n[2] * d};
	double	coordsV2Sized [3]	= {v2n[0] * d, v2n[1] * d, v2n[2] * d};
	vtkVector<double, 3>	v1s (coordsV1Sized);
	vtkVector<double, 3>	v2s (coordsV2Sized);
	Math::Point		m1 (
		p1.getX ( ) + v1s [0], p1.getY ( ) + v1s [1], p1.getZ ( ) + v1s [2]);
	Math::Point		m2 (
		p1.getX ( ) + v2s [0], p1.getY ( ) + v2s [1], p1.getZ ( ) + v2s [2]);

	vtkAssembly*	assembly	= vtkAssembly::New ( );
	vtkArcSource*	arcSource	= 	vtkArcSource::New ( );
	vtkPolyDataMapper*	mapper	= vtkPolyDataMapper::New ( );
	vtkActor*			actor	= vtkActor::New ( );
	arcSource->SetCenter (p1.getX ( ), p1.getY ( ), p1.getZ ( ));
	arcSource->SetPoint1 (m1.getX ( ), m1.getY ( ), m1.getZ ( ));
	arcSource->SetPoint2 (m2.getX ( ), m2.getY ( ), m2.getZ ( ));
	arcSource->NegativeOn ( );	// => On fait les 270 degrés.
	arcSource->SetResolution (270);
	const Color&	color	= properties.getWireColor ( );
	actor->GetProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
	actor->GetProperty ( )->SetLineWidth (properties.getLineWidth ( ));
#ifndef VTK_5
	mapper->SetInputConnection (arcSource->GetOutputPort ( ));
#else	// VTK_5
	mapper->SetInput (arcSource->GetOutput ( ));
#endif	// VTK_5
	mapper->ScalarVisibilityOff ( );
	actor->SetMapper (mapper);
	actor->PickableOff ( );
	actor->DragableOff ( );
	assembly->AddPart (actor);
	actor->Delete ( );		actor	= 0;
	arcSource->Delete ( );	arcSource	= 0;
	mapper->Delete ( );		mapper	= 0;

	// Une flèche au bout de l'arc :
	vtkArrowSource*		arrow	= vtkArrowSource::New ( );
	mapper	= vtkPolyDataMapper::New ( );
	actor	= vtkActor::New ( );
	const double			d2	= d / 4.;	// Taille pointe flèche
	double	coordsV3Sized [3]	= {v1n[0] * d2, v1n[1] * d2, v1n[2] * d2};
	vtkVector<double, 3>	v3s (coordsV3Sized);
	Math::Point		m3 (
		m2.getX ( ) + v3s [0], m2.getY ( ) + v3s [1], m2.getZ ( ) + v3s [2]);
	// Soit les vecteurs ua, flèche VTK initiale, et va, flèche demandée.
	// Soit wa un vecteur ortogonal à ua et va, autour duquel est effectuée la
	// rotation (=> rotation autour d'un angle quelconque).
	// Aspects redimensionnement : On passe d'un vecteur de norme 1 sur X à
	// un vecteur de norme va.abs ( ) => multiplication de mat (0, 0) par
	// va.abs ( ).
	const Vector	ua (1., 0., 0.), va (m2, m3);
	const Vector	wa   = ua * va;	// produit vectoriel
	// Rem : u et v sont non nuls, u car connu, v car testé ci-dessus.
	const double	cosinus	= scaMul (ua, va) / (ua.abs ( ) * va.abs ( ));
	const double	sinus	= std::sqrt (1 - cosinus * cosinus);
	// On laisse à VTK de calculer la matrice de transformation :
	vtkTransform*	transform	= vtkTransform::New ( ); 	// Identité
	transform->Scale (va.abs ( ), va.abs ( ), va.abs ( ));
	const double	angle	= acos (cosinus) * 180. / M_PI;
	transform->RotateWXYZ (angle, wa.getX ( ), wa.getY ( ), wa.getZ ( ));
	transform->PostMultiply ( );
	transform->Translate (m2.getX ( ), m2.getY ( ), m2.getZ ( ));
	actor->SetUserMatrix (transform->GetMatrix ( ));
	// L'aspect de la flèche :
	// Tip		: tête de la flèche (partie conique)
	// La flèche va être redimensionnée (cf. transformation ci-dessus).
	// Sa longueur avant transformation est 1 ((0, 0, 0) -> (1, 0, 0)).
	// => à prendre en compte dans les longueurs des rayons.
	arrow->SetTipResolution (24);
	arrow->SetTipRadius (0.5);			// Longueur du rayon du cône
	arrow->SetTipLength (1.);
	actor->GetProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
#ifndef VTK_5
	mapper->SetInputConnection (arrow->GetOutputPort ( ));
#else	// VTK_5
	mapper->SetInput (arrow->GetOutput ( ));
#endif	// VTK_5
	mapper->ScalarVisibilityOff ( );
	actor->SetMapper (mapper);
	actor->PickableOff ( );
	actor->DragableOff ( );
	assembly->AddPart (actor);
	actor->Delete ( );		actor	= 0;
	arrow->Delete ( );		arrow	= 0;
	mapper->Delete ( );		mapper	= 0;

	assembly->PickableOff ( );
	assembly->DragableOff ( );
	if (true == display)
		getRenderer ( ).AddViewProp (assembly);

	// On trace l'axe :
	actor	= (vtkActor*)createSegment (p1, p2, properties, false);
	assembly->AddPart (actor);
	actor->Delete ( );	actor	= 0;

// JUSTE POUR LA MISE AU POINT, ON TRACE M1-M2 :
/*	DisplayProperties	pr (properties);
	pr.setWireColor (Color (255, 0, 0));
	actor	= (vtkActor*)createSegment (m1, m2, pr, false);
	assembly->AddPart (actor);
	actor->Delete ( );	actor	= 0; */
// FIN MISE AU POINT

	return assembly;
}	// VTKRenderingManager::createRotationAxe


VTKRenderingManager::RepresentationID VTKRenderingManager::createOutlineBox (
							const Math::Point& p1, const Math::Point& p2,
							const DisplayProperties& properties, bool display)
{
cout << endl << endl << endl << endl << endl
     << "VTKRenderingManager::createOutlineBox : FONCTION NON TESTEE." << endl
     << "Merci d'enlever ces lignes si tout se passe comme prévu ou de "
     << "prévenir Charles en cas de bogue."
     << endl << endl << endl << endl << endl;
	if (p1 == p2)
		return 0;

	vtkOutlineSource*	outline	= vtkOutlineSource::New ( );
	vtkPolyDataMapper*	mapper	= vtkPolyDataMapper::New ( );
	vtkActor*			actor	= vtkActor::New ( );

	double	bounds [6]	= 
	{ p1.getX ( ) < p2.getX ( ) ? p1.getX ( ) : p2.getX ( ),
	  p1.getX ( ) > p2.getX ( ) ? p1.getX ( ) : p2.getX ( ),
	  p1.getY ( ) < p2.getY ( ) ? p1.getY ( ) : p2.getY ( ),
	  p1.getY ( ) > p2.getY ( ) ? p1.getY ( ) : p2.getY ( ),
	  p1.getZ ( ) < p2.getZ ( ) ? p1.getZ ( ) : p2.getZ ( ),
	  p1.getZ ( ) > p2.getZ ( ) ? p1.getZ ( ) : p2.getZ ( )
	};

	const Color&	color	= properties.getWireColor ( );
	actor->GetProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
	actor->GetProperty ( )->SetLineWidth (properties.getLineWidth ( ));
#ifndef VTK_5
	mapper->SetInputConnection (outline->GetOutputPort ( ));
#else	// VTK_5
	mapper->SetInput (outline->GetOutput ( ));
#endif	// VTK_5
	actor->SetMapper (mapper);
	actor->PickableOff ( );
	actor->DragableOff ( );

	if (true == display)
		getRenderer ( ).AddViewProp (actor);

	outline->Delete ( );	outline	= 0;
	mapper->Delete ( );		mapper	= 0;

	return actor;
}	// VTKRenderingManager::createOutlineBox


VTKRenderingManager::RepresentationID VTKRenderingManager::createCloudRepresentation (
		const vector<Math::Point>& points, const DisplayProperties& properties,
		bool display)
{
	VTKMgx3DActor*			actor	= 0;
	vtkDataSetMapper*		mapper	= 0;
	vtkUnstructuredGrid*	grid	= 0;
	VTKMgx3DEntityRepresentation::createPointsCloudRepresentation (0, actor, mapper, grid, points);
	CHECK_NULL_PTR_ERROR (actor)
	CHECK_NULL_PTR_ERROR (mapper)
	CHECK_NULL_PTR_ERROR (grid)
	vtkProperty*	property	= actor->GetProperty ( );
	CHECK_NULL_PTR_ERROR (property)
	property->SetPointSize (properties.getPointSize ( ));
	const Color&	color	= properties.getCloudColor ( );
	property->SetColor (color.getRed( ) / 255., color.getGreen( ) / 255.,
	                    color.getBlue( ) / 255.);
	property->SetRepresentationToPoints ( );
	actor->PickableOff ( );
	actor->DragableOff ( );

	if (true == display)
		getRenderer ( ).AddViewProp (actor);

	grid->Delete ( );		grid	= 0;
	mapper->Delete ( );		mapper	= 0;

	return actor;
}	// VTKRenderingManager::createCloudRepresentation


VTKRenderingManager::RepresentationID
	VTKRenderingManager::createSegmentsWireRepresentation (
			const vector<Math::Point>& points, const vector<size_t>& segments,
			const DisplayProperties& properties, bool display)
{
	VTKMgx3DActor*			actor	= 0;
	vtkDataSetMapper*		mapper	= 0;
	vtkUnstructuredGrid*	grid	= 0;
	VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (0, actor, mapper, grid, points, segments);
	CHECK_NULL_PTR_ERROR (actor)
	CHECK_NULL_PTR_ERROR (mapper)
	CHECK_NULL_PTR_ERROR (grid)
	vtkProperty*	property	= actor->GetProperty ( );
	CHECK_NULL_PTR_ERROR (property)
	property->SetPointSize (properties.getPointSize ( ));
	const Color&	color	= properties.getWireColor ( );
	property->SetColor (color.getRed( ) / 255., color.getGreen( ) / 255.,
	                    color.getBlue( ) / 255.);
	property->SetRepresentationToSurface ( );	// Pour le filaire
	property->SetInterpolationToFlat ( );
	property->SetLineWidth (properties.getLineWidth ( ));
	actor->PickableOff ( );
	actor->DragableOff ( );

	if (true == display)
		getRenderer ( ).AddViewProp (actor);

	grid->Delete ( );		grid	= 0;
	mapper->Delete ( );		mapper	= 0;

	return actor;
}	// VTKRenderingManager::createSegmentsWireRepresentation


RenderingManager::RepresentationID VTKRenderingManager::createTextRepresentation (
                const Math::Point& pos, size_t number,
                const DisplayProperties& properties, bool display)
{
	vtkActor2D*		actor	= vtkActor2D::New ( );
	vtkTextMapper*	mapper	= vtkTextMapper::New ( );
	const string	text	= string (" ") + NumericConversions::toStr (number);
	mapper->SetInput (text.c_str ( ));
	actor->SetMapper (mapper);
	actor->GetPositionCoordinate( )->SetCoordinateSystemToWorld( );
	actor->GetPositionCoordinate ( )->SetValue (
								pos.getX ( ), pos.getY ( ), pos.getZ ( ));
	Color					color		= properties.getWireColor ( );
	int						fontFamily	= 0, fontSize	= 12;
	bool					bold	= false,	italic	= false;
	// Rem CP : on ne fait pas getFontProperties (..., color) car les
	// arêtes topo ont un code couleur pour la discrétisation.
	// => on utilise une instance de Color qui ne sera pas utilisé.
	Color	c (0, 0, 0);
	properties.getFontProperties (fontFamily, fontSize, bold, italic, c);
	mapper->GetTextProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
	mapper->GetTextProperty ( )->SetFontFamily (fontFamily);
	mapper->GetTextProperty ( )->SetFontSize (fontSize);
	mapper->GetTextProperty ( )->SetBold (bold);
	mapper->GetTextProperty ( )->SetItalic (italic);
	mapper->GetTextProperty ( )->ShadowOff ( );
	mapper->Delete ( );		mapper	= 0;
	actor->PickableOff ( );

	if (true == display)
		getRenderer ( ).AddViewProp (actor);

	return actor;
}	// VTKRenderingManager::createTextRepresentation


void VTKRenderingManager::destroyRepresentation (RenderingManager::RepresentationID id, bool hide)
{
	vtkActor*	actor	= (vtkActor*)id;
	if (0 != actor)
	{
		if (true == hide)
			getRenderer ( ).RemoveViewProp (actor);
		actor->Delete ( );			actor	= 0;
	}	// if (0 != actor)
}	// VTKRenderingManager::destroyRepresentation


RenderingManager::PointInteractor* VTKRenderingManager::createPointInteractor (
	Math::Point point, double dx, double dy, double dz, RenderingManager::InteractorObserver* observer)
{
	VTKPointInteractor*	interactor	=
				new VTKPointInteractor (point, dx, dy, dz, *this, observer);

	return interactor;
}	// VTKRenderingManager::createPointInteractor


RenderingManager::ConstrainedPointInteractor*
	VTKRenderingManager::createConstrainedPointInteractor (
		Math::Point point, Entity* constraint, size_t factor, RenderingManager::InteractorObserver* observer)
{
	VTKConstrainedPointInteractor*	interactor	=
			new VTKConstrainedPointInteractor (point, constraint, factor, *this, observer);

	return interactor;
}	// VTKRenderingManager::createConstrainedPointInteractor


RenderingManager::PlaneInteractor* VTKRenderingManager::createPlaneInteractor (
							Math::Point point, Math::Vector normal,
							double xMin, double xMax, double yMin, double yMax,
							double zMin, double zMax,
							RenderingManager::InteractorObserver* observer)
{
	VTKPlaneInteractor*	interactor	=
		new VTKPlaneInteractor (
			point, normal, xMin, xMax, yMin, yMax, zMin, zMax, *this, observer);

	return interactor;
}	// VTKRenderingManager::createPlaneInteractor


void VTKRenderingManager::destroyInteractor (RenderingManager::Interactor* interactor)
{
	delete interactor;
}	// VTKRenderingManager::destroyInteractor


RenderingManager::ColorTable* VTKRenderingManager::getColorTable (
					const RenderingManager::ColorTableDefinition& definition)
{
	for (vector<VTKRenderingManager::VTKColorTable*>::iterator
			it = _colorTables.begin ( ); _colorTables.end ( ) != it; it++)
		if (true == (*it)->getDefinition ( ).comparable (definition))
			return *it;

	VTKRenderingManager::VTKColorTable*	table	=
					new VTKRenderingManager::VTKColorTable (*this, definition);

	table->getScalarBarWidget ( ).SetInteractor (getRenderWindow ( ).GetInteractor ( ));
	_colorTables.push_back (table);
//	table->getScalarBarWidget ( ).SetEnabled (true);
//	table->getScalarBarWidget ( ).Render ( );

	return table;
}	// VTKRenderingManager::getColorTable


vector<RenderingManager::ColorTable*> VTKRenderingManager::getColorTables ( ) const
{
	vector<RenderingManager::ColorTable*>	tables;
	for (vector<VTKRenderingManager::VTKColorTable*>::const_iterator
			it = _colorTables.begin ( ); _colorTables.end ( ) != it; it++)
		tables.push_back (*it);

	return tables;
}	// VTKRenderingManager::getColorTables


void VTKRenderingManager::removeColorTable (RenderingManager::ColorTable* table)
{
	VTKRenderingManager::VTKColorTable*	vtable	=
					dynamic_cast<VTKRenderingManager::VTKColorTable*>(table);
	if (0 == vtable)
	{
		INTERNAL_ERROR (exc, "Table de couleur de type invalide.", "VTKRenderingManager::removeColorTable")
		throw exc;
	}	// if (0 == vtable)

	vector<VTKRenderingManager::VTKColorTable*>::iterator it	=
			std::find (_colorTables.begin ( ), _colorTables.end ( ), vtable);
	if (_colorTables.end ( ) != it)
		_colorTables.erase (it);
	delete vtable;
}	// VTKRenderingManager::removeColorTable


void VTKRenderingManager::setTableDefinition (const RenderingManager::ColorTableDefinition& definition)
{
	RenderingManager::ColorTable*   table  = getColorTable (definition);
	if (0 == table)
		throw Exception (UTF8String ("VTKRenderingManager::setTableDefinition : absence de table de couleurs correspondante.", Charset::UTF_8));

	table->setDefinition (definition);
}   // VTKRenderingManager::setTableDefinition


void VTKRenderingManager::setSelectionManager (VTKMgx3DSelectionManager* manager)
{
	RenderingManager::setSelectionManager (manager);
	if (0 != _pickerCommand)
	{
		_pickerCommand->SetSelectionManager (manager);
//		_pickerCommand->SetLogDispatcher (&getContext ( ).getLogDispatcher ( ));
	}
	VTKSelectionManagerDimFilter*	dimFilter	=
					dynamic_cast<VTKSelectionManagerDimFilter*>(manager);
	if (0 != dimFilter)
		dimFilter->setRenderingManager (this);

	vtkRenderWindowInteractor*	interactor		= true == hasRenderWindow( ) ?  getRenderWindow( ).GetInteractor( ) : 0;
	vtkInteractorObserver*		interactorStyle	= 0 == interactor ? 0 : interactor->GetInteractorStyle ( );
	vtkMgx3DInteractorStyle*	mgxInteractorStyle	= dynamic_cast<vtkMgx3DInteractorStyle*>(interactorStyle);
	if (0 != mgxInteractorStyle)
		mgxInteractorStyle->SetSelectionManager (manager);
}	// VTKRenderingManager::setSelectionManager


VTKMgx3DPicker& VTKRenderingManager::getPicker ( )
{
	if (0 == _magix3dPicker)
		throw Exception (UTF8String ("VTKRenderingManager::getPicker : absence de picker.", Charset::UTF_8));

	return *_magix3dPicker;
}	// VTKRenderingManager::getPicker


VTKMgx3DPickerCommand& VTKRenderingManager::getPickerCommand ( )
{
	if (0 == _pickerCommand)
		throw Exception (UTF8String ("VTKRenderingManager::getPickerCommand : absence de picker.", Charset::UTF_8));

	return *_pickerCommand;
}	// VTKRenderingManager::getPickerCommand


void VTKRenderingManager::setPickingTools (VTKMgx3DPicker* picker, VTKMgx3DPickerCommand* pickerCommand)
{
	if (picker != _magix3dPicker)
	{
		if (0 != _pickerCommandTag)
		{
			_magix3dPicker->RemoveObserver (_pickerCommandTag);
			_magix3dPicker->Delete ( );		_magix3dPicker	= 0;
			_pickerCommandTag	= 0;
		}	// if (0 != _magix3dPicker)
	}
	if ((pickerCommand != _pickerCommand) &&  (0 != _pickerCommand))
	{
		_pickerCommand->Delete ( ); _pickerCommand		= 0;
	}	// if ((pickerCommand != _pickerCommand) &&  (0 != _pickerCommand))	

	if ((0 != picker) && (0 != pickerCommand))
	{
		_magix3dPicker	= picker;
		_pickerCommand	= pickerCommand;
		_magix3dPicker->Register (0);
		_pickerCommand->Register (0);
		_pickerCommandTag	= _magix3dPicker->AddObserver (vtkCommand::EndPickEvent, _pickerCommand);
		if ((true == hasRenderWindow ( )) && (0 != getRenderWindow ( ).GetInteractor ( )))
		{
			getRenderWindow ( ).GetInteractor ( )->SetPicker (_magix3dPicker);
			getMgx3DInteractorStyle ( ).SetPickingTools (_magix3dPicker, _pickerCommand);
		}	// if ((true == hasRenderWindow ( )) && (0 != getRenderWindow ( ).GetInteractor ( )))
	}	// if ((0 != picker) && (0 != pickerCommand))
}	// VTKRenderingManager::setPickingTools


void VTKRenderingManager::getBoundingBox (
				double& xmin, double& xmax, double& ymin, double& ymax,
				double& zmin, double& zmax)
{
	double	bounds [6] = {0., 0., 0., 0., 0., 0.};
	getRenderer ( ).ComputeVisiblePropBounds (bounds);
	xmin	= bounds [0];		 xmax	= bounds [1];
	ymin	= bounds [2];		 ymax	= bounds [3];
	zmin	= bounds [4];		 zmax	= bounds [5];
}	// VTKRenderingManager::getBoundingBox


void VTKRenderingManager::getBackground (double& red, double& green, double& blue) const
{
	getRenderer ( ).GetBackground (red, green, blue);
}	// VTKRenderingManager::getBackground


Color VTKRenderingManager::getBackground ( ) const
{
	double	red	= 0.,	green	= 0.,	blue	= 0.;
	getBackground (red, green, blue);
	return Color ((unsigned char)(255 * red), (unsigned char)(255 * green),
	              (unsigned char)(255 * blue));
}	// VTKRenderingManager::getBackground


void VTKRenderingManager::setBackground (double red, double green, double blue)
{
	RenderingManager::setBackground (red, green, blue);

	getRenderer ( ).SetBackground (red, green, blue);
}	// VTKRenderingManager::setBackground


void VTKRenderingManager::setBackground (const Color& bg)
{
	RenderingManager::setBackground (bg);

	const double	red		= (double)(bg.getRed ( ) / 255.);
	const double	green	= (double)(bg.getGreen ( ) / 255.);
	const double	blue	= (double)(bg.getBlue ( ) / 255.);

	getRenderer ( ).SetBackground (red, green, blue);
}	// VTKRenderingManager::setBackground


void VTKRenderingManager::setDisplayTrihedron (bool display)
{
	BEGIN_TRY_CATCH_BLOCK

	if ((true == display) && (0 == _trihedron))
	{
		_trihedron	= vtkTrihedron::New ( );
		_trihedron->Label2DOn ( );
		_trihedron->PickableOff ( );
		_trihedronRenderer  = vtkRenderer::New ( );
		_trihedronRenderer->SetUseDepthPeeling (true);
		_trihedronRenderer->SetInteractive (0);
		_trihedronRenderer->SetViewport (0, 0, 0.2, 0.2);
		_trihedronRenderer->SetLayer (_trihedronLayer);
		_trihedronRenderer->AddActor (_trihedron);
		vtkCamera*  trihedronCamera = _trihedronRenderer->GetActiveCamera ( );
		trihedronCamera->SetParallelProjection (1);
	}	// if ((true == display) && (0 == _trihedron))

	if (true == display)
	{
		vtkTrihedronCommand*	trihedronCommand	= vtkTrihedronCommand::New ( );
		trihedronCommand->SetTrihedron (_trihedron);
		trihedronCommand->SetRenderer (_trihedronRenderer);
		getRenderWindow ( ).AddRenderer (_trihedronRenderer);
		_trihedronCommandTag	= getRenderer ( ).AddObserver (vtkCommand::StartEvent, trihedronCommand);
		vtkCamera*	camera	= getRenderer ( ).GetActiveCamera ( );
		if (0 != camera)
			trihedronCommand->SynchronizeViews (camera);
		trihedronCommand->Delete ( );
		_trihedronRenderer->ResetCamera ( );
	}	// if (true == display)
	else
	{
		if (0 != _trihedronCommandTag)
			getRenderer ( ).RemoveObserver (_trihedronCommandTag);
		_trihedronCommandTag	= 0;
		if (0 != _trihedronRenderer)
			getRenderWindow ( ).RemoveRenderer (_trihedronRenderer);
	}	// else if (true == display)

// getRenderWindow ( ).Render ( ) : fout la pagaille en mode C/S avec rendu
// côté serveur car pas de verrou MPI.	=> forceRender ( )
//	getRenderWindow ( ).Render ( );
	forceRender ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// VTKRenderingManager::setDisplayTrihedron


void VTKRenderingManager::setDisplayLandmark (bool display)
{
	BEGIN_TRY_CATCH_BLOCK

	if ((true == display) && (0 == _axisActor))
	{
		_axisActor	= vtkLandmarkActor::New ( );
		_axisActor->GetProperty ( )->SetColor (0., 0., 0.);
		setAxisProperties (getAxisProperties ( ));
		_axisActor->SetCornerOffset (0.);
		_axisActor->PickableOff ( );
		_axisActor->SetCamera (getRenderer ( ).GetActiveCamera ( ));
		getRenderer ( ).AddActor (_axisActor);
	}	// if ((true == display) && (0 == _axisActor))

	if ((false == display) && (0 != _axisActor))
	{
		getRenderer ( ).RemoveActor (_axisActor);
		_axisActor->Delete ( );
		_axisActor	= 0;
	}	// if ((false == display) && (0 != _axisActor))

// getRenderWindow ( ).Render ( ) : fout la pagaille en mode C/S avec rendu
// côté serveur car pas de verrou MPI.	=> forceRender ( )
//	getRenderWindow ( ).Render ( );
	forceRender ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// VTKRenderingManager::setDisplayLandmark


LandmarkAttributes VTKRenderingManager::getAxisProperties ( ) 
{	
	if (true == _axisProperties.autoAdjust)
	{
		vtkFloatingPointType    bounds [6]	=
				{ NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ) };
		getBoundingBox (
			bounds [0],bounds [1],bounds [2],bounds [3],bounds [4],bounds [5]);
		_axisProperties.axis1.min	= bounds [0];
		_axisProperties.axis1.max	= bounds [1];
		_axisProperties.axis2.min	= bounds [2];
		_axisProperties.axis2.max	= bounds [3];
		_axisProperties.axis3.min	= bounds [4];
		_axisProperties.axis3.max	= bounds [5];
	}	// if (true == _axisProperties.autoAdjust)

	return _axisProperties;
}	// VTKRenderingManager::getAxisProperties


void VTKRenderingManager::setAxisProperties (const LandmarkAttributes& attr)
{
	if (&_axisProperties != &attr)
		_axisProperties	= attr;

	if (0 != _axisActor)
	{
		if (false == _axisProperties.autoAdjust)
		{
			vtkFloatingPointType	bounds [6]	=
				{ attr.axis1.min, attr.axis1.max,
				  attr.axis2.min, attr.axis2.max,
				  attr.axis3.min, attr.axis3.max };
			_axisActor->SetBounds (bounds);
		}	// if (false == _axisProperties.autoAdjust)
		else
		{
			vtkFloatingPointType    bounds [6]	=
					{ NumericServices::doubleMachMax( ),
					  NumericServices::doubleMachMax( ),
					  NumericServices::doubleMachMax( ),
					  NumericServices::doubleMachMax( ),
					  NumericServices::doubleMachMax( ),
					  NumericServices::doubleMachMax( ) };
			getBoundingBox (
				bounds[0],bounds[1],bounds[2],bounds[3],bounds[4],bounds[5]);
			// On augmente de 2 * 10% la boite englobante de la vue a ajouter :
			for (int i = 0; i < 6; i += 2)
			{
				vtkFloatingPointType	delta	=
										0.1 * (bounds [i + 1] - bounds [i]);
				bounds [i]		= bounds [i] - delta;
				bounds [i + 1]	= bounds [i + 1] + delta;
			}	// for (int i = 0; i < 6; i += 2)
			_axisActor->SetBounds (bounds);
		}	// if (false == _axisProperties.autoAdjust)

		_axisActor->SetXAxisColor (_axisProperties.axis1.color.getDRed ( ),
						_axisProperties.axis1.color.getDGreen ( ),
						_axisProperties.axis1.color.getDBlue ( ));
		_axisActor->SetYAxisColor (_axisProperties.axis2.color.getDRed ( ),
						_axisProperties.axis2.color.getDGreen ( ),
						_axisProperties.axis2.color.getDBlue ( ));
		_axisActor->SetZAxisColor (_axisProperties.axis3.color.getDRed ( ),
						_axisProperties.axis3.color.getDGreen ( ),
						_axisProperties.axis3.color.getDBlue ( ));
		_axisActor->SetXUserTitle (_axisProperties.axis1.title.c_str ( ));
		_axisActor->SetYUserTitle (_axisProperties.axis2.title.c_str ( ));
		_axisActor->SetZUserTitle (_axisProperties.axis3.title.c_str ( ));
		_axisActor->SetXAxisLabelVisibility (_axisProperties.axis1.displayLabels);
		_axisActor->SetYAxisLabelVisibility (_axisProperties.axis2.displayLabels);
		_axisActor->SetZAxisLabelVisibility (_axisProperties.axis3.displayLabels);
		_axisActor->SetXAxisTickVisibility (_axisProperties.axis1.displayTicks);
		_axisActor->SetYAxisTickVisibility (_axisProperties.axis2.displayTicks);
		_axisActor->SetZAxisTickVisibility (_axisProperties.axis3.displayTicks);
		_axisActor->SetXAxisMinorTickVisibility (_axisProperties.axis1.displayMinorTicks);
		_axisActor->SetYAxisMinorTickVisibility (_axisProperties.axis2.displayMinorTicks);
		_axisActor->SetZAxisMinorTickVisibility (_axisProperties.axis3.displayMinorTicks);
		_axisActor->SetXLabelFormat (_axisProperties.axis1.format.c_str ( ));
		_axisActor->SetYLabelFormat (_axisProperties.axis2.format.c_str ( ));
		_axisActor->SetZLabelFormat (_axisProperties.axis3.format.c_str ( ));
		_axisActor->SetTickLocationToOutside ( );
		int	drawLines	= 1;
		_axisActor->SetDrawXGridlines (_axisProperties.axis1.displayLines);
		_axisActor->SetDrawYGridlines (_axisProperties.axis2.displayLines);
		_axisActor->SetDrawZGridlines (_axisProperties.axis3.displayLines);

		forceRender ( );
	}	// if (0 != _axisActor)
}	// VTKRenderingManager::setAxisProperties


void VTKRenderingManager::setDisplayFocalPoint (bool display)
{
	BEGIN_TRY_CATCH_BLOCK

	if ((true == display) && (0 == _focalPointAxesActor))
		_focalPointAxesActor	= vtkECMAxesActor::New ( );

	if (true == display)
	{
		CHECK_NULL_PTR_ERROR (_focalPointAxesActor)
		vtkCamera&	camera		= getCamera ( );
		double	x = 0., y = 0., z = 0.;
		camera.GetFocalPoint (x, y, z);
		_focalPointAxesActor->SetPosition (x - 1., x + 1., y - 1., y + 1., z - 1., z + 1.);
		getRenderer ( ).AddViewProp (_focalPointAxesActor);
	}	// if ((true == display) && (0 == _focalPointAxesActor))
	else
	{
		if (0 != _focalPointAxesActor)
			getRenderer ( ).RemoveViewProp (_focalPointAxesActor);
	}	// else if (true == display)

	forceRender ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// VTKRenderingManager::setDisplayFocalPoint


void VTKRenderingManager::enableMultipleLighting (bool enable)
{
	BEGIN_TRY_CATCH_BLOCK

	bool	updateView	= false;

	if (true == enable)
	{
		if (0 == _lightKit)
			_lightKit	= vtkLightKit::New ( );
		if (true == hasRenderer ( ))
		{
			_lightKit->AddLightsToRenderer (&getRenderer ( ));
			updateView	= true;
		}	// if (true == hasRenderer ( ))
	}
	else	// if (true == enable)
	{
		if (0 != _lightKit)
		{
			if (true == hasRenderer ( ))
			{
				_lightKit->RemoveLightsFromRenderer (&getRenderer ( ));
				updateView	= true;
			}	// if (true == hasRenderer ( ))

			_lightKit->Delete ( );
			_lightKit	= 0;
		}	// if (0 != _lightKit)
	}	// if (true == enable)

	forceRender ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// VTKRenderingManager::enableMultipleLighting


void VTKRenderingManager::resizeView (unsigned long w, unsigned long h)
{

	if (true == hasRenderWindow ( ))
		getRenderWindow ( ).SetSize (w, h);
}	// VTKRenderingManager::resizeView


void VTKRenderingManager::zoomView (double factor)
{
	if (0 != _camera)
		_camera->Zoom (factor);
}	// VTKRenderingManager::zoomView


void VTKRenderingManager::getViewDefinition (
			double& x, double& y, double& z, double& fx, double& fy, double& fz,
			double& upx, double& upy, double& upz)
{
	if (0 != _camera)
	{
		_camera->GetPosition (x, y, z);
		_camera->GetViewUp (upx, upy, upz);
		_camera->GetFocalPoint (fx, fy, fz);
	}	// if (0 != _camera)
	else
	{
		x	= y		= z		= 0.;
		fx	= fy	= fz	= 0.;
		upx	= upy	= upz	= 0.;
	}
}	// VTKRenderingManager::getViewDefinition


void VTKRenderingManager::defineView (
				double x, double y, double z, double fx, double fy, double fz,
				double upx, double upy, double upz)
{
	if (0 != _camera)
	{
		_camera->SetPosition (x, y, z);
		_camera->SetViewUp (upx, upy, upz);
		_camera->SetFocalPoint (fx, fy, fz);
		// ResetCameraClippingRange : en son absence il y a de bonnes chances
		// que certains objets ne soient plus affichés bien qu'ils devraient 
		// être visibles.
		if (true == hasRenderer ( ))
			getRenderer ( ).ResetCameraClippingRange ( );
	}	// if (0 != _camera)
	if (0 != _focalPointAxesActor)
		_focalPointAxesActor->SetPosition (fx - 1., fx + 1., fy - 1., fy + 1., fz - 1., fz + 1.);
}	// VTKRenderingManager::defineView


void VTKRenderingManager::resetView (bool render)
{
	if (0 != _camera)
	{
		// Annulation préalable d'un éventuel zoom :
		_camera->SetViewAngle (30.);	// Mode perspective
		_camera->SetParallelScale (1.);	// Mode parallele

		// Recadrage :
		if (true == hasRenderer ( ))
			getRenderer ( ).ResetCamera ( );
		if (0 != _focalPointAxesActor)
		{
			double	x = 0., y = 0., z = 0.;
			_camera->GetFocalPoint (x, y, z);
			_focalPointAxesActor->SetPosition (x - 1., x + 1., y - 1., y + 1., z - 1., z + 1.);
		}	// if (0 != _focalPointAxesActor)
	}	// if (0 != _camera)

	RenderingManager::resetView (render);
//_renderer->PrintSelf (cout,*vtkIndent::New( ));
}	// VTKRenderingManager::resetView


void VTKRenderingManager::setParallelProjection (bool parallel)
{
	if ((0 != _camera) && (parallel != (bool)_camera->GetParallelProjection( )))
	{
		if (true == parallel)
			_camera->ParallelProjectionOn ( );
		else
			_camera->ParallelProjectionOff ( );
	}	// if ((0 != _camera) && ...
}	// VTKRenderingManager::setParallelProjection


void VTKRenderingManager::updateView ( )
{
	if ((0 != _axisActor) && (true == _axisProperties.autoAdjust))
	{
		vtkFloatingPointType    bounds [6]	=
				{ NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ),
				  NumericServices::doubleMachMax( ) };
		getBoundingBox (
			bounds [0],bounds [1],bounds [2],bounds [3],bounds [4],bounds [5]);
		_axisActor->SetBounds (bounds);
	}	// if ((0 != _axisActor) && (true == _axisProperties.autoAdjust))
}	// VTKRenderingManager::updateView


void VTKRenderingManager::displayxOyViewPlane ( )
{
	vtkRenderWindowInteractor*	interactor		= true == hasRenderWindow( ) ? getRenderWindow( ).GetInteractor( ) : 0;
	vtkInteractorObserver*		interactorStyle	= 0 == interactor ? 0 : interactor->GetInteractorStyle ( );
	vtkUnifiedInteractorStyle*	unifiedInteractorStyle	= dynamic_cast<vtkUnifiedInteractorStyle*>(interactorStyle);
	if (0 != unifiedInteractorStyle)
		unifiedInteractorStyle->DisplayxOyViewPlane ( );
	else
		throw Exception (UTF8String ("VTKRenderingManager::displayxOyViewPlane : absence d'interacteur VTK supportant cette action.", Charset::UTF_8));
}	// VTKRenderingManager::displayxOyViewPlane


void VTKRenderingManager::displayxOzViewPlane ( )
{
	vtkRenderWindowInteractor*	interactor		= true == hasRenderWindow( ) ? getRenderWindow( ).GetInteractor( ) : 0;
	vtkInteractorObserver*		interactorStyle	= 0 == interactor ? 0 : interactor->GetInteractorStyle ( );
	vtkUnifiedInteractorStyle*	unifiedInteractorStyle	= dynamic_cast<vtkUnifiedInteractorStyle*>(interactorStyle);
	if (0 != unifiedInteractorStyle)
		unifiedInteractorStyle->DisplayxOzViewPlane ( );
	else
		throw Exception (UTF8String ("VTKRenderingManager::displayxOzViewPlane : absence d'interacteur VTK supportant cette action.", Charset::UTF_8));
}	// VTKRenderingManager::displayxOzViewPlane


void VTKRenderingManager::displayyOzViewPlane ( )
{
	vtkRenderWindowInteractor*	interactor		= true == hasRenderWindow( ) ? getRenderWindow( ).GetInteractor( ) : 0;
	vtkInteractorObserver*		interactorStyle	= 0 == interactor ? 0 : interactor->GetInteractorStyle ( );
	vtkUnifiedInteractorStyle*	unifiedInteractorStyle	= dynamic_cast<vtkUnifiedInteractorStyle*>(interactorStyle);
	if (0 != unifiedInteractorStyle)
		unifiedInteractorStyle->DisplayyOzViewPlane ( );
	else
		throw Exception (UTF8String ("VTKRenderingManager::displayyOzViewPlane : absence d'interacteur VTK supportant cette action.", Charset::UTF_8));
}	// VTKRenderingManager::displayyOzViewPlane


void VTKRenderingManager::resetRoll ( )
{
	vtkRenderWindowInteractor*	interactor		= true == hasRenderWindow( ) ? getRenderWindow( ).GetInteractor( ) : 0;
	vtkInteractorObserver*		interactorStyle	= 0 == interactor ? 0 : interactor->GetInteractorStyle ( );
	vtkUnifiedInteractorStyle*	unifiedInteractorStyle	= dynamic_cast<vtkUnifiedInteractorStyle*>(interactorStyle);
	if (0 != unifiedInteractorStyle)
		unifiedInteractorStyle->ResetRoll ( );
	else
		throw Exception (UTF8String ("VTKRenderingManager::resetRoll : absence d'interacteur VTK supportant cette action.", Charset::UTF_8));
}	// VTKRenderingManager::resetRoll


void VTKRenderingManager::forceRender ( )
{
	if (false == displayLocked ( ))
	{
		if ((true == hasRenderer ( )) && (true == hasRenderWindow ( )))
		{
/*			if (0 != _vtkWidget)
				_vtkWidget->updateView (true);
			else*/
				getRenderWindow ( ).Render ( );
		}	// if ((true == hasRenderer ( )) && (true == hasRenderWindow ( )))
	}	// if (false == displayLocked ( ))
}	// VTKRenderingManager::forceRender


void VTKRenderingManager::setFrameRates (double stillFR, double desiredFR)
{
	try
	{
		getRenderWindow ( ).SetDesiredUpdateRate (desiredFR);
		if (0 != getRenderWindow ( ).GetInteractor ( ))
		{
			getRenderWindow ( ).GetInteractor ( )->SetStillUpdateRate (stillFR);
			getRenderWindow ( ).GetInteractor ( )->SetDesiredUpdateRate (desiredFR);
		}	// if (0 != getRenderWindow ( ).GetInteractor ( ))
	}
	catch (...)
	{
	}
}	// VTKRenderingManager::setFrameRates


bool VTKRenderingManager::hasRenderer ( ) const
{
	return 0 == _renderer ? false : true;
}	// VTKRenderingManager::hasRenderer


bool VTKRenderingManager::hasRenderWindow ( ) const
{
	return 0 == _renderWindow ? false : true;
}	// VTKRenderingManager::hasRenderWindow


vtkRenderer& VTKRenderingManager::getRenderer ( ) const
{
	if (0 != _renderer)
		return *_renderer;

	throw Exception (UTF8String ("VTKRenderingManager::getRenderer : absence de renderer.", Charset::UTF_8));
}	// VTKRenderingManager::getRenderer


vtkRenderer& VTKRenderingManager::getRenderer ( )
{
	if (0 != _renderer)
		return *_renderer;

	throw Exception (UTF8String ("VTKRenderingManager::getRenderer : absence de renderer.", Charset::UTF_8));
}	// VTKRenderingManager::getRenderer


vtkCamera& VTKRenderingManager::getCamera ( )
{
	if (0 != _camera)
		return *_camera;

	throw Exception (UTF8String ("VTKRenderingManager::getCamera : absence de caméra.", Charset::UTF_8));
}	// VTKRenderingManager::getCamera


vtkRenderWindow& VTKRenderingManager::getRenderWindow ( )
{
	if (0 != _renderWindow)
		return *_renderWindow;

	throw Exception (UTF8String ("VTKRenderingManager::getRenderWindow : absence de window.", Charset::UTF_8));
}	// VTKRenderingManager::getRenderWindow


vtkMgx3DInteractorStyle& VTKRenderingManager::getMgx3DInteractorStyle ( )
{
	vtkRenderWindow&		window		= getRenderWindow ( );
	if (0 == window.GetInteractor ( ))
		throw Exception (UTF8String ("VTKRenderingManager::getMgx3DInteractorStyle : absence d'interacteur.", Charset::UTF_8));
	vtkInteractorObserver*	observer	= window.GetInteractor ( )->GetInteractorStyle ( );
	if (0 == observer)
		throw Exception (UTF8String ("VTKRenderingManager::getMgx3DInteractorStyle : absence de style d'interacteur.", Charset::UTF_8));

	vtkMgx3DInteractorStyle*	mgxInteractor	= dynamic_cast<vtkMgx3DInteractorStyle*>(observer);
	if (0 == mgxInteractor)
		throw Exception (UTF8String ("VTKRenderingManager::getMgx3DInteractorStyle : le style d'interacteur n'est pas un style Magix 3D.", Charset::UTF_8));

	return *mgxInteractor;
}	// VTKRenderingManager::getMgx3DInteractorStyle


void VTKRenderingManager::focalPointModifiedCallback ( )
{
	if (0 != _focalPointAxesActor)
	{
		vtkCamera&	camera		= getCamera ( );
		double	x = 0., y = 0., z = 0.;
		camera.GetFocalPoint (x, y, z);
		_focalPointAxesActor->SetPosition (x - 1., x + 1., y - 1., y + 1., z - 1., z + 1.);
		forceRender ( );
	}	// if (0 != _focalPointAxesActor)
}	// VTKRenderingManager::focalPointModifiedCallback


Utils::GraphicalEntityRepresentation* VTKRenderingManager::createRepresentation (Entity& entity)
{
 	GeomEntity*			geomEntity		= dynamic_cast<GeomEntity*>(&entity);
	TopoEntity*			topoEntity		= dynamic_cast<TopoEntity*>(&entity);
	MeshEntity*			meshEntity		= dynamic_cast<MeshEntity*>(&entity);
	CoordinateSystem::SysCoord*	repEntity		= dynamic_cast<CoordinateSystem::SysCoord*>(&entity);
	StructuredMeshEntity*		structuredMeshEntity	= dynamic_cast<StructuredMeshEntity*>(&entity);

	if (0 != geomEntity)
		return new VTKMgx3DGeomEntityRepresentation (*geomEntity);
	else if (0 != topoEntity)
		return new VTKMgx3DTopoEntityRepresentation (*topoEntity);
	else if (0 != repEntity)
		return new VTKMgx3DSysCoordEntityRepresentation (*repEntity);
	else if (0 != meshEntity)
	{
	    // cas particulier pour les sous-volumes, il n'y a pas de groupe associé dans GMDS
	    SubVolume* sv = dynamic_cast<SubVolume*>(&entity);
	    SubSurface* ss = dynamic_cast<SubSurface*>(&entity);
	    if (sv || ss)
	        return new VTKMgx3DMeshEntityRepresentation (*meshEntity);
	    else
	        return new VTKGMDSEntityRepresentation (*meshEntity);
	}	// else if (0 != meshEntity)
	else if (0 != structuredMeshEntity)
		return new VTKMgx3DStructuredMeshEntityRepresentation (*structuredMeshEntity);

	return 0;
}	// VTKRenderingManager::createRepresentation


void VTKRenderingManager::displayRepresentation (
	Utils::GraphicalEntityRepresentation& representation, bool show,
	bool forceUpdate, unsigned long mask)
{
	VTKEntityRepresentation*	vtkRep	= dynamic_cast<VTKEntityRepresentation*>(&representation);
	CHECK_NULL_PTR_ERROR (vtkRep);
	if (true == show)
	{
		vtkRep->setRenderingManager (this);	// Nécessaire pour le code de projetections, affichage 3D/CELL_VALUES
		vtkRep->updateRepresentation (mask, forceUpdate);
	}	// if (true == show)
	vtkRep->show (*this, show);

	forceRender ( );
}	// VTKRenderingManager::displayRepresentation


void VTKRenderingManager::initialize (bool compositing)
{
	GraphicalRepresentationFactoryIfc::setInstance (new VTKGraphicalRepresentationFactory ( ));
	if (false == compositing)
	{
		vtkECMFactory*	ecmVTKFactory	= vtkECMFactory::New ( );
		vtkObjectFactory::RegisterFactory (ecmVTKFactory);
		ecmVTKFactory->Delete ( );
	}	// if (false == compositing)
	
	// ATTENTION : appel vital pour un bon affichage des bras.
	// En son absence les segments de droites, coplanaires avec des surfaces à
	// afficher, ne sont que partiellement dessinés => il est difficile 
	// d'apprécier les limites des mailles.
	vtkMapper::SetResolveCoincidentTopologyToPolygonOffset ( );

	// CP : éviter un appel à exit (1) en cas de memory leak détecté à la terminaison
	// du programme :
	vtkDebugLeaks::SetExitError (0);
}	// VTKRenderingManager::initialize


void VTKRenderingManager::finalize ( )
{
}	// VTKRenderingManager::finalize


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
