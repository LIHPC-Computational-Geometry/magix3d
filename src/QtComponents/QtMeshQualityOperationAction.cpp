/**
 * \file        QtMeshQualityOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        18/07/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/InfoCommand.h"
#include "Mesh/SubSurface.h"
#include "Mesh/Surface.h"
#include "Mesh/SubVolume.h"
#include "Mesh/Volume.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMeshQualityOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

#include "Mesh/MeshManagerIfc.h"
#include "Mesh/MeshItf.h"
#include "Mesh/Mgx3DQualifSerie.h"

#include <GMDS/IG/IGMesh.h>

using namespace std;
using namespace TkUtil;
using namespace GQualif;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace GQualif
{
	class GMDSQualifSerie;
}	// namespace GQualif



namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtMeshQualityOperationPanel
// ===========================================================================

QtMeshQualityOperationPanel::QtMeshQualityOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (
			0/*parent*/, mainWindow, action, helpURL, helpTag),
	  _qualifWidget (0), _displayCellsButton (0), _initializeButton (0),
	  _analysedMeshEntities ( ), _meshEntities ( ), _gmdsSurfaces ( ), _gmdsVolumes ( )
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QVBoxLayout*	vlayout	= new QVBoxLayout (0);
	vlayout->setSpacing (QtConfiguration::spacing);
	vlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);
	setLayout (vlayout);
	QScrollArea*	scrollArea	= new QScrollArea (this);
	vlayout->addWidget (scrollArea, 10);
	scrollArea->setFrameStyle (QFrame::NoFrame | QFrame::Plain);
	scrollArea->setWidgetResizable (true);
	const QSizePolicy::Policy	grow	=
									(QSizePolicy::Policy)QSizePolicy::GrowFlag;
	scrollArea->setSizePolicy (grow, grow);
	Internal::Context*	context	=
					dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	_qualifWidget	= new QtMgx3DQualifWidget (this, context);
	scrollArea->setWidget (_qualifWidget);

	// Les boutons : ATTENTION, dans le QtQualifWidget.
	_displayCellsButton	= new QPushButton("Afficher", _qualifWidget);
	_qualifWidget->getButtonsLayout ( ).addWidget (_displayCellsButton);
	connect (_displayCellsButton, SIGNAL (clicked (bool)), this,
							SLOT (displayCellsCallback( )));
	_initializeButton	= new QPushButton("Réinitialiser", _qualifWidget);
	_qualifWidget->getButtonsLayout ( ).addWidget (_initializeButton);
	connect (_initializeButton, SIGNAL (clicked (bool)), this,
							SLOT (reinitializeCallback( )));
	QPushButton*	closeButton	 = new QPushButton("Fermer", _qualifWidget);
	_qualifWidget->getButtonsLayout ( ).addWidget (closeButton);
	connect (closeButton, SIGNAL(clicked (bool)), this, SLOT(closeCallback( )));
	QPushButton*	helpButton	 = new QPushButton("Aide ...", _qualifWidget);
	_qualifWidget->getButtonsLayout ( ).addWidget (helpButton);
	connect (helpButton, SIGNAL (clicked (bool)), this, SLOT (helpCallback( )));

	autoUpdate ( );
}	// QtMeshQualityOperationPanel::QtMeshQualityOperationPanel


QtMeshQualityOperationPanel::QtMeshQualityOperationPanel (
										const QtMeshQualityOperationPanel& cao)
	: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow (0),	0, "", ""),
	  _qualifWidget (0), _displayCellsButton (0), _initializeButton (0),
	  _analysedMeshEntities ( ), _meshEntities ( ), _gmdsSurfaces ( ), _gmdsVolumes ( )
{
	MGX_FORBIDDEN ("QtMeshQualityOperationPanel copy constructor is not allowed.");
}	// QtMeshQualityOperationPanel::QtMeshQualityOperationPanel (const QtMeshQualityOperationPanel&)


