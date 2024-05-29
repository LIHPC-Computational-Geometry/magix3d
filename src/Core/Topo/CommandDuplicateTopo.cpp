/*----------------------------------------------------------------------------*/
/*
 * \file CommandDuplicateTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21/3/2013
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandDuplicateTopo.h"

#include "Geom/CommandGeomCopy.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"

#include "Utils/Common.h"

#include "Topo/TopoHelper.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"

#include "Group/Group3D.h"
#include "Group/Group2D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_DUPTOPO
/*----------------------------------------------------------------------------*/
CommandDuplicateTopo::
CommandDuplicateTopo(Internal::Context& c,
		Geom::CommandGeomCopy* geom_cmd)
:CommandEditTopo(c, "Copies de topologies")
, m_geomCmd(geom_cmd)
, m_volume(0)
{
	// récupération des entités topologiques référencées par la géométrie concernée par la copie
	std::list<TopoEntity*> l_te;
	const std::vector<Geom::GeomEntity*>& geom_entities = geom_cmd->getEntities();
	for (std::vector<Geom::GeomEntity*>::const_iterator iter = geom_entities.begin();
	iter != geom_entities.end(); ++iter){
		const std::vector<Topo::TopoEntity* >& refTopo = (*iter)->getRefTopo();

		l_te.insert(l_te.end(), refTopo.begin(), refTopo.end());
	}


	l_te.sort(Utils::Entity::compareEntity);
	l_te.unique();

	m_entities.insert(m_entities.end(),l_te.begin(),l_te.end());

}
/*----------------------------------------------------------------------------*/
CommandDuplicateTopo::CommandDuplicateTopo(Internal::Context& c,
        std::vector<Block*>& vb,
        Geom::Volume* vol)
:CommandEditTopo(c, "Copie d'une topologie")
, m_geomCmd(0)
, m_volume(vol)
{
	for (std::vector<Block*>::iterator iter=vb.begin(); iter!=vb.end(); ++iter)
		m_entities.push_back(*iter);
}
/*----------------------------------------------------------------------------*/
CommandDuplicateTopo::CommandDuplicateTopo(Internal::Context& c,
        std::vector<Block*>& vb,
        const std::string& ng)
:CommandEditTopo(c, "Copie d'une topologie")
, m_geomCmd(0)
, m_volume(0)
, m_groupName(ng)
{
	for (std::vector<Block*>::iterator iter=vb.begin(); iter!=vb.end(); ++iter)
		m_entities.push_back(*iter);
}
/*----------------------------------------------------------------------------*/
CommandDuplicateTopo::
~CommandDuplicateTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandDuplicateTopo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

#ifdef _DEBUG_DUPTOPO
    std::cout<<"CommandDuplicateTopo::internalExecute() ..."<<std::endl;
#endif

    // duplique toutes les entités topologiques sélectionnées et les entités de niveau inférieur
    duplicate();

    // met à jour les interpolations
    updateInterpolate();

    // met à jour les groupes topo
    updateGroups();

#ifdef _DEBUG_DUPTOPO
    std::cout<<"CommandDuplicateTopo::internalExecute() FIN"<<std::endl;
