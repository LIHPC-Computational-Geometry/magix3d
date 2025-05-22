/*----------------------------------------------------------------------------*/
// pythonerie à mettre au début (pour permettre ifndef Py_PYTHON_H)
#include <Python.h>
#include <memory>				// unique_ptr
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshModificationBySepa.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/MeshManager.h"
#include "Utils/SerializedRepresentation.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include <gmds/io/VTKWriter.h>
#include <gmds/ig/Node.h>
#ifdef USE_SEPA3D
#include "Sepa3DCore/Separatrice.h"
#endif
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MeshModificationBySepa::
MeshModificationBySepa()
 : MeshModificationItf(),
   m_center(Utils::Math::Point(0,0,0))
{

}
/*----------------------------------------------------------------------------*/
MeshModificationBySepa::
MeshModificationBySepa(Utils::Math::Point& ACenter, std::vector<std::string>& AFilenames)
 : MeshModificationItf(),
   m_center(ACenter), m_sepafiles(AFilenames)
{

}
/*----------------------------------------------------------------------------*/
MeshModificationBySepa::
~MeshModificationBySepa()
{

}
/*----------------------------------------------------------------------------*/
MeshModificationBySepa::
MeshModificationBySepa(const MeshModificationBySepa& ASepa)
:MeshModificationItf(),
 m_sepafiles(ASepa.m_sepafiles),
 m_center(ASepa.m_center),
 m_deltas(ASepa.m_deltas),
 m_faces(ASepa.m_faces)
{

}
/*----------------------------------------------------------------------------*/
MeshModificationBySepa* MeshModificationBySepa::
clone() const
{
	return new MeshModificationBySepa(*this);
}
/*----------------------------------------------------------------------------*/
void MeshModificationBySepa::
addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const
{
	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Modification Sepa", std::string()));
}
/*----------------------------------------------------------------------------*/
void
MeshModificationBySepa::setCenter(const Point ACenter)
{
	m_center = ACenter;
}
/*----------------------------------------------------------------------------*/
void
MeshModificationBySepa::addControlFile(const std::string& AFilename)
{
	m_sepafiles.push_back(AFilename);
}
/*----------------------------------------------------------------------------*/
//void
//MeshModificationBySepa::writeDeltaVTKFile(const std::string& AFilename)
//{
//std::cout<<"writeDeltaVTKFile m_deltas.size() "<<m_deltas.size()<<std::endl;
//std::cout<<"writeDeltaVTKFile m_faces.size() "<<m_faces.size()<<std::endl;
//
//	// temporary mesh
//	gmds::MeshModel* meshModel = new gmds::MeshModel(gmds::F|gmds::N|gmds::F2N);
//	gmds::Mesh* mesh = new gmds::Mesh(*meshModel);
//
//	gmds::Variable<double>* deltas = mesh->newVariable<double>(gmds::GMDS_NODE,"delta");
//
//	std::map<gmds::Node, gmds::Node> old2newNodes;
//	std::map<int,double>::iterator itn = m_deltas.begin();
//
//	for(; itn != m_deltas.end(); itn++) {
//		gmds::Node newNode = mesh->newNode(itn->first.getPoint());
//		(*deltas)[newNod[iNode].id()()] = itn->second;
//		old2newNodes[itn->first] = newNode;
//	}
//
//	for(int iFace=0; iFace<m_faces.size(); iFace++) {
//		gmds::Face current_face = m_faces[iFace];
//		std::vector<gmds::Node> oldNodes = current_face.get<gmds::Node>();
//		std::vector<gmds::Node> newNodes;
//		for(int iNode=0; iNode<oldNodes.size(); iNode++) {
//			newNodes.push_back(old2newNodes[oldNodes[iNode]]);
//		}
//		mesh->newFace(newNodes);
//	}
//
//	gmds::VTKWriter<gmds::Mesh> writer(*mesh);
//	writer.write(AFilename,gmds::N|gmds::F);
//	delete meshModel;
//}
/*----------------------------------------------------------------------------*/
void
MeshModificationBySepa::writeControlVTKFile(const std::string& AFilename)
{
#ifdef USE_SEPA3D
	sepa3d::Separatrice* sepa = new sepa3d::Separatrice();
	gmds::math::Point center(m_center.getX(),m_center.getY(),m_center.getZ());
	sepa->setCenter(center);

	if(m_sepafiles.size() == 0) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"MeshModificationBySepa::applyModification Pas de fichier pour sepa.";
		throw TkUtil::Exception(message);
	}

	for(int iFile=0; iFile<m_sepafiles.size(); iFile++) {
		sepa->readControlFile(m_sepafiles[iFile]);
	}
	sepa->buildMesh();

	sepa->writeControlMesh(AFilename);

	delete sepa;