QtMeshQualityOperationPanel& QtMeshQualityOperationPanel::operator = (
											const QtMeshQualityOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshQualityOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshQualityOperationPanel::QtMeshQualityOperationPanel (const QtMeshQualityOperationPanel&)


QtMeshQualityOperationPanel::~QtMeshQualityOperationPanel ( )
{
	try
	{
		if (0 != getMainWindow ( ))
			getMainWindow ( )->unregisterAdditionalOperationPanel (*this);

		clear ( );
		clearSeries ( );

		if (0 != getMgx3DOperationAction ( ))
		{
			// Ugly code :
			delete getMgx3DOperationAction ( );
		}
	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau Qualité de "
		      << "maillage : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau Qualité de "
		      << "maillage : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant la fermeture du "
		      << "panneau Qualité de maillage.";
		log (ErrorLog (error));
	}
}	// QtMeshQualityOperationPanel::~QtMeshQualityOperationPanel


void QtMeshQualityOperationPanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtMeshQualityOperationPanel::void setVisible


void QtMeshQualityOperationPanel::commandModifiedCallback (InfoCommand& infoCommand)
{
	// Est-on concerné ?
	if (0 == infoCommand.getNbMeshInfoEntity ( ))
		return;
			
	bool	reinited	= false;
	for (uint i = 0; (false == reinited) && (i < infoCommand.getNbMeshInfoEntity ( )); i++)
	{
		InfoCommand::type   t	= InfoCommand::DELETED;
		MeshEntity*			me	= 0;
		infoCommand.getMeshInfoEntity (i, me, t);
		// Est-ce une entité soumise à analyse ? Si oui on réinitialise,
		// sinon on ne fait rien :
		for (vector<Mgx3D::Mesh::MeshEntity*>::iterator itme = _analysedMeshEntities.begin ( );
		     _analysedMeshEntities.end ( ) != itme; itme++)
		{
			if (*itme == me)
		    {
				switch (t)
				{
					case InfoCommand::DELETED		: _analysedMeshEntities.erase (itme);
						// + la suite, pas de break
					case InfoCommand::DISPMODIFIED	: reinited	= true;	break;						
				}	// switch (t)
				
				break;
			}	// if (*itme == me)
		}	// for (uint i = 0; (false == reinited) && (i < infoCommand.getNbMeshInfoEntity ( )); i++)

		if (true == reinited)
			autoUpdate ( );
	}	// for (uint i = 0; i < infoCommand.getNbMeshInfoEntity ( ); i++)
}	// QtMeshQualityOperationPanel::commandModifiedCallback


void QtMeshQualityOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	clear ( );
	clearSeries ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtMeshQualityOperationPanel::reset


void QtMeshQualityOperationPanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	UTF8String	error (Charset::UTF_8);

	try
	{
		QtMgx3DOperationPanel::validate ( );
	}
	catch (const Exception& exc)
	{
		error << exc.getFullMessage ( );
	}
	catch (...)
	{
		error << "QtMeshQualityOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshQualityOperationPanel::validate


void QtMeshQualityOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	clear ( );
	clearSeries ( );
}	// QtMeshQualityOperationPanel::cancel


void QtMeshQualityOperationPanel::autoUpdate ( )
{
	try
	{

		Internal::Context*	context	=
				dynamic_cast<Internal::Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)
		Mesh::MeshManager*	manager	=
				dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
		CHECK_NULL_PTR_ERROR (manager)
		CHECK_NULL_PTR_ERROR (getContext ( ).getMeshManager ( ).getMesh ( ))
		gmds::IGMesh&	mesh	=
				getContext ( ).getMeshManager ( ).getMesh ( )->getGMDSMesh ( );

		clearSeries ( );

		QtMgx3DOperationPanel::autoUpdate ( );

		getQualityWidget ( ).removeSeries (true);
		clear ( );

		// Récupération des éléments de maillage sélectionnés :
		vector<string>	selectedSurfacesNames	=
				getContext ( ).getSelectionManager ( ).getEntitiesNames (
						Entity::MeshSurface);
		vector<string>	selectedVolumesNames	=
				getContext ( ).getSelectionManager ( ).getEntitiesNames (
						Entity::MeshVolume);
		//		gmds::Mesh<MeshItf::TMask>&	gmdsMesh	=
		//						(gmds::Mesh<MeshItf::TMask>&)mesh.getGMDSMesh ( );

		// les groupes de mailles qui seront évalués par Qualif
		std::vector<Mesh::Volume*> volumes;
		std::vector<Mesh::Surface*> surfaces;

		for (vector<string>::const_iterator its = selectedSurfacesNames.begin ( );
				selectedSurfacesNames.end ( ) != its; its++)
		{
			Mesh::Surface*	surface	=
					getContext ( ).getMeshManager ( ).getSurface (*its, true);
			CHECK_NULL_PTR_ERROR (surface)
			surfaces.push_back(surface);
		}
		for (vector<string>::const_iterator itv = selectedVolumesNames.begin ( );
				selectedVolumesNames.end ( ) != itv; itv++)
		{
			Mesh::Volume*	volume	=
					getContext ( ).getMeshManager ( ).getVolume (*itv, true);
			CHECK_NULL_PTR_ERROR (volume)
			volumes.push_back(volume);
		}

		// s'il n'y a rien de sélectionné, on prend tout [EB]
		if (volumes.empty())
			getContext ( ).getMeshManager().getVolumes(volumes);

		if (volumes.empty() && surfaces.empty())
			getContext ( ).getMeshManager().getSurfaces(surfaces);

		for (std::vector<Mesh::Surface*> ::const_iterator iter = surfaces.begin ( );
				surfaces.end() != iter; iter++)
		{
			vector<gmds::Face>	faces;
			(*iter)->getGMDSFaces (faces);
			gmds::IGMesh::surface&	s	= mesh.newSurface((*iter)->getName());
			_gmdsSurfaces.push_back ((*iter)->getName());
			for (vector<gmds::Face>::const_iterator itf = faces.begin ( );
					faces.end ( ) != itf; itf++)
				s.add (*itf);
			_analysedMeshEntities.push_back (*iter);
			Mgx3DQualifSerie* serie	= new Mgx3DQualifSerie(s, (*iter)->getName(), "", *iter);
			getQualityWidget ( ).addSerie (serie);
		}	// for (vector<string>::const_iterator iter = ...

		for (std::vector<Mesh::Volume*> ::const_iterator iter = volumes.begin ( );
				volumes.end() != iter; iter++)
		{
			vector<gmds::Region>	regions;
			(*iter)->getGMDSRegions (regions);
			gmds::IGMesh::volume&	v	= mesh.newVolume ((*iter)->getName());
			_gmdsVolumes.push_back ((*iter)->getName());
			for (vector <gmds::Region>::const_iterator itr = regions.begin ( );
					regions.end ( ) != itr; itr++)
				v.add (*itr);
			_analysedMeshEntities.push_back (*iter);
			Mgx3DQualifSerie*	serie	= new Mgx3DQualifSerie(v, (*iter)->getName(), "", *iter);
			getQualityWidget ( ).addSerie (serie);
		}	// for (vector<string>::const_iterator iter = ...

	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau Qualité de "
			  << "maillage : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau Qualité de "
			  << "maillage : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant l'actualisation du "
			  << "panneau Qualité de maillage.";
		log (ErrorLog (error));
	}
}	// QtMeshQualityOperationPanel::autoUpdate


