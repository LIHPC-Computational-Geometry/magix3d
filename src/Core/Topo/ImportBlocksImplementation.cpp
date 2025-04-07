//
// Created by calderans on 4/3/25.
//

/*----------------------------------------------------------------------------*/
#include "Topo/ImportBlocksImplementation.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/

namespace Mgx3D {
namespace Topo {

/*----------------------------------------------------------------------------*/

ImportBlocksImplementation::ImportBlocksImplementation(Internal::Context &c, Internal::InfoCommand *icmd,
                            const std::string &n) : m_c(c), m_icmd(icmd), m_filename(n) {
}
/*----------------------------------------------------------------------------*/
ImportBlocksImplementation::~ImportBlocksImplementation() {}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::internalExecute() {

    // check file extension
    std::string suffix = m_filename;
    int suffix_start = m_filename.find_last_of(".");
    suffix.erase(0, suffix_start + 1);
    if (suffix != "blk")
        throw TkUtil::Exception(
                TkUtil::UTF8String("Mauvaise extension de fichier (.blk)", TkUtil::Charset::UTF_8));

    std::ifstream s(m_filename.c_str(), std::ios::in);
    if (!s) {
        std::string mess = "Impossible to read file " + m_filename;
        throw TkUtil::Exception(mess);
    }

    /*----------------------------------------------------------------------------*/
    if (!moveStreamOntoFirst(s, "POINTS")) {
        std::string mess = "BLK read error: no POINTS keyword found";
        throw TkUtil::Exception(mess);
    }
    int nb_nodes;
    std::string coord_type;
    s >> nb_nodes;

    std::vector<double> x, y, z;
    x.resize(nb_nodes);
    y.resize(nb_nodes);
    z.resize(nb_nodes);

    for (int i = 0; i < nb_nodes; i++) {
        s >> x[i] >> y[i] >> z[i];
    }

    /*----------------------------------------------------------------------------*/
    if (!moveStreamOntoFirst(s, "EDGES")) {
        std::string mess = "BLK read error: no EDGES keyword found";
        throw TkUtil::Exception(mess);
    }
    int nb_edges;
    s >> nb_edges;

    std::vector<int> n0, n1;

    for (int i = 0; i < nb_edges; i++) {
        s >> n0[i] >> n1[i];
    }

    if (!moveStreamOntoFirst(s, "Discr")) {
        std::string mess = "BLK read error: no Discr keyword found";
        throw TkUtil::Exception(mess);
    }
    int nb_discr;
    s >> nb_discr;
    if (nb_edges != nb_discr) {
        std::string mess = "BLK read error: number of discretization info different from number of edges";
        throw TkUtil::Exception(mess);
    }
    std::vector<CoEdgeMeshingProperty *> emps;
    emps.resize(nb_discr);
    int disc_type;
    int nb;
    for (int i = 0; i < nb_discr; i++) {
        s >> disc_type;
        if (disc_type == 0) {
            s >> nb;
            EdgeMeshingPropertyUniform emp(nb);
            emps[i] = &emp;
        } else if (disc_type == 1) {
            double raison;
            s >> nb >> raison;
            EdgeMeshingPropertyGeometric emp(nb, raison);
            emps[i] = &emp;
        } else {
            std::string mess = "BLK read error: type of discretization not supported";
            throw TkUtil::Exception(mess);
        }
    }

    /*----------------------------------------------------------------------------*/
    if (!moveStreamOntoFirst(s, "FACES")) {
        std::string mess = "BLK read error: no FACES keyword found";
        throw TkUtil::Exception(mess);
    }
    int nb_faces;
    s >> nb_faces;

    //std::vector<std::vector<int>> faces_edges;
    //faces_edges.resize(nb_faces);
    std::vector<std::vector<std::vector<int> > > faces_edges(nb_faces,
                                                             std::vector<std::vector<int> >(4,
                                                                                            std::vector<int>()));
    for (int i = 0; i < nb_faces; i++) {
        std::vector<int> fi_edges;
        std::string str;
        for (int e_i = 0; e_i < 4; e_i++) {
            s >> str;
            while (str == "]") {
                if (str == "[") {
                    s >> str;
                } else {
                    int e_id;
                    s >> e_id;
                    fi_edges.push_back(e_id);
                    faces_edges[i][e_i] = fi_edges;
                }
            }
        }
    }

    /*----------------------------------------------------------------------------*/

    // TODO lecture des blocs

    /*----------------------------------------------------------------------------*/
    std::vector<std::string> vnames;
    vnames.resize(nb_nodes);
    for (int i = 0; i < nb_nodes; i++) {
        Vertex *vtx = new Vertex(m_c, Utils::Math::Point(x[i], y[i], z[i]));
        vnames[i] = vtx->getName();
        m_icmd->addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
    }


    std::vector<std::string> enames;
    enames.resize(nb_edges);
    for (int i = 0; i < nb_edges; i++) {
        Vertex *v0 = m_c.getTopoManager().getVertex(vnames[n0[i]]);
        Vertex *v1 = m_c.getTopoManager().getVertex(vnames[n1[i]]);
        CoEdge *edge = new CoEdge(m_c, emps[i], v0, v1);
        enames[i] = edge->getName();
        m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
    }


    std::vector<std::string> fnames;
    fnames.resize(nb_faces);
    for (int i = 0; i < nb_faces; ++i) {

        std::vector<Edge *> edges;
        edges.resize(4);

        for (int j = 0; j < 4; ++j) {
            //Ici on créer les aretes de la face

            std::vector<int> coedges_in_face = faces_edges[i][j];
            std::vector<CoEdge *> coedges;
            coedges.resize(coedges_in_face.size());
            coedges[0] = m_c.getTopoManager().getCoEdge(enames[coedges_in_face[0]]);
            bool v1changed = false;
            std::string v1Edge = m_c.getTopoManager().getCoEdge(enames[coedges_in_face[0]])->getVertex(
                    0)->getName();
            std::string v2Edge = m_c.getTopoManager().getCoEdge(enames[coedges_in_face[0]])->getVertex(
                    1)->getName();

            for (int i_e = 1; i_e < coedges_in_face.size(); i_e++) {
                coedges[i_e] = m_c.getTopoManager().getCoEdge(enames[coedges_in_face[i_e]]);
                std::string v1current = m_c.getTopoManager().getCoEdge(enames[coedges_in_face[i_e]])->getVertex(
                        0)->getName();
                std::string v2current = m_c.getTopoManager().getCoEdge(enames[coedges_in_face[i_e]])->getVertex(
                        1)->getName();

                if (v1Edge == v1current) {
                    v1Edge = v2current;
                    v1changed = true;
                } else if (v1Edge == v2current) {
                    v1Edge = v1current;
                    v1changed = true;
                } else if (v2Edge == v1current) {
                    v2Edge = v2current;
                } else if (v2Edge == v2current) {
                    v2Edge = v1current;
                }
            }
            if (v1changed) {
                Vertex *v1 = m_c.getTopoManager().getVertex(v2Edge);
                Vertex *v2 = m_c.getTopoManager().getVertex(v1Edge);

                Edge *edge = new Edge(m_c, v1, v2, coedges);
                edges[j] = edge;
            }
        }

        CoFace *face = new CoFace(m_c, edges, true);
        fnames[i] = face->getName();
        m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);

    }

    // TODO Creation des blocs


    s.close();
}

/*----------------------------------------------------------------------------*/
bool ImportBlocksImplementation::moveStreamOntoFirst(std::ifstream &s, const std::string &AString) {
    //go to the beginning of the stream
    s.clear();
    s.seekg(0, std::ios::beg);
    std::string str;
    bool found = false;
    while (!found && (s) >> str) {
        found = (std::string::npos != str.find(AString));
    }

    return found;
}

    void ImportBlocksImplementation::getPreviewRepresentation(Utils::DisplayRepresentation &dr) {

    }


/*----------------------------------------------------------------------------*/
}
}
