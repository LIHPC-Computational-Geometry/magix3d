/**
 * \file        Qt3DGraphicalWidget.cpp
 * \author      Charles PIGNEROL
 * \date        30/11/2010
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/Qt3DGraphicalWidget.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"
#include "Internal/Resources.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>
#include <memory>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE Qt3DGraphicalWidget
// ===========================================================================

Qt3DGraphicalWidget::Qt3DGraphicalWidget (QWidget* parent, RenderingManager* renderingManager)
	: QWidget (parent),
	  _renderingManager (renderingManager)
{
	createGui ( );
}	// Qt3DGraphicalWidget::Qt3DGraphicalWidget


Qt3DGraphicalWidget::Qt3DGraphicalWidget (const Qt3DGraphicalWidget&)
	: QWidget (0), _renderingManager (0)
{
	MGX_FORBIDDEN ("Qt3DGraphicalWidget copy constructor is not allowed.");
}	// Qt3DGraphicalWidget::Qt3DGraphicalWidget (const Qt3DGraphicalWidget&)


Qt3DGraphicalWidget& Qt3DGraphicalWidget::operator = (const Qt3DGraphicalWidget&)
{
	MGX_FORBIDDEN ("Qt3DGraphicalWidget assignment operator is not allowed.");
	return *this;
}	// Qt3DGraphicalWidget::Qt3DGraphicalWidget (const Qt3DGraphicalWidget&)


Qt3DGraphicalWidget::~Qt3DGraphicalWidget ( )
{
	delete _renderingManager;			_renderingManager	= 0;
}	// Qt3DGraphicalWidget::~Qt3DGraphicalWidget


void Qt3DGraphicalWidget::writeSettings (QSettings& settings)
{
	settings.beginGroup ("GraphicalWidget");
	settings.setValue ("size", size ( ));
	settings.endGroup ( );
}	// Qt3DGraphicalWidget::writeSettings


void Qt3DGraphicalWidget::readSettings (QSettings& settings)
{
	settings.beginGroup ("GraphicalWidget");
	resize (settings.value ("size", size ( )).toSize ( ));
	settings.endGroup ( );
}	// Qt3DGraphicalWidget::readSettings


RenderingManager& Qt3DGraphicalWidget::getRenderingManager ( )
{
	if (0 == _renderingManager)
		throw Exception (UTF8String ("Qt3DGraphicalWidget::getRenderingManager : absence de gestionnaire de rendu associé.", Charset::UTF_8));

	return *_renderingManager;
}	// Qt3DGraphicalWidget::getRenderingManager


void Qt3DGraphicalWidget::setRenderingManager (RenderingManager* renderingManager)
{
	if (renderingManager == _renderingManager)
		return;

	delete _renderingManager;
	_renderingManager	= renderingManager;
}	// Qt3DGraphicalWidget::setRenderingManager


const QWidget* Qt3DGraphicalWidget::getRenderingWidget ( ) const
{
	return 0;
}	// Qt3DGraphicalWidget::getRenderingWidget


QWidget* Qt3DGraphicalWidget::getRenderingWidget ( )
{
	return 0;
}	// Qt3DGraphicalWidget::getRenderingWidget


void Qt3DGraphicalWidget::updateConfiguration ( )
{
	if (0 != _renderingManager)	
	{
		_renderingManager->setBackground (
					Resources::instance ( )._background.getRed ( ),
					Resources::instance ( )._background.getGreen ( ),
					Resources::instance ( )._background.getBlue ( ));
	}
}	// Qt3DGraphicalWidget::updateConfiguration



void Qt3DGraphicalWidget::createGui ( )
{
}	// Qt3DGraphicalWidget::createGui


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
