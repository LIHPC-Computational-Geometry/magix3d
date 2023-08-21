/**
 * \file		StructuredMeshEntity.h
 * \author		Charles PIGNEROL
 * \date		04/12/2018
 */


#ifndef STRUCTURED_MESH_ENTITY_H
#define STRUCTURED_MESH_ENTITY_H

#include "Structured/StructuredMesh.h"
#include "Structured/Material.h"
#include "Internal/InternalEntity.h"
#include "Utils/Property.h"
#include "Utils/DisplayProperties.h"

#include <vector>


namespace Mgx3D
{


namespace Structured
{

/**
 * Entité <I>maillage structuré</I> utilisée par logiciel de projection <I>Euler vers Lagrange</I>.
 */
class StructuredMeshEntity : public Mgx3D::Internal::InternalEntity
{
	public :

	/**
	 * Constructeur.
	 * \param	Contexte d'utilisation de ce logiciel.
	 * \param	Propriétés de l'entité.
	 * \param	Propriétés d'affichage de l'entité.
	 * \param	Maillage structuré représenté.
	 * \param	Matériaux associés au maillage représenté.
	 * \warning	<B>ATTENTION : maillage et matériaux sont adoptés par l'entité qui prend donc en
	 *		charge leur destruction.</B>
	 */
	StructuredMeshEntity (
		Mgx3D::Internal::Context& context, Mgx3D::Utils::Property* prop, Mgx3D::Utils::DisplayProperties* disp,
		Mgx3D::Structured::StructuredMesh&, const std::vector<Mgx3D::Structured::Material*>& materials);

	/**
	 * \return	La dimension du maillage (3)
	 */
	virtual int getDim ( ) const
	{ return 3; }

	/**
	 * Destructeur. Détruit le maillage représenté.
	 */
	virtual ~StructuredMeshEntity ( );

	/**
	 * \return	Le nom du type d'objet (un nom distinct par type d'objet)
	 */
	virtual std::string getTypeName ( ) const;

	/**
	 * \return	Le type de l'objet (Entity::StructuredMesh par défaut).
	 */
	virtual Utils::Entity::objectType getType ( ) const;

	/**
	 * \return	Le maillage représenté.
	 */
	virtual const Mgx3D::Structured::StructuredMesh& getMesh ( ) const;
	virtual Mgx3D::Structured::StructuredMesh& getMesh ( );

	/** Accesseur par indirection au maillage. */
	virtual const Mgx3D::Structured::StructuredMesh* operator -> ( ) const;
	virtual Mgx3D::Structured::StructuredMesh* operator -> ( );


	private :

	/**
	 * Constructeur de copie, opérateur = : interdits.
	 */
	StructuredMeshEntity (const StructuredMeshEntity&);
	StructuredMeshEntity& operator = (const StructuredMeshEntity&);

	/** Le maillage représenté. */
	Mgx3D::Structured::StructuredMesh*		_mesh;

	/** Les matériaux associés au maillage. */
	std::vector<Mgx3D::Structured::Material*>	_materials;
};	// class StructuredMesh

}	// namespace Structured

}	// namespace Mgx3D



#endif	// STRUCTURED_MESH_ENTITY_H

