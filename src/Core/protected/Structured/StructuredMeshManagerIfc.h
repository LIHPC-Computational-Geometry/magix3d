/*
 * \file StructuredMeshManagerIfc.h
 * \author Charles PIGNEROL
 * \date 03/12/2018
 */


#ifndef STRUCTURED_MESH_MANAGER_IFC_H
#define STRUCTURED_MESH_MANAGER_IFC_H

#include "Structured/StructuredMeshEntity.h"
#include "Structured/Material.h"
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Utils/SwigCompletion.h"
#include <string>
#include <vector>



namespace Mgx3D 
{

namespace Structured
{

/*----------------------------------------------------------------------------*/
/**
 * \class StructuredMeshManagerIfc
 * \brief Gestionnaire des opérations effectuées au niveau du module
 *        de maillages structurés.
 */
class StructuredMeshManagerIfc: public Mgx3D::Internal::CommandCreator 
{
	public:

	/** Constructeur
	 *
	 *  \param	Nom unique de l'instance (utile en environnement distribué).
	 *  \param	Contexte d'utilisation
	 */
	StructuredMeshManagerIfc (const std::string& name, Mgx3D::Internal::ContextIfc* c);

	/**
	 * Destructeur
	 */
	virtual ~StructuredMeshManagerIfc ( );

	/**
	 * Les commandes proposées par le gestionnaire de données structurées.
	 */
	//@{

	/*
	 * Chargement d'un maillage structuré à partir d'une <I>bande HIC</I>.
	 * \param	répertoire où sont localisées les données
	 * \param	type de données structurées (ex : <I>HIc_Eul</I>).
	 * \param	Nombre de processeurs à utiliser	
	 * \see		releaseMesh
	 * \exception	<B>A IMPLEMENTER DANS DES CLASSES DERIVEES</B> Spécialisation selon les besoins.
	 */
	virtual Mgx3D::Internal::M3DCommandResultIfc* loadHIC (const std::string& path, const std::string& type, size_t procNum);
	SET_SWIG_COMPLETABLE_METHOD(loadHIC)

	/**
	 * Libération du maillage structuré (toutes partitions).
	 * \warning	<B>Méthode à surcharger, lève une exception par défaut.</B>
	 */
	virtual Mgx3D::Internal::M3DCommandResultIfc* releaseMesh ( );
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
#endif	// SWIG


	private:

#ifndef SWIG
	/**
	 * Constructeur de copie. Interdit.
	 */
	StructuredMeshManagerIfc (const StructuredMeshManagerIfc&);

	/**
	 * Opérateur =. Interdit.
	 */
	StructuredMeshManagerIfc& operator = (const StructuredMeshManagerIfc&);
	#endif	// SWIG
};	// class StructuredMeshManagerIfc


}	// namespace Structured


} // namespace Mgx3D


#endif	// STRUCTURED_MESH_MANAGER_IFC_H
