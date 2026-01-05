/*----------------------------------------------------------------------------*/
/*
 * \file CommandAddRemoveGroupName.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 oct. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_COMMANDADDREMOVEGROUPNAME_H_
#define MGX3D_MESH_COMMANDADDREMOVEGROUPNAME_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Vector.h"
#include "Group/GroupHelperForCommand.h"
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
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
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandAddRemoveGroupName
 *  \brief Commande permettant d'ajouter des entités à un groupe
 */
/*----------------------------------------------------------------------------*/
class CommandAddRemoveGroupName: public CommandCreateMesh
{
public:
	/// Type d'opération
	typedef enum{
		add,
		remove,
		set
	} groupOperation;

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur avec une liste d'entités géométriques de même dimension
     *
     *  \param c le contexte
     *  \param entities les entités géométriques à ajouter au groupe
     *  \param dim la dimension des entités
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Geom::GeomEntity*>& entities,
            const int dim,
            const std::string& groupName,
            groupOperation ope);

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur avec une liste d'entités topologiques de même dimension
     *
     *  \param c le contexte
     *  \param entities les entités topologiques à ajouter au groupe
     *  \param dim la dimension des entités
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Topo::TopoEntity*>& entities,
            const int dim,
            const std::string& groupName,
            groupOperation ope);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    ~CommandAddRemoveGroupName() = default;

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();

private:
    /*------------------------------------------------------------------------*/
    ///  met à jour les relation entre volumes de maillage et la topologie
    void updateMesh(Geom::GeomEntity* e, std::string grpName, bool add);
    void updateMesh(Topo::TopoEntity* e, std::string grpName, bool add);
    void updateMesh(std::vector<Topo::Block*>& blocs, std::string grpName, bool add);
    void updateMesh(std::vector<Topo::CoFace*>& cofaces, std::string grpName, bool add);
    void updateMesh(std::vector<Topo::CoEdge*>& coedges, std::string grpName, bool add);

    /// helper pour la gestion des groupes dans les commandes
    Group::GroupHelperForCommand m_group_helper;

    /// la liste des entités géométriques concernées
    std::vector<Geom::GeomEntity*> m_geom_entities;

    /// la liste des entités topologiques concernées
    std::vector<Topo::TopoEntity*> m_topo_entities;

    /// nom du groupe concerné
    std::string m_groupName;

    /// la dimension des entités concernées
    int m_dim;

    /// le type d'opération
    groupOperation m_ope;

    /// archivage des noms des groupes par entité géométrique pour undo-redo
    std::map<Geom::GeomEntity*, std::vector<std::string> > m_geom_groups;

    /// archivage des noms des groupes par entité topologiques pour undo-redo
    std::map<Topo::TopoEntity*, std::vector<std::string> > m_topo_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_COMMANDADDREMOVEGROUPNAME_H_ */
/*----------------------------------------------------------------------------*/