#endif

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::get(std::vector<Topo::Block*>& blocs)
{
	for (std::vector<TopoEntity*>::iterator iter = m_entities.begin();
			iter != m_entities.end(); ++iter)
		if ((*iter)->getDim() == 3)
			blocs.push_back(m_corr_block[(Block*)(*iter)]);
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::get(std::vector<Topo::CoFace*>& cofaces)
{
	for (std::vector<TopoEntity*>::iterator iter = m_entities.begin();
			iter != m_entities.end(); ++iter)
		if ((*iter)->getDim() == 2)
			cofaces.push_back(m_corr_coface[(CoFace*)(*iter)]);
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::
duplicate()
{

    for (std::vector<TopoEntity*>::iterator iter = m_entities.begin();
            iter != m_entities.end(); ++iter){

        if ((*iter)->getDim() == 3)
            duplicate((Block*)(*iter));
        else if ((*iter)->getDim() == 2)
            duplicate((CoFace*)(*iter));
        else if ((*iter)->getDim() == 1)
            duplicate((CoEdge*)(*iter));
        else if ((*iter)->getDim() == 0)
            duplicate((Vertex*)(*iter));

    } // end for iter

}
/*----------------------------------------------------------------------------*/
Vertex* CommandDuplicateTopo::
duplicate(Vertex* ve)
{
    Vertex* new_vtx = m_corr_vertex[ve];
    if (new_vtx == 0){

        new_vtx = new Vertex(getContext(), ve->getCoord());

        MAJRelation(ve->getGeomAssociation(), new_vtx);

        m_corr_vertex[ve] = new_vtx;

        getInfoCommand().addTopoInfoEntity(new_vtx, Internal::InfoCommand::CREATED);
    }
    return new_vtx;
}
/*----------------------------------------------------------------------------*/
CoEdge* CommandDuplicateTopo::
duplicate(CoEdge* ce)
{
    CoEdge* new_coedge = m_corr_coedge[ce];
    if (new_coedge == 0){
        Vertex* new_vtx1 = duplicate(ce->getVertex(0));
        Vertex* new_vtx2 = duplicate(ce->getVertex(1));

        CoEdgeMeshingProperty* cemp = ce->getMeshingProperty()->clone();
        new_coedge = new CoEdge(getContext(), cemp, new_vtx1, new_vtx2);

        MAJRelation(ce->getGeomAssociation(), new_coedge);

        m_corr_coedge[ce] = new_coedge;

        getInfoCommand().addTopoInfoEntity(new_coedge, Internal::InfoCommand::CREATED);
    }
    return new_coedge;
}
/*----------------------------------------------------------------------------*/
Edge* CommandDuplicateTopo::
duplicate(Edge* ed)
{
    Edge* new_edge = m_corr_edge[ed];
    if (new_edge == 0){
        Vertex* new_vtx1 = duplicate(ed->getVertex(0));
        Vertex* new_vtx2 = duplicate(ed->getVertex(1));

        const std::vector<CoEdge*> & coedges = ed->getCoEdges();
        std::vector<CoEdge*> new_coedges;
        for (std::vector<CoEdge*>::const_iterator iter = coedges.begin();
                iter != coedges.end(); ++iter)
            new_coedges.push_back(duplicate(*iter));

        new_edge = new Edge(getContext(), new_vtx1, new_vtx2, new_coedges);

        // reprise des ratios
        for (std::vector<CoEdge*>::const_iterator iter = coedges.begin();
                        iter != coedges.end(); ++iter){
            uint ratio = ed->getRatio(*iter);
            if (ratio > 1)
                new_edge->setRatio(duplicate(*iter), ratio);
        }

        MAJRelation(ed->getGeomAssociation(), new_edge);

        m_corr_edge[ed] = new_edge;

        getInfoCommand().addTopoInfoEntity(new_edge, Internal::InfoCommand::CREATED);
    }
    return new_edge;
}
/*----------------------------------------------------------------------------*/
CoFace* CommandDuplicateTopo::
duplicate(CoFace* cf)
{
    CoFace* new_coface = m_corr_coface[cf];

    if (new_coface == 0){

        const std::vector<Vertex*> & vertices = cf->getVertices();
        std::vector<Vertex*> new_vertices;
        for (std::vector<Vertex*>::const_iterator iter = vertices.begin();
                iter != vertices.end(); ++iter)
            new_vertices.push_back(duplicate(*iter));

        const std::vector<Edge*> & edges = cf->getEdges();
        std::vector<Edge*> new_edges;
        for (std::vector<Edge*>::const_iterator iter = edges.begin();
                iter != edges.end(); ++iter)
            new_edges.push_back(duplicate(*iter));

        new_coface = new CoFace(getContext(), new_edges, new_vertices, cf->isStructured(), cf->hasHole());

        // copie des propriétés
        CoFaceMeshingProperty* new_ppty = cf->getCoFaceMeshingProperty()->clone();
        CoFaceMeshingProperty* old_ppty = new_coface->setProperty(new_ppty);
        delete old_ppty;

        MAJRelation(cf->getGeomAssociation(), new_coface);

        m_corr_coface[cf] = new_coface;

        getInfoCommand().addTopoInfoEntity(new_coface, Internal::InfoCommand::CREATED);
    }

    return new_coface;
}
/*----------------------------------------------------------------------------*/
Face* CommandDuplicateTopo::
duplicate(Face* fa)
{
    Face* new_face = m_corr_face[fa];

    if (new_face == 0){

        const std::vector<Vertex*> & vertices = fa->getVertices();
        std::vector<Vertex*> new_vertices;
        for (std::vector<Vertex*>::const_iterator iter = vertices.begin();
                iter != vertices.end(); ++iter)
            new_vertices.push_back(duplicate(*iter));

        const std::vector<CoFace*> & cofaces = fa->getCoFaces();
        std::vector<CoFace*> new_cofaces;
        for (std::vector<CoFace*>::const_iterator iter = cofaces.begin();
                iter != cofaces.end(); ++iter)
            new_cofaces.push_back(duplicate(*iter));

        new_face = new Face(getContext(), new_cofaces, new_vertices, fa->isStructured());

        // reprise des ratios
        for (uint i=0; i<2; i++)
            for (std::vector<CoFace*>::const_iterator iter = cofaces.begin();
                    iter != cofaces.end(); ++iter){
                uint ratio = fa->getRatio(*iter, i);
                if (ratio > 1)
                    new_face->setRatio(duplicate(*iter), ratio, i);
            }

        MAJRelation(fa->getGeomAssociation(), new_face);

        m_corr_face[fa] = new_face;

        getInfoCommand().addTopoInfoEntity(new_face, Internal::InfoCommand::CREATED);
    }

    return new_face;
}

/*----------------------------------------------------------------------------*/
Block* CommandDuplicateTopo::
duplicate(Block* bl)
{
    Block* new_block = m_corr_block[bl];

    if (new_block == 0){

        const std::vector<Vertex*> & vertices = bl->getVertices();
        std::vector<Vertex*> new_vertices;
        for (std::vector<Vertex*>::const_iterator iter = vertices.begin();
                iter != vertices.end(); ++iter)
            new_vertices.push_back(duplicate(*iter));

        const std::vector<Face*> & faces = bl->getFaces();
        std::vector<Face*> new_faces;
        for (std::vector<Face*>::const_iterator iter = faces.begin();
                iter != faces.end(); ++iter)
            new_faces.push_back(duplicate(*iter));

        new_block = new Block(getContext(), new_faces, new_vertices, bl->isStructured());

        // copie des propriétés
        BlockMeshingProperty* new_ppty = bl->getBlockMeshingProperty()->clone();
        BlockMeshingProperty* old_ppty = new_block->setProperty(new_ppty);
        delete old_ppty;

        MAJRelation(bl->getGeomAssociation(), new_block);

        m_corr_block[bl] = new_block;

        getInfoCommand().addTopoInfoEntity(new_block, Internal::InfoCommand::CREATED);

        if (!m_groupName.empty()){
	    	Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());
			grp->add(new_block);
			new_block->getGroupsContainer().add(grp);
		}
    }

    return new_block;
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::
MAJRelation(Geom::GeomEntity* ge, TopoEntity* te)
{
	if (m_geomCmd){
		if (ge){
			Geom::GeomEntity* ge_cpy = m_geomCmd->getRef2NewEntities()[ge];
#ifdef _DEBUG_DUPTOPO
			std::cout << "MAJRelation("<<ge->getName()<<", "<<te->getName()<<") donne "
					<<(ge_cpy?ge_cpy->getName():"rien")<<std::endl;
#endif
			te->setGeomAssociation(ge_cpy);
		}
	}
	else if (m_volume) {
		if (te->getDim() == 3){
			te->setGeomAssociation(m_volume);
		}
	} else if (ge) {
		te->setGeomAssociation(0);
	}
}

/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::updateInterpolate()
{
	for (std::map<CoEdge*, CoEdge*>::iterator iter = m_corr_coedge.begin();
			iter!=m_corr_coedge.end(); ++iter){
		CoEdge* old_coedge = iter->first;
		CoEdge* new_coedge = iter->second;

		EdgeMeshingPropertyInterpolate* interpol_old = dynamic_cast<EdgeMeshingPropertyInterpolate*>(old_coedge->getMeshingProperty());

		if (interpol_old){
			EdgeMeshingPropertyInterpolate* interpol_new = dynamic_cast<EdgeMeshingPropertyInterpolate*>(new_coedge->getMeshingProperty());
			if (interpol_new == 0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message<<"Erreur interne avec l'arête "<<old_coedge->getName()<<" et sa copie "<<new_coedge->getName()<<", une seule est interpolée";
				throw TkUtil::Exception (message);
			}

			std::vector<std::string> old_coedges_names = interpol_old->getCoEdges();
			if (!old_coedges_names.empty()){

				std::vector<std::string> new_coedges_names;

				for (uint i=0; i<old_coedges_names.size(); i++){
					CoEdge* old_coedge_ref = getContext().getLocalTopoManager().getCoEdge(old_coedges_names[i], false);
					if (old_coedge_ref == 0){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message<<"Erreur interne avec l'arête "<<old_coedge->getName()<<", on ne trouve pas l'arête "
								<<old_coedges_names[i]<<" référencée par l'interpolation ";
						throw TkUtil::Exception (message);
					}
					CoEdge* new_coedge_ref = m_corr_coedge[old_coedge_ref];
					if (new_coedge_ref == 0){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message<<"Erreur interne avec l'arête "<<old_coedge_ref->getName()<<", on ne trouve pas de copie de l'arête référencée";
						throw TkUtil::Exception (message);
					}

					new_coedges_names.push_back(new_coedge_ref->getName());
				} // end for i

				interpol_new->setCoEdges(new_coedges_names);

			} // if (!old_coedges_names.empty())

			std::string old_coface_name = interpol_old->getCoFace();
			if (!old_coface_name.empty()){
				CoFace* old_coface = getContext().getLocalTopoManager().getCoFace(old_coface_name, false);

				if (old_coface == 0){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
					message<<"Erreur interne avec la face "<<old_coedge->getName()<<", on ne trouve pas la face "
							<<old_coface_name<<" référencée par l'interpolation";
					throw TkUtil::Exception (message);
				}
				CoFace* new_coface = m_corr_coface[old_coface];
				if (new_coface == 0){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
					message<<"Erreur interne avec l'arête "<<old_coedge->getName()<<", on ne trouve pas de copie de la face référencée "<<old_coface_name;
					throw TkUtil::Exception (message);
				}
				std::string new_coface_name = new_coface->getName();
				interpol_new->setCoFace(new_coface_name);

			} // if (!old_coface_name.empty())

		} // if (interpol_old)

	} // for (iter...

}
/*----------------------------------------------------------------------------*/
void CommandDuplicateTopo::updateGroups()
{
	for (std::map<Block*, Block*>::iterator iter = m_corr_block.begin();
				iter!=m_corr_block.end(); ++iter){
		Block* t1 = (*iter).first;
		Block* t2 = (*iter).second;
		std::vector<Group::Group3D*> grp;
		t1->getGroups(grp);
		for (std::vector<Group::Group3D*>::iterator iter2 = grp.begin();
                iter2 != grp.end(); ++iter2){
			(*iter2)->add(t2);
			t2->getGroupsContainer().add(*iter2);
		}
	}
	for (std::map<CoFace*, CoFace*>::iterator iter = m_corr_coface.begin();
				iter!=m_corr_coface.end(); ++iter){
		CoFace* t1 = (*iter).first;
		CoFace* t2 = (*iter).second;
		std::vector<Group::Group2D*> grp;
		t1->getGroups(grp);
		for (std::vector<Group::Group2D*>::iterator iter2 = grp.begin();
                iter2 != grp.end(); ++iter2){
			(*iter2)->add(t2);
			t2->getGroupsContainer().add(*iter2);
		}
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