QtMgx3DQualifWidget& QtMeshQualityOperationPanel::getQualityWidget ( )
{
	CHECK_NULL_PTR_ERROR (_qualifWidget)
	return *_qualifWidget;
}	// QtMeshQualityOperationPanel::getQualityWidget


void QtMeshQualityOperationPanel::clear ( )
{
	Internal::Context*	context	=
					dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	Mesh::MeshManager*	manager	=
			dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
	CHECK_NULL_PTR_ERROR (manager)
	CHECK_NULL_PTR_ERROR (getContext ( ).getMeshManager ( ).getMesh ( ))
	gmds::IGMesh&	mesh	=
				getContext ( ).getMeshManager ( ).getMesh ( )->getGMDSMesh ( );

	// On masque les entités :
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	getMainWindow ( )->getEntitiesPanel ( ).removeMeshEntities (_meshEntities);

	for (vector<MeshEntity*>::iterator it = _meshEntities.begin ( );
	     _meshEntities.end ( ) != it; it++)
	{
		// On détruit ici les éléments de maillage Magix et GMDS car ce n'est
		// pas fait par ailleurs.
		unique_ptr<MeshEntity>	entity (*it);	// => Destruction forcée de *it.
		Mesh::Surface*	surface	= dynamic_cast<Mesh::Surface*>(*it);
		Mesh::Volume*	volume	= dynamic_cast<Mesh::Volume*>(*it);
		if (0 != surface)
		{
			try
			{
				mesh.deleteSurface (mesh.getSurface ((*it)->getName ( )));
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage "
				      << "du panneau Qualité de maillage.";
				log (ErrorLog (error));
			}
			try
			{
				manager->remove (surface);
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage "
				      << "du panneau Qualité de maillage.";
				log (ErrorLog (error));
			}
		}	// if (0 != surface)
		else if (0 != volume)
		{
			try
			{
				mesh.deleteVolume (mesh.getVolume ((*it)->getName ( )));
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage "
				      << "du panneau Qualité de maillage.";
				log (ErrorLog (error));
			}
			try
			{
				manager->remove (volume);
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau "
				      << "Qualité de maillage : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage "
				      << "du panneau Qualité de maillage.";
				log (ErrorLog (error));
			}
		}	// else if (0 != volume)
		else
		{
			UTF8String	message (Charset::UTF_8);
			message << "Type non supporté pour l'élément de maillage "
			        << (*it)->getName ( ) << " de type "
			        << Entity::objectTypeToObjectTypeName ((*it)->getType ( ))
			        << ".";
			INTERNAL_ERROR (exc, "QtMeshQualityOperationPanel::clear", message)
			throw exc;
		}
	}	// for (vector<MeshEntity*>::iterator it = _meshEntities.begin ( ); ...
	_analysedMeshEntities.clear ( );
	_meshEntities.clear ( );
}	// QtMeshQualityOperationPanel::clear


