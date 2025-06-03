#ifndef MGX3D_MEMENTOENTITY_H_
#define MGX3D_MEMENTOENTITY_H_
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo
{
    class TopoEntity;
}

namespace Group
{
    class Group0D;
    class Group1D;
    class Group2D;
    class Group3D;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomProperty;
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class MementoEntity
 * \brief Classe permettant le stockage des informations caractérisant une
 *        entité géométrique dans le cadre du undo/redo.
 *
 *        Par défaut, le système de commandes de Mgx3D permet de stocker les
 *        entités de Magix3D qui ont été supprimées, modifiées et/ou ajoutées.
 *        Dans le cas d'entités que l'on modifie, c'est de leur responsabilité
 *        de stocker leurs "modifications". La classe MementoEntity joue
 *        ce rôle pour les entités géométriques.
 *
 *        Dans le cadre du pattern Mémento, la classe MementoEntity joue
 *        le rôle de Mémento, les classes filles de GeomEntity jouent le rôle
 *        de créateurs et les classes de Commandes géométriques ayant besoin
 *        de ce type d'informations de sauvegarde jouent le rôle de Demandeur
 *        et de Gardien des mémentos.
 *
 *        Pour le moment, une seule classe de Mémento géométrique est mise en
 *        place (celle-ci). Il est possible qu'il soit nécessaire de
 *        spécialiser le mémento pour chaque type d'entités géométriques
 *        (sommets, courbes, surfaces, volumes)
 *
 */
class MementoEntity
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
	MementoEntity()
: m_property(0)
{;}

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~MementoEntity(){;}

    inline std::vector<Topo::TopoEntity* >&
    getTopoEntities()  {
        return m_topo_entities;
    }
    inline void  setTopoEntities(std::vector<Topo::TopoEntity* >& topoEntities){
        m_topo_entities = topoEntities;
    }

    inline std::vector<Volume*>& getVolumes() {
        return m_volumes;
    }
    inline void setVolumes(std::vector<Volume*>& entities) {
        m_volumes = entities;
    }

    inline std::vector<Surface*>& getSurfaces() {
        return m_surfaces;
    }
    inline void setSurfaces(std::vector<Surface*>& entities) {
        m_surfaces = entities;
    }

    inline std::vector<Curve*>& getCurves() {
        return m_curves;
    }
    inline void setCurves(std::vector<Curve*>& entities) {
        m_curves = entities;
    }

    inline std::vector<Vertex*>& getVertices() {
        return m_vertices;
    }
    inline void setVertices(std::vector<Vertex*>& entities) {
        m_vertices = entities;
    }

    inline std::vector<Group::Group0D*>& getGroups0D() {
        return m_groups0D;
    }
    inline void setGroups0D(std::vector<Group::Group0D*>& grps) {
        m_groups0D = grps;
    }

    inline std::vector<Group::Group1D*>& getGroups1D() {
        return m_groups1D;
    }
    inline void setGroups1D(std::vector<Group::Group1D*>& grps) {
        m_groups1D = grps;
    }

    inline std::vector<Group::Group2D*>& getGroups2D() {
        return m_groups2D;
    }
    inline void setGroups2D(std::vector<Group::Group2D*>& grps) {
        m_groups2D = grps;
    }

    inline std::vector<Group::Group3D*>& getGroups3D() {
        return m_groups3D;
    }
    inline void setGroups3D(std::vector<Group::Group3D*>& grps) {
        m_groups3D = grps;
    }

    inline std::vector<TopoDS_Shape>& getOCCShapes() {
        return m_shapes;
    }
    inline void setOCCShapes(const std::vector<TopoDS_Shape>& g) {
        m_shapes = g;
    }

    inline GeomProperty* getProperty() {
        return m_property;
    }
    inline void setProperty(GeomProperty* g) {
        m_property = g;
    }
private :

    /// liens vers la topologie
    std::vector<Topo::TopoEntity* > m_topo_entities;

    std::vector<Volume*>    m_volumes;
    std::vector<Surface*>   m_surfaces;
    std::vector<Curve*>     m_curves;
    std::vector<Vertex*>    m_vertices;

    /// Listes des groupes 1D auxquels appartient cette courbe
    std::vector<Group::Group0D*> m_groups0D;
    std::vector<Group::Group1D*> m_groups1D;
    std::vector<Group::Group2D*> m_groups2D;
    std::vector<Group::Group3D*> m_groups3D;

    /* ancienne représentation géométrique des entités que l'on a modifiées */
    std::vector<TopoDS_Shape> m_shapes;

    /* ancienne propriétés des entités que l'on a modifiées. Il
     * est de la responsabilité de l'objet de les détruire */
    GeomProperty*           m_property;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MEMENTOENTITY_H_ */
/*----------------------------------------------------------------------------*/
