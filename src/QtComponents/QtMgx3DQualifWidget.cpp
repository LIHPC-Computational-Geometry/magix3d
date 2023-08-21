/**
 * \file		QtMgx3DQualifWidget.cpp
 * \author		Charles PIGNEROL
 * \date		02/08/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DQualifWidget.h"

#include "Utils/Common.h"
#include "Mesh/Mgx3DQualifSerie.h"
#include "Mesh/Surface.h"
#include "Mesh/Volume.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QwtCharts/QwtChartsManager.h>


using namespace GQualif;
using namespace gmds;
using namespace Mgx3D;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Utils;
using namespace TkUtil;
using namespace std;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                         Classe QtMgx3DQualifWidget
// ===========================================================================


QtMgx3DQualifWidget::QtMgx3DQualifWidget (QWidget* parent, Context* context)
	: QtQualifWidget (parent, "Magix 3D"), _context (context),
	  _popupMenu (0)
{
	_popupMenu	= new QMenu (this);
	if (0 != QwtChartsManager::getEditChartAction ( ))
		_popupMenu->addAction (QwtChartsManager::getEditChartAction ( ));

	setContextMenuPolicy (Qt::CustomContextMenu);
	connect (this, SIGNAL(customContextMenuRequested(const QPoint&)),
	         this, SLOT (displayPopupMenu (const QPoint&)));
	setDomainType (QtQualifWidget::COMPUTED_DOMAIN);
}	// QtMgx3DQualifWidget::QtMgx3DQualifWidget


QtMgx3DQualifWidget::QtMgx3DQualifWidget (const QtMgx3DQualifWidget&)
	: QtQualifWidget (0, "Magix 3D"), _context (0),
	  _popupMenu (0)
{
	MGX_FORBIDDEN ("QtMgx3DQualifWidget copy constructor is not allowed.")
}	// QtMgx3DQualifWidget::QtMgx3DQualifWidget


QtMgx3DQualifWidget& QtMgx3DQualifWidget::operator = (
													const QtMgx3DQualifWidget&)
{
	MGX_FORBIDDEN ("QtMgx3DQualifWidget assignment operator is not allowed.")
	return *this;
}	// QtMgx3DQualifWidget::operator =


QtMgx3DQualifWidget::~QtMgx3DQualifWidget ( )
{
}	// QtMgx3DQualifWidget::~QtMgx3DQualifWidget


void QtMgx3DQualifWidget::updateHistogram ( )
{
	QtQualifWidget::updateHistogram ( );
}	// QtMgx3DQualifWidget::updateHistogram


vector< gmds::IGMesh::surface*>
							QtMgx3DQualifWidget::getSelectedClassesSurfaces ( )
{
	vector<gmds::IGMesh::surface*>	surfaces;

	const QwtExtendedMultiBarChart&	histogram	=
										getHistogramPanel ( ).getHistogram ( );
	vector< pair<size_t, size_t> >	selection	= histogram.getSelection ( );
	Mesh::MeshItf*					mesh		=
							getContext ( ).getMeshManager ( ).getMesh ( );
	CHECK_NULL_PTR_ERROR (mesh)
	for (vector< pair<size_t, size_t> >::iterator its = selection.begin ( );
	     selection.end ( ) != its; its++)
	{
		const Mesh::Mgx3DQualifSerie*	serie	=
			dynamic_cast<const Mesh::Mgx3DQualifSerie*>(&getSerie ((*its).first));
		CHECK_NULL_PTR_ERROR (serie)
		if (true == serie->isVolumic ( ))
			continue;
		vector <gmds::TCellID>	cellsIds;
		serie->getGMDSCellsIndexes (cellsIds, (*its).second);
		if (0 == cellsIds.size ( ))
			continue;

		UTF8String	name (Charset::UTF_8);
		name << serie->getName ( ) << "_Class_" << (*its).second;
		gmds::IGMesh::surface&	surface	=
								mesh->getGMDSMesh ( ).newSurface (name.iso ( ));
		surfaces.push_back (&surface);

		for (vector <gmds::TCellID>::const_iterator itc = cellsIds.begin ( );
		     cellsIds.end ( ) != itc; itc++)
		{
			surface.add (serie->getGMDSFace (*itc));
		}	// for (vector <gmds::TCellID>::const_iterator itc = ...
	}	// for (vector< pair<size_t, size_t> >::iterator its = ...

	return surfaces;
}	// QtMgx3DQualifWidget::getSelectedClassesSurfaces


vector< gmds::IGMesh::volume*>
							QtMgx3DQualifWidget::getSelectedClassesVolumes ( )
{
	vector<gmds::IGMesh::volume*>	volumes;

	const QwtExtendedMultiBarChart&	histogram	=
										getHistogramPanel ( ).getHistogram ( );
	vector< pair<size_t, size_t> >	selection	= histogram.getSelection ( );
	Mesh::MeshItf*					mesh		=
							getContext ( ).getMeshManager ( ).getMesh ( );
	CHECK_NULL_PTR_ERROR (mesh)
	for (vector< pair<size_t, size_t> >::iterator its = selection.begin ( );
	     selection.end ( ) != its; its++)
	{
		const Mesh::Mgx3DQualifSerie*	serie	=
			dynamic_cast<const Mesh::Mgx3DQualifSerie*>(&getSerie ((*its).first));
		CHECK_NULL_PTR_ERROR (serie)

		if (false == serie->isVolumic ( ))
			continue;
		vector <gmds::TCellID>	cellsIds;
		serie->getGMDSCellsIndexes (cellsIds, (*its).second);
		if (0 == cellsIds.size ( ))
			continue;

		UTF8String	name (Charset::UTF_8);
		name << serie->getName ( )
		     << "_Class_" << (*its).second;
		gmds::IGMesh::volume&	volume	=
								mesh->getGMDSMesh ( ).newVolume (name.iso ( ));
		volumes.push_back (&volume);

		for (vector <gmds::TCellID>::const_iterator itc = cellsIds.begin ( );
		     cellsIds.end ( ) != itc; itc++)
		{
			volume.add (serie->getGMDSRegion (*itc));
		}	// for (vector <gmds::TCellID>::const_iterator itc = ...
	}	// for (vector< pair<size_t, size_t> >::iterator its = ...

	return volumes;
}	// QtMgx3DQualifWidget::getSelectedClassesVolumes


Context& QtMgx3DQualifWidget::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_context)
	return *_context;
}	// QtMgx3DQualifWidget::getContext


const Context& QtMgx3DQualifWidget::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_context)
	return *_context;
}	// QtMgx3DQualifWidget::getContext


void QtMgx3DQualifWidget::focusInEvent (QFocusEvent*)
{
	QwtChartsManager::getManager ( ).focusChanged (&getHistogramPanel ( ));
}	// QtMgx3DQualifWidget::focusInEvent


void QtMgx3DQualifWidget::displayPopupMenu (const QPoint& point)
{
	if (0 != _popupMenu)
		_popupMenu->popup (mapToGlobal (point));
	QwtChartsManager::getManager ( ).focusChanged (&getHistogramPanel ( ));
}	// QtMgx3DQualifWidget::displayPopupMenu


void QtMgx3DQualifWidget::editGraphicCallback ( )
{
	
}	// QtMgx3DQualifWidget::editGraphicCallback


}	// namespace QtComponents

}	// namespace Mgx3D

