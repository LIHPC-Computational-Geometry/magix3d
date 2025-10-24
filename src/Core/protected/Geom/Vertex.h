#ifndef MGX3D_GEOM_VERTEX_H_
#define MGX3D_GEOM_VERTEX_H_
/*----------------------------------------------------------------------------*/
#include <TopoDS_Vertex.hxx>
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Services/MementoService.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
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

    friend class Services::MementoService;
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
    Vertex(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, TopoDS_Vertex& shape);

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
    /** \brief  Fournit un résumé textuel de l'entité.
     */
    virtual std::string getSummary ( ) const;

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
    /** \brief  Ajoute c comme courbe incidente
     *
     *  \param c un pointeur sur une courbe
     */
    void add(Curve* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime c de la liste des courbes incidentes. Si c n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
    void remove(Curve* c);

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
    friend std::ostream& operator<<(std::ostream& str, const Vertex& v);
    /*------------------------------------------------------------------------*/
    /** \brief Surcharge pour l'écriture dans les flux
     */
    friend TkUtil::UTF8String& operator<<(TkUtil::UTF8String& str, const Vertex& v);

private:
    /// accès aux courbes incidentes
    std::vector<Curve*> m_curves;
    /// représentation open cascade
    TopoDS_Vertex m_occ_vertex;
};
/*----------------------------------------------------------------------------*/

} // end namespace Geom

} // end namespace Mgx3D

#endif /* MGX3D_GEOM_VERTEX_H_ */
