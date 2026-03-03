//
// Created by calderans on 21/01/2026.
//

#include "Topo/CommandAero.h"

#include <utility>

#include "gmds/aero/AeroPipeline_3D.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/TopoManager.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandAero::CommandAero(Internal::Context &c, std::string filename) : CommandCreateTopo(c, "Génération d'une structure de blocs via la méthode Aero"),
    m_c(c), m_filename(std::move(filename)) {

}
/*----------------------------------------------------------------------------*/
CommandAero::~CommandAero() {

}
/*----------------------------------------------------------------------------*/
void CommandAero::internalExecute() {
    int pos = m_filename.find_last_of('/');
    std::string dir = m_filename.substr(0, pos+1);
    gmds::AeroPipeline_3D* pipeline = new gmds::AeroPipeline_3D(m_filename,dir);

    pipeline->execute();
    gmds::Blocking3D *blocking = pipeline->getBlocking();

    for (auto n : blocking->nodes()) {
        gmds::Node node = blocking->get<gmds::Node>(n);

        if (!node.getIDs<gmds::Region>().empty()) {
            Point p(node.X(), node.Y(), node.Z());

            Vertex *vtx = new Vertex(m_c, p);

            m_gmdsNode2TopoNode[n] = vtx;
            getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
            m_group_helper.addToGroup("Aero_0D", vtx);
        }
    }
    for (auto e : blocking->edges()) {
        gmds::Edge edge = blocking->get<gmds::Edge>(e);

        int n0 = edge.getIDs<gmds::Node>()[0];
        int n1 = edge.getIDs<gmds::Node>()[1];

        Vertex *v0 = m_gmdsNode2TopoNode[n0];
        Vertex *v1 = m_gmdsNode2TopoNode[n1];
        EdgeMeshingPropertyUniform* emp = new EdgeMeshingPropertyUniform(10);
        CoEdge *coedge = new CoEdge(m_c, emp, v0, v1);
        m_gmdsEdge2TopoEdge[e] = coedge;
        getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Aero_1D", coedge);
    }
    for (auto f : blocking->faces()) {
        gmds::Face face = blocking->get<gmds::Face>(f);

        int e0 = face.getIDs<gmds::Edge>()[0];
        int e1 = face.getIDs<gmds::Edge>()[1];
        int e2 = face.getIDs<gmds::Edge>()[2];
        int e3 = face.getIDs<gmds::Edge>()[3];


        std::vector<Topo::Edge*> edges;
        Edge* edge0 = new Edge(m_c, m_gmdsEdge2TopoEdge[e0]);
            getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
        Edge* edge1 = new Edge(m_c, m_gmdsEdge2TopoEdge[e1]);
            getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
        Edge* edge2 = new Edge(m_c, m_gmdsEdge2TopoEdge[e2]);
            getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
        Edge* edge3 = new Edge(m_c, m_gmdsEdge2TopoEdge[e3]);
            getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);
        edges.push_back(edge0);
        edges.push_back(edge1);
        edges.push_back(edge2);
        edges.push_back(edge3);

        CoFace *coface = new CoFace(m_c, edges, true);
        getInfoCommand().addTopoInfoEntity(coface, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Aero_2D", coface);
        m_gmdsFace2TopoFace[f] = coface;
    }
    for (auto r : blocking->regions()) {
        gmds::Region region = blocking->get<gmds::Region>(r);
        std::vector<gmds::TCellID> gnodesID = region.getIDs<gmds::Node>();
        std::vector<gmds::TCellID> nodesID;
        nodesID.resize(8);
        nodesID[0] = region.getIDs<gmds::Node>()[0];
        nodesID[1] = region.getIDs<gmds::Node>()[3];
        nodesID[2] = region.getIDs<gmds::Node>()[1];
        nodesID[3] = region.getIDs<gmds::Node>()[2];
        nodesID[4] = region.getIDs<gmds::Node>()[4];
        nodesID[5] = region.getIDs<gmds::Node>()[7];
        nodesID[6] = region.getIDs<gmds::Node>()[5];
        nodesID[7] = region.getIDs<gmds::Node>()[6];


        std::vector<gmds::TCellID> facesID;
        facesID.resize(6);
        //On reordonne les faces gmds pour correspondre aux faces mgx
        facesID[0] = region.getIDs<gmds::Face>()[2];
        facesID[1] = region.getIDs<gmds::Face>()[3];
        facesID[2] = region.getIDs<gmds::Face>()[4];
        facesID[3] = region.getIDs<gmds::Face>()[5];
        facesID[4] = region.getIDs<gmds::Face>()[0];
        facesID[5] = region.getIDs<gmds::Face>()[1];

        std::vector<Topo::Vertex*> vertices;
        std::vector<std::string> nvertices;
        std::vector<std::string> gvertices;

        for (auto n : nodesID) {
            vertices.push_back(m_gmdsNode2TopoNode[n]);
        }

        std::vector<Topo::Face*> faces;
        for (auto f : facesID) {
            Face* topoface = new Face(m_c,m_gmdsFace2TopoFace[f]);
            getInfoCommand().addTopoInfoEntity(topoface, Internal::InfoCommand::CREATED);
            faces.push_back(topoface);
        }

        Block* block = new Block(m_c, faces, vertices, true);
        block->structure(&getInfoCommand());
        m_gmdsBlock2TopoBlock[r] = block;
        getInfoCommand().addTopoInfoEntity(block, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Aero_3D", block);
    }
    free(pipeline);

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();
}

void CommandAero::getPreviewRepresentation(Utils::DisplayRepresentation &dr) {

}
}
}