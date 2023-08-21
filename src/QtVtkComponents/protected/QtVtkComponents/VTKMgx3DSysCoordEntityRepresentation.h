/**
 * \file		VTKMgx3DSysCoordEntityRepresentation.h
 * \author		Eric Brière de l'Isle
 * \date		23/5/2018
 */
#ifndef VTK_MGX3D_SYSCOORD_REPRESENTATION_H
#define VTK_MGX3D_SYSCOORD_REPRESENTATION_H

#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"

#include <SysCoord/SysCoord.h>


namespace Mgx3D 
{

/*!
 * \namespace Mgx3D::QtVtkComponents
 *
 * \brief Espace de nom des classes pour l'affichage via Qt.
 *
 *
 */
namespace QtVtkComponents
{

/**
 * \brief		Classe <I>représentant graphique d'entité</I> spécialisée pour
 *				un affichage 3D reposant sur <I>VTK</I> d'entité <I>Magix 3D</I>
 *				de type topologique utilisant des structures de données
 *				<I>Magix 3D</I> pour leur description.
 *
 */
class VTKMgx3DSysCoordEntityRepresentation :
		public QtVtkComponents::VTKMgx3DEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	VTKMgx3DSysCoordEntityRepresentation (Mgx3D::CoordinateSystem::SysCoord& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKMgx3DSysCoordEntityRepresentation ( );

	/**
	 * Affichage/masquage de l'entité représentée.
	 * \param	Gestionnaire de rendu utilisé.
	 * \param	<I>true</I> s'il faut afficher l'entité, <I>false</I> s'il
	 * 			faut l'enlever.
	 */
	virtual void show (
			Mgx3D::QtVtkComponents::VTKRenderingManager& renderer, bool show);

	/**
	 * Met à jour la représentation graphique VTK de l'entité représentée.
	 * \param	Nouveau masque de représentation
	 * \param	<I>true</I> s'il faut réactualiser la représentation en toutes
	 * 			circonstances, y compris si le masque transmis est identique
	 * 			à l'actuel, <I>false</I> dans le cas contraire.
	 */
	virtual void updateRepresentation (unsigned long mask, bool force);


	protected :

	/**
	 * Créé la représentation "trièdre"
	 */
	virtual void createTrihedronRepresentation ( );


	/**
	 * Détruit les représentations graphiques actuelles.
	 */
	virtual void destroyRepresentations (bool realyDestroy);

	/**
	 * Actualise les attributs de représentation (couleur, épaisseur des traits,
	 * ...). Prend en compte le caractère sélectionné ou non de l'entité dans
	 * la représentation graphique.
	 * \warning	N'actualise pas la mise en évidence (<I>highlight</I>).
	 * \see		isSelected
	 * \see		updateHighlightRepresentation
	 */
	virtual void updateRepresentationProperties ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKMgx3DSysCoordEntityRepresentation (VTKMgx3DSysCoordEntityRepresentation&);
	VTKMgx3DSysCoordEntityRepresentation& operator = (
									const VTKMgx3DSysCoordEntityRepresentation&);

};	// class VTKMgx3DSysCoordEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_MGX3D_SYSCOORD_REPRESENTATION_H
