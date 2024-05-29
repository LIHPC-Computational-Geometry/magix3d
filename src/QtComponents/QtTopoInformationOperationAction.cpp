/**
 * \file        QtTopoInformationOperationAction.cpp
 * \author		Eric Brière de l'Isle
 * \date		25/5/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/InfoCommand.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopoInformationOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtHelpWindow.h>

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>
#include <QtUtil/QtUnicodeHelper.h>

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
//               LA CLASSE QtTopoInformationOperationPanel
// ===========================================================================

QtTopoInformationOperationPanel::QtTopoInformationOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (
			0/*parent*/, mainWindow, action, helpURL, helpTag),
			_nbBlocksLabel (0),
			_nb_dom_str_dirLabel (0),
			_nb_dom_str_rotLabel (0),
			_nb_dom_str_transLabel (0),
			_nb_dom_unstrLabel (0),
			_nb_regions_totLabel (0),
			_nb_regions_str_dirLabel (0),
			_nb_regions_str_rotLabel (0),
			_nb_regions_str_transLabel (0),
			_nb_regions_unstrLabel (0),
			_nb_cofacesLabel (0),
			_nb_fac_str_dirLabel (0),
			_nb_fac_str_rotLabel (0),
			_nb_fac_str_transLabel (0),
			_nb_fac_unstrLabel (0),
			_nb_faces_totLabel (0),
			_nb_faces_str_dirLabel (0),
			_nb_faces_str_rotLabel (0),
			_nb_faces_str_transLabel (0),
			_nb_faces_unstrLabel (0),
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
	_nbBlocksLabel	= new QLabel ("Nombre de blocs : ", this);
	vlayout->addWidget (_nbBlocksLabel);
	_nb_dom_str_dirLabel = new QLabel (QString::fromUtf8("Nombre de blocs structurés suivant direction : "), this);
	vlayout->addWidget (_nb_dom_str_dirLabel);
	_nb_dom_str_rotLabel = new QLabel (QString::fromUtf8("Nombre de blocs structurés suivant rotation : "), this);
	vlayout->addWidget (_nb_dom_str_rotLabel);
	_nb_dom_str_transLabel = new QLabel (QString::fromUtf8("Nombre de blocs structurés avec transfini : "), this);
	vlayout->addWidget (_nb_dom_str_transLabel);
	_nb_dom_unstrLabel = new QLabel (QString::fromUtf8("Nombre de blocs non-structurés : "), this);
	vlayout->addWidget (_nb_dom_unstrLabel);
	vlayout->addSpacing(20);

	_nb_regions_totLabel = new QLabel (QString::fromUtf8("Nombre de polyèdres au total : "), this);
	vlayout->addWidget (_nb_regions_totLabel);
	_nb_regions_str_dirLabel = new QLabel (QString::fromUtf8("Nombre de polyèdres structurés suivant direction : "), this);
	vlayout->addWidget (_nb_regions_str_dirLabel);
	_nb_regions_str_rotLabel = new QLabel (QString::fromUtf8("Nombre de polyèdres structurés suivant rotation : "), this);
	vlayout->addWidget (_nb_regions_str_rotLabel);
	_nb_regions_str_transLabel = new QLabel (QString::fromUtf8("Nombre de polyèdres structurés avec transfini : "), this);
	vlayout->addWidget (_nb_regions_str_transLabel);
	_nb_regions_unstrLabel = new QLabel (QString::fromUtf8("Nombre de polyèdres non-structurés : "), this);
	vlayout->addWidget (_nb_regions_unstrLabel);
	vlayout->addSpacing(20);

	_nb_cofacesLabel = new QLabel ("Nombre de faces communes : ", this);
	vlayout->addWidget (_nb_cofacesLabel);
	_nb_fac_str_dirLabel= new QLabel (QString::fromUtf8("Nombre de faces structurées suivant direction : "), this);
	vlayout->addWidget (_nb_fac_str_dirLabel);
	_nb_fac_str_rotLabel = new QLabel (QString::fromUtf8("Nombre de faces structurées suivant rotation : "), this);
	vlayout->addWidget (_nb_fac_str_rotLabel);
	_nb_fac_str_transLabel = new QLabel (QString::fromUtf8("Nombre de faces structurées avec transfini : "), this);
	vlayout->addWidget (_nb_fac_str_transLabel);
	_nb_fac_unstrLabel = new QLabel (QString::fromUtf8("Nombre de faces non-structurées : "), this);
	vlayout->addWidget (_nb_fac_unstrLabel);
	vlayout->addSpacing(20);

	_nb_faces_totLabel = new QLabel ("Nombre de polygones au total : ", this);
	vlayout->addWidget (_nb_faces_totLabel);
	_nb_faces_str_dirLabel = new QLabel (QString::fromUtf8("Nombre de polygones structurés suivant direction : "), this);
	vlayout->addWidget (_nb_faces_str_dirLabel);
	_nb_faces_str_rotLabel = new QLabel (QString::fromUtf8("Nombre de polygones structurés suivant rotation : "), this);
	vlayout->addWidget (_nb_faces_str_rotLabel);
	_nb_faces_str_transLabel = new QLabel (QString::fromUtf8("Nombre de polygones structurés avec transfini : "), this);
	vlayout->addWidget (_nb_faces_str_transLabel);
	_nb_faces_unstrLabel = new QLabel (QString::fromUtf8("Nombre de polygones non-structurés : "), this);
	vlayout->addWidget (_nb_faces_unstrLabel);
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
}	// QtTopoInformationOperationPanel::QtTopoInformationOperationPanel


