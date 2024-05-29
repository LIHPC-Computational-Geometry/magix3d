/**
 * \file        QtMeshInformationOperationAction.cpp
 * \author		Eric Brière de l'Isle
 * \date		4/3/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/InfoCommand.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMeshInformationOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtHelpWindow.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>

#include <QPushButton>
#include <QVBoxLayout>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//               LA CLASSE QtMeshInformationOperationPanel
// ===========================================================================

QtMeshInformationOperationPanel::QtMeshInformationOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (
			0/*parent*/, mainWindow, action, helpURL, helpTag),
			_nbBlocksLabel (0), _nbMeshedBlocksLabel (0),
			_nbCreatedRegionLabel (0),_nbProvideRegionLabel (0),
			_nbFacesLabel (0), _nbMeshedFacesLabel (0),
			_nbCreatedFaceLabel (0), _nbProvideFaceLabel (0),
			_helpURL(helpURL), _helpTag(helpTag)
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QVBoxLayout*	vlayout	= new QVBoxLayout (0);
	vlayout->setSpacing (QtConfiguration::spacing);
	vlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);
	setLayout (vlayout);

	// Le résultat :
	_nbMeshedBlocksLabel= new QLabel (QString::fromUtf8("Nombre de blocs maillés : "), this);
	vlayout->addWidget (_nbMeshedBlocksLabel);
	_nbBlocksLabel	= new QLabel ("Nombre de blocs : ", this);
	vlayout->addWidget (_nbBlocksLabel);
	_nbCreatedRegionLabel= new QLabel (QString::fromUtf8("Nombre de polyèdres créés : "), this);
	vlayout->addWidget (_nbCreatedRegionLabel);
	_nbProvideRegionLabel= new QLabel (QString::fromUtf8("Nombre de polyèdres prévus : "), this);
	vlayout->addWidget (_nbProvideRegionLabel);
	vlayout->addSpacing(20);

	_nbMeshedFacesLabel= new QLabel (QString::fromUtf8("Nombre de faces maillées : "), this);
	vlayout->addWidget (_nbMeshedFacesLabel);
	_nbFacesLabel	= new QLabel ("Nombre de faces : ", this);
	vlayout->addWidget (_nbFacesLabel);
	_nbCreatedFaceLabel= new QLabel (QString::fromUtf8("Nombre de polygones créés : "), this);
	vlayout->addWidget (_nbCreatedFaceLabel);
	_nbProvideFaceLabel= new QLabel (QString::fromUtf8("Nombre de polygones prévus : "), this);
	vlayout->addWidget (_nbProvideFaceLabel);
	vlayout->addSpacing(20);

	vlayout->addSpacing(20);

	// Les boutons :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	vlayout->addLayout (hlayout);
	QPushButton*	closeButton	 = new QPushButton("Fermer", this);
	hlayout->addWidget (closeButton);
	connect (closeButton, SIGNAL(clicked (bool)), this, SLOT(closeCallback( )));
	QPushButton*	helpButton	 = new QPushButton("Aide ...", this);
	hlayout->addWidget (helpButton, 0);
	connect (helpButton, SIGNAL (clicked (bool)), this, SLOT (helpCallback( )));


	hlayout->addStretch (2.);

	vlayout->addStretch (2.);

	autoUpdate ( );
}	// QtMeshInformationOperationPanel::QtMeshInformationOperationPanel


QtMeshInformationOperationPanel::QtMeshInformationOperationPanel (
								const QtMeshInformationOperationPanel& dmop)
	: QtMgx3DOperationPanel (
				0,
				*new QtMgx3DMainWindow (0),
				0, "", ""),
				_nbBlocksLabel (0),_nbMeshedBlocksLabel (0),
				_nbCreatedRegionLabel (0),_nbProvideRegionLabel (0),
				_nbFacesLabel (0), _nbMeshedFacesLabel (0),
				_nbCreatedFaceLabel (0), _nbProvideFaceLabel (0),
				_helpURL(), _helpTag()
{
	MGX_FORBIDDEN ("QtMeshInformationOperationPanel copy constructor is not allowed.");
}	// QtMeshInformationOperationPanel::QtMeshInformationOperationPanel (const QtMeshInformationOperationPanel&)


