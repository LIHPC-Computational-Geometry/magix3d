/*----------------------------------------------------------------------------*/
/*
 * \file CommandMirrorTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 12/4/2016
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandMirrorTopo.h"

#include "Utils/Common.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// OCC
#include <gp_Trsf.hxx>
#include <gp_Ax2.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandMirrorTopo::
CommandMirrorTopo(Internal::Context& c,
		std::vector<TopoEntity*>& ve,
		Utils::Math::Plane* plane)
:CommandTransformTopo(c, "Symétrie d'une topologie seule", ve)
, m_plane(plane)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Symétrie de la topologie seule ";
	for (uint i=0; i<ve.size() && i<5; i++)
		comments << " " << ve[i]->getName();
	if (ve.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandMirrorTopo::
CommandMirrorTopo(Internal::Context& c,
		CommandDuplicateTopo* cmd,
		Utils::Math::Plane* plane,
		bool all_topo)
:CommandTransformTopo(c, "Symétrie d'une copie de la topologie", cmd, all_topo)
, m_plane(plane)
{
}
/*----------------------------------------------------------------------------*/
CommandMirrorTopo::
CommandMirrorTopo(Internal::Context& c,
		Utils::Math::Plane* plane)
:CommandTransformTopo(c, "Symétrie de tout")
, m_plane(plane)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Symétrie de toute la topologie";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandMirrorTopo::
~CommandMirrorTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandMirrorTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandMirrorTopo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // création de l'opérateur d'homothétie via OCC
    gp_Trsf transf;
    Utils::Math::Point plane_pnt = m_plane->getPoint();
    Utils::Math::Vector plane_vec = m_plane->getNormal();
    transf.SetMirror(gp_Ax2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ())));

    // applique la transformation
    transform(&transf);

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandMirrorTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
