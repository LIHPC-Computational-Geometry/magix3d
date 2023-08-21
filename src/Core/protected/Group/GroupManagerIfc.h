/*----------------------------------------------------------------------------*/
/** \file GroupManagerIfc.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUPMANAGER_IFC_H_
#define MGX3D_GROUP_GROUPMANAGER_IFC_H_
/*----------------------------------------------------------------------------*/
#include <Python.h>
#include <vector>
#include "Utils/SelectionManagerIfc.h"
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Geom/GeomEntity.h"
#include "Mesh/MeshEntity.h"
#include "Topo/TopoEntity.h"
#include "SysCoord/SysCoord.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class Context;
}

namespace Utils {
namespace Math {
class Point;
class Vector;
class Rotation;
}
}
using Mgx3D::Utils::Math::Point;
using Mgx3D::Utils::Math::Vector;
using Mgx3D::Utils::Math::Rotation;

namespace Mesh {
class SurfacicSmoothing;
class VolumicSmoothing;
class MeshModificationBySepa;
}

namespace Geom {
class Volume;
class Surface;
class Curve;
class Vertex;
}

namespace Topo {
class Block;
class CoFace;
class CoEdge;
class Vertex;
}

namespace SysCoord {
class SysCoord;
}

/*----------------------------------------------------------------------------*/
namespace Group {

class Group0D;
class Group1D;
class Group2D;
class Group3D;

/*----------------------------------------------------------------------------*/
/**
 * \class GroupManagerIfc
 * \brief Interface de gestionnaire des opérations effectuées au niveau des groupes
 *
 */
class GroupManagerIfc : public Internal::CommandCreator{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    GroupManagerIfc(const std::string& name, Internal::ContextIfc* c);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GroupManagerIfc();

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	virtual void clear();
#endif
    /*------------------------------------------------------------------------*/
    /** Retourne une string avec les informations relatives à l'entité */
    virtual std::string getInfos(const std::string& name, int dim) const;

