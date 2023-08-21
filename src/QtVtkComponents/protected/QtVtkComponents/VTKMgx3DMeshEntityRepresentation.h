/**
 * \file		VTKMgx3DMeshEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		22/05/2012
 */
#ifndef VTK_MGX3D_MESH_ENTITY_REPRESENTATION_H
#define VTK_MGX3D_MESH_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"

#include <Mesh/MeshEntity.h>


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
 *				de type maillage utilisant des structures de données
 *				<I>Magix 3D</I> pour leur description.
 *
 */
class VTKMgx3DMeshEntityRepresentation :
						public QtVtkComponents::VTKMgx3DEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	VTKMgx3DMeshEntityRepresentation (Mgx3D::Mesh::MeshEntity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKMgx3DMeshEntityRepresentation ( );

	
	protected :

	/**
	 * Créé la représentation "nuage" de l'entité représentée.
	 * \see createCloudRepresentation (const vector<Math::Point>&)
	 * \see createSurfacicRepresentation
	 * \see createWireRepresentation
	 */
	virtual void createCloudRepresentation ( );

	/**
	 * Créé la représentation volumique de l'entité représentée.
	 * \see createSurfacicRepresentation
	 * \see createVolumicRepresentation (const vector<Math::Point>&, const vector<size_t>&)
	 */
	virtual void createVolumicRepresentation ( );

	/**
	 * Créé la représentation volumique de l'entité représentée à partir des
	 * données transmises en argument.
	 * \param		Noeuds du maillages
	 * \param		Nombres de noeuds et indices des noeuds pour chaque
	 *				polyèdre
	 */
	virtual void createVolumicRepresentation (
							const std::vector<Utils::Math::Point>& points,
							const std::vector<size_t>& cells);

	/**
	 * Créé la représentation surfacique de l'entité représentée.
	 * \see createCloudRepresentation
	 * \see createSurfacicRepresentation (const vector<Math::Point>&, const vector<size_t>&)
	 * \see createWireRepresentation
	 */
	virtual void createSurfacicRepresentation ( );

	/**
	 * Créé la représentation filaire de l'entité représentée.
	 * \see createCloudRepresentation
	 * \see createWireRepresentation (const vector<Math::Point>&, const vector<size_t>&)
	 * \see createSurfacicRepresentation
	 */
	virtual void createWireRepresentation ( );

	/**
	 * Créé la représentation filaire complémentaire "ISO" de l'entité
	 * représentée.
	 * \see createWireRepresentation
	 * \see createIsoWireRepresentation (const vector<Math::Point>&, const vector<size_t>&)
	 */
	virtual void createIsoWireRepresentation ( );

	/**
	 * Créé la représentation "association vectorielle" entre 2 entités
	 * (ex : association topologie vers géométrie).
	 * \warning		<B>Ne fait rien dans cette classe (pas de création de
	 *				représentation).</B>
	 */
	virtual void createAssociationVectorRepresentation ( );

	/**
	 * Retourne une "représentation raffinée" de l'entité : points et
	 * éventuellement structuration segmentaire/triangulaire.
	 * En paramètre le degré de raffinement attendu (ex : 10 pour x10 par
	 * rapport à la représentation par défaut, ...).
	 * \return	true si la discrétisation a été faite selon les instructions,
	 *		false si elle doit être faite par ailleurs (par exemple par VTK).
	 */
	virtual bool getRefinedRepresentation (
		std::vector<Utils::Math::Point>& points, std::vector<size_t>& discretization,
		size_t factor);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKMgx3DMeshEntityRepresentation (VTKMgx3DMeshEntityRepresentation&);
	VTKMgx3DMeshEntityRepresentation& operator = (
									const VTKMgx3DMeshEntityRepresentation&);
};	// class VTKMgx3DMeshEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_MGX3D_MESH_ENTITY_REPRESENTATION_H
