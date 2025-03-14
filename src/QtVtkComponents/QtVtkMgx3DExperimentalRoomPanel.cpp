/**
 * \file        QtVtkMgx3DExperimentalRoomPanel.cpp
 * \author      Charles PIGNEROL
 * \date        30/08/2016
 */

#ifdef USE_EXPERIMENTAL_ROOM

#include "Internal/ContextIfc.h"
#include "QtComponents/QtRepresentationTypesPanel.h"
#include "QtVtkComponents/QtVtkMgx3DExperimentalRoomPanel.h"
#include "QtVtkComponents/VTKEntityRepresentation.h"
#include "Utils/Common.h"
#include "Utils/Entity.h"
#include "Utils/Unit.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtAutoWaitingCursor.h>

#include <QMessageBox>

#include <cmath>

#include <assert.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::QtVtkComponents;


// ============================================================================
//                              FONCTIONS STATIQUES
// ============================================================================

namespace Mgx3D
{

namespace QtVtkComponents
{
	
ExperimentRay::LANDMARK landmarkToExpRoomLandmark (Utils::Landmark::kind landmark)
{
	ExperimentRay::LANDMARK	lk	= (ExperimentRay::LANDMARK)-1;
	switch (landmark)
	{
		case Landmark::maillage	: lk	= ExperimentRay::MESH_LANDMARK;			break;
		case Landmark::chambre	: lk	= ExperimentRay::CARTESIAN_EXP_ROOM;	break;
		default	:
			throw Exception (UTF8String ("Repère non défini (Menu Session.Repère).", Charset::UTF_8));
	}	// switch (landmark)	

	return lk;
}	// landmarkToExpRoomLandmark

}	// namespace QtVtkComponents

}	// namespace Mgx3D


// ===========================================================================
//                        LA CLASSE QtVtkMgx3DExperimentalRoomPanel
// ===========================================================================


namespace Mgx3D
{

namespace QtVtkComponents
{

QtVtkMgx3DExperimentalRoomPanel::QtVtkMgx3DExperimentalRoomPanel (
		QtMgx3DMainWindow* parent, const UTF8String& name, const UTF8String& expLandmarkName, const UTF8String& appTitle,
		vtkRenderer& renderer)
	: QtVtkExperimentalRoomPanel ((QWidget*)parent, name, expLandmarkName, appTitle, renderer, true),
	  _context (0)
{
}	// QtVtkMgx3DExperimentalRoomPanel::QtVtkMgx3DExperimentalRoomPanel


QtVtkMgx3DExperimentalRoomPanel::QtVtkMgx3DExperimentalRoomPanel (const QtVtkMgx3DExperimentalRoomPanel&)
	: QtVtkExperimentalRoomPanel (
		0, "Invalid QtVtkMgx3DExperimentalRoomPanel", "?", "?", *vtkRenderer::New ( )),
	  _context (0)
{
	MGX_FORBIDDEN("QtVtkMgx3DExperimentalRoomPanel copy constructor is not allowed.");
}	// QtVtkMgx3DExperimentalRoomPanel::QtVtkMgx3DExperimentalRoomPanel (const QtVtkMgx3DExperimentalRoomPanel&)


QtVtkMgx3DExperimentalRoomPanel& QtVtkMgx3DExperimentalRoomPanel::operator = (const QtVtkMgx3DExperimentalRoomPanel&)
{
	MGX_FORBIDDEN ("QtVtkMgx3DExperimentalRoomPanel assignment operator is not allowed.");
	return *this;
}	// QtVtkMgx3DExperimentalRoomPanel::QtVtkMgx3DExperimentalRoomPanel (const QtVtkMgx3DExperimentalRoomPanel&)


QtVtkMgx3DExperimentalRoomPanel::~QtVtkMgx3DExperimentalRoomPanel ( )
{
}	// QtVtkMgx3DExperimentalRoomPanel::~QtVtkMgx3DExperimentalRoomPanel


void QtVtkMgx3DExperimentalRoomPanel::setExperimentalRoom (ExperimentalRoom& room, ExperimentalRoomIOManager* ios)
{
	QtVtkExperimentalRoomPanel::setExperimentalRoom (room, ios);
}	// QtVtkMgx3DExperimentalRoomPanel::setExperimentalRoom


Context& QtVtkMgx3DExperimentalRoomPanel::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_context)
	return *_context;
}	// QtVtkMgx3DExperimentalRoomPanel::getContext


