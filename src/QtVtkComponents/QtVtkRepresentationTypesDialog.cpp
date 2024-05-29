/**
 * \file        QtVtkRepresentationTypesDialog.cpp
 * \author      Charles PIGNEROL
 * \date        24/11/2014
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/QtVtkRepresentationTypesDialog.h"
#include "Utils/Common.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ThreeStates.h>
#include <QtUtil/QtConfiguration.h>

#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//     LA CLASSE QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel
// ===========================================================================

QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel::QtVtkFontPropertiesPanel (
	QWidget* parent, const string& name, int fontFamily, int fontSize,
	THREESTATES bold, THREESTATES italic, const Color& fontColor)
	: QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel (parent),
	  _fontPanel (0)
{
	QHBoxLayout*	layout	= new QHBoxLayout (this);
	layout->setMargin (0);
	layout->setSpacing (0);
	layout->setContentsMargins (0, 0, 0, 0);
	_fontPanel	= new QtVTKFontParametersPanel (
		this, name.c_str ( ), fontFamily, fontSize, fontColor, bold, italic);
	layout->addWidget (_fontPanel);
}	// QtVtkFontPropertiesPanel::QtVtkFontPropertiesPanel


QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel::QtVtkFontPropertiesPanel (
			const QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel&)
	: QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel (0),
	  _fontPanel (0)
{
	MGX_FORBIDDEN ("QtVtkRepresentationTypesDialog copy constructor is not allowed.");
}	// QtVtkFontPropertiesPanel::QtVtkFontPropertiesPanel


QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel&
	QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel::operator = (
			 const QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel&)
{
	MGX_FORBIDDEN ("QtVtkRepresentationTypesDialog operator = is not allowed.");
	return *this;
}	// QtVtkFontPropertiesPanel::operator =


QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel::~QtVtkFontPropertiesPanel ( )
{
}	// QtVtkFontPropertiesPanel::~QtVtkFontPropertiesPanel


void QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel::getFontProperties (
		int& family, int& size, THREESTATES& bold, THREESTATES& italic,
		Color& color, bool& colorModified) const
{
	CHECK_NULL_PTR_ERROR (_fontPanel)
	family		= _fontPanel->getFontFamily ( );
	size		= _fontPanel->getFontSize ( );
	bold		= _fontPanel->isBold ( );
	italic		= _fontPanel->isItalic ( );
	color		= _fontPanel->getColor ( );
	colorModified	= _fontPanel->isColorModified ( );
}	// QtVtkFontPropertiesPanel::getFontProperties


// ===========================================================================
//                        LA CLASSE QtVtkRepresentationTypesDialog
// ===========================================================================

QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog (
			QtMgx3DMainWindow* parent,
			const string& name, const string& appTitle,
			const vector<Entity*>& entities,
			const std::string& helpURL, const std::string& helpTag
		)
	: QtRepresentationTypesDialog (parent, appTitle, name, entities, helpURL, helpTag)
{
	bool	globalMode	= getRenderingManager( ).useGlobalDisplayProperties( );
	bool			inited	= false;
	THREESTATES		bold = OFF, italic = OFF;
	int				fontFamily	= 0, fontSize	= 12;
	bool			fontFamily3S	= true, fontSize3S	= true,
					fontColor3S = true, valueName3S = true;
	Color			fontColor (0, 0, 0);
	for (vector<Entity*>::const_iterator it1 = entities.begin ( ); entities.end ( ) != it1; it1++)
	{
		CHECK_NULL_PTR_ERROR ((*it1)->getDisplayProperties ( ).getGraphicalRepresentation ( ))
		RenderedEntityRepresentation*	rep	=
			dynamic_cast<RenderedEntityRepresentation*>(
				(*it1)->getDisplayProperties ( ).getGraphicalRepresentation( ));
		CHECK_NULL_PTR_ERROR (rep)
		DisplayProperties	properties	= true == globalMode ?
								rep->getDisplayPropertiesAttributes ( ) :
								(*it1)->getDisplayProperties ( );
		if (false == inited)
		{
			bool	fontBold	= false, fontItalic	= false;
			properties.getFontProperties (
						fontFamily, fontSize, fontBold, fontItalic, fontColor);
			bold	= true == fontBold   ? ON : OFF;
			italic	= true == fontItalic ? ON : OFF;
			inited	= true;
		}	// if (false == inited)
		else
		{
			int			tmpFontFamily	= 0, tmpFontSize	= 12;
			bool		tmpBold	= OFF, tmpItalic	= OFF;
			Color		tmpFontColor (0, 0, 0);
			properties.getFontProperties (
				tmpFontFamily, tmpFontSize, tmpBold, tmpItalic, tmpFontColor);
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
		}	// else if (false == inited)
	}	// for (vector<Entity*>::const_iterator it1 = entities.begin ( );

	QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel*	fontPanel	=
		new QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel (&getRepresentationTypesPanel ( ), "Police de caractères",
				fontFamily, fontSize, bold, italic, fontColor);
	getRepresentationTypesPanel ( ).setFontPropertiesPanel (*fontPanel); 

	adjustSize ( );
}	// QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog


QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog (
	QtMgx3DMainWindow* parent, const string& appTitle, bool globalMode,
			THREESTATES repCloud, THREESTATES repCurves,
			THREESTATES repIsoCurves, THREESTATES repSurfaces,
            THREESTATES repVolumes, THREESTATES repNames,
			THREESTATES repAssociations, THREESTATES topoColor,
            THREESTATES repMeshShape, THREESTATES repDiscretisationType,
			THREESTATES repNbMesh,
            bool pointsSelectable, bool linesSelectable,
            bool isoLinesSelectable, bool surfacesSelectable,
            bool volumesSelectable, bool namesSelectable,
			bool associationsSelectable,
            bool meshShapeSelectable, bool discRepSelectable,
            bool nbMeshSelectable,
			const Color& cloudColor, 
			const Color& curvesColor, 
			const Color& isoCurvesColor, 
			const Color& surfacesColor, 
			const Color& volumesColor,
			bool cloudColorOK, bool curvesColorOK, bool isoCurvesColorOK,
			bool surfacesColorOK, bool volumesColorOK,
			double cloudOpacity, double curvesOpacity, double isoCurvesOpacity,
			double surfacesOpacity, double volumesOpacity,
			bool cloudOpacityOK, bool curvesOpacityOK, bool isoCurvesOpacityOK,
			bool surfacesOpacityOK, bool volumesOpacityOK,
			double shrinkFactor, bool shrinkFactorOK,
			double arrowComul, bool arrowComulOK,
			int fontFamily, int fontSize,
			THREESTATES bold, THREESTATES italic,
			const Color& fontColor,
			bool displayValue, QtRepresentationTypesPanel::DISPLAYED_VALUE displayedValue,
			const set< string >& nodesValuesNames,
			const set< string >& cellsValuesNames,
			const string& displayedValueName,
			const string& helpURL, const string& helpTag)
	: QtRepresentationTypesDialog (parent, appTitle, globalMode, 
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
		displayValue, displayedValue, nodesValuesNames, cellsValuesNames,displayedValueName, helpURL, helpTag)
{
	QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel*	fontPanel	=
		new QtVtkRepresentationTypesDialog::QtVtkFontPropertiesPanel (
						&getRepresentationTypesPanel ( ), "Police de caractères", fontFamily, fontSize, bold, italic, fontColor);
	getRepresentationTypesPanel ( ).setFontPropertiesPanel (*fontPanel); 

	adjustSize ( );
}	// QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog


QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog (const QtVtkRepresentationTypesDialog&)
	: QtRepresentationTypesDialog (0, "Invalid dialog", "Invalid dialog",
			vector<Entity*> ( ), string ( ), string ( ))
{
	MGX_FORBIDDEN ("QtVtkRepresentationTypesDialog copy constructor is not allowed.");
}	// QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog (const QtVtkRepresentationTypesDialog&)


QtVtkRepresentationTypesDialog& QtVtkRepresentationTypesDialog::operator = (const QtVtkRepresentationTypesDialog&)
{
	MGX_FORBIDDEN ("QtVtkRepresentationTypesDialog assignment operator is not allowed.");
	return *this;
}	// QtVtkRepresentationTypesDialog::QtVtkRepresentationTypesDialog (const QtVtkRepresentationTypesDialog&)


QtVtkRepresentationTypesDialog::~QtVtkRepresentationTypesDialog ( )
{
}	// QtVtkRepresentationTypesDialog::~QtVtkRepresentationTypesDialog


}	// namespace QtVtkComponents

}	// namespace Mgx3D
