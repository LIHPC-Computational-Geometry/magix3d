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
    //std::string filename = m_filename.substr(pos + 1);
    gmds::AeroPipeline_3D pipeline(m_filename,dir);

    pipeline.execute();
    gmds::Blocking3D* blocking(pipeline.getBlocking());

    std::cout<<"nb nodes "<<blocking->getNbNodes()<<std::endl;

    for (auto n : blocking->nodes()) {
        gmds::Node node = blocking->get<gmds::Node>(n);

        Point p(node.X(), node.Y(), node.Z());

        Vertex *vtx = new Vertex(m_c, p);

        m_gmdsNode2TopoNode[n] = vtx;
        getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Aero_0D", vtx);
    }
    std::cout<<"nb edges "<<blocking->getNbEdges()<<std::endl;
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
    std::cout<<"nb faces "<<blocking->getNbFaces()<<std::endl;
    for (auto f : blocking->faces()) {
        gmds::Face face = blocking->get<gmds::Face>(f);

        int e0 = face.getIDs<gmds::Edge>()[0];
        int e1 = face.getIDs<gmds::Edge>()[1];
        int e2 = face.getIDs<gmds::Edge>()[2];
        int e3 = face.getIDs<gmds::Edge>()[3];


        std::vector<Topo::Edge*> edges;
        Edge* edge0 = new Edge(m_c, m_gmdsEdge2TopoEdge[e0]);
        Edge* edge1 = new Edge(m_c, m_gmdsEdge2TopoEdge[e1]);
        Edge* edge2 = new Edge(m_c, m_gmdsEdge2TopoEdge[e2]);
        Edge* edge3 = new Edge(m_c, m_gmdsEdge2TopoEdge[e3]);
        edges.push_back(edge0);
        edges.push_back(edge1);
        edges.push_back(edge2);
        edges.push_back(edge3);

        CoFace *coface = new CoFace(m_c, edges, true);
        m_gmdsFace2TopoFace[f] = coface;
        getInfoCommand().addTopoInfoEntity(coface, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Aero_2D", coface);
    }
    std::cout<<"nb blocks "<<blocking->getNbRegions()<<std::endl;
    for (auto r : blocking->regions()) {
        gmds::Region region = blocking->get<gmds::Region>(r);
        std::vector<gmds::TCellID> nodesID = region.getIDs<gmds::Node>();
        std::vector<gmds::TCellID> facesID;
        facesID.reserve(6);
        //On reordonne les faces gmds pour correspondre aux faces mgx
        facesID[0] = region.getIDs<gmds::Face>()[0];
        facesID[1] = region.getIDs<gmds::Face>()[5];
        facesID[2] = region.getIDs<gmds::Face>()[1];
        facesID[3] = region.getIDs<gmds::Face>()[3];
        facesID[4] = region.getIDs<gmds::Face>()[2];
        facesID[5] = region.getIDs<gmds::Face>()[4];

        std::vector<Topo::Vertex*> vertices;
        for (auto n : nodesID) {
            vertices.push_back(m_gmdsNode2TopoNode[n]);
        }
        std::vector<Topo::Face*> faces;
        for (auto f : facesID) {
            Face* face = new Face(m_c,m_gmdsFace2TopoFace[f]);
            faces.push_back(face);
        }

        Block* block = new Block(m_c, faces, vertices, true);
        m_gmdsBlock2TopoBlock[r] = block;
        getInfoCommand().addTopoInfoEntity(block, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Aero_3D", block);
    }

}

void CommandAero::getPreviewRepresentation(Utils::DisplayRepresentation &dr) {

}
}
}