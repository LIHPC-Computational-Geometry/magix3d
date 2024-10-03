/**
 * \file        QtMgx3DMainWindow.cpp
 * \author      Charles PIGNEROL
 * \date        29/11/2010
 */

#include "Internal/ContextIfc.h"

#include "Utils/Command.h"
#include "Utils/Magix3DEvents.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/Common.h"
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/GeomProperty.h"
#include "Internal/Context.h"
#include "Internal/Resources.h"
#include "Internal/CommandInternal.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/PythonWriter.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DSelectionDialog.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtLandmarkDialog.h"
#include "QtComponents/QtMdlOptionsDialog.h"
#include "QtComponents/QtImportOptionsDialog.h"
#include "QtComponents/QtMgx3DScriptFileDialog.h"
#include "QtComponents/QtGuiStateDialog.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtColorTablesEditorDialog.h"

// Les opérations géométriques :
#include "QtComponents/QtArcCircleOperationAction.h"
#include "QtComponents/QtBoxOperationAction.h"
#include "QtComponents/QtCircleOperationAction.h"
#include "QtComponents/QtConeOperationAction.h"
#include "QtComponents/QtCurveByProjectionOnSurfaceAction.h"
#include "QtComponents/QtCylinderOperationAction.h"
#include "QtComponents/QtSpherePartOperationAction.h"
#include "QtComponents/QtSphereOperationAction.h"
#include "QtComponents/QtDistanceMeasurementOperationAction.h"
#include "QtComponents/QtAngleMeasurementOperationAction.h"
#include "QtComponents/QtExtremaMeshingEdgeLengthOnEdgeAction.h"
#include "QtComponents/QtBooleanOpOperationAction.h"
#include "QtComponents/QtPrismCreationAction.h"
#include "QtComponents/QtGeomEntityByRevolutionCreationAction.h"
#include "QtComponents/QtGeomEntityCommon2DOperationAction.h"
#include "QtComponents/QtSegmentOperationAction.h"
#include "QtComponents/QtGeomAddToGroupOperationAction.h"
#include "QtComponents/QtGeomEntityCopyOperationAction.h"
#include "QtComponents/QtGeomEntityCopyByOffsetAction.h"
#include "QtComponents/QtGeomEntityDestructionAction.h"
#include "QtComponents/QtGeomPlaneCutAction.h"
#include "QtComponents/QtGeomSurfaceCreationAction.h"
#include "QtComponents/QtGeomVolumeCreationAction.h"
#include "QtComponents/QtGeometryGlueCurvesAction.h"
#include "QtComponents/QtGeometryGlueSurfacesAction.h"
#include "QtComponents/QtGeometryHomothetyAction.h"
#include "QtComponents/QtGeometryRotationAction.h"
#include "QtComponents/QtGeometryTranslationAction.h"
#include "QtComponents/QtGeometryMirrorAction.h"
#include "QtComponents/QtVertexOperationAction.h"
#include "QtComponents/QtLengthUnitDialog.h"
#include "Geom/CommandImportIGES.h"
// Les opérations topologiques :
#include "QtComponents/QtBlocksByExtrusionCreationAction.h"
#include "QtComponents/QtBlocksByRevolutionCreationAction.h"
#include "QtComponents/QtBlockAssociationAction.h"
#include "QtComponents/QtTopologyAddToGroupOperationAction.h"
#include "QtComponents/QtTopologyBlockCreationAction.h"
#include "QtComponents/QtTopologyBlockExtractionAction.h"
#include "QtComponents/QtTopologyVertexCreationAction.h"
#include "QtComponents/QtTopologyEdgeCreationAction.h"
#include "QtComponents/QtTopologyFaceCreationAction.h"
#include "QtComponents/QtTopoEntitiesAssociationsAction.h"
#include "QtComponents/QtTopoEntityCopyOperationAction.h"
#include "QtComponents/QtTopologyFuse2BlocksAction.h"
#include "QtComponents/QtTopologyGlue2BlocksAction.h"
#include "QtComponents/QtTopologyInsertHoleAction.h"
#include "QtComponents/QtTopologyPlaceVertexAction.h"
#include "QtComponents/QtTopologyProjectVerticesAction.h"
#include "QtComponents/QtTopologySnapVerticesAction.h"
#include "QtComponents/QtTopologySplitBlockAction.h"
#include "QtComponents/QtTopologySplitFaceWithOGridAction.h"
#include "QtComponents/QtTopologySplitBlockWithOGridAction.h"
#include "QtComponents/QtTopologyExtendSplitBlockAction.h"
#include "QtComponents/QtTopologyExtendSplitFaceAction.h"
#include "QtComponents/QtTopologySplitFaceAction.h"
#include "QtComponents/QtTopologySplitFacesAction.h"
#include "QtComponents/QtTopologyVerticesAlignmentAction.h"
#include "QtComponents/QtTopoEntityDestructionAction.h"
#include "QtComponents/QtBlockMeshingPropertyAction.h"
#include "QtComponents/QtTopologyEdgeCutAction.h"
#include "QtComponents/QtTopologyEdgeDirectionAction.h"
#include "QtComponents/QtEdgeMeshingPropertyAction.h"
#include "QtComponents/QtFaceMeshingPropertyAction.h"
#include "QtComponents/QtTopologyHomothetyAction.h"
#include "QtComponents/QtTopologyRotationAction.h"
#include "QtComponents/QtTopologyTranslationAction.h"
#include "QtComponents/QtTopologyMirrorAction.h"
#include "QtComponents/QtTopologyUnrefineBlockAction.h"
#include "QtComponents/QtTopoInformationOperationAction.h"
// Les opérations maillages :
#include "QtComponents/QtGroupProjectionOnPlaneAction.h"
#include "QtComponents/QtMeshSubVolumeBetweenSheetsAction.h"
#include "QtComponents/QtMeshBlocksOperationAction.h"
#include "QtComponents/QtMeshFacesOperationAction.h"
#include "QtComponents/QtMeshExplorerOperationAction.h"
#include "QtComponents/QtMeshQualityOperationAction.h"
#include "QtComponents/QtMeshInformationOperationAction.h"
#include "QtComponents/QtSmoothSurfaceOperationAction.h"
#include "QtComponents/QtSmoothVolumeOperationAction.h"
// Les opérations repères :
#include "QtComponents/QtSysCoordOperationAction.h"
#include "QtComponents/QtSysCoordRotationAction.h"
#include "QtComponents/QtSysCoordTranslationAction.h"

#include <TkUtil/File.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/ProcessLog.h>
#include <TkUtil/ScriptingLog.h>
#include <TkUtil/WarningLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NetworkData.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/Process.h>
#include <TkUtil/UtilInfos.h>
#include <PythonUtil/PythonInfos.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/UserData.h>
#include <QtUtil/QtActionAutoLock.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtCoordinatesDialog.h>
#include <QtUtil/QtFileDialogUtilities.h>
//#include <QtUtil/QtHelpWindow.h>
#include <patchlevel.h>	// PY_VERSION
#if PY_MAJOR_VERSION >= 3
#	include <QtPython3/QtPython.h>
#else	// PY_MAJOR_VERSION >= 3
#	include <QtPython/QtPython.h>
#endif	// PY_MAJOR_VERSION >= 3
#include <QtUtil/QtUnicodeHelper.h>
//#include <QtUtil/QtExternalLinkDialog.h>
#include <QtUtil/QtStringHelper.h>
#include <QtUtil/QtUtilInfos.h>
#include <GQualif/QualifHelper.h>
#include <PrefsCore/PreferencesInfos.h>
#include <PrefsQt/QtPreferencesInfos.h>
#include <QtQualif/QtQualifServices.h>
#include <QtUtil/QtObjectSignalBlocker.h>
#include <QwtCharts/QwtChartsManager.h>
#include <Lima/enum.h>
//#include <Lima/malipp.h>
#include <LimaP/reader.h>
#include <IQualif.h>
#include <TkUtil/Timer.h>

#include <QSettings>
#include <QThread>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabWidget>


#include <iostream>
#include <stdlib.h>		// getenv
#include <cmath>
#include <strings.h>	// strcasecmp
#include <cgnslib.h>			// CGNS_DOTVERS
#include <mesquite_version.h>	// MSQ_VERSION_STRING

#include <xercesc/util/XercesVersion.hpp>
#include <QtComponents/QtTopologyFuse2EdgesAction.h>
#include <QtComponents/QtTopologyFuse2FacesAction.h>
#include <QtComponents/QtTopologyFuse2VerticesAction.h>
#include <QtComponents/QtTopologyDegenerateBlockAction.h>
#include <QtComponents/QtTopologyFuseEdgesAction.h>



#include <Standard_Version.hxx> // OCC

// NECESSAIRE POUR LES IMAGES AU FORMAT XPM
#include "images/union.xpm"

#undef LOCK_INSTANCE
#define LOCK_INSTANCE AutoReferencedMutex autoReferencedMutex (TkUtil::ObjectBase::getMutex ( ));


using namespace std;
using namespace TkUtil;
using namespace Preferences;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Structured;
using namespace Mgx3D::Internal;
//using namespace Mgx3D::Commands;


namespace Mgx3D
{

namespace QtComponents
{

	static bool compareExtensions(const string &ext1, const string &ext2);

/// ajoute pre au début de chacune des lignes de us
	static UTF8String addCharAtBeginLines(char pre, UTF8String &us);


	QtAutoDisablabledAction::QtAutoDisablabledAction(
			const QString &label, QObject *parent)
			: QAction(label, parent)
	{
		connect(this, SIGNAL(triggered()), this, SLOT(disable()));
	}    // QtAutoDisablabledAction::QtAutoDisablabledAction


	QtAutoDisablabledAction::QtAutoDisablabledAction(
			const QIcon &icon, const QString &label, QObject *parent)
			: QAction(icon, label, parent)
	{
		connect(this, SIGNAL(triggered()), this, SLOT(disable()));
	}    // QtAutoDisablabledAction::QtAutoDisablabledAction


	void QtAutoDisablabledAction::disable()
	{
		setEnabled(false);
	}    // QtAutoDisablabledAction::disable


// ===========================================================================
//                LA CLASSE QtMgx3DMainWindow::WindowsActions
// ===========================================================================

		QtMgx3DMainWindow::WindowsActions::WindowsActions()
				: _preferencesAction(0), _editSettingsAction(0), _quitAction(0),
				  _print3DViewAction(0), _print3DViewToFileAction(0),
				  _useGlobalDisplayPropertiesAction(0),
				  _displayTrihedronAction(0), _displayLandmarkAction(0),
				  _parametrizeLandmarkAction(0), _displayFocalPointAction(0),
#ifdef USE_EXPERIMENTAL_ROOM
                  _loadRaysAction(0), _importRaysAction(0),
                  _saveRaysAction(0), _saveAsRaysAction(0),
                  _setRaysContextAction(0), _setRaysTargetSurfacesAction(0),
                  _loadDiagsAction(0), _importDiagsAction(0),
                  _saveDiagsAction(0), _saveAsDiagsAction(0),
#endif	// USE_EXPERIMENTAL_ROOM
                  _displayLookupTableAction(0), _displayLookupTableEditorAction(0),
                  _xOyViewAction(0), _xOzViewAction(0), _yOzViewAction(0),
                  _resetViewAction(0), _clearViewAction(0), _zoomViewAction(0), _parallelViewAction(0),
                  _undoAction(0), _redoAction(0), _clearAction(0),
                  _meterAction(0), _centimeterAction(0), _millimeterAction(0),
                  _micrometerAction(0), _undefinedUnitAction(0),
                  _meshLandmarkAction(0),
                  _expRoomLandmarkAction(0),
                  _undefinedLandmarkAction(0),
                  _mesh2DDimAction(0),
                  _importAction(0), _exportAllAction(0), _exportSelectionAction(0),
                  _saveMagix3DScriptAction(0), _saveAsMagix3DScriptAction(0), _savePythonConsoleAction (0),
                  _longCommandAction(0), _openCascadeLongCommandAction(0),
                  _executePythonScriptAction(0),
                  _distanceMeasurementAction(0), _angleMeasurementAction(0), _extremaMeshingEdgeLengthOnEdgeAction(0),
                  _topoOptimMeshMethod(0),
                  _topoInformationAction(0), _meshInformationAction(0),
                  _topoRefineAction(0), _topoSetDefaultNbMeshingEdgesAction(0),
                  _topoNew3x3BoxesWithTopoAction(0),
                  _meshSelectionAction(0), _meshVisibleAction(0), _meshAllAction(0),
                  _unrefineMeshRepresentationAction(0),
                  _addMeshExplorerAction(0), _addMeshQualityAction(0),
                  _displaySelectedGroupsAction(0), _hideSelectedGroupsAction(0),
                  _displaySelectedTypesAction(0), _hideSelectedTypesAction(0),
                  _displaySelectedEntitiesAction(0), _hideSelectedEntitiesAction(0),
                  _displaySelectionPropertiesAction(0),
                  _displayComputableSelectionPropertiesAction(0),
                  _selectionRepresentationAction(0),
                  _selectEntitiesAction(0),
                  _selectVisibleEntitiesAction(0), _unselectVisibleEntitiesAction(0),
                  _selectFusableEdgesAction(0), _unselectFusableEdgesAction(0),
                  _selectInvalidEdges(0), _unselectInvalidEdges(0),
                  _selectBorderFacesAction(0), _unselectBorderFacesAction(0),
                  _selectFacesWithoutBlockAction(0), _unselectFacesWithoutBlockAction(0),
                  _selectSemiConformFacesAction(0), _unselectSemiConformFacesAction(0),
                  _selectInvalidFaces(0), _unselectInvalidFaces(0),
                  _selectUnstructuredFaces(0), _unselectUnstructuredFaces(0),
                  _selectTransfiniteFaces(0), _unselectTransfiniteFaces(0),
                  _selectInvalidBlocks(0), _unselectInvalidBlocks(0),
                  _selectUnstructuredBlocks(0), _unselectUnstructuredBlocks(0),
                  _selectTransfiniteBlocks(0), _unselectTransfiniteBlocks(0),
                  _selectNodesAction(0), _selectEdgesAction(0), _selectSurfacesAction(0),
                  _selectVolumesAction(0), _selectionModeAction(0),
                  _showCommandMonitorDialogAction(0),
                  _displayUsersGuideAction(0), _displayUsersGuideContextAction(0),
                  _displayWikiAction(0), _displayTutorialAction(0), _displayPythonAPIUsersGuideAction(0),
                  _displayQualifAction(0), _displayShortKeyAction(0), _displaySelectAction(0),
                  _displayHistoriqueAction(0), _aboutDialogAction(0),
                  _mgx3DScriptsMenu(0),
                  _recentFilesCapacity(Resources::instance()._recentScriptCapacity.getValue()),
                  _mgx3DScriptsActions(0), _undoManager(0)
		{
			_mgx3DScriptsActions = new QAction *[_recentFilesCapacity];
			for (size_t i = 0; i < _recentFilesCapacity; i++)
			{
				_mgx3DScriptsActions[i] = 0;
			}    // for (size_t i = 0; i < _recentFilesCapacity; i++)
		}    // WindowsActions::WindowsActions


		QtMgx3DMainWindow::WindowsActions::WindowsActions(const QtMgx3DMainWindow::WindowsActions &wa)
				: _preferencesAction(wa._preferencesAction),
				  _editSettingsAction(wa._editSettingsAction),
				  _quitAction(wa._quitAction),
				  _print3DViewAction(wa._print3DViewAction),
				  _print3DViewToFileAction(wa._print3DViewToFileAction),
				  _useGlobalDisplayPropertiesAction(wa._useGlobalDisplayPropertiesAction),
				  _displayTrihedronAction(wa._displayTrihedronAction),
				  _displayLandmarkAction(wa._displayLandmarkAction),
				  _parametrizeLandmarkAction(wa._parametrizeLandmarkAction),
				  _displayFocalPointAction(wa._displayFocalPointAction),
#ifdef USE_EXPERIMENTAL_ROOM
                  _loadRaysAction(wa._loadRaysAction),
                  _importRaysAction(wa._importRaysAction),
                  _saveRaysAction(wa._saveRaysAction),
                  _saveAsRaysAction(wa._saveAsRaysAction),
                  _setRaysContextAction(wa._setRaysContextAction),
                  _setRaysTargetSurfacesAction(wa._setRaysTargetSurfacesAction),
                  _loadDiagsAction(wa._loadDiagsAction),
                  _importDiagsAction(wa._importDiagsAction),
                  _saveDiagsAction(wa._saveDiagsAction),
                  _saveAsDiagsAction(wa._saveAsDiagsAction),
#endif	// USE_EXPERIMENTAL_ROOM
                  _displayLookupTableAction(wa._displayLookupTableAction),
                  _displayLookupTableEditorAction(wa._displayLookupTableEditorAction),
                  _xOyViewAction(wa._xOyViewAction),
                  _xOzViewAction(wa._xOzViewAction),
                  _yOzViewAction(wa._yOzViewAction),
                  _resetViewAction(wa._resetViewAction),
                  _clearViewAction(wa._clearViewAction),
                  _zoomViewAction(wa._zoomViewAction),
                  _parallelViewAction(wa._parallelViewAction),
                  _undoAction(wa._undoAction),
                  _redoAction(wa._redoAction),
                  _clearAction(wa._clearAction),
                  _meterAction(wa._meterAction),
                  _centimeterAction(wa._centimeterAction),
                  _millimeterAction(wa._millimeterAction),
                  _micrometerAction(wa._micrometerAction),
                  _undefinedUnitAction(wa._undefinedUnitAction),
                  _meshLandmarkAction(wa._meshLandmarkAction),
                  _expRoomLandmarkAction(wa._expRoomLandmarkAction),
                  _undefinedLandmarkAction(wa._undefinedLandmarkAction),
                  _mesh2DDimAction(wa._mesh2DDimAction),
                  _importAction(wa._importAction),
                  _exportAllAction(wa._exportAllAction),
                  _exportSelectionAction(wa._exportSelectionAction),
                  _saveMagix3DScriptAction(wa._saveMagix3DScriptAction),
                  _saveAsMagix3DScriptAction(wa._saveAsMagix3DScriptAction),
                  _savePythonConsoleAction (wa._savePythonConsoleAction),
                  _longCommandAction(wa._longCommandAction),
                  _openCascadeLongCommandAction(wa._openCascadeLongCommandAction),
                  _executePythonScriptAction(wa._executePythonScriptAction),
                  _distanceMeasurementAction(wa._distanceMeasurementAction),
                  _angleMeasurementAction(wa._angleMeasurementAction),
                  _extremaMeshingEdgeLengthOnEdgeAction(wa._extremaMeshingEdgeLengthOnEdgeAction),
                  _topoOptimMeshMethod(wa._topoOptimMeshMethod),
                  _topoInformationAction(wa._topoInformationAction),
                  _meshInformationAction(wa._meshInformationAction),
                  _topoRefineAction(wa._topoRefineAction),
                  _topoSetDefaultNbMeshingEdgesAction(wa._topoSetDefaultNbMeshingEdgesAction),
                  _topoNew3x3BoxesWithTopoAction(wa._topoNew3x3BoxesWithTopoAction),
                  _meshSelectionAction(wa._meshSelectionAction),
                  _meshVisibleAction(wa._meshVisibleAction),
                  _meshAllAction(wa._meshAllAction),
                  _unrefineMeshRepresentationAction(wa._unrefineMeshRepresentationAction),
                  _addMeshExplorerAction(wa._addMeshExplorerAction),
                  _addMeshQualityAction(wa._addMeshQualityAction),
                  _displaySelectedGroupsAction(wa._displaySelectedGroupsAction),
                  _hideSelectedGroupsAction(wa._hideSelectedGroupsAction),
                  _displaySelectedTypesAction(wa._displaySelectedTypesAction),
                  _hideSelectedTypesAction(wa._hideSelectedTypesAction),
                  _displaySelectedEntitiesAction(wa._displaySelectedEntitiesAction),
                  _hideSelectedEntitiesAction(wa._hideSelectedEntitiesAction),
                  _displaySelectionPropertiesAction(wa._displaySelectionPropertiesAction),
                  _displayComputableSelectionPropertiesAction(wa._displayComputableSelectionPropertiesAction),
                  _selectionRepresentationAction(wa._selectionRepresentationAction),
                  _selectEntitiesAction(wa._selectEntitiesAction),
                  _selectVisibleEntitiesAction(wa._selectVisibleEntitiesAction),
                  _unselectVisibleEntitiesAction(wa._unselectVisibleEntitiesAction),
                  _selectFusableEdgesAction(wa._selectFusableEdgesAction),
                  _selectFacesWithoutBlockAction(wa._selectFacesWithoutBlockAction),
                  _unselectFacesWithoutBlockAction(wa._unselectFacesWithoutBlockAction),
                  _unselectFusableEdgesAction(wa._unselectFusableEdgesAction),
                  _selectInvalidEdges(wa._selectInvalidEdges),
                  _unselectInvalidEdges(wa._unselectInvalidEdges),
                  _selectBorderFacesAction(wa._selectBorderFacesAction),
                  _unselectBorderFacesAction(wa._unselectBorderFacesAction),
                  _selectSemiConformFacesAction(wa._selectSemiConformFacesAction),
                  _unselectSemiConformFacesAction(wa._unselectSemiConformFacesAction),
                  _selectInvalidFaces(wa._selectInvalidFaces),
                  _unselectInvalidFaces(wa._unselectInvalidFaces),
                  _selectUnstructuredFaces(wa._selectUnstructuredFaces),
                  _unselectUnstructuredFaces(wa._unselectUnstructuredFaces),
                  _selectTransfiniteFaces(wa._selectTransfiniteFaces),
                  _unselectTransfiniteFaces(wa._unselectTransfiniteFaces),
                  _selectInvalidBlocks(wa._selectInvalidBlocks),
                  _unselectInvalidBlocks(wa._unselectInvalidBlocks),
                  _selectUnstructuredBlocks(wa._selectUnstructuredBlocks),
                  _unselectUnstructuredBlocks(wa._unselectUnstructuredBlocks),
                  _selectTransfiniteBlocks(wa._selectTransfiniteBlocks),
                  _unselectTransfiniteBlocks(wa._unselectTransfiniteBlocks),
                  _selectNodesAction(wa._selectNodesAction),
                  _selectEdgesAction(wa._selectEdgesAction),
                  _selectSurfacesAction(wa._selectSurfacesAction),
                  _selectVolumesAction(wa._selectVolumesAction),
                  _selectionModeAction(wa._selectionModeAction),
                  _showCommandMonitorDialogAction(wa._showCommandMonitorDialogAction),
                  _displayUsersGuideAction(wa._displayUsersGuideAction),
                  _displayUsersGuideContextAction(wa._displayUsersGuideContextAction),
                  _displayWikiAction(wa._displayWikiAction),
                  _displayTutorialAction(wa._displayTutorialAction),
                  _displayPythonAPIUsersGuideAction(wa._displayPythonAPIUsersGuideAction),
                  _displayQualifAction(wa._displayQualifAction),
                  _displayShortKeyAction(wa._displayShortKeyAction),
                  _displaySelectAction(wa._displaySelectAction),
                  _displayHistoriqueAction(wa._displayHistoriqueAction),
                  _aboutDialogAction(wa._aboutDialogAction),
                  _mgx3DScriptsMenu(wa._mgx3DScriptsMenu),
                  _recentFilesCapacity(wa._recentFilesCapacity),
                  _mgx3DScriptsActions(wa._mgx3DScriptsActions),
                  _undoManager(wa._undoManager)
		{
		}    // WindowsActions::WindowsActions


		QtMgx3DMainWindow::WindowsActions &QtMgx3DMainWindow::WindowsActions::operator=(const QtMgx3DMainWindow::WindowsActions &wa)
		{
			if (&wa != this)
			{
				_preferencesAction       = wa._preferencesAction;
				_editSettingsAction      = wa._editSettingsAction;
				_quitAction              = wa._quitAction;
				_print3DViewAction       = wa._print3DViewAction;
				_print3DViewToFileAction = wa._print3DViewToFileAction;
				_useGlobalDisplayPropertiesAction = wa._useGlobalDisplayPropertiesAction;
				_displayTrihedronAction               = wa._displayTrihedronAction;
				_displayLandmarkAction                = wa._displayLandmarkAction;
				_parametrizeLandmarkAction            = wa._parametrizeLandmarkAction;
				_displayFocalPointAction              = wa._displayFocalPointAction;
#ifdef USE_EXPERIMENTAL_ROOM				
				_loadRaysAction                       = wa._loadRaysAction;
				_importRaysAction                     = wa._importRaysAction;
				_saveRaysAction                       = wa._saveRaysAction;
				_saveAsRaysAction                     = wa._saveAsRaysAction;
				_setRaysContextAction                 = wa._setRaysContextAction;
				_setRaysTargetSurfacesAction          = wa._setRaysTargetSurfacesAction;
				_loadDiagsAction                      = wa._loadDiagsAction;
				_importDiagsAction                    = wa._importDiagsAction;
				_saveDiagsAction                      = wa._saveDiagsAction;
				_saveAsDiagsAction                    = wa._saveAsDiagsAction;
#endif	// USE_EXPERIMENTAL_ROOM
				_displayLookupTableAction             = wa._displayLookupTableAction;
				_displayLookupTableEditorAction       = wa._displayLookupTableEditorAction;
				_xOyViewAction                        = wa._xOyViewAction;
				_xOzViewAction                        = wa._xOzViewAction;
				_yOzViewAction                        = wa._yOzViewAction;
				_resetViewAction                      = wa._resetViewAction;
				_clearViewAction                      = wa._clearViewAction;
				_zoomViewAction                       = wa._zoomViewAction;
				_parallelViewAction                   = wa._parallelViewAction;
				_undoAction                           = wa._undoAction;
				_redoAction                           = wa._redoAction;
				_clearAction                          = wa._clearAction;
				_meterAction                          = wa._meterAction;
				_centimeterAction                     = wa._centimeterAction;
				_millimeterAction                     = wa._millimeterAction;
				_micrometerAction                     = wa._micrometerAction;
				_undefinedUnitAction                  = wa._undefinedUnitAction;
				_meshLandmarkAction                   = wa._meshLandmarkAction;
				_expRoomLandmarkAction                = wa._expRoomLandmarkAction;
				_undefinedLandmarkAction              = wa._undefinedLandmarkAction;
				_mesh2DDimAction                      = wa._mesh2DDimAction;
				_importAction                         = wa._importAction;
				_exportAllAction                      = wa._exportAllAction;
				_exportSelectionAction                = wa._exportSelectionAction;
				_saveMagix3DScriptAction              = wa._saveMagix3DScriptAction;
				_saveAsMagix3DScriptAction            = wa._saveAsMagix3DScriptAction;
				_savePythonConsoleAction              = wa._savePythonConsoleAction;
				_longCommandAction                    = wa._longCommandAction;
				_openCascadeLongCommandAction         = wa._openCascadeLongCommandAction;
				_executePythonScriptAction            = wa._executePythonScriptAction;
				_distanceMeasurementAction            = wa._distanceMeasurementAction;
				_angleMeasurementAction               = wa._angleMeasurementAction;
				_extremaMeshingEdgeLengthOnEdgeAction = wa._extremaMeshingEdgeLengthOnEdgeAction;
				_topoOptimMeshMethod                  = wa._topoOptimMeshMethod;
				_topoInformationAction                = wa._topoInformationAction;
				_meshInformationAction                = wa._meshInformationAction;
				_topoRefineAction                     = wa._topoRefineAction;
				_topoSetDefaultNbMeshingEdgesAction   = wa._topoSetDefaultNbMeshingEdgesAction;
				_topoNew3x3BoxesWithTopoAction        = wa._topoNew3x3BoxesWithTopoAction;
				_meshSelectionAction                  = wa._meshSelectionAction;
				_meshVisibleAction                    = wa._meshVisibleAction;
				_meshAllAction                        = wa._meshAllAction;
				_unrefineMeshRepresentationAction = wa._unrefineMeshRepresentationAction;
				_addMeshExplorerAction         = wa._addMeshExplorerAction;
				_addMeshQualityAction          = wa._addMeshQualityAction;
				_displaySelectedGroupsAction   = wa._displaySelectedGroupsAction;
				_hideSelectedGroupsAction      = wa._hideSelectedGroupsAction;
				_displaySelectedTypesAction    = wa._displaySelectedTypesAction;
				_hideSelectedTypesAction       = wa._hideSelectedTypesAction;
				_displaySelectedEntitiesAction = wa._displaySelectedEntitiesAction;
				_hideSelectedEntitiesAction    = wa._hideSelectedEntitiesAction;
				_displaySelectionPropertiesAction = wa._displaySelectionPropertiesAction;
				_displayComputableSelectionPropertiesAction = wa._displayComputableSelectionPropertiesAction;
				_selectionRepresentationAction              = wa._selectionRepresentationAction;
				_selectEntitiesAction                       = wa._selectEntitiesAction;
				_selectVisibleEntitiesAction                = wa._selectVisibleEntitiesAction;
				_unselectVisibleEntitiesAction              = wa._unselectVisibleEntitiesAction;
				_selectFusableEdgesAction                   = wa._selectFusableEdgesAction;
				_unselectFusableEdgesAction                 = wa._unselectFusableEdgesAction;
				_selectInvalidEdges                         = wa._selectInvalidEdges;
				_unselectInvalidEdges                       = wa._unselectInvalidEdges;
				_selectBorderFacesAction                    = wa._selectBorderFacesAction;
				_unselectBorderFacesAction                  = wa._unselectBorderFacesAction;
				_selectFacesWithoutBlockAction              = wa._selectFacesWithoutBlockAction;
				_unselectFacesWithoutBlockAction = wa._unselectFacesWithoutBlockAction;
				_selectSemiConformFacesAction   = wa._selectSemiConformFacesAction;
				_unselectSemiConformFacesAction = wa._unselectSemiConformFacesAction;
				_selectInvalidFaces             = wa._selectInvalidFaces;
				_unselectInvalidFaces           = wa._unselectInvalidFaces;
				_selectUnstructuredFaces        = wa._selectUnstructuredFaces;
				_unselectUnstructuredFaces      = wa._unselectUnstructuredFaces;
				_selectTransfiniteFaces         = wa._selectTransfiniteFaces;
				_unselectTransfiniteFaces       = wa._unselectTransfiniteFaces;
				_selectInvalidBlocks            = wa._selectInvalidBlocks;
				_unselectInvalidBlocks          = wa._unselectInvalidBlocks;
				_selectUnstructuredBlocks       = wa._selectUnstructuredBlocks;
				_unselectUnstructuredBlocks     = wa._unselectUnstructuredBlocks;
				_selectTransfiniteBlocks        = wa._selectTransfiniteBlocks;
				_unselectTransfiniteBlocks      = wa._unselectTransfiniteBlocks;
				_selectNodesAction              = wa._selectNodesAction;
				_selectEdgesAction              = wa._selectEdgesAction;
				_selectSurfacesAction           = wa._selectSurfacesAction;
				_selectVolumesAction            = wa._selectVolumesAction;
				_selectionModeAction            = wa._selectionModeAction;
				_showCommandMonitorDialogAction = wa._showCommandMonitorDialogAction;
				_displayUsersGuideAction        = wa._displayUsersGuideAction;
				_displayUsersGuideContextAction = wa._displayUsersGuideContextAction;
				_displayWikiAction              = wa._displayWikiAction;
				_displayTutorialAction          = wa._displayTutorialAction;
				_displayPythonAPIUsersGuideAction = wa._displayPythonAPIUsersGuideAction;
				_displayQualifAction     = wa._displayQualifAction;
				_displayShortKeyAction   = wa._displayShortKeyAction;
				_displaySelectAction     = wa._displaySelectAction;
				_displayHistoriqueAction = wa._displayHistoriqueAction;
				_aboutDialogAction       = wa._aboutDialogAction;
				_mgx3DScriptsMenu        = wa._mgx3DScriptsMenu;
				_recentFilesCapacity     = wa._recentFilesCapacity;
				_mgx3DScriptsActions     = wa._mgx3DScriptsActions;
				_undoManager             = wa._undoManager;
			}    // if (&wa != this)

			return *this;
		}    // WindowsActions::operator =


		QtMgx3DMainWindow::WindowsActions::~WindowsActions()
		{
			delete[] _mgx3DScriptsActions;
			_mgx3DScriptsActions = 0;
		}    // WindowsActions::~WindowsActions


		void QtMgx3DMainWindow::WindowsActions::setEnabled(bool enable)
		{
// _longCommandAction toujours active, pour le fun, permet de tester
// à fond le multithreadisme :
//	_longCommandAction->setEnabled (enable);
			// Quit toujours actif :
			// _preferencesAction->setEnabled (enable);
			// _quitAction->setEnabled (enable);
			// _print3DViewAction->setEnabled (enable);
			if (0 != _undoAction)
			{
				if ((0 != _undoManager) && (true == enable))
					_undoAction->setEnabled(0 == _undoManager->undoableCommand() ? false : true);
				else
					_undoAction->setEnabled(false);
			}   // if (0 != _undoAction)
			if (0 != _redoAction)
			{
				if ((0 != _undoManager) && (true == enable))
					_redoAction->setEnabled(0 == _undoManager->redoableCommand() ? false : true);
				else
					_redoAction->setEnabled(enable);
			}   // if (0 != _redoAction)
			if (0 != _clearAction)
				_clearAction->setEnabled(enable);
			if (0 != _mesh2DDimAction)
				_mesh2DDimAction->setEnabled(enable);
			if (0 != _importAction)
				_importAction->setEnabled(enable);
			if (0 != _exportAllAction)
				_exportAllAction->setEnabled(enable);
			if (0 != _exportSelectionAction)
				_exportSelectionAction->setEnabled(enable);
			if (0 != _saveMagix3DScriptAction)
				_saveMagix3DScriptAction->setEnabled(enable);
			if (0 != _saveAsMagix3DScriptAction)
				_saveAsMagix3DScriptAction->setEnabled(enable);
			if (0 != _savePythonConsoleAction)
				_savePythonConsoleAction->setEnabled (enable);
			if (0 != _executePythonScriptAction)
				_executePythonScriptAction->setEnabled(enable);
			if (0 != _mgx3DScriptsMenu)
				_mgx3DScriptsMenu->setEnabled(enable);
			if (0 != _distanceMeasurementAction)
				_distanceMeasurementAction->setEnabled(enable);
			if (0 != _angleMeasurementAction)
				_angleMeasurementAction->setEnabled(enable);
			if (0 != _extremaMeshingEdgeLengthOnEdgeAction)
				_extremaMeshingEdgeLengthOnEdgeAction->setEnabled(enable);
			if (0 != _meshInformationAction)
				_meshInformationAction->setEnabled(enable);
			if (0 != _topoOptimMeshMethod)
				_topoOptimMeshMethod->setEnabled(enable);
			if (0 != _topoInformationAction)
				_topoInformationAction->setEnabled(enable);
			if (0 != _topoRefineAction)
				_topoRefineAction->setEnabled(enable);
			if (0 != _topoSetDefaultNbMeshingEdgesAction)
				_topoSetDefaultNbMeshingEdgesAction->setEnabled(enable);
			if (0 != _topoNew3x3BoxesWithTopoAction)
				_topoNew3x3BoxesWithTopoAction->setEnabled(enable);
			if (0 != _meshSelectionAction)
				_meshSelectionAction->setEnabled(enable);
			if (0 != _meshVisibleAction)
				_meshVisibleAction->setEnabled(enable);
			if (0 != _meshAllAction)
				_meshAllAction->setEnabled(enable);
			if (0 != _unrefineMeshRepresentationAction)
				_unrefineMeshRepresentationAction->setEnabled(enable);
			if (0 != _addMeshExplorerAction)
				_addMeshExplorerAction->setEnabled(enable);
			if (0 != _addMeshQualityAction)
				_addMeshQualityAction->setEnabled(enable);
#ifdef USE_EXPERIMENTAL_ROOM
			if (0 != _loadRaysAction)
				_loadRaysAction->setEnabled(enable);
			if (0 != _importRaysAction)
				_importRaysAction->setEnabled(enable);
			if (0 != _saveRaysAction)
				_saveRaysAction->setEnabled(enable);
			if (0 != _saveAsRaysAction)
				_saveAsRaysAction->setEnabled(enable);
			if (0 != _setRaysContextAction)
				_setRaysContextAction->setEnabled(enable);
			if (0 != _setRaysTargetSurfacesAction)
				_setRaysTargetSurfacesAction->setEnabled(enable);
			if (0 != _loadDiagsAction)
				_loadDiagsAction->setEnabled(enable);
			if (0 != _importDiagsAction)
				_importDiagsAction->setEnabled(enable);
			if (0 != _saveDiagsAction)
				_saveDiagsAction->setEnabled(enable);
			if (0 != _saveAsDiagsAction)
				_saveAsDiagsAction->setEnabled(enable);
#endif	// USE_EXPERIMENTAL_ROOM
		}    // WindowsActions::setEnabled


// ===========================================================================
//                        LA CLASSE QtMgx3DStateView
// ===========================================================================


		unsigned short    QtMgx3DStateView::gaugeSize              = 32;
		unsigned short    QtMgx3DStateView::volatilesMessagesDelay = 5000;
		unsigned short    QtMgx3DStateView::timerDelay             = 500; // en millisecondes


		QtMgx3DStateView::QtMgx3DStateView(const QString &title, QWidget *parent)
				: QWidget(parent), _state(CommandIfc::DONE), _timer(0), _on(false),
				  _label(0), _labelTimer(0)
		{
			setFixedSize(gaugeSize, gaugeSize);
			_timer = new QTimer(this);
			connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
		}    // QtMgx3DStateView::QtMgx3DStateView


		QtMgx3DStateView::QtMgx3DStateView(const QtMgx3DStateView &)
				: QWidget(0), _state(CommandIfc::DONE), _timer(0), _on(false),
				  _label(0), _labelTimer(0)
		{
			MGX_FORBIDDEN("QtMgx3DStateView copy constructor is not allowed.");
		}    // QtMgx3DStateView::QtMgx3DStateView


		QtMgx3DStateView &QtMgx3DStateView::operator=(const QtMgx3DStateView &)
		{
			MGX_FORBIDDEN("QtMgx3DStateView assignment operator is not allowed.");
			return *this;
		}    // QtMgx3DStateView::operator =


		QtMgx3DStateView::~QtMgx3DStateView()
		{
			if (0 != _timer)
				delete _timer;
			_timer = 0;
			if (0 != _labelTimer)
				delete _labelTimer;
			_labelTimer = 0;
		}    // QtMgx3DStateView::~QtMgx3DStateView


