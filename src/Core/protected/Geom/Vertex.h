/*----------------------------------------------------------------------------*/
/** \file Vertex.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_VERTEX_H_
#define MGX3D_GEOM_VERTEX_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group0D;
}
namespace Topo {
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
class Curve;
class Surface;
class Volume;
class GeomProperty;
/*----------------------------------------------------------------------------*/
/**
 * \class Vertex
 * \brief Sommet de la géométrie
 */
class Vertex: public GeomEntity {

    static const char* typeNameGeomVertex;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop    les propriétés associées à la courbe
     *  \param compProp les propriétés de calcul
     */
#ifndef SWIG
    Vertex(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, GeomRepresentation* compProp);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&);

      /// Destructeur
    virtual ~Vertex();

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
    /** \brief  retourne la dimension de l'entité géométrique
     */
    int getDim() const {return 0;}

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

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit un résumé textuel de l'entité.
     */
#ifndef SWIG
    virtual std::string getSummary ( ) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    double computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets géométriques incidents via une
     *          courbe
     *
     *  \param vertices les sommets incidents
     */
    virtual void get(std::vector<Vertex*>& vertices) const;

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
    /// retourne la liste des sommets topologiques qui pointent sur cette courbe
    virtual void get(std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur le sommet. P est modifié
     *  \param P le point à projeter
     */
    virtual void project(Utils::Math::Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur le sommet, le résultat est le point P2.
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const ;

    /*------------------------------------------------------------------------*/
    /** \brief  Return the number of curve incident to this point.
     *
     */
    virtual size_t getNbCurves() const{
        return m_curves.size();
    };

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute c comme courbe incidente
     *
     *  \param c un pointeur sur une courbe
     */
#ifndef SWIG
    void add(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime c de la liste des courbes incidentes. Si c n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    void remove(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   retourne un point sur l'objet au centre si possible
     */
    virtual Utils::Math::Point getCenteredPosition() const;

    /*------------------------------------------------------------------------*/
    /** \brief   retourne la position du Vertex
     */
    virtual Utils::Math::Point getCoord() const;


    inline Utils::Math::Point getPoint() const{
        return getCoord();
    }
/*------------------------------------------------------------------------*/
    /** \brief   retourne la coordonnée en X du sommet
     */
    double getX() const;
    /*------------------------------------------------------------------------*/
    /** \brief   retourne la coordonnée en Y du sommet
     */
    double getY() const;
    /*------------------------------------------------------------------------*/
    /** \brief   retourne la coordonnée en Z du sommet
     */
    double getZ() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGeomVertex;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::GeomVertex;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
     */
    static std::string getTinyName() {return "Pt";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Vertex suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** \brief Surcharge pour l'écriture dans les flux
     */
#ifndef SWIG
    friend std::ostream& operator<<(std::ostream& str, const Vertex& v);
    /*------------------------------------------------------------------------*/
    /** \brief Surcharge pour l'écriture dans les flux
     */
    friend TkUtil::UTF8String& operator<<(TkUtil::UTF8String& str, const Vertex& v);
#endif

    /*------------------------------------------------------------------------*/
    /** Ajoute le groupe parmis ceux auxquels appartient le sommet */
#ifndef SWIG
    void add(Group::Group0D* grp);
#endif

    /** Retire le groupe parmis ceux auxquels appartient le sommet */
#ifndef SWIG
    void remove(Group::Group0D* grp);
#endif

    /** Recherche le groupe parmis ceux auxquels appartient le sommet
     * return vrai si trouvé */
#ifndef SWIG
    bool find(Group::Group0D* grp);
#endif

    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

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
private:

    /// accès aux courbes incidentes
    std::vector<Curve*> m_curves;

    /// Listes des groupes 0D auxquels appartient ce sommet
    std::vector<Group::Group0D*> m_groups;
};
/*----------------------------------------------------------------------------*/

} // end namespace Geom

} // end namespace Mgx3D

#endif /* MGX3D_GEOM_VERTEX_H_ */
