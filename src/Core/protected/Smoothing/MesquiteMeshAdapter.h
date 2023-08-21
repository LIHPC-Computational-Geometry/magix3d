/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteMeshAdapter.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 févr. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MESQUITEMESHADAPTER_H_
#define MESQUITEMESHADAPTER_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <map>
#include <sys/types.h> // pour uint
#include <GMDS/Utils/CommonTypes.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
class Node;
class Face;
}
/*----------------------------------------------------------------------------*/
// Mesquite
#include <MeshImpl.hpp>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
  /** @brief Classe pont entre structure de maillage Gmds dans Magix3D et structure Mesquite.
  */
class MesquiteMeshAdapter : public MESQUITE_NS::MeshImpl
{
public:
	/// passage de Gmds vers Mesquite dans le cas d'une surface
	MesquiteMeshAdapter(std::vector<gmds::Face>& gmdsPolygones,
			std::vector<gmds::Node>& gmdsNodes,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			std::map<gmds::TCellID, bool>& isPolyInverted,
			uint maskFixed);

	/// retour de Mesquite vers Gmds (modif des positions des noeuds)
	virtual ~MesquiteMeshAdapter();

private:
	/// copie de la liste des noeuds concernés pour les modifier
	std::vector<gmds::Node> m_gmdsNodes;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESQUITEMESHADAPTER_H_ */
