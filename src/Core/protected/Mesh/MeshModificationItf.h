/*----------------------------------------------------------------------------*/
/*
 * \file MeshModificationItf.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5 déc. 2013
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHMODIFICATION_H_
#define MESHMODIFICATION_H_
/*----------------------------------------------------------------------------*/
#include <map>
#include <sys/types.h>	// uint
#include "Utils/Common.h"

/*----------------------------------------------------------------------------*/
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
 * \class MeshModificationItf
 *
 * Interface d'un objet qui va modifier un maillage
 *
 */
class MeshModificationItf {
public:

	MeshModificationItf() {}

	virtual ~MeshModificationItf() {}

	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual void addToDescription (Mgx3D::Utils::SerializedRepresentation*) const =0;

protected:
	MeshModificationItf(const MeshModificationItf&)
    {
        MGX_FORBIDDEN("MeshModificationItf::MeshModificationItf is not allowed.");
    }

	MeshModificationItf& operator = (const MeshModificationItf&)
    {
        MGX_FORBIDDEN("MeshModificationItf::operator = is not allowed.");
        return *this;
    }
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESHMODIFICATION_H_ */