void QtVtkMgx3DExperimentalRoomPanel::setContext (Context& context, bool global)
{
	const Landmark::kind	landmark	= context.getLandmark ( );
	ExperimentRay::LANDMARK	lk			= landmarkToExpRoomLandmark (landmark);
	const Unit::lengthUnit	unit		= context.getLengthUnit ( );
	if (Unit::undefined == unit)
		throw Exception (UTF8String ("Unité de longueur non définie (Menu Session.Unité).", Charset::UTF_8));

	setMeterPow (Unit::toMeterExposantFactor (unit));
	setRenderingLandmark (lk);
	_context	= &context;

	useSelection (context, global);
}	// QtVtkMgx3DExperimentalRoomPanel::setContext


void QtVtkMgx3DExperimentalRoomPanel::useSelection (Context& context, bool global)
{
	// Y a-t-il des entités géométriques de sélectionnées qui feraient office de
	// cibles aux rayons ?
	vector<vtkUnstructuredGrid*>	grids;
	vector<vtkPolyData*>			polydatas;
	vector<string>					names;
	setTargetSurfaces (grids, polydatas, names);

	uint nbEntitiesWithoutTriangulation = 0;
	uint nbEntitiesWithTriangulation = 0;
	vector<Entity*>	entities	= context.getSelectionManager ( ).getEntities ( );
	for(vector<Entity*>::iterator it = entities.begin ( );
	    entities.end ( ) != it; it++)
	{
		if (false == (*it)->getDisplayProperties ( ).isDisplayable ( ))
			continue;
		DisplayProperties::GraphicalRepresentation*	gr	= 0;
		switch ((*it)->getType ( ))
		{
			case Entity::GeomSurface	:
			case Entity::GeomVolume		:
			case Entity::MeshSurface	:
			case Entity::MeshSubSurface	:
			case Entity::MeshVolume		:
			case Entity::MeshSubVolume	:
			{
				unsigned long   mask    = 0;
				gr  = (*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
				CHECK_NULL_PTR_ERROR (gr)
				mask  = true == global ? context.globalMask ((*it)->getType ( )) : gr->getRepresentationMask ( );   // Correctif CP 31/07/20 : inefficace en mode global
				bool useSurfaceMask = (0 != (GraphicalEntityRepresentation::SURFACES & mask) ? true : false);
				bool useVolumeMask  = (0 != (GraphicalEntityRepresentation::VOLUMES & mask) ? true : false);
				// pas de représentation triangulaire
				if (!useSurfaceMask && !useVolumeMask)
					nbEntitiesWithoutTriangulation += 1;
				else
					nbEntitiesWithTriangulation += 1;
			}
			break;
		}	// switch ((*it)->getType ( ))
		if (0 == gr)
			continue;

		VTKEntityRepresentation*	vtkRepresentation	= dynamic_cast<VTKEntityRepresentation*>(gr);
		vtkPolyData*				polydata			= 0 == vtkRepresentation ? 0 : vtkRepresentation->getSurfacicPolyData ( );
		if (0 == polydata)
			continue;

		polydatas.push_back (polydata);
		names.push_back ((*it)->getName ( ));
	}	// for(vector<Entity*>::iterator it = entities.begin ( ); ...

//	std::cout<<"nbEntitiesWithoutTriangulation = "<<nbEntitiesWithoutTriangulation<<std::endl;
//	std::cout<<"nbEntitiesWithTriangulation = "<<nbEntitiesWithTriangulation<<std::endl;

	// analyse de ce qui a été vu pour un éventuel message d'erreur explicite pour l'utilisateur
	if (entities.empty() || 0 == (nbEntitiesWithoutTriangulation+nbEntitiesWithTriangulation)){
		throw Exception (UTF8String ("Il faut sélectionner des surfaces ou des volumes (géométriques ou de maillage).", Charset::UTF_8));
	}
	else if (nbEntitiesWithTriangulation == 0 && nbEntitiesWithoutTriangulation > 0) {
		throw Exception (UTF8String ("Pour les entités géométriques, il faut activer la représentation dite surfacique (et non pas filaire)", Charset::UTF_8));
	}

	setTargetSurfaces (grids, polydatas, names);	// Susceptible de modifier unit.

	updateRaysRepresentations (true, true);
}	// QtVtkMgx3DExperimentalRoomPanel::useSelection


void QtVtkMgx3DExperimentalRoomPanel::synchronizeMeshUnit (char exp)
{
	getContext ( ).setLengthUnit ((Unit::lengthUnit)(exp));
	setMeterPow (exp);
}	// QtVtkMgx3DExperimentalRoomPanel::synchronizeMeshUnit


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// USE_EXPERIMENTAL_ROOM
