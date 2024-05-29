/*----------------------------------------------------------------------------*/
/** \file EntitiesHelper.h
 *
 *  \author Charles PIGNEROL
 *
 *  \date 14/01/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef INTERNAL_ENTITIES_HELPER_H_
#define INTERNAL_ENTITIES_HELPER_H_

#include "Geom/GeomEntity.h"
#include "Topo/TopoEntity.h"

#include <TkUtil/InternalError.h>

#include <vector>
#include <string>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils{
class Command;
}
namespace Geom{
class Curve;
class Surface;
}
namespace Topo{
class Vertex;
class CoEdge;
class CoFace;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/**
 * \brief		Services utilitaires concernant les entités : récupération de
 *				listes typées, de listes de noms, ...
 */
class EntitiesHelper
{
public:

	/**
	 * Les conversions de listes.
	 */
	//@{

	/**
	 * \return		La liste des entités géométriques contenues dans la liste d'entités transmises en arguments.
	 */
	static std::vector<Mgx3D::Geom::GeomEntity*> geomEntities (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * \return		La liste des entités topologiques contenues dans la liste d'entités transmises en arguments.
	 */
	static std::vector<Mgx3D::Topo::TopoEntity*> topoEntities (const std::vector<Mgx3D::Utils::Entity*>& entities);
	static std::vector<Mgx3D::Topo::Block*> topoBlocks (const std::vector<Mgx3D::Utils::Entity*>& entities);
	static std::vector<Mgx3D::Topo::CoFace*> topoCoFaces (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * \return		La liste d'entités dont les identifiants sont transmis en arguments.
	 */
	static std::vector<Mgx3D::Utils::Entity*> entities (const std::vector<unsigned long>& ids);

	/**
	 * \return		La liste d'identifiants uniques des entités transmises en arguments.
	 */
	static std::vector<unsigned long> uids (const std::vector<Mgx3D::Utils::Entity*>& entities);

	//@}	// Les conversions de listes.

	/**
	 * Utilitaires concernant les noms des entités.
	 */
	//@{

	/**
	 * Compare les 2 entités transmises en argument selon leur nom. A utiliser, par exemple, pour ordonner des
	 * vecteurs d'entités, opération utile en mode parallèle lors d'opérations sur une sélection d'entités,
	 * la class GroupManager retournant des vecteurs ordonnés selon l'adresse en mémoire ...
	 */
	static bool compare (const Mgx3D::Utils::Entity* e1, const Mgx3D::Utils::Entity* e2);

	/**
	 * Pour les messages. Convertit la liste d'entités transmises en argument en
	 * une chaîne de caractères composée des noms des entités (ex : <I>3
	 * entités géométriques a,b et c</I>).
	 * \param		Liste d'entités dont on souhaite recueillir les noms.
	 * \param		Nombre d'entités à partir duquel on ne mettra pas les noms
	 *				dans la chaîne retournée mais le nombre d'entités (
	 *				<I>n entités</I>, ou <I>n entités géométriques</I>).
	 */
	static std::string entitiesNames (const std::vector<Mgx3D::Utils::Entity*>& entities, size_t max = 100);
	static std::string entitiesNames (const std::vector<Mgx3D::Geom::GeomEntity*>& entities, size_t max =100);
	static std::string entitiesNames (const std::vector<Mgx3D::Topo::TopoEntity*>& entities, size_t max =100);

	//@}	// Utilitaires concernant les noms des entités.


	/// Retourne vrai si l'une des entités géométriques a un lien sur une entité topologique
	static bool hasTopoRef(std::vector<Geom::GeomEntity*>& entities);

	/** Conversion d'une liste de commandes en une liste de lignes de scripts
	 *  sans avoir de noms qui apparaissent mais juste des références sur les objets créés par les commandes précédentes
	 */
	static std::vector<TkUtil::UTF8String> replaceNameByCommandRef(std::vector<Utils::Command*>& commands);


    /// recherche des courbes associées à un sommet topologique
	static void getAssociatedCurves(Topo::Vertex* vtx, std::vector<Geom::Curve*>& curves);

    /// recherche des surfaces associées à un sommet topologique
	static void getAssociatedSurfaces(Topo::Vertex* vtx, std::vector<Geom::Surface*>& surfaces);

    /// recherche des surfaces associé à une arête topologique
	static void getAssociatedSurfaces(Topo::CoEdge* coedge, std::vector<Geom::Surface*>& surfaces);

	/// recherche d'une surface commune à tous les groupes de surfaces associés aux différentes arêtes d'une coface
	static Geom::Surface* getCommonSurface(Topo::CoFace* coface);

    /// retourne la distance entre 2 entités, une exception pour les cas non prévus
    static double length(const Utils::Entity* e1, const Utils::Entity* e2);

    /// retourne l'angle entre 3 entités, une exception pour les cas non prévus
    static double angle(const Utils::Entity* ec, const Utils::Entity* e1, const Utils::Entity* e2);

	/// recherche le type d'un objet en fonction de son nom
	static Utils::Entity::objectType getObjectType(TkUtil::UTF8String& nomObj);

private :

	/**
	 * Constructeurs et destructeurs. Opérations interdites.
	 */
	//@{
	EntitiesHelper ( );
	EntitiesHelper (const EntitiesHelper&);
	EntitiesHelper& operator = (const EntitiesHelper&);
	~EntitiesHelper ( );
	//@}
};	// class EntitiesHelper


/**
 * \return		Une liste d'entité issue d'une liste d'entités typées.
 */
template < typename E > inline std::vector< Mgx3D::Utils::Entity* > entitiesFromTypedEntities (
								const std::vector< E* >& typedEntities, bool strict = true)
{
	std::vector< Mgx3D::Utils::Entity* >	entities;

	for (typename std::vector< E* >::const_iterator	it	= typedEntities.begin ( );
		 typedEntities.end ( ) != it; it++)
	{
		Mgx3D::Utils::Entity*	entity	= dynamic_cast<Mgx3D::Utils::Entity*>(*it);
		if ((0 == entity) && (true == strict))
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Erreur, l'entité " << (*it)->getName ( )
			        << " n'est pas du bon type.";
			INTERNAL_ERROR (exc, message, "entitiesFromTypedEntities")
			throw exc;
		}	// if ((0 == entity) && (true == strict))
		entities.push_back (entity);
	}

	return entities;
}	// entitiesFromTypedEntities


/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INTERNAL_ENTITIES_HELPER_H_ */
/*----------------------------------------------------------------------------*/
