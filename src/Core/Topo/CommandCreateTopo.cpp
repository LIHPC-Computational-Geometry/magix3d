/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Topo/CommandCreateTopo.h"
#include "Topo/TopoManager.h"
#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/Edge.h"
#include "Topo/Face.h"
#include "Topo/Block.h"
#include "Geom/Volume.h"
#include "Geom/CommandCreateGeom.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandCreateTopo::CommandCreateTopo(Internal::Context& c, std::string name)
: CommandEditTopo (c, name)
, m_blocks ( )
, m_cofaces ( )
, m_bounding_entities ( )
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

    Utils::EntitySet<Topo::Vertex*> l_v(&Utils::Entity::compareEntity);
    Utils::EntitySet<Topo::CoEdge*> l_ce(&Utils::Entity::compareEntity);
    Utils::EntitySet<Topo::Edge*> l_e(&Utils::Entity::compareEntity);
    Utils::EntitySet<Topo::CoFace*> l_cf(&Utils::Entity::compareEntity);
    Utils::EntitySet<Topo::Face*> l_f(&Utils::Entity::compareEntity);

    for (Block* block : m_blocks) {
        auto vertices = block->getVertices();
        auto coedges = block->getCoEdges();
        auto cofaces = block->getCoFaces();
        l_v.insert(vertices.begin(), vertices.end());
        l_ce.insert(coedges.begin(), coedges.end());
        l_cf.insert(cofaces.begin(), cofaces.end());

        // entités pour lesquelles il n'est pas prévu de les représenté ni de les utiliser
        // mais pour lesquelles il est utile de les marquer comme CREATED
        auto edges = block->getEdges();
        auto faces = block->getFaces();
        l_e.insert(edges.begin(), edges.end());
        l_f.insert(faces.begin(), faces.end());
    }

    for (CoFace* coface : m_cofaces) {
        const std::vector<Topo::Vertex* > & vertices = coface->getVertices();
        std::vector<Topo::CoEdge* > coedges;
        coface->getCoEdges(coedges);

        l_v.insert(vertices.begin(), vertices.end());
        l_ce.insert(coedges.begin(), coedges.end());
        l_cf.insert(coface);

        // entités pour lesquelles il n'est pas prévu de les représenté ni de les utiliser
        // mais pour lesquelles il est utile de les marquer comme CREATED
        std::vector<Topo::Edge* > edges = coface->getEdges();
        l_e.insert(edges.begin(), edges.end());
    }

    // enregistrement dans InfoCommand de toutes les entités topologiques associées
    // aux différents blocs
    Internal::InfoCommand& icmd = getInfoCommand();
    for (Block* block : m_blocks)
        icmd.addTopoInfoEntity(block, Internal::InfoCommand::CREATED);

    for (Topo::Vertex* vertex : l_v)
        icmd.addTopoInfoEntity(vertex, Internal::InfoCommand::CREATED);

    for (Topo::Edge* edge : l_e)
        icmd.addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);

    for (Topo::Face* face: l_f)
        icmd.addTopoInfoEntity(face, Internal::InfoCommand::CREATED);

    for (Topo::CoEdge* coedge : l_ce)
        icmd.addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);

    for (Topo::CoFace* coface : l_cf)
        icmd.addTopoInfoEntity(coface, Internal::InfoCommand::CREATED);
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
