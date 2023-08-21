/*----------------------------------------------------------------------------*/
/*
 * \file MeshModificationByPythonFunction.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5 déc. 2013
 */
/*----------------------------------------------------------------------------*/
// pythonerie à mettre au début (pour permettre ifndef Py_PYTHON_H)
#include <Python.h>
#include <memory>				// unique_ptr
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshModificationByPythonFunction.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/MeshManager.h"
#include "Utils/SerializedRepresentation.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/Node.h"
/*----------------------------------------------------------------------------*/
#if PY_MAJOR_VERSION >= 3
#define PyString_AsString PyUnicode_AsUTF8
#endif	// # PY_MAJOR_VERSION < 3
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MeshModificationByPythonFunction::
MeshModificationByPythonFunction(PyObject* py_obj, eTypeCoord typeCoord)
 : MeshModificationItf()
, m_py_obj(py_obj)
, m_typeCoord(typeCoord)
{

}
/*----------------------------------------------------------------------------*/
MeshModificationByPythonFunction::
~MeshModificationByPythonFunction()
{

}
/*----------------------------------------------------------------------------*/
void MeshModificationByPythonFunction::
addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const
{
	PyObject* pyName = PyObject_GetAttrString(m_py_obj, "func_name");
	std::string strName(PyString_AsString(pyName));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Fonction Python", strName));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Type de coordonnées", std::string(m_typeCoord==polar?"polaires":"cartésiennes")));

}
/*----------------------------------------------------------------------------*/
void MeshModificationByPythonFunction::
applyModification(std::vector<gmds::Node >& gmdsNodes,
		std::map<gmds::TCellID, uint>& filtre_nodes,
		uint maskFixed)
{
#ifdef _DEBUG2
	std::cout<<"MeshModificationByPythonFunction::applyModification pour "<<gmdsNodes.size()<<" noeuds, maskFixed = "<<maskFixed<<std::endl;
	uint nodeCount = 0;
#endif

	for (std::vector<gmds::Node >::iterator iter = gmdsNodes.begin();
			iter != gmdsNodes.end(); ++iter){
		gmds::Node nd = *iter;

		// on passe les noeuds marqués
#ifdef _DEBUG2
		std::cout<<"Node "<<nd.getID()<<" filtre : "<<filtre_nodes[nd.getID()]<<std::endl;
#endif
		if (filtre_nodes[nd.getID()] == maskFixed)
			continue;
#ifdef _DEBUG2
	nodeCount += 1;
#endif

		double rho,theta,phi;

		PyObject* T = PyTuple_New(3);
		PyObject* py_coord1;
		PyObject* py_coord2;
		PyObject* py_coord3;

		if (m_typeCoord == polar){
			rho = std::sqrt(nd.X()*nd.X()+nd.Y()*nd.Y()+nd.Z()*nd.Z());
			if (Utils::Math::MgxNumeric::isNearlyZero(rho))
				theta = 0;
			else
				theta = std::asin(nd.Z()/rho);
			phi = std::atan2(nd.Y(), nd.X());

			py_coord1 = Py_BuildValue("d",rho);
			py_coord2 = Py_BuildValue("d",theta);
			py_coord3 = Py_BuildValue("d",phi);

		}
		else {
			py_coord1 = Py_BuildValue("d",nd.X());
			py_coord2 = Py_BuildValue("d",nd.Y());
			py_coord3 = Py_BuildValue("d",nd.Z());
		}

		PyTuple_SetItem(T,0,py_coord1);
		PyTuple_SetItem(T,1,py_coord2);
		PyTuple_SetItem(T,2,py_coord3);

		PyObject* R = PyObject_Call(m_py_obj, T, NULL);

		Py_DECREF(T);

		if (R){
			// récupération des 3 coordonnées en retour
			py_coord1 = PyList_GetItem(R,0);
			py_coord2 = PyList_GetItem(R,1);
			py_coord3 = PyList_GetItem(R,2);

			double res1 = PyFloat_AsDouble(py_coord1);
			double res2 = PyFloat_AsDouble(py_coord2);
			double res3 = PyFloat_AsDouble(py_coord3);

			if (m_typeCoord == polar){
				rho   = res1;
				theta = res2;
				phi   = res3;
				res1 = rho * std::cos(theta) * std::cos(phi);
				res2 = rho * std::cos(theta) * std::sin(phi);
				res3 = rho * std::sin(theta);
			}

			nd.setX(res1);
			nd.setY(res2);
			nd.setZ(res3);
		}
		else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message <<"Erreur interne lors de l'appel à PyObject_Call pour noeud d'id "<<(long)nd.getID()
					<<". Certainement un problème dans la fonction de perturbation en Python.";
			throw TkUtil::Exception(message);
		}
		Py_DECREF(R);

	} // end for iter
#ifdef _DEBUG2
	std::cout<<"nodeCount = "<<nodeCount<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
