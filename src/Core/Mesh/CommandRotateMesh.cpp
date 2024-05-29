/*----------------------------------------------------------------------------*/
/*
 * \file CommandRotateMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandRotateMesh.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// OCC
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandRotateMesh::
CommandRotateMesh(Internal::Context& c,
        const Utils::Math::Rotation& rot)
:CommandTransformMesh(c, "Rotation de tout le maillage")
, m_rotation(rot)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Rotation de tout le maillage";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandRotateMesh::
~CommandRotateMesh()
{
}
/*----------------------------------------------------------------------------*/
void CommandRotateMesh::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandRotateMesh::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // création de l'opérateur de rotation via OCC
    gp_Trsf transf;
    Point axis1 = m_rotation.getAxis1();
    Point axis2 = m_rotation.getAxis2();

    gp_Pnt p1(axis1.getX(),axis1.getY(),axis1.getZ());
    gp_Dir dir( axis2.getX()-axis1.getX(),
                axis2.getY()-axis1.getY(),
                axis2.getZ()-axis1.getZ());
    gp_Ax1 axis(p1,dir);
    transf.SetRotation(axis, m_rotation.getAngle() * M_PI/180.0);

    // applique la transformation
    transform(&transf);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandRotateMesh::internalUndo()
{
	// on fait la commande inverse
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandRotateMesh::execute pour la commande " << getName ( )
	            		<< " de nom unique " << getUniqueName ( );

	// création de l'opérateur de rotation via OCC
	gp_Trsf transf;
	Point axis1 = m_rotation.getAxis1();
	Point axis2 = m_rotation.getAxis2();

	gp_Pnt p1(axis1.getX(),axis1.getY(),axis1.getZ());
	gp_Dir dir( axis2.getX()-axis1.getX(),
			axis2.getY()-axis1.getY(),
			axis2.getZ()-axis1.getZ());
	gp_Ax1 axis(p1,dir);
	transf.SetRotation(axis, -m_rotation.getAngle() * M_PI/180.0);

	// applique la transformation
	transform(&transf);

	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
