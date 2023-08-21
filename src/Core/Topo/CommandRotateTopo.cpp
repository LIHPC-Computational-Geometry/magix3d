/*----------------------------------------------------------------------------*/
/*
 * \file CommandRotateTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/9/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandRotateTopo.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Topo/Block.h"
#include "Topo/TopoHelper.h"
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
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandRotateTopo::
CommandRotateTopo(Internal::Context& c,
        std::vector<TopoEntity*>& ve,
        const Utils::Math::Rotation& rot)
:CommandTransformTopo(c, "Rotation d'une topologie seule", ve)
, m_rotation(rot)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Rotation de la topologie seule";
	for (uint i=0; i<ve.size() && i<5; i++)
		comments << " " << ve[i]->getName();
	if (ve.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandRotateTopo::
CommandRotateTopo(Internal::Context& c,
		CommandDuplicateTopo* cmd,
        const Utils::Math::Rotation& rot,
		bool all_topo)
:CommandTransformTopo(c, "Rotation d'une copie de la topologie", cmd, all_topo)
, m_rotation(rot)
{
}
/*----------------------------------------------------------------------------*/
CommandRotateTopo::
CommandRotateTopo(Internal::Context& c,
        const Utils::Math::Rotation& rot)
:CommandTransformTopo(c, "Rotation de tout")
, m_rotation(rot)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Rotation de toute la topologie";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandRotateTopo::
~CommandRotateTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandRotateTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandRotateTopo::execute pour la commande " << getName ( )
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

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandRotateTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
