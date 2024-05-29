/**
 * \file		VTKMgx3DEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		28/11/2011
 */
#ifndef VTK_MGX3D_ENTITY_REPRESENTATION_H
#define VTK_MGX3D_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKEntityRepresentation.h"

class vtkTransform;

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
 *				utilisant des structures de données <I>Magix 3D</I> pour leur
 *				description.
 *
 */
class VTKMgx3DEntityRepresentation :
						public QtVtkComponents::VTKEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	VTKMgx3DEntityRepresentation (Mgx3D::Utils::Entity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKMgx3DEntityRepresentation ( );

	/**
	 * Créé la représentation "nuage" de l'entité représentée à partir des
	 * points transmis en arguments.
	 * \param		Eventuelle entité représentée
	 * \param		Acteur, mapper et grille <I>VTK</I> utilisés pour la
	 * 				représentation "nuage" de l'entité.
	 * \param		Points du nuage
	 */
	static void createPointsCloudRepresentation (
						Mgx3D::Utils::Entity* entity,
						VTKMgx3DActor*& actor, vtkDataSetMapper*& mapper,
						vtkUnstructuredGrid*& grid,
						const std::vector<Mgx3D::Utils::Math::Point>& points);

	/**
	 * Créé la représentation filaire de l'entité représentée à partir des
	 * points et segments transmis en arguments.
	 * \param		Eventuelle entité représentée
	 * \param		Acteur, mapper et grille <I>VTK</I> utilisés pour la
	 * 				représentation "filaire" de l'entité.
	 * \param		Points et segments définissant la représentation.
	 */
	static void createSegmentsWireRepresentation (
			Mgx3D::Utils::Entity* entity,
			VTKMgx3DActor*& actor, vtkDataSetMapper*& mapper,
			vtkUnstructuredGrid*& grid,
			const std::vector<Mgx3D::Utils::Math::Point>& points,
			const std::vector<size_t>& segments);

	/**
	 * Créé la représentation surfacique de l'entité représentée à partir des
	 * points et triangles transmis en arguments.
	 * \param		Eventuelle entité représentée
	 * \param		Acteur, mapper et grille <I>VTK</I> utilisés pour la
	 * 			représentation "filaire" de l'entité.
	 * \param		Points et triangles définissant la représentation.
	 */
	static void createTrianglesSurfacicRepresentation (
			Mgx3D::Utils::Entity* entity,
			VTKMgx3DActor*& actor, vtkDataSetMapper*& mapper,
			vtkUnstructuredGrid*& grid,
			const std::vector<Mgx3D::Utils::Math::Point>& points,
			const std::vector<size_t>& triangles);


	protected :


    /**
     * Créé la représentation volumique de l'entité représentée.
     *
     * Rien n'est prévu en dehors du maillage
     *
     * \see VTKGMDSEntityRepresentation
     */
    virtual void createVolumicRepresentation ( ) {}

	/**
	 * Créé la représentation surfacique de l'entité représentée à partir des
	 * points et triangles transmis en arguments.
	 */
	virtual void createTrianglesSurfacicRepresentation (
						const std::vector<Mgx3D::Utils::Math::Point>& points,
						const std::vector<size_t>& triangles);

	/**
	 * Créé la représentation surfacique de l'entité représentée à partir des
	 * points et polygones transmis en arguments.
	 * Les polygones sont définis par nombres de points puis indices des
	 * points.
	 */
	virtual void createPolygonsSurfacicRepresentation (
						const std::vector<Mgx3D::Utils::Math::Point>& points,
						const std::vector<size_t>& triangles);

	/**
	 * Créé la représentation "association vectorielle" entre 2 entités
	 * (ex : association topologie vers géométrie).
	 * \param		Vecteur représentant l'association
	 * \param		Couleur du vecteur
	 */
	virtual void createAssociationVectorRepresentation (
						const std::vector<Utils::Math::Point>& vector,
						const TkUtil::Color& color);

	/**
	 * Créé la représentation "repère local"
	 * \param transf la transformation
	 */
	virtual void createTrihedronRepresentation (
			vtkTransform* transf);

	/**
	 * Créé une "représentation raffinée".
	 * En paramètre le degré de raffinement attendu (ex : 10 pour x10 par
	 * rapport à la représentation par défaut, ...).
	 * \see	getRefinedRepresentation
	 * \see destroyRefinedRepresentation
	 */
	virtual void createRefinedRepresentation (size_t factor);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKMgx3DEntityRepresentation (VTKMgx3DEntityRepresentation&);
	VTKMgx3DEntityRepresentation& operator = (
										const VTKMgx3DEntityRepresentation&);
};	// class VTKMgx3DEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_MGX3D_ENTITY_REPRESENTATION_H