void QtMeshQualityOperationPanel::clearSeries ( )
{

	Internal::Context*	context	=
					dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	Mesh::MeshManager*	manager	=
			dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
	CHECK_NULL_PTR_ERROR (manager)
	CHECK_NULL_PTR_ERROR (getContext ( ).getMeshManager ( ).getMesh ( ))
	gmds::IGMesh&	gmdsMesh	=
				getContext ( ).getMeshManager ( ).getMesh ( )->getGMDSMesh ( );

	for (vector<string>::iterator its = _gmdsSurfaces.begin ( );
	     _gmdsSurfaces.end ( ) != its; its++)
	{
		try
		{
			gmdsMesh.deleteSurface (gmdsMesh.getSurface (*its));
		}
		catch (const Exception& exc)
		{
			UTF8String	error (Charset::UTF_8);
			error << "Erreur survenue durant le nettoyage du panneau "
			      << "Qualité de maillage : " << exc.getFullMessage ( );
			log (ErrorLog (error));
		}
		catch (const exception& e)
		{
			UTF8String	error (Charset::UTF_8);
			error << "Erreur survenue durant le nettoyage du panneau "
			      << "Qualité de maillage : " << e.what ( );
			log (ErrorLog (error));
		}
		catch (...)
		{
			UTF8String	error (Charset::UTF_8);
			error << "Erreur non documentée survenue durant le nettoyage "
			      << "du panneau Qualité de maillage.";
			log (ErrorLog (error));
		}
	}	// for (vector<string>::iterator its = _gmdsSurfaces.begin ( ); ...
	_gmdsSurfaces.clear ( );

	for (vector<string>::iterator itv = _gmdsVolumes.begin ( );
	     _gmdsVolumes.end ( ) != itv; itv++)
	{
		try
		{
			gmdsMesh.deleteVolume (gmdsMesh.getVolume (*itv));
		}
		catch (const Exception& exc)
		{
			UTF8String	error (Charset::UTF_8);
			error << "Erreur survenue durant le nettoyage du panneau "
			      << "Qualité de maillage : " << exc.getFullMessage ( );
			log (ErrorLog (error));
		}
		catch (const exception& e)
		{
			UTF8String	error (Charset::UTF_8);
			error << "Erreur survenue durant le nettoyage du panneau "
			      << "Qualité de maillage : " << e.what ( );
			log (ErrorLog (error));
		}
		catch (...)
		{
			UTF8String	error (Charset::UTF_8);
			error << "Erreur non documentée survenue durant le nettoyage "
			      << "du panneau Qualité de maillage.";
			log (ErrorLog (error));
		}
	}	// for (vector<string>::iterator itv = _gmdsVolumes.begin ( ); ...
	_analysedMeshEntities.clear ( );
	_gmdsVolumes.clear ( );
}	// QtMeshQualityOperationPanel::clearSeries


