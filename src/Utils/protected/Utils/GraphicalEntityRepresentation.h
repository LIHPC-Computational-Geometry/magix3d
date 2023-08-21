/**
 * \file		GraphicalEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		24/11/2011
 */
#ifndef GRAPHICAL_ENTITY_REPRESENTATION_H
#define GRAPHICAL_ENTITY_REPRESENTATION_H


#include "Utils/Entity.h"
#include "Utils/DisplayProperties.h"


namespace Mgx3D 
{

namespace Utils
{

/**
 * \brief		Représente graphiquement une entité. Classe abstraite à
 *				implémenter pour l'API graphique utilisée (<I>VTK</I>, ...).
 */
class GraphicalEntityRepresentation :
				public Mgx3D::Utils::DisplayProperties::GraphicalRepresentation
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~GraphicalEntityRepresentation ( );

	/**
	 * \return		L'entité représentée.
	 */
	Mgx3D::Utils::Entity* getEntity ( )
	{ return _entity; }
	const Mgx3D::Utils::Entity* getEntity ( ) const
	{ return _entity; }

	/**
	 * La représentation de l'entité.
	 */
	//@{
	/**
	 * Afficher les noeuds.
	 * \see		CURVES
	 * \see		SURFACES
	 * \see		updateRepresentation
	 */
	static const unsigned long CLOUDS;

	/**
	 * Afficher les arêtes.
	 * \see		CLOUDS
	 * \see		ISOCURVES
	 * \see		SURFACES
	 * \see		MESH_SHAPE
	 * \see		ASSOCIATIONS
	 * \see		updateRepresentation
	 */
	static const unsigned long CURVES;

	/**
	 * Afficher les segments complémentaires de courbes "iso" pour une
	 * représentation plus complète.
	 * \see		CLOUDS
	 * \see		CURVES
	 * \see		SURFACES
	 * \see		updateRepresentation
	 */
	static const unsigned long ISOCURVES;

	/**
	 * Afficher les surfaces.
	 * \see		CLOUDS
	 * \see		CURVES
	 * \see		ASSOCIATIONS
	 * \see		updateRepresentation
	 */
	static const unsigned long SURFACES;

    /**
     * Afficher les volumes.
     * \see     updateRepresentation
     */
    static const unsigned long VOLUMES;

	/**
	 * Afficher les noms des entités.
     * \see     updateRepresentation
	 */
	static const unsigned long	NAMES;

	/**
	 * Afficher les arêtes topologiques projetées sur le maillage.
     * \see     updateRepresentation
	 */
	static const unsigned long	MESH_SHAPE;

	/**
	 * Afficher les associations (ex : arêtes topologiques vers courbes
	 * géométriques).
     * \see     updateRepresentation
	 */
	static const unsigned long	ASSOCIATIONS;

	/**
	 * Afficher le triêdre
     * \see     updateRepresentation
	 */
	static const unsigned long	TRIHEDRON;

	/**
	 * Afficher une représentation du type de discrétisation (pour arêtes, cofaces et blocs).
     * \see     updateRepresentation
	 */
	static const unsigned long	DISCRETISATIONTYPE;

	/**
	 * Afficher le nombre de "bras" de la discrétisation (pour les arêtes).
     * \see     updateRepresentation
	 */
	static const unsigned long	NBMESHINGEDGE;

	/**
	 * Afficher les valeurs aux noeuds.
     * \see     CELLS_VALUES
     * \see     updateRepresentation
	 */
	static const unsigned long	NODES_VALUES;

	/**
	 * Afficher les valeurs aux mailles.
     * \see     NODES_VALUES
     * \see     updateRepresentation
	 */
	static const unsigned long	CELLS_VALUES;

	/**
	 * \return	Retourne un masque de représentation par défaut pour
	 *			les entités de type renseigné en argument.
	 * \param	Type d'entité visé.
	 */
	static unsigned long getDefaultRepresentationMask (
			Mgx3D::Utils::Entity::objectType entititesType);

    /**
     * \return  Retourne un masque de visibilité suivant ce qui
     *  est ou non affichable suivant un type d'entité
     *  renseigné en argument.
     * \param   Type d'entité visé.
     */
    static unsigned long getVisibleTypeByEntityMask (
            Mgx3D::Utils::Entity::objectType entititesType);

	/**
	 * \return	<I>true</I> si des informations textuelles doivent être
	 * 			représentées (selon le masque passé en argument), <I>false</I>
	 * 			dans le cas contraire. Les informations textuelles peuvent
	 * 			être de différentes natures (noms, nombre d'arêtes) et
	 * 			éventuellement composites.
	 */
	static bool displayTextualRepresentation (unsigned long mask);

	/**
	 * \return	Les éventuelles informations textuelles à afficher. Dépend du
	 * 			masque de représentation transmis en argument. Par défaut
	 * 			cette méthode gère le nom de l'entité.
	 */
	virtual std::string getRepresentationText (unsigned long mask) const;

	/**
	 * \return	La position où afficher les informations textuelles représentant
	 * 			l'entité. Dépend du masque de représentation transmis en argument.
	 */
	virtual Mgx3D::Utils::Math::Point getRepresentationTextPosition (
													unsigned long mask) const;

	/**
	 * Met à jour la représentation graphique de l'entité
	 * représentée. Créé cette représentation graphique si nécessaire.
	 * Ne fait rien par défaut, si ce n'est mémoriser le masque de
	 * représentation à utiliser.
	 * \param	Nouveau masque de représentation
	 * \param	<I>true</I> s'il faut réactualiser la représentation en toutes
	 *			circonstances, y compris si le masque transmis est identique
	 *			à l'actuel, <I>false</I> dans le cas contraire.
	 * \see		getRepresentationMask
	 * \see		CLOUDS
	 * \see		CURVES
	 * \see		SURFACES
	 */
	virtual void updateRepresentation (unsigned long mask, bool force);

	/**
	 * \return		Une chaîne de caractères décrivant le masque transmis en
	 *				argument.
	 */
	static std::string rep3DMaskToString (unsigned long mask);
	//@}


	protected :

	/**
	 * \param		Entité représentée.
	 */
	GraphicalEntityRepresentation (Mgx3D::Utils::Entity& entity);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	GraphicalEntityRepresentation (const GraphicalEntityRepresentation&);
	GraphicalEntityRepresentation& operator = (
										const GraphicalEntityRepresentation&);

	/** L'entité représentée. */
	Mgx3D::Utils::Entity*		_entity;
};	// class GraphicalEntityRepresentation


}	// namespace Utils

}	// namespace Mgx3D

#endif	// GRAPHICAL_ENTITY_REPRESENTATION_H
