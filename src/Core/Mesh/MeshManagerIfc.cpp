/*----------------------------------------------------------------------------*/
/*
 * \file MeshManagerIfc.cpp
 *
 *  \author Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 02/02/2012
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshManagerIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/

MeshManagerIfc::MeshManagerIfc (const std::string& name, Internal::ContextIfc* c)
	: Internal::CommandCreator(name, c)
{
}

/*----------------------------------------------------------------------------*/

MeshManagerIfc::MeshManagerIfc (const MeshManagerIfc& mm)
	: Internal::CommandCreator (mm)
{
	MGX_FORBIDDEN ("MeshManagerIfc copy constructor is not allowed.")
}

/*----------------------------------------------------------------------------*/

MeshManagerIfc& MeshManagerIfc::operator = (const MeshManagerIfc& mm)
{
	MGX_FORBIDDEN ("MeshManagerIfc assignment operator is not allowed.")
	return *this;
}

/*----------------------------------------------------------------------------*/

MeshManagerIfc::~MeshManagerIfc()
{
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::clear()
{
	throw TkUtil::Exception ("MeshManagerIfc::clear should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::readMli(std::string nom, std::string prefix)
{
	throw TkUtil::Exception ("MeshManagerIfc::readMli should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::writeMli ( std::string nom )
{
	throw TkUtil::Exception ("MeshManagerIfc::writeMli should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::writeVTK ( std::string nom )
{
	throw TkUtil::Exception ("MeshManagerIfc::writeVTK should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::writeCGNS ( std::string nom )
{
	throw TkUtil::Exception ("MeshManagerIfc::writeCGNS should be overloaded.");
}
/*----------------------------------------------------------------------------*/

void MeshManagerIfc::smooth ()
{
	throw TkUtil::Exception ("MeshManagerIfc::smooth should be overloaded.");
}
/*----------------------------------------------------------------------------*/

bool MeshManagerIfc::compareWithMesh(std::string nom)
{
    throw TkUtil::Exception ("MeshManagerIfc::compareWithMesh should be overloaded.");
}

/*----------------------------------------------------------------------------*/

MeshItf* MeshManagerIfc::getMesh ( )
{
    throw TkUtil::Exception ("MeshManagerIfc::getMesh should be overloaded.");
}

/*----------------------------------------------------------------------------*/

MeshManagerIfc::strategy  MeshManagerIfc::getStrategy ( )
{
	throw TkUtil::Exception ("MeshManagerIfc::getStrategy should be overloaded.");
}
/*----------------------------------------------------------------------------*/

void MeshManagerIfc::setStrategy (const MeshManagerIfc::strategy& s)
{
	throw TkUtil::Exception ("MeshManagerIfc::getStrategy should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
			MeshManagerIfc::newBlocksMesh(std::vector<std::string>& names)
{
    throw TkUtil::Exception ("MeshManagerIfc::newBlocksMesh should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
			MeshManagerIfc::newFacesMesh(std::vector<std::string>& names)
{
    throw TkUtil::Exception ("MeshManagerIfc::newFacesMesh should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManagerIfc::newAllBlocksMesh()
{
	throw TkUtil::Exception ("MeshManagerIfc::newAllBlocksMesh should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* MeshManagerIfc::newAllFacesMesh()
{
    throw TkUtil::Exception ("MeshManagerIfc::newAllFacesMesh should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
MeshManagerIfc::newSubVolumeBetweenSheets(std::vector<std::string>& blocks_name, std::string narete,
			int pos1, int pos2, std::string groupName)
{
	throw TkUtil::Exception ("MeshManagerIfc::newSubVolumeBetweenSheets should be overloaded.");
}
/*----------------------------------------------------------------------------*/
//std::vector<double> MeshManagerIfc::getQuality(const std::string& name, int dim, Qualif::Critere critere, int types)
//{
//    throw TkUtil::Exception ("MeshManagerIfc::getQuality should be overloaded.");
//}
/*----------------------------------------------------------------------------*/
CommandMeshExplorer* MeshManagerIfc::newExplorer(CommandMeshExplorer* oldExplo, int inc, std::string narete, bool asCommand)
{
    throw TkUtil::Exception ("MeshManagerIfc::newExplorer should be overloaded.");
}
/*----------------------------------------------------------------------------*/
CommandMeshExplorer* MeshManagerIfc::endExplorer(CommandMeshExplorer* oldExplo, bool asCommand)
{
    throw TkUtil::Exception ("MeshManagerIfc::endExplorer should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Cloud* MeshManagerIfc::getCloud(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getCloud should be overloaded.");
}
/*----------------------------------------------------------------------------*/
//void MeshManagerIfc::getClouds(std::vector<Cloud*>&) const
//{
//	throw TkUtil::Exception ("MeshManagerIfc::getClouds should be overloaded.");
//}
/*----------------------------------------------------------------------------*/
Line* MeshManagerIfc::getLine(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getLine should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Surface* MeshManagerIfc::getSurface(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getSurface should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::getSurfaces(std::vector<Surface*>&) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getSurfaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Volume* MeshManagerIfc::getVolume(const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getVolume should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void MeshManagerIfc::getVolumes(std::vector<Volume*>&) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getVolumes should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int MeshManagerIfc::getNbClouds (bool onlyVisible) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getNbClouds should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int MeshManagerIfc::getNbSurfaces (bool onlyVisible) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getNbSurfaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int MeshManagerIfc::getNbVolumes (bool onlyVisible) const
{
	throw TkUtil::Exception ("MeshManagerIfc::getNbVolumes should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int MeshManagerIfc::getNbNodes ( )
{
    throw TkUtil::Exception ("MeshManagerIfc::getNbNodes should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int MeshManagerIfc::getNbFaces ( )
{
    throw TkUtil::Exception ("MeshManagerIfc::getNbFaces should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int MeshManagerIfc::getNbRegions ( )
{
    throw TkUtil::Exception ("MeshManagerIfc::getNbRegions should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string MeshManagerIfc::getInfos(const std::string& name, int dim) const
{
    throw TkUtil::Exception ("MeshManagerIfc::getInfos should be overloaded.");
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
