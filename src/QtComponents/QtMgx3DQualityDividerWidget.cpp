/**
 * \file		QtMgx3DQualityDividerWidget.cpp
 * \author		Charles PIGNEROL
 * \date		07/01/2025
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DQualityDividerWidget.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "Utils/GraphicalEntityRepresentation.h"
#include "Utils/Common.h"
#include "Mesh/Mgx3DQualifSerie.h"
#include "Mesh/Surface.h"
#include "Mesh/SubSurface.h"
#include "Mesh/Volume.h"
#include "Mesh/SubVolume.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtErrorManagement.h>


using namespace GQualif;
using namespace gmds;
using namespace Mgx3D;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Utils;
using namespace TkUtil;
using namespace std;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                         Classe QtMgx3DQualityDividerWidget
// ===========================================================================


QtMgx3DQualityDividerWidget::QtMgx3DQualityDividerWidget (QWidget* parent, Context* context, QtMgx3DMainWindow* mainWindow)
	: QtQualityDividerWidget (parent, "Magix 3D"), _mainWindow (mainWindow), _displayButton (0), _initializeButton (0), _context (context), _extractions ( ), _criterionName ( )
{
	_displayButton	= new QPushButton ("Afficher", this);
	getButtonsLayout ( ).addWidget (_displayButton);
	connect (_displayButton, SIGNAL (clicked (bool)), this, SLOT (displayCellsCallback( )));
	_initializeButton	= new QPushButton("Réinitialiser", this);
	getButtonsLayout ( ).addWidget (_initializeButton);
	connect (_initializeButton, SIGNAL (clicked (bool)), this, SLOT (reinitializeCallback( )));
}	// QtMgx3DQualityDividerWidget::QtMgx3DQualityDividerWidget


QtMgx3DQualityDividerWidget::QtMgx3DQualityDividerWidget (const QtMgx3DQualityDividerWidget&)
	: QtQualityDividerWidget (0, "Magix 3D"), _mainWindow (0), _displayButton (0), _initializeButton (0), _context (0), _extractions ( ), _criterionName ( )
{
	MGX_FORBIDDEN ("QtMgx3DQualityDividerWidget copy constructor is not allowed.")
}	// QtMgx3DQualityDividerWidget::QtMgx3DQualityDividerWidget


QtMgx3DQualityDividerWidget& QtMgx3DQualityDividerWidget::operator = (const QtMgx3DQualityDividerWidget&)
{
	MGX_FORBIDDEN ("QtMgx3DQualityDividerWidget assignment operator is not allowed.")
	return *this;
}	// QtMgx3DQualityDividerWidget::operator =


QtMgx3DQualityDividerWidget::~QtMgx3DQualityDividerWidget ( )
{
	removeDataGroupSubsets ( );
}	// QtMgx3DQualityDividerWidget::~QtMgx3DQualityDividerWidget


void QtMgx3DQualityDividerWidget::computeCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	removeDataGroupSubsets ( );
	_criterionName	= Qualif::CRITERESTR [getCriterion ( )];

	QtQualityDividerWidget::computeCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : recherche de mailles selon critère.")
}	// QtMgx3DQualityDividerWidget::computeCallback


void QtMgx3DQualityDividerWidget::displayExtraction (size_t i, bool display)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Mesh::MeshItf*								mesh	= getContext ( ).getMeshManager ( ).getMesh ( );
	CHECK_NULL_PTR_ERROR (mesh)
	MeshEntity*									entity	= 0;	// L'entité (surface/volume) à afficher
	DisplayProperties::GraphicalRepresentation*	rep		= 0;	// et sa représentation graphique
	AbstractQualifSerie*	serie	= dynamic_cast<AbstractQualifSerie*>(&getSerie (i));
	CHECK_NULL_PTR_ERROR (serie)
	map<AbstractQualifSerie*, MeshEntity*>::iterator	it	= _extractions.find (serie);
	if (_extractions.end ( ) != it)
		entity	= it->second;
	else
	{
		MeshManager*	meshManager	= dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager ( ));
		CHECK_NULL_PTR_ERROR (meshManager)
		gmds::Mesh&			gmdsMesh	= meshManager->getMesh ( )->getGMDSMesh ( );
		UTF8String	name (Charset::UTF_8);
		name << serie->getName ( ) << '_' << _criterionName;
		if (true == serie->isVolumic ( ))
		{
			const Mesh::Mgx3DVolumeQualifSerie* volumeSerie	= dynamic_cast<const Mesh::Mgx3DVolumeQualifSerie*>(serie);
			CHECK_NULL_PTR_ERROR (volumeSerie)
			vector <gmds::TCellID>	cellsIds;
			volumeSerie->getGMDSCellsIndexes (cellsIds, 0);	// Une seule classe => 0
			gmds::CellGroup<gmds::Region>*	gmdsVolume	= mesh->getGMDSMesh ( ).newGroup<gmds::Region> (name);

			SubVolume*	volume	= new SubVolume (getContext ( ), getContext ( ).newProperty (Entity::MeshVolume, name), getContext ( ).newDisplayProperties (Entity::MeshVolume), 0);
			CHECK_NULL_PTR_ERROR (volume)
			for (vector <gmds::TCellID>::const_iterator itc = cellsIds.begin ( ); cellsIds.end ( ) != itc; itc++)
			{
				gmds::Region	region	= gmdsMesh.get<gmds::Region>(*itc);
				volume->addRegion (region);
			}
			meshManager->add (volume);
			entity	= volume;
		}	// if (true == serie->isVolumic ( ))
		else
		{
			const Mesh::Mgx3DSurfaceQualifSerie* surfaceSerie	= dynamic_cast<const Mesh::Mgx3DSurfaceQualifSerie*>(serie);
			CHECK_NULL_PTR_ERROR (surfaceSerie)
			vector <gmds::TCellID>	cellsIds;
			surfaceSerie->getGMDSCellsIndexes (cellsIds, 0);	// Une seule classe => 0
			gmds::CellGroup<gmds::Face>*	gmdsSurface	= mesh->getGMDSMesh ( ).newGroup<gmds::Face> (name);

			SubSurface*	surface	= new SubSurface (getContext ( ), getContext ( ).newProperty (Entity::MeshSurface, name), getContext ( ).newDisplayProperties (Entity::MeshSurface), 0);
			CHECK_NULL_PTR_ERROR (surface)
			for (vector <gmds::TCellID>::const_iterator itc = cellsIds.begin ( ); cellsIds.end ( ) != itc; itc++)
			{
				gmds::Face face	= gmdsMesh.get<gmds::Face>(*itc);
				surface->addFace (face);
			}
			meshManager->add (surface);
			entity	= surface;
		}	// else if (true == serie->isVolumic ( ))

		_extractions.insert (pair<AbstractQualifSerie*, MeshEntity*>(serie, entity));
		getContext ( ).newGraphicalRepresentation (*entity);
		rep		= entity->getDisplayProperties ( ).getGraphicalRepresentation ( );
		if (true == serie->isVolumic ( ))
		{
			CHECK_NULL_PTR_ERROR (rep)
			rep->setRepresentationMask (GraphicalEntityRepresentation::CURVES | GraphicalEntityRepresentation::VOLUMES);
		}	// if (true == serie->isVolumic ( ))
		CHECK_NULL_PTR_ERROR (_mainWindow)
		vector<MeshEntity*>	entities;
		entities.push_back (entity);
		_mainWindow->getEntitiesPanel ( ).addMeshEntities (entities, true);
	}	// else if (_extractions.end ( ) != it)

	CHECK_NULL_PTR_ERROR (entity)
	entity->getDisplayProperties ( ).setDisplayed (display);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : recherche de mailles selon critère.")
}	// QtMgx3DQualityDividerWidget::displayExtraction


Context& QtMgx3DQualityDividerWidget::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_context)
	return *_context;
}	// QtMgx3DQualityDividerWidget::getContext


const Context& QtMgx3DQualityDividerWidget::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_context)
	return *_context;
}	// QtMgx3DQualityDividerWidget::getContext


void QtMgx3DQualityDividerWidget::removeDataGroupSubsets ( )
{
	MeshManager*	manager	= dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager ( ));
	CHECK_NULL_PTR_ERROR (manager)
	gmds::Mesh&		mesh	= manager->getMesh ( )->getGMDSMesh ( );

	// On masque les entités :
	CHECK_NULL_PTR_ERROR (_mainWindow)
	vector<MeshEntity*>	entities;
	for (map<AbstractQualifSerie*, MeshEntity*>::iterator ite = _extractions.begin ( ); _extractions.end ( ) != ite; ite++)
		entities.push_back (ite->second);
	_mainWindow->getEntitiesPanel ( ).removeMeshEntities (entities);		

	while (false == _extractions.empty ( ))
	{
		map<AbstractQualifSerie*, MeshEntity*>::iterator it = _extractions.begin ( );
		// On détruit ici les éléments de maillage Magix et GMDS car ce n'est pas fait par ailleurs.
		unique_ptr<MeshEntity>	entity (it->second);	// => Destruction forcée de *it.
		Surface*	surface	= dynamic_cast<Mesh::Surface*>(it->second);
		Volume*		volume	= dynamic_cast<Mesh::Volume*>(it->second);
		if (0 != volume)
		{
			try
			{
				mesh.deleteGroup<gmds::Region>(mesh.getGroup<gmds::Region>(entity->getName ( )));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non renseignée survenue durant le nettoyage du panneau Recherche selon critère.";
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
			try
			{
				manager->remove (volume);
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non renseignée survenue durant le nettoyage du panneau Recherche selon critère.";
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
		}	// if (0 != volume)
		if (0 != surface)
		{
			try
			{
				mesh.deleteGroup<gmds::Face>(mesh.getGroup<gmds::Face>(entity->getName ( )));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non renseignée survenue durant le nettoyage du panneau Recherche selon critère.";
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
			try
			{
				manager->remove (surface);
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non renseignée survenue durant le nettoyage du panneau Recherche selon critère.";
				getContext ( ).getLogDispatcher ( ).log (ErrorLog (error));
			}
		}	// if (0 != surface)
		
		_extractions.erase (it);
	}	// while (false == _extractions.empty ( ))
}	// QtMgx3DQualityDividerWidget::removeDataGroupSubsets


void QtMgx3DQualityDividerWidget::displayCellsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	
	displaySelectedExtractions (true);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : recherche de mailles selon critère.")
}	// QtMgx3DQualityDividerWidget::displayCellsCallback


void QtMgx3DQualityDividerWidget::reinitializeCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	removeDataGroupSubsets ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation du panneau de recherche de mailles selon critère")
}	// QtMgx3DQualityDividerWidget::reinitializeCallback

}	// namespace QtComponents

}	// namespace Mgx3D

