/**
 * \file        QtRepresentationTypesDialog.cpp
 * \author      Charles PIGNEROL
 * \date        17/01/2011
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Internal/Context.h"
#include "QtComponents/QtRepresentationTypesDialog.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ThreeStates.h>
#include <QtUtil/QtConfiguration.h>

#include <QBoxLayout>
#include <QLabel>

#include <map>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{

/// Enlève de sharedNames les noms non contenus dans names.
static void updateSet (set<string>& sharedNames, const vector<string>& names);
static void updateMask (unsigned long& newMask, unsigned long dlgMask,
                        unsigned long kind, THREESTATES used);
static void updateMask (
	unsigned long& newMask, QtRepresentationTypesPanel::DISPLAYED_VALUE kind);


// ===========================================================================
//                        LA CLASSE QtRepresentationTypesDialog
// ===========================================================================

QtRepresentationTypesDialog::QtRepresentationTypesDialog (
			QtMgx3DMainWindow* parent,
			const string& name, const string& appTitle,
			const vector<Entity*>& entities,
			const std::string& helpURL, const std::string& helpTag
		)
	: QDialog (parent), _mainWindow (parent),
	  _representationTypesPanel (0), _closurePanel (0), _entities (entities), _globalMasks ( )
{
	CHECK_NULL_PTR_ERROR (parent)
	setWindowTitle (appTitle.c_str ( ));
	bool	globalMode		= getRenderingManager ( ).useGlobalDisplayProperties ( );
	bool	useGeomColor	= getRenderingManager ( ).topoUseGeomColor ( );

	THREESTATES		points	= OFF, lines = OFF, isoLines = OFF, surfaces = OFF,
					volumes = OFF, names = OFF,
					associations = OFF,
					topoUseGeomColor = true == useGeomColor ? ON : OFF,
					meshShape = OFF,
					discRep = OFF, nbMeshRep = OFF, bold = OFF, italic = OFF;
	bool			pointsColor3S	= true, linesColor3S = true,
					surfacesColor3S = true, volumesColor3S	= true;
	bool			pointsOpacity3S	= true, linesOpacity3S = true,
					surfacesOpacity3S = true, volumesOpacity3S	= true;
	Color			pointsColor (0, 0, 0), linesColor (0, 0, 0),
					surfacesColor (0, 0, 0), volumesColor (0, 0, 0);
	double			pointsOpacity = 1., linesOpacity = 1., surfacesOpacity = 1.,
					volumesOpacity	= 1.;
	bool			shrinkFactor3S	= true;
	double			shrinkFactor	= 1.;
	bool			arrowComul3S	= true;
	double			arrowComul	= 1.;
	bool			pointsSelectable = false, linesSelectable = false,
					isoLinesSelectable = false;
	bool			surfacesSelectable = false, volumesSelectable = false,
					namesSelectable = false,
					associationsSelectable = false, meshShapeSelectable = false,
					discRepSelectable	= false, nbMeshRepSelectable	= false;
	bool			nodesValuesDisp		= false, cellsValuesDisp		= false;
	set <string >	nodesValuesNames, cellsValuesNames;
	string			valueName;
	bool			inited	= false;
	int				fontFamily	= 0, fontSize	= 12;
	bool			fontFamily3S	= true, fontSize3S	= true,
					fontColor3S = true, valueName3S = true;
	Color			fontColor (0, 0, 0);
	for (vector<Entity*>::const_iterator it1 = entities.begin ( ); entities.end ( ) != it1; it1++)
	{
		CHECK_NULL_PTR_ERROR ((*it1)->getDisplayProperties ( ).getGraphicalRepresentation ( ))
		RenderedEntityRepresentation*	rep	= dynamic_cast<RenderedEntityRepresentation*>((*it1)->getDisplayProperties ( ).getGraphicalRepresentation( ));
		CHECK_NULL_PTR_ERROR (rep)
		DisplayProperties	properties	= true == globalMode ? rep->getDisplayPropertiesAttributes ( ) : (*it1)->getDisplayProperties ( );
		// On prend ici en compte le mode d'affichage des propriétés : global ou non (MGXDDD-208) :
		unsigned long	mask	= rep->getUsedRepresentationMask ( );
		if (true == globalMode)
		{
			if (_globalMasks.end ( ) == _globalMasks.find ((*it1)->getType ( )))
				_globalMasks.insert (std::pair<Entity::objectType, unsigned long>((*it1)->getType ( ), mask));
		}	// if (true == globalMode)
//			(*it1)->getDisplayProperties ( ).getGraphicalRepresentation (
//												)->getRepresentationMask ( );
		bool			pointsOn	= 0 != (GraphicalEntityRepresentation::CLOUDS & mask) ? true : false;
		bool			curvesOn	= 0 != (GraphicalEntityRepresentation::CURVES & mask) ? true : false;
		bool			isoCurvesOn	= 0 != (GraphicalEntityRepresentation::ISOCURVES & mask) ? true : false;
		bool			surfacesOn	= 0 != (GraphicalEntityRepresentation::SURFACES & mask) ? true : false;
        bool            volumesOn	= 0 != (GraphicalEntityRepresentation::VOLUMES & mask) ? true : false;
        bool            namesOn		= 0 != (GraphicalEntityRepresentation::NAMES & mask) ? true : false;
        bool            associationsOn  = 0 != (GraphicalEntityRepresentation::ASSOCIATIONS & mask) ? true : false;
        bool            meshShapeOn  = 0 != (GraphicalEntityRepresentation::MESH_SHAPE & mask) ? true : false;
        bool            discRepOn	= 0 != (GraphicalEntityRepresentation::DISCRETISATIONTYPE & mask) ? true : false;
        bool            nbMeshRepOn	= 0 != (GraphicalEntityRepresentation::NBMESHINGEDGE & mask) ? true : false;
//		DisplayProperties&	properties	= (*it1)->getDisplayProperties ( );
		if (false == inited)
		{
			points		= true == pointsOn ? ON : OFF;
			lines		= true == curvesOn ? ON : OFF;
			isoLines	= true == isoCurvesOn ? ON : OFF;
			surfaces	= true == surfacesOn ? ON : OFF;
            volumes    	= true == volumesOn ? ON : OFF;
            names    	= true == namesOn ? ON : OFF;
            associations	= true == associationsOn ? ON : OFF;
            meshShape       = true == meshShapeOn ? ON : OFF;
            discRep    	= true == discRepOn ? ON : OFF;
            nbMeshRep       = true == nbMeshRepOn ? ON : OFF;
			inited		= true;
			pointsColor	= properties.getCloudColor ( );
			linesColor	= properties.getWireColor ( );
			surfacesColor	= properties.getSurfacicColor ( );
			volumesColor	= properties.getVolumicColor ( );
			pointsOpacity	= properties.getCloudOpacity ( );
			linesOpacity	= properties.getWireOpacity ( );
			surfacesOpacity	= properties.getSurfacicOpacity ( );
			volumesOpacity	= properties.getVolumicOpacity ( );
			shrinkFactor	= properties.getShrinkFactor ( );
			arrowComul	= properties.getArrowComul ( );
			bool	fontBold	= false, fontItalic	= false;
			properties.getFontProperties (fontFamily, fontSize, fontBold, fontItalic, fontColor);
			bold	= true == fontBold   ? ON : OFF;
			italic	= true == fontItalic ? ON : OFF;
			valueName		= properties.getValueName ( );
		
			const vector<string>	nvNames	= (*it1)->getNodesValuesNames ( );
			for (vector<string>::const_iterator itn = nvNames.begin ( ); nvNames.end ( ) != itn; itn++)
				nodesValuesNames.insert (*itn);
			const vector<string>	cvNames	= (*it1)->getCellsValuesNames ( );
			for (vector<string>::const_iterator itc = cvNames.begin ( ); cvNames.end ( ) != itc; itc++)
				cellsValuesNames.insert (*itc);
		}	// if (false == inited)
		else
		{
			points		= newState (points, pointsOn);
			lines		= newState (lines, curvesOn);
			isoLines	= newState (isoLines, isoCurvesOn);
			surfaces	= newState (surfaces, surfacesOn);
            names     	= newState (names, namesOn);
			associations= newState (associations, associationsOn);
			meshShape   = newState (meshShape, meshShapeOn);
			discRep     = newState (discRep, discRepOn);
			nbMeshRep   = newState (nbMeshRep, nbMeshRepOn);
			if (pointsColor != properties.getCloudColor ( ))
				pointsColor3S	= false;
			if (linesColor != properties.getWireColor ( ))
				linesColor3S	= false;
			if (surfacesColor != properties.getSurfacicColor ( ))
				surfacesColor3S	= false;
			if (volumesColor != properties.getVolumicColor ( ))
				volumesColor3S	= false;
			if (pointsOpacity != properties.getCloudOpacity ( ))
				pointsOpacity3S	= false;
			if (linesOpacity != properties.getWireOpacity ( ))
				linesOpacity3S	= false;
			if (surfacesOpacity	!= properties.getSurfacicOpacity( ))
				surfacesOpacity3S	= false;
			if (volumesOpacity != properties.getVolumicOpacity( ))
				volumesOpacity3S= false;
			if (shrinkFactor != properties.getShrinkFactor ( ))
				shrinkFactor3S	= false;	
			if (arrowComul != properties.getArrowComul ( ))
				arrowComul3S	= false;
			int			tmpFontFamily	= 0, tmpFontSize	= 12;
			bool		tmpBold	= OFF, tmpItalic	= OFF;
			Color		tmpFontColor (0, 0, 0);
			properties.getFontProperties (tmpFontFamily, tmpFontSize, tmpBold, tmpItalic, tmpFontColor);
			if (fontFamily != tmpFontFamily)
			{
				fontFamily3S	= false;
				fontFamily		= -1;
			}
			if (fontSize != tmpFontSize)
			{
				fontSize3S	= false;
				fontSize	= -1;
			}
			if (fontColor != tmpFontColor)
				fontColor3S	= false;
			const string&	tmpValueName	= properties.getValueName ( );
			if (valueName != tmpValueName)
				valueName3S	= false;

			const vector<string>	nvNames	= (*it1)->getNodesValuesNames ( );
			const vector<string>	cvNames	= (*it1)->getCellsValuesNames ( );
			updateSet (nodesValuesNames, nvNames);
			updateSet (cellsValuesNames, cvNames);
		}	// else if (false == inited)

		// ce sont les types des objets qui déterminent ce qu'il est possible d'avoir comme type d'affichage
		Utils::Entity::objectType type = (*it1)->getType();
		unsigned long   maskVis = GraphicalEntityRepresentation::getVisibleTypeByEntityMask(type);
		pointsSelectable	= pointsSelectable   || (GraphicalEntityRepresentation::CLOUDS & maskVis);
		linesSelectable		= linesSelectable   || (GraphicalEntityRepresentation::CURVES & maskVis);
		isoLinesSelectable	= isoLinesSelectable   || (GraphicalEntityRepresentation::ISOCURVES & maskVis);
		surfacesSelectable	= surfacesSelectable   || (GraphicalEntityRepresentation::SURFACES & maskVis);
		volumesSelectable	= volumesSelectable   || (GraphicalEntityRepresentation::VOLUMES & maskVis);
		namesSelectable		= namesSelectable   || (GraphicalEntityRepresentation::NAMES & maskVis);
		meshShapeSelectable	= meshShapeSelectable   || (GraphicalEntityRepresentation::MESH_SHAPE & maskVis);
        associationsSelectable	= associationsSelectable   || (GraphicalEntityRepresentation::ASSOCIATIONS & maskVis);
        discRepSelectable	= discRepSelectable   || (GraphicalEntityRepresentation::DISCRETISATIONTYPE & maskVis);
        nbMeshRepSelectable	= nbMeshRepSelectable || (GraphicalEntityRepresentation::NBMESHINGEDGE & maskVis);
		nodesValuesDisp		= nodesValuesDisp || (GraphicalEntityRepresentation::NODES_VALUES & maskVis);
		cellsValuesDisp		= cellsValuesDisp || (GraphicalEntityRepresentation::CELLS_VALUES & maskVis);
	}	// for (vector<Entity*>::iterator it1 = ...

	const bool		allowValues	= (true == nodesValuesDisp) || (true == cellsValuesDisp) ? true : false;
	QtRepresentationTypesPanel::DISPLAYED_VALUE	valuesType	= QtRepresentationTypesPanel::NO_VALUE;
	if (0 != valueName.length ( ) && (0 != nodesValuesNames.size ( ) + cellsValuesNames.size ( )))
	{
		if (nodesValuesNames.end ( ) != std::find (nodesValuesNames.begin ( ), nodesValuesNames.end ( ), valueName))
			valuesType	= QtRepresentationTypesPanel::NODE_VALUE;
		else if (cellsValuesNames.end ( ) != std::find (cellsValuesNames.begin ( ), cellsValuesNames.end ( ), valueName))
			valuesType	= QtRepresentationTypesPanel::CELL_VALUE;
	}	// if (0 != nodesValuesNames.size ( ) + cellsValuesNames.size ( ))
	if (false == valueName3S)
		valueName.clear ( );

	QBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (5);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);
	_representationTypesPanel	= new QtRepresentationTypesPanel (
		this, name, globalMode, 
		points, lines, isoLines, surfaces, volumes, names,
		associations, topoUseGeomColor, meshShape, discRep, nbMeshRep,
		pointsSelectable, linesSelectable, isoLinesSelectable,
		surfacesSelectable, volumesSelectable, namesSelectable,
		associationsSelectable, meshShapeSelectable,
		discRepSelectable, nbMeshRepSelectable,
		pointsColor, linesColor, linesColor, surfacesColor, volumesColor,
		pointsColor3S, linesColor3S, linesColor3S,
		surfacesColor3S, volumesColor3S,
		pointsOpacity, linesOpacity, linesOpacity,
		surfacesOpacity, volumesOpacity,
		pointsOpacity3S, linesOpacity3S, linesOpacity3S,
		surfacesOpacity3S, volumesOpacity3S,
		shrinkFactor, shrinkFactor3S, arrowComul, arrowComul3S,
		fontFamily, fontSize, bold, italic, fontColor,
		allowValues, valuesType,
		nodesValuesNames, cellsValuesNames, valueName);
	layout->addWidget (_representationTypesPanel, 1000);

	_closurePanel	= new QtDlgClosurePanel (this, false, "Appliquer", "", "Fermer", helpURL, helpTag);
	layout->addWidget (_closurePanel, 1);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(applyCallback ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);
	layout->activate ( );

	adjustSize ( );
}	// QtRepresentationTypesDialog::QtRepresentationTypesDialog


QtRepresentationTypesDialog::QtRepresentationTypesDialog (
	QtMgx3DMainWindow* parent, const std::string& appTitle, bool globalMode,
			TkUtil::THREESTATES repCloud, TkUtil::THREESTATES repCurves,
			TkUtil::THREESTATES repIsoCurves, TkUtil::THREESTATES repSurfaces,
            TkUtil::THREESTATES repVolumes, TkUtil::THREESTATES repNames,
			TkUtil::THREESTATES repAssociations, TkUtil::THREESTATES topoColor,
            TkUtil::THREESTATES repMeshShape, TkUtil::THREESTATES repDiscretisationType,
			TkUtil::THREESTATES repNbMesh,
            bool pointsSelectable, bool linesSelectable,
            bool isoLinesSelectable, bool surfacesSelectable,
            bool volumesSelectable, bool namesSelectable,
			bool associationsSelectable,
            bool meshShapeSelectable, bool discRepSelectable,
            bool nbMeshSelectable,
			const TkUtil::Color& cloudColor, 
			const TkUtil::Color& curvesColor, 
			const TkUtil::Color& isoCurvesColor, 
			const TkUtil::Color& surfacesColor, 
			const TkUtil::Color& volumesColor,
			bool cloudColorOK, bool curvesColorOK, bool isoCurvesColorOK,
			bool surfacesColorOK, bool volumesColorOK,
			double cloudOpacity, double curvesOpacity, double isoCurvesOpacity,
			double surfacesOpacity, double volumesOpacity,
			bool cloudOpacityOK, bool curvesOpacityOK, bool isoCurvesOpacityOK,
			bool surfacesOpacityOK, bool volumesOpacityOK,
			double shrinkFactor, bool shrinkFactorOK,
			double arrowComul, bool arrowComulOK,
			int fontFamily, int fontSize,
			TkUtil::THREESTATES bold, TkUtil::THREESTATES italic,
			const TkUtil::Color& fontColor,
			bool displayValue, QtRepresentationTypesPanel::DISPLAYED_VALUE displayedValue,
			const std::set< std::string >& nodesValuesNames,
			const std::set< std::string >& cellsValuesNames,
			const std::string& displayedValueName,
			const std::string& helpURL, const std::string& helpTag)
	: QDialog (parent), _mainWindow (parent),
	  _representationTypesPanel (0), _closurePanel (0), _entities ( ), _globalMasks ( )
{
	CHECK_NULL_PTR_ERROR (parent)
	setWindowTitle (appTitle.c_str ( ));			
	QBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (5);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);
	_representationTypesPanel	= new QtRepresentationTypesPanel (
		this, appTitle, globalMode, 
		repCloud, repCurves, repIsoCurves, repSurfaces, repVolumes, repNames, repAssociations, topoColor,
        repMeshShape,  repDiscretisationType, repNbMesh,
		pointsSelectable, linesSelectable, isoLinesSelectable, surfacesSelectable, volumesSelectable, namesSelectable,
		associationsSelectable, meshShapeSelectable, discRepSelectable, nbMeshSelectable,
		cloudColor, curvesColor, isoCurvesColor, surfacesColor, volumesColor,
		cloudColorOK, curvesColorOK, isoCurvesColorOK, surfacesColorOK, volumesColorOK,
		cloudOpacity, curvesOpacity, isoCurvesOpacity, surfacesOpacity, volumesOpacity,
		cloudOpacityOK, curvesOpacityOK, isoCurvesOpacityOK, surfacesOpacityOK, volumesOpacityOK,
		shrinkFactor, shrinkFactorOK, arrowComul, arrowComulOK,
		fontFamily, fontSize, bold, italic, fontColor,
		displayValue, displayedValue, nodesValuesNames, cellsValuesNames,displayedValueName);
	layout->addWidget (_representationTypesPanel, 1000);

	_closurePanel	= new QtDlgClosurePanel (this, false, "Appliquer", "", "Fermer", helpURL, helpTag);
	layout->addWidget (_closurePanel, 1);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(applyCallback ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);
	layout->activate ( );

	adjustSize ( );
}	// QtRepresentationTypesDialog::QtRepresentationTypesDialog


QtRepresentationTypesDialog::QtRepresentationTypesDialog (const QtRepresentationTypesDialog&)
	: QDialog (0),
	  _mainWindow (0), _representationTypesPanel (0), _closurePanel (0), _entities ( ), _globalMasks ( )
{
	MGX_FORBIDDEN ("QtRepresentationTypesDialog copy constructor is not allowed.");
}	// QtRepresentationTypesDialog::QtRepresentationTypesDialog (const QtRepresentationTypesDialog&)


QtRepresentationTypesDialog& QtRepresentationTypesDialog::operator = (const QtRepresentationTypesDialog&)
{
	MGX_FORBIDDEN ("QtRepresentationTypesDialog assignment operator is not allowed.");
	return *this;
}	// QtRepresentationTypesDialog::QtRepresentationTypesDialog (const QtRepresentationTypesDialog&)


QtRepresentationTypesDialog::~QtRepresentationTypesDialog ( )
{
}	// QtRepresentationTypesDialog::~QtRepresentationTypesDialog


bool QtRepresentationTypesDialog::colorModified ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::colorModified", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->colorModified ( );
}	// QtRepresentationTypesDialog::colorModified


QtRepresentationTypesPanel::ExtendedDisplayProperties
					QtRepresentationTypesDialog::getDisplayProperties ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getDisplayProperties", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getDisplayProperties ( );
}	// QtRepresentationTypesDialog::getDisplayProperties


THREESTATES QtRepresentationTypesDialog::useCloudRepresentation ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useCloudRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useCloudRepresentation ( );
}	// QtRepresentationTypesDialog::useCloudRepresentation


bool QtRepresentationTypesDialog::cloudRepresentationModified ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::cloudRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->cloudRepresentationModified ( );
}	// QtRepresentationTypesDialog::cloudRepresentationModified


bool QtRepresentationTypesDialog::useCloudColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useCloudColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useCloudColor ( );
}	// QtRepresentationTypesDialog::useCloudColor


Color QtRepresentationTypesDialog::getCloudColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getCloudColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getCloudColor ( );
}	// QtRepresentationTypesDialog::getCloudColor


bool QtRepresentationTypesDialog::useCloudOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useCloudOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useCloudOpacity ( );
}	// QtRepresentationTypesDialog::useCloudOpacity


double QtRepresentationTypesDialog::getCloudOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getCloudOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getCloudOpacity ( );
}	// QtRepresentationTypesDialog::getCloudOpacity


THREESTATES QtRepresentationTypesDialog::useCurvesRepresentation ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useCurvesRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useCurvesRepresentation ( );
}	// QtRepresentationTypesDialog::useCurvesRepresentation


bool QtRepresentationTypesDialog::curvesRepresentationModified ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::curvesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->curvesRepresentationModified ( );
}	// QtRepresentationTypesDialog::curvesRepresentationModified


bool QtRepresentationTypesDialog::useCurvesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useCurvesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useCurvesColor ( );
}	// QtRepresentationTypesDialog::useCurvesColor


Color QtRepresentationTypesDialog::getCurvesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getCurvesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getCurvesColor ( );
}	// QtRepresentationTypesDialog::getCurvesColor


bool QtRepresentationTypesDialog::useCurvesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useCurvesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useCurvesOpacity ( );
}	// QtRepresentationTypesDialog::useCurvesOpacity


double QtRepresentationTypesDialog::getCurvesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getCurvesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getCurvesOpacity ( );
}	// QtRepresentationTypesDialog::getCurvesOpacity


THREESTATES QtRepresentationTypesDialog::useIsoCurvesRepresentation ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useIsoCurvesRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useIsoCurvesRepresentation ( );
}	// QtRepresentationTypesDialog::useIsoCurvesRepresentation


bool QtRepresentationTypesDialog::isoCurvesRepresentationModified ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::isoCurvesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->isoCurvesRepresentationModified ( );
}	// QtRepresentationTypesDialog::isoCurvesRepresentationModified


bool QtRepresentationTypesDialog::useIsoCurvesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useIsoCurvesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useIsoCurvesColor ( );
}	// QtRepresentationTypesDialog::useIsoCurvesColor


Color QtRepresentationTypesDialog::getIsoCurvesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getIsoCurvesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getIsoCurvesColor ( );
}	// QtRepresentationTypesDialog::getIsoCurvesColor


bool QtRepresentationTypesDialog::useIsoCurvesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useIsoCurvesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useIsoCurvesOpacity ( );
}	// QtRepresentationTypesDialog::useIsoCurvesOpacity


double QtRepresentationTypesDialog::getIsoCurvesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getIsoCurvesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getIsoCurvesOpacity ( );
}	// QtRepresentationTypesDialog::getIsoCurvesOpacity


THREESTATES QtRepresentationTypesDialog::useSurfacesRepresentation ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useSurfacesRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useSurfacesRepresentation ( );
}	// QtRepresentationTypesDialog::useSurfacesRepresentation


bool QtRepresentationTypesDialog::surfacesRepresentationModified ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::surfacesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->surfacesRepresentationModified ( );
}	// QtRepresentationTypesDialog::surfacesRepresentationModified


bool QtRepresentationTypesDialog::useSurfacesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useSurfacesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useSurfacesColor ( );
}	// QtRepresentationTypesDialog::useSurfacesColor


Color QtRepresentationTypesDialog::getSurfacesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getSurfacesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getSurfacesColor ( );
}	// QtRepresentationTypesDialog::getSurfacesColor


bool QtRepresentationTypesDialog::useSurfacesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useSurfacesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useSurfacesOpacity ( );
}	// QtRepresentationTypesDialog::useSurfacesOpacity


double QtRepresentationTypesDialog::getSurfacesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getSurfacesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getSurfacesOpacity ( );
}	// QtRepresentationTypesDialog::getSurfacesOpacity


THREESTATES QtRepresentationTypesDialog::useVolumesRepresentation ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useVolumesRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->useVolumesRepresentation ( );
}   // QtRepresentationTypesDialog::useVolumesRepresentation


THREESTATES QtRepresentationTypesDialog::useNamesRepresentation ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useNamesRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->useNamesRepresentation ( );
}   // QtRepresentationTypesDialog::useNamesRepresentation


bool QtRepresentationTypesDialog::volumesRepresentationModified ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::volumesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->volumesRepresentationModified ( );
}	// QtRepresentationTypesDialog::volumesRepresentationModified


bool QtRepresentationTypesDialog::useVolumesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useVolumesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useVolumesColor ( );
}	// QtRepresentationTypesDialog::useVolumesColor


Color QtRepresentationTypesDialog::getVolumesColor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getVolumesColor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getVolumesColor ( );
}	// QtRepresentationTypesDialog::getVolumesColor


bool QtRepresentationTypesDialog::useVolumesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useVolumesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useVolumesOpacity ( );
}	// QtRepresentationTypesDialog::useVolumesOpacity


double QtRepresentationTypesDialog::getVolumesOpacity ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getVolumesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getVolumesOpacity ( );
}	// QtRepresentationTypesDialog::getVolumesOpacity


bool QtRepresentationTypesDialog::useShrinkFactor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useShrinkFactor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useShrinkFactor ( );
}	// QtRepresentationTypesDialog::useShrinkFactor


double QtRepresentationTypesDialog::getShrinkFactor ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getShrinkFactor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getShrinkFactor ( );
}	// QtRepresentationTypesDialog::getShrinkFactor


bool QtRepresentationTypesDialog::useArrowComul ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useArrowComul", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->useArrowComul ( );
}	// QtRepresentationTypesDialog::useArrowComul


double QtRepresentationTypesDialog::getArrowComul ( ) const
{
	if (0 == _representationTypesPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getArrowComul", "Widget non instancié.")
		throw exc;
	}	// if (0 == _representationTypesPanel)

	return _representationTypesPanel->getArrowComul ( );
}	// QtRepresentationTypesDialog::getArrowComul


void QtRepresentationTypesDialog::getFontProperties (
	int& family, int& size, THREESTATES& bold, THREESTATES& italic,
	Color& color, bool& colorModified) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getFontProperties", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    _representationTypesPanel->getFontProperties (family, size, bold, italic, color, colorModified);
}	// QtRepresentationTypesDialog::getFontProperties


QtRepresentationTypesPanel::DISPLAYED_VALUE
				QtRepresentationTypesDialog::getDisplayedValueType ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getDisplayedValue", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->getDisplayedValueType ( );
}	// QtRepresentationTypesDialog::getDisplayedValueType


string QtRepresentationTypesDialog::getDisplayedValueName ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getDisplayedValueName", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->getDisplayedValueName ( );
}	// QtRepresentationTypesDialog::getDisplayedValueName


THREESTATES QtRepresentationTypesDialog::useAssociationsRepresentation ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useAssociationsRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->useAssociationsRepresentation ( );
}   // QtRepresentationTypesDialog::useAssociationsRepresentation


THREESTATES QtRepresentationTypesDialog::topoUseGeomColor ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::topoUseGeomColor", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->topoUseGeomColor ( );
}   // QtRepresentationTypesDialog::topoUseGeomColor


THREESTATES QtRepresentationTypesDialog::useMeshShapeRepresentation ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useMeshShapeRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->useMeshShapeRepresentation ( );
}   // QtRepresentationTypesDialog::useMeshShapeRepresentation


THREESTATES QtRepresentationTypesDialog::useDiscretisationRepresentation ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useDiscretisationRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->useDiscretisationRepresentation ( );
}   // QtRepresentationTypesDialog::useDiscretisationRepresentation


THREESTATES QtRepresentationTypesDialog::useNbMeshRepresentation ( ) const
{
    if (0 == _representationTypesPanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::useNbMeshRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _representationTypesPanel)

    return _representationTypesPanel->useNbMeshRepresentation ( );
}   // QtRepresentationTypesDialog::useDiscretisationRepresentation


QtDlgClosurePanel& QtRepresentationTypesDialog::getClosurePanel ( )
{
	if (0 == _closurePanel)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesDialog::getClosurePanel", "Widget non instancié.")
        throw exc;
    }   // if (0 == _closurePanel)

    return *_closurePanel;
}	// QtRepresentationTypesDialog::getClosurePanel


QtRepresentationTypesPanel& QtRepresentationTypesDialog::getRepresentationTypesPanel ( )
{
	CHECK_NULL_PTR_ERROR (_representationTypesPanel)
	return *_representationTypesPanel;
}	// QtRepresentationTypesDialog::getRepresentationTypesPanel


QtMgx3DMainWindow& QtRepresentationTypesDialog::getMainWindow ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtRepresentationTypesDialog::getMainWindow


RenderingManager& QtRepresentationTypesDialog::getRenderingManager ( )
{
	return getMainWindow ( ).getGraphicalWidget ( ).getRenderingManager ( );
}	// QtRepresentationTypesDialog::getRenderingManager


void QtRepresentationTypesDialog::applyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	const bool	globalMode	= getRenderingManager ( ).useGlobalDisplayProperties ( );
	const QtRepresentationTypesPanel::ExtendedDisplayProperties newProperties	= getDisplayProperties ( );
	THREESTATES	useGeomColorForTopo	= true == getRenderingManager ( ).topoUseGeomColor ( ) ? ON : OFF;
	if (topoUseGeomColor ( ) != getRenderingManager ( ).topoUseGeomColor ( ))
		getRenderingManager ( ).topoUseGeomColor (topoUseGeomColor ( ));
		
	if (true == globalMode)
	{
		Context&	context	= getRenderingManager ( ).getContext ( );
		for (Entity::objectType ot = Entity::GeomVertex; ot < Entity::undefined;
		     ot = (Entity::objectType)(ot + 1))
		{
			if (_globalMasks.end ( ) == _globalMasks.find (ot))
				continue;

			DisplayProperties&	globalProps	= context.globalDisplayProperties (ot);
			QtRepresentationTypesPanel::ExtendedDisplayProperties::updateDisplayProperties (newProperties, ot, globalMode, globalProps);
			unsigned long	newMask	= 0;
			unsigned long	mask	= context.globalMask (ot);
			updateMask (newMask, mask, GraphicalEntityRepresentation::CLOUDS,
			            useCloudRepresentation ( ));
			updateMask (newMask, mask, GraphicalEntityRepresentation::CURVES,
			            useCurvesRepresentation ( ));
			updateMask (newMask, mask, GraphicalEntityRepresentation::ISOCURVES,
			            useIsoCurvesRepresentation ( ));
			updateMask (newMask, mask, GraphicalEntityRepresentation::SURFACES,
			            useSurfacesRepresentation ( ));
			updateMask (newMask, mask, GraphicalEntityRepresentation::VOLUMES,
			            useVolumesRepresentation ( ));
			updateMask (newMask, mask, GraphicalEntityRepresentation::NAMES,
			            useNamesRepresentation ( ));
			updateMask (newMask, mask,
			            GraphicalEntityRepresentation::ASSOCIATIONS,
			            useAssociationsRepresentation ( ));
			updateMask (newMask, mask,
			            GraphicalEntityRepresentation::MESH_SHAPE,
			            useMeshShapeRepresentation ( ));
			updateMask (newMask, mask,
			            GraphicalEntityRepresentation::DISCRETISATIONTYPE,
			            useDiscretisationRepresentation ( ));
			updateMask (newMask, mask,
			            GraphicalEntityRepresentation::NBMESHINGEDGE,
			            useNbMeshRepresentation ( ));
			updateMask (newMask, getDisplayedValueType ( ));
			context.globalMask (ot)	= newMask;
		}	// for (Entity::objectType ot = Entity::GeomVertex; ...

		// Actualisation des représentations :
		for (vector<Entity*>::const_iterator it = _entities.begin ( );
		     _entities.end ( ) !=it; it++)
		{
			DisplayProperties::GraphicalRepresentation*	rep	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
			if (0 != rep)
			{
				unsigned long	mask	= rep->getRepresentationMask ( );
				rep->updateRepresentation (mask, true);
			}	// if (0 != rep)
		}	// for (vector<Entity*>::iterator it = _entities.begin ( ); ...
	}	// if (true == globalMode)
	else
	{

	// On applique le nouveau masque de représentation aux entités
	// sélectionnées :
	const bool	forceUpdate	=
		(true == newProperties.useShrinkFactor ( )) || (true == newProperties.useArrowComul ( )) ?
		true : false;
	for (vector<Entity*>::const_iterator it2 = _entities.begin ( );
	     _entities.end ( ) != it2; it2++)
	{
		CHECK_NULL_PTR_ERROR ((*it2)->getDisplayProperties ( ).getGraphicalRepresentation ( ))
		unsigned long	mask	=
			(*it2)->getDisplayProperties ( ).getGraphicalRepresentation ( )->getRepresentationMask ( );
		unsigned long	newMask	= 0;
		updateMask (newMask, mask, GraphicalEntityRepresentation::CLOUDS,
		            useCloudRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::CURVES,
		            useCurvesRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::ISOCURVES,
		            useIsoCurvesRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::SURFACES,
		            useSurfacesRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::VOLUMES,
		            useVolumesRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::NAMES,
		            useNamesRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::ASSOCIATIONS,
		            useAssociationsRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::MESH_SHAPE,
		            useMeshShapeRepresentation ( ));
		updateMask (newMask, mask,
		            GraphicalEntityRepresentation::DISCRETISATIONTYPE,
		            useDiscretisationRepresentation ( ));
		updateMask (newMask, mask, GraphicalEntityRepresentation::NBMESHINGEDGE,
		            useNbMeshRepresentation ( ));
		updateMask (newMask, getDisplayedValueType ( ));

		DisplayProperties&	properties	= (*it2)->getDisplayProperties ( );
		QtRepresentationTypesPanel::ExtendedDisplayProperties
			::updateDisplayProperties (getDisplayProperties ( ),
								(*it2)->getType ( ), globalMode, properties);
		getRenderingManager ( ).updateRepresentation (**it2, newMask, forceUpdate);
	}	// for (vector<Entity*>::iterator it2 = ...

	}	// else if (true == globalMode)

	// En cas de changement de couleurs il faudra procéder à une actualisation
	// des entités affichées : des entités topo peuvent avoir à synchroniser
	// leur couleur d'affichage avec celle de l'entité géométrique associée :
	if ((OFF != topoUseGeomColor ( )) && (true == colorModified ( )))
		getRenderingManager ( ).updateRepresentations ( );

	getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, getMainWindow ( ).getAppTitle ( ))
}	// QtRepresentationTypesDialog::applyCallback


// ============================================================================
//                               FONCTIONS STATIQUES
// ============================================================================

/**
 * Enlève de sharedNames les noms non contenus dans names.
 */