QtMeshInformationOperationPanel&
					QtMeshInformationOperationPanel::operator = (
									const QtMeshInformationOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshInformationOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshInformationOperationPanel::QtMeshInformationOperationPanel (const QtMeshInformationOperationPanel&)


QtMeshInformationOperationPanel::~QtMeshInformationOperationPanel ( )
{
	try
	{
		if (0 != getMainWindow ( ))
			getMainWindow ( )->unregisterAdditionalOperationPanel (*this);

		if (0 != getMgx3DOperationAction ( ))
		{
			// Ugly code :
			delete getMgx3DOperationAction ( );
		}
	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau information maillage"
		      << " : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau information maillage"
		      << " : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant la fermeture du "
		      << "panneau information maillage.";
		log (ErrorLog (error));
	}
}	// QtMeshInformationOperationPanel::~QtMeshInformationOperationPanel


void QtMeshInformationOperationPanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtMeshInformationOperationPanel::void setVisible


void QtMeshInformationOperationPanel::commandModifiedCallback (
												InfoCommand& infoCommand)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	autoUpdate();

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtMeshInformationOperationPanel::commandModifiedCallback


void QtMeshInformationOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_nbBlocksLabel)
	CHECK_NULL_PTR_ERROR (_nbMeshedBlocksLabel)
	CHECK_NULL_PTR_ERROR (_nbCreatedRegionLabel)
	CHECK_NULL_PTR_ERROR (_nbProvideRegionLabel)
	CHECK_NULL_PTR_ERROR (_nbFacesLabel)
	CHECK_NULL_PTR_ERROR (_nbMeshedFacesLabel)
	CHECK_NULL_PTR_ERROR (_nbCreatedFaceLabel)
	CHECK_NULL_PTR_ERROR (_nbProvideFaceLabel)

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK
		_nbBlocksLabel->clear();
		_nbMeshedBlocksLabel->clear();
		_nbCreatedRegionLabel->clear();
		_nbProvideRegionLabel->clear();
		_nbFacesLabel->clear();
		_nbMeshedFacesLabel->clear();
		_nbCreatedFaceLabel->clear();
		_nbProvideFaceLabel->clear();

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtMeshInformationOperationPanel::cancel


