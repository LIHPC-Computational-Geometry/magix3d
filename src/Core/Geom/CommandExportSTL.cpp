/*----------------------------------------------------------------------------*/
/*
 * CommandExportSTL.cpp
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include <Geom/CommandExportSTL.h>
#include "Geom/GeomManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExportSTL::
CommandExportSTL(Internal::Context& context,
		GeomEntity* geomEntity,
		const std::string& fileName)
: Internal::CommandInternal(context, "Export STL")
, m_impl(context,geomEntity,fileName)
{
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Export STL de "<<geomEntity->getName();
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandExportSTL::~CommandExportSTL()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportSTL::
internalExecute()
{
    // écriture du STL
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
