/**
 * \file        QtMgx3DGroupBox.h
 * \author      Charles PIGNEROL
 * \date        12/06/2018
 */

#ifndef Q_MGX3D_GROUP_BOX_H
#define Q_MGX3D_GROUP_BOX_H

#include <QWidget>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Panneau nommé (1ère ligne) recevant des composants dans un panneau en seconde ligne.
 * Le gestionnaire de mise en forme (QLayout) de ce panneau en seconde ligne est à
 * fournir via <I>setAreaLayout</I>.
 */
class QtMgx3DGroupBox : public QWidget
{
	public :

	QtMgx3DGroupBox (const QString& title, QWidget* parent, int margin = 0, int spacing = 0);
	virtual ~QtMgx3DGroupBox ( );

	/**
	 * @return	La zone contenant les composants.
	 * \see		setAreaLayout
	 */
	virtual QWidget& widget ( );
	virtual const QWidget& widget ( ) const;

	/**
	 * \param	Gestionnaire de mise en forme des composants ajoutés.
	 */
	virtual void setAreaLayout (QLayout*);
	virtual void setAreaMargin (int margin);
	virtual void setAreaSpacing (int spacing);


	private :

	QtMgx3DGroupBox (const QtMgx3DGroupBox&);
	QtMgx3DGroupBox& operator = (const QtMgx3DGroupBox&);
	QWidget*	_widget;
};	// class QtMgx3DGroupBox


}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// Q_MGX3D_GROUP_BOX_H
