/*----------------------------------------------------------------------------*/
/** \file Volume.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_VOLUME_H_
#define MGX3D_GEOM_VOLUME_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group3D;
}
namespace Topo {
class Block;
class CoFace;
class CoEdge;
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomProperty;
/*----------------------------------------------------------------------------*/
/**
 * \class Volume
 * \brief Interface définissant les services que doit fournir tout volume
 *        géométrique
 */
class Volume: public GeomEntity {

    static const char* typeNameGeomVolume;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop    les propriétés associées au volume
     *  \param compProp les propriétés de calcul
     */
#ifndef SWIG
    Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, GeomRepresentation* compProp=0);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Volume();

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    double computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief Vérifie que la surface ASurf est contenue dans *this
     *  \param ASurf la surface contenue
     *  \return true si elle est contenue, false sinon
     */
    virtual bool contains(Volume* vol) const;


    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité en ses sous-entités.
     */
#ifndef SWIG
    virtual void split(std::vector<Surface*>& surf,
                       std::vector<Curve*  >& curv,
                       std::vector<Vertex* >&  vert);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    int getDim() const {return 3;}

    /*------------------------------------------------------------------------*/
    /** \brief  récupère la liste des geom_entity référencés par (*this)
     *
     *  \param entities la liste des entités géométriques que l'on récupère
     */
    virtual void getRefEntities(std::vector<GeomEntity*>& entities);

    /*------------------------------------------------------------------------*/
    /** \brief  récupère la liste des geom_entity référencés par (*this) et la
     *          supprime ceux appartenant à entities.
     */
#ifndef SWIG
    virtual void clearRefEntities(std::list<GeomEntity*>& vertices,
            std::list<GeomEntity*>& curves,
            std::list<GeomEntity*>& surfaces,
            std::list<GeomEntity*>& volumes);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets géométriques incidents
     *
     *  \param vertices les sommets incidents
     */
    virtual void get(std::vector<Vertex*>& vertices) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets géométriques incidents
     * et effectue un tri géométrique
     *
     *  \param vertices les sommets incidents
     */
    virtual void getGeomSorted(std::vector<Vertex*>& vertices) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux courbes géométriques incidentes
     *
     *  \param curves les courbes incidents
     */
    virtual void get(std::vector<Curve*>& curves) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux surfaces géométriques incidentes
     *
     *  \param surfaces les surfaces incidentes
     */
    virtual void get(std::vector<Surface*>& surfaces) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux volumes géométriques incidents
     *
     *  \param volumes les volumes incidents
     */
    virtual void get(std::vector<Volume*>& volumes) const;

    /*------------------------------------------------------------------------*/
     /** \brief  Fournit l'accès aux blocs topologiques qui pointent sur ce volume
      */
     virtual void get(std::vector<Topo::Block*>& blocs) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur le volume. P est modifié
     *  \param P le point à projeter
     *  [fonction non disponible]
     */
    virtual void project(Utils::Math::Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur le volume, le résultat est le point P2.
     * [fonction non disponible]
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const ;

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute s comme surface incidente
     *
     *  \param s un pointeur sur une surface
     */
#ifndef SWIG
    void add(Surface* s);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime s de la liste des surfaces incidentes. Si s n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param s un pointeur sur une surface
     */
#ifndef SWIG
    void remove(Surface* s);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute à la liste les entités de niveaux inférieurs
     * (surfaces, courbes et sommets), sans tri ni suppression des doublons
     *
     *  \param liste d'entity
     */
#ifndef SWIG
    virtual void addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   retourne un point sur l'objet au centre si possible
     * \author Eric Brière de l'Isle
     */
    virtual Utils::Math::Point getCenteredPosition() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGeomVolume;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::GeomVolume;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Vol";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Volume suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Ajoute le groupe parmis ceux auxquels appartient le volume */
#ifndef SWIG
    void add(Group::Group3D* grp);
#endif

    /** Retire le groupe parmis ceux auxquels appartient le volume */
#ifndef SWIG
    void remove(Group::Group3D* grp);
#endif

    /** Recherche le groupe parmis ceux auxquels appartient le volume
     * return vrai si trouvé */
#ifndef SWIG
    bool find(Group::Group3D* grp);
#endif

    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

    /// Retourne la liste des groupes 3D auxquels appartient cette entité
    virtual const std::vector<Group::Group3D*>& getGroups() const {return m_groups;}

    /// Affecte une nouvelle liste des groupes auxquels appartient cette entité
    virtual void setGroups(std::vector<Group::GroupEntity*>& grp);

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet
     */
#ifndef SWIG
    virtual void setDestroyed(bool b);
#endif

	/*------------------------------------------------------------------------*/
	/** \brief	Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
	 * \return	Description, à détruire par l'appelant.
	 */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  MAJ de l'état interne de l'objet géométrique spécifique au type
     *          d'entités géométriques (classes filles donc). Cette méthode est
     *          appelée par setMemento(...)
     */
    virtual void setFromSpecificMemento(MementoGeomEntity& mem);

    /*------------------------------------------------------------------------*/
    /** \brief  récupération de l'état interne de l'objet géométrique
     *          spécifique au type d'entités géométriques (classes filles
     *          donc).
     */
    virtual void createSpecificMemento(MementoGeomEntity& mem);
protected:

    /** surfaces incidentes au volume */
    std::vector<Surface*> m_surfaces;

    /// Listes des groupes 3D auxquels appartient ce volume
    std::vector<Group::Group3D*> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_VOLUME_H_ */
/*----------------------------------------------------------------------------*/
