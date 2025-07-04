/*----------------------------------------------------------------------------*/
/** \file GroupManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUPMANAGER_H_
#define MGX3D_GROUP_GROUPMANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/SelectionManager.h"
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResult.h"
#include "Geom/GeomEntity.h"
#include "Mesh/MeshEntity.h"
#include "Topo/TopoEntity.h"
#include "SysCoord/SysCoord.h"
/*----------------------------------------------------------------------------*/
#include <vector>
#include <map>
/*----------------------------------------------------------------------------*/
#include <Python.h>
#include <sys/types.h>           // uint sur Bull
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
class Entity;
}
namespace Internal {
class Context;
class InfoCommand;
}
namespace Topo {
class TopoEntity;
class Block;
class Vertex;
class Edge;
class CoEdge;
class Face;
class CoFace;
}
namespace Geom {
class GeomEntity;
}
namespace Mesh {
class MeshEntity;
class SurfacicSmoothing;
class VolumicSmoothing;
class MeshModificationBySepa;
}
namespace SysCoord {
class SysCoord;
}
/*----------------------------------------------------------------------------*/
namespace Group {

class Group3D;
class Group2D;
class Group1D;
class Group0D;
class GroupEntity;

/*----------------------------------------------------------------------------*/
/**
 * \class GroupManager
 * \brief Gestionnaire des opérations effectuées au niveau des groupes
 *
 */
class GroupManager final : public Internal::CommandCreator {
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    GroupManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    ~GroupManager();

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /** Réinitialisation     */
    void clear();
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   Activativation ou non de la propagation
     *
     *  L'affichage des entités se fait suivant un sytème de filtres
     *  La propagation (activée par défaut) permet l'affichage des entités
     *  de niveaux inférieurs.
     *
     *  Par exemple, si l'on active l'affichage d'un groupe 3D et des surfaces,
     *  alors on aura l'affichage des surfaces associées aux volumes de ce groupe 3D.
     *
     *  Sans propagation, on pourra avoir seulement l'affichage des volumes du groupe,
     *  en ayant activer le groupe 3D et les volumes.
     *
     */
    bool getPropagate();

    /*------------------------------------------------------------------------*/
    /** Retourne une string avec les informations relatives à l'entité */
    std::string getInfos(const std::string& name, int dim) const;

