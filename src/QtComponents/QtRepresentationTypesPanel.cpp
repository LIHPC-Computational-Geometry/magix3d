/**
 * \file        QtRepresentationTypesPanel.cpp
 * \author      Charles PIGNEROL
 * \date        17/01/2011
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtRepresentationTypesPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DScrollArea.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/ThreeStates.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QColorDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;



namespace Mgx3D
{

namespace QtComponents
{


// ============================================================================
//                         LA CLASSE QtColorationWidget
// ============================================================================

static QColor colorToQColor (Color c);


QtColorationWidget::QtColorationWidget (
				QWidget* parent, const Color& bg, bool useColor)
	: QWidget (parent), _initialColor (bg), _useColor (useColor)
{
	setAutoFillBackground (true);
	setBackground (colorToQColor (bg));
}	// QtColorationWidget::QtColorationWidget


QtColorationWidget::QtColorationWidget (const QtColorationWidget&)
	: QWidget (0), _initialColor (0., 0., 0.), _useColor (false)
{
	MGX_FORBIDDEN ("QtColorationWidget copy constructor is not allowed.");
}	// QtColorationWidget::QtColorationWidget


QtColorationWidget& QtColorationWidget::operator = (const QtColorationWidget&)
{
	MGX_FORBIDDEN ("QtColorationWidget assignment operator is not allowed.");
	return *this;
}	// QtColorationWidget::operator =


QtColorationWidget::~QtColorationWidget ( )
{
}	// QtColorationWidget::~QtColorationWidget


bool QtColorationWidget::isModified ( ) const
{
	return _initialColor == getColor ( ) ? false : true;
}	// QtColorationWidget::isModified


void QtColorationWidget::setBackground (const QColor& c)
{
	QPalette	p	= palette ( );
	p.setColor (QPalette::Active, QPalette::Window, c);
	setPalette (p);
}	// QtColorationWidget::setBackground


Color QtColorationWidget::getColor ( ) const
{
	QColor	c	= palette ( ).color (QPalette::Window);
//	qreal	r = 0., g = 0., b = 0., a = 0.;
//	c.getRgbF (&r, &g, &b, &a);
	int	r = 0, g = 0, b = 0, a = 0.;
	c.getRgb (&r, &g, &b, &a);

	return Color (r, g, b);
}	// QtColorationWidget::getColor


bool QtColorationWidget::useColor ( ) const
{
	return _useColor;
}	// QtColorationWidget::useColor


void QtColorationWidget::editColorCallback ( )
{
	QPalette	p	= palette ( );
	QColor		c	= p.color (QPalette::Window);
	c	= QColorDialog::getColor (c, this, QString::fromUtf8("Choix de la couleur"), QColorDialog::DontUseNativeDialog);
	if (true == c.isValid ( ))
	{
		setBackground (c);
		_useColor	= true;
	}
}	// QtColorationWidget::editColorCallback


// ============================================================================
//                         LA CLASSE QtOpacityWidget
// ============================================================================


QtOpacityWidget::QtOpacityWidget (
				QWidget* parent, double opacity, bool useOpacity)
	: QtDoubleTextField (opacity, parent),
	  _initialOpacity (opacity), _useOpacity (useOpacity)
{
	setNotation (QDoubleValidator::StandardNotation);
	setRange (0., 1.);
	setDecimals (2);
	setVisibleColumns (4);
	setValue (opacity);
}	// QtOpacityWidget::QtOpacityWidget


QtOpacityWidget::QtOpacityWidget (const QtOpacityWidget&)
	: QtDoubleTextField (0), _initialOpacity (1.), _useOpacity (false)
{
	MGX_FORBIDDEN ("QtOpacityWidget copy constructor is not allowed.");
}	// QtOpacityWidget::QtOpacityWidget


QtOpacityWidget& QtOpacityWidget::operator = (const QtOpacityWidget&)
{
	MGX_FORBIDDEN ("QtOpacityWidget assignment operator is not allowed.");
	return *this;
}	// QtOpacityWidget::operator =


QtOpacityWidget::~QtOpacityWidget ( )
{
}	// QtOpacityWidget::~QtOpacityWidget


bool QtOpacityWidget::isModified ( ) const
{
	return _initialOpacity == getOpacity ( ) ? false : true;
}	// QtOpacityWidget::isModified


double QtOpacityWidget::getOpacity ( ) const
{
	return getValue ( );
}	// QtOpacityWidget::getOpacity


bool QtOpacityWidget::useOpacity ( ) const
{
	return _useOpacity;
}	// QtOpacityWidget::useOpacity


bool QtOpacityWidget::validate ( )
{
	if (true == QtDoubleTextField::validate ( ))
	{
		_useOpacity	= true;
        return true;
	}
    return false;
}	// QtOpacityWidget::validate


// ============================================================================
//                     LA CLASSE ExtendedDisplayProperties
// ============================================================================

QtRepresentationTypesPanel::ExtendedDisplayProperties::ExtendedDisplayProperties ( )
	: DisplayProperties ( ),
	  _useCloudColor (true), _useWireColor (true),
	  _useSurfacicColor (true), _useVolumicColor (true),
	  _useCloudOpacity (true), _useWireOpacity (true),
	  _useSurfacicOpacity (true), _useVolumicOpacity (true),
	  _useShrinkFactor (true), _useArrowComul (true)
{
}	// ExtendedDisplayProperties::ExtendedDisplayProperties


QtRepresentationTypesPanel::ExtendedDisplayProperties::ExtendedDisplayProperties (const DisplayProperties& dp)
	: DisplayProperties (dp),
	  _useCloudColor (true), _useWireColor (true),
	  _useSurfacicColor (true), _useVolumicColor (true),
	  _useCloudOpacity (true), _useWireOpacity (true),
	  _useSurfacicOpacity (true), _useVolumicOpacity (true),
	  _useShrinkFactor (true), _useArrowComul (true)
{
}	// ExtendedDisplayProperties::ExtendedDisplayProperties


QtRepresentationTypesPanel::ExtendedDisplayProperties::ExtendedDisplayProperties (
	const QtRepresentationTypesPanel::ExtendedDisplayProperties& edp)
	: DisplayProperties (edp),
	  _useCloudColor (edp._useCloudColor), _useWireColor (edp._useWireColor),
	  _useSurfacicColor (edp._useSurfacicColor),
	  _useVolumicColor (edp._useVolumicColor),
	  _useCloudOpacity (edp._useCloudOpacity),
	  _useWireOpacity (edp._useWireOpacity),
	  _useSurfacicOpacity (edp._useSurfacicOpacity),
	  _useVolumicOpacity (edp._useVolumicOpacity),
	  _useShrinkFactor (edp._useShrinkFactor),
	  _useArrowComul (edp._useArrowComul)
{
}	// ExtendedDisplayProperties::ExtendedDisplayProperties


QtRepresentationTypesPanel::ExtendedDisplayProperties&
	QtRepresentationTypesPanel::ExtendedDisplayProperties::operator = (
			const QtRepresentationTypesPanel::ExtendedDisplayProperties& edp)
{
	if (&edp != this)
	{
		DisplayProperties::operator = (edp);
		_useCloudColor		= edp._useCloudColor;
		_useWireColor		= edp._useWireColor;
		_useSurfacicColor	= edp._useSurfacicColor;
		_useVolumicColor	= edp._useVolumicColor;
		_useCloudOpacity	= edp._useCloudOpacity;
		_useWireOpacity		= edp._useWireOpacity;
		_useSurfacicOpacity	= edp._useSurfacicOpacity;
		_useVolumicOpacity	= edp._useVolumicOpacity;
		_useShrinkFactor	= edp._useShrinkFactor;
		_useArrowComul		= edp._useArrowComul;
	}	// if (&edp != this)

	return *this;
}	// ExtendedDisplayProperties::ExtendedDisplayProperties


QtRepresentationTypesPanel::ExtendedDisplayProperties::~ExtendedDisplayProperties ( )
{
}	// ExtendedDisplayProperties::~ExtendedDisplayProperties


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useCloudColor ( ) const
{
	return _useCloudColor;
}	// ExtendedDisplayProperties::useCloudColor


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useCloudColor (bool use)
{
	_useCloudColor	= use;
}	// ExtendedDisplayProperties::useCloudColor


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useWireColor ( ) const
{
	return _useWireColor;
}	// ExtendedDisplayProperties::useWireColor


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useWireColor (bool use)
{
	_useWireColor	= use;
}	// ExtendedDisplayProperties::useWireColor


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useSurfacicColor ( ) const
{
	return _useSurfacicColor;
}	// ExtendedDisplayProperties::useSurfacicColor


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useSurfacicColor (bool use)
{
	_useSurfacicColor	= use;
}	// ExtendedDisplayProperties::useSurfacicColor


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useVolumicColor ( ) const
{
	return _useVolumicColor;
}	// ExtendedDisplayProperties::useCloudColor


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useVolumicColor (bool use)
{
	_useVolumicColor	= use;
}	// ExtendedDisplayProperties::useVolumicColor


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useCloudOpacity ( ) const
{
	return _useCloudOpacity;
}	// ExtendedDisplayProperties::useCloudOpacity


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useCloudOpacity (bool use)
{
	_useCloudOpacity	= use;
}	// ExtendedDisplayProperties::useCloudOpacity


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useWireOpacity ( ) const
{
	return _useWireOpacity;
}	// ExtendedDisplayProperties::useWireOpacity


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useWireOpacity (bool use)
{
	_useWireOpacity	= use;
}	// ExtendedDisplayProperties::useWireOpacity


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useSurfacicOpacity ( ) const
{
	return _useSurfacicOpacity;
}	// ExtendedDisplayProperties::useSurfacicOpacity


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useSurfacicOpacity (bool use)
{
	_useSurfacicOpacity	= use;
}	// ExtendedDisplayProperties::useSurfacicOpacity


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useVolumicOpacity ( ) const
{
	return _useVolumicOpacity;
}	// ExtendedDisplayProperties::useVolumicOpacity


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useVolumicOpacity (bool use)
{
	_useVolumicOpacity	= use;
}	// ExtendedDisplayProperties::useVolumicOpacity


bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useShrinkFactor ( ) const
{
	return _useShrinkFactor;
}	// ExtendedDisplayProperties::useShrinkFactor


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useShrinkFactor (bool use)
{
	_useShrinkFactor	= use;
}	// ExtendedDisplayProperties::useShrinkFactor



bool QtRepresentationTypesPanel::ExtendedDisplayProperties::useArrowComul ( ) const
{
	return _useArrowComul;
}	// ExtendedDisplayProperties::useArrowComul


void QtRepresentationTypesPanel::ExtendedDisplayProperties::useArrowComul (bool use)
{
	_useArrowComul	= use;
}	// ExtendedDisplayProperties::useArrowComul


void QtRepresentationTypesPanel::ExtendedDisplayProperties::updateDisplayProperties (
		const ExtendedDisplayProperties& p1, Utils::Entity::objectType type,
		bool globalMode, Utils::DisplayProperties& p2)
{
	switch (type)
	{
		// Pour tout ce qui est topologie on ne modifie pas les couleurs car
		// elles comportent des codes, utilisés quelque soit le mode de
		// représentation (propriétés globales/individuelles, cf. MGXDDD-208).
		case Entity::TopoBlock	:
		case Entity::TopoFace	:
		case Entity::TopoCoFace	:
		case Entity::TopoEdge	:
		case Entity::TopoCoEdge	:
		case Entity::TopoVertex	:
			break;
		default					:
			// Couleurs entités géométriques et maillage : on n'y touche qu'en
			// mode non global (MGXDDD-208) :
			if (false == globalMode)
			{
				if (true == p1.useCloudColor ( ))
					p2.setCloudColor (p1.getCloudColor ( ));
				if (true == p1.useWireColor ( ))
					p2.setWireColor (p1.getWireColor ( ));
				if (true == p1.useSurfacicColor ( ))
					p2.setSurfacicColor (p1.getSurfacicColor ( ));
				if (true == p1.useVolumicColor ( ))
					p2.setVolumicColor (p1.getVolumicColor ( ));
			}	// if (false == globalMode)
			p2.setValueName (p1.getValueName ( ));
	}	// switch (type)

	if (true == p1.useCloudOpacity ( ))
		p2.setCloudOpacity (p1.getCloudOpacity ( ));
	if (true == p1.useWireOpacity ( ))
		p2.setWireOpacity (p1.getWireOpacity ( ));
	if (true == p1.useSurfacicOpacity ( ))
		p2.setSurfacicOpacity (p1.getSurfacicOpacity ( ));
	if (true == p1.useVolumicOpacity ( ))
		p2.setVolumicOpacity (p1.getVolumicOpacity ( ));
	if (true == p1.useShrinkFactor ( ))
		p2.setShrinkFactor (p1.getShrinkFactor ( ));
	if (true == p1.useArrowComul ( ))
		p2.setArrowComul (p1.getArrowComul ( ));
	int		fontFamily	= -1,		fontSize	= -1;
	bool	bold		= false,	italic		= false;
	Color	fontColor (0, 0, 0);
	p1.getFontProperties (fontFamily, fontSize, bold, italic, fontColor);
	p2.setFontProperties (fontFamily, fontSize, bold, italic, fontColor);
}	// ExtendedDisplayProperties::updateDisplayProperties


// ============================================================================
//                              FONCTIONS STATIQUES
// ============================================================================


static THREESTATES checkStateToThreeState (Qt::CheckState state)
{
	switch (state)
	{
		case	Qt::Unchecked			: return OFF;
		case	Qt::PartiallyChecked	: return UNDETERMINED;
		case	Qt::Checked				: return ON;
	}	// switch (state)

	UTF8String	message (Charset::UTF_8);
	message << "checkStateToThreeState. Etat inconnu : "
	        << (unsigned long)state << ".";
	throw Exception (message);
}	// checkStateToThreeState


static Qt::CheckState threeStateToCheckState (THREESTATES state)
{
	switch (state)
	{
		case ON				: return Qt::Checked;
		case OFF			: return Qt::Unchecked;
		case UNDETERMINED	: return Qt::PartiallyChecked;
	}	// switch (state)

	UTF8String	message (Charset::UTF_8);
	message << "threeStateToCheckState. Etat inconnu : "
	        << (unsigned long)state << ".";
	throw Exception (message);
}	// threeStateToCheckState


static QColor colorToQColor (Color c)
{
//	QColor	qc ((int)(255 * c.getRed ( )), (int)(255 * c.getGreen ( )),
//			    (int)(255 * c.getBlue ( )));
	QColor	qc	(c.getRed ( ), c.getGreen ( ), c.getBlue ( ));

	return qc;
}	// colorToQColor


static QWidget* createColorWidgets (
	QtColorationWidget*& colorWidget, QPushButton*& colorButton,
	const Color& color, bool useColor)
{
	QWidget*		parent	= new QWidget ( );
	QHBoxLayout*	layout	= new QHBoxLayout (parent);
	parent->setLayout (layout);

	// Widget montrant la couleur :
	colorWidget	= new QtColorationWidget (0, color, useColor);
	layout->addWidget (colorWidget);

	// Bouton de modification de la couleur :
	colorButton	= new QPushButton ("Couleur ...", 0);
	layout->addWidget (colorButton);
	colorWidget->connect (colorButton, SIGNAL (clicked ( )), colorWidget,
	         SLOT (editColorCallback ( )));

	QSize	size	= colorButton->sizeHint ( );
	size.setWidth (size.width ( ) / 2);
	colorWidget->setMinimumSize (size);

	return parent;
}	// createColorWidgets


static QWidget* createOpacityWidget (QtOpacityWidget*& dtf, double value)
{
	QWidget*		parent	= new QWidget ( );
	QHBoxLayout*	layout	= new QHBoxLayout (parent);
	parent->setLayout (layout);
	QLabel*	label	= new QLabel (QString::fromUtf8("Opacité :"));
	layout->addWidget (label);
	dtf	= new QtOpacityWidget (0, value, false);
	layout->addWidget (dtf);
	UTF8String	tip (Charset::UTF_8);
	tip << "Opacité des entités, comprise entre 0 (transparence totale) et 1 (opacité totale).";
	label->setToolTip (UTF8TOQSTRING (tip));
	dtf->setToolTip (UTF8TOQSTRING (tip));

	return parent;
}	// createOpacityWidget


static QWidget* createShrinkWidget (QtOpacityWidget*& dtf, double value)
{
	QWidget*		parent	= new QWidget ( );
	QHBoxLayout*	layout	= new QHBoxLayout (parent);
	parent->setLayout (layout);
	QLabel*	label	= new QLabel (QString::fromUtf8("Rétrécissement :"));
	layout->addWidget (label);
	dtf	= new QtOpacityWidget (0, value, false);
	layout->addWidget (dtf);
	UTF8String	tip (Charset::UTF_8);
	tip << "Facteur de rétrécissement des entités, compris entre 0 (taille nulle) et 1"
	    << " (taille normale).";
	label->setToolTip (UTF8TOQSTRING (tip));
	dtf->setToolTip (UTF8TOQSTRING (tip));

	return parent;
}	// createShrinkWidget


static QWidget* createArrowComulWidget (QtOpacityWidget*& dtf, double value, double min, double max)
{
	QWidget*		parent	= new QWidget ( );
	QHBoxLayout*	layout	= new QHBoxLayout (parent);
	parent->setLayout (layout);
	QLabel*	label	= new QLabel (QString::fromUtf8("Agrandissement :"));
	layout->addWidget (label);
	dtf	= new QtOpacityWidget (0, 1, false);
	layout->addWidget (dtf);
	dtf->setRange (min, max);
	dtf->setValue (value);
	UTF8String	tip (Charset::UTF_8);
	try
	{
		tip << "Facteur de grossissement des flèches \"association topologie -> géométrie\""
		    << ", compris entre " << NumericConversions::shortestRepresentation (min, 1, 3) 
		    << " et " << NumericConversions::shortestRepresentation (max, 3, 3)
		    << ". Taille par défaut : 1.";
	}
	catch (...)
	{
	}
	label->setToolTip (UTF8TOQSTRING (tip));
	dtf->setToolTip (UTF8TOQSTRING (tip));

	return parent;
}	// createArrowComulWidget


// ===========================================================================
//    LA CLASSE QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel
// ===========================================================================


QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel::QtAbstractFontPropertiesPanel (QWidget* parent)
	: QWidget (parent)
{
}	// QtAbstractFontPropertiesPanel::QtAbstractFontPropertiesPanel


QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel::QtAbstractFontPropertiesPanel (
		const QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel&)
	: QWidget (0)
{
	MGX_FORBIDDEN ("QtAbstractFontPropertiesPanel copy constructor is not allowed.");
}	// QtAbstractFontPropertiesPanel::QtAbstractFontPropertiesPanel


QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel&
	QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel::operator = (
		const QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel&)
{
	MGX_FORBIDDEN ("QtAbstractFontPropertiesPanel assignment operator is not allowed.");
	return *this;
}	// QtAbstractFontPropertiesPanel::operator =


QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel::~QtAbstractFontPropertiesPanel ( )
{
}	// QtAbstractFontPropertiesPanel::~QtAbstractFontPropertiesPanel


void QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel::getFontProperties (
	int& family, int& size,
	TkUtil::THREESTATES& bold, TkUtil::THREESTATES& italic,
	TkUtil::Color& color, bool& colorModified) const
{
	throw Exception ("QtAbstractFontPropertiesPanel::getFontProperties should be overloaded.");
}	// QtAbstractFontPropertiesPanel::getFontProperties


// ===========================================================================
//                        LA CLASSE QtRepresentationTypesPanel
// ===========================================================================

static const int	margin	= 0;
static const int	spacing	= 0;

QtRepresentationTypesPanel::QtRepresentationTypesPanel (
			QWidget* parent, const string& name, bool globalMode,
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
			int fontFamily, int fontSize, THREESTATES bold, THREESTATES italic,
			const Color& fontColor,
			bool displayValue,
			QtRepresentationTypesPanel::DISPLAYED_VALUE displayedValue,
			const set< string >& nodesValuesNames,
			const set< string >& cellsValuesNames,
			const string& displayedValueName
			)
	: QtMgx3DGroupBox (name.c_str ( ), parent, 5, 0),
	  _nodesValuesNames (nodesValuesNames),
	  _cellsValuesNames (cellsValuesNames),
	  _repCloudCheckBox (0), _repCurvesCheckBox (0), _repIsoCurvesCheckBox (0),
	  _repSurfacesCheckBox (0), _repVolumesCheckBox (0), _repNamesCheckBox (0),
	  _repAssociationCheckBox (0), _topoColorCheckBox (0),
	  _repDiscretisationCheckBox (0), _repNbMeshCheckBox (0),
	  _cloudColorWidget (0), _curvesColorWidget (0), _isoCurvesColorWidget (0),
	  _surfacesColorWidget (0), _volumesColorWidget (0),
	  _cloudColorButton (0), _curvesColorButton (0), _isoCurvesColorButton (0),
	  _surfacesColorButton (0), _volumesColorButton (0),
	  _cloudOpacityTextField (0), _curvesOpacityTextField (0),
	  _isoCurvesOpacityTextField (0), _surfacesOpacityTextField (0),
	  _volumesOpacityTextField (0),
	  _shrinkFactorTextField (0), _arrowComulTextField (0),
	  _valueTypeComboBox (0), _valueNameComboBox (0),
	  _fontPanel (0), _gridLayout (0)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout ( );
	setAreaLayout (mainLayout);
	mainLayout->setSpacing (0);
	mainLayout->setMargin (5);
	QtMgx3DScrollArea*	scrollArea	= new QtMgx3DScrollArea (this);
#ifdef QT_4
	scrollArea->setZoomFactor (1.15, 1.3);	// Cooking
#else	// QT_4
	scrollArea->setZoomFactor (1.15, 1.4);	// Cooking
#endif	// QT_4
	scrollArea->setFrameShape (QFrame::NoFrame);
	scrollArea->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
	mainLayout->addWidget (scrollArea);

	scrollArea->setWidgetResizable (true);
	QWidget*	panel		= new QWidget ( );
	scrollArea->setWidget (panel);
	_gridLayout			= new QGridLayout ( );
	_gridLayout->setMargin (margin);
	_gridLayout->setSpacing (0);
	_gridLayout->setSizeConstraint (QLayout::SetMinimumSize);
	panel->setLayout (_gridLayout);

	int			row		= 0;
	QLabel*			label		= 0;	
	QHBoxLayout*		hlayout		= 0;
	QtMgx3DGroupBox*	groupBox	= 0;
	QWidget*		w		= 0;
	QPalette		p;
	QSize			colorSize;

	// 0ème (!) rangée : mode d'affichage :
	UTF8String	modeString ("Mode : propriétés d'affichage ", Charset::UTF_8);
	modeString << (true == globalMode ? "globales" : "individuelles");
	label	= new QLabel (UTF8TOQSTRING (modeString), this);
	_gridLayout->addWidget (label, row, 0, 1, 3);	row++;

	// 1ère rangée : points :
	groupBox			= new QtMgx3DGroupBox ("Points", this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	_repCloudCheckBox	= new QCheckBox ("Afficher", 0);
	_repCloudCheckBox->setTristate (UNDETERMINED == repCloud ? true : false);
	_repCloudCheckBox->setCheckState (threeStateToCheckState (repCloud));
	hlayout->addWidget (_repCloudCheckBox);
	w	= createColorWidgets (
			_cloudColorWidget, _cloudColorButton, cloudColor, curvesColorOK);
	hlayout->addWidget (w);
	w->setEnabled (!globalMode);	// Mode global => couleurs non modifiables
	w	= createOpacityWidget (_cloudOpacityTextField, cloudOpacity);
	hlayout->addWidget (w);
	groupBox->setVisible(pointsSelectable);

	// 2ème rangée : filaire :
	groupBox			= new QtMgx3DGroupBox ("Filaire", this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	_repCurvesCheckBox	= new QCheckBox ("Afficher", 0);
	hlayout->addWidget (_repCurvesCheckBox);
	_repCurvesCheckBox->setTristate (UNDETERMINED == repCurves ? true : false);
	_repCurvesCheckBox->setCheckState (threeStateToCheckState (repCurves));
	w	= createColorWidgets (
			_curvesColorWidget, _curvesColorButton, curvesColor, curvesColorOK);
	hlayout->addWidget (w);
	w->setEnabled (!globalMode);	// Mode global => couleurs non modifiables
	w	= createOpacityWidget (_curvesOpacityTextField, curvesOpacity);
	hlayout->addWidget (w);
	groupBox->setVisible(linesSelectable);

	// 3ème rangée : isofilaire :
	groupBox			= new QtMgx3DGroupBox ("IsoFilaire", this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	_repIsoCurvesCheckBox	= new QCheckBox ("Afficher", groupBox);
	hlayout->addWidget (_repIsoCurvesCheckBox);
	_repIsoCurvesCheckBox->setTristate (
								UNDETERMINED == repIsoCurves ? true : false);
	_repIsoCurvesCheckBox->setCheckState (threeStateToCheckState(repIsoCurves));
	w	= createColorWidgets (
				_isoCurvesColorWidget, _isoCurvesColorButton, isoCurvesColor,
				isoCurvesColorOK);
	hlayout->addWidget (w);
	w->setEnabled (!globalMode);	// Mode global => couleurs non modifiables
	w	= createOpacityWidget (_isoCurvesOpacityTextField, isoCurvesOpacity);
	hlayout->addWidget (w);
	groupBox->setVisible(isoLinesSelectable);

	// 4ème rangée : surfacique :
	groupBox			= new QtMgx3DGroupBox ("Surfaces", this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	_repSurfacesCheckBox	= new QCheckBox ("Afficher", 0);
	hlayout->addWidget (_repSurfacesCheckBox);
	_repSurfacesCheckBox->setTristate (
								UNDETERMINED == repSurfaces ? true : false);
	_repSurfacesCheckBox->setCheckState (threeStateToCheckState (repSurfaces));
	w	= createColorWidgets (
				_surfacesColorWidget, _surfacesColorButton, surfacesColor,
				surfacesColorOK);
	hlayout->addWidget (w);
	w->setEnabled (!globalMode);	// Mode global => couleurs non modifiables
	w	= createOpacityWidget (_surfacesOpacityTextField, surfacesOpacity);
	hlayout->addWidget (w);
	groupBox->setVisible(surfacesSelectable);

	// 5ème rangée : volumique :
	groupBox			= new QtMgx3DGroupBox ("Volumes", this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	_repVolumesCheckBox    = new QCheckBox ("Afficher", 0);
	hlayout->addWidget (_repVolumesCheckBox, row, 0);   row++;
	_repVolumesCheckBox->setTristate (
                                UNDETERMINED == repVolumes ? true : false);
	_repVolumesCheckBox->setCheckState (threeStateToCheckState (repVolumes));
	w	= createColorWidgets (
		_volumesColorWidget, _volumesColorButton, volumesColor, volumesColorOK);
	hlayout->addWidget (w);
	w->setEnabled (!globalMode);	// Mode global => couleurs non modifiables
	w	= createOpacityWidget (_volumesOpacityTextField, volumesOpacity);
	hlayout->addWidget (w);
	groupBox->setVisible(volumesSelectable);

	// Ligne suivante : facteur de rétrécissement des entités :
	groupBox			= new QtMgx3DGroupBox (QString::fromUtf8("Propriétés communes"), this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);	
	groupBox->setAreaMargin (0);
	groupBox->setAreaSpacing (0);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	w	= createShrinkWidget (_shrinkFactorTextField, shrinkFactor);
	w->setContentsMargins (0, 0, 0, 0);
	hlayout->addWidget (w);
	hlayout->addStretch (100);
	groupBox->setVisible (true);

	// 6ème ligne : associations topo->geom
	groupBox			= new QtMgx3DGroupBox ("Associations", this);
	hlayout				= new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);	row++;
	_repAssociationCheckBox    = new QCheckBox ("Afficher", 0);
	hlayout->addWidget (_repAssociationCheckBox, row, 0);	//   row++;
	_repAssociationCheckBox->setTristate (
                                UNDETERMINED == repAssociations ? true : false);
	_repAssociationCheckBox->setCheckState (threeStateToCheckState (repAssociations));
	_topoColorCheckBox    = new QCheckBox (QString::fromUtf8("Couleur géométrie"), 0);
	_topoColorCheckBox->setToolTip (QString::fromUtf8("Coché les entités topologiques seront de la couleurs des entités géométriques associées."));
	hlayout->addWidget (_topoColorCheckBox, row, 0);   row++;
	_topoColorCheckBox->setTristate (UNDETERMINED == topoColor ? true : false);
	_topoColorCheckBox->setCheckState (threeStateToCheckState (topoColor));
	w	= createArrowComulWidget (_arrowComulTextField, true == arrowComulOK ? arrowComul : 1., 0.1, 10.);
	hlayout->addWidget (w, row, 0);   row++;
	groupBox->setVisible(associationsSelectable);

	// 7ème ligne : projection des contours topologiques
	groupBox            = new QtMgx3DGroupBox (QString::fromUtf8("Topologie projetée"), this);
	hlayout             = new QHBoxLayout ( );
	groupBox->setAreaLayout (hlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);   row++;
	_repMeshShapeCheckBox    = new QCheckBox ("Afficher", 0);
	hlayout->addWidget (_repMeshShapeCheckBox, row, 0);   row++;
	_repMeshShapeCheckBox->setTristate (UNDETERMINED == repMeshShape ? true : false);
    	_repMeshShapeCheckBox->setCheckState (
                                threeStateToCheckState (repMeshShape));
    	groupBox->setVisible(meshShapeSelectable);

    	// 8ème ligne : affichage du type de discrétisation
    	groupBox            = new QtMgx3DGroupBox (QString::fromUtf8("Type de discrétisation"), this);
    	hlayout             = new QHBoxLayout ( );
    	groupBox->setAreaLayout (hlayout);
    	groupBox->setAreaMargin (margin);
    	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
    	_gridLayout->addWidget (groupBox, row, 0);   row++;
    	_repDiscretisationCheckBox    = new QCheckBox ("Afficher", 0);
    	hlayout->addWidget (_repDiscretisationCheckBox, row, 0);   row++;
    	_repDiscretisationCheckBox->setTristate (
                                UNDETERMINED == repDiscretisationType ? true : false);
	_repDiscretisationCheckBox->setCheckState (
                                threeStateToCheckState (repDiscretisationType));
    	groupBox->setVisible(discRepSelectable);

    	// 9ème ligne : affichage du nombre de bras de la discrétisation
    	groupBox            = new QtMgx3DGroupBox (QString::fromUtf8("Nombre de bras de la discrétisation"), this);
    	hlayout             = new QHBoxLayout ( );
    	groupBox->setAreaLayout (hlayout);
    	groupBox->setAreaMargin (margin);
    	groupBox->setAreaSpacing (spacing);
    	groupBox->setContentsMargins (0, 0, 0, 0);
    	_gridLayout->addWidget (groupBox, row, 0);   row++;
    	_repNbMeshCheckBox    = new QCheckBox ("Afficher", 0);
    	hlayout->addWidget (_repNbMeshCheckBox, row, 0);   row++;
    	_repNbMeshCheckBox->setTristate (UNDETERMINED == repNbMesh ? true : false);
    	_repNbMeshCheckBox->setCheckState (threeStateToCheckState (repNbMesh));
    	groupBox->setVisible(nbMeshSelectable);

    	// 10ème ligne : affichage du nom
    	groupBox            = new QtMgx3DGroupBox ("Nom", this);
    	hlayout             = new QHBoxLayout ( );
    	groupBox->setAreaLayout (hlayout);
    	groupBox->setAreaMargin (margin);
    	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
    	_gridLayout->addWidget (groupBox, row, 0);   row++;
    	_repNamesCheckBox    = new QCheckBox ("Afficher", 0);
    	hlayout->addWidget (_repNamesCheckBox, row, 0);   row++;
    	_repNamesCheckBox->setTristate (UNDETERMINED == repNames ? true : false);
    	_repNamesCheckBox->setCheckState (threeStateToCheckState (repNames));
    	groupBox->setVisible(namesSelectable);

	// 11-ème rangée : fonte utilisée. Dans une classe dérivée,
	// spécialisée (VTK).

	// 12-ème rangée : valeur (éventuellement) affichée (aux noeuds/mailles) :
	groupBox					= new QtMgx3DGroupBox (QString::fromUtf8("Valeur représentée"), this);
	QVBoxLayout*	vlayout		= new QVBoxLayout ( );
	groupBox->setAreaLayout (vlayout);
	groupBox->setAreaMargin (margin);
	groupBox->setAreaSpacing (spacing);
	groupBox->setContentsMargins (0, 0, 0, 0);
	_gridLayout->addWidget (groupBox, row, 0);   row++;
	hlayout		= new QHBoxLayout ( );
	vlayout->addLayout (hlayout);
	label	=	new QLabel ("Type :", 0);
	hlayout->addWidget (label);
	_valueTypeComboBox	= new QComboBox (0);
	_valueTypeComboBox->addItem ("Aucun", QtRepresentationTypesPanel::NO_VALUE);
	_valueTypeComboBox->addItem (
				"Valeurs aux noeuds", QtRepresentationTypesPanel::NODE_VALUE);
	_valueTypeComboBox->addItem (
				"Valeurs aux mailles", QtRepresentationTypesPanel::CELL_VALUE);
	connect (_valueTypeComboBox, SIGNAL (currentIndexChanged (int)), this,
	         SLOT (valueTypeCallback ( )));
	hlayout->addWidget (_valueTypeComboBox);
	{
		QtObjectSignalBlocker	blocker (_valueTypeComboBox);
		int	index	= _valueTypeComboBox->findData (displayedValue);
		_valueTypeComboBox->setCurrentIndex (index);
	}
	hlayout		= new QHBoxLayout ( );
	vlayout->addLayout (hlayout);
	label	=   new QLabel ("Nom  :", 0);
	hlayout->addWidget (label);
	_valueNameComboBox	= new QComboBox (0);
	hlayout->addWidget (_valueNameComboBox);
	valueTypeCallback ( );
	{
		QtObjectSignalBlocker	blocker (_valueNameComboBox);
		int	index	= _valueNameComboBox->findText (displayedValueName.c_str ( ));
		_valueNameComboBox->setCurrentIndex (index);
	}
	groupBox->setVisible (displayValue);
	if ((0 == nodesValuesNames.size ( )) && (0 == cellsValuesNames.size ( )))
		groupBox->setVisible (false);

	const QSize	size	= sizeHint ( );
	QMargins	m	= contentsMargins ( );
	QSize	s (size.width ( ) + m.left ( ) + m.right ( ),
		   size.height ( ) + m.bottom ( ) + m.top ( ));
	setMinimumSize (s);
}	// QtRepresentationTypesPanel::QtRepresentationTypesPanel


QtRepresentationTypesPanel::QtRepresentationTypesPanel (const QtRepresentationTypesPanel&)
	: QtMgx3DGroupBox ("", 0),
	  _nodesValuesNames ( ), _cellsValuesNames ( ),
	  _repCloudCheckBox (0), _repCurvesCheckBox (0), _repIsoCurvesCheckBox (0),
	  _repSurfacesCheckBox (0), _repVolumesCheckBox(0), _repNamesCheckBox (0),
	  _repAssociationCheckBox (0), _repMeshShapeCheckBox(0),
	  _repDiscretisationCheckBox(0), _repNbMeshCheckBox (0),
	  _cloudColorWidget (0), _curvesColorWidget (0), _isoCurvesColorWidget (0),
	  _surfacesColorWidget (0), _volumesColorWidget (0),
	  _cloudColorButton (0), _curvesColorButton (0), _isoCurvesColorButton (0),
	  _surfacesColorButton (0), _volumesColorButton (0),
	  _cloudOpacityTextField (0), _curvesOpacityTextField (0),
	  _isoCurvesOpacityTextField (0), _surfacesOpacityTextField (0),
	  _volumesOpacityTextField (0),
	  _shrinkFactorTextField (0), _arrowComulTextField (0),
	  _valueTypeComboBox (0), _valueNameComboBox (0),
	  _fontPanel (0), _gridLayout (0)
{
	MGX_FORBIDDEN ("QtRepresentationTypesPanel copy constructor is not allowed.");
}	// QtRepresentationTypesPanel::QtRepresentationTypesPanel (const QtRepresentationTypesPanel&)


QtRepresentationTypesPanel& QtRepresentationTypesPanel::operator = (
											const QtRepresentationTypesPanel&)
{
	MGX_FORBIDDEN ("QtRepresentationTypesPanel assignment operator is not allowed.");
	return *this;
}	// QtRepresentationTypesPanel::QtRepresentationTypesPanel (const QtRepresentationTypesPanel&)


QtRepresentationTypesPanel::~QtRepresentationTypesPanel ( )
{
}	// QtRepresentationTypesPanel::~QtRepresentationTypesPanel


bool QtRepresentationTypesPanel::colorModified ( ) const
{
	if ((0 == _cloudColorWidget)     || (0 == _curvesColorWidget) ||
	    (0 == _isoCurvesColorWidget) || (0 == _surfacesColorWidget) ||
	    (0 == _volumesColorWidget))
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::colorModified", "Widget non instancié.")
		throw exc;
	}	// if ((0 == _repCloudCheckBox) ...

	return (true == _cloudColorWidget->isModified ( ))     ||
	       (true == _curvesColorWidget->isModified ( ))    ||
	       (true == _isoCurvesColorWidget->isModified ( )) ||
	       (true == _surfacesColorWidget->isModified ( ))  ||
	       (true == _volumesColorWidget->isModified ( ))    ? true : false;
}	// QtRepresentationTypesPanel::colorModified


QtRepresentationTypesPanel::ExtendedDisplayProperties
					QtRepresentationTypesPanel::getDisplayProperties ( ) const
{
	QtRepresentationTypesPanel::ExtendedDisplayProperties	properties;

	if (true == useCloudColor ( ))
		properties.setCloudColor (getCloudColor ( ));
	else
		properties.useCloudColor (false);
	if (true == useCurvesColor ( ))
		properties.setWireColor (getCurvesColor ( ));
	else
		properties.useWireColor (false);
	if (true == useSurfacesColor ( ))
		properties.setSurfacicColor (getSurfacesColor ( ));
	else
		properties.useSurfacicColor (false);
	if ((true == useVolumesColor ( )) && (ON == useVolumesRepresentation ( )))
		properties.setVolumicColor (getVolumesColor ( ));
	else
		properties.useVolumicColor (false);
	if (true == useCloudOpacity ( ))
		properties.setCloudOpacity (getCloudOpacity ( ));
	else
		properties.useCloudOpacity (false);
	if (true == useCurvesOpacity ( ))
		properties.setWireOpacity (getCurvesOpacity ( ));
	else
		properties.useWireOpacity (false);
	if (true == useSurfacesOpacity ( ))
		properties.setSurfacicOpacity (getSurfacesOpacity ( ));
	else
		properties.useSurfacicOpacity (false);
	if (true == useVolumesOpacity ( ))
		properties.setVolumicOpacity (getVolumesOpacity ( ));
	else
		properties.useVolumicOpacity (false);
	if (true == useShrinkFactor ( ))
		properties.setShrinkFactor (getShrinkFactor ( ));
	else
		properties.useShrinkFactor (false);
	if (true == useArrowComul ( ))
		properties.setArrowComul (getArrowComul ( ));
	else
		properties.useArrowComul (false);
	int			fontFamily  = -1,	fontSize	= -1;
	bool		colorModified = false;
	THREESTATES	bold		= OFF,	italic		= OFF;
	Color		fontColor (0, 0, 0), eFontColor (0, 0, 0);
	getFontProperties (fontFamily, fontSize, bold, italic, fontColor, colorModified);
	fontColor	= true == colorModified ? fontColor : eFontColor;
	properties.setFontProperties (
						fontFamily, fontSize, bold, italic, fontColor);
	properties.setValueName (getDisplayedValueName ( ));

	return properties;
}	// QtRepresentationTypesPanel::getDisplayProperties


THREESTATES QtRepresentationTypesPanel::useCloudRepresentation ( ) const
{
	if (0 == _repCloudCheckBox)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useCloudRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _repCloudCheckBox)

	return checkStateToThreeState (_repCloudCheckBox->checkState ( ));
}	// QtRepresentationTypesPanel::useCloudRepresentation


bool QtRepresentationTypesPanel::cloudRepresentationModified ( ) const
{
	if ((0 == _repCloudCheckBox) || (0 == _cloudColorWidget) ||
	    (0 == _cloudOpacityTextField))
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::cloudRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if ((0 == _repCloudCheckBox) ...

	return (true == _repCloudCheckBox->checkState ( )) &&
	       ((true == _cloudColorWidget->isModified ( )) ||
	        (true == _cloudOpacityTextField->isModified ( ))) ?
		true : false;
}	// QtRepresentationTypesPanel::cloudRepresentationModified


bool QtRepresentationTypesPanel::useCloudColor ( ) const
{
	if (0 == _cloudColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useCloudColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _cloudColorWidget)

	return _cloudColorWidget->useColor ( );
}	// QtRepresentationTypesPanel::useCloudColor


Color QtRepresentationTypesPanel::getCloudColor ( ) const
{
	if (0 == _cloudColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getCloudColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _cloudColorWidget)

	return _cloudColorWidget->getColor ( );
}	// QtRepresentationTypesPanel::getCloudColor


bool QtRepresentationTypesPanel::useCloudOpacity ( ) const
{
	if (0 == _cloudOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useCloudOpacity", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _cloudOpacityTextField)

	return _cloudOpacityTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useCloudOpacity


double QtRepresentationTypesPanel::getCloudOpacity ( ) const
{
	
	if (0 == _cloudOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getCloudOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _cloudOpacityTextField)

	return _cloudOpacityTextField->getOpacity ( );
}	// QtRepresentationTypesPanel::getCloudOpacity


THREESTATES QtRepresentationTypesPanel::useCurvesRepresentation ( ) const
{
	if (0 == _repCurvesCheckBox)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useCurvesRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _repCurvesCheckBox)

	return checkStateToThreeState (_repCurvesCheckBox->checkState ( ));
}	// QtRepresentationTypesPanel::useCurvesRepresentation


bool QtRepresentationTypesPanel::curvesRepresentationModified ( ) const
{
	if ((0 == _repCurvesCheckBox) || (0 == _curvesColorWidget) ||
	    (0 == _curvesOpacityTextField))
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::curvesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if ((0 == _repCurvesCheckBox) ...

	return (true == _repCurvesCheckBox->checkState ( )) &&
	       ((true == _curvesColorWidget->isModified ( )) ||
	        (true == _curvesOpacityTextField->isModified ( ))) ?
		true : false;
}	// QtRepresentationTypesPanel::curvesRepresentationModified


bool QtRepresentationTypesPanel::useCurvesColor ( ) const
{
	if (0 == _curvesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useCurvesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _curvesColorWidget)

	return _curvesColorWidget->useColor ( );
}	// QtRepresentationTypesPanel::useCurvesColor


Color QtRepresentationTypesPanel::getCurvesColor ( ) const
{
	if (0 == _curvesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getCurvesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _curvesColorWidget)

	return _curvesColorWidget->getColor ( );
}	// QtRepresentationTypesPanel::getCurvesColor


bool QtRepresentationTypesPanel::useCurvesOpacity ( ) const
{
	if (0 == _curvesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useCurvesOpacity", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _curvesOpacityTextField)

	return _curvesOpacityTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useCurvesOpacity


double QtRepresentationTypesPanel::getCurvesOpacity ( ) const
{
	
	if (0 == _curvesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getCurvesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _curvesOpacityTextField)

	return _curvesOpacityTextField->getOpacity ( );
}	// QtRepresentationTypesPanel::getCurvesOpacity


THREESTATES QtRepresentationTypesPanel::useIsoCurvesRepresentation ( ) const
{
	if (0 == _repIsoCurvesCheckBox)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useIsoCurvesRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _repIsoCurvesCheckBox)

	return checkStateToThreeState (_repIsoCurvesCheckBox->checkState ( ));
}	// QtRepresentationTypesPanel::useIsoCurvesRepresentation


bool QtRepresentationTypesPanel::isoCurvesRepresentationModified ( ) const
{
	if ((0 == _repIsoCurvesCheckBox) || (0 == _isoCurvesColorWidget) ||
	    (0 == _isoCurvesOpacityTextField))
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::isoCurvesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if ((0 == _repCloudCheckBox) ...

	return (true == _repIsoCurvesCheckBox->checkState ( )) &&
	       ((true == _isoCurvesColorWidget->isModified ( )) ||
	        (true == _isoCurvesOpacityTextField->isModified ( ))) ?
		true : false;
}	// QtRepresentationTypesPanel::isoCurvesRepresentationModified


bool QtRepresentationTypesPanel::useIsoCurvesColor ( ) const
{
	if (0 == _isoCurvesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useIsoCurvesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _isoCurvesColorWidget)

	return _isoCurvesColorWidget->useColor ( );
}	// QtRepresentationTypesPanel::useIsoCurvesColor


Color QtRepresentationTypesPanel::getIsoCurvesColor ( ) const
{
	if (0 == _isoCurvesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getIsoCurvesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _isoCurvesColorWidget)

	return _isoCurvesColorWidget->getColor ( );
}	// QtRepresentationTypesPanel::getIsoCurvesColor


bool QtRepresentationTypesPanel::useIsoCurvesOpacity ( ) const
{
	if (0 == _isoCurvesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useIsoCurvesOpacity", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _isoCurvesOpacityTextField)

	return _isoCurvesOpacityTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useIsoCurvesOpacity


double QtRepresentationTypesPanel::getIsoCurvesOpacity ( ) const
{
	
	if (0 == _isoCurvesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getIsoCurvesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _isoCurvesOpacityTextField)

	return _isoCurvesOpacityTextField->getOpacity ( );
}	// QtRepresentationTypesPanel::getIsoCurvesOpacity


THREESTATES QtRepresentationTypesPanel::useSurfacesRepresentation ( ) const
{
	if (0 == _repSurfacesCheckBox)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useSurfacesRepresentation", "Widget non instancié.")
		throw exc;
	}	// if (0 == _repSurfacesCheckBox)

	return checkStateToThreeState (_repSurfacesCheckBox->checkState ( ));
}	// QtRepresentationTypesPanel::useSurfacesRepresentation


bool QtRepresentationTypesPanel::surfacesRepresentationModified ( ) const
{
	if ((0 == _repSurfacesCheckBox) || (0 == _surfacesColorWidget) ||
	    (0 == _surfacesOpacityTextField))
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::surfacesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if ((0 == _repSurfacesCheckBox) ...

	return (true == _repSurfacesCheckBox->checkState ( )) &&
	       ((true == _surfacesColorWidget->isModified ( )) ||
	        (true == _surfacesOpacityTextField->isModified ( ))) ?
		true : false;
}	// QtRepresentationTypesPanel::surfacesRepresentationModified


bool QtRepresentationTypesPanel::useSurfacesColor ( ) const
{
	if (0 == _surfacesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useSurfacesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _surfacesColorWidget)

	return _surfacesColorWidget->useColor ( );
}	// QtRepresentationTypesPanel::useSurfacesColor


Color QtRepresentationTypesPanel::getSurfacesColor ( ) const
{
	if (0 == _surfacesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getSurfacesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _surfacesColorWidget)

	return _surfacesColorWidget->getColor ( );
}	// QtRepresentationTypesPanel::getSurfacesColor


bool QtRepresentationTypesPanel::useSurfacesOpacity ( ) const
{
	if (0 == _surfacesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useSurfacesOpacity", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _surfacesOpacityTextField)

	return _surfacesOpacityTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useSurfacesOpacity


double QtRepresentationTypesPanel::getSurfacesOpacity ( ) const
{
	
	if (0 == _surfacesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getSurfacesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _surfacesOpacityTextField)

	return _surfacesOpacityTextField->getOpacity ( );
}	// QtRepresentationTypesPanel::getSurfacesOpacity


THREESTATES QtRepresentationTypesPanel::useVolumesRepresentation ( ) const
{
    if (0 == _repVolumesCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useVolumesRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _repVolumesCheckBox)

    return checkStateToThreeState (_repVolumesCheckBox->checkState ( ));
}   // QtRepresentationTypesPanel::useVolumesRepresentation


bool QtRepresentationTypesPanel::volumesRepresentationModified ( ) const
{
	if ((0 == _repVolumesCheckBox) || (0 == _volumesColorWidget) ||
	    (0 == _volumesOpacityTextField))
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::volumesRepresentationModified", "Widget non instancié.")
		throw exc;
	}	// if ((0 == _repVolumesCheckBox) ...

	return (true == _repVolumesCheckBox->checkState ( )) &&
	       ((true == _volumesColorWidget->isModified ( )) ||
	        (true == _volumesOpacityTextField->isModified ( ))) ?
		true : false;
}	// QtRepresentationTypesPanel::volumesRepresentationModified


bool QtRepresentationTypesPanel::useVolumesColor ( ) const
{
	if (0 == _volumesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useVolumesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _volumesColorWidget)

	return _volumesColorWidget->useColor ( );
}	// QtRepresentationTypesPanel::useVolumesColor


Color QtRepresentationTypesPanel::getVolumesColor ( ) const
{
	if (0 == _volumesColorWidget)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getVolumesColor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _volumesColorWidget)

	return _volumesColorWidget->getColor ( );
}	// QtRepresentationTypesPanel::getVolumesColor


bool QtRepresentationTypesPanel::useVolumesOpacity ( ) const
{
	if (0 == _volumesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useVolumesOpacity", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _volumesOpacityTextField)

	return _volumesOpacityTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useVolumesOpacity


double QtRepresentationTypesPanel::getVolumesOpacity ( ) const
{
	
	if (0 == _volumesOpacityTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getVolumesOpacity", "Widget non instancié.")
		throw exc;
	}	// if (0 == _volumesOpacityTextField)

	return _volumesOpacityTextField->getOpacity ( );
}	// QtRepresentationTypesPanel::getVolumesOpacity


THREESTATES QtRepresentationTypesPanel::useNamesRepresentation ( ) const
{
    if (0 == _repNamesCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useNamesRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _repNamesCheckBox)

    return checkStateToThreeState (_repNamesCheckBox->checkState ( ));
}   // QtRepresentationTypesPanel::useNamesRepresentation


bool QtRepresentationTypesPanel::useShrinkFactor ( ) const
{
	if (0 == _shrinkFactorTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useShrinkFactor", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _shrinkFactorTextField)

	return _shrinkFactorTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useShrinkFactor


double QtRepresentationTypesPanel::getShrinkFactor ( ) const
{
	
	if (0 == _shrinkFactorTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getShrinkFactor", "Widget non instancié.")
		throw exc;
	}	// if (0 == _shrinkFactorTextField)

	return _shrinkFactorTextField->getOpacity ( );
}	// QtRepresentationTypesPanel::getShrinkFactor


void QtRepresentationTypesPanel::getFontProperties (
		int& family, int& size, THREESTATES& bold, THREESTATES& italic,
		Color& color, bool& colorModified) const
{
	if (0 == _fontPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::getFontProperties", "Absence de panneau de saisie de la fonte.")
		throw exc;
	}	// if (0 == _fontPanel)

	_fontPanel->getFontProperties (family, size, bold, italic, color, colorModified);
}	// QtRepresentationTypesPanel::getFontProperties


QtRepresentationTypesPanel::DISPLAYED_VALUE
					QtRepresentationTypesPanel::getDisplayedValueType ( ) const
{
	CHECK_NULL_PTR_ERROR (_valueTypeComboBox)

	return (QtRepresentationTypesPanel::DISPLAYED_VALUE)
		_valueTypeComboBox->itemData (_valueTypeComboBox->currentIndex (
																)).toInt (0);
}	// QtRepresentationTypesPanel::getDisplayedValueType


string QtRepresentationTypesPanel::getDisplayedValueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_valueNameComboBox)

	return _valueNameComboBox->currentText ( ).toStdString ( );
}	// QtRepresentationTypesPanel::getDisplayedValueName


THREESTATES
	QtRepresentationTypesPanel::useAssociationsRepresentation ( ) const
{
    if (0 == _repAssociationCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useAssociationsRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _repAssociationCheckBox)

    return checkStateToThreeState (_repAssociationCheckBox->checkState ( ));
}   // QtRepresentationTypesPanel::useAssociationsRepresentation


bool QtRepresentationTypesPanel::useArrowComul ( ) const
{
	if (0 == _arrowComulTextField)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useArrowComul", "Widget non instancié.") 
		throw exc;
	}	// if (0 == _arrowComulTextField)

	return _arrowComulTextField->useOpacity ( );
}	// QtRepresentationTypesPanel::useArrowComul


double QtRepresentationTypesPanel::getArrowComul ( ) const
{
	if (0 == _arrowComulTextField)
	{
	INTERNAL_ERROR (exc, "QtRepresentationTypesPane::getArrowComul", "Widget non instancié.")
	throw exc;
	}	// if (0 == _arrowComulTextField)

	return _arrowComulTextField->getValue ( );
}	// QtRepresentationTypesPanel::getArrowComul


THREESTATES
	QtRepresentationTypesPanel::topoUseGeomColor ( ) const
{
    if (0 == _topoColorCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPane::topoUseGeomColor", "Widget non instancié.")
        throw exc;
    }   // if (0 == _topoColorCheckBox)

    return checkStateToThreeState (_topoColorCheckBox->checkState ( ));
}   // QtRepresentationTypesPanetopoUseGeomColor


THREESTATES
    QtRepresentationTypesPanel::useMeshShapeRepresentation ( ) const
{
    if (0 == _repMeshShapeCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useMeshShapeRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _repMeshShapeCheckBox)

    return checkStateToThreeState (_repMeshShapeCheckBox->checkState ( ));
}   // QtRepresentationTypesPanel::useMeshShapeRepresentation


THREESTATES
    QtRepresentationTypesPanel::useDiscretisationRepresentation ( ) const
{
    if (0 == _repDiscretisationCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useDiscretisationRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _repDiscretisationCheckBox)

    return checkStateToThreeState (_repDiscretisationCheckBox->checkState ( ));
}   // QtRepresentationTypesPanel::useDiscretisationRepresentation

THREESTATES
    QtRepresentationTypesPanel::useNbMeshRepresentation ( ) const
{
    if (0 == _repNbMeshCheckBox)
    {
        INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::useNbMeshRepresentation", "Widget non instancié.")
        throw exc;
    }   // if (0 == _repNbMeshCheckBox)

    return checkStateToThreeState (_repNbMeshCheckBox->checkState ( ));
}   // QtRepresentationTypesPanel::useNbMeshRepresentation


void QtRepresentationTypesPanel::valueTypeCallback ( )
{	// Callback => no throw
	try
	{
		CHECK_NULL_PTR_ERROR (_valueNameComboBox)
		_valueNameComboBox->clear ( );

		switch (getDisplayedValueType ( ))
		{
			case QtRepresentationTypesPanel::NODE_VALUE	:
			{
				for (set<string>::const_iterator it =
					_nodesValuesNames.begin( ); _nodesValuesNames.end( ) != it;
					it++)
					_valueNameComboBox->addItem ((*it).c_str ( ));
			}
			break;
			case QtRepresentationTypesPanel::CELL_VALUE	:
			{
				for (set<string>::const_iterator it =
					_cellsValuesNames.begin( ); _cellsValuesNames.end( ) != it;
					it++)
					_valueNameComboBox->addItem ((*it).c_str ( ));
			}
			break;
		}	// switch (getDisplayedValueType ( ))
	}

	catch (const Exception& exc)
	{
		cerr << "Unexpected exception caught in QtRepresentationTypesPanel::valueTypeCallback : " << exc.getFullMessage ( ) << endl;
	}
	catch (...)
	{
		cerr << "Unexpected exception caught in QtRepresentationTypesPanel::valueTypeCallback." << endl;
	}
}	// QtRepresentationTypesPanel::valueTypeCallback


void QtRepresentationTypesPanel::setFontPropertiesPanel (
			QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel& panel)
{
	if (0 != _fontPanel)
	{
		INTERNAL_ERROR (exc, "QtRepresentationTypesPanel::setFontPropertiesPanel", "Panneau de saisie de la fonte déjà affecté.")
		throw exc;
	}	// if (0 != _fontPanel)

	_fontPanel	= &panel;
//	QGridLayout*	gridlayout	= dynamic_cast<QGridLayout*>(layout ( ));
	CHECK_NULL_PTR_ERROR (_gridLayout)
	_gridLayout->addWidget (
		_fontPanel, _gridLayout->rowCount ( ), 0, 1, _gridLayout->columnCount( ));
}	// QtRepresentationTypesPanel::setFontPropertiesPanel


}	// namespace QtComponents

}	// namespace Mgx3D
