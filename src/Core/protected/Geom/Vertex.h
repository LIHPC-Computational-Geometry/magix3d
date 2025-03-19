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
#include <TopoDS_Vertex.hxx>
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

    friend class MementoManager;
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
            GeomProperty* gprop, TopoDS_Vertex& shape);
#endif
    const TopoDS_Vertex& getOCCVertex() const { return m_occ_vertex; }

    virtual void apply(std::function<void(const TopoDS_Shape&)> const& lambda) const;
    virtual void applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda);
    virtual void accept(ConstGeomEntityVisitor& v) const { v.visit(this); }
    virtual void accept(GeomEntityVisitor& v) { v.visit(this); }

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&);

      /// Destructeur
    virtual ~Vertex();

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
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    virtual void computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux courbes géométriques incidentes
     *
     *  \param curves les courbes incidents
     */
    const std::vector<Curve*>& getCurves() const {return m_curves;}

    /*------------------------------------------------------------------------*/
    /// retourne la liste des sommets topologiques qui pointent sur cette courbe
    virtual void get(std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur le sommet. P est modifié
     *  \param P le point à projeter
     */
    virtual uint project(Utils::Math::Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur le sommet, le résultat est le point P2.
     */
    virtual uint project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const ;

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

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual const std::vector<Group::Group0D*>& getGroups() const {return m_groups;}

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

private:
    /// accès aux courbes incidentes
    std::vector<Curve*> m_curves;
    /// Listes des groupes 0D auxquels appartient ce sommet
    std::vector<Group::Group0D*> m_groups;
    /// représentation open cascade
    TopoDS_Vertex m_occ_vertex;
};
/*----------------------------------------------------------------------------*/

} // end namespace Geom

} // end namespace Mgx3D

#endif /* MGX3D_GEOM_VERTEX_H_ */
