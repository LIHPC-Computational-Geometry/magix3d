	/*----------------------------------------------------------------------------*/
/** \file InfoCommand.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/12/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef INTERNAL_INFOCOMMAND_H_
#define INTERNAL_INFOCOMMAND_H_

#include <TkUtil/Mutex.h>
#include <sys/types.h>
#include <vector>
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class GeomEntity;
}
namespace Topo {
class TopoEntity;
}
namespace Mesh {
class MeshEntity;
}
namespace Group {
class GroupEntity;
}
namespace CoordinateSystem {
class SysCoord;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/**
   @brief Stocke les informations concernant une commande

   On y trouve les listes d'entités crées, modifiées et détruites.
   On y distingue chacun des grands types (Geom, Topo, Mesh et Group)

 */
class InfoCommand {
public:
    /// Constructeur par défaut
    InfoCommand();

    /// Destructeur
    ~InfoCommand();

    /// les différents types de modification, dans l'ordre d'importance
    typedef enum{
        UNITIALIZED = 0,    // cas initial (non initialisé)
        LENGTHUNITMODIFIED, // modification de l'unité de longueur
        MESHDIMMODIFIED,    // modification de la dimension du maillage en sortie
        LANDMARKMODIFIED,   // modification du repère
        OTHERMODIFIED,      // cas où le DisplayRepresentation ne change pas
        DISPMODIFIED,       // cas du DisplayRepresentation à refaire
        VISIBILYCHANGED,    // cas d'une entité qui change de visibilité (visible/cachée)
        CREATED,            // entité créée
        DELETED,            // marquée à détruire
        ENABLE,             // marquée à réveiller (pour les groupes)
        DISABLE,            // marque pour celle que l'on a réveillé et que l'on cache à nouveau
        NONE                // on ne tient pas compte de l'entité
    } type;

    struct GeomEntityInfo {
        Geom::GeomEntity* m_geom_entity;
        type m_type;
    };

    struct TopoEntityInfo {
        Topo::TopoEntity* m_topo_entity;
        type m_type;
    };

    struct MeshEntityInfo {
        Mesh::MeshEntity* m_mesh_entity;
        type m_type;
    };

    struct StructuredMeshEntityInfo {
        Structured::StructuredMeshEntity* m_mesh_entity;
        type m_type;
    };

    struct SysCoordEntityInfo {
    	CoordinateSystem::SysCoord* m_sys_coord_entity;
        type m_type;
    };

    /// Fonction de comparaison suivant l'unique id entre TopoEntityInfo
    static bool compareTopoEntityInfo(TopoEntityInfo& tei1, TopoEntityInfo& tei2)
    {
    	return (tei1.m_topo_entity->getUniqueId()<tei2.m_topo_entity->getUniqueId());
    }

    /// ajoute une information sur le type de modification lorsqu'il s'agit du
    //contexte.
    void addContextInfo (type t);

    /// accès aux informations associées à une modification de contexte. 
    void getContextInfo(type &t);

    /// ajoute une information sur le type de modification pour une entité géométrique
    void addGeomInfoEntity(Geom::GeomEntity* entity, type t);

    /** ajoute une information sur le type de modification pour une entité topologique
     * \return vrai s'il y a eu un changement de type
     */
    bool addTopoInfoEntity(Topo::TopoEntity* entity, type t);

    /** ajoute une information sur le type de modification pour un groupe d'entités de maillage
     *  \return vrai s'il y a eu un changement de type
     */
    bool addMeshInfoEntity(Mesh::MeshEntity* entity, type t);

    /// ajoute une information sur le type de modification pour un groupe
    void addGroupInfoEntity(Group::GroupEntity* entity, type t);

    /// ajoute une information sur le type de modification pour un groupe
    void addSysCoordInfoEntity(CoordinateSystem::SysCoord* entity, type t);

    /** ajoute une information sur le type de modification pour une entité maillage
     * structuré
     * \return vrai s'il y a eu un changement de type
     */
    void addStructuredMeshInfoEntity(Structured::StructuredMeshEntity* entity, type t);

    /// retourne le nombre d'entités géométriques concernées par une modification
    uint getNbGeomInfoEntity() {return m_geom_entities_info.size();}

    /// retourne le nombre d'entités de maillage concernées par une modification
    uint getNbMeshInfoEntity() {return m_mesh_entities_info.size();}

    /// retourne le nombre d'entités de repères concernés par une modification
    uint getNbSysCoordInfoEntity() {return m_sys_coord_entities_info.size();}

