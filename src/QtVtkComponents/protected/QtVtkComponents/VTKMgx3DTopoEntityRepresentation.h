/**
 * \file		VTKMgx3DTopoEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		24/11/2011
 */
#ifndef VTK_MGX3D_TOPO_ENTITY_REPRESENTATION_H
#define VTK_MGX3D_TOPO_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"

#include <Topo/TopoEntity.h>

#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
#include <vtkLabeledDataMapper.h>
#include <vtkSelectVisiblePoints.h>
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION


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
 *				de type topologique utilisant des structures de données
 *				<I>Magix 3D</I> pour leur description.
 *
 */
class VTKMgx3DTopoEntityRepresentation :
						public QtVtkComponents::VTKMgx3DEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	VTKMgx3DTopoEntityRepresentation (Mgx3D::Topo::TopoEntity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKMgx3DTopoEntityRepresentation ( );

	/**
	 * \param		Gestionnaire de rendus 3D pour les affichages si celui-ci
	 *				n'a pas été déjà transmis via <I>show</I>. Actualise 
	 *				également les propriétés d'affichage selon le contexte du
	 *				gestionnaire (ex : utilisation de la couleur de l'entités
	 *				géométrique associée).
	 * \see			getRenderingManager
	 */
	virtual void setRenderingManager(Mgx3D::QtComponents::RenderingManager* rm);

	/**
	 * Affichage/masquage de l'entité représentée.
	 * \param	Gestionnaire de rendu utilisé.
	 * \param	<I>true</I> s'il faut afficher l'entité, <I>false</I> s'il
	 * 			faut l'enlever.
	 */
	virtual void show (
			Mgx3D::QtVtkComponents::VTKRenderingManager& renderer, bool show);

	/**
	 * \return	Les éventuelles informations textuelles à afficher.
	 * 			Gère nom et nombre d'arêtes.
	 */
	virtual std::string getRepresentationText (unsigned long mask) const;

	/**
	 * \return	La position où afficher les informations textuelles représentant
	 * 			l'entité. Dépend du masque de représentation transmis en
	 * 			argument.
	 */
	virtual Mgx3D::Utils::Math::Point getRepresentationTextPosition (
													unsigned long mask) const;

	/**
	 * Met à jour la représentation graphique VTK de l'entité représentée.
	 * \param	Nouveau masque de représentation
	 * \param	<I>true</I> s'il faut réactualiser la représentation en toutes
	 * 			circonstances, y compris si le masque transmis est identique
	 * 			à l'actuel, <I>false</I> dans le cas contraire.
	 */
	virtual void updateRepresentation (unsigned long mask, bool force);

	/**
	 * \return		La couleur à utiliser pour représenter l'entité 
	 * 				pour la valeur de représentation transmise
	 * 				(CLOUDS, CURVES, ISOCURVES, ...). Prend en compte l'éventuel
	 * 				caractere sélectionné et/ou mis en évidence, les préférences
	 * 				utilisateurs, le mode d'affichage (standard/couleur entité
	 * 				géométrique associée).
	 */
	virtual TkUtil::Color getColor (unsigned long rep) const;

	/**
	 * Le nom du champ scalaire qui porte le nombre de "bras" de l'arête. */
	static const std::string	edgesNumFieldName;


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
     * Créé la représentation "nuage" de l'entité représentée avec projection sur la modélisation.
     * \see createCloudRepresentation
     */
    virtual void createMeshShapeCloudRepresentation ( );

	/**
     * Créé la représentation filaire de l'entité représentée avec projection sur la modélisation.
     * \see createWireRepresentation
     */
    virtual void createMeshShapeWireRepresentation ( );

	/**
     * Créé la représentation de la discrétisation.
     * \see createWireRepresentation
     */
    virtual void createDiscretisationRepresentation ( );

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

#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	/**
	 * Dans le cas d'une arête, créé la représentation 2D du nombre de bras de
	 * l'arête.
	 * \param	Type de représentation à laquelle celle-ci est associée.
	 */
	virtual void createMeshingEdgesNumRepresentation (unsigned long mask);
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION

	/**
	 * Détruit les représentations graphiques actuelles.
	 */
	virtual void destroyRepresentations (bool realyDestroy);

	/**
	 * Actualise les attributs de représentation (couleur, épaisseur des traits,
	 * ...). Prend en compte le caractère sélectionné ou non de l'entité dans
	 * la représentation graphique.
	 * \warning	N'actualise pas la mise en évidence (<I>highlight</I>).
	 * \see		isSelected
	 * \see		updateHighlightRepresentation
	 */
	virtual void updateRepresentationProperties ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKMgx3DTopoEntityRepresentation (VTKMgx3DTopoEntityRepresentation&);
	VTKMgx3DTopoEntityRepresentation& operator = (
									const VTKMgx3DTopoEntityRepresentation&);

#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	/** Nombre d'arêtes de maillage. */
	vtkActor2D*					_meshingEdgesNumActor;
	vtkLabeledDataMapper*		_meshingEdgesNumMapper;

	/** Le choix d'afficher ou non le nombre d'arêtes de maillage. */
	vtkSelectVisiblePoints*		_visibleMeshingEdgesFilter;
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
};	// class VTKMgx3DTopoEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_MGX3D_TOPO_ENTITY_REPRESENTATION_H
