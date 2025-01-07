/**
 * \file		QtMgx3DQualityDividerWidget.h
 * \author		Charles PIGNEROL
 * \date		07/01/2025
 */

#ifndef QT_MGX3D_QUALITY_DIVIDER_WIDGET_H
#define QT_MGX3D_QUALITY_DIVIDER_WIDGET_H

#include "Internal/ContextIfc.h"

#include <QtQualif/QtQualityDividerWidget.h>

#include "Mesh/MeshItf.h"
#include "Internal/Context.h"

#include <gmds/utils/CommonTypes.h>
#include <map>


namespace Mgx3D
{

namespace QtComponents
{

class QtMgx3DMainWindow;

/**
 * <P>Classe de Widget <I>Qt</I> permettant de rechercher dans un maillage les mailles correspondant à un certain profil de qualité
 * (domaine min-max d'un critère de qualité), l'objectif étant de rechercher les mailles invalides dans une botte de foin !
 * </P>
 *
 * <P>Les méthodes de cette classe sont susceptibles de lever des exceptions de type <I>TkUtil::Exception</I>.
 * </P>
 *
 * \see		AbstractQualifSerie
 */
class QtMgx3DQualityDividerWidget : public GQualif::QtQualityDividerWidget
{
	Q_OBJECT

	public :

	/**
	 * Constructeur.
	 * \param		Widget parent.
	 * \param		Context d'utilisation du widget
	 * \param		Fenêtre Magix3D principale, pour les opérations d'affichage
	 */
	QtMgx3DQualityDividerWidget (QWidget* parent, Mgx3D::Internal::Context* context, QtMgx3DMainWindow* mainWindow);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DQualityDividerWidget ( );

    /**
	 * Effectue l'extraction selon les paramètres définis par l'utilisateur.
	 */
	virtual void computeCallback ( );


	protected :

	/**
	 * Affiche/masque l'extraction dont l'indice est transmis en argument.
	 * @param		Indice de l'extraction à afficher.
	 * @param		<I>true</I> s'il faut l'afficher, <I>false</I> s'il faut la masquer.
	 */
	virtual void displayExtraction (size_t i, bool display);
	
	/**
	 * \return		Le context d'utilisation du widget.
	 */
	virtual Mgx3D::Internal::Context& getContext ( );
	virtual const Mgx3D::Internal::Context& getContext ( ) const;

	/**
	 * Détruit les entités (surfaces, volumes, ...) créées pour affichage.
	 */
	virtual void removeDataGroupSubsets ( );
	
	
	protected slots :

	/**
	 * Callback sur la fonction "Afficher" les classes sélectionnées.
	 */
	virtual void displayCellsCallback ( );

	/**
	 * Callback Qt. Réinitialise l'histogramme en fonction de la sélection en cours en appelant <I>autoUpdate</I>.
	 */
	virtual void reinitializeCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DQualityDividerWidget (const QtMgx3DQualityDividerWidget&);
	QtMgx3DQualityDividerWidget& operator = (const QtMgx3DQualityDividerWidget&);

	/** La fenêtre principale. */
	QtMgx3DMainWindow*														_mainWindow;
	
	/** Les boutons du panneau. */
	QPushButton																*_displayButton, *_initializeButton;

	/** Le context Magix 3D d'utilisation du panneau. */
	Mgx3D::Internal::Context*												_context;
	
	/** Les groupes affichés correspondant aux extractions obtenues. */
	std::map<GQualif::AbstractQualifSerie*, Mgx3D::Mesh::MeshEntity*>		_extractions;
};	// class QtMgx3DQualityDividerWidget


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_QUALITY_DIVIDER_WIDGET_H
