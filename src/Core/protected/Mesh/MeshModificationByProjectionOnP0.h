/*----------------------------------------------------------------------------*/
/*
 * \file MeshModificationByProjectionOnP0.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20/3/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHMODIFICATIONBYPROJECTIONONP0_H_
#define MESHMODIFICATIONBYPROJECTIONONP0_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshModificationItf.h"
#include <GMDS/Utils/CommonTypes.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
 * \class MeshModificationByProjectionOnP0
 *
 * Objet qui va modifier un maillage par remise à 0 d'une des coordonnées pour les noeuds
 *
 */
class MeshModificationByProjectionOnP0 : public MeshModificationItf {
public:

	enum eTypeCoord {X,Y,Z};

	MeshModificationByProjectionOnP0(eTypeCoord typeCoord);

	virtual ~MeshModificationByProjectionOnP0();

	/** Fonction d'appel pour modifier un ensemble de noeuds du maillage
	 */
	virtual void applyModification(std::vector<gmds::Node >& gmdsNodes);

	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual void addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const;

protected:
	MeshModificationByProjectionOnP0(const MeshModificationByProjectionOnP0&)
    :MeshModificationItf(), m_typeCoord(X)
    {
        MGX_FORBIDDEN("MeshModificationByProjectionOnP0::MeshModificationByProjectionOnP0 is not allowed.");
    }

	MeshModificationByProjectionOnP0& operator = (const MeshModificationByProjectionOnP0&)
    {
        MGX_FORBIDDEN("MeshModificationByProjectionOnP0::operator = is not allowed.");
        return *this;
    }
private:

	/// type de coordonnées sur lesquelles doit être fait la remise à 0
	eTypeCoord m_typeCoord;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESHMODIFICATIONBYPROJECTIONONP0_H_ */
