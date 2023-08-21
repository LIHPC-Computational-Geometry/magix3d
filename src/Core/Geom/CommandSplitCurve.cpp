/*
 * CommandSplitCurve.cpp
 *
 *  Created on: 12/4/2018
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandSplitCurve.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomSplitCurveImplementation.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandSplitCurve::
CommandSplitCurve(Internal::Context& c, Curve* crv, const Point& pt)
: CommandEditGeom(c, "SplitCurve")
{
    m_impl = new GeomSplitCurveImplementation(c, crv, pt);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Découpage de "<<crv->getName() << " auprès de "<<pt;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitCurve::~CommandSplitCurve()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandSplitCurve::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandSplitCurve::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