    /*------------------------------------------------------------------------*/
    /// retourne un nom par défaut pour les entités qui n'en aurait pas
    std::string getDefaultName(int dim) const;

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    Group3D* getGroup3D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /// retourne l'entité à partir du nom, et la créé si elle n'existe pas
    Group3D* getNewGroup3D(const std::string& name, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    Group2D* getGroup2D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /// retourne l'entité à partir du nom, et la créé si elle n'existe pas
    Group2D* getNewGroup2D(const std::string& name, Internal::InfoCommand* icmd);

     /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    Group1D* getGroup1D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /// retourne l'entité à partir du nom, et la créé si elle n'existe pas
    Group1D* getNewGroup1D(const std::string& name, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// retourne l'entité à partir du nom, une exception si elle n'existe pas
    Group0D* getGroup0D(const std::string& name, const bool exceptionIfNotFound=true) const;

    /// retourne l'entité à partir du nom, et la créé si elle n'existe pas
    Group0D* getNewGroup0D(const std::string& name, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// retourne la liste des groupes 3D, sans ou avec ceux détruits
    void getGroup3D(std::vector<Group3D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes 2D, sans ou avec ceux détruits
    void getGroup2D(std::vector<Group2D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes 1D, sans ou avec ceux détruits
    void getGroup1D(std::vector<Group1D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes 0D, sans ou avec ceux détruits
    void getGroup0D(std::vector<Group0D*>& grp, const bool onlyLive=true) const;

    /// retourne la liste des groupes de dimensions dims, sans ou avec ceux détruits
    void getGroups (std::vector<GroupEntity*>& grp, Mgx3D::Utils::SelectionManager::DIM dims, const bool onlyLive=true) const;

    /*------------------------------------------------------------------------*/
    /** Fonction qui à partir d'un filtre (stocké) et des ensembles des groupes qui
     * sont rendus visibles et invisibles,
     * renseigne sur les entités à rendre visible ou invisible pour
     * la géométrie, la topologie et le maillage
     */
    void getAddedShownAndHidden(
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
    void getAddedShownAndHidden(
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
    void getShownEntities(
		const Utils::FilterEntity::objectType visibilityMask,
		std::vector<Utils::Entity*>& entities);

    /** Met à jour les DisplayProperties
     *
     * Pour le moment, se contente de modifier isDisplayed()
     * pour savoir si une entité est à afficher ou non
     */
    void updateDisplayProperties(Internal::InfoCommand* icmd);


    /** Repère les groupes vides et les détruits
     */
    void updateDeletedGroups(Internal::InfoCommand* icmd);

    /** Destruction d'un groupe et suppression du vecteur
     *  Nécessaire pour préserver l'unicité des uid
     */
    void deleteEntity(GroupEntity* ge);

    /*------------------------------------------------------------------------*/
    /** Fonction qui à partir de tous les groupes du manager,
     * parcours les entités non vues et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMarkAllGroups(const Utils::FilterEntity::objectType visibilityMask,
            std::map<Geom::GeomEntity*, uint>& filtre_geom,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
			std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
            uint mark);

    /** Fonction qui à partir d'un vecteur de groupes,
     * parcours les entités non vues et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(const std::vector<Group::GroupEntity*>& groups,
                const Utils::FilterEntity::objectType visibilityMask,
                std::map<Geom::GeomEntity*, uint>& filtre_geom,
                std::map<Topo::TopoEntity*, uint>& filtre_topo,
				std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
                uint mark);

    /** Fonction qui à partir d'un vecteur de groupes,
     * recherche les entités de maillage accessibles via le masque
     * et les ajoutes au vecteur résultant
     */
    void addMeshGroups(const std::vector<Group::GroupEntity*>& groups,
            const Utils::FilterEntity::objectType visibilityMask,
            std::vector<Mesh::MeshEntity*>& meshAdded);

    /** Fonction qui à partir de l'ensemble des groupes
     * recherche les entités de maillage accessibles via le masque
     * et les ajoutes au vecteur résultant
     */
    void addMeshGroups(const Utils::FilterEntity::objectType visibilityMask,
            std::vector<Mesh::MeshEntity*>& meshAdded);

    /*------------------------------------------------------------------------*/
    /** Fonction qui à partir d'un Group3D, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Group3D* grp,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Geom::GeomEntity*, uint>& filtre_geom,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
			std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
            uint mark);

    /** Fonction qui à partir d'un Group2D, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Group2D* grp,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Geom::GeomEntity*, uint>& filtre_geom,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'un Group1D, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Group1D* grp,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Geom::GeomEntity*, uint>& filtre_geom,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'un Group0D, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Group0D* grp,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Geom::GeomEntity*, uint>& filtre_geom,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'un bloc, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Topo::Block* blk,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'une face, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Topo::Face* face,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'une face commune, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Topo::CoFace* coface,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'une arête, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Topo::Edge* edge,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'une arête commune, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Topo::CoEdge* coedge,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'un sommet, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(Topo::Vertex* vertex,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<Topo::TopoEntity*, uint>& filtre_topo,
            uint mark);

    /** Fonction qui à partir d'un repère, parcours les entités non vues
     * et accessibles via le masque
     * pour ajouter aux différents filtres la marque (mark)
     */
    void addMark(CoordinateSystem::SysCoord* rep,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
            uint mark);

    /*------------------------------------------------------------------------*/
    /** Retourne les volumes géométriques à partir des groupes sélectionnés   */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Volume*>& volumes);

    /*------------------------------------------------------------------------*/
    /** Retourne les surfaces géométriques à partir des groupes sélectionnés  */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Surface*>& surfaces);

    /*------------------------------------------------------------------------*/
    /** Retourne les courbes géométriques à partir des groupes sélectionnés   */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Curve*>& curves);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets géométriques à partir des groupes sélectionnés   */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** Retourne les blocs topologiques à partir des groupes sélectionnés     */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Topo::Block*>& blocks);

    /*-----------------------------------------------------------------------------*/
    /** Retourne les faces communes topologiques à partir des groupes sélectionnés */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Topo::CoFace*>& cofaces);

    /*------------------------------------------------------------------------------*/
    /** Retourne les arêtes communes topologiques à partir des groupes sélectionnés */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Topo::CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets topologiques à partir des groupes sélectionnés   */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** Retourne les volumes de mailles à partir des groupes sélectionnés     */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Volume*>& volumes);

    /*------------------------------------------------------------------------*/
    /** Retourne les surfaces de mailles à partir des groupes sélectionnés    */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Surface*>& surfaces);

    /*------------------------------------------------------------------------*/
    /** Retourne les lignes de bras à partir des groupes sélectionnés         */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Line*>& lines);

    /*------------------------------------------------------------------------*/
    /** Retourne les nuages de noeuds à partir des groupes sélectionnés       */
    void get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Cloud*>& clouds);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des groupes visibles
     */
    std::vector<GroupEntity*> getVisibles() const;
#endif

