/*----------------------------------------------------------------------------*/
/** \file		CommandResultIfc.cpp
 *  \author		Team Magix3D
 *  \date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#include "Utils/CommandResultIfc.h"
#include "Utils/Common.h"

using namespace Mgx3D::Utils;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils
{

/*----------------------------------------------------------------------------*/

CommandResultIfc::CommandResultIfc ( )
{
}	// CommandResultIfc::CommandResultIfc


CommandResultIfc::CommandResultIfc (const CommandResultIfc&)
{
	MGX_FORBIDDEN ("CommandResultIfc copy constructor is not allowed.")
}	// CommandResultIfc::CommandResultIfc


CommandResultIfc& CommandResultIfc::operator = (const CommandResultIfc&)
{
	MGX_FORBIDDEN ("CommandResultIfc assignment operator is not allowed.")
	return *this;
}	// CommandResultIfc::operator =


CommandResultIfc::~CommandResultIfc ( )
{
}	// CommandResultIfc::~CommandResultIfc


/*----------------------------------------------------------------------------*/

}	// namespace Utils
/*----------------------------------------------------------------------------*/

}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/