    /// retourne le nombre d'entités de maillage structuré concernées par une modification
    uint getNbStructuredMeshInfoEntity() {return m_structured_mesh_entities_info.size();}

    /// accès aux entités géométriques concernées par une modification et au type de modification
    void getGeomInfoEntity(uint ind, Geom::GeomEntity* &entity, type &t);

    /// accès aux entités géométriques concernées par une modification et au type de modification
    std::vector<GeomEntityInfo>& getGeomInfoEntities() {return m_geom_entities_info;}
    const std::vector<GeomEntityInfo>& getGeomInfoEntities() const {return m_geom_entities_info;}

    /// accès aux entités topologique concernées par une modification et au type de modification
    std::map<Topo::TopoEntity*, type>& getTopoInfoEntity() {return m_topo_entities_info;}
    const std::map<Topo::TopoEntity*, type>& getTopoInfoEntity() const {return m_topo_entities_info;}
    std::vector<InfoCommand::TopoEntityInfo> getSortedTopoInfoEntity();

    /// accès aux entités de maillage concernées par une modification et au type de modification
    void getMeshInfoEntity(uint ind, Mesh::MeshEntity* &entity, type &t);
    std::vector <MeshEntityInfo>&  getMeshInfoEntities() {return m_mesh_entities_info;}
    const std::vector <MeshEntityInfo>&  getMeshInfoEntities() const {return m_mesh_entities_info;}

    /// accès aux entités de type groupe
    std::map<Group::GroupEntity*, type>& getGroupInfoEntity() {return m_group_entities_info;}
    const std::map<Group::GroupEntity*, type>& getGroupInfoEntity() const {return m_group_entities_info;}

    /// accès aux repères concernés par une modification et au type de modification
    void getSysCoordInfoEntity(uint ind, CoordinateSystem::SysCoord* &entity, type &t);

    /// accès aux entités de maillages structurés concernées par une modification et au type de modification
    void getStructuredMeshInfoEntity(uint ind, Structured::StructuredMeshEntity* &entity, type &t);
    std::vector <StructuredMeshEntityInfo>&  getStructuredMeshInfoEntities() {return m_structured_mesh_entities_info;}
    const std::vector <StructuredMeshEntityInfo>&  getStructuredMeshInfoEntities() const {return m_structured_mesh_entities_info;}

    /** Fonction de permutation de toutes les entités créées avec celles détruites
     *  et mise à jour de l'état des objets
     */
    void permCreatedDeleted();

    /// fonction de nettoyage
    void clear();

    /** Fonction de de mise à jour des connectivités entre entités lors de
     * lors de leur première destruction
     *
     *  \param entitiesToRemove entités qui doivent être supprimées
     */
    void setDestroyAndUpdateConnectivity(std::vector<Geom::GeomEntity*>& entitiesToRemove);

    /// transforme en une chaine lisible le type
    static std::string type2String(const InfoCommand::type &t);

    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const InfoCommand &);
    friend std::ostream & operator << (std::ostream & , const InfoCommand &);

protected:

    /// Constructeur par copie (interdit)
    InfoCommand(const InfoCommand&);

    /// Opérateur de copie (interdit)
    InfoCommand& operator = (const InfoCommand&);


private:

    /// Mutex pour protéger les les accès concurrents.
    TkUtil::Mutex	m_mutex;

    /// Type, lorsque cela concerne le contexte.
    type	m_context_type;

    /// ensemble des entités géométriques concernées par la commande et type de modification
    std::vector <GeomEntityInfo> m_geom_entities_info;

    /// ensemble des entités topologiques concernées par la commande et type de modification
    std::map<Topo::TopoEntity*, type> m_topo_entities_info;

    /// ensemble des entités de maillage concernées par la commande et type de modification
    std::vector <MeshEntityInfo> m_mesh_entities_info;

    /// ensemble des entités de type groupe concernées par la commande et type de modification
    std::map<Group::GroupEntity*, type> m_group_entities_info;

    /// ensemble des repères concernés par la commande et type de modification
    std::vector <SysCoordEntityInfo> m_sys_coord_entities_info;

    /// ensemble des entités de maillages structurés concernées par la commande et type de modification
    std::vector <StructuredMeshEntityInfo> m_structured_mesh_entities_info;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INTERNAL_INFOCOMMAND_H_ */
/*----------------------------------------------------------------------------*/
