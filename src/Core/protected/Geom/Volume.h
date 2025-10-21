#ifndef MGX3D_GEOM_VOLUME_H_
#define MGX3D_GEOM_VOLUME_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Services/MementoService.h"
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
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
    friend class Services::MementoService;
    friend class GeomModificationBaseClass;
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
    Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, TopoDS_Shape& shape);

    const TopoDS_Shape& getOCCShape() const { return m_occ_shape; }

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
    virtual ~Volume();

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
    /** \brief  retourne la dimension de l'entité géométrique
     */
    int getDim() const {return 3;}

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux surfaces géométriques incidentes
     *
     *  \param surfaces les surfaces incidentes
     */
    virtual const std::vector<Surface*>& getSurfaces() const {return m_surfaces;}

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
    virtual void addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const;

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
    void add(Group::Group3D* grp);

    /** Retire le groupe parmis ceux auxquels appartient le volume */
    void remove(Group::Group3D* grp);

    /** Recherche le groupe parmis ceux auxquels appartient le volume
     * return vrai si trouvé */
    bool find(Group::Group3D* grp);

    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

    /// Retourne la liste des groupes 3D auxquels appartient cette entité
    virtual const std::vector<Group::Group3D*>& getGroups() const {return m_groups;}

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet
     */
    virtual void setDestroyed(bool b);

private:
    /// surfaces incidentes au volume
    std::vector<Surface*> m_surfaces;
    /// Listes des groupes 3D auxquels appartient ce volume
    std::vector<Group::Group3D*> m_groups;
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
