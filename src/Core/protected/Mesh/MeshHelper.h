/*----------------------------------------------------------------------------*/
/*
 * \file MeshHelper.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 juin 2019
 */
/*----------------------------------------------------------------------------*/
#ifndef PROTECTED_MESH_MESHHELPER_H_
#define PROTECTED_MESH_MESHHELPER_H_
/*----------------------------------------------------------------------------*/
#include <vector>

namespace gmds {
class Edge;
class Node;
}

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * \brief       Services utilitaires concernant les entités de maillage
 */
class MeshHelper
{
public:

	/// liste des ids triés, sans doublon sauf au extrémités si cela boucle
	static std::vector<uint> getOrderedNodesId(std::vector<gmds::Edge> bras);

	/// liste des noeuds (certainement en double)
	static std::vector<gmds::Node> getNodes(std::vector<gmds::Edge> bras);

private :

    /**
     * Constructeurs et destructeurs. Opérations interdites.
     */
    //@{
	MeshHelper ( );
	MeshHelper (const MeshHelper&);
	MeshHelper& operator = (const MeshHelper&);
    ~MeshHelper ( );
    //@}
};  // class TopoHelper

/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROTECTED_MESH_MESHHELPER_H_ */