void QtMeshQualityOperationPanel::displayCellsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	clear ( );

	Internal::Context*	context	=
					dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	Mesh::MeshManager*	manager	=
			dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
	CHECK_NULL_PTR_ERROR (manager)

	vector< gmds::IGMesh::surface* >	surfaces =
							getQualityWidget ( ).getSelectedClassesSurfaces ( );
	vector< gmds::IGMesh::volume* >	volumes =
							getQualityWidget ( ).getSelectedClassesVolumes ( );
	vector<Mesh::MeshEntity*>	entities;
	for (vector< gmds::IGMesh::surface*
			>::const_iterator its = surfaces.begin ( ); surfaces.end ( ) != its;
		its++)
	{
		// From CommandCreateMesh::addNewSurface :
		Mesh::SubSurface*	surface	=
			new Mesh::SubSurface (*context,
				context->newProperty (
								Entity::MeshSurface, (*its)->name ( )),
							context->newDisplayProperties(Entity::MeshSurface),
							0);
		gmds::IGMesh::surface&	gmdsSurface	= **its;
		for (size_t i = 0; i < (*its)->size ( ); i++)
		{
			gmds::Face	face	= gmdsSurface [i];
			surface->addFace (face);
		}	// for (size_t i = 0; i < (*its)->size ( ); i++)
		context->newGraphicalRepresentation (*surface);
		_meshEntities.push_back (surface);
		manager->add (surface);
		surface->getDisplayProperties ( ).setDisplayed (true);
		entities.push_back (surface);
	}
	for (vector< gmds::IGMesh::volume*
			>::const_iterator itv = volumes.begin ( ); volumes.end ( ) != itv;
			itv++)
	{
		Mesh::SubVolume*	volume	=
				new Mesh::SubVolume (*context,
						context->newProperty (
								Entity::MeshVolume, (*itv)->name ( )),
						context->newDisplayProperties(Entity::MeshVolume),
						0);
		gmds::IGMesh::volume&	gmdsVolume	= **itv;
		for (size_t i = 0; i < (*itv)->size ( ); i++)
		{
			gmds::Region	region	= gmdsVolume [i];
			volume->addRegion (region);
		}
		context->newGraphicalRepresentation (*volume);
		DisplayProperties::GraphicalRepresentation*	rep	=
				volume->getDisplayProperties ( ).getGraphicalRepresentation ( );
		CHECK_NULL_PTR_ERROR (rep)
		rep->setRepresentationMask (GraphicalEntityRepresentation::CURVES |
		                            GraphicalEntityRepresentation::VOLUMES);
		_meshEntities.push_back (volume);
		manager->add (volume);
		volume->getDisplayProperties ( ).setDisplayed (true);
		entities.push_back (volume);
	}
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	getMainWindow ( )->getEntitiesPanel ( ).addMeshEntities (entities, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage des mailles des classes sélectionnées du panneau qualité de maillage.")
}	// QtMeshQualityOperationPanel::displayCellsCallback


void QtMeshQualityOperationPanel::reinitializeCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	autoUpdate ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation du panneau de qualité de maillage")
}	// QtMeshQualityOperationPanel::reinitializeCallback


void QtMeshQualityOperationPanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau de qualité de maillage")
}	// QtMeshQualityOperationPanel::closeCallback


// ===========================================================================
//                  LA CLASSE QtMeshQualityOperationAction
// ===========================================================================

QtMeshQualityOperationAction::QtMeshQualityOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (
						icon, text, mainWindow, tooltip),
	  _observers ( ), _mutex (false)
{
	QtMeshQualityOperationPanel*	operationPanel	=
		new QtMeshQualityOperationPanel (
			0, text.toStdString ( ),
			mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).meshQualityOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).meshQualityOperationTag);
	setWidget (operationPanel);
}	// QtMeshQualityOperationAction::QtMeshQualityOperationAction


QtMeshQualityOperationAction::QtMeshQualityOperationAction (const QtMeshQualityOperationAction&)
	: QtMgx3DMeshOperationAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _observers ( ), _mutex (false)
{
	MGX_FORBIDDEN ("QtMeshQualityOperationAction copy constructor is not allowed.")
}	// QtMeshQualityOperationAction::QtMeshQualityOperationAction


QtMeshQualityOperationAction& QtMeshQualityOperationAction::operator = (const QtMeshQualityOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshQualityOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshQualityOperationAction::operator =


QtMeshQualityOperationAction::~QtMeshQualityOperationAction ( )
{
}	// QtMeshQualityOperationAction::~QtMeshQualityOperationAction


QtMeshQualityOperationPanel* QtMeshQualityOperationAction::getMeshQualityPanel ( )
{
	return dynamic_cast<QtMeshQualityOperationPanel*>(getOperationPanel ( ));
}	// QtMeshQualityOperationAction::getMeshQualityPanel


void QtMeshQualityOperationAction::executeOperation ( )
{
	if (false == _mutex.tryLock ( ))
		return;

	try
	{
	    // Validation paramétrage :
	    QtMgx3DMeshOperationAction::executeOperation ( );

// ICI : faire appel à Qualif

	    _mutex.unlock ( );

	}
	catch (...)
	{
		_mutex.unlock ( );
		throw;
		// unique_ptr => delete oldQuality
	}
}	// QtMeshQualityOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
