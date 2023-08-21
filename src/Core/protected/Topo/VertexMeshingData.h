/*----------------------------------------------------------------------------*/
/*
 * \file VertexMeshingData.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef VERTEXMESHINGDATA_H_
#define VERTEXMESHINGDATA_H_
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/Node.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Relation entre un sommet et le maillage associé (le noeud)
 */
class VertexMeshingData {
public:
    VertexMeshingData()
    : m_is_meshed(false)
    , m_node(0)
    {}

    ~VertexMeshingData()
    {}

    VertexMeshingData* clone() {
        VertexMeshingData* emd = new VertexMeshingData();
        emd->m_is_meshed = m_is_meshed;
        emd->m_node = m_node;
        return emd;
    }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
    bool isMeshed() const {return m_is_meshed;}

    /** Modificateur de l'état du maillage */
    void setMeshed(bool val) { m_is_meshed = val; }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur le noeud gmds */
    gmds::TCellID node() {return m_node;}

    /** Modificateur sur le noeud gmds */
    void setNode(gmds::TCellID nd) {m_node = nd;}


private:
    /// Maillage associé créé ou non
    bool m_is_meshed;

    /// Le noeud (gmds) associés
    gmds::TCellID m_node;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* VERTEXMESHINGDATA_H_ */
