/**
 * \file        QtVtkMgx3DMainWindow.h
 * \author      Charles PIGNEROL
 * \date        12/10/2012
 */

#include <mgx_config.h>

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/QtVtkMgx3DMainWindow.h"
#include "QtVtkComponents/QtVtkMgx3DApplication.h"
#include "QtVtkComponents/QtVtkGraphicalWidget.h"
#include "QtVtkComponents/QtVtkRepresentationTypesDialog.h"
#include "QtVtkComponents/VTKConfiguration.h"
#include "QtVtkComponents/VTKEntityRepresentation.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/InformationLog.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <QtVtk/QtVtk.h>
#include <QtVtk/QtVtkGraphicWidget.h>
#include <QtVtk/QtVTKPrintHelper.h>
#include <VtkContrib/VtkContribInfos.h>
#ifdef USE_EXPERIMENTAL_ROOM
#include <ExperimentalRoom/QtExpRoomFileDialog.h>
#include <ExperimentalRoom/ExperimentalRoomDistribution.h>
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
#include <ExperimentalRoomTExtension/TRaysIO.h>
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
#include <ExperimentalRoom/VisRadCsvIO.h>
#include <ExperimentalRoom/ERDRaysIO.h>
#endif	// USE_EXPERIMENTAL_ROOM

#include <TkUtil/MemoryError.h>
#include <TkUtil/ReferencedMutex.h>

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

#include <vtkVersion.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;

#undef LOCK_INSTANCE
#define LOCK_INSTANCE AutoReferencedMutex autoReferencedMutex (TkUtil::ObjectBase::getMutex ( ));


