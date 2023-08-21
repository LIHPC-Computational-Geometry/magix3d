/**
 * \file		VTKMgx3DStructuredMeshEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		07/12/2018
 */
#ifndef VTK_MGX3D_STRUCTURED_MESH_ENTITY_REPRESENTATION_H
#define VTK_MGX3D_STRUCTURED_MESH_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"

#include <Structured/StructuredMeshEntity.h>


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
 * \brief	Classe <I>représentant graphique d'entité</I> spécialisée pour
 *		un affichage 3D reposant sur <I>VTK</I> d'entité <I>Magix 3D</I>
 *		de type <U>maillage structuré</U> utilisant des structures de données
 *		<I>Magix 3D</I> pour leur description.
 * ===========================================================================================
 * \warning	<B>Cette classe permet d'afficher des maillages structurés non issus de ||
 *		Magix3D. Ses mécanismes sont très différents de ceux des entités Magix 3D   ||
 *		types entités géométriques ou topologiques.</B>                             ||
 *		L'implémentation actuelle repose sur la classe vtkUnstructuredGrid.         ||
 *		Intérêt : on n'affiche que les mailles contenant un matériau. C'est d'autant||
 *		plus intéressant en cas de demande de transparence, opération très coûteuse ||
 *		en cas de rendu composite.                                                  ||
 * ===========================================================================================
 */
class VTKMgx3DStructuredMeshEntityRepresentation : public QtVtkComponents::VTKMgx3DEntityRepresentation
{
	public :

	/**
	 * \param	Entité représentée
	 */
	VTKMgx3DStructuredMeshEntityRepresentation (Mgx3D::Structured::StructuredMeshEntity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKMgx3DStructuredMeshEntityRepresentation ( );

	/**
	 * \return	Une référence sur l'entité représentée.
	 */
	virtual const Mgx3D::Structured::StructuredMeshEntity& getStructuredMeshEntity ( ) const;
	virtual Mgx3D::Structured::StructuredMeshEntity& getStructuredMeshEntity ( );

	
	protected :

	/**
	 * Actualise les attributs de représentation (couleur, épaisseur des traits,
	 * ...). Prend en compte le caractère sélectionné ou non de l'entité dans
	 * la représentation graphique.
	 */
	virtual void updateRepresentationProperties ( );
	
	/**
	 * Créé la représentation "nuage" de l'entité représentée.
	 * \see createSurfacicRepresentation
	 * \see createWireRepresentation
	 */
	virtual void createCloudRepresentation ( );

	/**
	 * Créé la représentation volumique de l'entité représentée.
	 * \see createSurfacicRepresentation
	 * \see createVolumicRepresentation (const Mgx3D::Structured::StructuredMesh&)
	 */
	virtual void createVolumicRepresentation ( );

	/**
	 * Créé la représentation volumique de l'entité représentée à partir du maillage transmis en argument.
	 * \param		Maillage structuré représenté.
	 */
	virtual void createVolumicRepresentation (const Mgx3D::Structured::StructuredMesh& mesh);

	/**
	 * Créé la représentation surfacique de l'entité représentée.
	 * \see createCloudRepresentation
	 * \see createWireRepresentation
	 */
	virtual void createSurfacicRepresentation ( );

	/**
	 * Créé la représentation filaire de l'entité représentée.
	 * \see createCloudRepresentation
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
	 * \warning		<B>Ne fait rien dans cette classe (pas de création de représentation).</B>
	 */
	virtual void createAssociationVectorRepresentation ( );

	/**
	 * Retourne une "représentation raffinée" de l'entité
	 * \return	false (ne fait rien dans cette mplémentation)
	 */
	virtual bool getRefinedRepresentation (
		std::vector<Utils::Math::Point>& points, std::vector<size_t>& discretization, size_t factor);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKMgx3DStructuredMeshEntityRepresentation (VTKMgx3DStructuredMeshEntityRepresentation&);
	VTKMgx3DStructuredMeshEntityRepresentation& operator = (const VTKMgx3DStructuredMeshEntityRepresentation&);
};	// class VTKMgx3DStructuredMeshEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_MGX3D_STRUCTURED_MESH_ENTITY_REPRESENTATION_H
