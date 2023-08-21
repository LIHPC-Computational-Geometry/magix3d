/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingData.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef BLOCKMESHINGDATA_H_
#define BLOCKMESHINGDATA_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include <GMDS/Utils/CommonTypes.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Relation entre un bloc et le maillage associé (les noeuds et les polyèdres)
 */
class BlockMeshingData {
public:
    BlockMeshingData()
    : m_is_meshed(false)
    , m_is_premeshed(false)
	, m_is_mesh_crossed(false)
	, m_points(0)
    {
    	//std::cout<<"BlockMeshingData()"<<std::endl;
    }

    ~BlockMeshingData()
    {
    	//std::cout<<"~BlockMeshingData()"<<std::endl;
    }

    BlockMeshingData* clone() {
        BlockMeshingData* emd = new BlockMeshingData();
        emd->m_is_meshed = m_is_meshed;
        emd->m_is_premeshed = m_is_premeshed;
        emd->m_is_mesh_crossed = m_is_mesh_crossed;
        emd->m_nodes.insert(emd->m_nodes.end(), m_nodes.begin(), m_nodes.end());
        emd->m_poly.insert(emd->m_poly.end(), m_poly.begin(), m_poly.end());
        emd->m_points =  m_points;
        return emd;
    }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état des points du maillage */
    bool isPreMeshed() const {return m_is_premeshed;}

    /** Modificateur de l'état des points du maillage */
    void setPreMeshed(bool val) { m_is_premeshed = val; }

    /** Accesseur sur l'état du maillage */
    bool isMeshed() const {return m_is_meshed;}

    /** Modificateur de l'état du maillage */
    void setMeshed(bool val) { m_is_meshed = val; }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
    bool isMeshCrossed() const {return m_is_mesh_crossed;}

    /** Modificateur de l'état du maillage */
    void setMeshCrossed(bool val) { m_is_mesh_crossed = val; }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des points */
    Utils::Math::Point*& points() {return m_points;}

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des noeuds gmds */
    std::vector<gmds::TCellID>& nodes() {return m_nodes;}

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des polyêdres gmds */
    std::vector<gmds::TCellID>& regions() {return m_poly;}

private:
    /// Maillage associé créé ou non
    bool m_is_meshed;

    /// Calcul des points pour le maillage
    bool m_is_premeshed;

    /// Maillage associé avec maille croisée ou non
    bool m_is_mesh_crossed;

    /// Liste des noeuds (gmds) associés
    std::vector<gmds::TCellID> m_nodes;

    /// Liste des polyêdres (gmds) associés
    std::vector<gmds::TCellID> m_poly;

    /// Les points pour le maillage
    Utils::Math::Point* m_points;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* BLOCKMESHINGDATA_H_ */