namespace Mgx3D
{

namespace QtVtkComponents
{

QtVtkMgx3DMainWindow::QtVtkMgx3DMainWindow (QWidget* parent, Qt::WindowFlags flags)
	: QtMgx3DMainWindow (parent, 0, flags)
#ifdef USE_EXPERIMENTAL_ROOM
	, _experimentalRoomPanel (0)
#endif	// USE_EXPERIMENTAL_ROOM
{
}	// QtVtkMgx3DMainWindow::QtVtkMgx3DMainWindow


QtVtkMgx3DMainWindow::QtVtkMgx3DMainWindow (const QtVtkMgx3DMainWindow&)
	: QtMgx3DMainWindow (0, 0, 0)
#ifdef USE_EXPERIMENTAL_ROOM
	, _experimentalRoomPanel (0)
#endif	// USE_EXPERIMENTAL_ROOM
{
	MGX_FORBIDDEN ("QtVtkMgx3DMainWindow copy constructor is not allowed.");
}	// QtVtkMgx3DMainWindow::QtVtkMgx3DMainWindow


QtVtkMgx3DMainWindow& QtVtkMgx3DMainWindow::operator =(const QtVtkMgx3DMainWindow&)
{
	MGX_FORBIDDEN ("QtVtkMgx3DMainWindow assignment operator is not allowed.");
	return *this;
}	// QtVtkMgx3DMainWindow::QtVtkMgx3DMainWindow


QtVtkMgx3DMainWindow::~QtVtkMgx3DMainWindow ( )
{
	unregisterReferences ( );
}	// QtVtkMgx3DMainWindow::~QtVtkMgx3DMainWindow


void QtVtkMgx3DMainWindow::init (const string& name, ContextIfc* context, QtGroupsPanel* groupsPanel, QtEntitiesPanel* entitiesPanel)
{
	QtMgx3DMainWindow::init (name, context, groupsPanel, entitiesPanel);
}	// QtVtkMgx3DMainWindow::QtVtkMgx3DMainWindow



QtRepresentationTypesDialog* QtVtkMgx3DMainWindow::createRepresentationTypesDialog (
		const string& name, const std::vector<Mgx3D::Utils::Entity*>& entities,
		const std::string& helpURL, const std::string& helpTag)
{
	return new QtVtkRepresentationTypesDialog (this, name, getAppTitle ( ), entities, helpURL, helpTag);
}	// QtVtkMgx3DMainWindow::createRepresentationTypesDialog


void QtVtkMgx3DMainWindow::setGraphicalWidget (Qt3DGraphicalWidget& widget)
{
	QtMgx3DMainWindow::setGraphicalWidget (widget);

	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&widget);
	CHECK_NULL_PTR_ERROR (vgw)
	CHECK_NULL_PTR_ERROR (vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetMgx3DPicker ( ))
	CHECK_NULL_PTR_ERROR (getActions ( )._selectionModeAction)
	const bool	boundingBox	= false == getActions ( )._selectionModeAction->isChecked ( ) ? true : false;
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetMgx3DPicker ( )->SetMode (
					true == boundingBox ? VTKMgx3DPicker::BOUNDING_BOX : VTKMgx3DPicker::DISTANCE);
	bool	enabled	= 0 == getActions ( )._selectVisibleAction ? false : getActions ( )._selectVisibleAction->isChecked ( );
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetForegroundSelection (enabled);
	getActions ( )._selectVisibleAction->setEnabled (vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetRubberBand ( ));
}	// QtVtkMgx3DMainWindow::setGraphicalWidget


#ifdef USE_EXPERIMENTAL_ROOM
void QtVtkMgx3DMainWindow::createRaysPanel ( )
{
	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	vtkRenderer&	renderer	= vgw->getVTKRenderingManager ( ).getRenderer ( );
	_experimentalRoomPanel	= new QtVtkMgx3DExperimentalRoomPanel (
				this, UTF8String ("Chambre expérimentale", Charset::UTF_8),
				UTF8String ("Chambre expérimentale", Charset::UTF_8), getAppTitle ( ), renderer);
	setExperimentalRoomPanel (_experimentalRoomPanel);

	_experimentalRoomPanel->addContextualHelpItems(true,
			QtMgx3DApplication::HelpSystem::instance ( ).rayURL,
			QtMgx3DApplication::HelpSystem::instance ( ).rayTag,
			QtMgx3DApplication::HelpSystem::instance ( ).rayIOsTag,
			QtMgx3DApplication::HelpSystem::instance ( ).rayBoundingBoxTag,
			QtMgx3DApplication::HelpSystem::instance ( ).raysParametersTag,
			QtMgx3DApplication::HelpSystem::instance ( ).raysGraphicalParametersTag);

	// On étend le menu :
	if (0 != getRoomMenu ( ))
	{
		getRoomMenu ( )->insertAction (getActions ( )._loadRaysAction,
				_experimentalRoomPanel->getExpRoomViewAction ( ));
		getRoomMenu ( )->insertSeparator (getActions ( )._loadRaysAction);
		getRoomMenu ( )->insertAction (getActions ( )._loadRaysAction,
			_experimentalRoomPanel->getExpRoomToMeshTransformationAction ( ));
		getRoomMenu ( )->insertSeparator (getActions ( )._loadRaysAction);
		getRoomMenu ( )->insertAction (getActions ( )._loadRaysAction,
						_experimentalRoomPanel->getBoundingBoxAction ( ));
		getRoomMenu ( )->insertSeparator (getActions ( )._loadRaysAction);
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getRaysParametersAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
				_experimentalRoomPanel->getRaysGraphicalParametersAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getDisplayRaysAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getHideRaysAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getDisplaySpotsAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getHideSpotsAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getDisplayNamesAction ( ));
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getHideNamesAction ( ));
		getRoomMenu ( )->insertSeparator (getActions ( )._loadDiagsAction);
		getRoomMenu ( )->insertAction (getActions ( )._loadDiagsAction,
						_experimentalRoomPanel->getRemoveRayAction ( ));
		// Les diagnostics :
		getRoomMenu ( )->insertSeparator (getActions ( )._loadDiagsAction);
		getRoomMenu ( )->addSeparator ( );
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getDiagsParametersAction ( ));
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getDiagsGraphicalParametersAction ( ));
		getRoomMenu ( )->addSeparator ( );
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getDisplayDiagsAction ( ));
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getHideDiagsAction ( ));
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getDiagViewAction ( ));
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getDCodeInfosAction ( ));
		getRoomMenu ( )->addSeparator ( );
		getRoomMenu ( )->addAction (
				_experimentalRoomPanel->getRemoveDiagAction ( ));
		// L'aide :
		getRoomMenu ( )->addSeparator ( );
		getRoomMenu ( )->addAction (_experimentalRoomPanel->getGlobalHelpAction ( ));
	}	// if (0 != getRoomMenu ( ))

	// Le menu contextuel : les pointages lasers ...
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getRaysParametersAction ( ),
			getActions ( )._loadRaysAction);
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getRaysParametersAction ( ),
			getActions ( )._importRaysAction);
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getRaysParametersAction ( ),
			getActions ( )._saveRaysAction);
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getRaysParametersAction ( ),
			getActions ( )._saveAsRaysAction);
	// ... les diagnostics :
	_experimentalRoomPanel->getContextMenu ( ).insertSeparator (
			_experimentalRoomPanel->getDiagsParametersAction ( ));
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getDiagsParametersAction ( ),
			getActions ( )._loadDiagsAction);
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getDiagsParametersAction ( ),
			getActions ( )._importDiagsAction);
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getDiagsParametersAction ( ),
			getActions ( )._saveDiagsAction);
	_experimentalRoomPanel->getContextMenu ( ).insertAction (
			_experimentalRoomPanel->getDiagsParametersAction ( ),
			getActions ( )._saveAsDiagsAction);
	_experimentalRoomPanel->getContextMenu ( ).insertSeparator (
			_experimentalRoomPanel->getDiagsParametersAction ( ));
}	// QtVtkMgx3DMainWindow::createRaysPanel
#endif	// USE_EXPERIMENTAL_ROOM


void QtVtkMgx3DMainWindow::setEntitySeizureManager (EntitySeizureManager* seizureManager)
{
	try
	{
		QtVtkGraphicalWidget*		vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
		CHECK_NULL_PTR_ERROR (vgw)
		vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetEntitySeizureManager (seizureManager);
	}
	catch (...)
	{
	}

	QtMgx3DMainWindow::setEntitySeizureManager (seizureManager);
}	// QtVtkMgx3DMainWindow::setEntitySeizureManager


