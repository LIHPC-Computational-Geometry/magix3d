/*----------------------------------------------------------------------------*/
/** \file Surface.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_SURFACE_H_
#define MGX3D_GEOM_SURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Face.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group2D;
}

namespace Topo {
class CoFace;
class CoEdge;
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Loop;
class Curve;
class Volume;
class Vertex;
class GeomProperty;
/*----------------------------------------------------------------------------*/
/**
 * \class Surface
 * \brief Interface définissant les services que doit fournir toute surface
 *        géométrique
 */
class Surface: public GeomEntity {

    friend class MementoManager;
    static const char* typeNameGeomSurface;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop les propriétés associées à la surface
     *  \param shape la shape OCC
     */
#ifndef SWIG
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, TopoDS_Face& shape);
#endif

    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop les propriétés associées à la surface composite
     *  \param shapes les shapes OCC
     */
#ifndef SWIG
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, std::vector<TopoDS_Face>& shapes);
#endif
    const std::vector<TopoDS_Face>& getOCCFaces() const { return m_occ_faces; }

    virtual void apply(std::function<void(const TopoDS_Shape&)> const& lambda) const;
    virtual void applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda);
    virtual void accept(ConstGeomEntityVisitor& v) const { v.visit(this); }
    virtual void accept(GeomEntityVisitor& v) { v.visit(this); }

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Surface();

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    virtual void computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    virtual int getDim() const {return 2;}

    /*------------------------------------------------------------------------*/
    /** \brief Vérifie que la surface ASurf est contenue dans *this
     *  \param ASurf la surface contenue
     *  \return true si elle est contenue, false sinon
     */
    virtual bool contains(Surface* ASurf) const;

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la normale à une surface en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la normale recherchée suivant le projeté du point
     */
    virtual void normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const;

     /*------------------------------------------------------------------------*/
     /** \brief Retourne le point de parametre (u,v) dans l'espace de parametre
      *         de la surface. Si (u,v) est choisi hors des bornes [U1,U2] x
      *         [V1,V2] retournées par getParametricBounds, une exception est
      *         levée. La robustesse de cette méthode repose sur OCC
      */
    virtual Utils::Math::Point getPoint(const double u, const double v) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur la surface. P est modifié
     *  \param P le point à projeter
     */
    virtual uint project(Utils::Math::Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur la surface, le résultat est le point P2.
     */
    virtual uint project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute v comme volume incident
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
      virtual void add(Volume* v);

#endif


#ifndef SWIG
    virtual void add(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime v de la liste des volumes incidents. Si v n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    virtual void remove(Volume* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime c de la liste des courbes incidentes. Si c n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    virtual void remove(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute à la liste les entités de niveaux inférieurs
     * (courbes et sommets), sans tri ni suppression des doublons
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
    virtual std::string getTypeName() const {return typeNameGeomSurface;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::GeomSurface;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Surf";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Surface suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Ajoute le groupe parmis ceux auxquels appartient la surface */
#ifndef SWIG
    virtual void add(Group::Group2D* grp);
#endif

    /** Retire le groupe parmis ceux auxquels appartient la surface */
#ifndef SWIG
    virtual void remove(Group::Group2D* grp);
#endif

    /** Recherche le groupe parmis ceux auxquels appartient la surface
     * return vrai si trouvé */
#ifndef SWIG
    virtual bool find(Group::Group2D* grp);
#endif

    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

    /// Retourne la liste des groupes 2D auxquels appartient cette entité
    virtual const std::vector<Group::Group2D*>& getGroups() const {return m_groups;}

    /// Affecte une nouvelle liste des groupes auxquels appartient cette entité
    virtual void setGroups(std::vector<Group::GroupEntity*>& grp);

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Return the curves incident to this surface
     */
    const std::vector<Curve*>& getCurves() const { return m_curves; }

    /*------------------------------------------------------------------------*/
    /** \brief  Return the volumes incident to this surface
     */
    const std::vector<Volume*>& getVolumes() const { return m_volumes; }    

    /*------------------------------------------------------------------------*/
    /// retourne la liste des faces topologiques communes qui pointent sur cette surface
    virtual void get(std::vector<Topo::CoFace*>& cofaces);

    /// retourne la liste des arêtes topologiques communes qui pointent sur cette surface
    virtual void get(std::vector<Topo::CoEdge*>& coedges);

    /// retourne la liste des sommets topologiques qui pointent sur cette surface
    virtual void get(std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet
     */
#ifndef SWIG
    virtual void setDestroyed(bool b);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la surface est un plan ou pas
     */
    virtual bool isPlanar() const;

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

private:
    std::vector<Curve*> m_curves;
    std::vector<Volume*> m_volumes;
    /// représentation open cascade
    std::vector<TopoDS_Face> m_occ_faces;
    /// Listes des groupes 2D auxquels appartient cette surface
    std::vector<Group::Group2D*> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_SURFACE_H_ */
/*----------------------------------------------------------------------------*/

