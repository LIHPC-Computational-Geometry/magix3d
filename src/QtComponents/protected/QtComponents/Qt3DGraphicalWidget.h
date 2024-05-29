/**
 * \file		Qt3DGraphicalWidget.h
 * \author		Charles PIGNEROL
 * \date		30/11/2010
 */
#ifndef QT_3D_GRAPHICAL_WIDGET_H
#define QT_3D_GRAPHICAL_WIDGET_H


#include "QtComponents/RenderingManager.h"
#include "Utils/GraphicalEntityRepresentation.h"

#include <QSettings>
#include <QWidget>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * \brief		Classe générique de widget graphique 3D. Cette classe est à
 *				dériver pour spécialiser avec une API 3D (VTK, Paraview).
 * </P>
 */
class Qt3DGraphicalWidget : public QWidget
{
	public :

	/**
	 * Appelle createGui.
	 * \param	Widget parent.
	 * \param	Eventuel gestionnaire de rendu (adopté)
	 * \see		createGui
	 */
	Qt3DGraphicalWidget (QWidget* parent, RenderingManager* renderingManager);

	/**
	 * RAS.
	 */
	virtual ~Qt3DGraphicalWidget ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Enregistre les paramètres d'affichage (taille, position, ...) de cette
	 * fenêtre.
	 * \see		readSettings
	 */
	virtual void writeSettings (QSettings& settings);

	/**
	 * Lit et s'applique les paramètres d'affichage (taille, position, ...) de
	 * cette fenêtre.
	 * \see		writeSettings
	 */
	virtual void readSettings (QSettings& settings);

	//@}	// Divers IHM

	/**
	 * La gestion des entités et du rendu.
	 */
	//@{

	/**
	 * \return		Le gestionnaire de rendu
	 * \exception	Une exception en l'absence d'instance associée
	 */
	virtual RenderingManager& getRenderingManager ( );
	
	/**
	 * \param		L'éventuel nouveau gestionnaire de rendu. Détruit l'ancien.
	 */
	virtual void setRenderingManager (RenderingManager* renderingManager);
	
	//@}	// La gestion des entités et du rendu.


	/**
	 * Les opérations graphiques.
	 */
	//@{

	/**
	 * \return		Le widget dans lequel sont effectués les rendus graphiques.
	 *				Méthode à surcharger.
	 */
	virtual const QWidget* getRenderingWidget ( ) const;
	virtual QWidget* getRenderingWidget ( );

	//@}

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
	 */
	virtual void createGui ( );


	private :

	// Opérations interdites :
	Qt3DGraphicalWidget (const Qt3DGraphicalWidget&);
	Qt3DGraphicalWidget& operator = (const Qt3DGraphicalWidget&);

	/** Le gestionnaire d'entités et de rendu. */
	RenderingManager*			_renderingManager;
};	// class Qt3DGraphicalWidget


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_3D_GRAPHICAL_WIDGET_H