QtTopoInformationOperationPanel::QtTopoInformationOperationPanel (
								const QtTopoInformationOperationPanel& dmop)
	: QtMgx3DOperationPanel (
				0,
				*new QtMgx3DMainWindow (0),
				0, "", ""),
				_nbBlocksLabel (0),
				_nb_dom_str_dirLabel (0),
				_nb_dom_str_rotLabel (0),
				_nb_dom_str_transLabel (0),
				_nb_dom_unstrLabel (0),
				_nb_regions_totLabel (0),
				_nb_regions_str_dirLabel (0),
				_nb_regions_str_rotLabel (0),
				_nb_regions_str_transLabel (0),
				_nb_regions_unstrLabel (0),
				_nb_cofacesLabel (0),
				_nb_fac_str_dirLabel (0),
				_nb_fac_str_rotLabel (0),
				_nb_fac_str_transLabel (0),
				_nb_fac_unstrLabel (0),
				_nb_faces_totLabel (0),
				_nb_faces_str_dirLabel (0),
				_nb_faces_str_rotLabel (0),
				_nb_faces_str_transLabel (0),
				_nb_faces_unstrLabel (0),
				_helpURL(), _helpTag()
{
	MGX_FORBIDDEN ("QtTopoInformationOperationPanel copy constructor is not allowed.");
}	// QtTopoInformationOperationPanel::QtTopoInformationOperationPanel (const QtTopoInformationOperationPanel&)


QtTopoInformationOperationPanel&
					QtTopoInformationOperationPanel::operator = (
									const QtTopoInformationOperationPanel&)
{
	MGX_FORBIDDEN ("QtTopoInformationOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopoInformationOperationPanel::QtTopoInformationOperationPanel (const QtTopoInformationOperationPanel&)


QtTopoInformationOperationPanel::~QtTopoInformationOperationPanel ( )
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
		error << "Erreur survenue durant la fermeture du panneau information topologie"
		      << " : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau information topologie"
		      << " : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant la fermeture du "
		      << "panneau information topologie.";
		log (ErrorLog (error));
	}
}	// QtTopoInformationOperationPanel::~QtTopoInformationOperationPanel


void QtTopoInformationOperationPanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtTopoInformationOperationPanel::void setVisible


void QtTopoInformationOperationPanel::commandModifiedCallback (
												InfoCommand& infoCommand)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	autoUpdate();

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopoInformationOperationPanel::commandModifiedCallback


void QtTopoInformationOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_nbBlocksLabel)
	CHECK_NULL_PTR_ERROR (_nb_dom_str_dirLabel)
	CHECK_NULL_PTR_ERROR (_nb_dom_str_rotLabel)
	CHECK_NULL_PTR_ERROR (_nb_dom_str_transLabel)
	CHECK_NULL_PTR_ERROR (_nb_dom_unstrLabel)
	CHECK_NULL_PTR_ERROR (_nb_regions_totLabel)
	CHECK_NULL_PTR_ERROR (_nb_regions_str_dirLabel)
	CHECK_NULL_PTR_ERROR (_nb_regions_str_rotLabel)
	CHECK_NULL_PTR_ERROR (_nb_regions_str_transLabel)
	CHECK_NULL_PTR_ERROR (_nb_regions_unstrLabel)
	CHECK_NULL_PTR_ERROR (_nb_cofacesLabel)
	CHECK_NULL_PTR_ERROR (_nb_fac_str_dirLabel)
	CHECK_NULL_PTR_ERROR (_nb_fac_str_rotLabel)
	CHECK_NULL_PTR_ERROR (_nb_fac_str_transLabel)
	CHECK_NULL_PTR_ERROR (_nb_fac_unstrLabel)
	CHECK_NULL_PTR_ERROR (_nb_faces_totLabel)
	CHECK_NULL_PTR_ERROR (_nb_faces_str_dirLabel)
	CHECK_NULL_PTR_ERROR (_nb_faces_str_rotLabel)
	CHECK_NULL_PTR_ERROR (_nb_faces_str_transLabel)
	CHECK_NULL_PTR_ERROR (_nb_faces_unstrLabel)

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK
		_nbBlocksLabel->clear();
		_nb_dom_str_dirLabel->clear();
		_nb_dom_str_rotLabel->clear();
		_nb_dom_str_transLabel->clear();
		_nb_dom_unstrLabel->clear();
		_nb_regions_totLabel->clear();
		_nb_regions_str_dirLabel->clear();
		_nb_regions_str_rotLabel->clear();
		_nb_regions_str_transLabel->clear();
		_nb_regions_unstrLabel->clear();
		_nb_cofacesLabel->clear();
		_nb_fac_str_dirLabel->clear();
		_nb_fac_str_rotLabel->clear();
		_nb_fac_str_transLabel->clear();
		_nb_fac_unstrLabel->clear();
		_nb_faces_totLabel->clear();
		_nb_faces_str_dirLabel->clear();
		_nb_faces_str_rotLabel->clear();
		_nb_faces_str_transLabel->clear();
		_nb_faces_unstrLabel->clear();

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopoInformationOperationPanel::cancel


void QtTopoInformationOperationPanel::autoUpdate ( )
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
		std::vector<Topo::CoFace*> cofaces;
		manager->getBlocks(blocks);
		manager->getCoFaces(cofaces);

		uint nb_blocs = blocks.size();
	    uint nb_dom_str_dir = 0;
	    uint nb_dom_str_rot = 0;
	    uint nb_dom_str_trans = 0;
	    uint nb_dom_unstr = 0;

	    uint nb_regions_tot = 0;
	    uint nb_regions_str_dir = 0;
	    uint nb_regions_str_rot = 0;
	    uint nb_regions_str_trans = 0;
	    uint nb_regions_unstr = 0;

	    uint nb_cofaces = 0;
	    uint nb_fac_str_dir = 0;
	    uint nb_fac_str_rot = 0;
	    uint nb_fac_str_trans = 0;
	    uint nb_fac_unstr = 0;

	    uint nb_faces_tot = 0;
	    uint nb_faces_str_dir = 0;
	    uint nb_faces_str_rot = 0;
	    uint nb_faces_str_trans = 0;
	    uint nb_faces_unstr = 0;

	    for (std::vector<Topo::Block* >::iterator iter = blocks.begin();
	    		iter != blocks.end(); ++iter){
	    	Topo::Block* bloc = *iter;
	    	size_t nb = bloc->getMeshingData()->regions().size();
	    	nb_regions_tot += nb;
	    	if (bloc->getMeshLaw() == Topo::BlockMeshingProperty::directional){
	    		nb_dom_str_dir += 1;
	    		nb_regions_str_dir += nb;
	    	}
	    	else if (bloc->getMeshLaw() == Topo::BlockMeshingProperty::rotational){
	    		nb_dom_str_rot += 1;
	    		nb_regions_str_rot += nb;
	    	}
	    	else if (bloc->getMeshLaw() == Topo::BlockMeshingProperty::transfinite){
	    		nb_dom_str_trans  += 1;
	    		nb_regions_str_trans += nb;
	    	}
	    	else if (bloc->getMeshLaw() == Topo::BlockMeshingProperty::delaunayTetgen
#ifdef USE_MESHGEMS
					|| bloc->getMeshLaw() == Topo::BlockMeshingProperty::delaunayMeshGemsVol
#endif
					){
	    		nb_dom_unstr += 1;
	    		nb_regions_unstr += nb;
	    	}
	    }

	    for (std::vector<Topo::CoFace*>::iterator iter = cofaces.begin();
	    		iter != cofaces.end(); ++iter){
	        Topo::CoFace* coface = *iter;
	        nb_cofaces +=1;
	        size_t nb = coface->getMeshingData()->faces().size();
	        nb_faces_tot += nb;
	        if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::directional){
	            nb_fac_str_dir += 1;
	            nb_faces_str_dir += nb;
	        }
	        else if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::rotational){
	            nb_fac_str_rot += 1;
	            nb_faces_str_rot += nb;
	        }
	        else if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::transfinite){
	            nb_fac_str_trans  += 1;
	            nb_faces_str_trans += nb;
	        }
	        else if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::delaunayGMSH
