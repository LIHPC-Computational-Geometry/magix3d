/*----------------------------------------------------------------------------*/
/*
 * \file CommandExtrudeTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/10/2017
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXTRUDETOPO_H_
#define COMMANDEXTRUDETOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandTransformTopo.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandExtrusion;
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandExtrudeTopo
 *  \brief Commande permettant la construction de la topologie 3D à partir d'une topologie 2D
 *  avec insertion d'un ogrid au centre
 *
 */
/*----------------------------------------------------------------------------*/
class CommandExtrudeTopo: public Topo::CommandTransformTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param commandGeom la commande géométrique pour retrouver les entités créées
     *  \param cofaces les CoFace concernées
     *  \param dv le vecteur directeur
     */
    CommandExtrudeTopo(Internal::Context& c,
    		Geom::CommandExtrusion* commandGeom,
    		std::vector<TopoEntity*>& cofaces,
			const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExtrudeTopo();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

    /// constitution de la liste des cofaces
    virtual void init();

private:

    /// construction des arêtes entre 2 séries de cofaces
    void constructExtrudeCoEdges(std::vector<CoFace*>& cofaces_0,
    		std::vector<CoFace*>& cofaces_1,
			std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
			uint ni);

    /// construction des faces entre 2 séries de cofaces
    void constructExtrudeFaces(std::vector<CoFace*>& cofaces_0,
    		std::vector<CoFace*>& cofaces_1,
			std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
			std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces);

    /// construction des blocs entre 2 séries de cofaces
    void constructExtrudeBlocks(std::vector<CoFace*>& cofaces_0,
    		std::vector<CoFace*>& cofaces_1,
			std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
			std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
			uint ni);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une extrusion (utilisation de V2C ou de C2S)
     */
    void updateGeomAssociation(Geom::GeomEntity* ge, CoEdge* coedge);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une extrusion (utilisation de C2S)
     */
    void updateGeomAssociation(Geom::GeomEntity* ge, Block* block);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une extrusion (utilisation de S2V)
     */
    void updateGeomAssociation(Geom::GeomEntity* ge, CoFace* coface);



private:
    /// la commande d'extrusion (suivant une direction) de la géométrie
    Geom::CommandExtrusion* m_commandGeom;

    /// les cofaces de la base
    std::vector<CoFace*> m_cofaces;

    /// le vecteur directeur
    Utils::Math::Vector m_dv;

 };
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXTRUDETOPO_H_ */
/*----------------------------------------------------------------------------*/
