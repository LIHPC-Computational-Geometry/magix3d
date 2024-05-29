/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitEdge.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/11/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSplitEdge.h"

#include "Utils/Common.h"
#include "Topo/CoEdge.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandSplitEdge::
CommandSplitEdge(Internal::Context& c, CoEdge* arete, double ratio_dec)
:CommandEditTopo(c, std::string("Découpage de l'arête ") + arete->getName())
, m_arete(arete)
, m_ratio_dec(ratio_dec)
, m_use_point(false)
{
#ifdef _DEBUG2
	std::cout<<"CommandSplitEdge::CommandSplitEdge avec arête "<<(arete?arete->getName():"VIDE")<<" et ratio de "<<ratio_dec<<std::endl;
#endif
    if (ratio_dec<=0.0 || ratio_dec>=1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval ]0 1[", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandSplitEdge::
CommandSplitEdge(Internal::Context& c, CoEdge* arete, const Point& pt)
:CommandEditTopo(c, std::string("Découpage de l'arête ") + arete->getName())
, m_arete(arete)
, m_ratio_dec(0)
, m_use_point(true)
, m_pt(pt)
{
#ifdef _DEBUG2
	std::cout<<"CommandSplitEdge::CommandSplitEdge avec arête "<<(arete?arete->getName():"VIDE")<<" et point de "<<pt<<std::endl;
#endif
	 m_ratio_dec = arete->computeRatio(pt);

    if (m_ratio_dec<=0.0 || m_ratio_dec>=1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval ]0 1[", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandSplitEdge::
~CommandSplitEdge()
{
#ifdef _DEBUG2
	std::cout<<"CommandSplitEdge::~CommandSplitEdge"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSplitEdge::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSplitEdge::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    if (m_arete->getNbMeshingEdges() == 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("On ne peut couper une arête avec un seul bras pour le maillage", TkUtil::Charset::UTF_8));

    uint ni_loc = (uint)((double)m_arete->getNbMeshingEdges() * m_ratio_dec);
    if (ni_loc == 0)
        ni_loc = 1;
    else if (ni_loc == m_arete->getNbMeshingEdges())
        ni_loc = m_arete->getNbMeshingEdges() - 1;

    // on coupe l'arête
    Vertex* new_vtx = m_arete->split(ni_loc, &getInfoCommand());

    if (m_use_point)
    	new_vtx->setCoord(m_pt);

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSplitEdge::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
