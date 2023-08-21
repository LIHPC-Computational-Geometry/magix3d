/**
 * \file		VTKGMDSEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		29/11/2011
 */
#ifndef VTK_GMDS_ENTITY_REPRESENTATION_H
#define VTK_GMDS_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKEntityRepresentation.h"
#include "Mesh/MeshImplementation.h"

namespace gmds
{
class Face;
class Node;
class IGMesh;
}

namespace Mgx3D 
{

namespace Mesh
{
class MeshEntity;
}

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
 *				utilisant des structures de données <I>GMDS</I> pour leur
 *				description.
 *
 */
class VTKGMDSEntityRepresentation :
						public QtVtkComponents::VTKEntityRepresentation
{
public :

	/**
	 * \param		Entité représentée
	 */
	VTKGMDSEntityRepresentation (Mgx3D::Utils::Entity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKGMDSEntityRepresentation ( );


protected :

    /**
	 * Créé la représentation "nuage" de l'entité représentée.
	 * \see createCloudRepresentation
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
     * Créé la représentation volumique de l'entité représentée.
     * \see createSurfacicRepresentation
     */
    virtual void createVolumicRepresentation ( );

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
	 */
	virtual void createIsoWireRepresentation ( );

	/**
	 *  Créé la représentation "association vectorielle" entre 2 entités
	 * (ex : association topologie vers géométrie).
	 * \warning		<B>Ne fait rien dans cette classe (pas de création de
	 *				représentation).</B>
	 */
	virtual void createAssociationVectorRepresentation ( );

	/**
	 * Créé la représentation "nuage" de l'entité représentée à partir des
	 * points contenus dans la structure GMDS.
	 */
	virtual void createMeshEntityCloudRepresentation (
	        Mesh::MeshEntity* meshEntity);

	/**
	 * Créé arêtes contenues dans la structure GMDS.
	 */
	virtual void createMeshEntityLineRepresentation (
	        Mesh::MeshEntity* meshEntity);

	/**
	 * Créé la représentation surfacique VTK (<I>_surfacicGrid</I>) de l'entité
	 * représentée à partir des points et polygones du maillage.
	 * \param l'entité de maillage
	 * \warning		<B>Ne créé pas l'acteur (<I>_surfacicActor</I> et le mapper
	 * 				<I>_surfacicMapper</I> nécessaires à son affichage.</B>
	 */
	virtual void createMeshEntitySurfacicRepresentation (
	        Mesh::MeshEntity* meshEntity);

    /**
     * Créé la représentation volumique VTK (<I>_volumicGrid</I>) de l'entité
     * représentée à partir des points et polyêdres du maillage.
     * \param l'entité de maillage
     * \warning     <B>Ne créé pas l'acteur (<I>_surfacicActor</I> et le mapper
     *              <I>_surfacicMapper</I> nécessaires à son affichage.</B>
     */
	virtual void createMeshEntityVolumicRepresentation(
            Mesh::MeshEntity* meshEntity);

	/**
	 * Effectue une homothétie suivant le facteur demandé
	 */
	virtual void doShrink(vtkPoints* points);

	/**
	 * Retourne une "représentation raffinée" de l'entité : points et
	 * éventuellement structuration segmentaire/triangulaire.
	 * En paramètre le degré de raffinement attendu (ex : 10 pour x10 par
	 * rapport a la représentation par défaut, ...).
	 * \return	true si la discrétisation a été faite selon les instructions,
	 *		false si elle doit être faite par ailleurs (par exemple par VTK).
	 */
	virtual bool getRefinedRepresentation (
		std::vector<Utils::Math::Point>& points, std::vector<size_t>& discretization,
		size_t factor);


private :

	/**
	 * Créé la représentation surfacique VTK pour cas 2D
	 */
	virtual void createMeshEntitySurfacicRepresentation2D(Mesh::MeshEntity* meshEntity, gmds::IGMesh& gmdsMesh);

	/**
	 * Créé la représentation surfacique VTK pour cas 3D
	 */
	virtual void createMeshEntitySurfacicRepresentation3D(Mesh::MeshEntity* meshEntity, gmds::IGMesh& gmdsMesh);


	/**
	 * Créé la représentation surfacique VTK pour des cofaces sans ratio de dégradation
	 */
	virtual void createCoFacesSurfacicRepresentationRatio1(std::vector<Topo::CoFace*> cofaces, gmds::IGMesh& gmdsMesh);

	/**
	 * Créé la représentation surfacique VTK pour des cofaces avec ratio de dégradation
	 */
	virtual void createCoFacesSurfacicRepresentationRatioN(std::vector<Topo::CoFace*> cofaces, gmds::IGMesh& gmdsMesh, int ratio);


	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKGMDSEntityRepresentation (VTKGMDSEntityRepresentation&);
	VTKGMDSEntityRepresentation& operator = (
										const VTKGMDSEntityRepresentation&);

};	// class VTKGMDSEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_GMDS_ENTITY_REPRESENTATION_H