void QtVtkMgx3DMainWindow::unregisterEntitySeizureManager (EntitySeizureManager* seizureManager)
{
	LOCK_INSTANCE

	try
	{
		QtVtkGraphicalWidget*		vgw	=
				dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
		CHECK_NULL_PTR_ERROR (vgw)
		vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetEntitySeizureManager (0);
	}
	catch (...)
	{
	}

	QtMgx3DMainWindow::unregisterEntitySeizureManager (seizureManager);
}	// QtVtkMgx3DMainWindow::unregisterEntitySeizureManager


Charset QtVtkMgx3DMainWindow::getDefaultRaysCharset ( ) const
{
	try
	{
		const Charset::CHARSET	cs	= Charset::str2charset (QtVtkMgx3DApplication::instance ( )._raysFileCharset.getStrValue ( ).c_str ( ));

		return Charset::UNKNOWN == cs ? Charset::UTF_8 : Charset (cs);
	}
	catch (...)
	{
	}

	return Charset::UTF_8;
}	// QtVtkMgx3DMainWindow::getDefaultRaysCharset


vector <QtAboutDialog::Component> QtVtkMgx3DMainWindow::getAboutDlgComponents ( ) const
{
	vector <QtAboutDialog::Component>	components	= QtMgx3DMainWindow::getAboutDlgComponents ( );
	UTF8String				vtkInfos (Charset::UTF_8), qtvtkInfos (Charset::UTF_8);
	vtkInfos << vtkVersion::GetVTKVersion ( ) << " (backend " << VTK_OPENGL_BACKEND << ")";
	components.push_back (QtAboutDialog::Component ("VTK", vtkInfos, UTF8String ("Affichage graphiques 3D", Charset::UTF_8), ""));
	UTF8String qtvtkComment ("Utilitaires Qt/VTK", Charset::UTF_8);
	qtvtkInfos << QtVtk::getVersion ( ).getVersion ( ) << " (" << QtVtkGraphicWidget::implementationInfos ( ) << ")";
	components.push_back (QtAboutDialog::Component ("QtVtk", qtvtkInfos, qtvtkComment, ""));
#ifdef USE_EXPERIMENTAL_ROOM
	UTF8String expRoomComment ("Utilitaires pointages laser et diagnostics", Charset::UTF_8);
	components.push_back (QtAboutDialog::Component ("ExperimentalRoom", ExperimentalRoom::getVersion ( ).getVersion ( ), expRoomComment, ""));
#endif	// USE_EXPERIMENTAL_ROOM
	components.push_back (QtAboutDialog::Component ("IceVtkContrib",  VtkContribInfos::getVersion( ), UTF8String ("Surcouche VTK", Charset::UTF_8), ""));

	return components;
}	// QtVtkMgx3DMainWindow::getAboutDlgComponents


void QtVtkMgx3DMainWindow::closeEvent (QCloseEvent* event)
{
#ifdef USE_EXPERIMENTAL_ROOM
	if (0 != _experimentalRoomPanel)
		_experimentalRoomPanel->clearExperimentalRoom ( );
#endif	// USE_EXPERIMENTAL_ROOM

	QtMgx3DMainWindow::closeEvent (event);
}	// QtVtkMgx3DMainWindow::closeEvent


#ifdef USE_EXPERIMENTAL_ROOM
QtVtkMgx3DExperimentalRoomPanel* QtVtkMgx3DMainWindow::getExperimentalRoomPanel ( )
{
	return _experimentalRoomPanel;
}	// QtVtkMgx3DMainWindow::getExperimentalRoomPanel
#endif	// USE_EXPERIMENTAL_ROOM


