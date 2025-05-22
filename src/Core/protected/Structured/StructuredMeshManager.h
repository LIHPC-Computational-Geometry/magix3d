#ifndef STRUCTURED_MESH_MANAGER_H
#define STRUCTURED_MESH_MANAGER_H
/*----------------------------------------------------------------------------*/
#include "Structured/StructuredMeshEntity.h"
#include "Structured/Material.h"
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResult.h"
#include "Utils/SwigCompletion.h"
/*----------------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D 
{
namespace Structured
{
/*----------------------------------------------------------------------------*/
/**
 * \class StructuredMeshManager
 * \brief Implémentation de gestionnaire des opérations effectuées au niveau du
 *        module de maillages structurés.
 */
class StructuredMeshManager final : public Mgx3D::Internal::CommandCreator 
{
	public:

	/** Constructeur
	 *
	 *  \param	Nom unique de l'instance (utile en environnement distribué).
	 *  \param	Contexte d'utilisation
	 */
	StructuredMeshManager (const std::string& name, Mgx3D::Internal::Context* c);

	/** 
	 * Destructeur
	 */
	~StructuredMeshManager ( );

	/**
	 * Les commandes proposées par le gestionnaire de données structurées.
	 */
	//@{

	/**
	 * Libération du maillage structuré (toutes partitions).
	 */
	Mgx3D::Internal::M3DCommandResult* releaseMesh ( );
	SET_SWIG_COMPLETABLE_METHOD(releaseMesh)

	//@}	// Les commandes proposées par le gestionnaire de données structurées.

#ifndef SWIG
	/**
	 * Les données structurées.
	 */
	//@{

	/**
	 * Affecter le maillage structuré au gestionnaire.
	 * \param	Numéro du processeur (cas d'un partitionnement au chargement).
	 * \param	Le (Partition du) maillage
	 * \see		releaseMeshEntities
	 * \see		getMeshEntity
	 * \see		getMesh
	 * \warning	Le maillage (et ses matériaux) est adopté et de ce fait il
	 *		sera détruit par ce gestionnaire.
	 */
	void setMeshEntity (size_t procNum, StructuredMeshEntity*& mesh);

	/**
	 * Décharger (toutes les partitions du) le maillage.
	 * \see		setMeshEntity
	 */
	void releaseMeshEntities ( );


	/**
	 * Accéder au maillage structuré.
	 * \return	Le(la partition demandée du) maillage structuré
	 * \param	Numéro du processeur (cas d'un partitionnement au chargement).
	 */
	const StructuredMeshEntity& getMeshEntity (size_t procNum) const;
	StructuredMeshEntity& getMeshEntity (size_t procNum);
	const StructuredMesh& getMesh (size_t procNum) const;
	StructuredMesh& getMesh (size_t procNum);

	/**
	 * \return	Les partitions chargées.
	 */
	std::vector<StructuredMeshEntity*> getMeshEntities ( ) const;

	//@}	// Les données structurées.
#endif	// SWIG


	private:

	/**
	 * Constructeur de copie. Interdit.
	 */
	StructuredMeshManager (const StructuredMeshManager&);

	/**
	 * Opérateur =. Interdit.
	 */
	StructuredMeshManager& operator = (const StructuredMeshManager&);

	/**
	 * Les maillages structurés pris en charge par l'instance.
	 * Clé d'entrée : numéro de processeur.
	 */
	std::map<int, StructuredMeshEntity*> _meshes;
};	// class StructuredMeshManager
}	// namespace Structured
} // namespace Mgx3D
#endif	// STRUCTURED_MESH_MANAGER_H