		void QtMgx3DStateView::setState(
				const string &cmdName, CommandIfc::status state)
		{
			if (state != _state)
			{
				UTF8String message(Charset::UTF_8);
				message << cmdName << ' ';
				_on    = false;
				_state = state;
				switch (_state)
				{
					case CommandIfc::STARTING        :
					case CommandIfc::PROCESSING        :
						message << "en cours.";
						if (0 != _timer)
							_timer->start(timerDelay);
						break;
					case CommandIfc::INITED            :    // suite undo
					case CommandIfc::FAIL            :
					case CommandIfc::DONE            :
					case CommandIfc::CANCELED        :
						message << CommandIfc::statusToString(_state);
						if (0 != _timer)
							_timer->stop();
						break;
				}    // switch (_state)

				if (0 != _label)
				{
					_label->setText(UTF8TOQSTRING(message));
					_label->resize(_label->sizeHint());
					if (0 != _labelTimer)
						_labelTimer->start(volatilesMessagesDelay);
				}    // if (0 != _label)

// Pb de update : le raffraichissement n'est pas immédiat. Cela peut donner le
// sentiment que tout est fini (indicateur vert) alors que des actions sont en
// cours, surtout si ces actions sont dans le même thread et de durée
// importante.
//		update ( );
				_label->repaint();
				repaint();
			}    // if (state != _state)
		}    // QtMgx3DStateView::setState


		void QtMgx3DStateView::setLabel(QLabel *label)
		{
			if (_label == label)
				return;

			if ((0 != _labelTimer) && (0 != _label))
				disconnect(_labelTimer, SIGNAL(timeout()), _label, SLOT(clear()));
			if ((0 == _labelTimer) && (0 != label))
			{
				_labelTimer = new QTimer(this);
				_labelTimer->setSingleShot(true);
			}    // if ((0 == _labelTimer) && (0 != label))
			_label = label;
			connect(_labelTimer, SIGNAL(timeout()), _label, SLOT(clear()));
			if (0 != _label)
				_label->setAlignment(Qt::AlignLeft);
		}    // QtMgx3DStateView::setLabel


		void QtMgx3DStateView::paintEvent(QPaintEvent *event)
		{
			static const QColor orange(255, 166, 0);
			QColor              background   = Qt::green;
			QSize               dimensions   = size();
			bool                intermittent = false;
			dimensions *= 0.5;
			int x = dimensions.width() / 2, y = dimensions.height() / 2;
			switch (_state)
			{
				case CommandIfc::STARTING        :
				case CommandIfc::PROCESSING        :
					background   = orange;
					intermittent = true;
					break;
				case CommandIfc::FAIL            :
					background = Qt::red;
					break;
				case CommandIfc::DONE            :
				case CommandIfc::CANCELED        :
				case CommandIfc::INITED            :
					background = Qt::green;
					break;
			}    // switch (_state)

			QPainter painter(this);
			painter.save();
			painter.setRenderHint(QPainter::HighQualityAntialiasing);
			if ((false == intermittent) || (false == _on))
			{    // On clignote un appel sur 2 durant processing
				QBrush brush(background, Qt::SolidPattern);
				painter.setBrush(brush);
			}    // if ((false == intermittent) || (false == _on))
			_on = true == intermittent ? !_on : false;
			painter.drawEllipse(x, y, dimensions.width(), dimensions.height());
			painter.setBrush(Qt::NoBrush);
			painter.setPen(Qt::SolidLine);
			painter.setPen(Qt::black);
			painter.drawEllipse(x, y, dimensions.width(), dimensions.height());
			painter.restore();
		}    // QtMgx3DStateView::paintEvent


// ===========================================================================
//                        LA CLASSE QtMgx3DMainWindow
// ===========================================================================


		static string scriptToUserScript(const string &fileName)
		{
			File              file(fileName);
			const string      extension = file.getExtension();
			string::size_type extLength =
					                  0 == extension.length() ? 0 : extension.length() + 1;
			UTF8String        userFileName(Charset::UTF_8);
			userFileName << file.getFullFileName().substr(
					0, file.getFullFileName().length() - extLength);
			userFileName << "_user";
			if (0 != extLength)
				userFileName << '.' << extension;

			return userFileName.ascii();
		}    // scriptToUserScript



// Le type de connection Qt utilisé par défaut.
/* Doc Qt 4.7.4 :
Qt::AutoConnection	0	(default) If the signal is emitted from a different
						thread than the receiving object, the signal is queued,
						behaving as Qt::QueuedConnection. Otherwise, the slot is
						invoked directly, behaving as Qt::DirectConnection. The
						type of connection is determined when the signal is
						emitted.
Qt::DirectConnection	1	The slot is invoked immediately, when the signal is
						emitted.
Qt::QueuedConnection	2	The slot is invoked when control returns to the
						event loop of the receiver's thread. The slot is
						executed in the receiver's thread.
Qt::BlockingQueuedConnection	4	Same as QueuedConnection, except the current
						thread blocks until the slot returns. This connection
						type should only be used where the emitter and receiver
						are in different threads. Note: Violating this rule can
						cause your application to deadlock.
Qt::UniqueConnection	0x80	Same as AutoConnection, but the connection is
						made only if it does not duplicate an existing
						connection. i.e., if the same signal is already
						connected to the same slot for the same pair of objects,
						then the connection will fail. This connection type was
						introduced in Qt 4.6.
Qt::AutoCompatConnection	3	The default type when Qt 3 support is enabled.
						Same as AutoConnection but will also cause warnings to
						be output in certain situations. See Compatibility
						Signals and Slots for further information.
*/
		Qt::ConnectionType    QtMgx3DMainWindow::defaultConnectionType = Qt::QueuedConnection;


		UrlFifo         QtMgx3DMainWindow::_recentScriptsURLFifo(10);    // Pas de valeur lue en configuration
		const string    QtMgx3DMainWindow::_recentScriptsFileName(".magix3d_recent_scripts");

		QtMgx3DMainWindow::QtMgx3DMainWindow(QWidget *parent, char *name, Qt::WindowFlags flags)
				: QMainWindow(parent, flags), ObjectBase(true),
				  SelectionManagerObserver(0),
				  _maxSizeSet(false), _context(0),
				  _groupsPanel(0), _entitiesPanel(0),
				  _graphicalWidget(0), _operationsPanel(0),
				  _additionalPanels(),
				  _projectMenu(0), _sessionMenu(0), _viewMenu(0),
				  _cadMenu(0), _topologyMenu(0), _meshingMenu(0), _selectionMenu(0),
                  _roomMenu(0), _toolsMenu(0), _helpMenu(0), _actionsDisabled(false), _pythonPanel(0),
                  _selectionCommonPropertiesPanel(0),
                  _selectionIndividualPropertiesPanel(0),
                  _logView(0), _statusView(0), _stateView(0), _pythonLogView(0),
                  _entitiesDockWidget(0), _operationsDockWidget(0),
                  _historyDockWidget(0), _selectionCPDockWidget(0),
                  _selectionIPDockWidget(0), _pythonDockWidget(0),
                  _pythonOutputDockWidget(0), _commandMonitorDockWidget(0),
                  _entitiesTabWidget(0),
                  _commandToolbar(0), _3dViewToolbar(0),
                  _meshingToolbar(0), _selectionToolbar(0),
                  _actions(),
                  _appTitle(QtMgx3DApplication::getAppTitle()),
                  _seizureManager(0),
                  _commandMonitorPanel(0),
#ifdef USE_EXPERIMENTAL_ROOM
                  _experimentalRoomPanel(0),
#endif	// USE_EXPERIMENTAL_ROOM
                  _repTypesDialog(0),
                  _pythonMinScript(), _pytMinScriptCharset(Charset::ASCII),
                  _encodageScripts((ContextIfc::encodageScripts) - 1)
		{
			setObjectName(0 == name ? "QtMgx3DMainWindow" : name);
		}    // QtMgx3DMainWindow::QtMgx3DMainWindow


		QtMgx3DMainWindow::QtMgx3DMainWindow(const QtMgx3DMainWindow &)
				: QMainWindow(0), ObjectBase(true),
				  SelectionManagerObserver(0),
				  _maxSizeSet(false), _context(0),
				  _groupsPanel(0), _entitiesPanel(0),
				  _graphicalWidget(0), _operationsPanel(0),
				  _additionalPanels(),
				  _projectMenu(0), _sessionMenu(0), _viewMenu(0),
				  _cadMenu(0), _topologyMenu(0), _meshingMenu(0), _selectionMenu(0),
                  _roomMenu(0), _toolsMenu(0), _helpMenu(0), _actionsDisabled(false), _pythonPanel(0),
                  _selectionCommonPropertiesPanel(0),
                  _selectionIndividualPropertiesPanel(0),
                  _logView(0), _statusView(0), _stateView(0), _pythonLogView(0),
                  _entitiesDockWidget(0), _operationsDockWidget(0),
                  _historyDockWidget(0), _selectionCPDockWidget(0),
                  _selectionIPDockWidget(0), _pythonDockWidget(0),
                  _pythonOutputDockWidget(0), _commandMonitorDockWidget(0),
                  _entitiesTabWidget(0),
                  _commandToolbar(0), _3dViewToolbar(0),
                  _meshingToolbar(0), _selectionToolbar(0),
                  _actions(),
                  _appTitle(QtMgx3DApplication::getAppTitle()),
                  _seizureManager(0),
                  _commandMonitorPanel(0),
#ifdef USE_EXPERIMENTAL_ROOM
                  _experimentalRoomPanel(0),
#endif	// USE_EXPERIMENTAL_ROOM
                  _repTypesDialog(0),
                  _pythonMinScript(), _pytMinScriptCharset(Charset::UNKNOWN),
                  _encodageScripts((ContextIfc::encodageScripts) - 1)
		{
			MGX_FORBIDDEN("QtMgx3DMainWindow copy constructor is not allowed.");
		}    // QtMgx3DMainWindow::QtMgx3DMainWindow (const QtMgx3DMainWindow&)


		QtMgx3DMainWindow &QtMgx3DMainWindow::operator=(const QtMgx3DMainWindow &)
		{
			MGX_FORBIDDEN("QtMgx3DMainWindow assignment operator is not allowed.");
			return *this;
		}    // QtMgx3DMainWindow::QtMgx3DMainWindow (const QtMgx3DMainWindow&)


		QtMgx3DMainWindow::~QtMgx3DMainWindow()
		{
			LOCK_INSTANCE

			BEGIN_QT_TRY_CATCH_BLOCK

			getActions()._undoManager = 0;
			getActions().setEnabled(false);

			getContext().getSelectionManager().clearSelection();
			if (0 != _seizureManager)
				_seizureManager->setInteractiveMode(false);
			_seizureManager = 0;
			unregisterReferences();

			for (vector<QtMgx3DOperationPanel *>::iterator ita =
					                                               _additionalPanels.begin();
			     _additionalPanels.end() != ita; ita++)
				(*ita)->setMainWindow(0);
			_additionalPanels.clear();

			if (0 != _commandMonitorPanel)
				_commandMonitorPanel->release();
//	delete _commandMonitorDialog;	_commandMonitorDialog	= 0;

			delete _repTypesDialog;
			_repTypesDialog = 0;

#ifdef _DEBUG

																																	// En mode release l'application quitte sans passer par le
	// destructeur de l'IHM => tout se passe bien.
	// En mode debug l'objectif est de vérifier que tout se passe
	// proprement.
	// La destruction des managers n'informe pas l'IHM (items, ...)
	// des destructions effectuées via un dispositif type callback
	// car c'est fait en temps normal via les InfoCommands
	// => l'IHM risque de vouloir lire des informations détruites
	// lors de sa destruction.
	// => On désactive ici la destruction d'éléments sensibles de
	// l'IHM.
	if (0 != _groupsPanel)
	{
		_groupsPanel->setVisible (false);
		_groupsPanel->setParent (0);
	}	// if (0 != _groupsPanel)
	if (0 != _entitiesPanel)
	{
		_entitiesPanel->setVisible (false);
		_entitiesPanel->setParent (0);
	}	// if (0 != _entitiesPanel)
	if (0 != _graphicalWidget)
	{
		_graphicalWidget->setVisible (false);
		_graphicalWidget->setParent (0);
	}	// if (0 != _graphicalWidget)
	if (0 != _operationsPanel)
	{
		_operationsPanel->setVisible (false);
		_operationsPanel->setParent (0);
	}	// if (0 != _operationsPanel)

	// Destruction du contexte actuel :
	setContext (0);

#endif    //#ifdef _DEBUG

			COMPLETE_QT_TRY_CATCH_BLOCK(false, this, getAppTitle())

			if (true == hasError)
				cerr << "Echec lors de la destruction de la fenêtre principale : " << endl
				     << errorString << endl;
		}    // QtMgx3DMainWindow::~QtMgx3DMainWindow
		
void QtMgx3DMainWindow::showReady ( )
{
	show( );	// => Initialise si nécessaire Open GL
	
	getGraphicalWidget ( ).getRenderingManager ( ).setDisplayTrihedron (Resources::instance ( )._displayTrihedron.getValue ( ));
	getGraphicalWidget ( ).getRenderingManager ( ).setDisplayFocalPoint (Resources::instance ( )._displayFocalPoint.getValue ( ));
}	// QtMgx3DMainWindow::showReady


		void QtMgx3DMainWindow::init(const std::string &name, ContextIfc *context, QtGroupsPanel *groupsPanel, QtEntitiesPanel *entitiesPanel)
		{
			if ((0 != _groupsPanel) || (0 != _entitiesPanel))
				throw Exception("QtMgx3DMainWindow::init : fenêtre déjà initialisée.");

			_groupsPanel   = groupsPanel;
			_entitiesPanel = entitiesPanel;
#ifdef _DEBUG

																																	// [EB] en mode release ce serait utile pour permettre la fermeture de la fenêtre d'aide par exemple lorsque l'utilisateur ferme l'application (avec un close), mais cela plante dans ce cas...

	// En mode debug on veut que close provoque la destruction de
	// la fenêtre pour passer par le destructeur et quitter
	// "proprement". (on ne rigole pas !)
	setAttribute (Qt::WA_DeleteOnClose, true);

#endif    // #ifdef _DEBUG

			if (0 == context)
				context = new Internal::Context(name, true);
			setContext(context);

			createGui();

			// sauvegarde du script dans un fichier généré automatiquement
			try
			{
				_pytMinScriptCharset = getDefaultScriptsCharset().charset();
				getContext().getScriptingManager().setCharset(_pytMinScriptCharset);
				getContext().getScriptingManager().initPython(getContext().newScriptingFileName());

				assert(0 != _pythonPanel);
			}
			catch (...)
			{
			}

			// On fait un fichier script utilisateur, qui contient les commandes saisies
			// par l'utilisateur et non celles générés par le noyau à partir des
			// commandes utilisateur :
			try
			{
				CHECK_NULL_PTR_ERROR(_pythonPanel)
				CHECK_NULL_PTR_ERROR(getContext().getScriptingManager().getPythonWriter())
				PythonWriter*	refWriter	= getContext ( ).getScriptingManager ( ).getPythonWriter ( );
				PythonWriter*	writer	= new Internal::PythonWriter (scriptToUserScript (refWriter->getFileName ( )), refWriter->getCharset ( ));
				writer->writeHeader();
				_pythonPanel->getLogDispatcher ( ).addStream (writer);
			}
			catch (...)
			{
			}

			// titre de la fenêtre principale
			UTF8String titre(Charset::UTF_8);
			titre << Resources::instance()._softwareName << " (" << MAGIX3D_VERSION /*GSCC_PROJECT_VERSION*/<< ")";
			setWindowTitle(UTF8TOQSTRING(titre));

			initPythonScripting();

			// Restauration des paramtètres de l'IHM de la session précédante :
			readSettings();

			// Réactivation éventuelle des actions toutes les 500 msecondes.
			// Utilise le système de timers de la classe QObject.
			startTimer(500);
		}    // QtMgx3DMainWindow::init


		void QtMgx3DMainWindow::writeSettings()
		{
			QSettings settings(Resources::instance()._organisation.c_str(),
			                   Resources::instance()._softwareName.c_str());
			File      file(settings.fileName().toStdString());
			File      dir(file.getPath());
			if (false == dir.exists())
				dir.create(false);

			settings.beginGroup("MainWindow");

			settings.setValue("geometry", geometry());

			// L'état de cette fenêtre (toolbars/dock windows).
			// Qt utilise la propriété objectName.
			settings.setValue("state", saveState());

			if (0 != _groupsPanel)
				_groupsPanel->writeSettings(settings);
			if (0 != _entitiesPanel)
				_entitiesPanel->writeSettings(settings);
			if (0 != _graphicalWidget)
				_graphicalWidget->writeSettings(settings);
			if (0 != _operationsPanel)
				_operationsPanel->writeSettings(settings);
			if (0 != _pythonPanel)
				_pythonPanel->writeSettings(settings);
			if (0 != _logView)
				_logView->writeSettings(settings);
			if (0 != _selectionCommonPropertiesPanel)
				_selectionCommonPropertiesPanel->writeSettings(settings);
			if (0 != _selectionIndividualPropertiesPanel)
				_selectionIndividualPropertiesPanel->writeSettings(settings);

			settings.endGroup();
		}    // QtMgx3DMainWindow::writeSettings


		void QtMgx3DMainWindow::readSettings()
		{
			if (false == Resources::instance()._loadGuiState.getValue())
				return;

			QSettings settings(Resources::instance()._organisation.c_str(),
			                   Resources::instance()._softwareName.c_str());

			settings.beginGroup("MainWindow");

			setGeometry(settings.value("geometry").toRect());

			// L'état (toolbars/docks widgets) de cette fenêtre :
			restoreState(settings.value("state").toByteArray());

			if (0 != _groupsPanel)
				_groupsPanel->readSettings(settings);
			if (0 != _entitiesPanel)
				_entitiesPanel->readSettings(settings);
			if (0 != _graphicalWidget)
				_graphicalWidget->readSettings(settings);
			if (0 != _operationsPanel)
				_operationsPanel->readSettings(settings);
			if (0 != _pythonPanel)
				_pythonPanel->readSettings(settings);
			if (0 != _logView)
				_logView->readSettings(settings);
			if (0 != _selectionCommonPropertiesPanel)
				_selectionCommonPropertiesPanel->readSettings(settings);
			if (0 != _selectionIndividualPropertiesPanel)
				_selectionIndividualPropertiesPanel->readSettings(settings);

			settings.endGroup();
		}    // QtMgx3DMainWindow::readSettings


		QtRepresentationTypesDialog *QtMgx3DMainWindow::createRepresentationTypesDialog(
				const string &name, const vector<Entity *> &entities,
				const string &helpURL, const string &helpTag)
		{
			return new QtRepresentationTypesDialog(
					this, name, getAppTitle(), entities, helpURL, helpTag);
		}    // QtMgx3DMainWindow::createRepresentationTypesDialog ( )


		void QtMgx3DMainWindow::setGraphicalWidget(Qt3DGraphicalWidget &widget)
		{
			if (0 != _graphicalWidget)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::setGraphicalWidget",
				               "Widget graphique déjà affecté.")
				throw exc;
			}    // if (0 != _graphicalWidget)

