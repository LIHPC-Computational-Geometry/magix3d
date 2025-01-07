/**
 * \file        QtMeshQualityDividerOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        07/01/2025
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
#include "QtComponents/QtMeshQualityDividerOperationAction.h"
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

#include <gmds/ig/Mesh.h>

using namespace std;
using namespace TkUtil;
using namespace GQualif;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;

namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtMeshQualityDividerOperationPanel
// ===========================================================================

QtMeshQualityDividerOperationPanel::QtMeshQualityDividerOperationPanel (
			QWidget* parent, const string& panelName, QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (0/*parent*/, mainWindow, action, helpURL, helpTag),
	  _qualifWidget (0), _analysedMeshEntities ( ), _meshEntities ( )
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QVBoxLayout*	vlayout	= new QVBoxLayout (0);
	vlayout->setSpacing (QtConfiguration::spacing);
	vlayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	setLayout (vlayout);
	QScrollArea*	scrollArea	= new QScrollArea (this);
	vlayout->addWidget (scrollArea, 10);
	scrollArea->setFrameStyle (QFrame::NoFrame | QFrame::Plain);
	scrollArea->setWidgetResizable (true);
	const QSizePolicy::Policy	grow	= (QSizePolicy::Policy)QSizePolicy::GrowFlag;
	scrollArea->setSizePolicy (grow, grow);
	Internal::Context*	context	= dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	_qualifWidget	= new QtMgx3DQualityDividerWidget (this, context, &mainWindow);
	scrollArea->setWidget (_qualifWidget);


	// Les boutons : ATTENTION, dans le QtQualifWidget.
	QPushButton*	closeButton	 = new QPushButton("Fermer", _qualifWidget);
	_qualifWidget->getButtonsLayout ( ).addWidget (closeButton);
	connect (closeButton, SIGNAL(clicked (bool)), this, SLOT(closeCallback( )));
	QPushButton*	helpButton	 = new QPushButton("Aide ...", _qualifWidget);
	_qualifWidget->getButtonsLayout ( ).addWidget (helpButton);
	connect (helpButton, SIGNAL (clicked (bool)), this, SLOT (helpCallback( )));

	autoUpdate ( );
}	// QtMeshQualityDividerOperationPanel::QtMeshQualityDividerOperationPanel


QtMeshQualityDividerOperationPanel::QtMeshQualityDividerOperationPanel (const QtMeshQualityDividerOperationPanel& cao)
	: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow (0),	0, "", ""),
	  _qualifWidget (0), _analysedMeshEntities ( ), _meshEntities ( )
{
	MGX_FORBIDDEN ("QtMeshQualityDividerOperationPanel copy constructor is not allowed.");
}	// QtMeshQualityDividerOperationPanel::QtMeshQualityDividerOperationPanel (const QtMeshQualityDividerOperationPanel&)


QtMeshQualityDividerOperationPanel& QtMeshQualityDividerOperationPanel::operator = (const QtMeshQualityDividerOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshQualityDividerOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshQualityDividerOperationPanel::QtMeshQualityDividerOperationPanel (const QtMeshQualityDividerOperationPanel&)


QtMeshQualityDividerOperationPanel::~QtMeshQualityDividerOperationPanel ( )
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
		error << "Erreur survenue durant la fermeture du panneau Qualité de maillage : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau Qualité de maillage : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant la fermeture du panneau Qualité de maillage.";
		log (ErrorLog (error));
	}
}	// QtMeshQualityDividerOperationPanel::~QtMeshQualityDividerOperationPanel


void QtMeshQualityDividerOperationPanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtMeshQualityDividerOperationPanel::void setVisible


void QtMeshQualityDividerOperationPanel::commandModifiedCallback (InfoCommand& infoCommand)
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
		// Est-ce une entité soumise à analyse ? Si oui on réinitialise, sinon on ne fait rien :
		for (vector<Mgx3D::Mesh::MeshEntity*>::iterator itme = _analysedMeshEntities.begin ( ); _analysedMeshEntities.end ( ) != itme; itme++)
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
}	// QtMeshQualityDividerOperationPanel::commandModifiedCallback


void QtMeshQualityDividerOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	clear ( );
	clearSeries ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtMeshQualityDividerOperationPanel::reset


void QtMeshQualityDividerOperationPanel::validate ( )
{
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
		error << "QtMeshQualityDividerOperationPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshQualityDividerOperationPanel::validate


void QtMeshQualityDividerOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	clear ( );
	clearSeries ( );
}	// QtMeshQualityDividerOperationPanel::cancel


