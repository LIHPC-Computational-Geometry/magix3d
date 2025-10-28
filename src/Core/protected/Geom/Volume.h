#ifndef MGX3D_GEOM_VOLUME_H_
#define MGX3D_GEOM_VOLUME_H_
#include "Geom/GeomEntity.h"
#include "Services/MementoService.h"
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class Volume
 * \brief Interface définissant les services que doit fournir tout volume
 *        géométrique
 */
class Volume: public GeomEntity {
    friend class Services::MementoService;
    friend class GeomModificationBaseClass;

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
    Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, TopoDS_Shape& shape);

    const TopoDS_Shape& getOCCShape() const { return m_occ_shape; }

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
    virtual ~Volume();

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    double computeArea() const override;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    void computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const override;

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    int getDim() const override { return 3; }

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux surfaces géométriques incidentes
     *
     *  \param surfaces les surfaces incidentes
     */
    const std::vector<Surface*>& getSurfaces() const {return m_surfaces;}

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute s comme surface incidente
     *
     *  \param s un pointeur sur une surface
     */
    void add(Surface* s);

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime s de la liste des surfaces incidentes. Si s n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param s un pointeur sur une surface
     */
    void remove(Surface* s);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute à la liste les entités de niveaux inférieurs
     * (surfaces, courbes et sommets), sans tri ni suppression des doublons
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
    Utils::Entity::objectType getType() const { return Utils::Entity::GeomVolume; }

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Vol";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Volume suivant son nom
     */
    static bool isA(const std::string& name);

private:
    /// surfaces incidentes au volume
    std::vector<Surface*> m_surfaces;
    /// représentation open cascade
    TopoDS_Shape m_occ_shape;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_VOLUME_H_ */
/*----------------------------------------------------------------------------*/
