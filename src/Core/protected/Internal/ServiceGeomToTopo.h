/*----------------------------------------------------------------------------*/
/*
 * \file ServiceGeomToTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#ifndef SERVICEGEOMTOTOPO_H_
#define SERVICEGEOMTOTOPO_H_

#include <map>

#include "Topo/TopoEntity.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class GeomEntity;
class Volume;
class Surface;
class Curve;
class Vertex;
}
namespace Topo {
class Block;
class Face;
class CoFace;
class Edge;
class CoEdge;
class Vertex;
}
namespace Internal {
class Context;
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class ServiceGeomToTopo
 *  \brief Service de construction de la topologie associée à une géométrie
 */
/*----------------------------------------------------------------------------*/
class ServiceGeomToTopo {
public:
    /** Constructeur et construction de la topologie */
    ServiceGeomToTopo(Internal::Context& c, Geom::GeomEntity* ge,
    		std::vector<Geom::Vertex*>& vertices, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Destructeur */
    ~ServiceGeomToTopo();

    /*------------------------------------------------------------------------*/
    /** Convertion du bloc issu de la géométrie en un bloc structuré
     * \return true en cas d'échec
     */
    bool convertBlockStructured(const int ni, const int nj, const int nk);

    /*------------------------------------------------------------------------*/
    /** Convertion du bloc issue de la géométrie en un bloc pour l'insertion
     * */
    void convertInsertionBlock();

   /*------------------------------------------------------------------------*/
    /** Retourne le bloc construit */
    Topo::Block* getBlock();

    /*------------------------------------------------------------------------*/
    /** Convertion de la coface issue de la géométrie en une coface structurée
     * \return true en cas d'échec
     */
    bool convertCoFaceStructured();

    /*------------------------------------------------------------------------*/
    /** Retourne la coface construite */
    Topo::CoFace* getCoFace();

protected:

    /*------------------------------------------------------------------------*/
    /// construction du bloc Topo / volume Geom
    Topo::Block* getBlock(Geom::Volume* vol);

    /*------------------------------------------------------------------------*/
    /// construction de la face Topo / surface Geom
    Topo::Face* getFace(Geom::Surface* surf);

    /*------------------------------------------------------------------------*/
    /// construction de la face commune Topo / surface Geom
    Topo::CoFace* getCoFace(Geom::Surface* surf);

    /*------------------------------------------------------------------------*/
    /// construction de l'arête Topo / courbe Geom
    Topo::Edge* getEdge(Geom::Curve* curve);

    /// construction des arêtes Topo suivant sommets extrémités
    std::vector<Topo::Edge*> getEdges(std::vector<Topo::CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
    /// récupération de l'arête Topo / courbe Geom
    Topo::CoEdge* getCoEdge(Geom::Curve* curve);

    /*------------------------------------------------------------------------*/
    /// construction du sommet Topo / sommet Geom
    Topo::Vertex* getVertex(Geom::Vertex* gv);

protected:
    /// le contexte
    Internal::Context& m_context;

    /// le volume de base
    Geom::Volume* m_volume;

    /// la surface de base
    Geom::Surface* m_surface;

    /// Les sommets extrémités qui déterminent la structuration pour une face d'une surface
    std::vector<Geom::Vertex*> m_extrem_vertices;

    /// la structure pour le stockage des entités créées ou modifiées
    Internal::InfoCommand* m_icmd;

    /// map pour stocker les correspondances entre volume et bloc
    std::map<Geom::Volume*, Topo::Block* > m_vol_block;

    /// map pour stocker les correspondances entre surface et face
    std::map<Geom::Surface*, Topo::Face* > m_surf_face;

    /// map pour stocker les correspondances entre surface et coface
    std::map<Geom::Surface*, Topo::CoFace* > m_surf_coface;

    /// map pour stocker les correspondances entre courbe et arête
    std::map<Geom::Curve*, Topo::Edge* > m_curve_edge;

    /// map pour stocker les correspondances entre sommet geom et topo
    std::map<Geom::Vertex*, Topo::Vertex* > m_som_vtx;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* SERVICEGEOMTOTOPO_H_ */