#ifdef USE_MESHGEMS
					|| coface->getMeshLaw() == Topo::CoFaceMeshingProperty::MeshGems
#endif	// USE_MESHGEMS
					){
	            nb_fac_unstr += 1;
	            nb_faces_unstr += nb;
	        }

	    }

		{
			UTF8String	text ("Nombre de blocs : ", Charset::UTF_8);
			text << (unsigned long)nb_blocs;
			_nbBlocksLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de blocs structurés suivant direction : ", Charset::UTF_8);
			text << (unsigned long)nb_dom_str_dir;
			_nb_dom_str_dirLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de blocs structurés suivant rotation : ", Charset::UTF_8);
			text << (unsigned long)nb_dom_str_rot;
			_nb_dom_str_rotLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de blocs structurés avec transfini : ", Charset::UTF_8);
			text << (unsigned long)nb_dom_str_trans;
			_nb_dom_str_transLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de blocs non-structurés : ", Charset::UTF_8);
			text << (unsigned long)nb_dom_unstr;
			_nb_dom_unstrLabel->setText (UTF8TOQSTRING (text));
		}

		{
			UTF8String	text ("Nombre de polyèdres au total : ", Charset::UTF_8);
			text << (unsigned long)nb_regions_tot;
			_nb_regions_totLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polyèdres structurés suivant direction : ", Charset::UTF_8);
			text << (unsigned long)nb_regions_str_dir;
			_nb_regions_str_dirLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polyèdres structurés suivant rotation : ", Charset::UTF_8);
			text << (unsigned long)nb_regions_str_rot;
			_nb_regions_str_rotLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polyèdres structurés avec transfini : ", Charset::UTF_8);
			text << (unsigned long)nb_regions_str_trans;
			_nb_regions_str_transLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polyèdres non-structurés : ", Charset::UTF_8);
			text << (unsigned long)nb_regions_unstr;
			_nb_regions_unstrLabel->setText (UTF8TOQSTRING (text));
		}

		{
			UTF8String	text ("Nombre de faces communes : ", Charset::UTF_8);
			text << (unsigned long)nb_cofaces;
			_nb_cofacesLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de faces structurées suivant direction : ", Charset::UTF_8);
			text << (unsigned long)nb_fac_str_dir;
			_nb_fac_str_dirLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de faces structurées suivant rotation : ", Charset::UTF_8);
			text << (unsigned long)nb_fac_str_rot;
			_nb_fac_str_rotLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de faces structurées avec transfini : ", Charset::UTF_8);
			text << (unsigned long)nb_fac_str_trans;
			_nb_fac_str_transLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de faces non-structurées : ", Charset::UTF_8);
			text << (unsigned long)nb_fac_unstr;
			_nb_fac_unstrLabel->setText (UTF8TOQSTRING (text));
		}

		{
			UTF8String	text ("Nombre de polygones au total : ", Charset::UTF_8);
			text << (unsigned long)nb_faces_tot;
			_nb_faces_totLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polygones structurés suivant direction : ", Charset::UTF_8);
			text << (unsigned long)nb_faces_str_dir;
			_nb_faces_str_dirLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polygones structurés suivant rotation : ", Charset::UTF_8);
			text << (unsigned long)nb_faces_str_rot;
			_nb_faces_str_rotLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polygones structurés avec transfini : ", Charset::UTF_8);
			text << (unsigned long)nb_faces_str_trans;
			_nb_faces_str_transLabel->setText (UTF8TOQSTRING (text));
		}
		{
			UTF8String	text ("Nombre de polygones non-structurés : ", Charset::UTF_8);
			text << (unsigned long)nb_faces_unstr;
			_nb_faces_unstrLabel->setText (UTF8TOQSTRING (text));
		}


		if (nb_blocs){
			_nbBlocksLabel->show();
			_nb_dom_str_dirLabel->show();
			_nb_dom_str_rotLabel->show();
			_nb_dom_str_transLabel->show();
			_nb_dom_unstrLabel->show();
			_nb_regions_totLabel->show();
			_nb_regions_str_dirLabel->show();
			_nb_regions_str_rotLabel->show();
			_nb_regions_str_transLabel->show();
			_nb_regions_unstrLabel->show();
		}
		else {
			_nbBlocksLabel->hide();
			_nb_dom_str_dirLabel->hide();
			_nb_dom_str_rotLabel->hide();
			_nb_dom_str_transLabel->hide();
			_nb_dom_unstrLabel->hide();
			_nb_regions_totLabel->hide();
			_nb_regions_str_dirLabel->hide();
			_nb_regions_str_rotLabel->hide();
			_nb_regions_str_transLabel->hide();
			_nb_regions_unstrLabel->hide();
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
}	// QtTopoInformationOperationPanel::autoUpdate


void QtTopoInformationOperationPanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau d'information sur le maillage")
}	// QtTopoInformationOperationPanel::closeCallback


void QtTopoInformationOperationPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (_helpURL, _helpTag );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide.")

} // QtTopoInformationOperationPanel::helpCallback


// ===========================================================================
//                  LA CLASSE QtTopoInformationOperationAction
// ===========================================================================

QtTopoInformationOperationAction::QtTopoInformationOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (
						icon, text, mainWindow, tooltip),
	  _mutex (false)
{
	QtTopoInformationOperationPanel*	operationPanel	=
		new QtTopoInformationOperationPanel (
			0, text.toStdString ( ),
			mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).topoInformationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).topoInformationTag);
	setWidget (operationPanel);
}	// QtTopoInformationOperationAction::QtTopoInformationOperationAction


QtTopoInformationOperationAction::QtTopoInformationOperationAction (const QtTopoInformationOperationAction&)
	: QtMgx3DGeomOperationAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _mutex (false)
{
	MGX_FORBIDDEN ("QtTopoInformationOperationAction copy constructor is not allowed.")
}	// QtTopoInformationOperationAction::QtTopoInformationOperationAction


QtTopoInformationOperationAction& QtTopoInformationOperationAction::operator = (const QtTopoInformationOperationAction&)
{
	MGX_FORBIDDEN ("QtTopoInformationOperationAction assignment operator is not allowed.")
	return *this;
}	// QtTopoInformationOperationAction::operator =


QtTopoInformationOperationAction::~QtTopoInformationOperationAction ( )
{
}	// QtTopoInformationOperationAction::~QtTopoInformationOperationAction


QtTopoInformationOperationPanel* QtTopoInformationOperationAction::getTopoInformationPanel ( )
{
	return dynamic_cast<QtTopoInformationOperationPanel*>(getOperationPanel ( ));
}	// QtTopoInformationOperationAction::getTopoInformationPanel



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