void QtMeshInformationOperationPanel::autoUpdate ( )
{
	try
	{

		Internal::Context*	context	=
				dynamic_cast<Internal::Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)
		Topo::TopoManager*	manager	=
				dynamic_cast<Topo::TopoManager*>(&getContext ( ).getTopoManager( ));
		CHECK_NULL_PTR_ERROR (manager)

		QtMgx3DOperationPanel::autoUpdate ( );

		std::vector<Topo::Block*> blocks;
		manager->getBlocks(blocks);

		uint nb_blocs = blocks.size();
		uint nb_bl_mailles = 0;
		uint nb_regions_unstructured = 0;
		uint nb_created_regions = 0;
		uint nb_provide_regions = 0;
		for (uint i=0; i<nb_blocs; i++){
			Topo::Block* bl = blocks[i];
			if (bl->isMeshed())
				nb_bl_mailles ++;
			if (not bl->isStructured())
				nb_regions_unstructured ++;
			if (bl->isMeshed() || bl->isStructured())
				nb_provide_regions += bl->getNbRegions();
		}
		nb_created_regions = getContext().getMeshManager().getNbRegions();

		{
			UTF8String	text ("Nombre de blocs : ", Charset::UTF_8);
			text << (unsigned long)nb_blocs;

			if (nb_regions_unstructured)
				text << " dont "<<(unsigned long)nb_regions_unstructured<<" de non-structurés";

			_nbBlocksLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de blocs maillés : ", Charset::UTF_8);
			text << (unsigned long)nb_bl_mailles;
			_nbMeshedBlocksLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polyèdres créés : ", Charset::UTF_8);
			text << (unsigned long)nb_created_regions;
			_nbCreatedRegionLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polyèdres prévus : ", Charset::UTF_8);
			text << (unsigned long)nb_provide_regions;
			_nbProvideRegionLabel->setText (UTF8TOQSTRING (text));
		}

		std::vector<Topo::CoFace*> cofaces;
		manager->getCoFaces(cofaces);

		uint nb_cofaces = cofaces.size();
		uint nb_cf_mailles = 0;
		uint nb_faces_unstructured = 0;
		uint nb_created_faces = 0;
		uint nb_provide_faces = 0;
		for (uint i=0; i<nb_cofaces; i++){
			Topo::CoFace* cf = cofaces[i];
			if (cf->isMeshed())
				nb_cf_mailles ++;
			if (not cf->isStructured())
				nb_faces_unstructured ++;
			if (cf->isMeshed() || cf->isStructured())
				nb_provide_faces += cf->getNbMeshingFaces();
		}
		nb_created_faces = getContext().getMeshManager().getNbFaces();

		{
			UTF8String	text ("Nombre de faces : ", Charset::UTF_8);
			text << (unsigned long)nb_cofaces;

			if (nb_faces_unstructured)
				text << " dont "<<(unsigned long)nb_faces_unstructured<<" de non-structurées";

			_nbFacesLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de faces maillées : ", Charset::UTF_8);
			text << (unsigned long)nb_cf_mailles;
			_nbMeshedFacesLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polygones créés : ", Charset::UTF_8);
			text << (unsigned long)nb_created_faces;
			_nbCreatedFaceLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polygones prévus : ", Charset::UTF_8);
			text << (unsigned long)nb_provide_faces;
			_nbProvideFaceLabel->setText (UTF8TOQSTRING (text));
		}


	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau information maillage"
		      << " : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau information maillage"
		      << " : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant l'actualisation du "
		      << "panneau information maillage.";
		log (ErrorLog (error));
	}
}	// QtMeshInformationOperationPanel::autoUpdate


void QtMeshInformationOperationPanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau d'information sur le maillage")
}	// QtMeshInformationOperationPanel::closeCallback


void QtMeshInformationOperationPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (_helpURL, _helpTag );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide.")

} // QtMeshInformationOperationPanel::helpCallback


// ===========================================================================
//                  LA CLASSE QtMeshInformationOperationAction
// ===========================================================================

QtMeshInformationOperationAction::QtMeshInformationOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (
						icon, text, mainWindow, tooltip),
	  _mutex (false)
{
	QtMeshInformationOperationPanel*	operationPanel	=
		new QtMeshInformationOperationPanel (
			0, text.toStdString ( ),
			mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).meshInformationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).meshInformationTag);
	setWidget (operationPanel);
}	// QtMeshInformationOperationAction::QtMeshInformationOperationAction


QtMeshInformationOperationAction::QtMeshInformationOperationAction (const QtMeshInformationOperationAction&)
	: QtMgx3DGeomOperationAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _mutex (false)
{
	MGX_FORBIDDEN ("QtMeshInformationOperationAction copy constructor is not allowed.")
}	// QtMeshInformationOperationAction::QtMeshInformationOperationAction


QtMeshInformationOperationAction& QtMeshInformationOperationAction::operator = (const QtMeshInformationOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshInformationOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshInformationOperationAction::operator =


QtMeshInformationOperationAction::~QtMeshInformationOperationAction ( )
{
}	// QtMeshInformationOperationAction::~QtMeshInformationOperationAction


QtMeshInformationOperationPanel* QtMeshInformationOperationAction::getMeshInformationPanel ( )
{
	return dynamic_cast<QtMeshInformationOperationPanel*>(getOperationPanel ( ));
}	// QtMeshInformationOperationAction::getMeshInformationPanel



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