void QtVtkMgx3DMainWindow::setLandmark (Utils::Landmark::kind lk)
{
	QtAutoWaitingCursor	cursor (true);

	BEGIN_QT_TRY_CATCH_BLOCK

	QtMgx3DMainWindow::setLandmark (lk);
#ifdef USE_EXPERIMENTAL_ROOM
	const ExperimentRay::LANDMARK	landmark	= Mgx3D::QtVtkComponents::landmarkToExpRoomLandmark (lk);
	if ((0 != getExperimentalRoomPanel ( )) && (getExperimentalRoomPanel ( )->getRenderingLandmark ( ) != landmark))
	{	
		getExperimentalRoomPanel ( )->setRenderingLandmark (landmark);
		getExperimentalRoomPanel ( )->updateRaysRepresentations (true, true);
	}	// if (0 != getExperimentalRoomPanel ( ))
#endif	// USE_EXPERIMENTAL_ROOM

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::setLandmark


void QtVtkMgx3DMainWindow::preferencesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtMgx3DMainWindow::preferencesCallback ( );

	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	CHECK_NULL_PTR_ERROR (vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetMgx3DPicker ( ))
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetMgx3DPicker ( )->SetCellTolerance (
						VTKConfiguration::instance ( )._cellPickerTolerance.getValue ( ));
	vgw->getVTKRenderingManager ( ).enableMultipleLighting (Resources::instance ( )._multipleLighting.getValue ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::preferencesCallback


void QtVtkMgx3DMainWindow::print3DViewCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
    CHECK_NULL_PTR_ERROR (vgw->getVTKWidget ( ).GetRenderWindow ( ))

//    getVTKWidget ( ).raise ( );
    QtVTKPrintHelper::PrinterStatus status  = QtVTKPrintHelper::print (*(vgw->getVTKWidget( ).GetRenderWindow( )), this);
    if (false == status.completed)
        return;

	UTF8String	message (Charset::UTF_8);
	if (QtVTKPrintHelper::PrinterStatus::PRINTER == status.destination)
		message << "Impression effectuée sur " << status.deviceName;
	else
		message << "Vue sauvegardée dans le fichier " << status.deviceName;
	message << ".";
	log (InformationLog (message));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::print3DViewCallback


void QtVtkMgx3DMainWindow::print3DViewToFileCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
    CHECK_NULL_PTR_ERROR (vgw->getVTKWidget ( ).GetRenderWindow ( ))

//    getVTKWidget ( ).raise ( );
    QtVTKPrintHelper::PrinterStatus status  = QtVTKPrintHelper::printToFile (*(vgw->getVTKWidget( ).GetRenderWindow( )), this);
    if (false == status.completed)
        return;

	UTF8String	message (Charset::UTF_8);
	if (QtVTKPrintHelper::PrinterStatus::PRINTER == status.destination)
		message << "Impression effectuée sur " << status.deviceName;
	else
		message << "Vue sauvegardée dans le fichier " << status.deviceName;
	message << ".";
	log (InformationLog (message));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::print3DViewToFileCallback


void QtVtkMgx3DMainWindow::selectionModeCallback (bool checked)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const bool	boundingBox	= !checked;
	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	CHECK_NULL_PTR_ERROR (vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetMgx3DPicker ( ))
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).GetMgx3DPicker ( )->SetMode (true == boundingBox ? VTKMgx3DPicker::BOUNDING_BOX : VTKMgx3DPicker::DISTANCE);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::selectionModeCallback


void QtVtkMgx3DMainWindow::pickingSelectionCallback (bool on)
{
	BEGIN_QT_TRY_CATCH_BLOCK
	
	QtMgx3DMainWindow::pickingSelectionCallback (on);	// actualisation menu/bouton

	if (false == on)
		return;
		
	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetRubberBand (false);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::pickingSelectionCallback


void QtVtkMgx3DMainWindow::rubberBandSelectionCallback (bool on)
{
	BEGIN_QT_TRY_CATCH_BLOCK
	
	QtMgx3DMainWindow::rubberBandSelectionCallback (on);	// actualisation menu/bouton

	if (false == on)
		return;
		
	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetRubberBand (true);
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetCompletelyInsideSelection (false);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::rubberBandSelectionCallback


void QtVtkMgx3DMainWindow::rubberBandInsideSelectionCallback (bool on)
{
	BEGIN_QT_TRY_CATCH_BLOCK
	
	QtMgx3DMainWindow::rubberBandInsideSelectionCallback (on);	// actualisation menu/bouton

	if (false == on)
		return;
		
	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetRubberBand (true);
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetCompletelyInsideSelection (true);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::rubberBandInsideSelectionCallback


void QtVtkMgx3DMainWindow::visibleSelectionCallback (bool on)
{
	BEGIN_QT_TRY_CATCH_BLOCK
	
	QtMgx3DMainWindow::visibleSelectionCallback (on);

	QtVtkGraphicalWidget*	vgw	= dynamic_cast<QtVtkGraphicalWidget*>(&getGraphicalWidget ( ));
	CHECK_NULL_PTR_ERROR (vgw)
	vgw->getVTKRenderingManager ( ).getMgx3DInteractorStyle ( ).SetForegroundSelection (on);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::visibleSelectionCallback


#ifdef USE_EXPERIMENTAL_ROOM
void QtVtkMgx3DMainWindow::loadRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	static string	lastDir (Process::getCurrentDirectory ( ));
	loadRays (lastDir);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::loadRaysCallback


void QtVtkMgx3DMainWindow::importRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const string	dbDir (LASERS_DB_DIR);
	string			dir	= dbDir;

	loadRays (dir);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::importRaysCallback


void QtVtkMgx3DMainWindow::saveRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)
	if ((0 != _experimentalRoomPanel->getRaysIOManager ( )) &&
	    (true == _experimentalRoomPanel->getRaysIOManager ( )->hasWriter ( )))
	{
		const Charset::CHARSET	encoding	=
			Charset::UNKNOWN != _experimentalRoomPanel->getRaysIOManager ( )->getCharset ( ) ?
			_experimentalRoomPanel->getRaysIOManager ( )->getCharset ( ) : getDefaultRaysCharset ( ).charset ( );
		 _experimentalRoomPanel->getRaysIOManager ( )->save (_experimentalRoomPanel->getExperimentalRoom ( ), ExperimentRay::LASER, encoding);
	}	// if ((0 != _experimentalRoomPanel->getRaysIOManager ( )) && ...
	else
		saveAsRaysCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::saveRaysCallback


void QtVtkMgx3DMainWindow::saveAsRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)

	File	file	(_experimentalRoomPanel->getExperimentalRoom ( ).getRaysFilePath ( ));
	string	dir	= file.getPath ( ).getFullFileName ( );
	vector<QtExpRoomFileDialog::Format>	formats;
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	tExt		= TRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("T", tExt, true, true));
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	erdExt		= ERDRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("Experimental Room Description", erdExt, true, true));
	static bool	hiddenRays	= false;
	if ((0 != _experimentalRoomPanel->getRaysIOManager ( )) &&
	    (true == _experimentalRoomPanel->getRaysIOManager ( )->hasWriter( )))	
		hiddenRays	= _experimentalRoomPanel->getRaysIOManager ( )->getWriter ( ).saveHiddenRays ( );
	const Charset::CHARSET	charset	=
			Charset::UNKNOWN != _experimentalRoomPanel->getRaysIOManager ( )->getCharset ( ) ?
			_experimentalRoomPanel->getRaysIOManager ( )->getCharset ( ) : getDefaultRaysCharset ( ).charset ( );
	QtExpRoomFileDialog	dialog (this, getAppTitle ( ).c_str ( ), ExperimentRay::LASER, QFileDialog::AcceptSave,
				        dir.c_str ( ), formats, charset);
	dialog.setModal (true);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.selectFile (file.getFullFileName ( ).c_str ( ));
	dialog.processVisiblesOnly (!hiddenRays);
	if (QDialog::Accepted != dialog.exec ( ))
		return;

	ExperimentalRoomIOManager*	ioManager	= 0;
	QStringList					fileList	= dialog.selectedFiles ( );
	file.setFullFileName (fileList [0].toStdString ( ));
	hiddenRays	= !dialog.processVisiblesOnly ( );

#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	if ((file.getExtension ( ) == T_INTERNAL_EXTENSION) || ((true == dialog.selectedNameFilter ( ).startsWith ("T ")) && ((false == file.exists ( )) || (ExperimentalRoomTIOManager::isTFile (file.getFullFileName ( ))))))
	{
		ioManager	= dynamic_cast<ExperimentalRoomTIOManager*>(_experimentalRoomPanel->getRaysIOManager ( ));
		if (0 == ioManager)
		{
			ioManager	= new ExperimentalRoomTIOManager ( );
			_experimentalRoomPanel->setRaysIOManager (ioManager);
		}	// if (0 == ioManager)
	}
	else
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
	if ((file.getExtension ( ) == ERD_INTERNAL_EXTENSION) || ((true == dialog.selectedNameFilter ( ).startsWith ("Experimental Room Description")) && ((false == file.exists ( )) || (ExperimentalRoomERDIOManager::isERDFile (file.getFullFileName ( ))))))
	{
		ioManager	= dynamic_cast<ExperimentalRoomERDIOManager*>(_experimentalRoomPanel->getRaysIOManager ( ));
		if (0 == ioManager)
		{
			ioManager	= new ExperimentalRoomERDIOManager ( );
			_experimentalRoomPanel->setRaysIOManager (ioManager);
		}	// if (0 == ioManager)
	}
	else
		throw Exception ("Format de fichier non reconnu");
	CHECK_NULL_PTR_ERROR (ioManager)
	ioManager->getWriter ( ).saveHiddenRays (hiddenRays);
	const Charset::CHARSET	encoding	= dialog.getCharset ( );
	ioManager->getWriter ( ).save (_experimentalRoomPanel->getExperimentalRoom ( ), file.getFullFileName ( ), ExperimentRay::LASER, encoding);
	_experimentalRoomPanel->setRaysFileName (file.getFullFileName( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::saveAsRaysCallback


void QtVtkMgx3DMainWindow::setRaysContextCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)
	_experimentalRoomPanel->setContext (getGraphicalWidget ( ).getRenderingManager ( ).getContext ( ), getGraphicalWidget ( ).getRenderingManager ( ).useGlobalDisplayProperties ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::setRaysContextCallback


void QtVtkMgx3DMainWindow::setRaysTargetSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)
	_experimentalRoomPanel->useSelection (getGraphicalWidget ( ).getRenderingManager ( ).getContext ( ), getGraphicalWidget ( ).getRenderingManager ( ).useGlobalDisplayProperties ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::setRaysTargetSurfacesCallback


void QtVtkMgx3DMainWindow::loadRays (string& dir)
{
	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)

	// On interdit le chargement en l'absence de contexte (d'affichage) valide :
	try
	{
		_experimentalRoomPanel->getContext ( );
	}
	catch (...)
	{
		try
		{
			_experimentalRoomPanel->setContext (getGraphicalWidget ( ).getRenderingManager ( ).getContext ( ), getGraphicalWidget ( ).getRenderingManager ( ).useGlobalDisplayProperties ( ));
		}
		catch (...)
		{
			throw Exception (UTF8String ("Absence de contexte d'affichage des pointages laser.\
Affecter repère et unité de longueur (menu Session) puis le contexte ainsi \
défini (Menu Chambre expérimentale).", Charset::UTF_8));
		}
	}

	vector<QtExpRoomFileDialog::Format>	formats;
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	tExt		= TRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("T", tExt, false, true));
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	erdExt		= ERDRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("Experimental Room Description", erdExt, false, true));
	vector<string>	visradExt	    = VisRadCsvReader::getFileExtensions ( );
	vector<string>	visradVrwExt	= VisRadVrwReader::getFileExtensions( );
	visradExt.insert (visradExt.end ( ), visradVrwExt.begin ( ), visradVrwExt.end ( ));
	formats.push_back (QtExpRoomFileDialog::Format ("VisRAD", visradExt, true, false));
	QString			lastDir (dir.c_str ( ));
	static QString	lastFilter ("");
	static bool		hiddenRays	= false;

	QtExpRoomFileDialog	dialog (this, getAppTitle ( ).c_str ( ), ExperimentRay::LASER, QFileDialog::AcceptOpen, lastDir.toStdString ( ), formats);
	dialog.setModal (true);
	dialog.processVisiblesOnly (!hiddenRays);
	dialog.setFileMode (QFileDialog::ExistingFile);

	if (QDialog::Accepted != dialog.exec ( ))
		return;

	lastDir				= dialog.directory ( ).absolutePath ( );
	hiddenRays			= !dialog.processVisiblesOnly ( );
	dir					= lastDir.toStdString ( );
	lastFilter			= dialog.selectedNameFilter ( );
	QStringList	fileList= dialog.selectedFiles ( );
	TkUtil::File	file (fileList [0].toStdString ( ));
	// Faudra t'il détruire les données applicatives de la chambre ?
	const bool			        eraseAppData= !file.isWritable ( );
	ExperimentalRoomIOManager*	ioManager	= 0;
	ExperimentalRoom*			room	    = 0;

	if ((file.getExtension ( ) == ERD_INTERNAL_EXTENSION) || ((file.getExtension ( ) == "xml") && (ExperimentalRoomERDIOManager::isERDFile (file.getFullFileName ( )))))
	{
		ioManager	= new ExperimentalRoomERDIOManager ( );
		room	    = new ExperimentalRoom (file.getFileName ( ), ExperimentalRoom::TARGET);
		ioManager->getReader ( ).loadRoom (file.getFullFileName ( ), *room, ExperimentRay::LASER, ExperimentRay::CARTESIAN_EXP_ROOM);
	}
	else if (file.getExtension ( ) == "csv")
	{
		ioManager	= new VisRadCsvIOManager ( );
		room	    = new ExperimentalRoom (file.getFileName ( ), ExperimentalRoom::TARGET);
		ioManager->getReader ( ).loadRoom (file.getFullFileName ( ), *room, ExperimentRay::LASER, ExperimentRay::CARTESIAN_EXP_ROOM);
	}	// if (file.getExtension ( ) == "csv")
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	else if ((file.getExtension ( ) == T_INTERNAL_EXTENSION) || ((file.getExtension ( ) == "xml") && (ExperimentalRoomTIOManager::isTFile (file.getFullFileName ( )))))
	{
		ioManager	= new ExperimentalRoomTIOManager ( );
		room	    = new ExperimentalRoom (file.getFileName ( ), ExperimentalRoom::TARGET);
		ioManager->getReader ( ).loadRoom (file.getFullFileName ( ), *room, ExperimentRay::LASER, ExperimentRay::CARTESIAN_EXP_ROOM);
	}
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
	else if (file.getExtension ( ) == "vrw")
	{
		ioManager	                    = new VisRadVrwIOManager ( );
		VisRadVrwReader*	reader		= dynamic_cast<VisRadVrwReader*>(&ioManager->getReader ( ));
		CHECK_NULL_PTR_ERROR (reader)
		reader->loadHiddenRays (hiddenRays);
		room	= new ExperimentalRoom (file.getFileName ( ), ExperimentalRoom::TARGET);
		ioManager->getReader ( ).loadRoom (file.getFullFileName ( ), *room, ExperimentRay::LASER, ExperimentRay::CARTESIAN_EXP_ROOM);
	}	// if (file.getExtension ( ) == "vrw")
	else
		throw Exception ("Format de fichier non reconnu");

	CHECK_NULL_PTR_ERROR (ioManager)
	CHECK_NULL_PTR_ERROR (room)
	if (true == eraseAppData)
	{
		room->clearRaysAppData ( );
		room->setRaysFilePath ("");
		ioManager->getReader ( ).setFilePath ("");
	}	// if (true == eraseAppData)
	_experimentalRoomPanel->setExperimentalRoom (*room, ioManager);
	_experimentalRoomPanel->setRaysFileName (file.getFullFileName( ));
}	// QtVtkMgx3DMainWindow::loadRays


