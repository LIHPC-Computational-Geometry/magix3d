/*----------------------------------------------------------------------------*/
/*
 * \file CommandClearGroupName.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/04/2019
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_COMMANDCLEARGROUPNAME_H_
#define MGX3D_MESH_COMMANDCLEARGROUPNAME_H_
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
/** \class CommandClearGroupName
 *  \brief Commande permettant d'ajouter des entités à un groupe
 */
/*----------------------------------------------------------------------------*/
class CommandClearGroupName: public CommandCreateMesh
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur avec une liste de volumes
     *
     *  \param c le contexte
     *  \param dim la dimension du group concerné
     *  \param groupName le nom du groupe
     */
    CommandClearGroupName(Internal::Context& c,
            int dim,
            const std::string& groupName);

     /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandClearGroupName();

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
    /// nom du groupe concerné
    std::string m_groupName;

    /// la dimension des entités concernées
    int m_dim;

    /// archivage des entités géométriques pour undo-redo
    std::vector<Geom::GeomEntity*> m_geom_entities;

    /// archivage des entités topologiques pour undo-redo
    std::vector<Topo::TopoEntity*> m_topo_entities;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_COMMANDCLEARGROUPNAME_H_ */
/*----------------------------------------------------------------------------*/
