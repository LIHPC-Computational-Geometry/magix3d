/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetEdgeMeshingPropertyToParallelCoEdges.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetEdgeMeshingPropertyToParallelCoEdges.h"

#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <set>
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandSetEdgeMeshingPropertyToParallelCoEdges::
CommandSetEdgeMeshingPropertyToParallelCoEdges(Internal::Context& c, CoEdgeMeshingProperty& emp, CoEdge* ed)
:CommandEditTopo(c, std::string("Changement de discrétisation pour ") + ed->getName())
, m_prop(emp.clone())
, m_coedge(ed)
{
}
/*----------------------------------------------------------------------------*/
CommandSetEdgeMeshingPropertyToParallelCoEdges::
~CommandSetEdgeMeshingPropertyToParallelCoEdges()
{
    delete m_prop;
}
/*----------------------------------------------------------------------------*/
void CommandSetEdgeMeshingPropertyToParallelCoEdges::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSetEdgeMeshingPropertyToParallelCoEdges::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // stockage des arêtes en cours de traitement
    std::vector<CoEdge*> coedges_in;

    // stockage des arêtes parallèles et du sens / discrétisation à cloner
    std::map<CoEdge*, bool> parallel_coedges;

    // filtre sur les coedges pour éviter de boucler indéfiniement
     std::set<CoEdge*> filtre_coedge;

    // on initialise avec celle passée en paramètre
     if (m_coedge){
    	 coedges_in.push_back(m_coedge);
    	 parallel_coedges[m_coedge] = false;
    	 filtre_coedge.insert(m_coedge);
     }

     // boucle tant que l'on a des arêtes non traitées
     while (!coedges_in.empty()){

    	 CoEdge* coedge_dep = coedges_in.back();
    	 coedges_in.pop_back();

    	 // boucle sur les cofaces autour
    	 std::vector<CoFace*> cofaces;
    	 coedge_dep->getCoFaces(cofaces);

    	 for (uint i=0; i<cofaces.size(); i++){
    		 CoFace* coface = cofaces[i];

    		 bool inverse_sens;
    		 CoEdge* coedge_vois = coface->getOppositeCoEdge(coedge_dep, inverse_sens);

    		 if (coedge_vois && filtre_coedge.find(coedge_vois)==filtre_coedge.end()){
    			 coedges_in.push_back(coedge_vois);
    			 if (parallel_coedges[coedge_dep])
    				 parallel_coedges[coedge_vois] = inverse_sens;
    			 else
    				 parallel_coedges[coedge_vois] = !inverse_sens;
    			 // pour ne pas la reprendre
    			 filtre_coedge.insert(coedge_vois);
    		 }
    	 } // end for i
     } // end while

     // remplace la propriété (fait une copie) et met l'ancienne dans la sauvegarde interne
     for (std::map<CoEdge*, bool>::iterator iter = parallel_coedges.begin();
    		 iter != parallel_coedges.end(); ++iter){
    	 CoEdge* coedge = (*iter).first;
    	 bool inverse_sens = (*iter).second;

    	 if (inverse_sens)
    		 m_prop->setDirect(!m_prop->getDirect());

    	 coedge->switchCoEdgeMeshingProperty(&getInfoCommand(), m_prop);

    	 if (inverse_sens)
    		 m_prop->setDirect(!m_prop->getDirect());

    	 if (m_prop->needGeomUpdate())
    		 coedge->getMeshingProperty()->update(coedge->getGeomAssociation());
     }

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSetEdgeMeshingPropertyToParallelCoEdges::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