void QtVtkMgx3DMainWindow::loadDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	static string	lastDir (Process::getCurrentDirectory ( ));
	loadDiags (lastDir);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::loadDiagsCallback


void QtVtkMgx3DMainWindow::importDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const string	dbDir (DIAGNOSTICS_DB_DIR);
	string			dir	= dbDir;

	loadDiags (dir);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::importDiagsCallback


void QtVtkMgx3DMainWindow::saveDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)
	if ((0 != _experimentalRoomPanel->getDiagsIOManager ( )) &&
	    (true == _experimentalRoomPanel->getDiagsIOManager ( )->hasWriter ( )))
	{
		const Charset::CHARSET	encoding	=
			Charset::UNKNOWN != _experimentalRoomPanel->getDiagsIOManager ( )->getCharset ( ) ?
			_experimentalRoomPanel->getDiagsIOManager ( )->getCharset ( ) : getDefaultRaysCharset ( ).charset ( );
		 _experimentalRoomPanel->getDiagsIOManager ( )->save (_experimentalRoomPanel->getExperimentalRoom ( ), ExperimentRay::DIAGNOSIS, encoding);
	}	// if ((0 != _experimentalRoomPanel->getDiagsIOManager ( )) && ...
	else
		saveAsDiagsCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::saveDiagsCallback