void QtMeshQualityDividerOperationPanel::autoUpdate ( )
{
	try
	{
		Internal::Context*	context	= dynamic_cast<Internal::Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)
		Mesh::MeshManager*	manager	= dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
		CHECK_NULL_PTR_ERROR (manager)
		CHECK_NULL_PTR_ERROR (getContext ( ).getMeshManager ( ).getMesh ( ))
		gmds::Mesh&	mesh	= getContext ( ).getMeshManager ( ).getMesh ( )->getGMDSMesh ( );

		clearSeries ( );

		QtMgx3DOperationPanel::autoUpdate ( );

		getQualityDividerWidget ( ).removeSeries (true);
		clear ( );

		// Récupération des éléments de maillage sélectionnés :
		vector<string>	selectedSurfacesNames	= getContext ( ).getSelectionManager ( ).getEntitiesNames (Entity::MeshSurface);
		vector<string>	selectedVolumesNames	= getContext ( ).getSelectionManager ( ).getEntitiesNames (Entity::MeshVolume);

		// les groupes de mailles qui seront évalués par Qualif
		std::vector<Mesh::Volume*> volumes;
		std::vector<Mesh::Surface*> surfaces;

		for (vector<string>::const_iterator its = selectedSurfacesNames.begin ( ); selectedSurfacesNames.end ( ) != its; its++)
		{
			Mesh::Surface*	surface	= getContext ( ).getMeshManager ( ).getSurface (*its, true);
			CHECK_NULL_PTR_ERROR (surface)
			surfaces.push_back(surface);
		}
		for (vector<string>::const_iterator itv = selectedVolumesNames.begin ( ); selectedVolumesNames.end ( ) != itv; itv++)
		{
			Mesh::Volume*	volume	= getContext ( ).getMeshManager ( ).getVolume (*itv, true);
			CHECK_NULL_PTR_ERROR (volume)
			volumes.push_back(volume);
		}

		// s'il n'y a rien de sélectionné, on prend tout [EB]
		if (volumes.empty())
			getContext ( ).getMeshManager().getVolumes(volumes);

		if (volumes.empty() && surfaces.empty())
			getContext ( ).getMeshManager().getSurfaces(surfaces);

		for (std::vector<Mesh::Surface*> ::const_iterator iter = surfaces.begin ( ); surfaces.end() != iter; iter++)
		{
			vector<gmds::Face>	faces;
			(*iter)->getGMDSFaces (faces);
			_analysedMeshEntities.push_back (*iter);
			getQualityDividerWidget ( ).addSerie (new Mgx3DSurfaceQualifSerie(faces, (*iter)->getName(), "", *iter));
		}	// for (vector<string>::const_iterator iter = ...

		for (std::vector<Mesh::Volume*> ::const_iterator iter = volumes.begin ( ); volumes.end() != iter; iter++)
		{
			vector<gmds::Region>	regions;
			(*iter)->getGMDSRegions (regions);
			_analysedMeshEntities.push_back (*iter);
			getQualityDividerWidget ( ).addSerie (new Mgx3DVolumeQualifSerie(regions, (*iter)->getName(), "", *iter));
		}	// for (vector<string>::const_iterator iter = ...

	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau Qualité de maillage : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau Qualité de maillage : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant l'actualisation du panneau Qualité de maillage.";
		log (ErrorLog (error));
	}
}	// QtMeshQualityDividerOperationPanel::autoUpdate


QtMgx3DQualityDividerWidget& QtMeshQualityDividerOperationPanel::getQualityDividerWidget ( )
{
	CHECK_NULL_PTR_ERROR (_qualifWidget)
	return *_qualifWidget;
}	// QtMeshQualityDividerOperationPanel::getQualityDividerWidget


