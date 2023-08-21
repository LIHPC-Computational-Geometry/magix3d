/*----------------------------------------------------------------------------*/
/*
 * \file CoEdgeMeshingData.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COEDGEMESHINGDATA_H_
#define COEDGEMESHINGDATA_H_
/*----------------------------------------------------------------------------*/
#include <GMDS/Utils/CommonTypes.h>

#include "Utils/Time.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {

class CoEdge;

/*----------------------------------------------------------------------------*/
/**
   @brief Relation entre une arête et le maillage associé (les noeuds et les bras)
 */
class CoEdgeMeshingData {

	friend class CoEdge;

public:
    CoEdgeMeshingData()
    : m_is_meshed(false)
	, m_is_premeshed(false)
    {}

    ~CoEdgeMeshingData()
    {}

    CoEdgeMeshingData* clone() {
        CoEdgeMeshingData* emd = new CoEdgeMeshingData();
        emd->m_is_meshed = m_is_meshed;
        emd->m_is_premeshed = m_is_premeshed;
        emd->m_nodes.insert(emd->m_nodes.end(), m_nodes.begin(), m_nodes.end());
        emd->m_edges.insert(emd->m_edges.end(), m_edges.begin(), m_edges.end());
        emd->m_points.insert(emd->m_points.end(), m_points.begin(), m_points.end());
        emd->m_points_time = m_points_time;
        return emd;
    }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état des points du maillage */
    bool isPreMeshed() const {return m_is_premeshed;}

    /** Modificateur de l'état des points du maillage */
    void setPreMeshed(bool val) const { m_is_premeshed = val; }

    /** Accesseur sur l'état du maillage */
    bool isMeshed() const {return m_is_meshed;}

    /** Modificateur de l'état du maillage */
    void setMeshed(bool val) { m_is_meshed = val; }

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des noeuds gmds */
    std::vector<gmds::TCellID>& nodes() {return m_nodes;}

    /** Accesseur sur la liste des bras gmds */
    std::vector<gmds::TCellID>& edges() {return m_edges;}

   /*------------------------------------------------------------------------*/
    /// accesseur sur l'heure de mise à jour des points
    const Utils::Time& getPointsTime() const {return m_points_time;}

    /// reinitialise l'heure de mise à jour des points
    void updatePointsTime() const {m_points_time.update();}

private:
    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des points */
    std::vector<Utils::Math::Point>& points() {return m_points;}

    /// Maillage associé créé ou non
    bool m_is_meshed;

    /// Calcul des points pour le maillage
    mutable bool m_is_premeshed;

    /// Liste des noeuds (gmds) associés
    std::vector<gmds::TCellID> m_nodes;

    /// Liste des bras (gmds) associés
    std::vector<gmds::TCellID> m_edges;

    /// Les points pour le maillage
    std::vector<Utils::Math::Point> m_points;

    /// Heure de la mise à jour des points
    mutable Utils::Time m_points_time;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COEDGEMESHINGDATA_H_ */
