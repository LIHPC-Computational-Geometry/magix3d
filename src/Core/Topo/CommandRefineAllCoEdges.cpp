/*----------------------------------------------------------------------------*/
/*
 * \file CommandRefineAllCoEdges.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 26/2/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandRefineAllCoEdges.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"

#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandRefineAllCoEdges::
CommandRefineAllCoEdges(Internal::Context& c, int ratio)
:CommandEditTopo(c, "Raffinement des arêtes")
, m_ratio(ratio)
{
    if (ratio<=0)
         throw TkUtil::Exception (TkUtil::UTF8String ("Le raffinement ne peut se faire qu'avec un ratio entier strictement positif", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandRefineAllCoEdges::
~CommandRefineAllCoEdges()
{
}
/*----------------------------------------------------------------------------*/
void CommandRefineAllCoEdges::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandRefineAllCoEdges::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    std::vector<CoEdge*> coedges;
    getContext().getLocalTopoManager().getCoEdges(coedges);

    for (std::vector<CoEdge*>::iterator iter=coedges.begin();
    		iter!=coedges.end(); ++iter){
    	CoEdge* coedge = *iter;
    	CoEdgeMeshingProperty* mp = coedge->getMeshingProperty();
    	CoEdgeMeshingProperty* new_mp = mp->clone();
    	// on adapte la raison pour que le bras du début reste de la même longueur
    	if (new_mp->getMeshLaw() == CoEdgeMeshingProperty::geometrique){
    		EdgeMeshingPropertyGeometric* geom_prop = dynamic_cast<EdgeMeshingPropertyGeometric*>(new_mp);
    		CHECK_NULL_PTR_ERROR(geom_prop);
    		double raison = geom_prop->getRatio();
    		double new_raison = std::pow(raison, 1/(double)m_ratio);
    		geom_prop->setRatio(new_raison);
    	}

    	new_mp->setNbEdges(new_mp->getNbEdges()*m_ratio);
    	coedge->switchCoEdgeMeshingProperty(&getInfoCommand(), new_mp);
    	delete new_mp;
    }

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandRefineAllCoEdges::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