static void updateSet (set<string>& sharedNames, const vector<string>& names)
{
	set<string>	newset;
	bool		usenewset	= false;

	for (set<string>::const_iterator itsn = sharedNames.begin ( );
		 sharedNames.end ( ) != itsn; itsn++)
		if (names.end ( ) ==
						std::find (names.begin ( ), names.end ( ), *itsn))
			usenewset	= true;
		else
			newset.insert (*itsn);

	if (true == usenewset)
		sharedNames	= newset;
}	// updateSet


static void updateMask (unsigned long& newMask, unsigned long dlgMask, unsigned long kind, THREESTATES used)
{
	if (UNDETERMINED == used)
		newMask	|= (dlgMask & kind);
	else if (ON == used)
		newMask	|= kind;
}	// updateMask


static void updateMask (unsigned long& newMask, QtRepresentationTypesPanel::DISPLAYED_VALUE kind)
{
	switch (kind)
	{
		case QtRepresentationTypesPanel::NODE_VALUE	:
				newMask	|= GraphicalEntityRepresentation::NODES_VALUES;	break;
		case QtRepresentationTypesPanel::CELL_VALUE	:
				newMask	|= GraphicalEntityRepresentation::CELLS_VALUES;	break;
	}	// switch (kind)
}	// updateMask


}	// namespace QtComponents

}	// namespace Mgx3D
