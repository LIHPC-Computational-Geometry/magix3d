/*----------------------------------------------------------------------------*/
/** \file CommandCreateTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 16/12/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/InfoCommand.h"
#include "Topo/CommandCreateTopo.h"
#include "Topo/TopoManager.h"
#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/Edge.h"
#include "Topo/Face.h"
#include "Topo/Block.h"

#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/CommandCreateGeom.h"

#include "Utils/Command.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>

#include <list>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandCreateTopo::CommandCreateTopo(Internal::Context& c, std::string name)
: CommandEditTopo (c, name)
, m_geom_entity(0)
, m_command_create_geom(0)
, m_structured(true)
{}
/*----------------------------------------------------------------------------*/
CommandCreateTopo::~CommandCreateTopo()
{
//#ifdef _DEBUG
//    std::cout<<"CommandCreateTopo::~CommandCreateTopo() pour la commande "<<getScriptComments()<<std::endl;
//#endif
   //deleteCreated();
}
/*----------------------------------------------------------------------------*/
void CommandCreateTopo::addCreatedBlock(Block* b)
{
    // enregistrement dans la commande
    m_blocks.push_back(b);
}
/*----------------------------------------------------------------------------*/
void CommandCreateTopo::addCreatedCoFace(CoFace* c)
{
    // enregistrement dans la commande
    m_cofaces.push_back(c);
}
/*----------------------------------------------------------------------------*/
void CommandCreateTopo::split()
{
    // on utilise une liste pour cumuler les recherches sur les différents blocs
    // et pour faire un tri et supprimer les doublons

    std::list<Topo::Vertex*> l_v;
    std::list<Topo::CoEdge*> l_ce;
    std::list<Topo::Edge*> l_e;
    std::list<Topo::CoFace*> l_cf;
    std::list<Topo::Face*> l_f;

    for (std::vector<Block* >::const_iterator iter = m_blocks.begin();
            iter != m_blocks.end(); ++iter) {
        const std::vector<Topo::Vertex* > & vertices = (*iter)->getVertices();
        std::vector<Topo::CoEdge* > coedges;
        std::vector<Topo::CoFace* > cofaces;

        (*iter)->getCoEdges(coedges);
        (*iter)->getCoFaces(cofaces);

        l_v.insert(l_v.end(), vertices.begin(), vertices.end());
        l_ce.insert(l_ce.end(), coedges.begin(), coedges.end());
        l_cf.insert(l_cf.end(), cofaces.begin(), cofaces.end());

        // entités pour lesquelles il n'est pas prévu de les représenté ni de les utiliser
        // mais pour lesquelles il est utile de les marquer comme CREATED
        std::vector<Topo::Edge* > edges;
        const std::vector<Topo::Face* > & faces = (*iter)->getFaces();
        (*iter)->getEdges(edges);
        l_e.insert(l_e.end(), edges.begin(), edges.end());
        l_f.insert(l_f.end(), faces.begin(), faces.end());
    }

    for (std::vector<CoFace* >::const_iterator iter = m_cofaces.begin();
            iter != m_cofaces.end(); ++iter) {
        const std::vector<Topo::Vertex* > & vertices = (*iter)->getVertices();
        std::vector<Topo::CoEdge* > coedges;
        (*iter)->getCoEdges(coedges);

        l_v.insert(l_v.end(), vertices.begin(), vertices.end());
        l_ce.insert(l_ce.end(), coedges.begin(), coedges.end());
        l_cf.push_back(*iter);

        // entités pour lesquelles il n'est pas prévu de les représenté ni de les utiliser
        // mais pour lesquelles il est utile de les marquer comme CREATED
        std::vector<Topo::Edge* > edges;
        (*iter)->getEdges(edges);
        l_e.insert(l_e.end(), edges.begin(), edges.end());
    }


    l_v.sort(Utils::Entity::compareEntity);
    l_v.unique();
    l_e.sort(Utils::Entity::compareEntity);
    l_e.unique();
    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();
    l_ce.sort(Utils::Entity::compareEntity);
    l_ce.unique();
    l_cf.sort(Utils::Entity::compareEntity);
    l_cf.unique();

    // enregistrement dans InfoCommand de toutes les entités topologiques associées
    // aux différents blocs
    Internal::InfoCommand& icmd = getInfoCommand();

    for (std::vector<Block* >::const_iterator iter = m_blocks.begin();
                iter != m_blocks.end(); ++iter)
        icmd.addTopoInfoEntity(*iter, Internal::InfoCommand::CREATED);

    for (std::list<Topo::Vertex*>::const_iterator iter = l_v.begin();
            iter != l_v.end(); ++iter)
        icmd.addTopoInfoEntity(*iter, Internal::InfoCommand::CREATED);

    for (std::list<Topo::Edge*>::const_iterator iter = l_e.begin();
            iter != l_e.end(); ++iter)
        icmd.addTopoInfoEntity(*iter, Internal::InfoCommand::CREATED);

    for (std::list<Topo::Face*>::const_iterator iter = l_f.begin();
            iter != l_f.end(); ++iter)
        icmd.addTopoInfoEntity(*iter, Internal::InfoCommand::CREATED);

    for (std::list<Topo::CoEdge*>::const_iterator iter = l_ce.begin();
            iter != l_ce.end(); ++iter)
        icmd.addTopoInfoEntity(*iter, Internal::InfoCommand::CREATED);

    for (std::list<Topo::CoFace*>::const_iterator iter = l_cf.begin();
            iter != l_cf.end(); ++iter)
        icmd.addTopoInfoEntity(*iter, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateTopo::validGeomEntity()
{
    // cas où on utilise une commande pour y récupérer l'entité géométrique créée
    if (m_command_create_geom){
        std::vector<Geom::GeomEntity*> res;
        m_command_create_geom->getResult(res);
        if (res.empty())
        	throw TkUtil::Exception (TkUtil::UTF8String ("Commande de création d'entité ne retourne rien pour CommandCreateTopo", TkUtil::Charset::UTF_8));
        setGeomEntity(res[0]);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateTopo::getBlocks(std::vector<Block* >& blocks) const
{
    blocks.clear();
    for(unsigned int i=0; i <m_blocks.size();i++){
        blocks.push_back(m_blocks[i]);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateTopo::getCoFaces(std::vector<CoFace* >& cofaces) const
{
    cofaces.clear();
    for(unsigned int i=0; i <m_cofaces.size();i++){
        cofaces.push_back(m_cofaces[i]);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
