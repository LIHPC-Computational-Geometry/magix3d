#ifndef VTK_MGX3D_ACTOR_H
#define VTK_MGX3D_ACTOR_H


#include <Utils/DisplayRepresentation.h>

#include <vtkLODActor.h>

#include <iostream>


namespace Mgx3D
{

namespace Utils
{
	class Entity;
}	// namespace Utils

}	// namespace Mgx3D


/** Classe d'acteur VTK assurant la jonction avec les entités Magix 3D
 * représentées (optimisation visant à limiter les temps de recherches
 * acteur -> entité, par exemple lors d'opérations de <I>picking</I>).
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class VTKMgx3DActor : public vtkLODActor
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~VTKMgx3DActor ( );

	/**
	 * Instanciateur VTK. RAS.
	 */
	static VTKMgx3DActor* New ( );

	virtual const char* GetClassName ( );

	virtual void PrintSelf (std::ostream& os, vtkIndent indent);

	virtual void Render (vtkRenderer*, vtkMapper*);

	/**
	 * \return		L'entité associée.	
	 * \see			SetEntity
	 */
	virtual Mgx3D::Utils::Entity* GetEntity ( );
	virtual const Mgx3D::Utils::Entity* GetEntity ( ) const;

	/**
	 * \param		L'entité associée.	
	 * \see			GetEntity
	 */
	virtual void SetEntity (Mgx3D::Utils::Entity* entity);

	/**
	 * \param		Le type de représentation portée par l'acteur.
	 * \see			GetRepresentationType
	 */
	virtual void SetRepresentationType (
									Mgx3D::Utils::DisplayRepresentation::type);

	/**
	 * \return		Le type de représentation portée par l'acteur.
	 * \see			setRepresentationType
	 * \see			IsSolid
	 * \see			IsWire
	 */
	virtual Mgx3D::Utils::DisplayRepresentation::type
												GetRepresentationType ( ) const;

	/**
	 * \return		<I>true</I> si la représentation est solide, <I>false</I>
	 * 				dans le cas contraire.
	 * \see			IsWire
	 * \see			GetRepresentationType
	 */
	virtual bool IsSolid ( ) const;

	/**
	 * \return		<I>true</I> si la représentation est filaire, <I>false</I>
	 * 				dans le cas contraire.
	 * \see			IsSolid
	 * \see			GetRepresentationType
	 */
	virtual bool IsWire ( ) const;


	protected :

	VTKMgx3DActor ( );

	/**
	 * Ne pas créer de LODs pour les entités de type topologique.
	 */
	virtual void CreateOwnLODs ( );


	private :

	VTKMgx3DActor (const VTKMgx3DActor&);
	VTKMgx3DActor& operator = (const VTKMgx3DActor&);

	/** L'entité Magix 3D associée.  */
	Mgx3D::Utils::Entity*						_entity;

	/** Le type de représentation de l'acteur. */
	Mgx3D::Utils::DisplayRepresentation::type	_representationType;
};	// class VTKMgx3DActor


#endif	// VTK_MGX3D_ACTOR_H

