/*----------------------------------------------------------------------------*/
/*
 * \file CommandRotateSysCoord.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Utils/Vector.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include <SysCoord/CommandRotateSysCoord.h>
#include <SysCoord/CommandDuplicateSysCoord.h>
#include <SysCoord/SysCoordManager.h>

/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// OCC
#include "gp_Dir.hxx"
#include "gp_Ax3.hxx"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
CommandRotateSysCoord::CommandRotateSysCoord(Internal::Context& c,
		SysCoord* syscoord, const Utils::Math::Rotation& rot)
: CommandEditSysCoord (c, "Rotation d'un repère")
, m_syscoord(syscoord)
, m_command(0)
, m_rotation(rot)
{
}
/*----------------------------------------------------------------------------*/
CommandRotateSysCoord::CommandRotateSysCoord(Internal::Context& c,
		CommandDuplicateSysCoord* cmd, const Utils::Math::Rotation& rot)
: CommandEditSysCoord (c, "Rotation d'un repère")
, m_syscoord(0)
, m_command(cmd)
, m_rotation(rot)
{
}
/*----------------------------------------------------------------------------*/
CommandRotateSysCoord::~CommandRotateSysCoord()
{
}
/*----------------------------------------------------------------------------*/
void CommandRotateSysCoord::internalExecute()
{
	if (m_syscoord == 0 && m_command)
		m_syscoord = m_command->getNewSysCoord();

	CHECK_NULL_PTR_ERROR(m_syscoord)

	m_syscoord->saveProperty(&getInfoCommand());

    // création de l'opérateur de rotation via OCC
    gp_Trsf tr;
    Point axis1 = m_rotation.getAxis1();
    Point axis2 = m_rotation.getAxis2();

    gp_Pnt p1(axis1.getX(),axis1.getY(),axis1.getZ());
    gp_Dir dir( axis2.getX()-axis1.getX(),
                axis2.getY()-axis1.getY(),
                axis2.getZ()-axis1.getZ());
    gp_Ax1 axis(p1,dir);

    tr.SetRotation(axis, m_rotation.getAngle() * M_PI/180.0);

    // utilisation de 3 points caractéristiques du repère pour suivre la rotation
    Point pCentre = m_syscoord->toGlobal(Point(0,0,0));
    Point pX      = m_syscoord->toGlobal(Point(1,0,0));
    Point pY      = m_syscoord->toGlobal(Point(0,1,0));

    pCentre = transform(tr, pCentre);
    pX      = transform(tr, pX);
    pY      = transform(tr, pY);

    m_syscoord->getProperty()->initProperties(pCentre, pX, pY);

	m_syscoord->update();

	m_syscoord->saveInternals(this);
}
/*----------------------------------------------------------------------------*/
Point CommandRotateSysCoord::transform(gp_Trsf& tr, Point& pt)
{
	double x = pt.getX();
	double y = pt.getY();
	double z = pt.getZ();
	tr.Transforms(x, y, z);
	return Point(x,y,z);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