    /*------------------------------------------------------------------------*/
    /** Retourne les entités géométriques à partir des groupes sélectionnées */
    virtual std::vector<std::string> getGeomEntities(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les volumes géométriques à partir des groupes sélectionnés   */
    virtual std::vector<std::string> getGeomVolumes(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les surfaces géométriques à partir des groupes sélectionnés  */
    virtual std::vector<std::string> getGeomSurfaces(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les courbes géométriques à partir des groupes sélectionnés   */
    virtual std::vector<std::string> getGeomCurves(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets géométriques à partir des groupes sélectionnés  */
    virtual std::vector<std::string> getGeomVertices(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les blocs topologiques à partir des groupes sélectionnés     */
    virtual std::vector<std::string> getTopoBlocks(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les faces communes topologiques à partir des groupes sélectionnés */
    virtual std::vector<std::string> getTopoFaces(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les arêtes communes topologiques à partir des groupes sélectionnés */
    virtual std::vector<std::string> getTopoEdges(const std::vector<std::string>& vg);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets topologiques à partir des groupes sélectionnés */
    virtual std::vector<std::string> getTopoVertices(const std::vector<std::string>& vg);


    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des groupes visibles */
    virtual std::vector<std::string> getVisibles() const;


    /*------------------------------------------------------------------------*/
    /** Ajoute une projection sur plan X=0 du maillage pour un groupe 2D
     *
     *  \param nom   nom du groupe avec lequel on effectue la projection
     */
    virtual void addProjectionOnPX0(const std::string& nom);

    /** Ajoute une projection sur plan Y=0 du maillage pour un groupe 2D
     *
     *  \param nom   nom du groupe avec lequel on effectue la projection
     */
    virtual void addProjectionOnPY0(const std::string& nom);

    /** Ajoute une projection sur plan Z=0 du maillage pour un groupe 2D
     *
     *  \param nom   nom du groupe avec lequel on effectue la projection
     */
    virtual void addProjectionOnPZ0(const std::string& nom);


    /** Ajoute une modification du maillage pour un groupe (2D ou 3D)
     *
     *  Il peut s'agir d'une perturbation sous forme de fonction Python
     *  Les coordonnées sont cartésiennes
     *
     *  \param nom       nom du groupe avec lequel on effectue la perturbation
     *  \param py_obj    l'objet python auquel il est fait appel pour modifier le maillage
     */
    virtual void addCartesianPerturbation(const std::string& nom, PyObject* py_obj);

    /** Ajoute une modification du maillage pour un groupe (2D ou 3D)
     *
     *  Il peut s'agir d'une perturbation sous forme de fonction Python
     *  Les coordonnées sont polaires
     *
     *  \param nom       nom du groupe avec lequel on effectue la perturbation
     *  \param py_obj    l'objet python auquel il est fait appel pour modifier le maillage
     */
    virtual void addPolarPerturbation(const std::string& nom, PyObject* py_obj);

    /*------------------------------------------------------------------------*/
    /** Ajoute un lissage surfacique du maillage pour un groupe 2D
     *
     * \param nom       nom du groupe 2D avec lequel on effectue le lissage
     * \param sm        la classe qui défini le lissage surfacique
     */
    virtual void addSmoothing(const std::string& nom, Mesh::SurfacicSmoothing& sm);

    /*------------------------------------------------------------------------*/
    /** Ajoute un lissage volumique du maillage pour un groupe 3D
     *
     * \param nom       nom du groupe 3D avec lequel on effectue le lissage
     * \param sm        la classe qui défini le lissage volumique
     */
    virtual void addSmoothing(const std::string& nom, Mesh::VolumicSmoothing& sm);

    /*------------------------------------------------------------------------*/
    /** Ajoute une séparatrice pour un groupe 2D
     *
     * \param nom       nom du groupe 2D sur lequel on applique la séparatrice
     * \param ASepa		la séparatrice
     */
    virtual void addSepa(const std::string& nom, Mesh::MeshModificationBySepa& ASepa);

    /*------------------------------------------------------------------------*/
    /** Ajoute le contenu d'un ensemble géométrique à un groupe donné
     *  */
    virtual void addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);

    /** Enlève le contenu d'un ensemble géométrique à un groupe donné
     *  */
    virtual void removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);

    /** Défini le contenu d'un ensemble géométrique à un groupe donné
     *  */
    virtual void setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);

    /** affecte un niveau pour classer les groupes
     *  \param vg la liste des noms de groupes
     *  \param dim la dimension des groupes
     *  \param level le niveau que l'on affecte aux groupes
     */
    virtual void setLevel(std::vector<std::string>& vg, int dim, int level);

    /*------------------------------------------------------------------------*/
    /** Vide un groupe suivant son nom et une dimension */
    virtual Internal::M3DCommandResultIfc* clearGroup(int dim, const std::string& groupName);

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    virtual Group3D* getGroup3D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    virtual Group2D* getGroup2D(const std::string& name, const bool exceptionIfNotFound=true) const;

     /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    virtual Group1D* getGroup1D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    virtual Group0D* getGroup0D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /// retourne la liste des groupes 3D, sans ou avec ceux détruits
    virtual void getGroup3D(std::vector<Group3D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes 2D, sans ou avec ceux détruits
    virtual void getGroup2D(std::vector<Group2D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes 1D, sans ou avec ceux détruits
    virtual void getGroup1D(std::vector<Group1D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes 0D, sans ou avec ceux détruits
    virtual void getGroup0D(std::vector<Group0D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes de dimensions dims, sans ou avec ceux détruits
    virtual void getGroups (std::vector<GroupEntity*>& grp, Mgx3D::Utils::SelectionManagerIfc::DIM dims, const bool onlyLive=true) const;

    /*------------------------------------------------------------------------*/
    /** Fonction qui à partir d'un filtre (stocké) et des ensembles des groupes qui
     * sont rendus visibles et invisibles,
     * renseigne sur les entités à rendre visible ou invisible pour
     * la géométrie, la topologie et le maillage
     */
    virtual void getAddedShownAndHidden(
            const std::vector<Group::GroupEntity*>& groupAddedShown,
            const std::vector<Group::GroupEntity*>& groupAddedHidden,
            std::vector<Geom::GeomEntity*>& geomAddedShown,
            std::vector<Geom::GeomEntity*>& geomAddedHidden,
            std::vector<Topo::TopoEntity*>& topoAddedShown,
            std::vector<Topo::TopoEntity*>& topoAddedHidden,
            std::vector<Mesh::MeshEntity*>& meshAddedShown,
            std::vector<Mesh::MeshEntity*>& meshAddedHidden,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden);

    /** Fonction qui à partir d'un filtre initial (stocké) et d'un nouveau filtre
     * sur les entités visibles
     * renseigne sur les entités à rendre visible ou invisible pour
     * la géométrie, la topologie et le maillage
     */
    virtual void getAddedShownAndHidden(
            const Utils::FilterEntity::objectType newVisibilityMask,
            std::vector<Geom::GeomEntity*>& geomAddedShown,
            std::vector<Geom::GeomEntity*>& geomAddedHidden,
            std::vector<Topo::TopoEntity*>& topoAddedShown,
            std::vector<Topo::TopoEntity*>& topoAddedHidden,
            std::vector<Mesh::MeshEntity*>& meshAddedShown,
            std::vector<Mesh::MeshEntity*>& meshAddedHidden,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
			std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden);

	/** Fonction qui à partir d'un filtre donné en argument retourne
	 * la liste des entités visibles correspondantes.
	 */
	virtual void getShownEntities(
			const Utils::FilterEntity::objectType visibilityMask,
			std::vector<Utils::Entity*>& entities);

    /*------------------------------------------------------------------------*/
    /** Retourne les volumes géométriques à partir des groupes sélectionnés   */
    virtual void get(const std::vector<std::string>& vg, std::vector<Geom::Volume*>& volumes);

    /*------------------------------------------------------------------------*/
    /** Retourne les surfaces géométriques à partir des groupes sélectionnés  */
    virtual void get(const std::vector<std::string>& vg, std::vector<Geom::Surface*>& surfaces);

    /*------------------------------------------------------------------------*/
    /** Retourne les courbes géométriques à partir des groupes sélectionnés   */
    virtual void get(const std::vector<std::string>& vg, std::vector<Geom::Curve*>& curves);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets géométriques à partir des groupes sélectionnés  */
    virtual void get(const std::vector<std::string>& vg, std::vector<Geom::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** Retourne les blocs topologiques à partir des groupes sélectionnés     */
    virtual void get(const std::vector<std::string>& vg, std::vector<Topo::Block*>& blocks);

    /*------------------------------------------------------------------------*/
    /** Retourne les faces communes topologiques à partir des groupes sélectionnés */
    virtual void get(const std::vector<std::string>& vg, std::vector<Topo::CoFace*>& cofaces);

    /*------------------------------------------------------------------------*/
    /** Retourne les arêtes communes topologiques à partir des groupes sélectionnés */
    virtual void get(const std::vector<std::string>& vg, std::vector<Topo::CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets topologiques à partir des groupes sélectionnés */
    virtual void get(const std::vector<std::string>& vg, std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** Retourne les volumes de mailles à partir des groupes sélectionnés   */
    virtual void get(const std::vector<std::string>& vg, std::vector<Mesh::Volume*>& volumes);

    /*------------------------------------------------------------------------*/
    /** Retourne les surfaces de mailles à partir des groupes sélectionnés  */
    virtual void get(const std::vector<std::string>& vg, std::vector<Mesh::Surface*>& surfaces);

    /*------------------------------------------------------------------------*/
    /** Retourne les lignes de mailles à partir des groupes sélectionnés  */
    virtual void get(const std::vector<std::string>& vg, std::vector<Mesh::Line*>& lines);

    /*------------------------------------------------------------------------*/
    /** Retourne les nuages de mailles à partir des groupes sélectionnés  */
    virtual void get(const std::vector<std::string>& vg, std::vector<Mesh::Cloud*>& clouds);

#endif


};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUPMANAGER_IFC_H_ */
/*----------------------------------------------------------------------------*/
