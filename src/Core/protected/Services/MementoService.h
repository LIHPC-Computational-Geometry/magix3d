#ifndef _MEMENTO_SERVICE_H_
#define _MEMENTO_SERVICE_H_

#include <map>
#include <vector>
#include <TopoDS_Shape.hxx>

namespace Mgx3D
{
    namespace Geom
    {
        class GeomProperty;
        class GeomEntity;
        class Vertex;
        class Curve;
        class Surface;
        class Volume;
    }

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
}

namespace Mgx3D::Services
{
    /*----------------------------------------------------------------------------*/
    /**
     * \class Memento
     * \brief Classe permettant le stockage des informations caractérisant une
     *        entité géométrique dans le cadre du undo/redo.
     *
     *        Par défaut, le système de commandes de Mgx3D permet de stocker les
     *        entités de Magix3D qui ont été supprimées, modifiées et/ou ajoutées.
     *        Dans le cas d'entités que l'on modifie, c'est de leur responsabilité
     *        de stocker leurs "modifications". La classe Memento joue
     *        ce rôle pour les entités géométriques.
     *
     *        Dans le cadre du pattern Mémento, la classe Memento joue
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
    struct Memento
    {
        std::vector<Topo::TopoEntity *> topo_entities;
        std::vector<Geom::Volume *> volumes;
        std::vector<Geom::Surface *> surfaces;
        std::vector<Geom::Curve *> curves;
        std::vector<Geom::Vertex *> vertices;

        /// Listes des groupes 1D auxquels appartient cette courbe
        std::vector<Group::Group0D *> groups0D;
        std::vector<Group::Group1D *> groups1D;
        std::vector<Group::Group2D *> groups2D;
        std::vector<Group::Group3D *> groups3D;

        /* ancienne représentation géométrique des entités que l'on a modifiées */
        std::vector<TopoDS_Shape> occ_shapes;

        /* ancienne propriétés des entités que l'on a modifiées. Il
         * est de la responsabilité de l'objet de les détruire */
        Geom::GeomProperty *property = nullptr;
    };

    class MementoService
    {
    public:
        const Memento createMemento(const Geom::GeomEntity *e) const;
        void saveMemento(Geom::GeomEntity *e, const Memento &mem);
        void saveMemento(Geom::GeomEntity *e) { saveMemento(e, createMemento(e)); }
        void permMementos();

    private:
        void setFromMemento(Geom::GeomEntity *e, const Memento &mem);

        /** pour les entitiés géométriques modifiées lors de l'opération, on
         * stocke leur "état interne" sous forme de mémento. Ce stockage est de la
         * responsabilité de chaque entité */
        std::map<Geom::GeomEntity *, Memento> m_mementos;
    };
}

#endif /* _MEMENTO_SERVICE_H_ */
