/*----------------------------------------------------------------------------*/
/** \file		M3DCommandResultIfc.cpp
 *  \author		Charles
 *  \date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#include "Internal/M3DCommandResultIfc.h"
#include "Internal/CommandInternalAdapter.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>

using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace TkUtil;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Internal
{

/*----------------------------------------------------------------------------*/

M3DCommandResultIfc::M3DCommandResultIfc (Command& cmd)
	: CommandResult (cmd)
{
}	// M3DCommandResultIfc::M3DCommandResultIfc


M3DCommandResultIfc::M3DCommandResultIfc ()
	: CommandResult (* new CommandInternalAdapter ("invalid"))
{
}	// M3DCommandResultIfc::M3DCommandResultIfc


M3DCommandResultIfc::M3DCommandResultIfc (const M3DCommandResultIfc&)
	: CommandResult (* new CommandInternalAdapter ("invalid"))
{
	MGX_FORBIDDEN ("M3DCommandResultIfc copyructor is not allowed.")
}	// M3DCommandResultIfc::M3DCommandResultIfc


M3DCommandResultIfc& M3DCommandResultIfc::operator = (
        const M3DCommandResultIfc&)
{
	MGX_FORBIDDEN ("M3DCommandResultIfc assignment operator is not allowed.")
	return *this;
}	// M3DCommandResultIfc::operator =


M3DCommandResultIfc::~M3DCommandResultIfc ( )
{
	unregisterReferences ( );
}	// M3DCommandResultIfc::~M3DCommandResultIfc


std::vector<std::string> M3DCommandResultIfc::getVolumes ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getVolumes. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getVolumes

std::string M3DCommandResultIfc::getVolume (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getVolume (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getVolume


std::string M3DCommandResultIfc::getVolume ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getVolume ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getVolume



std::vector<std::string> M3DCommandResultIfc::getSurfaces ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getSurfaces. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getSurfaces


std::string M3DCommandResultIfc::getSurface (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getSurface (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getSurface


std::string M3DCommandResultIfc::getSurface ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getSurface ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getSurface


std::vector<std::string> M3DCommandResultIfc::getCurves ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getCurves. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getCurves


std::string M3DCommandResultIfc::getCurve (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getCurve (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getCurve


std::string M3DCommandResultIfc::getCurve ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getCurve ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getCurve


std::vector<std::string> M3DCommandResultIfc::getVertices ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getVertices. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getVertices


std::string M3DCommandResultIfc::getVertex (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getVertex (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getVertex


std::string M3DCommandResultIfc::getVertex ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getVertex ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getVertex


std::vector<std::string> M3DCommandResultIfc::getBlocks ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getBlocks. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getBlocks


std::string M3DCommandResultIfc::getBlock (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getBlock (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getBlock


std::string M3DCommandResultIfc::getBlock ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getBlock ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getBlock


std::vector<std::string> M3DCommandResultIfc::getFaces ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getFaces. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getFaces


std::string M3DCommandResultIfc::getFace (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getFace (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getFace


std::string M3DCommandResultIfc::getFace ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getFace ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getFace


std::vector<std::string> M3DCommandResultIfc::getEdges ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getEdges. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getEdges


std::string M3DCommandResultIfc::getEdge (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getEdge (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getEdge


std::string M3DCommandResultIfc::getEdge ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getEdge ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getEdge


std::vector<std::string> M3DCommandResultIfc::getTopoVertices ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getTopoVertices. Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getTopoVertices


std::string M3DCommandResultIfc::getTopoVertex (size_t index)
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getTopoVertex (size_t). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getTopoVertex


std::string M3DCommandResultIfc::getTopoVertex ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getTopoVertex ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getTopoVertex


std::vector<std::string> M3DCommandResultIfc::getGroups0D ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getGroups0D ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getGroups0D

std::vector<std::string> M3DCommandResultIfc::getGroups1D ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getGroups1D ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getGroups1D

std::vector<std::string> M3DCommandResultIfc::getGroups2D ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getGroups2D ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getGroups2D

std::vector<std::string> M3DCommandResultIfc::getGroups3D ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getGroups3D ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getGroups3D

std::string M3DCommandResultIfc::getSysCoord ( )
{
	throw Exception (UTF8String ("M3DCommandResultIfc::getSysCoord ( ). Méthode non surchargée.", Charset::UTF_8));
}	// M3DCommandResultIfc::getSysCoord

/*----------------------------------------------------------------------------*/

}	// namespace Internal
/*----------------------------------------------------------------------------*/

}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/