void QtVtkMgx3DMainWindow::saveAsDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)

	File	file	(_experimentalRoomPanel->getExperimentalRoom ( ).getDiagsFilePath ( ));
	string	dir	= file.getPath ( ).getFullFileName ( );
	vector<QtExpRoomFileDialog::Format> formats;
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	tExt		= TRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("T", tExt, true, true));
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	erdExt		= ERDRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("Experimental Room Description", erdExt, true, true));
	const Charset::CHARSET	charset	=
			Charset::UNKNOWN != _experimentalRoomPanel->getDiagsIOManager ( )->getCharset ( ) ?
			_experimentalRoomPanel->getDiagsIOManager ( )->getCharset ( ) : getDefaultRaysCharset ( ).charset ( );
	QtExpRoomFileDialog	dialog (this, getAppTitle ( ).c_str ( ), ExperimentRay::DIAGNOSIS, QFileDialog::AcceptSave, dir.c_str ( ), formats, charset);
	dialog.setModal (true);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.selectFile (file.getFullFileName ( ).c_str ( ));
	if (QDialog::Accepted != dialog.exec ( ))
		return;

	ExperimentalRoomIOManager*	ioManager	= 0;
	QStringList					fileList	= dialog.selectedFiles ( );
	file.setFullFileName (fileList [0].toStdString ( ));