			_graphicalWidget = &widget;
			if (0 != _graphicalWidget->getRenderingWidget())
			{
				QSize size(Resources::instance()._graphicalWindowWidth, Resources::instance()._graphicalWindowHeight);
				if (false == Resources::instance()._graphicalWindowFixedSize.getValue())
				{
					_graphicalWidget->getRenderingWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
// Commit 1963 : on n'impose plus de taille minimum, pose des problèmes avec
// les petits écrans.
// 1963			_graphicalWidget->getRenderingWidget ( )->setMinimumSize (size);
				} else
					_graphicalWidget->getRenderingWidget()->setFixedSize(size);
			}    // if (0 !=  _graphicalWidget->getRenderingWidget ( ))
			_graphicalWidget->updateConfiguration();
			if (0 != getActions()._useGlobalDisplayPropertiesAction)
				_graphicalWidget->getRenderingManager().useGlobalDisplayProperties(getActions()._useGlobalDisplayPropertiesAction->isChecked());
			setCentralWidget(_graphicalWidget);
			CHECK_NULL_PTR_ERROR(_entitiesPanel)
			_entitiesPanel->setGraphicalWidget(_graphicalWidget);
			CHECK_NULL_PTR_ERROR(_selectionIndividualPropertiesPanel)
			_selectionIndividualPropertiesPanel->setGraphicalWidget(_graphicalWidget);
			if (0 != _pythonPanel)
				_pythonPanel->setGraphicalWidget(&widget);
			Context *context = dynamic_cast<Context *>(&getContext());
			if (0 != context)
				_graphicalWidget->getRenderingManager().setContext(context);
			// Pour _groupsPanel->setGraphicalWidget (...) il faut que le contexte soit affecté au rendering manager :
			CHECK_NULL_PTR_ERROR(_groupsPanel)
			_groupsPanel->setGraphicalWidget(_graphicalWidget);
		}    // QtMgx3DMainWindow::setGraphicalWidget


		Qt3DGraphicalWidget &QtMgx3DMainWindow::getGraphicalWidget()
		{
			if (0 == _graphicalWidget)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getGraphicalWidget",
				               "Widget graphique non affecté.")
				throw exc;
			}    // if (0 == _graphicalWidget)

			return *_graphicalWidget;
		}    // QtMgx3DMainWindow::getGraphicalWidget


		void QtMgx3DMainWindow::changeRepresentationTypes(
				const vector<Entity *> &entities)
		{
			BEGIN_QT_TRY_CATCH_BLOCK

			LOCK_INSTANCE

			if (0 != _repTypesDialog)
			{
				_repTypesDialog->raise();
				return;
			}    // if (0 != _repTypesDialog)

			// Verrouiller les opérations d'affichage (optimisation) :
			RenderingManager::DisplayLocker displayLocker(
					getGraphicalWidget().getRenderingManager());
			// Et actions : des entités en cours de modifications pourraient être
			// détruites.
			disableActions(true);

			_repTypesDialog = createRepresentationTypesDialog(
					"Types de représentation 3D", entities,
					QtMgx3DApplication::HelpSystem::instance().representationURL,
					QtMgx3DApplication::HelpSystem::instance().representationTag
			);
			connect(_repTypesDialog, SIGNAL(finished(int)), this,
			        SLOT(representationTypesDialogClosedCallback()));
			_repTypesDialog->show();

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::changeRepresentationTypes


		vector <QtAboutDialog::Component> QtMgx3DMainWindow::getAboutDlgComponents() const
		{
			// on vire le nom Lima du numéro de la version
			std::string lima_version(Lima::lima_version());
			if (lima_version.find("Lima.") == 0)
				lima_version = lima_version.substr(5, lima_version.length());

			vector <QtAboutDialog::Component> components;

			// pour l'IHM
			components.push_back(
					QtAboutDialog::Component("Qt", QT_VERSION_STR, UTF8String("Fenêtrage pour l'interface graphique", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("QtUtil", QtUtilInfos::getVersion().getVersion(), UTF8String("divers utilitaires IHM", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("Preferences", QtPreferencesInfos::getVersion().getVersion(), UTF8String("Panneau préférences utilisateur", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("QtQualif", GQualif::QtQualifServices::getVersion().getVersion(), UTF8String("Panneau pour Qualif", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("QwtCharts", QwtChartsManager::getVersion().getVersion(), UTF8String("Histogrammes du panneau qualité", Charset::UTF_8), ""));


			// Divers [à réordonner]
			components.push_back(
					QtAboutDialog::Component("TKUtil", UtilInfos::getVersion().getVersion(), UTF8String("Divers utilitaires", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("Xerces", XERCES_FULLVERSIONDOT, UTF8String("Lecteur/Ecrivain XML", Charset::UTF_8), ""));

			// Pythoneries
			components.push_back(
					QtAboutDialog::Component("Python", PY_VERSION, UTF8String("Interpreteur et API Python", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("Swig", SWIG_VERSION, UTF8String("Interface Python/C++", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("QtPython", QtPython::getVersion().getVersion(), UTF8String("Console Python", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("PythonUtil", PythonInfos::getVersion().getVersion(), UTF8String("Utilitaires pour commandes python", Charset::UTF_8), ""));

			// Maillage
			components.push_back(
					QtAboutDialog::Component("Lima++", lima_version, UTF8String("Ecrivain maillage", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("CGNS", UTF8String(CGNS_DOTVERS), UTF8String("Format maillage particulier", Charset::UTF_8), ""));
			//components.push_back(
			//		QtAboutDialog::Component("HDF5", Lima::MaliPPReader::lireVersionHDF().getVersion(), UTF8String("Ecrivain pour le format mli", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("Gepeto", "???", UTF8String("Utilitaires géométriques", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("GMDS", GMDS_VERSION, UTF8String("Structure de maillage", Charset::UTF_8), ""));
#ifdef USE_TRITON
			//components.push_back (
			//		QtAboutDialog::Component ("Triton", TRITON_VERSION, UTF8String ("Mailleur triangulaire", Charset::UTF_8), ""));
#else
			components.push_back(
					QtAboutDialog::Component("Triton", UTF8String("Déconnecté", Charset::UTF_8), UTF8String("Mailleur triangulaire", Charset::UTF_8), ""));
#endif
#ifdef USE_CAGE
																																	components.push_back (
		QtAboutDialog::Component ("CaGe", CAGE_VERSION, UTF8String ("Capture géométrique", Charset::UTF_8), ""));
#endif    // USE_CAGE
			components.push_back(
					QtAboutDialog::Component("NetGen", "???", UTF8String("Mailleur non structuré", Charset::UTF_8), ""));
			components.push_back(
					QtAboutDialog::Component("Mesquite", MSQ_VERSION, UTF8String("Lisseur", Charset::UTF_8), ""));
#ifdef USE_SMOOTH3D
																																	components.push_back (
		QtAboutDialog::Component ("Smooth3D", SMOOTH3D_VERSION, UTF8String ("Lisseur", Charset::UTF_8), ""));
#endif    // USE_SMOOTH3D

			// Modélisation
#ifdef USE_MDLPARSER			
			components.push_back(
					QtAboutDialog::Component("MDL", MDL_VERSION, UTF8String("Ecrivain format modélisation MDL", Charset::UTF_8), ""));
#else	// USE_MDLPARSER
			components.push_back(
					QtAboutDialog::Component("MDL", UTF8String("Déconnecté ", Charset::UTF_8), UTF8String("Ecrivain format modélisation MDL", Charset::UTF_8), ""));
#endif	// USE_MDLPARSER
#ifdef USE_DKOC
																																	components.push_back (
		QtAboutDialog::Component ("DKOC", DKOC_VERSION, UTF8String ("Lecteur Catia (Datakit)", Charset::UTF_8), ""));
#else
			components.push_back(
					QtAboutDialog::Component("DKOC", UTF8String("Déconnecté ", Charset::UTF_8), UTF8String("Lecteur Catia", Charset::UTF_8), ""));
#endif
			components.push_back(
					QtAboutDialog::Component("OCC", OCC_VERSION_COMPLETE, UTF8String("Moteur géométrique OpenCascade", Charset::UTF_8), ""));
#ifdef USE_SEPA3D
																																	components.push_back (
		QtAboutDialog::Component ("Sepa3D", SEPA3D_VERSION, UTF8String ("Surcouche pour gestion des séparatrices 3D", Charset::UTF_8), ""));
#endif    // USE_SEPA3D

			return components;
		}    // QtMgx3DMainWindow::getAboutDlgComponents


		void QtMgx3DMainWindow::observableModified(
				ReferencedObject *object, unsigned long event)
		{
			if (QThread::currentThread() != thread())
			{
				// Commande non séquentielle par exemple => passer au thread de l'IHM
				// pour actualisation de l'IHM :
				emit nonGuiThreadObservableModified(object, event);
				return;
			}    // if (QThread::currentThread ( ) != ...
//	QtAutoWaitingCursor	cursor (true);

			Command *command = dynamic_cast<Command *>(object);
			if (0 != command)
			{
				LOCK_INSTANCE

				if (UTIL_UNAVAILABLE_EVENT == event)
				{
					observableDeleted(object);
					_actions.setEnabled(true);
					if (0 != _pythonPanel)
						_pythonPanel->setUsabled(true);
				} else
					commandModified(*command, event);
			}    // if (0 != command)
			else
				if (COMMAND_STACK == event)
					updateActions();
		}    // QtMgx3DMainWindow::observableModified


		void QtMgx3DMainWindow::createGui()
		{
			BEGIN_QT_TRY_CATCH_BLOCK

			if (0 != _graphicalWidget)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::createGui", "IHM déjà créée.")
				throw exc;
			}    // if (0 != _graphicalWidget)

			setDockNestingEnabled(true);

			// Entités/Groupes :
			_entitiesDockWidget = new QDockWidget(QString::fromUtf8("Gestionnaire d'Entités"), this);
			_entitiesDockWidget->setObjectName("Entities manager panel");
			_entitiesTabWidget = new QTabWidget(this);
			if (0 == _groupsPanel)
				_groupsPanel   = new QtGroupsPanel(
						_entitiesTabWidget, this, "", getContext());
			else
				_groupsPanel->setMainWindow(this);
			_entitiesTabWidget->addTab(_groupsPanel, "Groupes");
//	getContext ( ).getGroupManager ( ).setPropagate (
//						_groupsPanel->groupsRepresentationPropagation ( ));
			if (0 == _entitiesPanel)
				_entitiesPanel = new QtEntitiesPanel(
						_entitiesTabWidget, this, "", getContext());
			else
				_entitiesPanel->setMainWindow(this);
			_entitiesTabWidget->addTab(_entitiesPanel, "Entités");
			_entitiesPanel->setLogStream(&getLogDispatcher());
			_entitiesDockWidget->setWidget(_entitiesTabWidget);
			addDockWidget(Qt::RightDockWidgetArea, _entitiesDockWidget);
			_entitiesDockWidget->setSizePolicy(
					QSizePolicy::Preferred, QSizePolicy::Preferred);

			// Opérations :
			_operationsDockWidget = new QDockWidget(QString::fromUtf8("Opérations"), this);
			_operationsDockWidget->setObjectName("Operations panel");
			if (0 == _operationsPanel)
				_operationsPanel = new QtMgx3DOperationsPanel(
						this, "", *this, getAppTitle());
			_operationsPanel->createGui();
			_operationsPanel->setLogStream(&getLogDispatcher());
			_operationsDockWidget->setWidget(_operationsPanel);
			addDockWidget(Qt::LeftDockWidgetArea, _operationsDockWidget);
//	operationsDockWidget->setSizePolicy (
//						QSizePolicy::Preferred, QSizePolicy::Preferred);
//						QSizePolicy::Minimum, QSizePolicy::Minimum);
			// Avoir entités + opérations sur 2 colonnes de la même zone, a priori à
			// droite :
			splitDockWidget(_entitiesDockWidget, _operationsDockWidget, Qt::Horizontal);

			// Le déroulement de la session :
			_historyDockWidget = new QDockWidget("Historique", this);
			_historyDockWidget->setObjectName("Historic panel");
			_logView = new QtMgx3DLogsView(this, Context::getLogsMask());
			_logView->enableDate(
					Resources::instance()._logDate, Resources::instance()._logTime);
			_logView->enableThreadID(Resources::instance()._logThreadID);
			_historyDockWidget->setWidget(_logView);
			getLogDispatcher().addStream(_logView);
			_logView->setMinimumSize(QSize(300, 100));
			addDockWidget(Qt::BottomDockWidgetArea, _historyDockWidget);
			_historyDockWidget->setSizePolicy(
					QSizePolicy::Preferred, QSizePolicy::Preferred);
			_statusView =
					new QtStatusLogOutputStream(statusBar(), Context::getLogsMask());
			_statusView->enableDate(
					Resources::instance()._logDate, Resources::instance()._logTime);
			_statusView->enableThreadID(Resources::instance()._logThreadID);
			QLabel *label = new QLabel("", this);
			statusBar()->addPermanentWidget(label);
			_stateView = new QtMgx3DStateView("Etat de l'IHM", this);
			_stateView->setLabel(label);
			statusBar()->addPermanentWidget(_stateView);
			getLogDispatcher().addStream(_statusView);

			// Les propriétés communes de la sélection :
			_selectionCPDockWidget =
					new QDockWidget(QString::fromUtf8("Propriétés communes de la sélection"), this);
			_selectionCPDockWidget->setObjectName("Common properties panel");
			_selectionCommonPropertiesPanel =
					new QtSelectionCommonPropertiesPanel(
							this, "", &getContext().getSelectionManager());
			_selectionCPDockWidget->setSizePolicy(
					QSizePolicy::Preferred, QSizePolicy::Preferred);
			_selectionCPDockWidget->setWidget(_selectionCommonPropertiesPanel);
			addDockWidget(Qt::BottomDockWidgetArea, _selectionCPDockWidget);

			// Les propriétés individuelles de la sélection :
			_selectionIPDockWidget =
					new QDockWidget(QString::fromUtf8("Propriétés individuelles de la sélection"), this);
			_selectionIPDockWidget->setObjectName("Individual properties panel");
			_selectionIndividualPropertiesPanel =
					new QtSelectionIndividualPropertiesPanel(
							this, "", &getContext().getSelectionManager());
			_selectionIPDockWidget->setSizePolicy(
					QSizePolicy::Preferred, QSizePolicy::Preferred);
			_selectionIPDockWidget->setWidget(_selectionIndividualPropertiesPanel);
			addDockWidget(Qt::BottomDockWidgetArea, _selectionIPDockWidget);

			// Le gestionnaire de commandes :
			// ATTENTION : on marche sur des oeufs ...
			_commandMonitorDockWidget =
					new QDockWidget("Moniteur de commandes", this);
			_commandMonitorDockWidget->setObjectName("Commands monitor panel");
			CommandManager *cmdMgr =
					               dynamic_cast<CommandManager *>(&getCommandManager());
			CHECK_NULL_PTR_ERROR(cmdMgr)
			_commandMonitorPanel = new QtCommandMonitorPanel(this, cmdMgr);
			_commandMonitorDockWidget->setSizePolicy(
					QSizePolicy::Preferred, QSizePolicy::Preferred);
			_commandMonitorDockWidget->setWidget(_commandMonitorPanel);
			addDockWidget(Qt::BottomDockWidgetArea, _commandMonitorDockWidget);

			// Exécution de commandes python :
			_pythonDockWidget = new QDockWidget("Commandes python", this);
			_pythonDockWidget->setObjectName("Python panel");
			_pythonPanel	= new QtMgx3DPythonConsole (this, this, "Magix 3D");
			QtDecoratedPythonConsole*	console	= new QtDecoratedPythonConsole (this, *_pythonPanel, Qt::LeftToolBarArea);
			_pythonDockWidget->setWidget (console);
			addDockWidget(Qt::BottomDockWidgetArea, _pythonDockWidget);
			_pythonDockWidget->setSizePolicy(
					QSizePolicy::Preferred, QSizePolicy::Preferred);
			_pythonPanel->setLogStream(&getLogDispatcher());
			// Les logs python (sortie standard/stderr en mode scripting) :
			_pythonLogView = new QtLogsView(this, Log::PROCESS);
			_pythonLogView->enableDate(false, false);
			_pythonLogView->enableThreadID(false);
			_pythonOutputDockWidget = new QDockWidget("Sorties python", this);
			_pythonOutputDockWidget->setObjectName("Python outputs");
			_pythonOutputDockWidget->setWidget(_pythonLogView);
			_pythonPanel->getLogDispatcher ( ).addStream (_pythonLogView);
			addDockWidget(Qt::BottomDockWidgetArea, _pythonOutputDockWidget);

			// Historique au premier plan :
			tabifyDockWidget(_pythonOutputDockWidget, _pythonDockWidget);
			tabifyDockWidget(_selectionCPDockWidget, _pythonDockWidget);
			tabifyDockWidget(_pythonDockWidget, _selectionIPDockWidget);
			tabifyDockWidget(_commandMonitorDockWidget, _selectionIPDockWidget);
			tabifyDockWidget(_selectionIPDockWidget, _historyDockWidget);

			// Actions et menus :
			createActions();
			createOperations();
			createMenu();
			createToolbars();
			updateActions();

			// Synchro entre composants :
			displaySelectedEntitiesPropertiesCallback();
			displaySelectedEntitiesComputablePropertiesCallback();

			// Aspects multithreading :
			connect(this, SIGNAL(nonGuiThreadObservableModified(
					TkUtil::ReferencedObject * , unsigned long)), this,
			        SLOT(observableModifiedCallback(
					        TkUtil::ReferencedObject * , unsigned long)),
			        defaultConnectionType);

			// Par défaut la fenêtre "Entités" à tendance à être 2 fois moins large
			// que souhaitée. On l'initialise à la bonne largeur :
//QLayout*	l	= layout ( );
//assert (0 != l);
			QDesktopWidget *desktopWidget = QApplication::desktop();
			if (0 != desktopWidget)
			{    // On empêche ici la fenêtre Magix 3D de devenir plus grande que l'écran
				// Pb : dans certains cas la main window atteint cette taille mais, ne
				// pouvant empieter sur la barre d'outils système, le bas de la fenêtre
				// est hors écran ... Comment l'empêcher ???
				// Un setMaximumSize est sans effet, le layout prend le dessus et
				// utilise son getMaximumSize à lui.
				// Seul  layout ( )->setSizeConstraint (QLayout::SetFixedSize) semble
				// efficace, mais il interdit tout redimensionnement de la fenêtre
				// principale. Pas top.
//		const QRect	geom	= desktopWidget->screenGeometry (this);
				const QRect geom = desktopWidget->availableGeometry(this);
				cout << "TAILLE DE L'ECRAN UTILISE : " << geom.width() << "x" << geom.height() << endl;
				QSize tbSize;
				if (0 != _commandToolbar)
					tbSize = _commandToolbar->sizeHint();
				int   border = tbSize.width() < tbSize.height() ?
				               tbSize.width() : tbSize.height();
				QSize size(geom.size().width() - 2 * border,
				           geom.size().height() - 2 * border);
// commit 1963 : on attend le premier évènement ResizeEvent avec géométrie
// calculée de la fenêtre (y compris barre de déplacement/bordures) pour
// calculer la taille max de la zone de travail que l'on fixe via setMaximumSize
// => fait dans resizeEvent
// 1963		setMaximumSize (size);
//		cout << "Taille de la fenêtre Magix 3D limitée à "
//		     << size.width ( ) << "x" << size.height ( ) << endl;
			}    // if (0 != desktopWidget)
			QSize          size           = _entitiesDockWidget->sizeHint();
			_entitiesDockWidget->setMinimumWidth(size.width());

//	_logView->setLogMask (Context::getLogsMask ( ));
//	_logView->setLogMask (Log::PRODUCTION | Log::TRACE_1);
//	getLogDispatcher( ).setMask (Log::ALL_KINDS);

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::createGui


		void QtMgx3DMainWindow::createMenu()
		{
			BEGIN_QT_TRY_CATCH_BLOCK

			// On s'assure ici que createActions ait bien été appelé :
			CHECK_NULL_PTR_ERROR(getActions()._quitAction)

			QMenuBar *menubar = menuBar();
			CHECK_NULL_PTR_ERROR(menubar)

			// Menu Projet :
			_projectMenu = new QMenu("&Projet", menubar);
			menubar->addMenu(_projectMenu);
#ifndef QT_4
			_projectMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_projectMenu->addAction(getActions()._preferencesAction);
			_projectMenu->addAction(getActions()._editSettingsAction);
			_projectMenu->addSeparator();
			_projectMenu->addAction(getActions()._quitAction);

			// Menu Session :
			_sessionMenu = new QMenu("&Session", menubar);
			menubar->addMenu(_sessionMenu);
#ifndef QT_4
			_sessionMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_sessionMenu->addAction(getActions()._undoAction);
			_sessionMenu->addAction(getActions()._redoAction);
			_sessionMenu->addSeparator();
			_sessionMenu->addAction(getActions()._clearAction);
			_sessionMenu->addSeparator();
			QMenu *unitMenu = new QMenu("Unité de longueur", _sessionMenu);
			_sessionMenu->addMenu(unitMenu);
#ifndef QT_4
			unitMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			unitMenu->addAction(getActions()._meterAction);
			unitMenu->addAction(getActions()._centimeterAction);
			unitMenu->addAction(getActions()._millimeterAction);
			unitMenu->addAction(getActions()._micrometerAction);
			unitMenu->addAction(getActions()._undefinedUnitAction);
			QMenu *landmarkMenu = new QMenu(QString::fromUtf8("Repère"), _sessionMenu);
			_sessionMenu->addMenu(landmarkMenu);
#ifndef QT_4
			landmarkMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			landmarkMenu->addAction(getActions()._meshLandmarkAction);
			landmarkMenu->addAction(getActions()._expRoomLandmarkAction);
			landmarkMenu->addAction(getActions()._undefinedLandmarkAction);
			_sessionMenu->addAction(getActions()._mesh2DDimAction);
			_sessionMenu->addSeparator();
			_sessionMenu->addAction(getActions()._importAction);
			_sessionMenu->addAction(getActions()._exportAllAction);
			_sessionMenu->addAction(getActions()._exportSelectionAction);
			_sessionMenu->addAction(getActions()._saveMagix3DScriptAction);
			_sessionMenu->addAction(getActions()._saveAsMagix3DScriptAction);
			_sessionMenu->addAction(getActions()._savePythonConsoleAction);
			_sessionMenu->addSeparator();
#ifdef _DEBUG
																																	_sessionMenu->addAction (getActions ( )._longCommandAction);
	_sessionMenu->addAction (getActions ( )._openCascadeLongCommandAction);
	_sessionMenu->addSeparator ( );
#endif
			_sessionMenu->addAction(getActions()._executePythonScriptAction);
			getActions()._mgx3DScriptsMenu =
					new QMenu(QString::fromUtf8("Scripts Magix 3D récents ..."), _sessionMenu);
#ifndef QT_4
			getActions()._mgx3DScriptsMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_sessionMenu->addMenu(getActions()._mgx3DScriptsMenu);
			UTF8String recentFileName(Charset::UTF_8);
			recentFileName << UserData().getHome() << "/" << _recentScriptsFileName;
			try
			{
				for (size_t i = 0; i < getActions()._recentFilesCapacity; i++)
				{
					CHECK_NULL_PTR_ERROR(getActions()._mgx3DScriptsMenu)
					getActions()._mgx3DScriptsActions[i] =
							new QAction("", getActions()._mgx3DScriptsMenu);
					connect(getActions()._mgx3DScriptsActions[i],
					        SIGNAL(triggered()), this,
					        SLOT(executeRecentMgx3DScriptCallback()));
				}    // for (size_t i = 0; i < getActions ( )._recentFilesCapacity; i++)

				_recentScriptsURLFifo.load(recentFileName);
				for (size_t i = 0; i < _recentScriptsURLFifo.count(); i++)
				{
					if (i >= getActions()._recentFilesCapacity)
						break;

					getActions()._mgx3DScriptsMenu->addAction(
							getActions()._mgx3DScriptsActions[i]);
					getActions()._mgx3DScriptsActions[i]->setText(
							_recentScriptsURLFifo.url(i).c_str());
				}    // for (size_t i = 0; i < _recentScriptsURLFifo.count ( ); i++)
			}
			catch (const Exception &exc)
			{
				cerr << "Impossibilité de charger les noms de scripts Magix 3D récemment "
				     << "exécutés depuis le fichier " << recentFileName << " : "
				     << exc.getFullMessage() << endl;
			}
			catch (...)
			{
				cerr << "Impossibilité de charger les noms de scripts Magix 3D récemment "
				     << "exécutés depuis le fichier " << recentFileName << " : "
				     << "erreur non documentée." << endl;
			}

			// Menu Vue :
			_viewMenu = new QMenu("&Vue", menubar);
			menubar->addMenu(_viewMenu);
#ifndef QT_4
			_viewMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_viewMenu->addAction(getActions()._print3DViewAction);
			_viewMenu->addAction(getActions()._print3DViewToFileAction);
			_viewMenu->addAction(getActions()._useGlobalDisplayPropertiesAction);
			_viewMenu->addAction(getActions()._displayTrihedronAction);
			_viewMenu->addAction(getActions()._displayLandmarkAction);
			_viewMenu->addAction(getActions()._parametrizeLandmarkAction);
			_viewMenu->addAction(getActions()._xOyViewAction);
			_viewMenu->addAction(getActions()._xOzViewAction);
			_viewMenu->addAction(getActions()._yOzViewAction);
			_viewMenu->addAction(getActions()._resetViewAction);
			_viewMenu->addAction(getActions()._clearViewAction);
			_viewMenu->addAction(getActions()._zoomViewAction);
			_viewMenu->addAction(getActions()._parallelViewAction);
			_viewMenu->addAction(getActions()._displayFocalPointAction);
			if (0 != getActions()._displayLookupTableAction)
				_viewMenu->addAction(getActions()._displayLookupTableAction);
			if (0 != getActions()._displayLookupTableEditorAction)
				_viewMenu->addAction(getActions()._displayLookupTableEditorAction);

			// Menu CAO :
//	_cadMenu	= new QMenu ("&CAO", menubar);
//	menubar->addMenu (_cadMenu);
//#ifndef QT_4
//	_cadMenu->setToolTipsVisible (true);
//#endif	// 	QT_4

			// Menu Topologie :
			_topologyMenu = new QMenu("&Topologie", menubar);
			menubar->addMenu(_topologyMenu);
#ifndef QT_4
			_topologyMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_topologyMenu->addAction(getActions()._topoSetDefaultNbMeshingEdgesAction);
			_topologyMenu->addAction(getActions()._topoRefineAction);
#ifdef _DEBUG
			_topologyMenu->addAction (getActions ( )._topoNew3x3BoxesWithTopoAction);
#endif
			_topologyMenu->addAction(getActions()._topoOptimMeshMethod);
			_topologyMenu->addAction(getActions()._topoInformationAction);

			// Menu maillage :
			_meshingMenu = new QMenu("&Maillage", menubar);
			menubar->addMenu(_meshingMenu);
#ifndef QT_4
			_meshingMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_meshingMenu->addAction(getActions()._meshSelectionAction);
			_meshingMenu->addAction(getActions()._meshVisibleAction);
			_meshingMenu->addAction(getActions()._meshAllAction);
			_meshingMenu->addSeparator();
			QMenu *representationMenu = new QMenu(QString::fromUtf8("Représentations"), _meshingMenu);
			_meshingMenu->addMenu(representationMenu);
#ifndef QT_4
			representationMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			representationMenu->addAction(
					getActions()._unrefineMeshRepresentationAction);
			_meshingMenu->addSeparator();
			_meshingMenu->addAction(getActions()._addMeshExplorerAction);
			_meshingMenu->addAction(getActions()._addMeshQualityAction);
			_meshingMenu->addSeparator();
			_meshingMenu->addAction(getActions()._meshInformationAction);

			// Menu Sélection :
			_selectionMenu = new QMenu(QString::fromUtf8("&Sélection"), menubar);
			menubar->addMenu(_selectionMenu);
#ifndef QT_4
			_selectionMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_selectionMenu->addAction(getActions()._displaySelectedEntitiesAction);
			_selectionMenu->addAction(getActions()._hideSelectedEntitiesAction);
			_selectionMenu->addAction(getActions()._displaySelectedGroupsAction);
			_selectionMenu->addAction(getActions()._hideSelectedGroupsAction);
			_selectionMenu->addAction(getActions()._displaySelectedTypesAction);
			_selectionMenu->addAction(getActions()._hideSelectedTypesAction);
			_selectionMenu->addSeparator();
			_selectionMenu->addAction(getActions()._displaySelectionPropertiesAction);
			_selectionMenu->addAction(
					getActions()._displayComputableSelectionPropertiesAction);
			_selectionMenu->addSeparator();
			_selectionMenu->addAction(getActions()._selectionRepresentationAction);

			_selectionMenu->addSeparator();

			_selectionMenu->addAction(getActions()._selectEntitiesAction);

			QMenu *criteriaSelectMenu = new QMenu(QString::fromUtf8("Sélection suivant critère"), menubar);
			_selectionMenu->addMenu(criteriaSelectMenu);
#ifndef QT_4
			criteriaSelectMenu->setToolTipsVisible(true);
#endif    // 	QT_4

			QMenu *visibleEntitiesMenu = new QMenu(QString::fromUtf8("Entités visibles ..."), menubar);
			criteriaSelectMenu->addMenu(visibleEntitiesMenu);
#ifndef QT_4
			visibleEntitiesMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			visibleEntitiesMenu->addAction(getActions()._selectVisibleEntitiesAction);

			QMenu *edgesMenu = new QMenu(QString::fromUtf8("Arêtes ..."), menubar);
			criteriaSelectMenu->addMenu(edgesMenu);
#ifndef QT_4
			edgesMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			edgesMenu->addAction(getActions()._selectFusableEdgesAction);
			edgesMenu->addAction(getActions()._selectInvalidEdges);

			QMenu *facesMenu = new QMenu("Faces ...", menubar);
			criteriaSelectMenu->addMenu(facesMenu);
#ifndef QT_4
			facesMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			facesMenu->addAction(getActions()._selectBorderFacesAction);
			facesMenu->addAction(getActions()._selectFacesWithoutBlockAction);
			facesMenu->addAction(getActions()._selectSemiConformFacesAction);
			facesMenu->addAction(getActions()._selectInvalidFaces);
			facesMenu->addAction(getActions()._selectUnstructuredFaces);
			facesMenu->addAction(getActions()._selectTransfiniteFaces);

			QMenu *blocksMenu = new QMenu("Blocs ...", menubar);
			criteriaSelectMenu->addMenu(blocksMenu);
#ifndef QT_4
			blocksMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			blocksMenu->addAction(getActions()._selectInvalidBlocks);
			blocksMenu->addAction(getActions()._selectUnstructuredBlocks);
			blocksMenu->addAction(getActions()._selectTransfiniteBlocks);

			_selectionMenu->addSeparator();

			QMenu *criteriaUnselectMenu = new QMenu(QString::fromUtf8("Désélection suivant critère"), menubar);
			_selectionMenu->addMenu(criteriaUnselectMenu);
#ifndef QT_4
			criteriaUnselectMenu->setToolTipsVisible(true);
#endif    // 	QT_4

			visibleEntitiesMenu = new QMenu(QString::fromUtf8("Entités visibles ..."), menubar);
			criteriaUnselectMenu->addMenu(visibleEntitiesMenu);
#ifndef QT_4
			visibleEntitiesMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			visibleEntitiesMenu->addAction(getActions()._unselectVisibleEntitiesAction);

			edgesMenu = new QMenu(QString::fromUtf8("Arêtes ..."), menubar);
			criteriaUnselectMenu->addMenu(edgesMenu);
#ifndef QT_4
			edgesMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			edgesMenu->addAction(getActions()._unselectFusableEdgesAction);
			edgesMenu->addAction(getActions()._unselectInvalidEdges);

			facesMenu = new QMenu("Faces ...", menubar);
			criteriaUnselectMenu->addMenu(facesMenu);
#ifndef QT_4
			facesMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			facesMenu->addAction(getActions()._unselectBorderFacesAction);
			facesMenu->addAction(getActions()._unselectBorderFacesAction);
			facesMenu->addAction(getActions()._unselectSemiConformFacesAction);
			facesMenu->addAction(getActions()._unselectInvalidFaces);
			facesMenu->addAction(getActions()._unselectUnstructuredFaces);
			facesMenu->addAction(getActions()._unselectTransfiniteFaces);

			blocksMenu = new QMenu("Blocs ...", menubar);
			criteriaUnselectMenu->addMenu(blocksMenu);
#ifndef QT_4
			blocksMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			blocksMenu->addAction(getActions()._unselectInvalidBlocks);
			blocksMenu->addAction(getActions()._unselectUnstructuredBlocks);
			blocksMenu->addAction(getActions()._unselectTransfiniteBlocks);

			_selectionMenu->addSeparator();

			_selectionMenu->addAction(getActions()._selectNodesAction);
			_selectionMenu->addAction(getActions()._selectEdgesAction);
			_selectionMenu->addAction(getActions()._selectSurfacesAction);
			_selectionMenu->addAction(getActions()._selectVolumesAction);
			_selectionMenu->addSeparator();
			_selectionMenu->addAction(getActions()._selectionModeAction);

#ifdef USE_EXPERIMENTAL_ROOM
			// Menu Chambre expérimentale :
			_roomMenu = new QMenu(QString::fromUtf8("&Chambre expérimentale"), menubar);
			menubar->addMenu(_roomMenu);
#ifndef QT_4
			_roomMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			if (0 != getActions()._setRaysContextAction)
				_roomMenu->addAction(getActions()._setRaysContextAction);
			_roomMenu->addSeparator();
			if (0 != getActions()._setRaysTargetSurfacesAction)
				_roomMenu->addAction(getActions()._setRaysTargetSurfacesAction);
			_roomMenu->addSeparator();
			if (0 != getActions()._loadRaysAction)
				_roomMenu->addAction(getActions()._loadRaysAction);
			if (0 != getActions()._importRaysAction)
				_roomMenu->addAction(getActions()._importRaysAction);
			if (0 != getActions()._saveRaysAction)
				_roomMenu->addAction(getActions()._saveRaysAction);
			if (0 != getActions()._saveAsRaysAction)
				_roomMenu->addAction(getActions()._saveAsRaysAction);
			_roomMenu->addSeparator();
			if (0 != getActions()._loadDiagsAction)
				_roomMenu->addAction(getActions()._loadDiagsAction);
			if (0 != getActions()._importDiagsAction)
				_roomMenu->addAction(getActions()._importDiagsAction);
			if (0 != getActions()._saveDiagsAction)
				_roomMenu->addAction(getActions()._saveDiagsAction);
			if (0 != getActions()._saveAsDiagsAction)
				_roomMenu->addAction(getActions()._saveAsDiagsAction);
#endif	// USE_EXPERIMENTAL_ROOM

			// Menu Outils :
			_toolsMenu = new QMenu("&Outils", menubar);
			menubar->addMenu(_toolsMenu);
#ifndef QT_4
			_toolsMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			if (0 != getActions()._showCommandMonitorDialogAction)
				_toolsMenu->addAction(getActions()._showCommandMonitorDialogAction);
			_toolsMenu->addAction(getActions()._distanceMeasurementAction);
			_toolsMenu->addAction(getActions()._angleMeasurementAction);
			_toolsMenu->addAction(getActions()._extremaMeshingEdgeLengthOnEdgeAction);

			// Menu Aide :
			_helpMenu = new QMenu("&Aide", menubar);
			menubar->addMenu(_helpMenu);
#ifndef QT_4
			_helpMenu->setToolTipsVisible(true);
#endif    // 	QT_4
			_helpMenu->addAction(getActions()._displayUsersGuideAction);
			_helpMenu->addAction(getActions()._displayUsersGuideContextAction);
			_helpMenu->addAction(getActions()._displayWikiAction);
			_helpMenu->addAction(getActions()._displayTutorialAction);
			_helpMenu->addAction(getActions()._displayPythonAPIUsersGuideAction);
			_helpMenu->addAction(getActions()._displayQualifAction);
			_helpMenu->addAction(getActions()._displayShortKeyAction);
			_helpMenu->addAction(getActions()._displaySelectAction);
			_helpMenu->addAction(getActions()._displayHistoriqueAction);
			_helpMenu->addSeparator();
			_helpMenu->addAction(getActions()._aboutDialogAction);

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::createMenu


		void QtMgx3DMainWindow::createToolbars()
		{
			BEGIN_QT_TRY_CATCH_BLOCK

			// On s'assure ici que createActions ait bien été appelé :
			CHECK_NULL_PTR_ERROR(getActions()._quitAction)

			// La barre d'outils "Commandes" :
			_commandToolbar = new QToolBar("Commandes", this);
			_commandToolbar->setObjectName("CommandsToolbar");
			_commandToolbar->setMovable(true);
			_commandToolbar->setFloatable(true);
			_commandToolbar->addAction(getActions()._undoAction);
			_commandToolbar->addAction(getActions()._redoAction);
			_commandToolbar->addSeparator();
			_commandToolbar->addAction(getActions()._importAction);
			_commandToolbar->addAction(getActions()._exportAllAction);
			_commandToolbar->addAction(getActions()._exportSelectionAction);
			addToolBar(Qt::TopToolBarArea, _commandToolbar);

			// La barre d'outils "Vue 3D" :
			_3dViewToolbar = new QToolBar(QString::fromUtf8("Définition vue 3D"), this);
			_3dViewToolbar->setObjectName("graphicalViewToolBar");
			_3dViewToolbar->setMovable(true);
			_3dViewToolbar->setFloatable(true);
			_3dViewToolbar->addAction(getActions()._print3DViewAction);
			_3dViewToolbar->addSeparator();
			_3dViewToolbar->addAction(getActions()._displayTrihedronAction);
			_3dViewToolbar->addAction(getActions()._displayLandmarkAction);
			_3dViewToolbar->addAction(getActions()._displayFocalPointAction);
			_3dViewToolbar->addSeparator();
			_3dViewToolbar->addAction(getActions()._xOyViewAction);
			_3dViewToolbar->addAction(getActions()._xOzViewAction);
			_3dViewToolbar->addAction(getActions()._yOzViewAction);
			_3dViewToolbar->addAction(getActions()._resetViewAction);
			if (0 != getActions()._displayLookupTableAction)
				_3dViewToolbar->addAction(getActions()._displayLookupTableAction);
			if (0 != getActions()._displayLookupTableEditorAction)
				_3dViewToolbar->addAction(
						getActions()._displayLookupTableEditorAction);
			addToolBar(Qt::TopToolBarArea, _3dViewToolbar);

			// Le maillage :
			_meshingToolbar = new QToolBar("Maillage", this);
			_meshingToolbar->setObjectName("meshingToolbar");
			_meshingToolbar->setMovable(true);
			_meshingToolbar->setFloatable(true);
			_meshingToolbar->addAction(getActions()._meshSelectionAction);
			_meshingToolbar->addAction(getActions()._meshVisibleAction);
			_meshingToolbar->addAction(getActions()._meshAllAction);
//	_meshingToolbar->addAction (getActions ( )._meshAction);
			addToolBar(Qt::TopToolBarArea, _meshingToolbar);

			// La barre  d'outils "Sélection" :
			_selectionToolbar = new QToolBar(QString::fromUtf8("Paramètres de sélection"), this);
			_selectionToolbar->setObjectName("selectionToolBar");
			_selectionToolbar->setMovable(true);
			_selectionToolbar->setFloatable(true);
			_selectionToolbar->addAction(getActions()._selectNodesAction);
			_selectionToolbar->addAction(getActions()._selectEdgesAction);
			_selectionToolbar->addAction(getActions()._selectSurfacesAction);
			_selectionToolbar->addAction(getActions()._selectVolumesAction);
			_selectionToolbar->addSeparator();
			_selectionToolbar->addAction(getActions()._selectionModeAction);
			addToolBar(Qt::TopToolBarArea, _selectionToolbar);

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::createToolbars


		void QtMgx3DMainWindow::createActions()
		{
			if (0 != getActions()._quitAction)
			{
				INTERNAL_ERROR(exc,
				               "QtMgx3DMainWindow::createActions", "Actions déjà créées.")
			}    // if (0 != getActions ( )._quitAction)

			_actions._preferencesAction = new QAction(QString::fromUtf8("Préférences ..."), this);
			connect(_actions._preferencesAction, SIGNAL(triggered()), this,
			        SLOT(preferencesCallback()), defaultConnectionType);
			_actions._editSettingsAction = new QAction("Etat de l'IHM ...", this);
			connect(_actions._editSettingsAction, SIGNAL(triggered()), this,
			        SLOT(editSettingsCallback()), defaultConnectionType);
			_actions._quitAction = new QAction("Quitter ...", this);
			connect(_actions._quitAction, SIGNAL(triggered()), this,
			        SLOT(exitCallback()));
			_actions._quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
			_actions._quitAction->setShortcutContext(Qt::ApplicationShortcut);

			// La session :
			_actions._undoAction =
					new QtAutoDisablabledAction(
							QIcon(":/images/undo.png"), "Annuler", this);
			connect(_actions._undoAction, SIGNAL(triggered()), this,
			        SLOT(undoCallback()), defaultConnectionType);
			_actions._undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
			_actions._undoAction->setShortcutContext(Qt::ApplicationShortcut);
			_actions._redoAction =
					new QtAutoDisablabledAction(
							QIcon(":/images/redo.png"), "Refaire", this);
			connect(_actions._redoAction, SIGNAL(triggered()), this,
			        SLOT(redoCallback()), defaultConnectionType);
			_actions._redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
			_actions._redoAction->setShortcutContext(Qt::ApplicationShortcut);
			_actions._clearAction = new QAction(QString::fromUtf8("Réinitialiser"), this);
			connect(_actions._clearAction, SIGNAL(triggered()), this,
			        SLOT(reinitializeCallback()), defaultConnectionType);
			QActionGroup *unitGroup = new QActionGroup(this);
			unitGroup->setExclusive(true);
			_actions._meterAction = new QAction("m", this);
			_actions._meterAction->setCheckable(true);
			connect(_actions._meterAction, SIGNAL(triggered()), this,
			        SLOT(unitMeterCallback()), defaultConnectionType);
			unitGroup->addAction(_actions._meterAction);
			_actions._centimeterAction = new QAction("cm", this);
			_actions._centimeterAction->setCheckable(true);
			connect(_actions._centimeterAction, SIGNAL(triggered()), this,
			        SLOT(unitCentimeterCallback()), defaultConnectionType);
			unitGroup->addAction(_actions._centimeterAction);
			_actions._millimeterAction = new QAction("mm", this);
			_actions._millimeterAction->setCheckable(true);
			connect(_actions._millimeterAction, SIGNAL(triggered()), this,
			        SLOT(unitMillimeterCallback()), defaultConnectionType);
			unitGroup->addAction(_actions._millimeterAction);
			QString name;
			name = QtStringHelper::muMin() + "m";
			_actions._micrometerAction = new QAction(name, this);
			_actions._micrometerAction->setCheckable(true);
			connect(_actions._micrometerAction, SIGNAL(triggered()), this,
			        SLOT(unitMicrometerCallback()), defaultConnectionType);
			unitGroup->addAction(_actions._micrometerAction);
			_actions._undefinedUnitAction = new QAction(QString::fromUtf8("unité indéfinie"), this);
			_actions._undefinedUnitAction->setCheckable(true);
			_actions._undefinedUnitAction->setEnabled(false);    // sinon exception !
			connect(_actions._undefinedUnitAction, SIGNAL(triggered()), this,
			        SLOT(unitUndefinedCallback()), defaultConnectionType);
			unitGroup->addAction(_actions._undefinedUnitAction);
			try
			{
				switch (getContext().getLengthUnit())
				{
					case Unit::meter        :
						_actions._meterAction->setChecked(true);
						break;
					case Unit::centimeter    :
						_actions._centimeterAction->setChecked(true);
						break;
					case Unit::milimeter    :
						_actions._millimeterAction->setChecked(true);
						break;
					case Unit::micron        :
						_actions._millimeterAction->setChecked(true);
						break;
					default                    :
						_actions._undefinedUnitAction->setChecked(true);
				}    // switch (getContext ( ).getLengthUnit ( ))
			}
			catch (...)
			{
				_actions._undefinedUnitAction->setChecked(true);
			}
			QActionGroup *landmarkGroup = new QActionGroup(this);
			landmarkGroup->setExclusive(true);
			_actions._meshLandmarkAction = new QAction("Maillage", this);
			_actions._meshLandmarkAction->setCheckable(true);
			connect(_actions._meshLandmarkAction, SIGNAL(triggered()), this,
			        SLOT(meshLandmarkCallback()), defaultConnectionType);
			landmarkGroup->addAction(_actions._meshLandmarkAction);
			_actions._expRoomLandmarkAction = new QAction(QString::fromUtf8("Chambre expérimentale"), this);
			_actions._expRoomLandmarkAction->setCheckable(true);
			connect(_actions._expRoomLandmarkAction, SIGNAL(triggered()), this,
			        SLOT(expRoomLandmarkCallback()), defaultConnectionType);
			landmarkGroup->addAction(_actions._expRoomLandmarkAction);
			_actions._undefinedLandmarkAction = new QAction(QString::fromUtf8("Indéfini"), this);
			_actions._undefinedLandmarkAction->setCheckable(true);
			_actions._undefinedLandmarkAction->setEnabled(false);
			connect(_actions._undefinedLandmarkAction, SIGNAL(triggered()), this,
			        SLOT(undefinedLandmarkCallback()), defaultConnectionType);
			landmarkGroup->addAction(_actions._undefinedLandmarkAction);
			try
			{
				switch (getContext().getLandmark())
				{
					case Landmark::maillage        :
						_actions._meshLandmarkAction->setChecked(true);
						break;
					case Landmark::chambre    :
						_actions._expRoomLandmarkAction->setChecked(true);
						break;
					default                    :
						_actions._undefinedLandmarkAction->setChecked(true);
				}    // switch (getContext ( ).getandmark ( ))
			}
			catch (...)
			{
				_actions._undefinedLandmarkAction->setChecked(true);
			}

			_actions._mesh2DDimAction = new QAction("Maillage en 2D", this);
			connect(_actions._mesh2DDimAction, SIGNAL(triggered()), this,
			        SLOT(mesh2DDimCallback()), defaultConnectionType);

			_actions._importAction =
					new QtAutoDisablabledAction(
							QIcon(":/images/import.png"), "Importer ...", this);
			connect(_actions._importAction, SIGNAL(triggered()), this,
			        SLOT(importCallback()), defaultConnectionType);
			_actions._exportAllAction =
					new QtAutoDisablabledAction(
							QIcon(":/images/export.png"), "Exporter tout ...", this);
			connect(_actions._exportAllAction, SIGNAL(triggered()), this,
			        SLOT(exportAllCallback()), defaultConnectionType);
			_actions._exportSelectionAction =
					new QtAutoDisablabledAction(
							QIcon(":/images/export_selection.png"),
							QString::fromUtf8("Exporter la CAO sélectionnée ..."), this);
			connect(_actions._exportSelectionAction, SIGNAL(triggered()), this, SLOT(exportSelectionCallback()), defaultConnectionType);
			_actions._saveMagix3DScriptAction = new QtAutoDisablabledAction(QIcon(":/images/export_mgx3d_script.png"), "Enregistrer le script Magix 3D minimum ...", this);
			_actions._saveMagix3DScriptAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
			connect(_actions._saveMagix3DScriptAction, SIGNAL(triggered()), this, SLOT(saveMagix3DScriptCallback()), defaultConnectionType);
			_actions._saveAsMagix3DScriptAction = new QtAutoDisablabledAction ("Enregistrer sous le script Magix 3D minimum ...", this);
			connect(_actions._saveAsMagix3DScriptAction, SIGNAL(triggered()), this, SLOT(saveAsMagix3DScriptCallback()), defaultConnectionType);
			_actions._savePythonConsoleAction = new QtAutoDisablabledAction ("Enregistrer la console python ...", this);
			connect(_actions._savePythonConsoleAction, SIGNAL(triggered()), this, SLOT(savePythonConsoleCallback ( )), defaultConnectionType);
#ifdef _DEBUG
	_actions._longCommandAction	= new QAction ("Longue commande", this);
	connect (_actions._longCommandAction, SIGNAL (triggered ( )), this,
	         SLOT (longCommandCallback ( )), defaultConnectionType);
	_actions._openCascadeLongCommandAction	= new QAction ("Longue commande Open Cascade (Boucle infinie sans progression apparente)", this);
	connect (_actions._openCascadeLongCommandAction, SIGNAL (triggered ( )), this,
	         SLOT (openCascadeLongCommandCallback ( )), defaultConnectionType);
#endif
			_actions._executePythonScriptAction = new QAction(QString::fromUtf8("Exécuter un script python ..."), this);
			connect(_actions._executePythonScriptAction, SIGNAL(triggered()), this,
			        SLOT(executePythonScriptCallback()), defaultConnectionType);

			// La vue 3D :
			_actions._print3DViewAction =
					new QAction(QIcon(":/images/print.png"), "Imprimer ...", this);
			connect(_actions._print3DViewAction, SIGNAL(triggered()), this,
			        SLOT(print3DViewCallback()), defaultConnectionType);
			_actions._print3DViewAction->setShortcut(
					QKeySequence(Qt::CTRL + Qt::Key_P));
			_actions._print3DViewAction->setShortcutContext(Qt::ApplicationShortcut);
			_actions._print3DViewToFileAction =
					new QAction("Imprimer dans un fichier ...", this);
			connect(_actions._print3DViewToFileAction, SIGNAL(triggered()), this,
			        SLOT(print3DViewToFileCallback()), defaultConnectionType);
			_actions._useGlobalDisplayPropertiesAction =
					new QAction(
							QString::fromUtf8("Utilisation des propriétés d'affichage globales"), this);
			_actions._useGlobalDisplayPropertiesAction->setCheckable(true);
			_actions._useGlobalDisplayPropertiesAction->setChecked(true);
			connect(_actions._useGlobalDisplayPropertiesAction, SIGNAL(toggled(bool)),
			        this, SLOT(useGlobalDisplayPropertiesCallback(bool)),
			        defaultConnectionType);
			_actions._displayTrihedronAction =
					new QAction(
							QIcon(":/images/trihedron.png"), QString::fromUtf8("Afficher le trièdre"), this);
			_actions._displayTrihedronAction->setCheckable(true);
			_actions._displayTrihedronAction->setChecked(
					Resources::instance()._displayTrihedron.getValue());
			connect(_actions._displayTrihedronAction, SIGNAL(toggled(bool)), this,
			        SLOT(displayTrihedronCallback(bool)), defaultConnectionType);
			_actions._displayLandmarkAction =
					new QAction(
							QIcon(":/images/landmark.png"), QString::fromUtf8("Afficher le repère"), this);
			_actions._displayLandmarkAction->setCheckable(true);
			_actions._displayLandmarkAction->setChecked(false);
			connect(_actions._displayLandmarkAction, SIGNAL(toggled(bool)), this,
			        SLOT(displayLandmarkCallback(bool)), defaultConnectionType);
			_actions._parametrizeLandmarkAction =
					new QAction(QString::fromUtf8("Paramétrer le repère"), this);
			connect(_actions._parametrizeLandmarkAction, SIGNAL(triggered()), this,
			        SLOT(parametrizeLandmarkCallback()), defaultConnectionType);
			_actions._displayFocalPointAction =
					new QAction(
							QIcon(":/images/focal_cross.png"), "Afficher le point focal",
							this);
			_actions._displayFocalPointAction->setCheckable(true);
			_actions._displayFocalPointAction->setChecked(
					Resources::instance()._displayFocalPoint.getValue());
			connect(_actions._displayFocalPointAction, SIGNAL(toggled(bool)), this,
			        SLOT(displayFocalPointCrossCallback(bool)),
			        defaultConnectionType);
			_actions._xOyViewAction =
					new QAction(QIcon(":/images/trihedron_xOy.png"),
					            "Afficher la vue du plan xOy", this);
			connect(_actions._xOyViewAction, SIGNAL(triggered()), this,
			        SLOT(xOyViewPlaneCallback()), defaultConnectionType);
			_actions._xOzViewAction =
					new QAction(QIcon(":/images/trihedron_xOz.png"),
					            "Afficher la vue du plan xOz", this);
			connect(_actions._xOzViewAction, SIGNAL(triggered()), this,
			        SLOT(xOzViewPlaneCallback()), defaultConnectionType);
			_actions._yOzViewAction =
					new QAction(QIcon(":/images/trihedron_yOz.png"),
					            "Afficher la vue du plan yOz", this);
			connect(_actions._yOzViewAction, SIGNAL(triggered()), this,
			        SLOT(yOzViewPlaneCallback()), defaultConnectionType);
			_actions._resetViewAction =
					new QAction(QIcon(":/images/reset_view.png"),
					            "Recadrer la vue", this);
			connect(_actions._resetViewAction, SIGNAL(triggered()), this,
			        SLOT(resetViewCallback()), defaultConnectionType);

			_actions._clearViewAction = new QAction("Vider la vue", this);
			connect(_actions._clearViewAction, SIGNAL(triggered()), this, SLOT(clearViewCallback()), defaultConnectionType);

			_actions._zoomViewAction = new QAction("Zoomer ...", this);
			connect(_actions._zoomViewAction, SIGNAL(triggered()), this, SLOT(zoomViewCallback()), defaultConnectionType);

			_actions._parallelViewAction = new QAction(QString::fromUtf8("Projection parallèle"), this);
			_actions._parallelViewAction->setCheckable(true);
			_actions._parallelViewAction->setChecked(false);
			connect(_actions._parallelViewAction, SIGNAL(toggled(bool)), this,
			        SLOT(parallelProjectionCallback(bool)),
			        defaultConnectionType);
			_actions._displayLookupTableAction = new QAction(QIcon(":/images/lookup_table.png"), "Afficher la table de couleurs", this);
			_actions._displayLookupTableAction->setCheckable(true);
			connect(_actions._displayLookupTableAction, SIGNAL(toggled(bool)), this,
			        SLOT(displayLookupTableCallback()), defaultConnectionType);
			_actions._displayLookupTableEditorAction =
					new QAction(QIcon(":/images/lookup_table_editor.png"),
					            QString::fromUtf8("Afficher l'éditeur de tables de couleurs"), this);
			connect(_actions._displayLookupTableEditorAction,
			        SIGNAL(triggered()), this,
			        SLOT(displayLookupTableEditorCallback()), defaultConnectionType);

			// La CAO :

			// La Topologie
			_actions._topoRefineAction =
					new QAction(QString::fromUtf8("Raffiner la discrétisation des arêtes ..."), this);
			connect(_actions._topoRefineAction, SIGNAL(triggered()), this,
			        SLOT(refineTopologyCallback()), defaultConnectionType);
			_actions._topoSetDefaultNbMeshingEdgesAction =
					new QAction(QString::fromUtf8("Modifier la discrétisation des arêtes par défaut ..."), this);
			connect(_actions._topoSetDefaultNbMeshingEdgesAction, SIGNAL(triggered()), this,
			        SLOT(defaultNbMeshingEdgesCallback()), defaultConnectionType);
#ifdef _DEBUG
																																	_actions._topoNew3x3BoxesWithTopoAction   =
            new QAction (QString::fromUtf8("Créer une grille de 3 X 3 boites avec les blocs ..."), this);
    connect (_actions._topoNew3x3BoxesWithTopoAction, SIGNAL (triggered ( )), this,
            SLOT (new3x3BoxesWithTopoCallback ( )), defaultConnectionType);
#endif
			_actions._topoOptimMeshMethod =
					new QAction(QString::fromUtf8("Optimise la méthode de maillage"), this);
			connect(_actions._topoOptimMeshMethod, SIGNAL(triggered()), this,
			        SLOT(optimMeshMethodCallback()), defaultConnectionType);
			_actions._topoInformationAction =
					new QAction("Informations sur la topologie", this);
			connect(_actions._topoInformationAction, SIGNAL(triggered()), this,
			        SLOT(addTopoInformationCallback()), defaultConnectionType);


			// Le Maillage
			_actions._meshSelectionAction = new QAction(
					QIcon(":/images/mesh_selection.png"), QString::fromUtf8("Mailler la sélection ..."),
					this);
			connect(_actions._meshSelectionAction, SIGNAL(triggered()), this,
			        SLOT(meshSelectionCallback()), defaultConnectionType);
			_actions._meshVisibleAction = new QAction(
					QIcon(":/images/mesh_visible.png"),
					"Mailler ce qui est visible ...", this);
			connect(_actions._meshVisibleAction, SIGNAL(triggered()), this,
			        SLOT(meshVisibleCallback()), defaultConnectionType);
			_actions._meshAllAction = new QAction(
					QIcon(":/images/meshing.png"), "Tout mailler ...", this);
			connect(_actions._meshAllAction, SIGNAL(triggered()), this,
			        SLOT(meshAllCallback()), defaultConnectionType);

			_actions._unrefineMeshRepresentationAction = new QAction(
					QString::fromUtf8("Déraffiner ..."), this);
			connect(_actions._unrefineMeshRepresentationAction, SIGNAL(triggered()),
			        this, SLOT(unrefineMeshRepresentationCallback()),
			        defaultConnectionType);

			_actions._addMeshExplorerAction =
					new QAction(QIcon(":/images/sheet_explorer.png"),
					            "Exploration de feuillets", this);
			connect(_actions._addMeshExplorerAction, SIGNAL(triggered()), this,
			        SLOT(addMeshExplorerCallback()), defaultConnectionType);
			_actions._addMeshQualityAction =
					new QAction(QIcon(":/images/mesh_quality.png"),
					            QString::fromUtf8("Evaluation de la qualité du maillage"), this);
			connect(_actions._addMeshQualityAction, SIGNAL(triggered()), this,
			        SLOT(addMeshQualityCallback()), defaultConnectionType);

			_actions._meshInformationAction =
					new QAction("Informations sur le maillage", this);
			connect(_actions._meshInformationAction, SIGNAL(triggered()), this,
			        SLOT(addMeshInformationCallback()), defaultConnectionType);


			// La sélection :
			_actions._displaySelectedGroupsAction =
					new QAction("Afficher les groupes", this);
			connect(_actions._displaySelectedGroupsAction, SIGNAL(triggered()), this,
			        SLOT(displaySelectedGroupsCallback()));
			_actions._hideSelectedGroupsAction =
					new QAction("Masquer les groupes", this);
			connect(_actions._hideSelectedGroupsAction, SIGNAL(triggered()), this,
			        SLOT(hideSelectedGroupsCallback()));

			_actions._displaySelectedTypesAction =
					new QAction(QString::fromUtf8("Afficher les entités de types sélectionnés"), this);
			connect(_actions._displaySelectedTypesAction, SIGNAL(triggered()), this,
			        SLOT(displaySelectedTypesCallback()));
			_actions._hideSelectedTypesAction =
					new QAction(QString::fromUtf8("Masquer les entités de types sélectionnés"), this);
			connect(_actions._hideSelectedTypesAction, SIGNAL(triggered()), this,
			        SLOT(hideSelectedTypesCallback()));

			_actions._displaySelectedEntitiesAction =
					new QAction(QString::fromUtf8("Afficher les entités sélectionnées"), this);
			connect(_actions._displaySelectedEntitiesAction, SIGNAL(triggered()), this,
			        SLOT(displaySelectedEntitiesCallback()));
			_actions._hideSelectedEntitiesAction =
					new QAction(QString::fromUtf8("Masquer les entités sélectionnées"), this);
			connect(_actions._hideSelectedEntitiesAction, SIGNAL(triggered()), this,
			        SLOT(hideSelectedEntitiesCallback()));
			_actions._displaySelectionPropertiesAction =
					new QAction(QString::fromUtf8("Afficher les propriétés des entités sélectionnées"), this);
			_actions._displaySelectionPropertiesAction->setCheckable(true);
			_actions._displaySelectionPropertiesAction->setChecked(true);
			connect(_actions._displaySelectionPropertiesAction, SIGNAL(triggered()),
			        this, SLOT(displaySelectedEntitiesPropertiesCallback()));
			_actions._displayComputableSelectionPropertiesAction =
					new QAction(QString::fromUtf8("Afficher les propriétés calculées des entités sélectionnées"), this);
			_actions._displayComputableSelectionPropertiesAction->setCheckable(true);
			_actions._displayComputableSelectionPropertiesAction->setChecked(false);
			connect(_actions._displayComputableSelectionPropertiesAction,
			        SIGNAL(triggered()), this,
			        SLOT(displaySelectedEntitiesComputablePropertiesCallback()));

			_actions._selectionRepresentationAction =
					new QAction(QString::fromUtf8("Représentations ..."), this);
			connect(_actions._selectionRepresentationAction, SIGNAL(triggered()), this,
			        SLOT(showRepresentationTypesCallback()));
			_actions._selectEntitiesAction = new QAction(QString::fromUtf8("Sélectionner des entités ..."), this);
			connect(_actions._selectEntitiesAction, SIGNAL(triggered()), this,
			        SLOT(selectEntitiesCallback()));
			_actions._selectEntitiesAction->setShortcut(
					QKeySequence(Qt::CTRL + Qt::Key_F));
			_actions._selectEntitiesAction->setShortcutContext(Qt::ApplicationShortcut);
			_actions._selectVisibleEntitiesAction = new QAction(QString::fromUtf8("Sélectionner les entités visibles"), this);
			connect(_actions._selectVisibleEntitiesAction, SIGNAL(triggered()),
			        this, SLOT(selectVisibleEntitiesCallback()));
			_actions._unselectVisibleEntitiesAction = new QAction(QString::fromUtf8("Désélectionner les entités visibles"), this);
			connect(_actions._unselectVisibleEntitiesAction, SIGNAL(triggered()),
			        this, SLOT(unselectVisibleEntitiesCallback()));
			_actions._selectFusableEdgesAction =
					new QAction(QString::fromUtf8("Sélectionner les arêtes fusionnables"), this);
			connect(_actions._selectFusableEdgesAction, SIGNAL(triggered()), this,
			        SLOT(selectFusableEdgesCallback()));
			_actions._unselectFusableEdgesAction =
					new QAction(QString::fromUtf8("Désélectionner les arêtes fusionnables"), this);
			connect(_actions._unselectFusableEdgesAction, SIGNAL(triggered()), this,
			        SLOT(unselectFusableEdgesCallback()));
			_actions._selectInvalidEdges =
					new QAction(QString::fromUtf8("Sélectionner les arêtes invalides"), this);
			connect(_actions._selectInvalidEdges, SIGNAL(triggered()), this,
			        SLOT(selectInvalidEdgesCallback()));
			_actions._unselectInvalidEdges =
					new QAction(QString::fromUtf8("Désélectionner les arêtes invalides"), this);
			connect(_actions._unselectInvalidEdges, SIGNAL(triggered()), this,
			        SLOT(unselectInvalidEdgesCallback()));

			_actions._selectBorderFacesAction =
					new QAction(QString::fromUtf8("Sélectionner les faces aux bords"), this);
			connect(_actions._selectBorderFacesAction, SIGNAL(triggered()), this,
			        SLOT(selectBorderFacesCallback()));
			_actions._unselectBorderFacesAction =
					new QAction(QString::fromUtf8("Désélectionner les faces aux bords"), this);
			connect(_actions._unselectBorderFacesAction, SIGNAL(triggered()), this,
			        SLOT(unselectBorderFacesCallback()));
			_actions._selectFacesWithoutBlockAction =
					new QAction(QString::fromUtf8("Sélectionner les faces sans blocs"), this);
			connect(_actions._selectFacesWithoutBlockAction, SIGNAL(triggered()), this,
			        SLOT(selectFacesWithoutBlockCallback()));
			_actions._unselectFacesWithoutBlockAction =
					new QAction(QString::fromUtf8("Désélectionner les faces sans blocs"), this);
			connect(_actions._unselectFacesWithoutBlockAction, SIGNAL(triggered()), this,
			        SLOT(unselectFacesWithoutBlockCallback()));
			_actions._selectSemiConformFacesAction =
					new QAction(QString::fromUtf8("Sélectionner les faces semi-conformes"), this);
			connect(_actions._selectSemiConformFacesAction, SIGNAL(triggered()),
			        this, SLOT(selectSemiConformFacesCallback()));
			_actions._unselectSemiConformFacesAction =
					new QAction(QString::fromUtf8("Désélectionner les faces semi-conformes"), this);
			connect(_actions._unselectSemiConformFacesAction, SIGNAL(triggered()),
			        this, SLOT(unselectSemiConformFacesCallback()));
			_actions._selectInvalidFaces =
					new QAction(QString::fromUtf8("Sélectionner les faces invalides"), this);
			connect(_actions._selectInvalidFaces, SIGNAL(triggered()),
			        this, SLOT(selectInvalidFacesCallback()));
			_actions._unselectInvalidFaces =
					new QAction(QString::fromUtf8("Désélectionner les faces invalides"), this);
			connect(_actions._unselectInvalidFaces, SIGNAL(triggered()),
			        this, SLOT(unselectInvalidFacesCallback()));
			_actions._selectUnstructuredFaces =
					new QAction(QString::fromUtf8("Sélectionner les faces non-structuées"), this);
			connect(_actions._selectUnstructuredFaces, SIGNAL(triggered()),
			        this, SLOT(selectUnstructuredFacesCallback()));
			_actions._unselectUnstructuredFaces =
					new QAction(QString::fromUtf8("Désélectionner les faces non-structuées"), this);
			connect(_actions._unselectUnstructuredFaces, SIGNAL(triggered()),
			        this, SLOT(unselectUnstructuredFacesCallback()));
			_actions._selectTransfiniteFaces =
					new QAction(QString::fromUtf8("Sélectionner les faces avec méthode transfinie"), this);
			connect(_actions._selectTransfiniteFaces, SIGNAL(triggered()),
			        this, SLOT(selectTransfiniteFacesCallback()));
			_actions._unselectTransfiniteFaces =
					new QAction(QString::fromUtf8("Désélectionner les faces avec méthode transfinie"), this);
			connect(_actions._unselectTransfiniteFaces, SIGNAL(triggered()),
			        this, SLOT(unselectTransfiniteFacesCallback()));

			_actions._selectInvalidBlocks =
					new QAction(QString::fromUtf8("Sélectionner les blocs invalides"), this);
			connect(_actions._selectInvalidBlocks, SIGNAL(triggered()),
			        this, SLOT(selectInvalidBlocksCallback()));
			_actions._unselectInvalidBlocks =
					new QAction(QString::fromUtf8("Désélectionner les blocs invalides"), this);
			connect(_actions._unselectInvalidBlocks, SIGNAL(triggered()),
			        this, SLOT(unselectInvalidBlocksCallback()));
			_actions._selectUnstructuredBlocks =
					new QAction(QString::fromUtf8("Sélectionner les blocs non-structurés"), this);
			connect(_actions._selectUnstructuredBlocks, SIGNAL(triggered()),
			        this, SLOT(selectUnstructuredBlocksCallback()));
			_actions._unselectUnstructuredBlocks =
					new QAction(QString::fromUtf8("Désélectionner les blocs non-structurés"), this);
			connect(_actions._unselectUnstructuredBlocks, SIGNAL(triggered()),
			        this, SLOT(unselectUnstructuredBlocksCallback()));
			_actions._selectTransfiniteBlocks =
					new QAction(QString::fromUtf8("Sélectionner les blocs avec méthode transfinie"), this);
			connect(_actions._selectTransfiniteBlocks, SIGNAL(triggered()),
			        this, SLOT(selectTransfiniteBlocksCallback()));
			_actions._unselectTransfiniteBlocks =
					new QAction(QString::fromUtf8("Désélectionner les blocs avec méthode transfinie"), this);
			connect(_actions._unselectTransfiniteBlocks, SIGNAL(triggered()),
			        this, SLOT(unselectTransfiniteBlocksCallback()));

			_actions._selectNodesAction =
					new QAction(QIcon(":/images/nodes.png"),
					            QString::fromUtf8("Sélection d'entités 0D"), this);
			_actions._selectNodesAction->setCheckable(true);
			if (0 != getSelectionManager())
				_actions._selectNodesAction->setChecked(
						getSelectionManager()->is0DSelectionActivated());
			connect(_actions._selectNodesAction, SIGNAL(toggled(bool)), this,
			        SLOT(selectNodesCallback(bool)), defaultConnectionType);
			_actions._selectEdgesAction =
					new QAction(QIcon(":/images/edges.png"),
					            QString::fromUtf8("Sélection d'entités 1D"), this);
			_actions._selectEdgesAction->setCheckable(true);
			if (0 != getSelectionManager())
				_actions._selectEdgesAction->setChecked(
						getSelectionManager()->is1DSelectionActivated());
			connect(_actions._selectEdgesAction, SIGNAL(toggled(bool)), this,
			        SLOT(selectEdgesCallback(bool)), defaultConnectionType);
			_actions._selectSurfacesAction =
					new QAction(QIcon(":/images/surfaces.png"),
					            QString::fromUtf8("Sélection d'entités 2D"), this);
			_actions._selectSurfacesAction->setCheckable(true);
			if (0 != getSelectionManager())
				_actions._selectSurfacesAction->setChecked(
						getSelectionManager()->is2DSelectionActivated());
			connect(_actions._selectSurfacesAction, SIGNAL(toggled(bool)), this,
			        SLOT(selectSurfacesCallback(bool)), defaultConnectionType);
			_actions._selectVolumesAction =
					new QAction(QIcon(":/images/volumes.png"),
					            QString::fromUtf8("Sélection d'entités 3D"), this);
			_actions._selectVolumesAction->setCheckable(true);
			if (0 != getSelectionManager())
				_actions._selectVolumesAction->setChecked(
						getSelectionManager()->is3DSelectionActivated());
			connect(_actions._selectVolumesAction, SIGNAL(toggled(bool)), this,
			        SLOT(selectVolumesCallback(bool)), defaultConnectionType);
			_actions._selectionModeAction =
					new QAction(QIcon(":/images/selection_mode.png"),
					            QString::fromUtf8("Mode de sélection des entités (filaire ou plein)"), this);
			_actions._selectionModeAction->setCheckable(true);
			_actions._selectionModeAction->setChecked(true);
			connect(_actions._selectionModeAction, SIGNAL(toggled(bool)), this,
			        SLOT(selectionModeCallback(bool)), defaultConnectionType);

#ifdef USE_EXPERIMENTAL_ROOM
			// La chambre expérimentale :
			_actions._setRaysContextAction =
					new QAction("Utiliser ce contexte", this);
			_actions._setRaysContextAction->setToolTip(QString::fromUtf8("Utilise le contexte courant (surface et volumes sélectionné,\nunité de longueur) lors de la projection des pointages laser."));
			connect(_actions._setRaysContextAction, SIGNAL(triggered()),
			        this, SLOT(setRaysContextCallback()));
			_actions._setRaysTargetSurfacesAction =
					new QAction(QString::fromUtf8("Projeter sur les surfaces ou volumes sélectionnés"), this);
			_actions._setRaysTargetSurfacesAction->setToolTip(QString::fromUtf8("Projette les pointages laser sur les surfaces et volumes sélectionnés."));
			connect(_actions._setRaysTargetSurfacesAction, SIGNAL(triggered()),
			        this, SLOT(setRaysTargetSurfacesCallback()));
			// Les pointages laser :
			_actions._loadRaysAction =
					new QAction("Charger des pointages laser ...", this);
			_actions._loadRaysAction->setToolTip(QString::fromUtf8("Affiche un sélecteur de fichier de pointages laser."));
			connect(_actions._loadRaysAction, SIGNAL(triggered()),
			        this, SLOT(loadRaysCallback()));
			_actions._importRaysAction =
					new QAction("Importer des pointages laser ...", this);
			_actions._importRaysAction->setToolTip(QString::fromUtf8("Affiche un sélecteur de fichier de pointages laser positionné dans la base de données."));
			connect(_actions._importRaysAction, SIGNAL(triggered()),
			        this, SLOT(importRaysCallback()));
			_actions._saveRaysAction =
					new QAction("Enregistrer les pointages laser", this);
			_actions._saveRaysAction->setToolTip(QString::fromUtf8("Enregistre les pointages laser dans le fichier en cours."));
			connect(_actions._saveRaysAction, SIGNAL(triggered()),
			        this, SLOT(saveRaysCallback()));
			_actions._saveAsRaysAction =
					new QAction("Enregistrer sous les pointages laser ...", this);
			_actions._saveAsRaysAction->setToolTip(QString::fromUtf8("Affiche un sélecteur de fichier pour enregistrer les pointages laser."));
			connect(_actions._saveAsRaysAction, SIGNAL(triggered()),
			        this, SLOT(saveAsRaysCallback()));
			// Les diagnostics :
			_actions._loadDiagsAction =
					new QAction("Charger des diagnostics ...", this);
			_actions._loadDiagsAction->setToolTip(QString::fromUtf8("Affiche un sélecteur de fichier de diagnostics."));
			connect(_actions._loadDiagsAction, SIGNAL(triggered()),
			        this, SLOT(loadDiagsCallback()));
			_actions._importDiagsAction =
					new QAction("Importer des diagnostics ...", this);
			_actions._importDiagsAction->setToolTip(QString::fromUtf8("Affiche un sélecteur de fichier de diagnostics positionné dans la base de données."));
			connect(_actions._importDiagsAction, SIGNAL(triggered()),
			        this, SLOT(importDiagsCallback()));
			_actions._saveDiagsAction =
					new QAction("Enregistrer des diagnostics", this);
			_actions._saveDiagsAction->setToolTip(QString::fromUtf8("Enregistre les diagnostics dans le fichier en cours."));
			connect(_actions._saveDiagsAction, SIGNAL(triggered()),
			        this, SLOT(saveDiagsCallback()));
			_actions._saveAsDiagsAction =
					new QAction("Enregistrer sous des diagnostics ...", this);
			_actions._saveAsDiagsAction->setToolTip(QString::fromUtf8("Affiche un sélecteur de fichier pour enregistrer les diagnostics."));
			connect(_actions._saveAsDiagsAction, SIGNAL(triggered()),
			        this, SLOT(saveAsDiagsCallback()));
#endif	// USE_EXPERIMENTAL_ROOM

			// Les outils :
/*
	_actions._showCommandMonitorDialogAction	=
			new QAction ("Moniteur de commandes ...", this);
	connect (_actions._showCommandMonitorDialogAction, SIGNAL (triggered ( )),
	         this, SLOT (showCommandMonitorDialogCallback ( )),
	         defaultConnectionType);
*/

			_actions._distanceMeasurementAction =
					new QAction(QIcon(":/images/distance.png"),
					            QString::fromUtf8("Mesure de la distance entre entités C.A.O. ou topo"), this);
			connect(_actions._distanceMeasurementAction, SIGNAL(triggered()), this,
			        SLOT(addDistanceMeasurementCallback()), defaultConnectionType);

			_actions._angleMeasurementAction =
					new QAction(QString::fromUtf8("Mesure de l'angle entre 3 points"), this);
			connect(_actions._angleMeasurementAction, SIGNAL(triggered()), this,
			        SLOT(addAngleMeasurementCallback()), defaultConnectionType);

			_actions._extremaMeshingEdgeLengthOnEdgeAction =
					new QAction(QString::fromUtf8("Calcul la longeur des bras aux extrémités d'une arête"), this);
			connect(_actions._extremaMeshingEdgeLengthOnEdgeAction, SIGNAL(triggered()), this,
			        SLOT(addExtremaMeshingEdgeLengthOnEdgeCallback()), defaultConnectionType);

			// L'aide :
			_actions._displayUsersGuideAction =
					new QAction("Manuel utilisateur (pdf)", this);
			connect(_actions._displayUsersGuideAction, SIGNAL(triggered()), this,
			        SLOT(usersGuideCallback()), defaultConnectionType);
			_actions._displayUsersGuideContextAction =
					new QAction("Aide contextuelle (html)", this);
			_actions._displayUsersGuideContextAction->setShortcut(
					QKeySequence(Qt::Key_F1));
			_actions._displayUsersGuideContextAction->setShortcutContext(
					Qt::ApplicationShortcut);
			connect(_actions._displayUsersGuideContextAction, SIGNAL(triggered()),
			        this, SLOT(usersGuideContextCallback()), defaultConnectionType);
			_actions._displayWikiAction = new QAction("Prise en main (Wiki Magix 3D)", this);
			connect(_actions._displayWikiAction, SIGNAL(triggered()),
			        this, SLOT(wikiCallback()), defaultConnectionType);
			_actions._displayTutorialAction = new QAction("Tutoriels", this);
			connect(_actions._displayTutorialAction, SIGNAL(triggered()),
			        this, SLOT(tutorialCallback()), defaultConnectionType);
			_actions._displayPythonAPIUsersGuideAction =
					new QAction("Manuel utilisateur API Python/Magix 3D", this);
			connect(_actions._displayPythonAPIUsersGuideAction, SIGNAL(triggered()),
			        this, SLOT(pythonAPIUsersGuideCallback()), defaultConnectionType);
			_actions._displayQualifAction =
					new QAction(QString::fromUtf8("Qualif (Qualité de maillages)"), this);
			connect(_actions._displayQualifAction, SIGNAL(triggered()),
			        this, SLOT(qualifCallback()), defaultConnectionType);
			_actions._displayShortKeyAction =
					new QAction("Raccourcis clavier", this);
			connect(_actions._displayShortKeyAction, SIGNAL(triggered()),
			        this, SLOT(shortKeyCallback()), defaultConnectionType);
			_actions._displaySelectAction =
					new QAction(QString::fromUtf8("Sélection d'entités"), this);
			connect(_actions._displaySelectAction, SIGNAL(triggered()),
			        this, SLOT(selectCallback()), defaultConnectionType);
			_actions._displayHistoriqueAction =
					new QAction("Historique", this);
			connect(_actions._displayHistoriqueAction, SIGNAL(triggered()),
			        this, SLOT(historiqueCallback()), defaultConnectionType);
			_actions._aboutDialogAction = new QAction("A propos de ...", this);
			connect(_actions._aboutDialogAction, SIGNAL(triggered()), this,
			        SLOT(aboutDialogCallback()), defaultConnectionType);
		}    // QtMgx3DMainWindow::createActions


		void QtMgx3DMainWindow::createOperations()
		{
			int dim = 0;

			// Les opérations géométriques :
			// Vertex :
			QtMgx3DOperationAction *vertexAction =
					                       new QtVertexOperationAction(
							                       QIcon(":/images/geom_create_vertex.png"),
							                       QString::fromUtf8("Création de point"), *this,
							                       QString::fromUtf8("Création de point."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*vertexAction, QtMgx3DOperationsPanel::GEOM_POINT_OPERATION);
			// Segment :
			QtMgx3DOperationAction *segmentAction =
					                       new QtSegmentOperationAction(
							                       QIcon(":/images/geom_create_segment.png"),
							                       QString::fromUtf8("Création de segment"), *this,
							                       QString::fromUtf8("Création de segment."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*segmentAction, QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION);
			// Arc de cercle :
			QtMgx3DOperationAction *arcCircleAction =
					                       new QtArcCircleOperationAction(
							                       QIcon(":/images/geom_create_arc_circle.png"),
							                       QString::fromUtf8("Création d'arc de cercle"), *this,
							                       QString::fromUtf8("Création d'arc de cercle."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*arcCircleAction, QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION);
			// Cercle :
			QtMgx3DOperationAction *circleAction =
					                       new QtCircleOperationAction(
							                       QIcon(":/images/geom_create_circle.png"),
							                       QString::fromUtf8("Création d'un cercle/d'une ellipse"), *this,
							                       QString::fromUtf8("Création d'un cercle/d'une ellipse."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*circleAction, QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION);
			// Courbe par projection de courbe sur une surface :
			QtMgx3DOperationAction *curveByProjAction =
					                       new QtCurveByProjectionOnSurfaceAction(
							                       QIcon(":/images/geom_create_curve_by_proj.png"),
							                       QString::fromUtf8("Création d'une courbe par projection orthogonale\nd'une courbe sur une surface"), *this,
							                       QString::fromUtf8("Création d'une courbe par projection orthogonale d'une courbe sur une surface."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*curveByProjAction, QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION);
			// Surface :
			QtGeomSurfaceCreationAction *surfaceAction =
					                            new QtGeomSurfaceCreationAction(
							                            QIcon(
									                            ":/images/create_surface_with_contour.png"),
							                            QString::fromUtf8("Création de surface"), *this, QString::fromUtf8("Création de surface.")
					                            );
			CHECK_NULL_PTR_ERROR(surfaceAction->getSurfaceCreationPanel())
			surfaceAction->getSurfaceCreationPanel()->setDimension(1);
			registerOperationAction(
					*surfaceAction, QtMgx3DOperationsPanel::GEOM_SURFACE_OPERATION);
			// Boite :
			QtMgx3DOperationAction *boxAction =
					                       new QtBoxOperationAction(
							                       QIcon(":/images/box.png"), QString::fromUtf8("Création de boite"), *this,
							                       QString::fromUtf8("Création de boite."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*boxAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Sphère :
			QtMgx3DOperationAction *sphereAction =
					                       new QtSphereOperationAction(
							                       QIcon(":/images/create_sphere.png"), QString::fromUtf8("Création de sphère"),
							                       *this, QString::fromUtf8("Création de sphère."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*sphereAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Cylindre :
			QtMgx3DOperationAction *cylinderAction =
					                       new QtCylinderOperationAction(
							                       QIcon(":/images/cylinder.png"), QString::fromUtf8("Création de cylindre"), *this,
							                       QString::fromUtf8("Création de cylindre."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*cylinderAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Cone :
			QtMgx3DOperationAction *coneAction =
					                       new QtConeOperationAction(
							                       QIcon(":/images/create_cone.png"), QString::fromUtf8("Création de cone"), *this,
							                       QString::fromUtf8("Création de cone."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*coneAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Aiguille :
			QtMgx3DOperationAction *spherepartAction =
					                       new QtSpherePartOperationAction(
							                       QIcon(":/images/create_aiguille.png"), QString::fromUtf8("Création d'aiguille"), *this,
							                       QString::fromUtf8("Création d'aiguille."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*spherepartAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Prisme :
			QtMgx3DOperationAction *prismAction =
					                       new QtPrismCreationAction(
							                       QIcon(
									                       ":/images/create_prism.png"), QString::fromUtf8("Création de prisme"), *this,
							                       QString::fromUtf8("Création de prisme."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*prismAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Volume :
			QtGeomVolumeCreationAction *volumeAction =
					                           new QtGeomVolumeCreationAction(
							                           QIcon(
									                           ":/images/create_volume_with_surfaces.png"),
							                           QString::fromUtf8("Création de volume"), *this, QString::fromUtf8("Création de volume.")
					                           );
			CHECK_NULL_PTR_ERROR(volumeAction->getVolumeCreationPanel())
			registerOperationAction(
					*volumeAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Création d'entité par révolution :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeomEntityByRevolutionCreationAction *entityByRevolutionAction =
						                                       new QtGeomEntityByRevolutionCreationAction(
								                                       QIcon(
										                                       ":/images/create_geom_entity_by_revolution.png"),
								                                       QString::fromUtf8("Création d'entité géométrique par révolution"), *this,
								                                       QString::fromUtf8("Création d'entité géométrique par révolution.")
						                                       );
				CHECK_NULL_PTR_ERROR(entityByRevolutionAction->getGeomEntityByRevolutionPanel())
				entityByRevolutionAction->getGeomEntityByRevolutionPanel(
				)->setDimension(SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(*entityByRevolutionAction,
				                        (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION; ...
			// Copie d'entités géométriques :
			dim = 0;
			for (int                       ot                  = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeomEntityCopyOperationAction *copyAction =
						                                new QtGeomEntityCopyOperationAction(
								                                QIcon(":/images/geom_copy.png"),
								                                QString::fromUtf8("Copie d'entités géométriques"),
								                                *this, QString::fromUtf8("Copie d'entités géométriques"));
				CHECK_NULL_PTR_ERROR(copyAction->getCopyPanel())
				copyAction->getCopyPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*copyAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_POINT_OPERATION; ...
			// Création par copie avec décallage :
			QtGeomEntityCopyByOffsetAction *copyByOffsetAction =
					                               new QtGeomEntityCopyByOffsetAction(QIcon(":/images/geom_surface_offset_copy.png"),
					                                                                  QString::fromUtf8("Copie de surface géométrique avec décallage"),
					                                                                  *this, QString::fromUtf8("Copie de surface géométrique avec décallage"));
			CHECK_NULL_PTR_ERROR(copyByOffsetAction->getCopyPanel())
			registerOperationAction(*copyByOffsetAction, QtMgx3DOperationsPanel::GEOM_SURFACE_OPERATION);
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++)
				getOperationsPanel().addSeparator(
						(QtMgx3DOperationsPanel::OPERATION_TYPES) ot);

			// Collage de courbes :
			QtGeometryGlueCurvesAction *glueCurvesAction =
					                           new QtGeometryGlueCurvesAction(
							                           QIcon(":/images/geom_glue_curves.png"),
							                           QString::fromUtf8("Collage de courbes géométriques"),
							                           *this, QString::fromUtf8("Collage de courbes géométriques"));
			registerOperationAction(
					*glueCurvesAction, QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION);
			// Collage de surfaces :
			QtGeometryGlueSurfacesAction *glueSurfacesAction =
					                             new QtGeometryGlueSurfacesAction(
							                             QIcon(":/images/geom_glue_surfaces.png"),
							                             QString::fromUtf8("Collage de surfaces géométriques"),
							                             *this, QString::fromUtf8("Collage de surfaces géométriques"));
			registerOperationAction(
					*glueSurfacesAction, QtMgx3DOperationsPanel::GEOM_SURFACE_OPERATION);
			// Destruction d'entités géométriques :
			dim = 0;
			for (int               ot          = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeomEntityDestructionAction *geomDestructionAction =
						                              new QtGeomEntityDestructionAction(
								                              QIcon(":/images/box_destruction.png"),
								                              QString::fromUtf8("Destruction d'entités géométriques."),
								                              *this, QString::fromUtf8("Destruction d'entités géométriques.")
						                              );
				geomDestructionAction->getDestructionPanel()
						->getEntityByDimensionSelectorPanel()->setDimensions(
								SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*geomDestructionAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (QtMgx3DOperationsPanel::OPERATION_TYPES ot = ...
			// Union booleenne :
			QtMgx3DOperationAction *fuseAction =
					                       new QtBooleanOpOperationAction(
							                       QIcon(QPixmap((const char **) union_xpm)), QString::fromUtf8("Opérations Booléennes"), *this,
							                       QString::fromUtf8("Opérations booléennes sur des volumes."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*fuseAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);
			// Intersection :
			QtGeomEntityCommon2DOperationAction *common2DCurveAction =
					                                    new QtGeomEntityCommon2DOperationAction(
							                                    QIcon(":/images/geom_common2d_curves.png"),
							                                    QString::fromUtf8("Intersection de 2 courbes."), *this, 1,
							                                    QString::fromUtf8("Intersection de 2 courbes."), QtMgx3DGroupNamePanel::CREATION);
			registerOperationAction(
					*common2DCurveAction, QtMgx3DOperationsPanel::GEOM_CURVE_OPERATION);
			QtGeomEntityCommon2DOperationAction *common2DSurfaceAction =
					                                    new QtGeomEntityCommon2DOperationAction(
							                                    QIcon(":/images/geom_common2d_surfaces.png"),
							                                    QString::fromUtf8("Intersection de 2 surfaces."), *this, 2,
							                                    QString::fromUtf8("Intersection de 2 surfaces."), QtMgx3DGroupNamePanel::CREATION);
			registerOperationAction(
					*common2DSurfaceAction, QtMgx3DOperationsPanel::GEOM_SURFACE_OPERATION);
			// Coupe :
			FilterEntity::objectType cutTypes      = (FilterEntity::objectType)(
					FilterEntity::GeomSurface | FilterEntity::GeomVolume);
			SelectionManagerIfc::DIM cutDimensions = (SelectionManagerIfc::DIM)(
					SelectionManagerIfc::D2 | SelectionManagerIfc::D3);
			QtMgx3DOperationAction *cutAction =
					                       new QtGeomPlaneCutAction(
							                       QIcon(":/images/geom_plane_cut.png"), QString::fromUtf8("Coupe par un plan d'entités géométriques"),
							                       cutDimensions, cutTypes, *this,
							                       QString::fromUtf8("Coupe par un plan de surfaces géométriques."),
							                       QtMgx3DGroupNamePanel::CREATION
					                       );
			registerOperationAction(
					*cutAction, QtMgx3DOperationsPanel::GEOM_SURFACE_OPERATION);
			cutAction =
					new QtGeomPlaneCutAction(
							QIcon(":/images/geom_plane_cut.png"), QString::fromUtf8("Coupe par un plan d'entités géométriques"),
							cutDimensions, cutTypes, *this,
							QString::fromUtf8("Coupe par un plan de volumes géométriques."),
							QtMgx3DGroupNamePanel::CREATION
					);
			registerOperationAction(
					*cutAction, QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION);

			// Ajout à un groupe d'entités géométriques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeomAddToGroupOperationAction *addAction =
						                                new QtGeomAddToGroupOperationAction(
								                                QIcon(":/images/geom_add_to_group.png"),
								                                QString::fromUtf8("Ajout/suppression/affectation d'entités géométriques\nà un groupe"),
								                                *this, QString::fromUtf8("Ajout/suppression/affectation d'entités géométriques à un groupe"));
				CHECK_NULL_PTR_ERROR(addAction->getAdditionPanel())
				addAction->getAdditionPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*addAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_POINT_OPERATION; ...
			// Translation d'entités géométriques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeometryTranslationAction *translateAction =
						                            new QtGeometryTranslationAction(
								                            QIcon(":/images/geom_translation.png"),
								                            QString::fromUtf8("Translation d'entités géométriques"),
								                            *this, QString::fromUtf8("Translation d'entités géométriques"));
				CHECK_NULL_PTR_ERROR(translateAction->getTranslationPanel())
				translateAction->getTranslationPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*translateAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_POINT_OPERATION; ...
			// Rotation d'entités géométriques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeometryRotationAction *rotateAction =
						                         new QtGeometryRotationAction(
								                         QIcon(":/images/geom_rotation.png"),
								                         QString::fromUtf8("Rotation d'entités géométriques"),
								                         *this, QString::fromUtf8("Rotation d'entités géométriques"));
				CHECK_NULL_PTR_ERROR(rotateAction->getRotationPanel())
				rotateAction->getRotationPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*rotateAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_POINT_OPERATION; ...
			// Homothétie d'entités géométriques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeometryHomothetyAction *homothetyAction =
						                          new QtGeometryHomothetyAction(
								                          QIcon(":/images/geom_homothety.png"),
								                          QString::fromUtf8("Homothétie d'entités géométriques"),
								                          *this, QString::fromUtf8("Homothétie d'entités géométriques"));
				CHECK_NULL_PTR_ERROR(homothetyAction->getHomothetyPanel())
				homothetyAction->getHomothetyPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*homothetyAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_POINT_OPERATION; ...
			// Symétries d'entités géométriques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::GEOM_POINT_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::GEOM_VOLUME_OPERATION; ot++, dim++)
			{
				QtGeometryMirrorAction *mirrorAction =
						                       new QtGeometryMirrorAction(
								                       QIcon(":/images/geom_mirror.png"),
								                       QString::fromUtf8("Symétrie d'entités géométriques"),
								                       *this, QString::fromUtf8("Symétrie d'entités géométriques"));
				CHECK_NULL_PTR_ERROR(mirrorAction->getMirrorPanel())
				mirrorAction->getMirrorPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*mirrorAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::GEOM_POINT_OPERATION; ...


			// Les opérations topologiques :

			// Création d'entités topologiques :
			// Création de sommet :
			QtMgx3DOperationAction *vertexByAssociation =
					                       new QtTopologyVertexCreationAction(
							                       QIcon(":/images/create_vertex.png"), QString::fromUtf8("Création de sommet par association à un point"), *this,
							                       QString::fromUtf8("Création d'un sommet topologique par association à une point géométrique.")
					                       );
			registerOperationAction(
					*vertexByAssociation, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			// Création d'arêtes :
			QtMgx3DOperationAction *edgeByAssociation =
					                       new QtTopologyEdgeCreationAction(
							                       QIcon(":/images/create_edge.png"), QString::fromUtf8("Création d'arête par association à une courbe"), *this,
							                       QString::fromUtf8("Création d'une arête topologique par association à une courbe géométrique.")
					                       );
			registerOperationAction(
					*edgeByAssociation, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			// Création de faces :
			QtMgx3DOperationAction *faceByAssociation =
					                       new QtTopologyFaceCreationAction(
							                       QIcon(":/images/create_face.png"), QString::fromUtf8("Création de face par association à une surface"), *this,
							                       QString::fromUtf8("Création de face topologique par association à une surface géométrique.")
					                       );
			registerOperationAction(
					*faceByAssociation, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			// Création de blocs :
			QtMgx3DOperationAction *blockByAssociation =
					                       new QtTopologyBlockCreationAction(
							                       QIcon(":/images/create_block.png"), QString::fromUtf8("Création de bloc par association à un volume"), *this,
							                       QString::fromUtf8("Création de bloc par association à un volume.")
					                       );
			registerOperationAction(
					*blockByAssociation, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Création de blocs par révolution :
			QtMgx3DOperationAction *blockByRevolutionAction =
					                       new QtBlocksByRevolutionCreationAction(
							                       QIcon(":/images/create_block_by_edge_revolution.png"), QString::fromUtf8("Création de blocs par révolution"), *this,
							                       QString::fromUtf8("Création de blocs par révolution de la topologie 2D.")
					                       );
			registerOperationAction(
					*blockByRevolutionAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Création de blocs par extrusion :
            QtMgx3DOperationAction *blockByExtrusionAction =
                                           new QtBlocksByExtrusionCreationAction(
                                                   QIcon(":/images/create_block_by_extrusion.png"), QString::fromUtf8("Création de blocs par extrusion"), *this,
                                                   QString::fromUtf8("Création de blocs par extrusion")
                                           );
            registerOperationAction(
                    *blockByExtrusionAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Copie de blocs :
			QtMgx3DOperationAction *copyTopoAction =
					                       new QtTopoEntityCopyOperationAction(
							                       QIcon(":/images/topo_copy.png"), QString::fromUtf8("Copie d'entités topologiques"),
							                       *this, QString::fromUtf8("Copie d'entités topologiques.")
					                       );
			registerOperationAction(
					*copyTopoAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Extraction de blocs :
			QtMgx3DOperationAction *extractBlockAction =
					                       new QtTopologyBlockExtractionOperationAction(
							                       QIcon(":/images/topo_extract_block.png"),
							                       QString::fromUtf8("Extraction de blocs topologiques"), *this,
							                       QString::fromUtf8("Extrait des blocs (copie dans un groupe, suppression des originaux).")
					                       );
			registerOperationAction(
					*extractBlockAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Destruction d'entités topologique :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++, dim++)
			{
				QtTopoEntityDestructionAction *topoDestructionAction =
						                              new QtTopoEntityDestructionAction(
								                              QIcon(":/images/block_destruction.png"),
								                              QString::fromUtf8("Destruction d'entités topologiques."),
								                              *this, QString::fromUtf8("Destruction d'entités topologiques.")
						                              );
				registerOperationAction(
						*topoDestructionAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
				// initialise avec la bonne dimension
				topoDestructionAction->getDestructionPanel()->setDimensions(
						SelectionManagerIfc::dimensionToDimensions(dim));

			}    // for (QtMgx3DOperationsPanel::OPERATION_TYPES ot = ...
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++)
				getOperationsPanel().addSeparator(
						(QtMgx3DOperationsPanel::OPERATION_TYPES) ot);

			// Ajout à un groupe d'entités topologiques :
			dim = 0;
			for (int                ot                 = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++, dim++)
			{
				QtTopologyAddToGroupOperationAction *addAction =
						                                    new QtTopologyAddToGroupOperationAction(
								                                    QIcon(":/images/topo_add_to_group.png"),
								                                    QString::fromUtf8("Ajout/suppression/affectation d'entités topologiques\nà un groupe"),
								                                    *this, QString::fromUtf8("Ajout/suppression/affectation d'entités topologiques à un groupe"));
				CHECK_NULL_PTR_ERROR(addAction->getAdditionPanel())
				addAction->getAdditionPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*addAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION; ...
			// Coupe et collage d'entités topologiques :
			QtTopologyEdgeCutAction *topoCutEdgeAction =
					                        new QtTopologyEdgeCutAction(
							                        QIcon(":/images/edge_cutting.png"),
							                        QString::fromUtf8("Découpage d'une arête topologique ..."), *this,
							                        QString::fromUtf8("Découpage d'arête topologique."));
			registerOperationAction(
					*topoCutEdgeAction, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			// Coupe de faces :
			QtMgx3DOperationAction *splitFacesAction =
					                       new QtTopologySplitFacesAction(
							                       QIcon(":/images/topo_split_faces.png"), QString::fromUtf8("[2D] Découpage de faces selon une arête"), *this,
							                       QString::fromUtf8("Découpage de faces selon une arête (uniquement pour cas 2D).")
					                       );
			registerOperationAction(
					*splitFacesAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			QtMgx3DOperationAction *splitFaceAction =
					                       new QtTopologySplitFaceAction(
							                       QIcon(":/images/topo_split_face.png"), QString::fromUtf8("Découpage d'une face selon une arête"), *this,
							                       QString::fromUtf8("Découpage d'une face selon une arête.")
					                       );
			registerOperationAction(
					*splitFaceAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			QtMgx3DOperationAction *splitFaceWithOGridAction =
					                       new QtTopologySplitFaceWithOGridAction(
							                       QIcon(":/images/topo_split_face_with_ogrid.png"),
							                       QString::fromUtf8("Découpage de faces avec o-grid"), *this,
							                       QString::fromUtf8("Découpage de faces avec o-grid.")
					                       );
			registerOperationAction(*splitFaceWithOGridAction,
			                        QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			QtMgx3DOperationAction *extendSplitFaceAction =
					                       new QtTopologyExtendSplitFaceAction(
							                       QIcon(":/images/topo_extend_split_face.png"),
							                       QString::fromUtf8("Prolongation de découpage de face\nà partir d'un sommet situé sur une arête"), *this,
							                       QString::fromUtf8("Prolongation de découpage de face à partir d'un sommet situé sur une arête.")
					                       );
			registerOperationAction(
					*extendSplitFaceAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			// Insertion de trou dans la topologie par duplication de cofaces :
			QtTopologyInsertHoleAction *insertHoleAction =
					                           new QtTopologyInsertHoleAction(
							                           QIcon(":/images/topo_hole.png"),
							                           QString::fromUtf8("Insertion d'un trou dans la topologie"), *this,
							                           QString::fromUtf8("Insertion d'un trou dans la topologie."));
			registerOperationAction(
					*insertHoleAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			// Coupe de blocs :
			QtMgx3DOperationAction *splitBlockAction =
					                       new QtTopologySplitBlockAction(
							                       QIcon(":/images/topo_split_block.png"),
							                       QString::fromUtf8("Découpage de blocs selon une arête"), *this,
							                       QString::fromUtf8("Découpage de blocs selon une arête.")
					                       );
			registerOperationAction(
					*splitBlockAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			QtMgx3DOperationAction *splitBlockWithOGridAction =
					                       new QtTopologySplitBlockWithOGridAction(
							                       QIcon(":/images/topo_split_block_with_ogrid.png"),
							                       QString::fromUtf8("Découpage de blocs avec o-grid"), *this,
							                       QString::fromUtf8("Découpage de blocs avec o-grid.")
					                       );
			registerOperationAction(*splitBlockWithOGridAction,
			                        QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			QtMgx3DOperationAction *extendSplitBlockAction =
					                       new QtTopologyExtendSplitBlockAction(
							                       QIcon(":/images/topo_extend_split_block.png"),
							                       QString::fromUtf8("Prolongation de découpage de bloc selon une arête"), *this,
							                       QString::fromUtf8("Prolongation de découpage de bloc selon une arête.")
					                       );
			registerOperationAction(
					*extendSplitBlockAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Fusion de sommets :
			QtTopologyFuse2VerticesAction *glueVerticesAction =
					                              new QtTopologyFuse2VerticesAction(
							                              QIcon(":/images/topo_glue_2_vertices.png"),
							                              QString::fromUtf8("Fusion de sommets topologiques"),
							                              *this, QString::fromUtf8("Fusion de sommets topologiques (et des arêtes et faces si possible) entre 2 topologies"));
			registerOperationAction(
					*glueVerticesAction, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			// Fusion d'arêtes :
			QtTopologyFuseEdgesAction *glueEdgesAction =
					                          new QtTopologyFuseEdgesAction(
							                          QIcon(":/images/topo_glue_edges.png"),
							                          QString::fromUtf8("Fusion d'arêtes topologiques"),
							                          *this, QString::fromUtf8("Fusion d'arêtes topologiques (collage \"bout à bout\" des arêtes)"));
			registerOperationAction(
					*glueEdgesAction, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			QtTopologyFuse2EdgesAction *fuse2EdgesAction =
					                           new QtTopologyFuse2EdgesAction(
							                           QIcon(":/images/topo_glue_2_edges.png"),
							                           QString::fromUtf8("Fusion de 2 arêtes topologiques"),
							                           *this, QString::fromUtf8("Fusion de 2 arêtes topologiques (1 seule arête est conservée)"));
			registerOperationAction(
					*fuse2EdgesAction, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			// Collage de faces topologiques :
			QtTopologyFuse2FacesAction *glue2facesAction =
					                           new QtTopologyFuse2FacesAction(
							                           QIcon(":/images/topo_glue_2_faces.png"),
							                           QString::fromUtf8("Fusion de faces topologiques"),
							                           *this, QString::fromUtf8("Fusion de faces topologiques entre blocs pas forcément reliés, sommets proches"));
			registerOperationAction(
					*glue2facesAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			// Collage de vertex topologiques :
			dim = 0;
			for (int                    ot                 = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_FACE_OPERATION; ot++, dim++)
			{
				QtTopologyDegenerateBlockAction *degenerateBlockAction =
						                                new QtTopologyDegenerateBlockAction(
								                                QIcon(":/images/topo_degenerate_block.png"),
								                                QString::fromUtf8("Fusion de sommets topologiques"),
								                                *this, QString::fromUtf8("Fusion de sommets topologiques dans un même bloc, dégénérescence du bloc"));
				CHECK_NULL_PTR_ERROR(degenerateBlockAction->getTopologyDegenerateBlockPanel())
				degenerateBlockAction->getTopologyDegenerateBlockPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*degenerateBlockAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION; ...
			// Collage de blocs topologiques :
			QtTopologyGlue2BlocksAction *glue2blocksAction =
					                            new QtTopologyGlue2BlocksAction(
							                            QIcon(":/images/topo_glue_2_blocks.png"),
							                            QString::fromUtf8("Collage de blocs topologiques"),
							                            *this, QString::fromUtf8("Collage de blocs (ou fusion de faces de blocs) topologiques, sommets proches"));
			registerOperationAction(
					*glue2blocksAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Fusion de blocs topologiques :
			QtTopologyFuse2BlocksAction *fuse2blocksAction =
					                            new QtTopologyFuse2BlocksAction(
							                            QIcon(":/images/topo_fuse_2_blocks.png"),
							                            QString::fromUtf8("Fusion de blocs topologiques"),
							                            *this, QString::fromUtf8("Fusion de blocs topologiques structuré d'un même volume"));
			registerOperationAction(
					*fuse2blocksAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++)
				getOperationsPanel().addSeparator(
						(QtMgx3DOperationsPanel::OPERATION_TYPES) ot);

			// Associations entités topologiques -> entité(s) géométrique(s) :
			// Vertices :
			QtTopologyPlaceVertexAction *placeVertexAction =
					                            new QtTopologyPlaceVertexAction(
							                            QIcon(":/images/topo_vertex_position2_256.png"),
							                            QString::fromUtf8("Positionnement d'un sommet topologique"),
							                            *this, QString::fromUtf8("Positionnement d'un sommet topologique"));
			registerOperationAction(
					*placeVertexAction, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			QtTopologyProjectVerticesAction *projectVerticesAction =
					                                new QtTopologyProjectVerticesAction(
							                                QIcon(":/images/topo_project_vertices.png"),
							                                QString::fromUtf8("Modification des coordonnées de sommets topologiques"),
							                                *this, QString::fromUtf8("Modification des coordonnées de sommets topologiques"));
			registerOperationAction(
					*projectVerticesAction, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			QtTopologyVerticesAlignmentAction *alignVerticesAction =
					                                  new QtTopologyVerticesAlignmentAction(
							                                  QIcon(":/images/topo_vertices_alignment.png"),
							                                  QString::fromUtf8("Alignement de sommets topologiques"),
							                                  *this, QString::fromUtf8("Alignement de sommets topologiques"));
			registerOperationAction(
					*alignVerticesAction, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			QtTopologySnapVerticesAction *snapVerticesAction =
					                             new QtTopologySnapVerticesAction(
							                             QIcon(":/images/topo_snap_vertices.png"),
							                             QString::fromUtf8("Projection de sommets topologiques sur\nleur géométrie associée"),
							                             *this, QString::fromUtf8("Projection de sommets topologiques sur leur géométrie associée"));
			registerOperationAction(
					*snapVerticesAction, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			QtTopoEntitiesAssociationsAction *topoAssociationAction =
					                                 new QtTopoEntitiesAssociationsAction(
							                                 QIcon(":/images/topo_entities_association.png"),
							                                 QString::fromUtf8("Associations entités topologiques\n0D, 1D, 2D -> entité(s) géométrique(s)"),
							                                 *this,
							                                 QString::fromUtf8("Modification des associations entités topologiques (0/1/2D) vers entité(s) géométrique(s)."),
							                                 SelectionManagerIfc::D0
					                                 );
			CHECK_NULL_PTR_ERROR(topoAssociationAction->getAssociationPanel())
//	topoAssociationAction->getAssociationPanel ( )->setDimension (
//								SelectionManagerIfc::dimensionToDimensions (0));
			registerOperationAction(
					*topoAssociationAction, QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION);
			topoAssociationAction =
					new QtTopoEntitiesAssociationsAction(
							QIcon(":/images/topo_entities_association.png"),
							QString::fromUtf8("Associations entités topologiques\n0D, 1D, 2D -> entité géométrique"),
							*this,
							QString::fromUtf8("Modification des associations entités topologiques (0/1/2D) vers entité géométrique."),
							SelectionManagerIfc::D1
					);
			CHECK_NULL_PTR_ERROR(topoAssociationAction->getAssociationPanel())
			registerOperationAction(
					*topoAssociationAction, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			topoAssociationAction =
					new QtTopoEntitiesAssociationsAction(
							QIcon(":/images/topo_entities_association.png"),
							QString::fromUtf8("Associations entités topologiques\n0D, 1D, 2D -> entité géométrique"),
							*this,
							QString::fromUtf8("Modification des associations entités topologiques (0/1/2D) vers entité géométrique."),
							SelectionManagerIfc::D2
					);
			CHECK_NULL_PTR_ERROR(topoAssociationAction->getAssociationPanel())
			registerOperationAction(
					*topoAssociationAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			QtBlockAssociationAction *blockAssociationAction =
					                         new QtBlockAssociationAction(
							                         QIcon(":/images/blocks_association.png"),
							                         QString::fromUtf8("Associations blocs topologiques\n-> volume géométrique"),
							                         *this,
							                         QString::fromUtf8("Modification des associations blocs topologiques vers volume géométrique.")
					                         );
			registerOperationAction(
					*blockAssociationAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++)
				getOperationsPanel().addSeparator(
						(QtMgx3DOperationsPanel::OPERATION_TYPES) ot);

			// Discrétisation pour maillage :
			QtEdgeMeshingPropertyAction *edgeMeshingPropertyAction =
					                            new QtEdgeMeshingPropertyAction(
							                            QIcon(":/images/edges_meshing_properties.png"),
							                            QString::fromUtf8("Discrétisation des arêtes"), *this,
							                            QString::fromUtf8("Paramétrage de la discrétisation des arêtes.")
					                            );
			registerOperationAction(
					*edgeMeshingPropertyAction, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			QtTopologyEdgeDirectionAction *edgeDirectionAction =
					                              new QtTopologyEdgeDirectionAction(
							                              QIcon(":/images/topo_edge_direction.png"),
							                              QString::fromUtf8("Inversion du sens de discrétisation\nd'arêtes topologiques"),
							                              *this, QString::fromUtf8("Sens de discrétisation d'arêtes topologiques"));
			registerOperationAction(
					*edgeDirectionAction, QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION);
			QtFaceMeshingPropertyAction *faceMeshingPropertyAction =
					                            new QtFaceMeshingPropertyAction(
							                            QIcon(":/images/faces_meshing_properties.png"),
							                            QString::fromUtf8("Paramétrage du maillage des faces"), *this,
							                            QString::fromUtf8("Paramétrage du maillage des faces.")
					                            );
			registerOperationAction(
					*faceMeshingPropertyAction, QtMgx3DOperationsPanel::TOPO_FACE_OPERATION);
			QtBlockMeshingPropertyAction *blockMeshingPropertyAction =
					                             new QtBlockMeshingPropertyAction(
							                             QIcon(":/images/blocks_meshing_properties.png"),
							                             "Paramétrage du maillage des blocs", *this,
							                             "Paramétrage du maillage des blocs."
					                             );
			registerOperationAction(
					*blockMeshingPropertyAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			// Déraffinement de blocs topologiques :
			QtTopologyUnrefineBlockAction *unrefineBlockAction =
					                              new QtTopologyUnrefineBlockAction(
							                              QIcon(":/images/block_unrefinment.png"),
							                              QString::fromUtf8("Déraffinement de blocs"), *this,
							                              QString::fromUtf8("Déraffinement de blocs.")
					                              );
			registerOperationAction(
					*unrefineBlockAction, QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION);
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_EDGE_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++)
				getOperationsPanel().addSeparator(
						(QtMgx3DOperationsPanel::OPERATION_TYPES) ot);

			// Diverses opérations d'entités topologiques : tanslation, rotation, ...
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++, dim++)
			{
				QtTopologyTranslationAction *translateAction =
						                            new QtTopologyTranslationAction(
								                            QIcon(":/images/topo_translation.png"),
								                            QString::fromUtf8("Translation d'entités topologiques"),
								                            *this, QString::fromUtf8("Translation d'entités topologiques"));
				CHECK_NULL_PTR_ERROR(translateAction->getTranslationPanel())
				translateAction->getTranslationPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*translateAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION; ...
			// Rotation d'entités topologiques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++, dim++)
			{
				QtTopologyRotationAction *rotateAction =
						                         new QtTopologyRotationAction(
								                         QIcon(":/images/topo_rotation.png"),
								                         QString::fromUtf8("Rotation d'entités topologiques"),
								                         *this, QString::fromUtf8("Rotation d'entités topologiques"));
				CHECK_NULL_PTR_ERROR(rotateAction->getRotationPanel())
				rotateAction->getRotationPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*rotateAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION; ...
			// Homothétie d'entités topologiques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++, dim++)
			{
				QtTopologyHomothetyAction *homothetyAction =
						                          new QtTopologyHomothetyAction(
								                          QIcon(":/images/topo_homothety.png"),
								                          QString::fromUtf8("Homothétie d'entités topologiques"),
								                          *this, QString::fromUtf8("Homothétie d'entités topologiques"));
				CHECK_NULL_PTR_ERROR(homothetyAction->getHomothetyPanel())
				homothetyAction->getHomothetyPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*homothetyAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION; ...
			// Symétries d'entités topologiques :
			dim = 0;
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++, dim++)
			{
				QtTopologyMirrorAction *mirrorAction =
						                       new QtTopologyMirrorAction(
								                       QIcon(":/images/topo_mirror.png"),
								                       QString::fromUtf8("Symétrie d'entités topologiques"),
								                       *this, QString::fromUtf8("Symétrie d'entités topologiques"));
				CHECK_NULL_PTR_ERROR(mirrorAction->getMirrorPanel())
				mirrorAction->getMirrorPanel()->setDimension(
						SelectionManagerIfc::dimensionToDimensions(dim));
				registerOperationAction(
						*mirrorAction, (QtMgx3DOperationsPanel::OPERATION_TYPES) ot);
			}    // for (int ot = (int)QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION; ...
			for (int ot = (int) QtMgx3DOperationsPanel::TOPO_VERTEX_OPERATION;
			     ot <= (int) QtMgx3DOperationsPanel::TOPO_BLOCK_OPERATION; ot++)
				getOperationsPanel().addSeparator(
						(QtMgx3DOperationsPanel::OPERATION_TYPES) ot);

			// Les opérations maillages :
			// Mailler :
			QtMeshFacesOperationAction *meshFacesAction =
					                           new QtMeshFacesOperationAction(
							                           QIcon(":/images/mesh_all_surfaces.png"),
							                           QString::fromUtf8("Mailler les surfaces"), *this,
							                           QString::fromUtf8("Sélection des faces topologiques à mailler."));
			registerOperationAction(
					*meshFacesAction, QtMgx3DOperationsPanel::MESH_SURFACE_OPERATION);

			// Projeter surface :
			QtGroupProjectionOnPlaneAction *projectSurfaceAction =
					                               new QtGroupProjectionOnPlaneAction(
							                               QIcon(":/images/mesh_surfacic_projection.png"),
							                               QString::fromUtf8("Projeter des surfaces sur un plan\n(à effectuer avant de mailler)"), *this,
							                               QString::fromUtf8("Opération à faire avant la génération du maillage.\nEffectue la projection de surfaces d'un même groupe sur un plan du repère cartésien."));
			registerOperationAction(
					*projectSurfaceAction, QtMgx3DOperationsPanel::MESH_SURFACE_OPERATION);

			// Lisser surface :
			QtSmoothSurfaceOperationAction *smoothSurfaceAction =
					                               new QtSmoothSurfaceOperationAction(
							                               QIcon(":/images/mesh_surfacic_smoothing.png"),
							                               QString::fromUtf8("Lisser une surface"), *this,
							                               QString::fromUtf8("Sélection d'une surface de maillage à lisser."));
			registerOperationAction(
					*smoothSurfaceAction, QtMgx3DOperationsPanel::MESH_SURFACE_OPERATION);


			QtMeshBlocksOperationAction *meshBlocksAction =
					                            new QtMeshBlocksOperationAction(
							                            QIcon(":/images/mesh_all_volumes.png"),
							                            QString::fromUtf8("Mailler les blocs"),
							                            *this,
							                            QString::fromUtf8("Sélection des blocs topologiques à mailler."));
			registerOperationAction(
					*meshBlocksAction, QtMgx3DOperationsPanel::MESH_VOLUME_OPERATION);

			// Lisser volume :
			QtSmoothVolumeOperationAction *smoothVolumeAction =
					                              new QtSmoothVolumeOperationAction(
							                              QIcon(":/images/mesh_volumic_smoothing.png"),
							                              QString::fromUtf8("Lisser un volume"), *this,
							                              QString::fromUtf8("Sélection d'un volume de maillage à lisser."));
			registerOperationAction(
					*smoothVolumeAction, QtMgx3DOperationsPanel::MESH_VOLUME_OPERATION);

			// Extractions de volumes :
			// Entre 2 feuillets :
			QtMeshSubVolumeBetweenSheetsAction *sheetsSubVolumeAction =
					                                   new QtMeshSubVolumeBetweenSheetsAction(
							                                   QIcon(":/images/mesh_vol_sheet_extraction.png"),
							                                   QString::fromUtf8("Volume entre feuillets"), *this,
							                                   QString::fromUtf8("Extraire un sous-volume délimité par 2 feuillets."));
			registerOperationAction(
					*sheetsSubVolumeAction, QtMgx3DOperationsPanel::MESH_VOLUME_OPERATION);

//	// Extraction de feuillet :
//	QtMeshSheetOperationAction*	sheetAction	=
//		new QtMeshSheetOperationAction (
//				QIcon (":/images/create_sheet.png"), "Extraction de feuillets",
//				*this,
//				"Extraction de feuillets.",
//				QtMgx3DGroupNamePanel::CREATION, true /* Commande Mgx 3D */);
//	registerOperationAction (
//				*sheetAction, QtMgx3DOperationsPanel::MESH_VOLUME_OPERATION);


			// Création d'un repère :
			QtSysCoordOperationAction *sysCoordAction =
					                          new QtSysCoordOperationAction(
							                          QIcon(":/images/syscoord.png"),
							                          QString::fromUtf8("Création de repère"), *this,
							                          QString::fromUtf8("Création d'un repère à partir de points."),
							                          QtMgx3DGroupNamePanel::CREATION
					                          );
			registerOperationAction(
					*sysCoordAction, QtMgx3DOperationsPanel::SYSCOORD_OPERATION);

			// Rotation d'un repère
			QtSysCoordRotationAction *rotateAction =
					                         new QtSysCoordRotationAction(
							                         QIcon(":/images/syscoord_rotation.png"),
							                         QString::fromUtf8("Rotation d'un repère"),
							                         *this, QString::fromUtf8("Rotation d'un repère autour d'un axe"));
			registerOperationAction(
					*rotateAction, QtMgx3DOperationsPanel::SYSCOORD_OPERATION);

			// Translation d'un repère
			QtSysCoordTranslationAction *translateAction =
					                            new QtSysCoordTranslationAction(
							                            QIcon(":/images/syscoord_translation.png"),
							                            QString::fromUtf8("Translation d'un repère"),
							                            *this, QString::fromUtf8("Translation d'un repère suivant un vecteur"));
			registerOperationAction(
					*translateAction, QtMgx3DOperationsPanel::SYSCOORD_OPERATION);

		}    // QtMgx3DMainWindow::createOperations

#ifdef USE_EXPERIMENTAL_ROOM
		QWidget *QtMgx3DMainWindow::getExperimentalRoomPanel()
		{
			return _experimentalRoomPanel;
		}    // QtMgx3DMainWindow::getExperimentalRoomPanel


		void QtMgx3DMainWindow::setExperimentalRoomPanel(QWidget *panel)
		{
			if (panel == _experimentalRoomPanel)
				return;

			CHECK_NULL_PTR_ERROR(_entitiesTabWidget)
			delete _experimentalRoomPanel;
			_experimentalRoomPanel = panel;
			if (0 != _experimentalRoomPanel)
				_entitiesTabWidget->addTab(_experimentalRoomPanel, "Pointages laser");
		}    // QtMgx3DMainWindow::setExperimentalRoomPanel
#endif	// USE_EXPERIMENTAL_ROOM

		void QtMgx3DMainWindow::initPythonScripting()
		{
			CHECK_NULL_PTR_ERROR(_pythonPanel)
			_pythonPanel->setInterpreterName("Magix3D interpreter");
			// On facilite la tache de l'utilisateur : récupération du contexte de cette session :
			char *env = getenv("MGX3D_PATH");
			if (0 != env)
			{
				_pythonPanel->executeCommand("import sys");
				const string mgx3DPath(env);
				UTF8String   path(Charset::UTF_8);
				path << "sys.path.append(\"" << mgx3DPath << "\")";
				_pythonPanel->executeCommand(path);
				UTF8String importLine(Charset::UTF_8);
				importLine << "import pyMagix3D as " << getMgx3DAlias();
				_pythonPanel->executeCommand(importLine);
				UTF8String contextLine(Charset::UTF_8);
				contextLine << getContextAlias() << " = " << getMgx3DAlias()
				            << ".getContext(\"" << getContext().getName() << "\")";
				_pythonPanel->executeCommand(contextLine);

				// quelques raccourcis
				UTF8String raccourci(Charset::UTF_8);

				raccourci << "gm = " << getContextAlias() << ".getGeomManager()";
				_pythonPanel->executeCommand(raccourci);
				raccourci.clear();
				raccourci << "tm = " << getContextAlias() << ".getTopoManager()";
				_pythonPanel->executeCommand(raccourci);
				raccourci.clear();
				raccourci << "mm = " << getContextAlias() << ".getMeshManager()";
				_pythonPanel->executeCommand(raccourci);
				raccourci.clear();

				PythonWriter*	writer	=		getContext ( ).getScriptingManager ( ).getPythonWriter ( );
				if (0 != writer)
				{
					// Ca vient quelque part du noyau, mais ça n'est pas exécuté
					// => le QtMgx3DPythonPanel ne l'écrira pas dans le flux.
					ScriptingLog ins(getContextAlias(), "getGeomManager", "gm");
					writer->log(ins);
					ins.setResult("tm");
					ins.setMethodName("getTopoManager");
					writer->log(ins);
					ins.setResult("mm");
					ins.setMethodName("getMeshManager");
					writer->log(ins);
				}    // if ((0 != _pythonPanel->getMgxScriptingManager ( )) ...
			}    // if (0 != env)
			else    // if (0 != env)
			{
				UTF8String error(Charset::UTF_8);
				error << "La variable d'environnement MGX3D_PATH pointant sur le "
				      << "binding Python de Magix 3D n'est pas positionnée. "
				      << "L'exécution de commandes Python pourrait ne pas fonctionner.";
				log(WarningLog(error));
			}    // else if (0 != env)
		}    // QtMgx3DMainWindow::initPythonScripting


		void QtMgx3DMainWindow::registerOperationAction(
				QtMgx3DOperationAction &action,
				QtMgx3DOperationsPanel::OPERATION_TYPES type)
		{
			getOperationsPanel().registerOperationAction(action, type);
		}    // QtMgx3DMainWindow::registerOperationAction


		void QtMgx3DMainWindow::timerEvent(QTimerEvent *event)
		{
/* Pb : en cas d'actions dont la durée est supérieure au timer cette fonction
 * n'est pas appelée ... */
/*
time_t	t;
time (&t);
cout << ctime (&t);
*/
			updateActions();

			QMainWindow::timerEvent(event);
		}    // QtMgx3DMainWindow::timerEvent


		void QtMgx3DMainWindow::updateActions()
		{
/*
 * NOTE CP sur l'(in)activation des actions :
 * Objectif de cette fonction : activer les actions si le contexte s'y prête :
 * - Absence de commande en cours d'exécution
 * - Absence d'évènements Qt en attente. En effet, un évènement en attente
 * peut provoquer l'exécution d'un slot, et être incompatible avec une
 * éventuelle interruption de main loop visant à raffraichir la fenêtre
 * graphique. Une telle interruption invoque QApplication::processEvents qui
 * traitera tous les évènements de la même manière, qu'ils soient purement
 * graphiques (affichage et interactions VTK), ou non (clic sur des boutons,
 * ...). Or l'expérience montre qu'on ne peut bloquer par exemple plusieurs
 * clics successifs sur undo ce qui met en péril la gestion de la pile des
 * commandes.
 * Exercer un contrôle plus fin semble quasi-impossible. Depuis Qt 4 la boucle
 * infinie repose sur la GLib (fonctions g_main_*,
 * cf /usr/include/glib-2.0/glib/* et classe QEventDispatcherGlib) qui a pour
 * avantage d'être générique et pour inconvénient d'être générique ! L'accès
 * aux évènements à traiter est impossible et dès lors tout filtrage également.
 */
			if ((0 == _actions._undoAction) || (true == actionsDisabled()))
				return;

			BEGIN_QT_TRY_CATCH_BLOCK

			// Version du 10/01/11 :
			// Si au moins une commande est en cours d'exécution on inhibe la
			// création et le lancement de commandes, que ce soit via les menus ou via
			// la console python.
			bool enable = !getCommandManager().hasRunningCommands();
			if (true == _pythonPanel->isRunning())
				enable = false;
			QtAutoWaitingCursor cursor (!enable);

			// Faut-il le laisser ou le commenter ? Provoque des menus grisés alors
			// qu'aucune commande Mgx n'est en cours, mais des évènements Qt sont en attente
			// de traitement et susceptible d'être à l'origine de commandes Mgx.
			//	if (true == QApplication::hasPendingEvents ( ))
			//		enable	= false;

			// Menu session :
			string     undoCmd(getUndoManager().undoableCommandName());
			UTF8String undoText("Annuler", Charset::UTF_8), redoText("Rejouer", Charset::UTF_8);
			if (0 != undoCmd.length())
				undoText << ' ' << undoCmd;
			_actions._undoAction->setText(UTF8TOQSTRING(undoText));
			if (enable)
				_actions._undoAction->setEnabled(0 == undoCmd.length() ? false : true);
			else
				_actions._undoAction->setEnabled(false);
			string redoCmd(getUndoManager().redoableCommandName());
			if (0 != redoCmd.length())
				redoText << ' ' << redoCmd;
			_actions._redoAction->setText(UTF8TOQSTRING(redoText));
			if (enable)
				_actions._redoAction->setEnabled(0 == redoCmd.length() ? false : true);
			else
				_actions._redoAction->setEnabled(false);

			_actions.setEnabled(enable);
			if (0 != _pythonPanel)
				_pythonPanel->setUsabled(enable);

			// Actualisation du panneau "Opérations" :
			getOperationsPanel().setEnabled(enable);
			if (0 != _stateView)
				_stateView->setState(getCommandManager().getCommandName(), getCommandManager().getStatus());

			COMPLETE_QT_TRY_CATCH_BLOCK(QtMgx3DApplication::displayUpdatesErrors(), this, getAppTitle())

			if (true == hasError)
			{
				UTF8String message(Charset::UTF_8);
				message << "Echec lors de la mise à jour des actions : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::updateActions


		void QtMgx3DMainWindow::setContext(ContextIfc *context)
		{
			if (context == _context)
				return;

			QtAutoWaitingCursor cursor(true);

			BEGIN_QT_TRY_CATCH_BLOCK

			// Le contexte peut changer. C'est par ailleurs l'IHM qui gère les préférences utilisateurs. C'est donc ici un
			// bon endroit pour gérer le type de log à afficher/enregistrer :
			if (0 != context)
			{
				context->getLogDispatcher().setMask(QtMgx3DApplication::getLogsMask());
//		Context*	ctx	= dynamic_cast<Context*>(context);
//		if (0 != ctx)
//		{
//			ctx->getStdLogStream ( ).setMask (QtMgx3DApplication::getLogsMask ( ));
//			ctx->getErrLogStream ( ).setMask (QtMgx3DApplication::getLogsMask ( ));
//		}	// if (0 != ctx)
			}    // if (0 != context)

			if (0 != _groupsPanel)
				_groupsPanel->setContext(context);

			Context *ctx = dynamic_cast<Context *>(context);
			if ((0 != _graphicalWidget) && (0 != ctx))
				_graphicalWidget->getRenderingManager().setContext(ctx);

			// Destruction du contexte => récupérer les flux confiés car sinon ils vont être détruits :
			if (0 != _logView)
				getLogDispatcher().releaseStream(_logView);
			if (0 != _statusView)
				getLogDispatcher().releaseStream(_statusView);

			// ATTENTION :
			// Etape très délicate et très peu testée. Qu'en sera t-il lors d'un
			// changement de contexte en cours de session ? Le contexte est connu par de
			// très nombreux objets de très nombreuses classes, et ici on ne met pas tout à
			// jour ...
			setSelectionManager(0);
			delete _context;
			_context = context;

			if ((0 != _pythonPanel) && (0 != _context))
			{
				_pythonPanel->setLogStream(&(getContext().getLogDispatcher()));

				// On fait un fichier script utilisateur, qui contient les commandes saisies
				// par l'utilisateur et non celles générés par le noyau à partir des
				// commandes utilisateur :
				try
				{
					CHECK_NULL_PTR_ERROR(getContext().getScriptingManager().getPythonWriter())
					ScriptingManager* userScriptingMgr = new ScriptingManager(dynamic_cast<Internal::Context *>(&getContext()));
					userScriptingMgr->initPython(scriptToUserScript(getContext().getScriptingManager().getPythonWriter( )->getFileName()));
					_pythonPanel->setMgxUserScriptingManager(userScriptingMgr);
				}
				catch (...)
				{
				}
			}    // if ((0 != _pythonPanel) && (0 != _context))

			// S'enregistrer auprès des gestionnaires d'objets (commande, undo, ...) :
			registerToManagers();

			// Changement de contexte => changement de gestionnaire de logs :
			try
			{    // Le contexte est peut être nul.
				if (0 != _logView)
					getLogDispatcher().addStream(_logView);
				if (0 != _statusView)
					getLogDispatcher().addStream(_statusView);
				getLogDispatcher().enableDate(
						Resources::instance()._logDate, Resources::instance()._logTime);
				getLogDispatcher().enableThreadID(Resources::instance()._logThreadID);
			}
			catch (...)
			{
			}

			// Mise à jour de l'IHM :
			if (0 != _context)
				updateActions();

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				UTF8String message(Charset::UTF_8);
				message << "Echec lors du changement de contexte : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::setContext


		ContextIfc &QtMgx3DMainWindow::getContext()
		{
			if (0 == _context)
				throw Exception(UTF8String("QtMgx3DMainWindow::getContext : absence de contexte.", Charset::UTF_8));

			return *_context;
		}    // QtMgx3DMainWindow::getContext


		const ContextIfc &QtMgx3DMainWindow::getContext() const
		{
			if (0 == _context)
				throw Exception(UTF8String("QtMgx3DMainWindow::getContext : absence de contexte.", Charset::UTF_8));

			return *_context;
		}    // QtMgx3DMainWindow::getContext


void QtMgx3DMainWindow::executePythonScript(const string &f)
{
	File          file(f);
	UTF8String    message(Charset::UTF_8);
	TkUtil::Timer timer;
	message << "Exécution du script Python " << file.getFullFileName() << " en cours ...";

	log(InformationLog(message));

	getContext().beginImportScript();
	timer.start();
	try
	{
		_pythonPanel->executeFile(f);
		timer.stop();
		getContext().endImportScript();
	}
	catch (...)
	{
		timer.stop();
		getContext().endImportScript();
		throw;
	}
	// titre de la fenêtre principale
	UTF8String titre(Charset::UTF_8);
	titre << Resources::instance()._softwareName << " (" << MAGIX3D_VERSION/*GSCC_PROJECT_VERSION*/<< ") " << file.getFileName();
	setWindowTitle(UTF8TOQSTRING(titre));

	message.clear();
	message << "Exécution du script Python " << file.getFullFileName() << " avec succès (en "
			<< (unsigned long) timer.duration() << " secondes au total).";
	log(InformationLog(message));
}    // QtMgx3DMainWindow::executePythonScript


		CommandManagerIfc &QtMgx3DMainWindow::getCommandManager()
		{
			return getContext().getCommandManager();
		}    // QtMgx3DMainWindow::getCommandManager


/*SelectionManagerIfc& QtMgx3DMainWindow::getSelectionManager ( )
{
	return getContext ( ).getSelectionManager ( );
}	// QtMgx3DMainWindow::getSelectionManager


const SelectionManagerIfc& QtMgx3DMainWindow::getSelectionManager ( ) const
{
	return getContext ( ).getSelectionManager ( );
}	// QtMgx3DMainWindow::getSelectionManager*/


		void QtMgx3DMainWindow::selectionCleared()
		{
			LOCK_INSTANCE

			if (0 != _seizureManager)
				_seizureManager->selectionCleared();

		}   // QtMgx3DMainWindow::selectionCleared


		void QtMgx3DMainWindow::entitiesAddedToSelection(
				const vector<Entity *> &entities)
		{
			LOCK_INSTANCE

			if (0 != _seizureManager)
				_seizureManager->entitiesAddedToSelection(entities);
		}    // QtMgx3DMainWindow::entitiesAddedToSelection


		void QtMgx3DMainWindow::entitiesRemovedFromSelection(
				const vector<Entity *> &entities, bool clear)
		{
			LOCK_INSTANCE

			if (0 != _seizureManager)
				_seizureManager->entitiesRemovedFromSelection(entities, clear);
		}   // QtMgx3DMainWindow::entitiesRemovedFromSelection


		EntitySeizureManager *QtMgx3DMainWindow::getEntitySeizureManager()
		{
			return _seizureManager;
		}    // QtMgx3DMainWindow::getEntitySeizureManager


		void QtMgx3DMainWindow::setEntitySeizureManager(
				EntitySeizureManager *seizureManager)
		{
			LOCK_INSTANCE

			if (seizureManager == _seizureManager)
				return;

			if (0 != _seizureManager)
				_seizureManager->setInteractiveMode(false);

			_seizureManager = seizureManager;
		}    // QtMgx3DMainWindow::setEntitySeizureManager


		void QtMgx3DMainWindow::unregisterEntitySeizureManager(
				EntitySeizureManager *seizureManager)
		{
			LOCK_INSTANCE

			if (seizureManager == _seizureManager)
				_seizureManager = 0;
		}    // QtMgx3DMainWindow::unregisterEntitySeizureManager


		Geom::GeomManagerIfc &QtMgx3DMainWindow::getGeomManager()
		{
			return getContext().getGeomManager();
		}    // QtMgx3DMainWindow::getGeomManager


		Topo::TopoManagerIfc &QtMgx3DMainWindow::getTopoManager()
		{
			return getContext().getTopoManager();
		}   // QtMgx3DMainWindow::getTopoManager


		Mesh::MeshManagerIfc &QtMgx3DMainWindow::getMeshManager()
		{
			return getContext().getMeshManager();
		}   // QtMgx3DMainWindow::getMeshManager


		Group::GroupManagerIfc &QtMgx3DMainWindow::getGroupManager()
		{
			return getContext().getGroupManager();
		}   // QtMgx3DMainWindow::getGroupManager


		Internal::M3DCommandManager &QtMgx3DMainWindow::getM3DCommandManager()
		{
			return getContext().getM3DCommandManager();
		}

		UndoRedoManagerIfc &QtMgx3DMainWindow::getUndoManager()
		{
			return getCommandManager().getUndoManager();
		}    // QtMgx3DMainWindow::getUndoManager


		void QtMgx3DMainWindow::closeEvent(QCloseEvent *event)
		{
			if (1 == QMessageBox::warning(this, getAppTitle().c_str(),
			                              "Souhaitez-vous réellement quitter cette application ?",
			                              "Oui", "Non", QString::null, 0, -1))
			{
				event->ignore();
				return;
			}

			// On a tendance à quitter Magix 3D via QApplication::exit. S'il y a du python en route ça ne pardonne pas ...
			if (0 != _pythonPanel)
				_pythonPanel->setRunningMode (QtPythonConsole::RM_CONTINUOUS);

			QMainWindow::closeEvent(event);

			if (true == Resources::instance()._saveGuiState.getValue())
				writeSettings();

			// [EB] pour entrainer la fermeture de la fenêtre d'aide par exemple
			QApplication::exit();

		}    // QtMgx3DMainWindow::closeEvent


		QtGroupsPanel &QtMgx3DMainWindow::getGroupsPanel()
		{
			if (0 == _groupsPanel)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getGroupsPanel",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _groupsPanel)

			return *_groupsPanel;
		}    // QtMgx3DMainWindow::getGroupsPanel


		const QtGroupsPanel &QtMgx3DMainWindow::getGroupsPanel() const
		{
			if (0 == _groupsPanel)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getGroupsPanel",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _groupsPanel)

			return *_groupsPanel;
		}    // QtMgx3DMainWindow::getGroupsPanel


		QtEntitiesPanel &QtMgx3DMainWindow::getEntitiesPanel()
		{
			if (0 == _entitiesPanel)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getEntitiesPanel",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _entitiesPanel)

			return *_entitiesPanel;
		}    // QtMgx3DMainWindow::getEntitiesPanel


		const QtEntitiesPanel &QtMgx3DMainWindow::getEntitiesPanel() const
		{
			if (0 == _entitiesPanel)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getEntitiesPanel",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _entitiesPanel)

			return *_entitiesPanel;
		}    // QtMgx3DMainWindow::getEntitiesPanel


		QtMgx3DOperationsPanel &QtMgx3DMainWindow::getOperationsPanel()
		{
			if (0 == _operationsPanel)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getOperationsPanel",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _operationsPanel)

			return *_operationsPanel;
		}    // QtMgx3DMainWindow::getOperationsPanel


		void QtMgx3DMainWindow::registerAdditionalOperationPanel(
				QtMgx3DOperationPanel &panel)
		{
			LOCK_INSTANCE

			_additionalPanels.push_back(&panel);
			CHECK_NULL_PTR_ERROR(_operationsPanel)
			_operationsPanel->registerAdditionalOperationPanel(panel);
		}    // QtMgx3DMainWindow::registerAdditionalOperationPanel


		void QtMgx3DMainWindow::unregisterAdditionalOperationPanel(
				QtMgx3DOperationPanel &panel)
		{
			LOCK_INSTANCE

			for (vector<QtMgx3DOperationPanel *>::iterator ita =
					                                               _additionalPanels.begin();
			     _additionalPanels.end() != ita; ita++)
				if (&panel == *ita)
				{
					_additionalPanels.erase(ita);
					CHECK_NULL_PTR_ERROR(_operationsPanel)
					_operationsPanel->unregisterAdditionalOperationPanel(panel);
					break;
				}
		}    // QtMgx3DMainWindow::unregisterAdditionalOperationPanel


		bool QtMgx3DMainWindow::actionsDisabled() const
		{
			return _actionsDisabled;
		}    // QtMgx3DMainWindow::actionsDisabled


		bool QtMgx3DMainWindow::disableActions(bool disable)
		{
			LOCK_INSTANCE

			_actions.setEnabled(!disable);
			if (0 != _pythonPanel)
				_pythonPanel->setUsabled(!disable);
			getOperationsPanel().setEnabled(!disable);

			const bool already = _actionsDisabled;
			_actionsDisabled = disable;

			return already;
		}    // QtMgx3DMainWindow::disableActions


		const QtMgx3DOperationsPanel &QtMgx3DMainWindow::getOperationsPanel() const
		{
			if (0 == _operationsPanel)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getOperationsPanel",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _operationsPanel)

			return *_operationsPanel;
		}    // QtMgx3DMainWindow::getOperationsPanel


		void QtMgx3DMainWindow::registerToManagers()
		{
			CommandManagerIfc *cmdMgr = 0;    // 0 si pas de contexte
			UndoRedoManager   *urm    = 0;    // 0 si pas de contexte
			SelectionManager  *sm     = 0;    // 0 si pas de contexte
			// ATTENTION :
			// Etape très délicate et très peu testée. Qu'en sera t-il lors d'un
			// changement de contexte en cours de session ? Le contexte est connu par de
			// très nombreux objets de très nombreuses classes, et ici on ne met pas tout à
			// jour ...
			try
			{
				cmdMgr = dynamic_cast<CommandManagerIfc *>(&getCommandManager());
			}
			catch (...)
			{
			}
			if (0 != cmdMgr)
				cmdMgr->addCommandObserver(this);
			// TODO [CP] UndoRedoManager doit avoir addObserver
			try
			{
				urm = dynamic_cast<UndoRedoManager *>(&getUndoManager());
			}
			catch (...)
			{
			}
			if (0 != urm)
				urm->addObserver(this, false);
			getActions()._undoManager = urm;
			// Attention, tout cela n'est pas pleinement validé, il faut voir
			// createGui, ... _entitiesPanel et _graphicalWidget peuvent être
			// affecter, de même que plein d'autres trucs.
			try
			{
				if (0 != _context)
					setSelectionManager(&_context->getSelectionManager());
				sm = dynamic_cast<SelectionManager *>(getSelectionManager());
//		if (0 != sm)
//			sm->addSelectionObserver ((SelectionManagerObserverIfc&)*this);
			}
			catch (...)
			{
			}
			if (0 != _entitiesPanel)
			{
				_entitiesPanel->setSelectionManager(sm);
			}    // if (0 != _entitiesPanel)
			if (0 != _graphicalWidget)
				_graphicalWidget->getRenderingManager().setSelectionManager(sm);
			if (0 != _selectionCommonPropertiesPanel)
				_selectionCommonPropertiesPanel->setSelectionManager(sm);
			if (0 != _selectionIndividualPropertiesPanel)
				_selectionIndividualPropertiesPanel->setSelectionManager(sm);
		}    // QtMgx3DMainWindow::registerToManagers


		void QtMgx3DMainWindow::commandModified(Command &command, unsigned long event)
		{
			refreshGui();

			QtAutoWaitingCursor cursor(true);
			bool                doRender = false;    // Raffraichir la vue 3D ?
			CommandInternal *commandInternal = dynamic_cast<CommandInternal *>(&command);
			if (0 != commandInternal)
			{
				if ((COMMAND_STATE == event)
				    && ((Command::DONE == command.getStatus())
				        || (Command::INITED == command.getStatus())))
				{
					LOCK_INSTANCE

					if (0 != _stateView)
						_stateView->setState("Actualisation IHM", CommandIfc::PROCESSING);

					InfoCommand &icmd = commandInternal->getInfoCommand();
					InfoCommand::type t = InfoCommand::UNITIALIZED;
					icmd.getContextInfo(t);

					// Modification du context ?
					bool contextModification = false;
					switch (t)
					{
						case InfoCommand::LENGTHUNITMODIFIED    :
							contextModification = true;
							lengthUnitModifiedEvent();
							break;
						case InfoCommand::LANDMARKMODIFIED      :
							contextModification = true;
							landmarkModifiedEvent();
							break;
					}    // switch (t)
//			if (true == contextModification)
//				return;

					// recherche des différentes entités créées/supprimées :

					// Traitement 1 : entités géométriques :
					// Optimisation : on transmet des listes d'entités
					uint i = 0;
					vector < Geom::GeomEntity * > geomAddedShown, geomAddedHidden, geomRemoved, geomModified;
					for (i = 0; i < icmd.getNbGeomInfoEntity(); i++)
					{
						GeomEntity *ge = 0;
						icmd.getGeomInfoEntity(i, ge, t);
						CHECK_NULL_PTR_ERROR(ge)

						// on passe les entités non représentables
						if (!ge->getDisplayProperties().isDisplayable())
							continue;


						if (InfoCommand::CREATED == t)
						{
							bool displayed =
									     ge->getDisplayProperties().isDisplayed();
							doRender = true == displayed ? true : doRender;
							if (true == displayed)
								geomAddedShown.push_back(ge);
							else
								geomAddedHidden.push_back(ge);
						} else
							if (InfoCommand::DELETED == t)
							{
								geomRemoved.push_back(ge);
							} else
								if (InfoCommand::DISPMODIFIED == t)
								{    // Mettre à jour toutes les représentations de l'entité,
									// qu'elles soient graphiques, textuelles ou autres.
									if (true == (*ge).getDisplayProperties().isDisplayed())
										getGraphicalWidget().getRenderingManager( ).updateRepresentation(*ge);
									doRender = true;
								}

						if (0 == (i % Resources::instance()._updateRefreshRate.getValue()))
							refreshGui();
					}    // for (uint i=0; i<icmd.getNbGeomInfoEntity(); i++)
					if (0 != geomAddedShown.size())
						getEntitiesPanel().addCADEntities(geomAddedShown, true);
					if (0 != geomAddedHidden.size())
						getEntitiesPanel().addCADEntities(geomAddedHidden, false);
					if (0 != geomRemoved.size())
						getEntitiesPanel().removeCADEntities(geomRemoved);

					// Traitement 2 : entités topologiques :
					vector < Topo::TopoEntity * > topoAddedShown, topoAddedHidden, topoRemoved, topoModified;
					std::vector <InfoCommand::TopoEntityInfo> topo_entities_info = icmd.getSortedTopoInfoEntity();
					i = 0;
					for (std::vector<InfoCommand::TopoEntityInfo>::iterator iter_tei = topo_entities_info.begin(); iter_tei != topo_entities_info.end(); ++iter_tei, i++)
					{
						Topo::TopoEntity *te = (*iter_tei).m_topo_entity;
						t                    = (*iter_tei).m_type;

						CHECK_NULL_PTR_ERROR(te)

						// on passe les entités non représentables
						if (!te->getDisplayProperties().isDisplayable())
							continue;

						if (InfoCommand::CREATED == t)
						{
							bool displayed = te->getDisplayProperties().isDisplayed();
							doRender = true == displayed ? true : doRender;
							if (true == displayed)
								topoAddedShown.push_back(te);
							else
								topoAddedHidden.push_back(te);
						} else
							if (InfoCommand::DELETED == t)
								topoRemoved.push_back(te);
							else
								if (InfoCommand::DISPMODIFIED == t)
								{
									// Mettre à jour toutes les représentations de l'entité, qu'elles soient graphiques, textuelles ou autres.
									//std::cout<<" changement de représentation pour "<<te->getName()<<std::endl;
									if (true == (*te).getDisplayProperties().isDisplayed())
										getGraphicalWidget().getRenderingManager ( ).updateRepresentation(*te);
									doRender = true;
								} else
									if (InfoCommand::VISIBILYCHANGED == t)
									{
										// les entités déjà avec une représentation mais qui ont un changement de visibilité

										//std::cout<<" changement de visibilité pour "<<te->getName()<<std::endl;
										RenderedEntityRepresentation *rer = dynamic_cast<RenderedEntityRepresentation *>(te->getDisplayProperties().getGraphicalRepresentation());
										if ((0 != rer) && (0 == rer->getRenderingManager()))
											rer->setRenderingManager(&getGraphicalWidget().getRenderingManager());
										const bool displayed = te->getDisplayProperties().isDisplayed();
										// Bug workaround : displayed déjà tagué alors que displayRepresentation l'évalue ... => on inverse :
										te->getDisplayProperties().setDisplayed(!displayed);
										// Puis on réaffecte => forcera l'actualisation de la représentation :
										if (0 != rer)
											getGraphicalWidget().getRenderingManager( ).displayRepresentation(*te, displayed, rer->getRepresentationMask());
									}

						if (0 == (i % Resources::instance()._updateRefreshRate.getValue()))
							refreshGui();
					}    // for (uint i=0; i<icmd.getNbTopoInfoEntity(); i++)
					if (0 != topoAddedShown.size())
						getEntitiesPanel().addTopologicEntities(topoAddedShown, true);
					if (0 != topoAddedHidden.size())
						getEntitiesPanel().addTopologicEntities(topoAddedHidden, false);
					if (0 != topoRemoved.size())
						getEntitiesPanel().removeTopologicEntities(topoRemoved);

					// Traitement 3 : entités de maillage :
					vector < Mesh::MeshEntity * > meshAddedShown, meshAddedHidden, meshRemoved, meshModified;
					for (i = 0; i < icmd.getNbMeshInfoEntity(); i++)
					{
						MeshEntity *me = 0;
						icmd.getMeshInfoEntity(i, me, t);

						CHECK_NULL_PTR_ERROR(me)

						// on passe les entités non représentables
						if (!me->getDisplayProperties().isDisplayable())
							continue;

						if (InfoCommand::CREATED == t)
						{
							bool displayed = me->getDisplayProperties().isDisplayed();
							doRender = true == displayed ? true : doRender;
							if (true == displayed)
								meshAddedShown.push_back(me);
							else
								meshAddedHidden.push_back(me);
						} else
							if (InfoCommand::DELETED == t)
								meshRemoved.push_back(me);
							else
								if (InfoCommand::DISPMODIFIED == t)
								{
									if (true == (*me).getDisplayProperties().isDisplayed())
										getGraphicalWidget().getRenderingManager( ).updateRepresentation(*me);
									doRender = true;
								}
						if (0 == (i % Resources::instance()._updateRefreshRate.getValue()))
							refreshGui();
					}
					if (0 != meshAddedShown.size())
						getEntitiesPanel().addMeshEntities(meshAddedShown, true);
					if (0 != meshAddedHidden.size())
						getEntitiesPanel().addMeshEntities(meshAddedHidden, false);
					if (0 != meshRemoved.size())
						getEntitiesPanel().removeMeshEntities(meshRemoved);

					// Traitement 4 : entités de type groupes :
					vector < Group::GroupEntity * > groupsAdded, groupsRemoved;
					map < Group::GroupEntity * , InfoCommand::type > &groupsInfos = icmd.getGroupInfoEntity();
					for (map<Group::GroupEntity *, InfoCommand::type>::const_iterator itg = groupsInfos.begin(); groupsInfos.end() != itg; itg++)
					{
						switch ((*itg).second)
						{
							case InfoCommand::CREATED        :
							case InfoCommand::ENABLE        :
								groupsAdded.push_back((*itg).first);
								break;
							case InfoCommand::DELETED        :
							case InfoCommand::DISABLE        :
								groupsRemoved.push_back((*itg).first);
								break;
							case InfoCommand::DISPMODIFIED    :
								break;
						}    // switch ((*itg).second)
					}    // for (map<Group::GroupEntity*, type>::const_iterator ...
					if (0 != groupsAdded.size())
						getGroupsPanel().addGroups(groupsAdded);
					if (0 != groupsRemoved.size())
						getGroupsPanel().removeGroups(groupsRemoved);

					// Traitement 5 : entités repères :
					for (i = 0; i < icmd.getNbSysCoordInfoEntity(); i++)
					{
						CoordinateSystem::SysCoord *rep = 0;
						icmd.getSysCoordInfoEntity(i, rep, t);
						CHECK_NULL_PTR_ERROR(rep)

						// on passe les entités non représentables
						if (!rep->getDisplayProperties().isDisplayable())
							continue;

						if (InfoCommand::CREATED == t)
						{
							bool displayed =
									     rep->getDisplayProperties().isDisplayed();
							doRender = true == displayed ? true : doRender;
							if (true == displayed)
								getEntitiesPanel().addSysCoordEntity(*rep, true);
							else
								getEntitiesPanel().addSysCoordEntity(*rep, false);
						} else
							if (InfoCommand::DELETED == t)
							{
								getEntitiesPanel().removeSysCoordEntity(*rep);
							} else
								if (InfoCommand::DISPMODIFIED == t)
								{    // Mettre à jour toutes les représentations de l'entité,
									// qu'elles soient graphiques, textuelles ou autres.
									if (true == (*rep).getDisplayProperties().isDisplayed())
										getGraphicalWidget().getRenderingManager().updateRepresentation(*rep);
									doRender = true;
								}

						if (0 == (i % Resources::instance()._updateRefreshRate.getValue()))
							refreshGui();
					}    // for (uint i=0; i<icmd.getNbSysCoordInfoEntity(); i++)

					// Traitement 6 : entités maillages structurés :
					vector < Structured::StructuredMeshEntity * >
					smeshAddedShown, smeshAddedHidden, smeshRemoved, smeshModified;
					for (i = 0; i < icmd.getNbStructuredMeshInfoEntity(); i++)
					{
						StructuredMeshEntity *sme = 0;
						icmd.getStructuredMeshInfoEntity(i, sme, t);
						CHECK_NULL_PTR_ERROR(sme)

						// on passe les entités non représentables
						if (!sme->getDisplayProperties().isDisplayable())
							continue;

						if (InfoCommand::CREATED == t)
						{
							// Les maillages structurés sont un cas à part, car ça ne sont pas de vraies entités Mgx 3D. Ils n'appartiennent
							// pas à un groupe, donc ça n'est pas au GroupManager (via GroupManager::updateDisplayProperties) de déterminer
							// s'ils doivent être affichés ou non à la création. Le panneau QtEntitiesPanel n'a pas vocation à exister ...
							// On récupère l'info directement depuis le QtGroupsPanel :
//					bool	displayed	= sme->getDisplayProperties ( ).isDisplayed ( );
							bool displayed = 0 == (getGroupsPanel().getCheckedEntitiesTypes() & FilterEntity::StructuredMesh) ? false : true;
							doRender = true == displayed ? true : doRender;
							if (true == displayed)
								smeshAddedShown.push_back(sme);
							else
								smeshAddedHidden.push_back(sme);
						} else
							if (InfoCommand::DELETED == t)
							{
								smeshRemoved.push_back(sme);
							} else
								if (InfoCommand::DISPMODIFIED == t)
								{    // Mettre à jour toutes les représentations de l'entité, qu'elles soient graphiques, textuelles ou autres.
									if (true == (*sme).getDisplayProperties().isDisplayed())
										getGraphicalWidget().getRenderingManager().updateRepresentation(*sme);
									doRender = true;
								}

						if (0 == (i % Resources::instance()._updateRefreshRate.getValue()))
							refreshGui();
					}    // for (uint i=0; i<icmd.getNbStructuredMeshInfoEntity(); i++)
					if (0 != smeshAddedShown.size())
						getEntitiesPanel().addStructuredMeshEntities(smeshAddedShown, true);
					if (0 != smeshAddedHidden.size())
						getEntitiesPanel().addStructuredMeshEntities(smeshAddedHidden, false);
					if (0 != smeshRemoved.size())
						getEntitiesPanel().removeStructuredMeshEntities(smeshRemoved);

					// Prévenir les panneaux additionnels de l'IHM (explorateur et qualité de maillage, ...) en vue d'une éventuelle actualisation :
					for (vector<QtMgx3DOperationPanel *>::iterator itap = _additionalPanels.begin(); _additionalPanels.end() != itap; itap++)
						(*itap)->commandModifiedCallback(icmd);

					// Une commande vient de s'achever avec succès => afficher l'équivalent en Python en l'ajoutant à l'historique :
					const string output;
					if (0 != _pythonPanel && commandInternal->isScriptable())
					{
						const UTF8String	comments	= commandInternal->getScriptComments ( );
						if (false == comments.empty ( ))
							getContext ( ).getScriptingManager ( ).addComment(comments);
						getContext ( ).getScriptingManager ( ).addCommand (commandInternal->getScriptCommand ( ));
						_pythonPanel->addToHistoric(commandInternal->getScriptCommand(), commandInternal->getScriptComments(), output, false, true);
					}    // if (0 != _pythonPanel && commandInternal->isScriptable())

					if (Command::DONE == command.getStatus())
					{
						// A t'on un évènement important justifiant d'une notification via boite de dialogue ?
						const string warn = commandInternal->getWarningToPopup();
						if (false == warn.empty())
						{
							QtMessageBox::systemNotification ("Magix3D", QtMgx3DApplication::getAppIcon ( ), "Commandes terminées avec avertissement.", QtMessageBox::URGENCY_CRITICAL);
							if (false == Resources::instance ( )._showAmodalDialogOnCommandError.getValue ( ))
								QtMessageBox::displayWarningMessage (this, getAppTitle().c_str ( ), warn);	// Défaut
							else
								QtMessageBox::displayWarningMessageInAppWorkspace (this, getAppTitle ( ), warn);	// Expérimental, Issue#112
						}
					}

					if (0 != _stateView)
						_stateView->setState("Actualisation IHM", CommandIfc::DONE);

				}    // if ((COMMAND_STATE == event) && ...
				else
					if ((COMMAND_STATE == event) && (Command::FAIL == command.getStatus()))
					{
						if ((true == Resources::instance()._showDialogOnCommandError.getValue()) && (false == command.isUserNotified()))
						{
							command.setUserNotified (true);
							QtMessageBox::systemNotification ("Magix3D", QtMgx3DApplication::getAppIcon ( ), "Commandes terminées en erreur.", QtMessageBox::URGENCY_CRITICAL);
							if (false == Resources::instance ( )._showAmodalDialogOnCommandError.getValue ( ))
								QtMessageBox::displayErrorMessage (this, getAppTitle ( ), command.getErrorMessage ( ));	// défaut
							else
								QtMessageBox::displayErrorMessageInAppWorkspace (this, getAppTitle ( ), command.getErrorMessage ( ));	// Expérimental, Issue#112
						}

						// pour permettre de rejouer cette commande qui a échouée, elle est transmise avec le status en erreur, cela peut permettre de la corriger
						if (0 != _pythonPanel && commandInternal->isScriptable())
						{
							UTF8String	cmd (Charset::UTF_8);
							cmd << " Commande en Erreur: " << commandInternal->getScriptCommand ( ) << "\n" << " Message : " << commandInternal->getErrorMessage ( );
							const UTF8String	comments (commandInternal->getScriptComments ( ));
							if (false == comments.empty ( ))
								getContext ( ).getScriptingManager ( ).addComment(comments);

							// [EB] on ajoute un "#" au début de chacune des lignes
							UTF8String	cmd2 = addCharAtBeginLines('#', cmd);
							getContext ( ).getScriptingManager ( ).addCommand (cmd2);
							_pythonPanel->addToHistoric(commandInternal->getScriptCommand(), commandInternal->getScriptComments(), commandInternal->getErrorMessage(), true, true);
						}    // if (0 != _pythonPanel && commandInternal->isScriptable())

						// On est en erreur => réactualiser les menus de l'IHM selon ce nouveau context.
						updateActions();
					}
			}    // if (0 != commandInternal)

			if (0 != _stateView)
				_stateView->setState(getCommandManager().getCommandName(), getCommandManager().getStatus());
			command.setUserNotified(true);

			if (true == doRender)
			{
				// [EB] pour la première commande on recentre la vue en + du forceRender
				if (1 == getCommandManager().getCommandIfcs().size())
					getGraphicalWidget().getRenderingManager().resetView(true);
				else
				{
					getGraphicalWidget().getRenderingManager().updateView();
					getGraphicalWidget().getRenderingManager().forceRender();
				}
			}
			// Si une commande vient de s'achever peut être y en-a-t-il en file d'attente à lancer :
			if (COMMAND_STATE == event)
			{
				switch (command.getStatus())
				{
					case CommandIfc::DONE        :
					case CommandIfc::CANCELED    :
					case CommandIfc::FAIL        :
//				getCommandManager ( ).processQueuedCommands ( );
						if ((false == getCommandManager().hasRunningCommands()) && (false == getCommandManager().hasQueuedCommands()))
							updateActions();
						break;
					case CommandIfc::INITED        :
						if (CommandIfc::UNDO == command.getPlayType())
							getOperationsPanel().autoUpdate();
						break;
				}    // switch (command.getStatus( ))
			}    // if (COMMAND_STATE == event)
		}    // QtMgx3DMainWindow::commandModified


		void QtMgx3DMainWindow::lengthUnitModifiedEvent()
		{
			QAction *action = 0;
			switch (getContext().getLengthUnit())
			{
				case Unit::undefined    :
					action = getActions()._undefinedUnitAction;
					break;
				case Unit::micron    :
					action = getActions()._micrometerAction;
					break;
				case Unit::milimeter    :
					action = getActions()._millimeterAction;
					break;
				case Unit::centimeter    :
					action = getActions()._centimeterAction;
					break;
				case Unit::meter    :
					action = getActions()._meterAction;
					break;
			}    // switch (getContext ( ).getLengthUnit ( ))

			if (0 == action)
			{
				UTF8String message(Charset::UTF_8);
				message << "Unité non référencée : " << (unsigned long) getContext().getLengthUnit() << ".";
				INTERNAL_ERROR(exc, message, "QtMgx3DMainWindow::lengthUnitModifiedEvent")
				throw exc;
			}    // if (0 == action)

			action->setChecked(true);
		}    // QtMgx3DMainWindow::lengthUnitModifiedEvent


		void QtMgx3DMainWindow::landmarkModifiedEvent()
		{
			QAction *action = 0;

			switch (getContext().getLandmark())
			{
				case Landmark::maillage    :
					action = getActions()._meshLandmarkAction;
					break;
				case Landmark::chambre    :
					action = getActions()._expRoomLandmarkAction;
					break;
				case Landmark::undefined    :
					action = getActions()._undefinedLandmarkAction;
					break;
			}    // switch (getContext ( ).getLandmark ( ))

			if (0 == action)
			{
				UTF8String message(Charset::UTF_8);
				message << "Repère non référencé : " << (unsigned long) getContext().getLandmark() << ".";
				INTERNAL_ERROR(exc, message, "QtMgx3DMainWindow::landmarkModifiedEvent")
				throw exc;
			}    // if (0 == action)

			action->setChecked(true);
		}    // QtMgx3DMainWindow::landmarkModifiedEvent


		const Qt3DGraphicalWidget &QtMgx3DMainWindow::getGraphicalWidget() const
		{
			if (0 == _graphicalWidget)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getGraphicalWidget",
				               "Widget graphique non instancié.")
				throw exc;
			}    // if (0 == _graphicalWidget)

			return *_graphicalWidget;
		}    // QtMgx3DMainWindow::getGraphicalWidget


		void QtMgx3DMainWindow::log(const Log &log)
		{
			getLogDispatcher().log(log);
		}    // QtMgx3DMainWindow::log


		LogDispatcher &QtMgx3DMainWindow::getLogDispatcher()
		{
			return getContext().getLogDispatcher();
		}    // QtMgx3DMainWindow::getLogDispatcher


		const LogDispatcher &QtMgx3DMainWindow::getLogDispatcher() const
		{
			return getContext().getLogDispatcher();
		}    // QtMgx3DMainWindow::getLogDispatcher


		QtLogsView &QtMgx3DMainWindow::getLogView()
		{
			if (0 == _logView)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getLogView",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == logView)

			return *_logView;
		}    // QtMgx3DMainWindow::getLogView


		const QtLogsView &QtMgx3DMainWindow::getLogView() const
		{
			if (0 == _logView)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getLogView",
				               "Panneau non instancié.")
				throw exc;
			}    // if (0 == _logView)

			return *_logView;
		}    // QtMgx3DMainWindow::getLogView


		QMenu &QtMgx3DMainWindow::getProjectMenu()
		{
			if (0 == _projectMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getProjectMenu",
				               "Menu non instancié.")
				throw exc;
			}    // if (0 == _projectMenu)

			return *_projectMenu;
		}    // QtMgx3DMainWindow::getProjectMenu


		const QMenu &QtMgx3DMainWindow::getProjectMenu() const
		{
			if (0 == _projectMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getProjectMenu",
				               "Menu non instancié.")
				throw exc;
			}    // if (0 == _projectMenu)

			return *_projectMenu;
		}    // QtMgx3DMainWindow::getProjectMenu


		QMenu &QtMgx3DMainWindow::getSessionMenu()
		{
			if (0 == _sessionMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getSessionMenu",
				               "Menu non instancié.")
				throw exc;
			}    // if (0 == _sessionMenu)

			return *_sessionMenu;
		}    // QtMgx3DMainWindow::getSessionMenu


		const QMenu &QtMgx3DMainWindow::getSessionMenu() const
		{
			if (0 == _sessionMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getSessionMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _sessionMenu)

			return *_sessionMenu;
		}    // QtMgx3DMainWindow::getSessionMenu


		QMenu &QtMgx3DMainWindow::getViewMenu()
		{
			if (0 == _viewMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getViewMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _viewMenu)

			return *_viewMenu;
		}    // QtMgx3DMainWindow::getViewMenu


		const QMenu &QtMgx3DMainWindow::getViewMenu() const
		{
			if (0 == _viewMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getViewMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _viewMenu)

			return *_viewMenu;
		}    // QtMgx3DMainWindow::getViewMenu


		QMenu &QtMgx3DMainWindow::getCADMenu()
		{
			if (0 == _cadMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getCADMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _cadMenu)

			return *_cadMenu;
		}    // QtMgx3DMainWindow::getCADMenu


		const QMenu &QtMgx3DMainWindow::getCADMenu() const
		{
			if (0 == _cadMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getCADMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _cadMenu)

			return *_cadMenu;
		}    // QtMgx3DMainWindow::getCADMenu


		QMenu &QtMgx3DMainWindow::getTopologyMenu()
		{
			if (0 == _topologyMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getTopologyMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _topologyMenu)

			return *_topologyMenu;
		}    // QtMgx3DMainWindow::getTopologyMenu


		const QMenu &QtMgx3DMainWindow::getTopologyMenu() const
		{
			if (0 == _topologyMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getTopologyMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _topologyMenu)

			return *_topologyMenu;
		}    // QtMgx3DMainWindow::getTopologyMenu


		QMenu &QtMgx3DMainWindow::getMeshingMenu()
		{
			if (0 == _sessionMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getMeshingMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _sessionMenu)

			return *_sessionMenu;
		}    // QtMgx3DMainWindow::getMeshingMenu


		const QMenu &QtMgx3DMainWindow::getMeshingMenu() const
		{
			if (0 == _meshingMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getMeshingMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _meshingMenu)

			return *_meshingMenu;
		}    // QtMgx3DMainWindow::getMeshingMenu


		QMenu &QtMgx3DMainWindow::getToolsMenu()
		{
			if (0 == _toolsMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getToolsMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _toolsMenu)

			return *_toolsMenu;
		}    // QtMgx3DMainWindow::getToolsMenu


		const QMenu &QtMgx3DMainWindow::getToolsMenu() const
		{
			if (0 == _toolsMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getToolsMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _toolsMenu)

			return *_toolsMenu;
		}    // QtMgx3DMainWindow::getToolsMenu


		QMenu &QtMgx3DMainWindow::getHelpMenu()
		{
			if (0 == _helpMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getHelpMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _helpMenu)

			return *_helpMenu;
		}    // QtMgx3DMainWindow::getHelpMenu


		const QMenu &QtMgx3DMainWindow::getHelpMenu() const
		{
			if (0 == _helpMenu)
			{
				INTERNAL_ERROR(exc, "QtMgx3DMainWindow::getHelpMenu", "Menu non instancié.")
				throw exc;
			}    // if (0 == _helpMenu)

			return *_helpMenu;
		}    // QtMgx3DMainWindow::getHelpMenu


		void QtMgx3DMainWindow::refreshGui()
		{
			QtMgx3DApplication::flushEventQueue();
		}    // QtMgx3DMainWindow::refreshGui


		void QtMgx3DMainWindow::resizeEvent(QResizeEvent *event)
		{
			if (false == _maxSizeSet)
			{
				QDesktopWidget *desktopWidget = QApplication::desktop();
				if (0 != desktopWidget)
				{
					// La taille maximale possible de la fenêtre principale, bordures
					// (barre de titre/déplacement, ...) comprise.
					const QRect geom = desktopWidget->availableGeometry(this);

					// La taille complète de la fenêtre, barre de titre/déplacement
					// comprise :
					const QSize frameDims = frameSize();

					// La taille de la fenêtre hors bordures :
					const QSize mwDims = size();

					// On en déduit la taille des bordures :
					const QSize borders((frameDims.width() - mwDims.width()),
					                    (frameDims.height() - mwDims.height()));

					if (false == borders.isEmpty())    // => w, h > 0
					{
						const QSize maxSize(geom.width() - borders.width(),
						                    geom.height() - borders.height());
						cout << "Taille de la fenêtre Magix 3D limitée à " << geom.width() << "x" << geom.height()
						     << " (bordures comprises), c.a.d. à " << maxSize.width() << "x" << maxSize.height()
						     << " pour la zone de travail complète." << endl;
						setMaximumSize(maxSize);
						_maxSizeSet = true;
					}    // if (0 != desktopWidget)
				}    // if (false == borders.isEmpty ( ))
			}    // if (false == _maxSizeSet)

			QMainWindow::resizeEvent(event);
		}    // QtMgx3DMainWindow::resizeEvent


		void QtMgx3DMainWindow::setLandmark(Utils::Landmark::kind landmark)
		{
			QtAutoWaitingCursor cursor(true);

			UTF8String message(Charset::UTF_8);

			BEGIN_QT_TRY_CATCH_BLOCK

					DISABLE_COMMAND_ACTIONS
			DISABLE_GRAPHICAL_OPERATIONS
			getContext().setLandmark(landmark);

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				message << "Echec lors du changement de repere : "
				        << " : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::setLandmark


		bool QtMgx3DMainWindow::requireUnitMeter(Mgx3D::Utils::Unit::lengthUnit suggested)
		{
			QtLengthUnitDialog dialog(0, getAppTitle(), suggested);
			if (QDialog::Accepted != dialog.exec())
				return false;

			setUnitMeter(dialog.lengthUnit());

			return true;
		}   // QtMgx3DMainWindow::requireUnitMeter


		void QtMgx3DMainWindow::setUnitMeter(Unit::lengthUnit unit)
		{
			QtAutoWaitingCursor cursor(true);

			UTF8String message(Charset::UTF_8);

			BEGIN_QT_TRY_CATCH_BLOCK

					DISABLE_COMMAND_ACTIONS
			DISABLE_GRAPHICAL_OPERATIONS
			getContext().setLengthUnit(unit);

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				message << "Echec lors du changement d'unité de longueur : "
				        << " : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::setUnitMeter


		void QtMgx3DMainWindow::setMesh2D()
		{
			QtAutoWaitingCursor cursor(true);

			UTF8String message(Charset::UTF_8);

			BEGIN_QT_TRY_CATCH_BLOCK

					DISABLE_COMMAND_ACTIONS
			DISABLE_GRAPHICAL_OPERATIONS
			getContext().setMesh2D();

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				message << "Echec lors du changement de dimension pour le maillage : "
				        << " : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)

		}   // QtMgx3DMainWindow::setMesh2D


		File QtMgx3DMainWindow::selectFileName(
				QWidget *parent, const string &dir, const string &filter,
				const string &title, QFileDialog::FileMode fm,
				QFileDialog::AcceptMode am)
		{
			QFileDialog fileDialog(this, title.c_str());
			fileDialog.setNameFilter(filter.c_str());
			fileDialog.setFileMode(fm);
			fileDialog.setAcceptMode(am);
			fileDialog.setDirectory(dir.c_str());
			fileDialog.setOption(QFileDialog::DontUseNativeDialog);

			if (QDialog::Accepted != fileDialog.exec())
				return File("");

			QStringList fileList = fileDialog.selectedFiles();
			File        file(fileList[0].toStdString());

			return file;
		}    // QtMgx3DMainWindow::selectFileName


		Charset QtMgx3DMainWindow::getDefaultScriptsCharset() const
		{
			try
			{
				const Charset::CHARSET cs = Charset::str2charset(Resources::instance()._scriptsCharset.getValue().ascii().c_str());

				return Charset::UNKNOWN == cs ? Charset::UTF_8 : Charset(cs);
			}
			catch (...)
			{
			}

			return Charset::UTF_8;
		}    // QtMgx3DMainWindow::getDefaultScriptsCharset


		Charset QtMgx3DMainWindow::getDefaultRaysCharset() const
		{
			return Charset::UTF_8;
		}    // QtMgx3DMainWindow::getDefaultScriptsCharset
		
		
void QtMgx3DMainWindow::savePythonConsole (bool withEnv)
{
	static QString	consoleFilePath;
	static bool		saveWithEnv	= true;
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	File			file (consoleFilePath.toStdString ( ));
	static	Charset::CHARSET	charset	= Charset::str2charset (Resources::instance ( )._scriptsCharset.getValue ( ).ascii ( ).c_str ( ));
	QtMgx3DScriptFileDialog	dialog (this, "Magix 3D - Enregistrement console Python", false, false, true, charset, true);
	if ((false == _pythonMinScript.empty ( )) && (true == file.exists ( )))
		lastDir	= file.getPath ( ).getFullFileName ( ).c_str ( );
	dialog.setDirectory (lastDir);
	if (false == _pythonMinScript.empty ( ))
		dialog.selectFile (file.getFileName ( ).c_str ( ));
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setConfirmOverwrite (false);	// Done in french later ...
	QStringList	filters;
	filters << "Magix 3D (*.py)";
	dialog.setNameFilters (filters);
	dialog.setEnvironmentPython (saveWithEnv);

	string	fileName;
	while (0 == fileName.length ( ))
	{
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		QStringList	fileList	= dialog.selectedFiles ( );
		if (0 == fileList.size ( ))
			continue;

		try
		{
			fileName	= checkMagix3DScriptFileName (fileList [0].toStdString ( ));
		}
		catch (TkUtil::Exception& e)
		{
			UTF8String	error (Charset::UTF_8);
			error << e.getMessage ( ) << "\nSouhaitez-vous choisir un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear ( ); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		} // catch (TkUtil::Exception& e)
		
		file.setFullFileName (fileName);

		try 
		{	
			bool fileExists = file.exists ( );

			if (fileExists) 
			{
				UTF8String	warning (Charset::UTF_8);
				warning << "Le fichier " << fileName << "\nexiste déjà. Souhaitez-vous l'écraser ?";
				switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (warning), "Oui", "Non", "Annuler", 0, 2))
				{
					case	0	: break;
					case	1	: fileName.clear(); continue;
					case	2	: return;
				}	// switch (QMessageBox::warning (...
			} // if (fileExists)
		}
		catch (TkUtil::Exception& e) 
		{
			UTF8String	error (Charset::UTF_8);
			error << "TkUtils::Exception file.exists ( ) " << e.getMessage()
				  << "\nTest d'existence du fichier " << fileName
				  << "\nimpossible. Le droit en exécution sur l'un des répertoires du chemin est probablement manquant. "
				  << "\nSouhaitez-vous choisir un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		} // catch (TkUtil::Exception& e)

		if (false == file.isWritable ( ))
		{
			UTF8String	error (Charset::UTF_8);
			error << "Vous n'avez pas les droits en écriture sur le fichier\n" << fileName << ".\nSouhaitez-vous sélectionner un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		}	// if (false == file.isWritable ( ))
	}	// while (0 == fileName.length ( ))

	lastDir				= dialog.directory ( ).absolutePath ( );
	saveWithEnv			= dialog.getEnvironmentPython ( );
	charset				= dialog.getCharset ( );
	
	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK
	
	CHECK_NULL_PTR_ERROR (getPythonPanel ( ))

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);

	getPythonPanel ( )->saveConsoleScript (fileName, charset, saveWithEnv);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'enregistrement de la console python dans le " << " fichier " << " :" << "\n" << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
}	// QtMgx3DMainWindow::savePythonConsole


		void QtMgx3DMainWindow::preferencesCallback()
		{
			BEGIN_QT_TRY_CATCH_BLOCK

					QtMgx3DApplication::editConfiguration(this, UTF8String("Magix 3D : préférences utilisateur.", Charset::UTF_8));

			// Les préférences ont peut être été modifiées => on actualise ce qui doit
			// l'être :
			// Interacteur :
			if (0 != _graphicalWidget)
				_graphicalWidget->updateConfiguration();

			// Vue 3D :
			_actions._displayTrihedronAction->setChecked(
					Resources::instance()._displayTrihedron.getValue());
//	_actions._displayLandmarkAction->setChecked (
//						Resources::instance ( )._displayLandmark.getValue ( ));
			_actions._displayFocalPointAction->setChecked(
					Resources::instance()._displayFocalPoint.getValue());

			// Logs :
			Log::TYPE mask = QtMgx3DApplication::getLogsMask();
			getLogDispatcher().setMask(mask);
//	Context*	context	= dynamic_cast<Context*>(&getContext ( ));
//	if (0 != context)
//	{
//		context->getStdLogStream ( ).setMask (
//										QtMgx3DApplication::getLogsMask ( ));
//		context->getErrLogStream ( ).setMask (
//										QtMgx3DApplication::getLogsMask ( ));
//	}
			getLogDispatcher().enableDate(
					Resources::instance()._logDate, Resources::instance()._logTime);
			getLogDispatcher().enableThreadID(Resources::instance()._logThreadID);
			getLogView().setLogMask(mask);
			if (0 != _statusView)
				_statusView->setMask(mask);
			if (getActions()._recentFilesCapacity !=
			    Resources::instance()._recentScriptCapacity.getValue())
				updateRecentScriptsURLFifoCapacity(
						Resources::instance()._recentScriptCapacity.getValue());
// Tester les logs :
/*
InformationLog  i ("Information");
WarningLog  w ("Warning");
ErrorLog    e ("Error");
ProcessLog  p ("Executable", "Process");
ScriptingLog    s ("Variable_name", "method");
TraceLog    t1 ("Trace 1", Log::TRACE_1);
TraceLog    t2 ("Trace 2", Log::TRACE_2);
TraceLog    t3 ("Trace 3", Log::TRACE_3);
TraceLog    t4 ("Trace 4", Log::TRACE_4);
TraceLog    t5 ("Trace 5", Log::TRACE_5);
log (i);
log (w);
log (e);
log (p);
log (s);
log (t1);
log (t2);
log (t3);
log (t4);
log (t5);
*/

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::preferencesCallback


		void QtMgx3DMainWindow::editSettingsCallback()
		{
			BEGIN_QT_TRY_CATCH_BLOCK

			const bool oldLoad = Resources::instance()._loadGuiState.getValue();
			const bool oldSave = Resources::instance()._saveGuiState.getValue();
			QSettings  settings(Resources::instance()._organisation.c_str(),
			                    Resources::instance()._softwareName.c_str());

			QtGuiStateDialog dialog(
					this, "Magix 3D - Sauvegarde/Restauration de l'tat de l'IHM",
					&Resources::instance()._loadGuiState,
					&Resources::instance()._saveGuiState,
					settings.fileName().toStdString());
			if (1 == dialog.exec())
			{    // MAJ éventuelle des opérations automatiques effectuées en début et
				// fin de session :
				if ((oldLoad != Resources::instance()._loadGuiState.getValue()) ||
				    (oldSave != Resources::instance()._saveGuiState.getValue()))
				{
					unique_ptr <Section> section(
							QtMgx3DApplication::getPersistantConfiguration());
					CHECK_NULL_PTR_ERROR(section.get())
					QtMgx3DApplication::updateConfiguration(
							*(section.get()), Resources::instance()._loadGuiState);
					QtMgx3DApplication::updateConfiguration(
							*(section.get()), Resources::instance()._saveGuiState);
					QtMgx3DApplication::setPersistantConfiguration(*(section.get()));
				}    // if ((oldLoad != Resources::instance ( )._loadGuiState.getValue (...

				// Tout est fait automatiquement par la boite de dialogue sauf
				// une sauvegarde immédiate de l'état de l'IHM :
				if (true == dialog.immediatelySaveState())
					writeSettings();
			}    // if (1 == dialog.exec ( ))

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::editSettingsCallback


		void QtMgx3DMainWindow::exitCallback()
		{

#ifndef _DEBUG
			if (0 != QtMessageBox::displayWarningMessage(this, getAppTitle(),
			                                             "Souhaitez-vous réellement quitter cette application ?", 100,
			                                             "Oui", "Non", 0, 0))
				return;
			QApplication::exit();

#else	// #ifndef _DEBUG

																																	BEGIN_QT_TRY_CATCH_BLOCK

	if (false == close ( ))
		return;

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, getAppTitle ( ))

	if (true == hasError)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Echec lors de la fermeture de l'application "
		        << " : " << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)

#endif    // #ifndef _DEBUG
		}    // QtMgx3DMainWindow::exitCallback


		void QtMgx3DMainWindow::undoCallback()
		{
			QtAutoWaitingCursor cursor(true);

			UTF8String message(Charset::UTF_8);

			BEGIN_QT_TRY_CATCH_BLOCK

					DISABLE_COMMAND_ACTIONS
			DISABLE_GRAPHICAL_OPERATIONS
			getContext().undo();

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				message << "Echec lors de l'annulation de la dernière commande "
				        << " : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::undoCallback


		void QtMgx3DMainWindow::redoCallback()
		{
			QtAutoWaitingCursor cursor(true);

			UTF8String message(Charset::UTF_8);

			BEGIN_QT_TRY_CATCH_BLOCK

					DISABLE_COMMAND_ACTIONS
			DISABLE_GRAPHICAL_OPERATIONS
			getContext().redo();

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				message << "Echec lors du rejeu de la dernière commande "
				        << " : " << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::redoCallback


		void QtMgx3DMainWindow::reinitializeCallback()
		{
			QtAutoWaitingCursor cursor(true);

			LOCK_INSTANCE

			try
			{
				// Des panneaux "opération", actifs ou non, peuvent avoir en cache des
				// pointeurs vers des entités en passe d'être détruites. On purge ici :
				if (0 != _operationsPanel)
					_operationsPanel->sessionCleared();
			}
			catch (...)
			{
			}

			BEGIN_QT_TRY_CATCH_BLOCK

			getContext().clearSession();

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())
		}    // QtMgx3DMainWindow::reinitializeCallback


		void QtMgx3DMainWindow::unitMeterCallback()
		{
			setUnitMeter(Unit::meter);
		}    // QtMgx3DMainWindow::unitMeterCallback


		void QtMgx3DMainWindow::unitCentimeterCallback()
		{
			setUnitMeter(Unit::centimeter);
		}    // QtMgx3DMainWindow::unitCentimeterCallback


		void QtMgx3DMainWindow::unitMillimeterCallback()
		{
			setUnitMeter(Unit::milimeter);
		}    // QtMgx3DMainWindow::unitMillimeterCallback


		void QtMgx3DMainWindow::unitMicrometerCallback()
		{
			setUnitMeter(Unit::micron);
		}    // QtMgx3DMainWindow::unitMicrometerCallback


		void QtMgx3DMainWindow::unitUndefinedCallback()
		{
			setUnitMeter(Unit::undefined);
		}    // QtMgx3DMainWindow::unitUndefinedCallback


		void QtMgx3DMainWindow::meshLandmarkCallback()
		{
			setLandmark(Utils::Landmark::maillage);
		}    // QtMgx3DMainWindow::meshLandmarkCallback


		void QtMgx3DMainWindow::expRoomLandmarkCallback()
		{
			setLandmark(Utils::Landmark::chambre);
		}    // QtMgx3DMainWindow::expRoomLandmarkCallback


		void QtMgx3DMainWindow::undefinedLandmarkCallback()
		{
			setLandmark(Utils::Landmark::undefined);
		}    // QtMgx3DMainWindow::undefinedLandmarkCallback


		void QtMgx3DMainWindow::mesh2DDimCallback()
		{
			setMesh2D();
		}   //QtMgx3DMainWindow::mesh2DDimCallback


		void QtMgx3DMainWindow::importCallback()
		{
			static QString lastDir(Process::getCurrentDirectory().c_str());
			static QString lastFilter("");

			QFileDialog dialog(this, "Magix 3D - Importation", lastDir, lastFilter);
			dialog.setFileMode(QFileDialog::ExistingFiles);
			dialog.setAcceptMode(QFileDialog::AcceptOpen);
			dialog.setOption(QFileDialog::DontUseNativeDialog);
			QStringList filters;
			filters << "Magix MDL (*.mdl)";
			filters << "Catia (*.CATPart *.CATProduct)";
			filters << "BREP (*.brep)";
			filters << "STEP (*.stp *.step)";
			filters << "IGES (*.igs *.iges)";
			UTF8String limaFilter(Charset::UTF_8);
			limaFilter << "Lima++ (";
			const char **limaExt = Lima::liste_format_lecture();
			size_t count = 0;
			while (NULL != *limaExt)
			{
				string extension(*limaExt);
				size_t comma = extension.find(',');
				if (string::npos != comma)
					extension = extension.substr(0, comma);
				if (0 == count)
					limaFilter << "*." << extension;
				else
					limaFilter << " *." << extension;
				limaExt++;
				count++;
			}    // while (NULL != *limaExt)
			limaFilter << ")";
//	filters << limaFilter.iso ( ).c_str ( );
			filters << UTF8TOQSTRING(limaFilter);
			dialog.setNameFilters(filters);

			if (QDialog::Accepted != dialog.exec())
				return;

			lastDir    = dialog.directory().absolutePath();
			lastFilter = dialog.selectedNameFilter();

			UTF8String message(Charset::UTF_8);

			BEGIN_QT_TRY_CATCH_BLOCK

			DISABLE_COMMAND_ACTIONS
			DISABLE_GRAPHICAL_OPERATIONS

			QStringList                fileList            = dialog.selectedFiles();
			string                     fileName;
			bool                       splitAll            = false;
			bool                       splitCompoundCurves = false;
			for (QStringList::Iterator it                  = fileList.begin(); fileList.end() != it;
			     it++)
			{
				fileName = (*it).toStdString();
				UTF8String msg(Charset::UTF_8);
				msg << "Importation des données du fichier " << fileName
				    << " au format " << lastFilter.toStdString() << ".";

				File file(fileName);
				if (true == compareExtensions(file.getExtension(), "mdl"))
				{
					QtMdlOptionsDialog mdlDialog(this, getAppTitle(), fileName);
					if (QDialog::Rejected == mdlDialog.exec())
					{
						msg << " Opération annulée par l'utilisateur.";
						log(InformationLog(msg));
						return;
					}    // if (QDialog::Rejected == mdlDialog.exec ( ))

					QtAutoWaitingCursor cursor(true);
					const bool          importAll    = mdlDialog.importAllContours();
					const bool          useAreaNames = mdlDialog.useAreaNames();
					const string        prefixName   = mdlDialog.getPrefix();
					const int           degMin       = mdlDialog.getDegMinBSpline();
					const int           degMax       = mdlDialog.getDegMaxBSpline();
					msg << " Importation de la topologie : "
					    << (true == mdlDialog.importTopology() ? "oui" : "non")
					    << ", tout importer : " << (true == importAll ? "oui" : "non")
					    << ", conserver les noms de zones : " << (true == useAreaNames ? "oui" : "non")
					    << ", décomposer en commandes élémentaires : "
					    << (true == mdlDialog.splitMgx3DCommands() ? "oui" : "non")
					    << ", préfixe : " << prefixName
					    << ", degMin : " << (short) degMin
					    << ", degMax : " << (short) degMax
					    << ".";
					log(InformationLog(msg));
					if (false == mdlDialog.splitMgx3DCommands())    // Défaut
					{
						if (true == mdlDialog.importTopology())
							getContext().getTopoManager().importMDL(fileName, importAll, useAreaNames, prefixName, degMin, degMax);
						else
							getContext().getGeomManager().importMDL(fileName, importAll, useAreaNames, prefixName, degMin, degMax);
					}    // if (false == mdlDialog.splitMgx3DCommands ( ))
					else
					{
						getContext().getGeomManager().mdl2CommandesMagix3D(fileName, mdlDialog.importTopology());
					}    // else if (false == mdlDialog.splitMgx3DCommands ( ))
				}    // Magix 2D
				else
					if ((true == compareExtensions(file.getExtension(), "CATPart")) ||
					    (true == compareExtensions(file.getExtension(), "CATProduct")))
					{
						if (false == splitAll)
						{
							QtImportOptionsDialog optionsDialog(this, getAppTitle(), fileName);
							if (QDialog::Rejected == optionsDialog.exec())
							{
								msg << " Opération annulée par l'utilisateur.";
								log(InformationLog(msg));
								return;
							}    // if (QDialog::Rejected == optionsDialog.exec ( ))

							splitCompoundCurves = optionsDialog.splitCompoundCurves();
							splitAll            = optionsDialog.forAll();
						}   // if (false == splitAll)

						log(InformationLog(msg));
						QtAutoWaitingCursor cursor(true);
						getContext().getGeomManager().importCATIA(fileName, true, splitCompoundCurves);
					}    // Catia
					else
						if ((true == compareExtensions(file.getExtension(), "stp")) ||
						    (true == compareExtensions(file.getExtension(), "step")))
						{
							if (false == splitAll)
							{
								QtImportOptionsDialog optionsDialog(this, getAppTitle(), fileName);
								if (QDialog::Rejected == optionsDialog.exec())
								{
									msg << " Opération annulée par l'utilisateur.";
									log(InformationLog(msg));
									return;
								}    // if (QDialog::Rejected == optionsDialog.exec ( ))

								splitCompoundCurves = optionsDialog.splitCompoundCurves();
								splitAll            = optionsDialog.forAll();
							}   // if (false == splitAll)
							log(InformationLog(msg));
							QtAutoWaitingCursor cursor(true);
							getContext().getGeomManager().importSTEP(fileName, true, splitCompoundCurves);
						}    // STEP
						else
							if ((true == compareExtensions(file.getExtension(), "igs")) ||
							    (true == compareExtensions(file.getExtension(), "iges")))
							{
								if (false == splitAll)
								{
									QtImportOptionsDialog optionsDialog(this, getAppTitle(), fileName);
									if (QDialog::Rejected == optionsDialog.exec())
									{
										msg << " Opération annulée par l'utilisateur.";
										log(InformationLog(msg));
										return;
									}    // if (QDialog::Rejected == optionsDialog.exec ( ))

									splitCompoundCurves = optionsDialog.splitCompoundCurves();
									splitAll            = optionsDialog.forAll();
								}   // if (false == splitAll)
								log(InformationLog(msg));

								if (Utils::Unit::undefined == getContext().getLengthUnit())
								{   // Suggérer l'unité de longueur du fichier IGES si elle est compatible
									IGESHeader header;
									UTF8String infos;
									header.read(fileName, infos);
									Unit::lengthUnit igesUnit = header.getMgxUnit(false);
									if (false == requireUnitMeter(igesUnit))
									{
										msg << " Opération annulée par l'utilisateur.";
										log(InformationLog(msg));
										return;
									}   // if (false == requireUnitMeter (igesUnit))
								}   // if (Utils::Unit::undefined == getContext ( ).getLengthUnit ( ))

								QtAutoWaitingCursor cursor(true);
								getContext().getGeomManager().importIGES(fileName, splitCompoundCurves);
							}    // IGES
							else
								if (true == compareExtensions(file.getExtension(), "brep"))
								{
									if (false == splitAll)
									{
										QtImportOptionsDialog optionsDialog(this, getAppTitle(), fileName);
										if (QDialog::Rejected == optionsDialog.exec())
										{
											msg << " Opération annulée par l'utilisateur.";
											log(InformationLog(msg));
											return;
										}    // if (QDialog::Rejected == optionsDialog.exec ( ))

										splitCompoundCurves = optionsDialog.splitCompoundCurves();
										splitAll            = optionsDialog.forAll();
									}   // if (false == splitAll)
									log(InformationLog(msg));

									QtAutoWaitingCursor cursor(true);
									getContext().getGeomManager().importBREP(fileName, true, splitCompoundCurves);
								}    // BREP
								else
									if (Lima::SUFFIXE != Lima::_Reader::detectFormat(file.getFileName()))
									{
										// [EB] sélection d'un préfixe ...
										bool   ok  = false;
										string pre = QInputDialog::getText(this, "Sélection d'un préfixe", "Préfixe :", QLineEdit::Normal, "", &ok).toStdString();
										if (false == ok)
											return;
										msg << " Avec comme préfixe pour les noms de groupes : " << pre;
										QtAutoWaitingCursor cursor(true);
										log(InformationLog(msg));
										getContext().getMeshManager().readMli(fileName, pre);
									}    // Lima
									else
									{
										UTF8String error(Charset::UTF_8);
										error << "Format du fichier " << fileName << " non supporté.";
										throw Exception(error);
									}
			}    // for (QStringList::Iterator it = fileList.begin ( ); ...

			COMPLETE_QT_TRY_CATCH_BLOCK(true, this, getAppTitle())

			if (true == hasError)
			{
				message << "Echec lors de l'importation des données contenues dans le "
				        << " fichier " << " :" << "\n"
				        << errorString;
				log(ErrorLog(message));
			}    // if (true == hasError)
		}    // QtMgx3DMainWindow::importCallback


static string limaWExtensions ( )
{
	UTF8String      extensions ("LIMA++ (");
	const char**	limaExt	= Lima::liste_format_ecriture ( );
	size_t          count   = 0;
	while (NULL != *limaExt)
	{
		string	extension (*limaExt);
		size_t	comma	= extension.find (',');
		if (string::npos != comma)
			extension	= extension.substr (0, comma);
		if (0 == count)
			extensions << "*." << extension;
		else
			extensions << " *." << extension;
		limaExt++;
		count++;
	}   // while (NULL != *limaExt)
	extensions << ")";

	return extensions.utf8 ( );
}   // limaWExtensions


static bool isLimaWExtension (const string& checked)
{
	const char**	limaExt	= Lima::liste_format_ecriture ( );
	while (NULL != *limaExt)
	{
		string	extension (*limaExt);
		size_t	comma	= extension.find (',');
		if (string::npos != comma)
			extension	= extension.substr (0, comma);
		if (true == compareExtensions (checked, extension))
			return true;
		limaExt++;
	}   // while (NULL != *limaExt)

	return false;
}   // limaWExtensions


void QtMgx3DMainWindow::exportAllCallback ( )
{
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	static QString	lastFilter ("");

	string		fileName;
	File		file ("");
	QFileDialog	dialog(this, "Magix 3D - Exportation", lastDir, lastFilter);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setConfirmOverwrite (false);	// Done in french later ...
	QStringList	filters;
	filters << limaWExtensions ( ).c_str ( );
	filters << "Magix MDL (*.mdl)";
	filters << "BREP (*.brep)";
	filters << "STEP (*.stp *.step)";
	filters << "IGES (*.igs *.iges)";
	filters << "CGNS (*.cgns)";
	dialog.setNameFilters (filters);

	while (0 == fileName.length ( ))
	{
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		QStringList	fileList	= dialog.selectedFiles ( );
		if (0 == fileList.size ( ))
			continue;

		const vector<string>	extensions	= QtFileDialogUtilities::nameFilterToExtensions (dialog.selectedNameFilter ( ));
		const string	extension	= 0 == extensions.size ( ) ? string ( ) : extensions [0];
		file.setFullFileName (fileList [0].toStdString ( ));
		if ((0 == file.getExtension ( ).length ( )) && (0 != extensions.size ( )))
			file.setFullFileName (fileList [0].toStdString ( ) + "." + extensions [0]);
		// Vérifier si l'extension correspond au filtre :
		else if (false == QtFileDialogUtilities::extensionMatches (file.getExtension ( ), extensions))
		{
			UTF8String	error;
			error << "L'extension du fichier " << file.getFullFileName ( ) << "\n ne correspond pas au filtre sélectionné (" << dialog.selectedNameFilter ( ).toStdString ( ) << ")";
			QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error),  "OK", QString::null, QString::null, 0, -1);
			fileName.clear ( );
			continue;
		}
		fileName	= file.getFullFileName ( );

		try {
			bool fileExists = file.exists ( );

			if (fileExists) {
				UTF8String	warning (Charset::UTF_8);
				warning << "Le fichier " << fileName << "\nexiste déjà. Souhaitez-vous l'écraser ?";
				switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (warning), "Oui", "Non", "Annuler", 0, 2))
				{
					case	0	: break;
					case	1	: fileName.clear(); continue;
					case	2	: return;
				}	// switch (QMessageBox::warning (...
			} // if (fileExists)
		}
		catch (TkUtil::Exception& e) {
			UTF8String	error (Charset::UTF_8);
			error << "TkUtils::Exception file.exists ( ) " << e.getMessage()
				  << "\nTest d'existence du fichier " << fileName
				  << "\nimpossible. Le droit en exécution sur l'un des répertoires du chemin "
				  << " est probablement manquant. "
				  << "\nSouhaitez-vous choisir un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		} // catch (TkUtil::Exception& e)

		if (false == file.isWritable ( ))
		{
			UTF8String	error (Charset::UTF_8);
			error << "Vous n'avez pas les droits en écriture sur le fichier\n" << fileName
			      << ".\nSouhaitez-vous sélectionner un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error),  "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		}	// if (false == file.isWritable ( ))
	}	// while (0 == fileName.length ( ))

	lastDir		= dialog.directory ( ).absolutePath ( );
	lastFilter	= dialog.selectedNameFilter ( );
	
	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);

	UTF8String	msg (Charset::UTF_8);
	msg << "Exportation de toutes les données dans le fichier " << fileName
	    << " au format " << lastFilter.toStdString ( ) << ".";

	if (true == compareExtensions (file.getExtension ( ), "mdl"))
	{
		vector<string>	geomEntities	= getContext ( ).getGeomManager ( ).getSurfaces ( );
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportMDL (geomEntities, fileName);
	}	// Magix 2D
	else if (true == compareExtensions (file.getExtension ( ), "brep"))
	{
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportBREP (fileName);
	}	// BREP
	else if ((true == compareExtensions (file.getExtension ( ), "stp")) ||
	         (true == compareExtensions (file.getExtension ( ), "step")))
	{
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportSTEP (fileName);
	}	// STEP
	else if ((true == compareExtensions (file.getExtension ( ), "igs")) ||
            (true == compareExtensions (file.getExtension ( ), "iges")))
	{
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportIGES (fileName);
	}	// IGES
	else if (true == isLimaWExtension (file.getExtension ( )))
	{
		SelectionManagerIfc&	selectionManager	= getContext ( ).getSelectionManager ( );
		log (InformationLog (msg));
		getContext ( ).getMeshManager ( ).writeMli (fileName);
	}	// Lima++
	else if (true == compareExtensions (file.getExtension ( ), "cgns"))
	{
		SelectionManagerIfc&	selectionManager	=
									getContext ( ).getSelectionManager ( );
		log (InformationLog (msg));
		getContext ( ).getMeshManager ( ).writeCGNS (fileName);
	}	// cgns
	else
	{
		UTF8String	error (Charset::UTF_8);
		error << "Format du fichier " << fileName << " non supporté.";
		throw Exception (error);
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'exportation des données dans le fichier " << " :" << "\n"
		        << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
}	// QtMgx3DMainWindow::exportAllCallback


void QtMgx3DMainWindow::exportSelectionCallback ( )
{
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	static QString	lastFilter ("");

	string		fileName;
	File		file ("");
	QFileDialog	dialog(
		this, "Magix 3D - Exportation de la sélection", lastDir, lastFilter);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setConfirmOverwrite (false);	// Done in french later ...
	QStringList	filters;
	filters << "Magix MDL (*.mdl)";
	filters << "BREP (*.brep)";
	filters << "STEP (*.stp *.step)";
	filters << "IGES (*.igs *.iges)";
//	filters << "Lima++ (*.unf *.mli)";
//	filters << "CGNS (*.cgns)";
	dialog.setNameFilters (filters);

	while (0 == fileName.length ( ))
	{
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		QStringList	fileList	= dialog.selectedFiles ( );
		if (0 == fileList.size ( ))
			continue;

		fileName	= fileList [0].toStdString ( );
		file.setFullFileName (fileName);

		try {
			bool fileExists = file.exists ( );

			if (fileExists) {
				UTF8String	warning (Charset::UTF_8);
				warning << "Le fichier " << fileName
				        << "\nexiste déjà. Souhaitez-vous l'écraser ?";
				switch (QMessageBox::warning (this, "Magix 3D",
				        UTF8TOQSTRING(warning), "Oui", "Non", "Annuler", 0, 2))
				{
					case	0	: break;
					case	1	: fileName.clear(); continue;
					case	2	: return;
				}	// switch (QMessageBox::warning (...
			} // if (fileExists)
		}
		catch (TkUtil::Exception& e) {
			UTF8String	error (Charset::UTF_8);
			error << "TkUtils::Exception file.exists ( ) " << e.getMessage()
				  << "\nTest d'existence du fichier " << fileName
				  << "\nimpossible. Le droit en exécution sur l'un des répertoires du chemin "
				  << " est probablement manquant. "
				  << "\nSouhaitez-vous choisir un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D",
					UTF8TOQSTRING (error),
					"Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		} // catch (TkUtil::Exception& e)

		if (false == file.isWritable ( ))
		{
			UTF8String	error (Charset::UTF_8);
			error << "Vous n'avez pas les droits en écriture sur le fichier\n"
			      << fileName
			      << ".\nSouhaitez-vous sélectionner un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D",
			        UTF8TOQSTRING (error), 
			        "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		}	// if (false == file.isWritable ( ))
	}	// while (0 == fileName.length ( ))

	lastDir		= dialog.directory ( ).absolutePath ( );
	lastFilter	= dialog.selectedNameFilter ( );
	
	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);
	SelectionManagerIfc&	selectionManager	= getContext ( ).getSelectionManager ( );
	vector<string>			selection;

	UTF8String	msg (Charset::UTF_8);
	msg << "Exportation des données sélectionnées dans le fichier " << fileName
	    << " au format " << lastFilter.toStdString ( ) << ".";

	if (true == compareExtensions (file.getExtension ( ), "mdl"))
	{
		selection	= getContext ( ).getSelectionManager ( ).getEntitiesNames (
				(FilterEntity::objectType)
				(FilterEntity::GeomSurface|FilterEntity::GeomCurve));
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportMDL (selection, fileName);
	}	// Magix 2D
	else if (true == compareExtensions (file.getExtension ( ), "brep"))
	{
		selection	= getContext ( ).getSelectionManager ( ).getEntitiesNames (
													FilterEntity::AllGeom);
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportBREP (selection, fileName);
	}	// BREP
	else if ((true == compareExtensions (file.getExtension ( ), "stp")) ||
	         (true == compareExtensions (file.getExtension ( ), "step")))
	{
		selection	= getContext ( ).getSelectionManager ( ).getEntitiesNames (
													FilterEntity::AllGeom);
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportSTEP (selection, fileName);
	}	// STEP
	else if ((true == compareExtensions (file.getExtension ( ), "igs")) ||
	        (true == compareExtensions (file.getExtension ( ), "iges")))
	{
		selection	= getContext ( ).getSelectionManager ( ).getEntitiesNames (
													FilterEntity::AllGeom);
		log (InformationLog (msg));
		getContext ( ).getGeomManager ( ).exportIGES (selection, fileName);
	}	// IGES
	else
	{
		UTF8String	error (Charset::UTF_8);
		error << "Format du fichier " << fileName << " non supporté.";
		throw Exception (error);
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'exportation de la sélection des données dans le fichier " << " :" << "\n"
		        << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
}	// QtMgx3DMainWindow::exportSelectionCallback


void QtMgx3DMainWindow::saveMagix3DScriptCallback ( )
{
	if (true == _pythonMinScript.empty ( ))
	{
		saveAsMagix3DScriptCallback ( );
		return;
	}	// if (true == _pythonMinScript.empty ( ))

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);

	getContext ( ).savePythonScript (_pythonMinScript.c_str ( ), _encodageScripts, _pytMinScriptCharset);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	UTF8String	message (Charset::UTF_8);
	if (true == hasError)
	{
		message << "Echec lors de l'exportation des données dans le fichier " << " :" << "\n" << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
	else {
		// titre de la fenêtre principale
		UTF8String	titre (Charset::UTF_8);
		titre << Resources::instance ( )._softwareName<<" ("<<MAGIX3D_VERSION/*GSCC_PROJECT_VERSION*/<< ") " << _pythonMinScript;
		setWindowTitle(UTF8TOQSTRING (titre));
	}
}	// QtMgx3DMainWindow::saveMagix3DScriptCallback


void QtMgx3DMainWindow::saveAsMagix3DScriptCallback ( )
{
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	File			file (_pythonMinScript);
	Charset::CHARSET	charset	= Charset::str2charset (Resources::instance ( )._scriptsCharset.getValue ( ).ascii ( ).c_str ( ));
	QtMgx3DScriptFileDialog	dialog (this, "Magix 3D - Enregistrement commandes Python", false, true, false, charset, true);
	if ((false == _pythonMinScript.empty ( )) && (true == file.exists ( )))
		lastDir	= file.getPath ( ).getFullFileName ( ).c_str ( );
	dialog.setDirectory (lastDir);
	if (false == _pythonMinScript.empty ( ))
		dialog.selectFile (file.getFileName ( ).c_str ( ));
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setConfirmOverwrite (false);	// Done in french later ...
	QStringList	filters;
	filters << "Magix 3D (*.py)";
	dialog.setNameFilters (filters);

	string	fileName;
	while (0 == fileName.length ( ))
	{
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		QStringList	fileList	= dialog.selectedFiles ( );
		if (0 == fileList.size ( ))
			continue;

		try
		{
			fileName	= checkMagix3DScriptFileName (fileList [0].toStdString ( ));
		}
		catch (TkUtil::Exception& e)
		{
			UTF8String	error (Charset::UTF_8);
			error << e.getMessage ( ) << "\nSouhaitez-vous choisir un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear ( ); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		} // catch (TkUtil::Exception& e)
		
		file.setFullFileName (fileName);

		try {
			bool fileExists = file.exists ( );

			if (fileExists) {
				UTF8String	warning (Charset::UTF_8);
				warning << "Le fichier " << fileName << "\nexiste déjà. Souhaitez-vous l'écraser ?";
				switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (warning), "Oui", "Non", "Annuler", 0, 2))
				{
					case	0	: break;
					case	1	: fileName.clear(); continue;
					case	2	: return;
				}	// switch (QMessageBox::warning (...
			} // if (fileExists)
		}
		catch (TkUtil::Exception& e) {
			UTF8String	error (Charset::UTF_8);
			error << "TkUtils::Exception file.exists ( ) " << e.getMessage()
				  << "\nTest d'existence du fichier " << fileName
				  << "\nimpossible. Le droit en exécution sur l'un des répertoires du chemin est probablement manquant. "
				  << "\nSouhaitez-vous choisir un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		} // catch (TkUtil::Exception& e)

		if (false == file.isWritable ( ))
		{
			UTF8String	error (Charset::UTF_8);
			error << "Vous n'avez pas les droits en écriture sur le fichier\n" << fileName << ".\nSouhaitez-vous sélectionner un autre fichier ?";
			switch (QMessageBox::warning (this, "Magix 3D", UTF8TOQSTRING (error), "Oui", "Annuler", QString::null, 0, -1))
			{
				case	0	: fileName.clear(); continue;
				case	1	: return;
			}	// switch (QMessageBox::warning (...
		}	// if (false == file.isWritable ( ))
	}	// while (0 == fileName.length ( ))

	lastDir				= dialog.directory ( ).absolutePath ( );
	
	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);

	getContext ( ).savePythonScript (fileName.c_str ( ), (ContextIfc::encodageScripts) dialog.getEncodageScript(), dialog.getCharset ( ));

	_pythonMinScript	= fileName;
	_encodageScripts	= (ContextIfc::encodageScripts)dialog.getEncodageScript ( );
	_pytMinScriptCharset	= dialog.getCharset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'exportation des données dans le " << " fichier " << " :" << "\n" << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
	else {
		// titre de la fenêtre principale
		UTF8String	titre (Charset::UTF_8);
		titre << Resources::instance ( )._softwareName<<" ("<<MAGIX3D_VERSION/*GSCC_PROJECT_VERSION*/<< ") " << file.getFileName();
		setWindowTitle(UTF8TOQSTRING (titre));
	}
}	// QtMgx3DMainWindow::saveAsMagix3DScriptCallback


void QtMgx3DMainWindow::savePythonConsoleCallback ( )
{
	savePythonConsole (false);
}	// QtMgx3DMainWindow::savePythonConsoleCallback


void QtMgx3DMainWindow::longCommandCallback ( )
{
	QtAutoWaitingCursor	cursor (true);

	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS
	getM3DCommandManager ( ).newLongCommand ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'exécution d'une longue commande : "  << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
}	// QtMgx3DMainWindow::longCommandCallback


void QtMgx3DMainWindow::openCascadeLongCommandCallback ( )
{
	QtAutoWaitingCursor	cursor (true);

	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS
	getM3DCommandManager ( ).newOpenCascadeLongCommand ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'exécution d'une longue commande : "  << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
}	// QtMgx3DMainWindow::openCascadeLongCommandCallback


void QtMgx3DMainWindow::executePythonScriptCallback ( )
{
	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	static string	lastDir (Process::getCurrentDirectory ( ));
/*
	static QString	lastDir(Process::getCurrentDirectory ( ).c_str ( ));
	QFileDialog		fileDialog (this, "Sélection d'un script Python");
	fileDialog.setFilter ("*.py");
	fileDialog.setFileMode (QFileDialog::ExistingFile);
	fileDialog.setAcceptMode (QFileDialog::AcceptOpen);
	fileDialog.setDirectory (lastDir);
	fileDialog.setOption (QFileDialog::DontUseNativeDialog);

	if (QDialog::Accepted != fileDialog.exec ( ))
		return;
*/
	File	file = selectFileName (this, lastDir, "*.py", "Sélection d'un script Python", QFileDialog::ExistingFile, QFileDialog::AcceptOpen);
	if (true == file.getFullFileName ( ).empty ( ))
		return;	// cancelled

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);
//	lastDir	= fileDialog.directory ( ).absolutePath ( );
	lastDir	= file.getPath ( ).getFullFileName ( );

//	QStringList		fileList	= fileDialog.selectedFiles ( );
//	File			file (fileList [0].toStdString ( ));
	message << "Exécution du script Python " << file.getFullFileName ( ) << ".\n";

	executePythonScript (file.getFullFileName ( ));

	message << "Exécution avec succès.";
	log (InformationLog (message));

	_recentScriptsURLFifo.add (file.getFullFileName ( ), true);
	// On l'ajoute (en-tête) dans le menu "scripts récents". Attention : la
	// liste de noms de scripts stockés peut être plus longue que la liste de
	// scripts affichés.
	const size_t	max	=
		_recentScriptsURLFifo.count ( ) > getActions ( )._recentFilesCapacity ?
		getActions ( )._recentFilesCapacity : _recentScriptsURLFifo.count ( );
	CHECK_NULL_PTR_ERROR (getActions ( )._mgx3DScriptsMenu)
	CHECK_NULL_PTR_ERROR (getActions ( )._mgx3DScriptsActions)
	CHECK_NULL_PTR_ERROR (getActions ( )._mgx3DScriptsActions [max - 1])
	if (0 == getActions ( )._mgx3DScriptsActions [max - 1]->menu ( ))
		getActions ( )._mgx3DScriptsMenu->addAction (getActions ( )._mgx3DScriptsActions [max - 1]);
	for (size_t i = getActions ( )._recentFilesCapacity - 1; i != 0; i--)
	{
		CHECK_NULL_PTR_ERROR (getActions ( )._mgx3DScriptsActions [i])
		CHECK_NULL_PTR_ERROR (getActions ( )._mgx3DScriptsActions [i - 1])
		if (0 != getActions ( )._mgx3DScriptsActions [i - 1]->text( ).length( ))
			getActions ( )._mgx3DScriptsActions [i]->setText (getActions ( )._mgx3DScriptsActions [i - 1]->text ( ));
	}	// for (size_t i = getActions ( )._recentFilesCapacity - 1; ...
//	getActions ( )._mgx3DScriptsActions [0]->setText (fileList [0]);
	getActions ( )._mgx3DScriptsActions [0]->setText (file.getFullFileName ( ).c_str ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		message << "Echec lors de l'exécution d'un script Python : " 
		        << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)
	else
	{
		try
		{
			UTF8String	recentFileName (Charset::UTF_8);
			recentFileName << UserData ( ).getHome ( ) << "/" << _recentScriptsFileName;
			_recentScriptsURLFifo.save (recentFileName);
		}
		catch (...)
		{ }
	}	// else if (true == hasError)

	_actions.setEnabled (true);
	if (0 != _pythonPanel)
		_pythonPanel->setUsabled (true);
}	// QtMgx3DMainWindow::executePythonScriptCallback


void QtMgx3DMainWindow::executeRecentMgx3DScriptCallback ( )
{
	UTF8String	message (Charset::UTF_8);

	BEGIN_QT_TRY_CATCH_BLOCK

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor	cursor (true);

	// Qt 4 ne permet pas de passer des arguments aux fonctions callback. On
	// peut néanmoins récupérer un pointeur sur l'emetteur du signal, donc sur
	// la QAction à l'origine du callback, via la méthode QObject::sender :
	QObject*	s	= sender ( );
	QAction*	a	= dynamic_cast<QAction*>(s);
	const string	fileName	= a->text ( ).toStdString ( );

	message << "Exécution du script Python " << fileName << ".\n";

	executePythonScript (fileName);

	message << "Exécution avec succès.";
	log (InformationLog (message));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Echec lors de l'exécution du script python : " << errorString;
		log (ErrorLog (message));
	}	// if (true == hasError)

	_actions.setEnabled (true);
	if (0 != _pythonPanel)
		_pythonPanel->setUsabled (true);
}	// QtMgx3DMainWindow::executeRecentMgx3DScriptCallback



void QtMgx3DMainWindow::print3DViewCallback ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

	throw Exception (UTF8String ("QtMgx3DMainWindow::print3DViewCallback : méthode à surcharger.", Charset::UTF_8));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::print3DViewCallback


void QtMgx3DMainWindow::print3DViewToFileCallback ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

	throw Exception (UTF8String ("QtMgx3DMainWindow::print3DViewToFileCallback : méthode à surcharger.", Charset::UTF_8));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::print3DViewToFileCallback


void QtMgx3DMainWindow::useGlobalDisplayPropertiesCallback (bool global)
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager (
										).useGlobalDisplayProperties (global);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::useGlobalDisplayPropertiesCallback


void QtMgx3DMainWindow::displayTrihedronCallback (bool display)
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ( ).setDisplayTrihedron(display);
	Resources::instance ( )._displayTrihedron.setValue (display);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displayTrihedronCallback


void QtMgx3DMainWindow::displayLandmarkCallback (bool display)
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ( ).setDisplayLandmark(display);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displayLandmarkCallback


void QtMgx3DMainWindow::parametrizeLandmarkCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	UTF8String			dlgTitle (Charset::UTF_8);
	dlgTitle << getAppTitle ( ) << " : Paramétrage du repère.";

	LandmarkAttributes	parameters	= getGraphicalWidget ( ).getRenderingManager ( ).getAxisProperties ( );
	QtLandmarkDialog	dialog (this, dlgTitle, getAppTitle ( ), true, "X", "Y", "Z", parameters);
//							QtMgx3DApplication::HelpSystem::instance ( ).landmarkDlgURL,
//							QtMgx3DApplication::HelpSystem::instance ( ).landmarkDlgTag);

	if (0 != dialog.exec ( ))
	{
		QtAutoWaitingCursor	waitingCursor (true);
		getGraphicalWidget ( ).getRenderingManager ( ).setAxisProperties (dialog.getParameters ( ));
	}	// if (0 != dialog.exec ( ))

	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::parametrizeLandmarkCallback


void QtMgx3DMainWindow::displayFocalPointCrossCallback (bool display)
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ().setDisplayFocalPoint (display);
	Resources::instance ( )._displayFocalPoint.setValue (display);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displayFocalPointCrossCallback


void QtMgx3DMainWindow::xOyViewPlaneCallback ( )
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ( ).displayxOyViewPlane ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::xOyViewPlaneCallback


void QtMgx3DMainWindow::xOzViewPlaneCallback ( )
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ( ).displayxOzViewPlane ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::xOzViewPlaneCallback


void QtMgx3DMainWindow::yOzViewPlaneCallback ( )
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ( ).displayyOzViewPlane ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::yOzViewPlaneCallback


void QtMgx3DMainWindow::resetViewCallback ( )
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager ( ).resetView (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::resetViewCallback


void QtMgx3DMainWindow::clearViewCallback ( )
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	const vector<Entity*> entities = getContext().getAllVisibleEntities();
    getGraphicalWidget ( ).getRenderingManager ( ).removeEntities (entities, DisplayRepresentation::DISPLAY_GEOM);
    //getGraphicalWidget ( ).getRenderingManager ( ).displayRepresentations(entities, false, 0, DisplayRepresentation::DISPLAY_GEOM);

    getGraphicalWidget ( ).getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::clearViewCallback


void QtMgx3DMainWindow::zoomViewCallback ( )
{
	bool			ok	= false;
	UTF8String	message (Charset::UTF_8);
	message << "Saisissez le facteur de grossissement (> 1) ou de réduction (< 1).";
	double 			factor	= QInputDialog::getDouble (NULL, "Magix3D", message.utf8 ( ).c_str ( ),1., 0., 100., 2, &ok);

	BEGIN_QT_TRY_CATCH_BLOCK
	if ((false == ok) || (1. == factor))
		return;

	QtAutoWaitingCursor	waitingCursor (true);
	getGraphicalWidget ( ).getRenderingManager ( ).zoomView (factor);
	getGraphicalWidget ( ).getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::zoomViewCallback



void QtMgx3DMainWindow::parallelProjectionCallback (bool parallel)
{
	QtAutoWaitingCursor	cursor (true);
	UTF8String			message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGraphicalWidget ( ).getRenderingManager (
										).setParallelProjection (parallel);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::parallelProjectionCallback


void QtMgx3DMainWindow::displayLookupTableCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 == _actions._displayLookupTableAction)
		return;

	vector<RenderingManager::ColorTableDefinition>	definitions;
	getGraphicalWidget ( ).getRenderingManager ( ).getDisplayedValues (
																definitions);

	if (0 == definitions.size ( ))
	{
	    if (true == _actions._displayLookupTableAction->isChecked ( ))
			throw Exception (UTF8String ("Affichage de la table des couleurs : absence de valeurs aux noeuds/mailles affichées.", Charset::UTF_8));

		// Masquer toutes les tables de couleurs existantes :
		vector<RenderingManager::ColorTable*>	tables	=
				 getGraphicalWidget ( ).getRenderingManager (
														).getColorTables ( );
		for (vector<RenderingManager::ColorTable*>::iterator
				it = tables.begin ( ); tables.end ( ) != it; it++)
			getGraphicalWidget ( ).getRenderingManager ( ).removeColorTable (*it);
		return;
	}	// if (0 == definitions.size ( ))

    RenderingManager::ColorTable*	table	=
			getGraphicalWidget ( ).getRenderingManager ( ).getColorTable (
															definitions [0]);
											
	table->display (_actions._displayLookupTableAction->isChecked ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	if (true == hasError)
	{
		if (0 != _actions._displayLookupTableAction)
		{
			QtObjectSignalBlocker	blocker(_actions._displayLookupTableAction);
			_actions._displayLookupTableAction->setChecked (false);
		}
	}	// if (true == hasError)
}	// QtMgx3DMainWindow::displayLookupTableCallback


void QtMgx3DMainWindow::displayLookupTableEditorCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	vector<RenderingManager::ColorTable*>	tables	=
			 getGraphicalWidget ( ).getRenderingManager ( ).getColorTables ( );
	if (0 == tables.size ( ))
		throw Exception (UTF8String ("Absence de table de couleurs.", Charset::UTF_8));

	QtColorTablesEditorDialog	dialog (this, getAppTitle ( ), "Editeur de table de couleurs", getGraphicalWidget ( ).getRenderingManager ( ), tables);
	dialog.exec ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

}	// QtMgx3DMainWindow::displayLookupTableEditorCallback


void QtMgx3DMainWindow::addDistanceMeasurementCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	QtDistanceMeasurementOperationAction*	measurementAction	=
		new QtDistanceMeasurementOperationAction (
				QIcon (":/images/distance.png"), "Distance entre entités",
				*this, 
				"Mesure de la distance entre entités C.A.O.");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addDistanceMeasurementCallback


void QtMgx3DMainWindow::addAngleMeasurementCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	QtAngleMeasurementOperationAction*	measurementAction	=
		new QtAngleMeasurementOperationAction (QIcon (""),
				"Angle entre 3 points",
				*this,
				"Mesure de l'angle entre 3 points.");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addAngleMeasurementCallback


void QtMgx3DMainWindow::addExtremaMeshingEdgeLengthOnEdgeCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	QtExtremaMeshingEdgeLengthOnEdgeAction*	infoCoEdgeAction	=
		new QtExtremaMeshingEdgeLengthOnEdgeAction (
				QIcon (""),
				"Longeur des bras aux extrémités",
				*this,
				"Donne la longeur des bras aux extrémités d'une arête");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addExtremaMeshingEdgeLengthOnEdgeCallback


void QtMgx3DMainWindow::refineTopologyCallback ( )
{
    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	bool	ok		= false;
	int		factor	= QInputDialog::getInt (
						this, "Raffinement de la topologie.", "Facteur :",
						2, 1, 1000000, 1, &ok);

	if (false == ok)
		return;

    message << "Raffinement des arêtes topologiques d'un facteur "
	        << (long)factor << ".";

	getTopoManager ( ).refine (factor);

    message << " Commande créée avec succès.";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

    CHECK_COMMAND_CREATION_STATUS
}	// QtMgx3DMainWindow::refineTopologyCallback


void QtMgx3DMainWindow::defaultNbMeshingEdgesCallback ( )
{
    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

	bool	ok		= false;
	int		oldDefault	= getTopoManager ( ).getDefaultNbMeshingEdges ( );
	int		newDefault	= QInputDialog::getInt (
						this, "Nombre d'arêtes de maillage par défaut.", "Nombre :",
						oldDefault, 1, 2147483647, 1, &ok);

	if ((false == ok) || (oldDefault == newDefault))
		return;

    message << "Définition du nombre d'arêtes de maillage par défaut à "
	        << (long)newDefault << ".";

	getTopoManager ( ).setDefaultNbMeshingEdges (newDefault);

    message << " Commande créée avec succès.";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

    CHECK_COMMAND_CREATION_STATUS
}	// QtMgx3DMainWindow::defaultNbMeshingEdgesCallback


void QtMgx3DMainWindow::new3x3BoxesWithTopoCallback ()
{
    QtAutoWaitingCursor cursor (true);

    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

    DISABLE_COMMAND_ACTIONS
    DISABLE_GRAPHICAL_OPERATIONS

    message << "Création d'une grille de 3 X 3 boites avec topologies";

    // tout est structuré par défaut pour le moment
//    getTopoManager ( ).newIJBoxesWithTopo (9, 9, false);
    getTopoManager ( ).newIJBoxesWithTopo (3, 3, true); // on alterne structuré / non-structuré

    message << " Commande créée avec succès.";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

    CHECK_COMMAND_CREATION_STATUS
}   // QtMgx3DMainWindow::new3x3BoxesWithTopoCallback ()


void QtMgx3DMainWindow::optimMeshMethodCallback ( )
{
	   UTF8String	message (Charset::UTF_8);
	BEGIN_QT_TRY_CATCH_BLOCK

    message << "Optimisation de la méthode de maillage pour tous les blocs";

	getTopoManager().replaceTransfiniteByDirectionalMeshMethodAsPossible();

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	CHECK_COMMAND_CREATION_STATUS

} // QtMgx3DMainWindow::optimMeshMethodCallback ( )

void QtMgx3DMainWindow::addTopoInformationCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	QtTopoInformationOperationAction*	topoInfoAction	=
		new QtTopoInformationOperationAction (QIcon (), "Information sur la topologie",
				*this,
				"Informations sur la topologie.");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addTopoInformationCallback


//void QtMgx3DMainWindow::newMeshOnSelectionCallback ( )
void QtMgx3DMainWindow::meshSelectionCallback ( )
{	// Fonction créée pour avoir une icône dans la barre de menu.
    QtAutoWaitingCursor cursor (true);

    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

    DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	// on récupère les noms des entités sélectionnées
	vector<string>	blocs = getContext ( ).getSelectionManager ( ).getEntitiesNames (Entity::TopoBlock);
    vector<string>	faces = getContext ( ).getSelectionManager ( ).getEntitiesNames (Entity::TopoCoFace);
	const size_t	count	= blocs.size ( ) + faces.size ( );

	// Si absence de sélection adéquate on maille tout :
	if (0 == count)
		throw Exception (UTF8String ("Mailler la sélection : absence de sélection.", Charset::UTF_8));

	if (0 != faces.size ( ))
	{
		if (0 != blocs.size ( ))
			message << "\n";
		if (faces.size() == 1)
			message << "Création du maillage pour le face :";
		else
			message << "Création du maillage pour les faces :";
		for (vector<string>::iterator itf = faces.begin ( ); itf != faces.end ( ); itf++)
				message << " " << (*itf);
		getMeshManager ( ).newFacesMesh (faces);
	}	// if (0 != faces.size ( ))
	if (0 != blocs.size ( ))
	{
		if (blocs.size() == 1)
			message << "Création du maillage pour le bloc :";
		else
			message << "Création du maillage pour les blocs :";
		for (vector<string>::iterator itb = blocs.begin ( );
		     itb != blocs.end ( ); itb++)
				message << " " << (*itb);
		getMeshManager ( ).newBlocksMesh (blocs);
	}	// if (0 != blocs.size ( ))

    message << " Commande créée avec succès.";
	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	CHECK_COMMAND_CREATION_STATUS
}	// QtMgx3DMainWindow::newMeshOnSelectionCallback


void QtMgx3DMainWindow::meshVisibleCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

    DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	// On récupère la liste des groupes visibles :
	const vector<string> visibleGroups	= getGroupManager ( ).getVisibles ( );
	if (0 == visibleGroups.size ( ))
		throw Exception (UTF8String ("Mailler les groupes visibles : absence de groupes visibles.", Charset::UTF_8));

	if (visibleGroups.size ( ) == 1)
		message << "Création du maillage pour le groupe "
		        << visibleGroups [0];
	else
	{
		message << "Création du maillage pour les groupes :";
		for (vector<string>::const_iterator itg = visibleGroups.begin ( );
		     itg != visibleGroups.end ( ); itg++)
				message << " " << (*itg);
	}	// else if (visibleGroups.size ( ) == 1)
	//getGroupManager ( ).mesh (visibleGroups);
	std::vector<std::string> topo;
	topo = getGroupManager().getTopoBlocks(visibleGroups);
	if (!topo.empty())
		getMeshManager().newBlocksMesh(topo);
	else {
		topo = getGroupManager().getTopoFaces(visibleGroups);
		if (!topo.empty())
			getMeshManager().newFacesMesh(topo);
		else {
			throw Exception (UTF8String ("Mailler les groupes visibles : absence de blocs et de faces à mailler.", Charset::UTF_8));
		}

	}

    message << ". Commande créée avec succès.";
	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));


	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	CHECK_COMMAND_CREATION_STATUS
}	// QtMgx3DMainWindow::meshVisibleCallback


void QtMgx3DMainWindow::meshAllCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

    DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	// On maille tous les blocs s'il y en a :
	if (0 != getContext ( ).getTopoManager ( ).getNbBlocks ( ))
	{
		message << "Création du maillage pour tous les blocs du maillage. ";
		getContext ( ).getMeshManager ( ).newAllBlocksMesh ( );
	}	// if (0 != getContext ( ).getTopoManager ( ).getNbBlocks ( ))
	else if (0 != getContext ( ).getTopoManager ( ).getNbFaces ( ))
	{	// => On maille les faces
		message << "Création du maillage pour toutes les faces du maillage. ";
		getContext ( ).getMeshManager ( ).newAllFacesMesh ( );
	}	// else if (0 != getContext ( ).getTopoManager ( ).getNbFaces ( ))
	else
	{
		throw Exception (UTF8String ("Il n'y a pas de topologie à mailler (ni face, ni bloc). ", Charset::UTF_8));
	}

    message << "Commande créée avec succès.";
	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	CHECK_COMMAND_CREATION_STATUS
}	// QtMgx3DMainWindow::meshAllCallback


void QtMgx3DMainWindow::unrefineMeshRepresentationCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	bool		ok		= false;
	const int	current	= getContext ( ).getRatioDegrad ( );

	int factor	= QInputDialog::getInt ( this, "Déraffinement de la représentation du maillage.", "Facteur :", current, 1, 1000000, 1, &ok);

	if ((false == ok) || (current == factor))
		return;

	DISABLE_COMMAND_ACTIONS
	DISABLE_GRAPHICAL_OPERATIONS

	QtAutoWaitingCursor cursor (true);

	getContext ( ).setRatioDegrad (factor);

	// Réafficher les entités maillages avec le nouveau facteur de déraffinement
	const vector<Entity*>	entities	= getGraphicalWidget ( ).getRenderingManager ( ).getDisplayedEntities ( );
	for (vector<Entity*>::const_iterator ite = entities.begin ( ); ite != entities.end ( ); ite++)
	{
		if (false == (*ite)->isMeshEntity ( ))
			continue;

		DisplayProperties&	displayProps							= (*ite)->getDisplayProperties ( );
		DisplayProperties::GraphicalRepresentation*	graphicalRep	= displayProps.getGraphicalRepresentation ( );
		CHECK_NULL_PTR_ERROR (graphicalRep)
		const unsigned long	mask	= graphicalRep->getRepresentationMask ( );
		graphicalRep->updateRepresentation (mask, true);
		getGraphicalWidget ( ).getRenderingManager ( ).forceRender ( );
	}	// for (vector<Entity*>::const_iterator ite = entities.begin ( ); ...

	UTF8String	message (Charset::UTF_8);
	message << "Déraffinement de la représentation du maillage d'un facteur "
	        << (unsigned long)factor << " effectué.";
	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unrefineMeshRepresentationCallback


void QtMgx3DMainWindow::addMeshExplorerCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    UTF8String	message (Charset::UTF_8);

    BEGIN_QT_TRY_CATCH_BLOCK

    // récupération de l'éventuelle arête sélectionnée :
    vector<string>	selectedEdges	= getContext ( ).getSelectionManager (
												).getEntitiesNames (Entity::TopoCoEdge);
	const string	selectedEdge (
					0 == selectedEdges.size ( ) ? string ( ) : selectedEdges [0]);

	QtMeshExplorerOperationAction*	sheetAction	=
		new QtMeshExplorerOperationAction (
				QIcon (":/images/create_sheet.png"), "Exploration de feuillets",
				*this, "Exploration de feuillets.",
				QtMgx3DGroupNamePanel::CREATION);
	vector<ObjectBase*>	observers;
	observers.push_back (this);
	sheetAction->addObservers (observers);

    message << "Exploration avec l'arête "
	        << (0 == selectedEdge.length ( ) ? "NULL" : selectedEdge.c_str ( )) << ".";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addMeshExplorerCallback


void QtMgx3DMainWindow::addMeshQualityCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	QtMeshQualityOperationAction*	qualityAction	=
		new QtMeshQualityOperationAction (
				QIcon (":/images/mesh_quality.png"), "Qualité de maillage",
				*this, 
				"Evaluation de la qualité du maillage.");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addMeshQualityCallback


void QtMgx3DMainWindow::addMeshInformationCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	QtMeshInformationOperationAction*	meshInfoAction	=
		new QtMeshInformationOperationAction (QIcon (), "Information sur le maillage",
				*this,
				"Informations sur le maillage prévisionnel et réalisé.");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::addMeshInformationCallback


void QtMgx3DMainWindow::displaySelectedGroupsCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGroupsPanel ( ).displaySelectedGroups (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displaySelectedGroupsCallback


void QtMgx3DMainWindow::hideSelectedGroupsCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGroupsPanel ( ).displaySelectedGroups (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::hideSelectedTypesCallback


void QtMgx3DMainWindow::displaySelectedTypesCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGroupsPanel ( ).displaySelectedTypes (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displaySelectedTypesCallback


void QtMgx3DMainWindow::hideSelectedTypesCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	getGroupsPanel ( ).displaySelectedTypes (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::hideSelectedTypesCallback


void QtMgx3DMainWindow::displaySelectedEntitiesCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

    getGraphicalWidget ( ).getRenderingManager ( ).displayRepresentationsSelectedEntities (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displaySelectedEntitiesCallback


void QtMgx3DMainWindow::hideSelectedEntitiesCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

    getGraphicalWidget ( ).getRenderingManager ( ).displayRepresentationsSelectedEntities (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::hideSelectedEntitiesCallback


void QtMgx3DMainWindow::displaySelectedEntitiesPropertiesCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getActions ( )._displaySelectionPropertiesAction)
	CHECK_NULL_PTR_ERROR (_selectionCommonPropertiesPanel)
	CHECK_NULL_PTR_ERROR (_selectionIndividualPropertiesPanel)

	_selectionCommonPropertiesPanel->displayProperties (
				getActions()._displaySelectionPropertiesAction->isChecked( ));
	_selectionIndividualPropertiesPanel->displayProperties (
				getActions()._displaySelectionPropertiesAction->isChecked( ));
//    getGraphicalWidget ( ).getRenderingManager ( ).displayRepresentationsSelectedEntities (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displaySelectedEntitiesPropertiesCallback


void QtMgx3DMainWindow::displaySelectedEntitiesComputablePropertiesCallback ( )
{
    QtAutoWaitingCursor cursor (true);

    BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getActions ( )._displayComputableSelectionPropertiesAction)
	CHECK_NULL_PTR_ERROR (_selectionCommonPropertiesPanel)
	CHECK_NULL_PTR_ERROR (_selectionIndividualPropertiesPanel)
	_selectionCommonPropertiesPanel->displayComputedProperties (
		getActions()._displayComputableSelectionPropertiesAction->isChecked( ));
	_selectionIndividualPropertiesPanel->displayComputedProperties (
		getActions()._displayComputableSelectionPropertiesAction->isChecked( ));
//    getGraphicalWidget ( ).getRenderingManager ( ).displayRepresentationsSelectedEntities (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::displaySelectedEntitiesComputablePropertiesCallback


void QtMgx3DMainWindow::showRepresentationTypesCallback ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

	disableActions (true);

	const vector<Entity*>	entities	=
					getContext ( ).getSelectionManager ( ).getEntities ( );
	vector<Entity*>			displayedEntities;
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
		if (true == (*it)->getDisplayProperties ( ).isDisplayed ( ))
			displayedEntities.push_back (*it);
	changeRepresentationTypes (displayedEntities);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::showRepresentationTypesCallback


void QtMgx3DMainWindow::representationTypesDialogClosedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	_repTypesDialog	= 0;
	disableActions (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::representationTypesDialogClosedCallback


void QtMgx3DMainWindow::selectEntitiesCallback ( )
{
	_actions.setEnabled (false);
	if (0 != _pythonPanel)
		_pythonPanel->setUsabled (false);

	BEGIN_QT_TRY_CATCH_BLOCK

	QtMgx3DSelectionDialog	dialog (*this);
	if (QDialog::Rejected == dialog.exec ( ))
		return;

	if (true == dialog.replaceSelection ( ))
		getContext ( ).getSelectionManager ( ).clearSelection ( );

	vector<string>	entities	= dialog.getUniqueNames ( );
	try
	{
		getContext ( ).addToSelection (entities);
	}
	catch (...)
	{
	}

	try
	{
		if (true == dialog.displaySelectedEntities ( ))
			getEntitiesPanel ( ).showSelectedEntities (true);
	}
	catch (...)
	{
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))

	_actions.setEnabled (true);
	if (0 != _pythonPanel)
		_pythonPanel->setUsabled (true);
}	// QtMgx3DMainWindow::selectEntitiesCallback


void QtMgx3DMainWindow::selectVisibleEntitiesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_graphicalWidget)

	vector<Entity*>	entities	=
			_graphicalWidget->getRenderingManager ( ).getDisplayedEntities ( );
	if (0 != entities.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectVisibleEntitiesCallback


void QtMgx3DMainWindow::unselectVisibleEntitiesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_graphicalWidget)

	vector<Entity*>	entities	=
			_graphicalWidget->getRenderingManager ( ).getDisplayedEntities ( );
	if (0 != entities.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectVisibleEntitiesCallback


void QtMgx3DMainWindow::selectFusableEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector < vector<string> >	names	=
					getContext ( ).getTopoManager ( ).getFusableEdges ( );
	vector<Entity*>			entities;
	for (vector < vector<string> >::const_iterator itg = names.begin ( );
	     names.end ( ) != itg; itg++)
	{
		for (vector<string>::const_iterator it = (*itg).begin ( ); 
		     (*itg).end ( ) != it; it++)
		{
			Topo::CoEdge*	coedge	=
				getContext ( ).getTopoManager ( ).getCoEdge (*it, false);
			if (0 != coedge)
				entities.push_back (coedge);
		}	// for (vector<string>::const_iterator it = names.begin ( );
	}	// for (vector < vector<string> >::const_iterator ...

	if (0 != entities.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectFusableEdgesCallback


void QtMgx3DMainWindow::unselectFusableEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector < vector<string> >	names	=
					getContext ( ).getTopoManager ( ).getFusableEdges ( );
	vector<Entity*>			entities;
	for (vector < vector<string> >::const_iterator itg = names.begin ( );
	     names.end ( ) != itg; itg++)
	{
		for (vector<string>::const_iterator it = (*itg).begin ( ); 
		     (*itg).end ( ) != it; it++)
		{
			Topo::CoEdge*	coedge	=
				getContext ( ).getTopoManager ( ).getCoEdge (*it, false);
			if (0 != coedge)
				entities.push_back (coedge);
		}	// for (vector<string>::const_iterator it = names.begin ( );
	}	// for (vector < vector<string> >::const_iterator ...

	if (0 != entities.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectFusableEdgesCallback


void QtMgx3DMainWindow::selectInvalidEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getInvalidEdges();
	vector<Entity*>			edges;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoEdge*	edge	=
				getContext ( ).getTopoManager ( ).getCoEdge (*it, false);
		if (0 != edge)
			edges.push_back (edge);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != edges.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (edges);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
} // QtMgx3DMainWindow::selectInvalidEdgesCallback


void QtMgx3DMainWindow::unselectInvalidEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getInvalidEdges();
	vector<Entity*>			edges;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoEdge*	edge	=
				getContext ( ).getTopoManager ( ).getCoEdge (*it, false);
		if (0 != edge)
			edges.push_back (edge);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != edges.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (edges);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
} // QtMgx3DMainWindow::unselectInvalidEdgesCallback


void QtMgx3DMainWindow::selectBorderFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getBorderFaces ( );
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( ); 
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectBorderFacesCallback


void QtMgx3DMainWindow::unselectBorderFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getBorderFaces ( );
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( ); 
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectBorderFacesCallback


void QtMgx3DMainWindow::selectFacesWithoutBlockCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getFacesWithoutBlock ( );
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectFacesWithoutBlockCallback


void QtMgx3DMainWindow::unselectFacesWithoutBlockCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getFacesWithoutBlock ( );
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectFacesWithoutBlockCallback


void QtMgx3DMainWindow::selectSemiConformFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getSemiConformalFaces ( );
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( ); 
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectSemiConformFacesCallback


void QtMgx3DMainWindow::unselectSemiConformFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getSemiConformalFaces ( );
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( ); 
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectSemiConformFacesCallback


void QtMgx3DMainWindow::selectInvalidFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getInvalidFaces();
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectInvalidFacesCallback


void QtMgx3DMainWindow::unselectInvalidFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getInvalidFaces();
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectInvalidFacesCallback


void QtMgx3DMainWindow::selectUnstructuredFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getUnstructuredFaces();
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectUnstructuredFacesCallback

void QtMgx3DMainWindow::unselectUnstructuredFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getUnstructuredFaces();
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectUnstructuredFacesCallback

void QtMgx3DMainWindow::selectTransfiniteFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getTransfiniteMeshLawFaces();
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectTransfiniteFacesCallback

void QtMgx3DMainWindow::unselectTransfiniteFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getTransfiniteMeshLawFaces();
	vector<Entity*>			faces;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::CoFace*	face	=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			faces.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != faces.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (faces);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectTransfiniteFacesCallback

void QtMgx3DMainWindow::selectInvalidBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getInvalidBlocks();
	vector<Entity*>			blocks;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Block*	block	=
				getContext ( ).getTopoManager ( ).getBlock (*it, false);
		if (0 != block)
			blocks.push_back (block);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != blocks.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (blocks);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectInvalidBlocksCallback


void QtMgx3DMainWindow::unselectInvalidBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getInvalidBlocks();
	vector<Entity*>			blocks;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Block*	block	=
				getContext ( ).getTopoManager ( ).getBlock (*it, false);
		if (0 != block)
			blocks.push_back (block);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != blocks.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (blocks);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectInvalidBlocksCallback

void QtMgx3DMainWindow::selectUnstructuredBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getUnstructuredBlocks();
	vector<Entity*>			blocks;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Block*	block	=
				getContext ( ).getTopoManager ( ).getBlock (*it, false);
		if (0 != block)
			blocks.push_back (block);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != blocks.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (blocks);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectUnstructuredBlocksCallback

void QtMgx3DMainWindow::unselectUnstructuredBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getUnstructuredBlocks();
	vector<Entity*>			blocks;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Block*	block	=
				getContext ( ).getTopoManager ( ).getBlock (*it, false);
		if (0 != block)
			blocks.push_back (block);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != blocks.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (blocks);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectUnstructuredBlocksCallback

void QtMgx3DMainWindow::selectTransfiniteBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getTransfiniteMeshLawBlocks();
	vector<Entity*>			blocks;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Block*	block	=
				getContext ( ).getTopoManager ( ).getBlock (*it, false);
		if (0 != block)
			blocks.push_back (block);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != blocks.size ( ))
		getContext ( ).getSelectionManager ( ).addToSelection (blocks);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectTransfiniteBlocksCallback

void QtMgx3DMainWindow::unselectTransfiniteBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const vector<string>	names	=
					getContext ( ).getTopoManager ( ).getTransfiniteMeshLawBlocks();
	vector<Entity*>			blocks;
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Block*	block	=
				getContext ( ).getTopoManager ( ).getBlock (*it, false);
		if (0 != block)
			blocks.push_back (block);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	if (0 != blocks.size ( ))
		getContext ( ).getSelectionManager ( ).removeFromSelection (blocks);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::unselectTransfiniteBlocksCallback

void QtMgx3DMainWindow::selectNodesCallback (bool enabled)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	getContext ( ).getSelectionManager ( ).activateSelection (
		true==enabled ? SelectionManagerIfc::D0 : SelectionManagerIfc::NO_DIM);
	CHECK_NULL_PTR_ERROR (getActions ( )._selectEdgesAction)
	CHECK_NULL_PTR_ERROR (getActions ( )._selectSurfacesAction)
	CHECK_NULL_PTR_ERROR (getActions ( )._selectVolumesAction)
	QtActionAutoLock	edgesLock (getActions ( )._selectEdgesAction);
	QtActionAutoLock	surfacesLock (getActions ( )._selectSurfacesAction);
	QtActionAutoLock	volumesLock (getActions ( )._selectVolumesAction);
	getActions ( )._selectEdgesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is1DSelectionActivated ( ));
	getActions ( )._selectSurfacesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is2DSelectionActivated ( ));
	getActions ( )._selectVolumesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is3DSelectionActivated ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectNodesCallback


void QtMgx3DMainWindow::selectEdgesCallback (bool enabled)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	getContext ( ).getSelectionManager ( ).activateSelection (
		true==enabled ? SelectionManagerIfc::D1 : SelectionManagerIfc::NO_DIM);
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectNodesAction)
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectSurfacesAction)
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectVolumesAction)
	QtActionAutoLock	nodesLock (getActions ( )._selectNodesAction);
	QtActionAutoLock	surfacesLock (getActions ( )._selectSurfacesAction);
	QtActionAutoLock	volumesLock (getActions ( )._selectVolumesAction);
	getActions ( )._selectNodesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is0DSelectionActivated ( ));
	getActions ( )._selectSurfacesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is2DSelectionActivated ( ));
	getActions ( )._selectVolumesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is3DSelectionActivated ( ));

	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectEdgesCallback


void QtMgx3DMainWindow::selectSurfacesCallback (bool enabled)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	getContext ( ).getSelectionManager ( ).activateSelection (
		true==enabled ? SelectionManagerIfc::D2 : SelectionManagerIfc::NO_DIM);
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectNodesAction)
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectEdgesAction)
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectVolumesAction)
	QtActionAutoLock	nodesLock (getActions ( )._selectNodesAction);
	QtActionAutoLock	edgesLock (getActions ( )._selectEdgesAction);
	QtActionAutoLock	volumesLock (getActions ( )._selectVolumesAction);
	getActions ( )._selectNodesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is0DSelectionActivated ( ));
	getActions ( )._selectEdgesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is1DSelectionActivated ( ));
	getActions ( )._selectVolumesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is3DSelectionActivated ( ));

	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectSurfacesCallback


void QtMgx3DMainWindow::selectVolumesCallback (bool enabled)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	getContext ( ).getSelectionManager ( ).activateSelection (
		true==enabled ? SelectionManagerIfc::D3 : SelectionManagerIfc::NO_DIM);
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectNodesAction)
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectEdgesAction)
	CHECK_NULL_PTR_ERROR ( getActions ( )._selectSurfacesAction)
	QtActionAutoLock	nodesLock (getActions ( )._selectNodesAction);
	QtActionAutoLock	edgesLock (getActions ( )._selectEdgesAction);
	QtActionAutoLock	surfacesLock (getActions ( )._selectSurfacesAction);
	getActions ( )._selectNodesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is0DSelectionActivated ( ));
	getActions ( )._selectEdgesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is1DSelectionActivated ( ));
	getActions ( )._selectSurfacesAction->setChecked (
			getContext ( ).getSelectionManager ( ).is2DSelectionActivated ( ));

	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectVolumesCallback


void QtMgx3DMainWindow::selectionModeCallback (bool boundingBox)
{
}	// QtMgx3DMainWindow::selectionModeCallback


#ifdef USE_EXPERIMENTAL_ROOM
void QtMgx3DMainWindow::loadRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::loadRaysCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::loadRaysCallback


void QtMgx3DMainWindow::importRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::importRaysCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::importRaysCallback


void QtMgx3DMainWindow::saveRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::saveRaysCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::saveRaysCallback


void QtMgx3DMainWindow::saveAsRaysCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::saveAsRaysCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::saveAsRaysCallback


void QtMgx3DMainWindow::setRaysContextCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::setRaysContextCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::setRaysContextCallback


void QtMgx3DMainWindow::setRaysTargetSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::setRaysTargetSurfacesCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::setRaysTargetSurfacesCallback


void QtMgx3DMainWindow::loadDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::loadDiagsCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::loadDiagsCallback


void QtMgx3DMainWindow::importDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::importDiagsCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::importDiagsCallback


void QtMgx3DMainWindow::saveDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::saveDiagsCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::saveDiagsCallback


void QtMgx3DMainWindow::saveAsDiagsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	throw Exception (UTF8String ("QtMgx3DMainWindow::saveAsDiagsCallback : méthode à surcharger.", Charset::UTF_8));
	 COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::saveAsDiagsCallback
#endif	// USE_EXPERIMENTAL_ROOM


void QtMgx3DMainWindow::showCommandMonitorDialogCallback ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

/*
	if (0 == _commandMonitorDialog)
	{
		const string	title ("Magix 3D : Moniteur de commandes");
		// ATTENTION : on marche sur des oeufs ...
		CommandManager*	cmdMgr	= dynamic_cast<CommandManager*>(&getCommandManager( ));
		CHECK_NULL_PTR_ERROR (cmdMgr)
		_commandMonitorDialog	=	new QtCommandMonitorDialog (
						this, title.c_str ( ), cmdMgr, false);
		_commandMonitorDialog->setWindowTitle (title.c_str ( ));	
		_commandMonitorDialog->setAttribute (Qt::WA_DeleteOnClose, false);
//		_commandMonitorDialog->setWindowModality (Qt::NonModal);
		_commandMonitorDialog->show ( );
	}	// if (0 == _commandMonitorDialog)
	else
	{
		_commandMonitorDialog->show ( );
		_commandMonitorDialog->raise ( );
	}
*/
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::showCommandMonitorDialogCallback


void QtMgx3DMainWindow::usersGuideCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	QtMgx3DApplication::HelpSystem::instance ( ).show(QtMgx3DApplication::HelpSystem::instance ( ).userManual);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::usersGuideCallback

void QtMgx3DMainWindow::usersGuideContextCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).indexURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::usersGuideContextCallback


void QtMgx3DMainWindow::wikiCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	QtMgx3DApplication::HelpSystem::instance ( ).show(QtMgx3DApplication::HelpSystem::instance ( ).wikiURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::wikiCallback


void QtMgx3DMainWindow::tutorialCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).tutorialURL,
														QtMgx3DApplication::HelpSystem::instance ( ).tutorialTag);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::tutorialCallback


void QtMgx3DMainWindow::pythonAPIUsersGuideCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).pythonAPIURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::pythonAPIUsersGuideCallback


void QtMgx3DMainWindow::qualifCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	//QtExternalLinkDialog (this, QtMgx3DApplication::HelpSystem::instance ( ).qualifURL, text, getAppTitle ( )).exec ( );
	QtMgx3DApplication::HelpSystem::instance ( ).show(QtMgx3DApplication::HelpSystem::instance ( ).qualifURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::qualifCallback


void QtMgx3DMainWindow::shortKeyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	//QtHelpWindow::displayURL (QtMgx3DApplication::HelpSystem::instance ( ).shortKeyURL,
	//		QtMgx3DApplication::HelpSystem::instance ( ).shortKeyTag);
	QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).shortKeyURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::shortKeyCallback


void QtMgx3DMainWindow::selectCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	//QtHelpWindow::displayURL (QtMgx3DApplication::HelpSystem::instance ( ).selectURL,
	//		QtMgx3DApplication::HelpSystem::instance ( ).selectTag);
	QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).selectURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::selectCallback


void QtMgx3DMainWindow::historiqueCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK
	//QtHelpWindow::displayURL (QtMgx3DApplication::HelpSystem::instance ( ).historiqueURL);
	QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).historiqueURL);
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::historiqueCallback


void QtMgx3DMainWindow::aboutDialogCallback ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

	QtAboutDialog	dialog (this, "Magix 3D", MAGIX3D_VERSION/*GSCC_PROJECT_VERSION*/,
					        QtMgx3DApplication::HelpSystem::instance ( ).wikiURL, "");
	const vector<QtAboutDialog::Component>	components	=
													getAboutDlgComponents ( );
	for (vector<QtAboutDialog::Component>::const_iterator it = 
			components.begin ( ); components.end ( ) != it; it++)
		dialog.addComponent (
					(*it)._name, (*it)._version, (*it)._what, (*it)._url);

	dialog.exec ( );
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::aboutDialogCallback


void QtMgx3DMainWindow::observableModifiedCallback (
									ReferencedObject* o, unsigned long event)
{
	BEGIN_QT_TRY_CATCH_BLOCK
	
	observableModified (o, event);
	
	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, getAppTitle ( ))
}	// QtMgx3DMainWindow::observableModifiedCallback


void QtMgx3DMainWindow::updateRecentScriptsURLFifoCapacity (size_t count)
{
	if ((0 == count) || (_recentScriptsURLFifo.capacity ( ) == count))
		return;

	// Redimensionnement par réinitialisation :
	_recentScriptsURLFifo	= UrlFifo (count, "");

	// On charge l'état actuel :
	UTF8String	recentFileName (Charset::UTF_8);
	recentFileName << UserData ( ).getHome ( ) << "/" << _recentScriptsFileName;
	_recentScriptsURLFifo.load (recentFileName);

	// Actualisation IHM :
	CHECK_NULL_PTR_ERROR (getActions ( )._mgx3DScriptsActions)
	const size_t	oldCount			= getActions ( )._recentFilesCapacity;
	getActions ( )._recentFilesCapacity	= count;
	for (size_t i = 0; i < oldCount; i++)
		delete getActions ( )._mgx3DScriptsActions [i];
	delete [] getActions ( )._mgx3DScriptsActions;
	getActions ( )._mgx3DScriptsActions	= new QAction* [count];
	for (size_t i = 0; i < count; i++)
	{
		getActions ( )._mgx3DScriptsActions [i]	= 
			new QAction ("", getActions ( )._mgx3DScriptsMenu);
		connect (getActions ( )._mgx3DScriptsActions [i],
		         SIGNAL (triggered ( )), this,
		         SLOT (executeRecentMgx3DScriptCallback ( )));
	}	// for (size_t i = 0; i < count; i++)
	for (size_t i = 0; i < _recentScriptsURLFifo.count ( ); i++)
	{
		if (i >= getActions ( )._recentFilesCapacity)
			break;
		getActions ( )._mgx3DScriptsMenu->addAction (
									getActions ( )._mgx3DScriptsActions [i]);
		getActions ( )._mgx3DScriptsActions [i]->setText (
									_recentScriptsURLFifo.url (i).c_str ( ));
	}	// for (size_t i = 0; i < _recentScriptsURLFifo.count ( ); i++)
}	// QtMgx3DMainWindow::updateRecentScriptsURLFifoCapacity


string QtMgx3DMainWindow::checkMagix3DScriptFileName (
												const string& filename) const
{
	const File		file (filename);
	const string	extension	= file.getExtension ( );
	if (true == extension.empty ( ))
	{
		UTF8String	path (Charset::UTF_8);
		path << filename << ".py";
		return path.iso ( );
	}

	if (0 != strcasecmp ("py", extension.c_str ( )))
	{
		UTF8String	error (Charset::UTF_8);
		error << "Le format \"" << extension
		      << "\" n'est pas un format de script Magix 3D.";
		throw Exception (error);
	}	// if (0 != strcasecmp ("py", extension.c_str ( )))

	return filename;
}	// QtMgx3DMainWindow::checkMagix3DScriptFileName


void QtMgx3DMainWindow::infoNewVersion(const TkUtil::Version& lastVersion,
			const TkUtil::Version& actualVersion)
{
	UTF8String	warning (Charset::UTF_8);
	warning << "Vous accédez pour la premiere fois à une nouvelle version de Magix3D (la "
			<<actualVersion.getVersion()<<") "
	        << "\nLa dernière utilisée était la "<<lastVersion.getVersion()
			<<"\n\nSouhaitez-vous voir les nouveautés ?";

	switch (QMessageBox::warning (this, "Magix 3D",
			UTF8TOQSTRING (warning), "Oui", "Non", 0, 1))
	{
	case	0	:
		//il faut ouvrir l'historique ...
		//QtHelpWindow::displayURL (QtMgx3DApplication::HelpSystem::instance ( ).historiqueURL);
		QtMgx3DApplication::HelpSystem::instance ( ).showUrl(QtMgx3DApplication::HelpSystem::instance ( ).historiqueURL);
		break;
	case	1	:
		// on ne fait rien
		break;
	}	// switch (QMessageBox::warning (...

} // QtMgx3DMainWindow::infoNewVersion

// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================

static bool compareExtensions (const string& ext1, const string& ext2)
{
	return 0 == strcasecmp (ext1.c_str ( ), ext2.c_str ( )) ? true : false;
}	// compareExtensions


static UTF8String addCharAtBeginLines(char pre, UTF8String& us)
{

	//std::cout<<"addCharAtBeginLines pour \""<<us<<"\""<<std::endl;

	UTF8String new_us (Charset::UTF_8);
	if (us.empty())
		return new_us;

	size_t pos1 = 0;
	size_t pos2 = 0;

	// position
	size_t length = us.length();
	do {
		pos2 = us.find('\n', pos1);
		//std::cout<<"pos1 = "<<pos1<<", pos2 = "<<pos2<<std::endl;

		if (pos2 != (size_t)-1){
			new_us << pre << us.substring(pos1, pos2);

			pos1 = pos2+1;
		}
		else
			pos1 = length;

	} while (pos1<length);

	return new_us;

} // addCharAtBeginLines



}	// namespace QtComponents

}	// namespace Mgx3D
