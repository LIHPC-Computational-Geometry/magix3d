/*----------------------------------------------------------------------------*/
/** \file Group3D.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUP3D_H_
#define MGX3D_GROUP_GROUP3D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class Volume;
}
namespace Topo {
class Block;
}
namespace Mesh {
class Volume;
}
namespace CoordinateSystem {
class SysCoord;
}

namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 3D.
 * Permet l'accès aux entités 3D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
class Group3D : public GroupEntity{

    static const char* typeNameGroup3D;

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    Group3D(Internal::Context& ctx, const std::string & nom,
 		   bool isDefaultGroup, uint level=1);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~Group3D();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension du groupe
     */
    int getDim() const {return 3;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGroup3D;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::Group3D;}

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Group3D &);
    friend std::ostream & operator << (std::ostream & , const Group3D &);

    /// retourne les infos sur l'entité
    virtual std::string getInfos();

     /*------------------------------------------------------------------------*/
	/// retourne les entités du groupe.
     virtual std::vector<Utils::Entity*> getEntities() const;

    /*------------------------------------------------------------------------*/
    /// accesseur sur la liste de volumes référencés
    std::vector<Geom::Volume*>& getVolumes() {return m_volumes;}
    const std::vector<Geom::Volume*>& getVolumes() const {return m_volumes;}

    /// accesseur sur la liste de blocs référencés
    std::vector<Topo::Block*>& getBlocks() {return m_blocks;}
    const std::vector<Topo::Block*>& getBlocks() const {return m_blocks;}

    /// accesseur sur la liste de volumes de maillage référencés
    std::vector<Mesh::Volume*>& getMeshVolumes() {return m_mesh_volumes;}
    const std::vector<Mesh::Volume*>& getMeshVolumes() const {return m_mesh_volumes;}

    /// accesseur sur la liste de repères référencés
    std::vector<CoordinateSystem::SysCoord*>& getSysCoord() {return m_sys_coord;}
    const std::vector<CoordinateSystem::SysCoord*>& getSysCoord() const {return m_sys_coord;}

   bool empty() const {return m_volumes.empty() && m_blocks.empty() && m_mesh_volumes.empty() && m_sys_coord.empty();}

    /*------------------------------------------------------------------------*/
    /// Enlève un volume du groupe
    void remove(Geom::Volume* vol, const bool exceptionIfNotFound=true);
    /// Enlève un bloc du groupe
    void remove(Topo::Block* bloc, const bool exceptionIfNotFound=true);
    /// Enlève un volume du groupe
    void remove(Mesh::Volume* vol, const bool exceptionIfNotFound=true);
    /// Enlève un repère du groupe
    void remove(CoordinateSystem::SysCoord* rep, const bool exceptionIfNotFound=true);

    /*------------------------------------------------------------------------*/
    /// Ajoute un volume au groupe
    void add(Geom::Volume* vol);
    /// Ajoute un bloc au groupe
    void add(Topo::Block* bloc);
    /// Ajoute un volume au groupe
    void add(Mesh::Volume* vol);
    /// Ajoute un repère au groupe
    void add(CoordinateSystem::SysCoord* rep);

    /*------------------------------------------------------------------------*/
    /// Recherche un volume dans le groupe
    bool find(Geom::Volume* vol);
    /// Recherche un bloc dans le groupe
    bool find(Topo::Block* bloc);
    /// Recherche un volume dans le groupe
    bool find(Mesh::Volume* vol);
    /// Recherche un repère dans le groupe
    bool find(CoordinateSystem::SysCoord* rep);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
     * \return  Description, à détruire par l'appelant.
     */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif


private:
    /// les volumes référencés par ce groupe
    std::vector<Geom::Volume*> m_volumes;

    /// les blocs référencés par ce groupe
    std::vector<Topo::Block*> m_blocks;

    /// les volumes de maillage référencés par ce groupe
    std::vector<Mesh::Volume*> m_mesh_volumes;

    /// les repères référencés par ce groupe
    std::vector<CoordinateSystem::SysCoord*> m_sys_coord;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP3D_H_ */
/*----------------------------------------------------------------------------*/
