#ifndef MGX3D_GEOM_SURFACE_H_
#define MGX3D_GEOM_SURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Services/MementoService.h"
#include "Utils/Vector.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Face.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Curve;
class Volume;
/*----------------------------------------------------------------------------*/
/**
 * \class Surface
 * \brief Interface définissant les services que doit fournir toute surface
 *        géométrique
 */
class Surface: public GeomEntity {

    friend class Services::MementoService;

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
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, TopoDS_Face& shape);

    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop les propriétés associées à la surface composite
     *  \param shapes les shapes OCC
     */
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, std::vector<TopoDS_Face>& shapes);

    const std::vector<TopoDS_Face>& getOCCFaces() const { return m_occ_faces; }

    void apply(std::function<void(const TopoDS_Shape&)> const& lambda) const override;
    void applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda) override;
    void accept(ConstGeomEntityVisitor& v) const override { v.visit(this); }
    void accept(GeomEntityVisitor& v) override { v.visit(this); }

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    GeomEntity* clone(Internal::Context&) override;

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Surface() = default;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    void computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const override;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    double computeArea() const override;

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    int getDim() const  override { return 2; }

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la normale à une surface en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la normale recherchée suivant le projeté du point
     */
    void normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const;

     /*------------------------------------------------------------------------*/
     /** \brief Retourne le point de parametre (u,v) dans l'espace de parametre
      *         de la surface. Si (u,v) est choisi hors des bornes [U1,U2] x
      *         [V1,V2] retournées par getParametricBounds, une exception est
      *         levée. La robustesse de cette méthode repose sur OCC
      */
    Utils::Math::Point getPoint(const double u, const double v) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute v comme volume incident
     *
     *  \param v un pointeur sur un volume
     */
    void add(Volume* v);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute c comme courbe incidente
     *
     *  \param c un pointeur sur une courbe
     */
    void add(Curve* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime v de la liste des volumes incidents. Si v n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
    void remove(Volume* v);

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime c de la liste des courbes incidentes. Si c n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
    void remove(Curve* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute à la liste les entités de niveaux inférieurs
     * (courbes et sommets), sans tri ni suppression des doublons
     *
     *  \param liste d'entity
     */
    void addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const;

    /*------------------------------------------------------------------------*/
    /** \brief   retourne un point sur l'objet au centre si possible
     * \author Eric Brière de l'Isle
     */
    Utils::Math::Point getCenteredPosition() const override;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    Utils::Entity::objectType getType() const override { return Utils::Entity::GeomSurface; }

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Surf";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Surface suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** \brief  Return the curves incident to this surface
     */
    const std::vector<Curve*>& getCurves() const { return m_curves; }

    /*------------------------------------------------------------------------*/
    /** \brief  Return the volumes incident to this surface
     */
    const std::vector<Volume*>& getVolumes() const { return m_volumes; } 

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la surface est un plan ou pas
     */
    bool isPlanar() const;

private:
    std::vector<Curve*> m_curves;
    std::vector<Volume*> m_volumes;
    /// représentation open cascade
    std::vector<TopoDS_Face> m_occ_faces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_SURFACE_H_ */
/*----------------------------------------------------------------------------*/