void QtMeshQualityDividerOperationPanel::clear ( )
{
	Internal::Context*	context	= dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	Mesh::MeshManager*	manager	= dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
	CHECK_NULL_PTR_ERROR (manager)
	CHECK_NULL_PTR_ERROR (getContext ( ).getMeshManager ( ).getMesh ( ))
	gmds::Mesh&	mesh	= getContext ( ).getMeshManager ( ).getMesh ( )->getGMDSMesh ( );

	// On masque les entités :
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	getMainWindow ( )->getEntitiesPanel ( ).removeMeshEntities (_meshEntities);

	for (vector<MeshEntity*>::iterator it = _meshEntities.begin ( ); _meshEntities.end ( ) != it; it++)
	{
		// On détruit ici les éléments de maillage Magix et GMDS car ce n'est pas fait par ailleurs.
		unique_ptr<MeshEntity>	entity (*it);	// => Destruction forcée de *it.
		Mesh::Surface*	surface	= dynamic_cast<Mesh::Surface*>(*it);
		Mesh::Volume*	volume	= dynamic_cast<Mesh::Volume*>(*it);
		if (0 != surface)
		{
			try
			{
				mesh.deleteGroup<gmds::Face>(mesh.getGroup<gmds::Face>((*it)->getName ( )));
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage du panneau Recherche selon critère.";
				log (ErrorLog (error));
			}
			try
			{
				manager->remove (surface);
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage du panneau Recherche selon critère.";
				log (ErrorLog (error));
			}
		}	// if (0 != surface)
		else if (0 != volume)
		{
			try
			{
				mesh.deleteGroup<gmds::Region>(mesh.getGroup<gmds::Region>((*it)->getName()));
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage du panneau Recherche selon critère.";
				log (ErrorLog (error));
			}
			try
			{
				manager->remove (volume);
			}
			catch (const Exception& exc)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << exc.getFullMessage ( );
				log (ErrorLog (error));
			}
			catch (const exception& e)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur survenue durant le nettoyage du panneau Recherche selon critère : " << e.what ( );
				log (ErrorLog (error));
			}
			catch (...)
			{
				UTF8String	error (Charset::UTF_8);
				error << "Erreur non documentée survenue durant le nettoyage du panneau Recherche selon critère.";
				log (ErrorLog (error));
			}
		}	// else if (0 != volume)
		else
		{
			UTF8String	message (Charset::UTF_8);
			message << "Type non supporté pour l'élément de maillage " << (*it)->getName ( ) << " de type " << Entity::objectTypeToObjectTypeName ((*it)->getType ( )) << ".";
			INTERNAL_ERROR (exc, "QtMeshQualityDividerOperationPanel::clear", message)
			throw exc;
		}
	}	// for (vector<MeshEntity*>::iterator it = _meshEntities.begin ( ); ...
	_analysedMeshEntities.clear ( );
	_meshEntities.clear ( );
}	// QtMeshQualityDividerOperationPanel::clear


void QtMeshQualityDividerOperationPanel::clearSeries ( )
{

	Internal::Context*	context	= dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	Mesh::MeshManager*	manager	= dynamic_cast<Mesh::MeshManager*>(&getContext ( ).getMeshManager( ));
	CHECK_NULL_PTR_ERROR (manager)
	CHECK_NULL_PTR_ERROR (getContext ( ).getMeshManager ( ).getMesh ( ))
	gmds::Mesh&	gmdsMesh	= getContext ( ).getMeshManager ( ).getMesh ( )->getGMDSMesh ( );

	_analysedMeshEntities.clear ( );
}	// QtMeshQualityDividerOperationPanel::clearSeries


void QtMeshQualityDividerOperationPanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau de qualité de maillage")
}	// QtMeshQualityDividerOperationPanel::closeCallback


// ===========================================================================
//                  LA CLASSE QtMeshQualityDividerOperationAction
// ===========================================================================

QtMeshQualityDividerOperationAction::QtMeshQualityDividerOperationAction (const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip),
	  _observers ( ), _mutex (false)
{
	QtMeshQualityDividerOperationPanel*	operationPanel	=
		new QtMeshQualityDividerOperationPanel (0, text.toStdString ( ), mainWindow, this, 
							QtMgx3DApplication::HelpSystem::instance ( ).meshQualityOperationURL, QtMgx3DApplication::HelpSystem::instance ( ).meshQualityOperationTag);
	setWidget (operationPanel);
}	// QtMeshQualityDividerOperationAction::QtMeshQualityDividerOperationAction


QtMeshQualityDividerOperationAction::QtMeshQualityDividerOperationAction (const QtMeshQualityDividerOperationAction&)
	: QtMgx3DMeshOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _observers ( ), _mutex (false)
{
	MGX_FORBIDDEN ("QtMeshQualityDividerOperationAction copy constructor is not allowed.")
}	// QtMeshQualityDividerOperationAction::QtMeshQualityDividerOperationAction


QtMeshQualityDividerOperationAction& QtMeshQualityDividerOperationAction::operator = (const QtMeshQualityDividerOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshQualityDividerOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshQualityDividerOperationAction::operator =


QtMeshQualityDividerOperationAction::~QtMeshQualityDividerOperationAction ( )
{
}	// QtMeshQualityDividerOperationAction::~QtMeshQualityDividerOperationAction


QtMeshQualityDividerOperationPanel* QtMeshQualityDividerOperationAction::getMeshQualityPanel ( )
{
	return dynamic_cast<QtMeshQualityDividerOperationPanel*>(getOperationPanel ( ));
}	// QtMeshQualityDividerOperationAction::getMeshQualityPanel


void QtMeshQualityDividerOperationAction::executeOperation ( )
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
}	// QtMeshQualityDividerOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