#else
	throw TkUtil::Exception (TkUtil::UTF8String ("Package Sepa3D non inclu", TkUtil::Charset::UTF_8));
#endif
}
/*----------------------------------------------------------------------------*/
void MeshModificationBySepa::
applyModification(std::vector<gmds::Node >& AGmdsNodes, std::vector<gmds::Face>& AGmdsFaces, Internal::Context& AContext)
{
#ifdef _DEBUG2
	std::cout<<"MeshModificationBySepa::applyModification pour "<<AGmdsNodes.size()<<" noeuds"<<std::endl;
	uint nodeCount = 0;
#endif

	// Sepa will work only in micrometers
	Utils::Unit::lengthUnit lu = AContext.getLengthUnit();
	double convertFactorToMicron = Utils::Unit::computeFactor(lu,Utils::Unit::micron);
	double convertFactorFromMicron = Utils::Unit::computeFactor(Utils::Unit::micron,lu);

	m_faces = AGmdsFaces;

#ifdef USE_SEPA3D
	sepa3d::Separatrice* sepa = new sepa3d::Separatrice();
	gmds::math::Point center(m_center.getX()*convertFactorToMicron,m_center.getY()*convertFactorToMicron,m_center.getZ()*convertFactorToMicron);
	sepa->setCenter(center);

	if(m_sepafiles.size() == 0) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"MeshModificationBySepa::applyModification Pas de fichier pour sepa.";
		throw TkUtil::Exception(message);
	}

	for(int iFile=0; iFile<m_sepafiles.size(); iFile++) {
		sepa->readControlFile(m_sepafiles[iFile]);
	}
	sepa->buildMesh();

	for (std::vector<gmds::Node >::iterator iter = AGmdsNodes.begin();
			iter != AGmdsNodes.end(); ++iter) {
		gmds::Node node = *iter;

		double delta = sepa->getRadialPerturbation(node.point()*convertFactorToMicron);
		m_deltas[node] = delta;

		gmds::math::Point point = node.point()*convertFactorToMicron;
		gmds::math::Point newPoint = (sepa->applyPerturbation(point)) * convertFactorFromMicron;
		node.setPoint(newPoint);
	}

//#ifdef _DEBUG2
//	std::cout<<"nodeCount = "<<nodeCount<<std::endl;
//#endif
#else
	throw TkUtil::Exception (TkUtil::UTF8String ("Package Sepa3D non inclu", TkUtil::Charset::UTF_8));
#endif
}
/*----------------------------------------------------------------------------*/
std::map<gmds::Node,double>&
MeshModificationBySepa::getDeltas()
{
	return m_deltas;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String
MeshModificationBySepa::getScriptCommand() const
{
	TkUtil::UTF8String	o (TkUtil::Charset::UTF_8);

	o << getMgx3DAlias() << ".MeshModificationBySepa(";
	o << m_center.getScriptCommand();
	o << ",[";
	for(int i=0; i<m_sepafiles.size(); i++) {
		if(i>0) {
			o << ", ";
		}
		o << "\"" << m_sepafiles[i] <<"\"";
	}
	o << "]";
	o << ")";

	return o;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
