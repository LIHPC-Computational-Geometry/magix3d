/**
 * \file		QtVtkMgx3DExperimentalRoomPanel.h
 * \author		Charles PIGNEROL
 * \date		30/08/2016
 */
 
#ifdef USE_EXPERIMENTAL_ROOM

#ifndef QT_LEM_EXPERIMENTAL_ROOM_PANEL_H
#define QT_LEM_RAYS_PANEL_H

#include <ExperimentalRoom/QtVtkExperimentalRoomPanel.h>
#include "Internal/Context.h"


namespace Mgx3D
{

namespace QtComponents
{
	class QtMgx3DMainWindow;
}	// namespace QtComponents

namespace QtVtkComponents
{

/** Conversion repère Magix 3D -> repère chambre d'expérience QtVtk. 
 * \throw	Lève une exception en cas d'erreur. 
 */
ExperimentRay::LANDMARK landmarkToExpRoomLandmark (Mgx3D::Utils::Landmark::kind);


/**
 * Panneau pemettant de gérer une salle d'experience <I>code T</I> avec des 
 * rayons et diagnostics à afficher dans une scène <I>Magix 3D</I>.
 */
class QtVtkMgx3DExperimentalRoomPanel : public QtVtkExperimentalRoomPanel
{
	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * @param	Nom du repère exprimental utilisé (a priori "Chambre
	 * 			expérimentale" ou "Conteneur".
	 * \param	Nom de l'application.
	 * \param	Afficheur <I>VTK</I> associé.
	 */
	QtVtkMgx3DExperimentalRoomPanel (
			QtComponents::QtMgx3DMainWindow* parent, const TkUtil::UTF8String& name,
			const TkUtil::UTF8String& expLandmarkName, const TkUtil::UTF8String& appTitle, vtkRenderer& renderer);

	/**
	 * RAS.
	 */
	virtual ~QtVtkMgx3DExperimentalRoomPanel ( );

	/**
	 * Propose si nécessaire d'ajuster l'unité du maillage à celle des
	 * pointages laser.
	 * \param	La nouvelle salle d'expérience représentée et adoptée.
	 * 			L'ancienne salle est détruite.
	 * \param	Eventuel gestionnaire d'IOs associé et adopté
	 * \see		getExperimentalRoom
	 * \see		checkUnities
	 */
	virtual void setExperimentalRoom (ExperimentalRoom&, ExperimentalRoomIOManager* ios);

	/**
 	 * \return	Le contexte d'utilisation du panneau
 	 */
	virtual Mgx3D::Internal::Context& getContext ( );

	/**
	 * Utilise l'unité de longueur et le repère du contexte transmis en argument. global vaut true si on est en mode
	 * d'affichage global, false dans le cas contraire
	 * Projette les pointages laser sur les surfaces actuellement sélectionnées.
	 * \see		checkUnities
	 * \see		getContext
	 * \see		useSelection
	 */
	virtual void setContext (Mgx3D::Internal::Context& context, bool global);

	/**
	 * Utilise la sélection du contexte transmis en argument pour y projeter les pointages laser.global vaut true si on est en mode
	 * d'affichage global, false dans le cas contraire
	 * \see		checkUnities
	 * \see		getContext
	 */
	virtual void useSelection (Mgx3D::Internal::Context& context, bool global);


	protected :

	/**
	 * Applique au maillage l'unité de longueur des rayons.
	 * \param		La puissance de 10 appliqué au mètre pour atteindre
	 * 				la nouvelle unité du maillage.
	 * \see			setMeterPow
	 * \see			getMeterPow
	 */
	virtual void synchronizeMeshUnit (char exp);


	private :

	// Opérations interdites :
	QtVtkMgx3DExperimentalRoomPanel (const QtVtkMgx3DExperimentalRoomPanel&);
	QtVtkMgx3DExperimentalRoomPanel& operator = (const QtVtkMgx3DExperimentalRoomPanel&);

	/** Le conetxte d'utilisation du panneau. */
	Mgx3D::Internal::Context*	_context;
};	// class QtVtkMgx3DExperimentalRoomPanel


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// QT_LEM_RAYS_PANEL_H

#endif	// USE_EXPERIMENTAL_ROOM
