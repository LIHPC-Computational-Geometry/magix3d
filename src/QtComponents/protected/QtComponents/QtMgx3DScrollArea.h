/**
 * \file        QtMgx3DScrollArea.h
 * \author      Charles PIGNEROL
 * \date        12/06/2018
 */

#ifndef Q_MGX3D_SCROLL_AREA_H
#define Q_MGX3D_SCROLL_AREA_H

#include <QScrollArea>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * <P>
 * <I>Scroll area</I> où l'on redéfini <I>sizeHint</I> de manière à éviter
 * d'avoir des ascenseurs. Cette surcharge devrait être inutile, mais ...
 * </P>
 *
 * \warning		<B>ATTENTION :</B> un <I>setFrameStyle (QFrame::NoFrame)</I>
 *			modifie le comportement du widget, qui ne se redimensionne plus
 *			si nécessaire.
 */
class QtMgx3DScrollArea : public QScrollArea
{
	public :

	QtMgx3DScrollArea (QWidget* parent = 0);
	virtual ~QtMgx3DScrollArea ( );

	/**
	 * \param	La taille souhaitée. Prend en compte l'éventuel zoom demandé. 
	 * \param	setZoomFactor
	 */
	virtual QSize sizeHint ( ) const;

	/**
	 * \param	Eventuel facteur d'agrandissement à appliquer à la taille
	 * 		calculée pour obtenir la taille désirée,
	 * \see		sizeHint
	 */
	virtual void setZoomFactor (double horizZoom = 1., double vertZoom = 1.);



	private :

	QtMgx3DScrollArea (const QtMgx3DScrollArea&);
	QtMgx3DScrollArea& operator = (const QtMgx3DScrollArea&);
	double		_hZoom, _vZoom;
};	// class QtMgx3DScrollArea


}	// namespace QtComponents

}	// namespace Magix3D


#endif	// Q_MGX3D_SCROLL_AREA_H
