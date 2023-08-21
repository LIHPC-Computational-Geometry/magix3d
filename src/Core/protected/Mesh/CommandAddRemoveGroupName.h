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
    /** \brief  Constructeur avec une liste de volumes
     *
     *  \param c le contexte
     *  \param volumes les volumes à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Geom::Volume*>& volumes,
            const std::string& groupName,
            groupOperation ope);

    /** \brief  Constructeur avec une liste de surfaces
     *
     *  \param c le contexte
     *  \param surfaces les surfaces à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param add si on est dans le cas d'un ajout
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Geom::Surface*>& surfaces,
            const std::string& groupName,
            groupOperation ope);

    /** \brief  Constructeur avec une liste de courbes
     *
     *  \param c le contexte
     *  \param curves les courbes à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param add si on est dans le cas d'un ajout
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Geom::Curve*>& curves,
            const std::string& groupName,
            groupOperation ope);

    /** \brief  Constructeur avec une liste de sommets géométriques
     *
     *  \param c le contexte
     *  \param vertices les sommets à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param add si on est dans le cas d'un ajout
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Geom::Vertex*>& vertices,
            const std::string& groupName,
            groupOperation ope);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur avec une liste de blocs
     *
     *  \param c le contexte
     *  \param blocks les blocs à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Topo::Block*>& blocks,
            const std::string& groupName,
            groupOperation ope);

    /** \brief  Constructeur avec une liste de cofaces
     *
     *  \param c le contexte
     *  \param cofaces les cofaces à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Topo::CoFace*>& cofaces,
            const std::string& groupName,
            groupOperation ope);

    /** \brief  Constructeur avec une liste de coedges
     *
     *  \param c le contexte
     *  \param coedges les coedges à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Topo::CoEdge*>& coedges,
            const std::string& groupName,
            groupOperation ope);

    /** \brief  Constructeur avec une liste de sommets topologiques
     *
     *  \param c le contexte
     *  \param vertices les sommets à ajouter au groupe
     *  \param groupName le nom du groupe auquel on ajoute les entités
     *  \param ope le type d'opération
     */
    CommandAddRemoveGroupName(Internal::Context& c,
            std::vector<Topo::Vertex*>& vertices,
            const std::string& groupName,
            groupOperation ope);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandAddRemoveGroupName();

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

    /*------------------------------------------------------------------------*/
    ///  met à jour les relation entre volumes de maillage et la topologie
    void updateMesh(Geom::Volume* vol, std::string grpName, bool add);
    void updateMesh(std::vector<Topo::Block*>& blocs, std::string grpName, bool add);

    ///  met à jour les relation entre surface de maillage et la topologie
    void updateMesh(Geom::Surface* surf, std::string grpName, bool add);
    void updateMesh(std::vector<Topo::CoFace*>& cofaces, std::string grpName, bool add);

    ///  met à jour les relation entre nuages de maillage et la topologie
    void updateMesh(Geom::Curve* crv, std::string grpName, bool add);
    void updateMesh(std::vector<Topo::CoEdge*>& coedges, std::string grpName, bool add);


private:

    /// ajoute un sommet à un groupe et réciproquement
    void addGroup(std::string ng, Geom::Vertex* vtx);
    /// enlève un sommet à un groupe et réciproquement
    void removeGroup(std::string ng, Geom::Vertex* vtx);

    /// ajoute une courbe à un groupe et réciproquement
    void addGroup(std::string ng, Geom::Curve* crv);
    /// enlève une courbe à un groupe et réciproquement
    void removeGroup(std::string ng, Geom::Curve* crv);

    /// ajoute une surface à un groupe et réciproquement
    void addGroup(std::string ng, Geom::Surface* srf);
    /// enlève une surface à un groupe et réciproquement
    void removeGroup(std::string ng, Geom::Surface* srf);

    /// ajoute un volume à un groupe et réciproquement
    void addGroup(std::string ng, Geom::Volume* vol);
    /// enlève un volume à un groupe et réciproquement
    void removeGroup(std::string ng, Geom::Volume* vol);


    /// ajoute un sommet à un groupe et réciproquement
    void addGroup(std::string ng, Topo::Vertex* vtx);
    /// enlève un sommet à un groupe et réciproquement
    void removeGroup(std::string ng, Topo::Vertex* vtx);

    /// ajoute une arête à un groupe et réciproquement
    void addGroup(std::string ng, Topo::CoEdge* coedge);
    /// enlève une arête à un groupe et réciproquement
    void removeGroup(std::string ng, Topo::CoEdge* coedge);

    /// ajoute une face à un groupe et réciproquement
    void addGroup(std::string ng, Topo::CoFace* coface);
    /// enlève une face à un groupe et réciproquement
    void removeGroup(std::string ng, Topo::CoFace* coface);

    /// ajoute un bloc à un groupe et réciproquement
    void addGroup(std::string ng, Topo::Block* bloc);
    /// enlève un bloc à un groupe et réciproquement
    void removeGroup(std::string ng, Topo::Block* bloc);



private:
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
