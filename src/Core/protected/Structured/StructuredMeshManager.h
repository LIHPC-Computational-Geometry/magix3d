/*
 * \file	StructuredMeshManagerIfc.h
 * \author	Charles PIGNEROL
 * \date	03/12/2018
 */


#ifndef STRUCTURED_MESH_MANAGER_H
#define STRUCTURED_MESH_MANAGER_H

#include "Structured/StructuredMeshManagerIfc.h"
#include "Internal/Context.h"

#include <map>


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
class StructuredMeshManager : public StructuredMeshManagerIfc 
{
	public:

	/** Constructeur
	 *
	 *  \param	Nom unique de l'instance (utile en environnement distribué).
	 *  \param	Contexte d'utilisation
	 */
	StructuredMeshManager (const std::string& name, Mgx3D::Internal::ContextIfc* c);

	/** 
	 * Destructeur
	 */
	virtual ~StructuredMeshManager ( );

	/**
	 * Les commandes proposées par le gestionnaire de données structurées.
	 */
	//@{

	/**
	 * Libération du maillage structuré (toutes partitions).
	 */
	virtual Mgx3D::Internal::M3DCommandResultIfc* releaseMesh ( );

	//@}	// Les commandes proposées par le gestionnaire de données structurées.

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
	virtual void setMeshEntity (size_t procNum, StructuredMeshEntity*& mesh);

	/**
	 * Décharger (toutes les partitions du) le maillage.
	 * \see		setMeshEntity
	 */
	virtual void releaseMeshEntities ( );


	/**
	 * Accéder au maillage structuré.
	 * \return	Le(la partition demandée du) maillage structuré
	 * \param	Numéro du processeur (cas d'un partitionnement au chargement).
	 */
	virtual const StructuredMeshEntity& getMeshEntity (size_t procNum) const;
	virtual StructuredMeshEntity& getMeshEntity (size_t procNum);
	virtual const StructuredMesh& getMesh (size_t procNum) const;
	virtual StructuredMesh& getMesh (size_t procNum);

	/**
	 * \return	Les partitions chargées.
	 */
	virtual std::vector<StructuredMeshEntity*> getMeshEntities ( ) const;

	//@}	// Les données structurées.


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
	std::map<int, StructuredMeshEntity*>	_meshes;
};	// class StructuredMeshManager


}	// namespace Structured


} // namespace Mgx3D


#endif	// STRUCTURED_MESH_MANAGER_H