#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	if ((file.getExtension ( ) == T_INTERNAL_EXTENSION) || ((true == dialog.selectedNameFilter ( ).startsWith ("T ")) && ((false == file.exists ( )) || (ExperimentalRoomTIOManager::isTFile (file.getFullFileName ( ))))))
	{
		ioManager	= dynamic_cast<ExperimentalRoomTIOManager*>(_experimentalRoomPanel->getDiagsIOManager ( ));
		if (0 == ioManager)
		{
			ioManager	= new ExperimentalRoomTIOManager ( );
			_experimentalRoomPanel->setDiagsIOManager (ioManager);
		}	// if (0 == ioManager)
	}
	else
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
		if ((file.getExtension ( ) == ERD_INTERNAL_EXTENSION) || ((true == dialog.selectedNameFilter ( ).startsWith ("Experimental Room Description")) && ((false == file.exists ( )) || (ExperimentalRoomERDIOManager::isERDFile (file.getFullFileName ( ))))))
	{
		ioManager	= dynamic_cast<ExperimentalRoomERDIOManager*>(_experimentalRoomPanel->getDiagsIOManager ( ));
		if (0 == ioManager)
		{
			ioManager	= new ExperimentalRoomERDIOManager ( );
			_experimentalRoomPanel->setDiagsIOManager (ioManager);
		}	// if (0 == ioManager)
	}
	else
		throw Exception ("Format de fichier non reconnu");

	CHECK_NULL_PTR_ERROR (ioManager)
	const Charset::CHARSET	encoding	= dialog.getCharset ( );
	ioManager->getWriter ( ).saveHiddenRays (!dialog.processVisiblesOnly ( ));
	ioManager->getWriter ( ).save (_experimentalRoomPanel->getExperimentalRoom( ),file.getFullFileName( ), ExperimentRay::DIAGNOSIS, encoding);
	_experimentalRoomPanel->setDiagsFileName (file.getFullFileName( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtVtkMgx3DMainWindow::saveAsDiagsCallback


void QtVtkMgx3DMainWindow::loadDiags (string& dir)
{
	CHECK_NULL_PTR_ERROR (_experimentalRoomPanel)

	// On interdit le chargement en l'absence de contexte (d'affichage) valide :
	try
	{
		_experimentalRoomPanel->getContext ( );
	}
	catch (...)
	{
		try
		{
			_experimentalRoomPanel->setContext (getGraphicalWidget ( ).getRenderingManager ( ).getContext ( ), getGraphicalWidget ( ).getRenderingManager ( ).useGlobalDisplayProperties ( ));
		}
		catch (...)
		{
			throw Exception (UTF8String ("Absence de contexte d'affichage des diagnostics.\
Affecter repère et unité de longueur (menu Session) puis le contexte ainsi \
défini (Menu Chambre expérimentale).", Charset::UTF_8));
		}
	}

	QString			lastDir (dir.c_str ( ));
	static QString	lastFilter ("");
	vector<QtExpRoomFileDialog::Format>	formats;
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	tExt		= TRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("T", tExt, false, true));
#endif	// USE_EXPERIMENTAL_ROOM_TRAYS
	vector<string>	erdExt		= ERDRaysIO::getFileExtensions ( );
	formats.push_back (QtExpRoomFileDialog::Format ("Experimental Room Description", erdExt, false, true));
	QtExpRoomFileDialog	dialog (this, getAppTitle ( ).c_str ( ), ExperimentRay::DIAGNOSIS, QFileDialog::AcceptOpen, lastDir.toStdString ( ), formats);
	dialog.setModal (true);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setFileMode (QFileDialog::ExistingFile);

	if (QDialog::Accepted != dialog.exec ( ))
		return;

	lastDir			= dialog.directory ( ).absolutePath ( );
	dir				= lastDir.toStdString ( );
	lastFilter		= dialog.selectedNameFilter ( );
	QStringList	fileList	= dialog.selectedFiles ( );
	TkUtil::File	file (fileList [0].toStdString ( ));
	// Faudra t'il détruire les données applicatives de la chambre ?
	const bool	eraseAppData	= !file.isWritable ( );

	if ((file.getExtension ( ) == ERD_INTERNAL_EXTENSION) || ((file.getExtension ( ) == "xml") && (ExperimentalRoomERDIOManager::isERDFile (file.getFullFileName ( )))))
	{
		ExperimentalRoomERDIOManager*	ioManager	= new ExperimentalRoomERDIOManager ( );
		ExperimentalRoom*	            room	    = new ExperimentalRoom (file.getFileName ( ), ExperimentalRoom::TARGET);
		ioManager->getReader ( ).loadRoom (file.getFullFileName ( ), *room, ExperimentRay::DIAGNOSIS, ExperimentRay::CARTESIAN_EXP_ROOM);
		vector<ExperimentRay*>	diags1	= room->getDiags ( ), diags2;
		if (true == eraseAppData)
		{
			room->clearDiagsAppData ( );
			room->setDiagsFilePath ("");
			ioManager->getReader ( ).setFilePath ("");
		}	// if (true == eraseAppData)
		for (vector<ExperimentRay*>::const_iterator itd = diags1.begin ( ); diags1.end ( ) != itd; itd++)
			diags2.push_back ((*itd)->clone ( ));
		diags1.clear ( );	// not mandatory
		_experimentalRoomPanel->setDiags (diags2, ioManager, room->getDiagsAppData ( ), true);
		_experimentalRoomPanel->setDiagsFileName (file.getFullFileName( ));
		diags2.clear ( );	// not mandatory
		delete room;	room	= 0;
	}
#ifdef USE_EXPERIMENTAL_ROOM_TRAYS
	else if ((file.getExtension ( ) == T_INTERNAL_EXTENSION) || ((file.getExtension ( ) == "xml") && (ExperimentalRoomTIOManager::isTFile (file.getFullFileName ( )))))
	{
		ExperimentalRoomTIOManager*	ioManager	= new ExperimentalRoomTIOManager ( );
		ExperimentalRoom*	        room	    = new ExperimentalRoom (file.getFileName ( ), ExperimentalRoom::TARGET);
		ioManager->getReader ( ).loadRoom (file.getFullFileName ( ), *room, ExperimentRay::DIAGNOSIS, ExperimentRay::CARTESIAN_EXP_ROOM);
		vector<ExperimentRay*>	diags1	= room->getDiags ( ), diags2;
		if (true == eraseAppData)
		{
			room->clearDiagsAppData ( );
			room->setDiagsFilePath ("");
			ioManager->getReader ( ).setFilePath ("");
		}	// if (true == eraseAppData)
		for (vector<ExperimentRay*>::const_iterator itd = diags1.begin ( ); diags1.end ( ) != itd; itd++)
			diags2.push_back ((*itd)->clone ( ));
		diags1.clear ( );	// not mandatory
		_experimentalRoomPanel->setDiags (diags2, ioManager, room->getDiagsAppData ( ), true);
		_experimentalRoomPanel->setDiagsFileName (file.getFullFileName( ));
		diags2.clear ( );	// not mandatory
		delete room;	room	= 0;
	}
#endif	 // USE_EXPERIMENTAL_ROOM_TRAYS
	else
		throw Exception ("Format de fichier non reconnu");
}	// QtVtkMgx3DMainWindow::loadDiags
#endif	// USE_EXPERIMENTAL_ROOM


}	// namespace QtVtkComponents

}	// namespace Mgx3D

