/**
 * \file		QtVtkGraphicalWidget.h
 * \author		Charles PIGNEROL
 * \date		23/11/2011
 */
#ifndef QT_VTK_GRAPHICAL_WIDGET_H
#define QT_VTK_GRAPHICAL_WIDGET_H


#include "QtVtkComponents/VTKRenderingManager.h"
#include "QtComponents/Qt3DGraphicalWidget.h"

#include <QtVtk/QtVtkGraphicWidget.h>

#include <vector>


namespace Mgx3D 
{

namespace QtVtkComponents
{

/**
 * \brief		Classe de widget graphique 3D reposant sur l'API VTK.
 *
 */
class QtVtkGraphicalWidget : public QtComponents::Qt3DGraphicalWidget
{
	public :

	/**
	 * Appelle createGui.
	 * \param	Widget parent.
	 * \see		createGui
	 */
	QtVtkGraphicalWidget (QWidget* parent, vtkRenderWindow*, QtComponents::RenderingManager* rm);

	/**
	 * RAS.
	 */
	virtual ~QtVtkGraphicalWidget ( );

	/**
	 * La gestion des entités et du rendu en environnement <I>Qt/VTK</I>.
	 */
	//@{

		/**
		 * \return	Une référence sur le gestionnaire d'entités/rendu pour <I>Qt/VTK</I>, s'il y en a une.
		 * \exception	Une exception est levée en l'absence d'un tel gestionnaire.
		 */
		virtual VTKRenderingManager& getVTKRenderingManager ( );

	//@}	// La gestion des entités et du rendu en environnement <I>Qt/VTK</I>.

	/**
	 * Les opérations graphiques.
	 */
	//@{

	/**
	 * \return		Le widget dans lequel sont effectués les rendus graphiques.
	 * \see			getVTKWidget
	 */
	virtual const QWidget* getRenderingWidget ( ) const;
	virtual QWidget* getRenderingWidget ( );

	/**
	 * \return		Le widget <I>Qt/VTK</I>
	 * \see			getRenderingWidget
	 */
	virtual QtVtkGraphicWidget& getVTKWidget ( );

	/**
	 * \param		Le nouveau widget <I>Qt/VTK</I>. Détruit l'éventuel ancien widget <I>Qt/VTK</I>.
	 */
	virtual void setVTKWidget (QtVtkGraphicWidget*);

	//@}	// Les opérations graphiques


	/**
	 * La gestion de la configuration.
	 */
	//@{

	/**
	 * Actualise les préférences utilisateurs utilisées.
	 */
	virtual void updateConfiguration ( );

	//@}	// La gestion de la configuration.


	protected :

	/**
	 * Création de l'IHM (aspects purements Qt).
	 * \param	Eventuel window <I>VTK</I> à utiliser.
	 */
	virtual void createGui (vtkRenderWindow* window);


	private :

	// Opérations interdites :
	QtVtkGraphicalWidget (const QtVtkGraphicalWidget&);
	QtVtkGraphicalWidget& operator = (const QtVtkGraphicalWidget&);

	/** Le widget <I>VTK</I> utilisée pour les affichages 3D. */
	QtVtkGraphicWidget*				_vtkWidget;
};	// class QtVtkGraphicalWidget


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// QT_VTK_GRAPHICAL_WIDGET_H
