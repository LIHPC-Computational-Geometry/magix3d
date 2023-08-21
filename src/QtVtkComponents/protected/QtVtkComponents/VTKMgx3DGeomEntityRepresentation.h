/**
 * \file		VTKMgx3DGeomEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		24/11/2011
 */
#ifndef VTK_MGX3D_GEOM_ENTITY_REPRESENTATION_H
#define VTK_MGX3D_GEOM_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"

#include <Geom/GeomEntity.h>


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
 *				de type géométrique utilisant des structures de données
 *				<I>Magix 3D</I> pour leur description.
 *
 */
class VTKMgx3DGeomEntityRepresentation :
						public QtVtkComponents::VTKMgx3DEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	VTKMgx3DGeomEntityRepresentation (Mgx3D::Geom::GeomEntity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKMgx3DGeomEntityRepresentation ( );


	protected :

	/**
	 * Créé la représentation "nuage" de l'entité représentée.
	 * \see createCloudRepresentation (const vector<Math::Point>&)
	 * \see createSurfacicRepresentation
	 * \see createWireRepresentation
	 */
	virtual void createCloudRepresentation ( );

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
	 * éventuellement structuration segmentaire ou triangulaire.
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
	VTKMgx3DGeomEntityRepresentation (VTKMgx3DGeomEntityRepresentation&);
	VTKMgx3DGeomEntityRepresentation& operator = (
									const VTKMgx3DGeomEntityRepresentation&);
};	// class VTKMgx3DGeomEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_MGX3D_GEOM_ENTITY_REPRESENTATION_H
