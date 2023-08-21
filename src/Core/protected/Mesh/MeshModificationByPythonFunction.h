/*----------------------------------------------------------------------------*/
/*
 * \file MeshModificationByPythonFunction.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5 déc. 2013
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHMODIFICATIONBYPYTHONFUNCTION_H_
#define MESHMODIFICATIONBYPYTHONFUNCTION_H_
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
 * \class MeshModificationByPythonFunction
 *
 * Objet qui va modifier un maillage suivant une fonction utilisateur en Python
 *
 */
class MeshModificationByPythonFunction : public MeshModificationItf {
public:

	/// type de coordonnées, polaires ou cartésiennes
	enum eTypeCoord {cartesian,polar};

	MeshModificationByPythonFunction(PyObject* py_obj, eTypeCoord typeCoord);

	virtual ~MeshModificationByPythonFunction();

	/** Fonction d'appel pour modifier un ensemble de noeuds du maillage
	 *  Ne sont modifiés que ceux dont la valeur est différente de la marque dans le filtre
	 */
	virtual void applyModification(std::vector<gmds::Node >& gmdsNodes,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			uint maskFixed);

	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual void addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const;

protected:
	MeshModificationByPythonFunction(const MeshModificationByPythonFunction&)
    :MeshModificationItf(), m_py_obj(0), m_typeCoord(cartesian)
    {
        MGX_FORBIDDEN("MeshModificationByPythonFunction::MeshModificationByPythonFunction is not allowed.");
    }

	MeshModificationByPythonFunction& operator = (const MeshModificationByPythonFunction&)
    {
        MGX_FORBIDDEN("MeshModificationByPythonFunction::operator = is not allowed.");
        return *this;
    }
private:
	/// fonction utilisateur en python de perturbation du maillage
	PyObject* m_py_obj;

	/// type de coordonnées sur lesquelles doit être utilisé la fonction python
	eTypeCoord m_typeCoord;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESHMODIFICATIONBYPYTHONFUNCTION_H_ */
