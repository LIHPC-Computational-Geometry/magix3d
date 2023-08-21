/**
 * \file        QtMgx3DScrollArea.cpp
 * \author      Charles PIGNEROL
 * \date        12/06/2018
 */

#include "QtComponents/QtMgx3DScrollArea.h"
#include "Utils/Common.h"

#include <QApplication>
#include <QDesktopWidget>

#include <math.h>


namespace Mgx3D
{

namespace QtComponents
{

QtMgx3DScrollArea::QtMgx3DScrollArea (QWidget* parent)
	: QScrollArea (parent), _hZoom (1.), _vZoom (1.)
{
}	// QtMgx3DScrollArea::QtMgx3DScrollArea


QtMgx3DScrollArea::QtMgx3DScrollArea (const QtMgx3DScrollArea&)
	: QScrollArea (0), _hZoom (1.), _vZoom (1.)
{
	MGX_FORBIDDEN ("QtMgx3DScrollArea copy constructor is not allowed.");
}	// QtMgx3DScrollArea::QtMgx3DScrollArea


QtMgx3DScrollArea& QtMgx3DScrollArea::operator = (const QtMgx3DScrollArea&)
{
	MGX_FORBIDDEN ("QtMgx3DScrollArea assignment operator is not allowed.");
	return *this;
}	// QtMgx3DScrollArea::operator =


QtMgx3DScrollArea::~QtMgx3DScrollArea ( )
{
}	// QtMgx3DScrollArea::~QtMgx3DScrollArea


QSize QtMgx3DScrollArea::sizeHint ( ) const
{
	/*
	 * Pb dans QScrollArea::sizeHint Qt 4.7.4 : malgré setWidgetResizable (true)
	 * et un widget parent lui offrant toute la place souhaitée (mis en évidence
	 * par colorisation des widgets) le QScrollArea utilise toujours les
	 * scrollbars verticales et refuse d'occuper plus d'environ 50pc de la
	 * hauteur du panneau scrollé. Aucun pb au niveau horizontal ... A noter que
	 * la taille du panneau scrollé est bonne, c.a.d. qu'on voit tout ses
	 * composants.
	 *
	 * Doc de QScrollArea::widgetResizable :
	 * ...
	 * If this property is set to true, the scroll area will automatically
	 * resize the widget in order to avoid scroll bars where they can be
	 * avoided, or to take advantage of extra space.
	 *
	 * On retourne ici la taille souhaitée, qui est celle du panneau scrollé +
	 * les marges du scrolleur :
	 */
	if (0 != widget ( ))
	{
		QMargins	m	= contentsMargins ( );
		QSize		ws	= widget ( )->sizeHint ( );

		QSize	s (ws.width ( ) + m.left ( ) + m.right ( ),
			   ws.height ( ) + m.bottom ( ) + m.top ( ));
		QSize	zs (_hZoom * s.width ( ), _vZoom * s.height ( ));
		if ((zs.width ( ) <= m.left ( ) + m.right ( )) || 
		    (zs.height ( ) <= m.bottom ( ) + m.top ( )))
			return s;

		QDesktopWidget*	desktop	= QApplication::desktop ( );
		if (0 != desktop)
		{
			const QRect geom    = desktop->availableGeometry (this);
			if ((zs.width ( ) > geom.width ( )) || (zs.height ( ) > geom.height ( )))
				return s;
		}	// if (0 != desktop)

		return zs;
	}

	return QScrollArea::sizeHint ( );
}	// QtMgx3DScrollArea::sizeHint


void QtMgx3DScrollArea::setZoomFactor (double horizZoom, double vertZoom)
{
	_hZoom	= fabs (horizZoom);
	_vZoom	= fabs (vertZoom);
}	// QtMgx3DScrollArea::setZoomFactor


}	// namespace QtComponents

}	// namespace Magix3D