    /*------------------------------------------------------------------------*/
    /** Retourne les volumes géométriques du groupe g de dimension d          */
    std::vector<std::string> getGeomVolumes(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les surfaces géométriques du groupe g de dimension d         */
    std::vector<std::string> getGeomSurfaces(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les courbes géométriques du groupe g de dimension d          */
    std::vector<std::string> getGeomCurves(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets géométriques du groupe g de dimension d          */
    std::vector<std::string> getGeomVertices(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les blocs topologiques du groupe g de dimension d            */
    std::vector<std::string> getTopoBlocks(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les faces communes topologiques du groupe g de dimension d   */
    std::vector<std::string> getTopoFaces(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les arêtes communes topologiques du groupe g de dimension d  */
    std::vector<std::string> getTopoEdges(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Retourne les sommets topologiques du groupe g de dimension d          */
    std::vector<std::string> getTopoVertices(const std::string& g, const int d);

    /*------------------------------------------------------------------------*/
    /** Ajoute une projection sur plan X=0 du maillage pour un groupe 2D
     *
     *  \param nom   nom du groupe avec lequel on effectue la projection
     */
    void addProjectionOnPX0(const std::string& nom);

    /** Ajoute une projection sur plan Y=0 du maillage pour un groupe 2D
     *
     *  \param nom   nom du groupe avec lequel on effectue la projection
     */
    void addProjectionOnPY0(const std::string& nom);

    /** Ajoute une projection sur plan Z=0 du maillage pour un groupe 2D
     *
     *  \param nom   nom du groupe avec lequel on effectue la projection
     */
    void addProjectionOnPZ0(const std::string& nom);

  /** Ajoute une modification du maillage pour un groupe (2D ou 3D)
     *
     *  Il peut s'agir d'une perturbation sous forme de fonction Python
     *  Les coordonnées sont cartésiennes
     *
     *  \param nom       nom du groupe avec lequel on effectue la perturbation
     *  \param py_obj    l'objet python auquel il est fait appel pour modifier le maillage
     */
    void addCartesianPerturbation(const std::string& nom, PyObject* py_obj);

    /*------------------------------------------------------------------------*/
    /** Ajoute une modification du maillage pour un groupe (2D ou 3D)
     *
     *  Il peut s'agir d'une perturbation sous forme de fonction Python
     *  Les coordonnées sont polaires
     *
     *  \param nom       nom du groupe avec lequel on effectue la perturbation
     *  \param py_obj    l'objet python auquel il est fait appel pour modifier le maillage
     */
    void addPolarPerturbation(const std::string& nom, PyObject* py_obj);

    /*------------------------------------------------------------------------*/
    /** Ajoute un lissage surfacique du maillage pour un groupe 2D
     *
     * \param nom       nom du groupe 2D avec lequel on effectue le lissage
     * \param sm        la classe qui défini le lissage surfacique
     */
    void addSmoothing(const std::string& nom, Mesh::SurfacicSmoothing& sm);

    /*------------------------------------------------------------------------*/
    /** Ajoute un lissage volumique du maillage pour un groupe 3D
     *
     * \param nom       nom du groupe 3D avec lequel on effectue le lissage
     * \param sm        la classe qui défini le lissage volumique
     */
    void addSmoothing(const std::string& nom, Mesh::VolumicSmoothing& sm);

    /*------------------------------------------------------------------------*/
    /** Ajoute une séparatrice pour un groupe 2D
     *
     * \param nom       nom du groupe 2D sur lequel on applique la séparatrice
     * \param ASepa		la séparatrice
     */
    void addSepa(const std::string& nom, Mesh::MeshModificationBySepa& ASepa);

     /** affecte un niveau pour classer les groupes
      *  \param vg la liste des noms de groupes
      *  \param dim la dimension des groupes
      *  \param level le niveau que l'on affecte aux groupes
      */
     void setLevel(std::vector<std::string>& vg, int dim, int level);

     /*------------------------------------------------------------------------*/
     /** Vide un groupe suivant son nom et une dimension */
     Internal::M3DCommandResult* clearGroup(int dim, const std::string& groupName);


private:
    /// Retourne le groupe correspondant à la dimension
    GroupEntity* getGroup(const std::string& name, const int dim) const;

    /// Conteneur pour les groupes 3D
    std::vector<Group3D*> m_group3D;

    /// Conteneur pour les groupes 2D
    std::vector<Group2D*> m_group2D;

    /// Conteneur pour les groupes 1D
    std::vector<Group1D*> m_group1D;

    /// Conteneur pour les groupes 0D
    std::vector<Group0D*> m_group0D;

    /// ancien masque
    Utils::FilterEntity::objectType m_visibilityMask;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
