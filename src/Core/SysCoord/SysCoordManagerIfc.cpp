/*----------------------------------------------------------------------------*/
/*
 * \file SysCoordManagerIfc.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 mai 2018
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/CommandManager.h"
#include <TkUtil/Exception.h>
#include <SysCoord/SysCoordManagerIfc.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
SysCoordManagerIfc::SysCoordManagerIfc(const std::string& name, Internal::ContextIfc* c)
:Internal::CommandCreator(name, c)
{
}
/*----------------------------------------------------------------------------*/
SysCoordManagerIfc::SysCoordManagerIfc(const SysCoordManagerIfc& gm)
:Internal::CommandCreator(gm)
{
	MGX_FORBIDDEN ("SysCoordManagerIfc copy constructor is not allowed.")
}
/*----------------------------------------------------------------------------*/
SysCoordManagerIfc& SysCoordManagerIfc::operator =(const SysCoordManagerIfc& gm)
{
	MGX_FORBIDDEN ("SysCoordManagerIfc::operator = is not allowed.")
	return *this;
}
/*----------------------------------------------------------------------------*/
SysCoordManagerIfc::~SysCoordManagerIfc ( )
{
}
/*----------------------------------------------------------------------------*/
void SysCoordManagerIfc::clear()
{
	throw TkUtil::Exception (TkUtil::UTF8String ("SysCoordManagerIfc::clear should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::newSysCoord(std::string groupName)
{
	throw TkUtil::Exception ("GeomManagerIfc::newSysCoord should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::newSysCoord(const Point& p, std::string groupName)
{
	throw TkUtil::Exception ("GeomManagerIfc::newSysCoord should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::newSysCoord(const Point& pCentre, const Point& pX, const Point& pY, std::string groupName)
{
	throw TkUtil::Exception ("GeomManagerIfc::newSysCoord should be overloaded.");
}
/*----------------------------------------------------------------------------*/
SysCoord* SysCoordManagerIfc::getSysCoord (const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception ("GeomManagerIfc::getSysCoord should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::translate(std::string name, const Vector& dp)
{
	throw TkUtil::Exception ("SysCoordManagerIfc::translate should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::copyAndTranslate(std::string name, const Vector& dp, std::string groupName)
{
	throw TkUtil::Exception ("SysCoordManagerIfc::copyAndTranslate should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::rotate(std::string name,
				const Utils::Math::Rotation& rot)
{
	throw TkUtil::Exception ("SysCoordManagerIfc::rotate should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::copyAndRotate(std::string name,
			const Utils::Math::Rotation& rot, std::string groupName)
{
	throw TkUtil::Exception ("SysCoordManagerIfc::copyAndRotate should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
SysCoordManagerIfc::copy(std::string name, std::string groupName)
{
	throw TkUtil::Exception ("SysCoordManagerIfc::copy should be overloaded.");
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
