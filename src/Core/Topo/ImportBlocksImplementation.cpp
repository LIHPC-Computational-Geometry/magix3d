//
// Created by calderans on 4/3/25.
//

/*----------------------------------------------------------------------------*/
#include "Topo/ImportBlocksImplementation.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Internal/InfoCommand.h"

#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
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
//ImportBlocksImplementation::~ImportBlocksImplementation() {}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::internalExecute() {

    // check file extension
    std::string suffix = m_filename;
    int suffix_start = m_filename.find_last_of('.');
    suffix.erase(0, suffix_start + 1);
    if (suffix != "blk")
        throw TkUtil::Exception(
                TkUtil::UTF8String("Mauvaise extension de fichier (.blk)", TkUtil::Charset::UTF_8));

    std::ifstream s(m_filename.c_str(), std::ios::in);
    if (!s) {
        std::string mess = "Impossible to read file " + m_filename;
        throw TkUtil::Exception(mess);
    }

        Group::Group0D* group0 = getStdContext()->getGroupManager().getNewGroup0D("Hors_Groupe_0D", m_icmd);
        Group::Group1D* group1 = getStdContext()->getGroupManager().getNewGroup1D("Hors_Groupe_1D", m_icmd);
        Group::Group2D* group2 = getStdContext()->getGroupManager().getNewGroup2D("Hors_Groupe_2D", m_icmd);
        Group::Group3D* group3 = getStdContext()->getGroupManager().getNewGroup3D("Hors_Groupe_3D", m_icmd);

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


        std::vector<Vertex*> vertices;
        vertices.resize(nb_nodes);
        std::vector<std::string> vnames;
        vnames.resize(nb_nodes);
        for (int i = 0; i < nb_nodes; i++) {
            Vertex *vtx = new Vertex(m_c, Utils::Math::Point(x[i], y[i], z[i]));
            vertices[i] = vtx;
            vnames[i] = vtx->getName();
            m_icmd->addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
            group0->add(vtx);
            vtx->getGroups().push_back(group0);
        }


    /*----------------------------------------------------------------------------*/
    if (!moveStreamOntoFirst(s, "EDGES")) {
        std::string mess = "BLK read error: no EDGES keyword found";
        throw TkUtil::Exception(mess);
    }
    int nb_edges;
    s >> nb_edges;

    std::vector<int> n0, n1;
    n0.resize(nb_edges);
    n1.resize(nb_edges);

    for (int i = 0; i < nb_edges; i++) {
        s >> n0[i] >> n1[i];
    }

        std::vector<CoEdge*> coedges;
        coedges.resize(nb_edges);
        std::vector<std::string> enames;
        enames.resize(nb_edges);
        for (int i = 0; i < nb_edges; i++) {
            Vertex *v0 = vertices[n0[i]];
            Vertex *v1 = vertices[n1[i]];
            EdgeMeshingPropertyUniform* emp = new EdgeMeshingPropertyUniform(10);
            CoEdge *edge = new CoEdge(m_c, emp, v0, v1);
            coedges[i] = edge;
            enames[i] = edge->getName();
            m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
            group1->add(edge);
            edge->getGroups().push_back(group1);
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
    std::vector<CoEdgeMeshingProperty*> emps;
    emps.resize(nb_discr);
    int disc_type;
    int nb;
    for (int i = 0; i < nb_discr; i++) {
        s >> disc_type;
        if (disc_type == 0) {
            s >> nb;
            //EdgeMeshingPropertyUniform emp(nb);
            emps[i] = new EdgeMeshingPropertyUniform(nb);
        } else if (disc_type == 1) {
            double raison;
            s >> nb >> raison;
            //EdgeMeshingPropertyGeometric emp(nb, raison);
            emps[i] = new EdgeMeshingPropertyGeometric(nb, raison);
        } else if (disc_type == 6) {
            s >> nb;
            std::vector<std::vector<int>> interpolate_edges(2,std::vector<int>());

            std::string str;
            for (int e_i = 0; e_i < 2; e_i++) {
                std::vector<int> edges;
                s >> str;
                while (str != "]") {
                    if (str == "[") {
                        s >> str;
                    } else {
                        int e_id;
                        e_id = std::stoi(str);
                        edges.push_back(e_id);
                        s >> str;

                    }
                }
                interpolate_edges[e_i] = edges;
            }

            std::vector<std::string> firstCoedgesNames;
            firstCoedgesNames.resize(interpolate_edges[0].size());
            std::vector<std::string> secondCoedgesNames;
            secondCoedgesNames.resize(interpolate_edges[1].size());

            int indice = 0;
            for (auto e_id : interpolate_edges[0]) {
                firstCoedgesNames[indice] = enames[e_id];
                indice++;
            }
            indice = 0;
            for (auto e_id : interpolate_edges[1]) {
                secondCoedgesNames[indice] = enames[e_id];
                indice++;
            }
            emps[i] = new EdgeMeshingPropertyGlobalInterpolate(nb, firstCoedgesNames,secondCoedgesNames);
        } else {
            std::string mess = "BLK read error: type of discretization not supported";
            throw TkUtil::Exception(mess);
        }
    }

        for (int i=0; i<coedges.size(); i++) {
            coedges[i]->setProperty(emps[i]);
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

        std::string str;
        for (int e_i = 0; e_i < 4; e_i++) {
            std::vector<int> fi_edges;
            s >> str;
            while (str != "]") {
                if (str == "[") {
                    s >> str;
                } else {
                    int e_id;
                    e_id = std::stoi(str);
                    fi_edges.push_back(e_id);
                    s >> str;

                }
            }
            faces_edges[i][e_i] = fi_edges;
        }
    }

        std::vector<CoFace*> cofaces;
        cofaces.resize(nb_faces);
        std::vector<std::string> fnames;
        fnames.resize(nb_faces);
        for (int i = 0; i < nb_faces; ++i) {

            std::vector<Edge *> edges;
            edges.resize(4);

            for (int j = 0; j < 4; ++j) {
                //Ici on créer les aretes de la face

                std::vector<int> coedges_in_face = faces_edges[i][j];
                std::vector<CoEdge *> coedges_f;
                coedges_f.resize(coedges_in_face.size());
                coedges_f[0] = coedges[coedges_in_face[0]];
                bool v1changed = false;
                std::string v1Edge = coedges[coedges_in_face[0]]->getVertex(
                        0)->getName();
                std::string v2Edge = coedges[coedges_in_face[0]]->getVertex(
                        1)->getName();

                for (int i_e = 1; i_e < coedges_in_face.size(); i_e++) {
                    coedges_f[i_e] = coedges[coedges_in_face[i_e]];
                    std::string v1current = coedges[coedges_in_face[i_e]]->getVertex(
                            0)->getName();
                    std::string v2current = coedges[coedges_in_face[i_e]]->getVertex(
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
                int iv1, iv2;
                auto pos = std::find(vnames.begin(), vnames.end(), v1Edge);
                iv1 = pos-vnames.begin();
                pos = std::find(vnames.begin(), vnames.end(), v2Edge);
                iv2 = pos-vnames.begin();
                if (v1changed) {
                    Vertex *v1 = vertices[iv2];
                    Vertex *v2 = vertices[iv1];

                    Edge *edge = new Edge(m_c, v1, v2, coedges_f);
                    m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
                    edges[j] = edge;
                }else{
                    Vertex *v1 = vertices[iv1];
                    Vertex *v2 = vertices[iv2];

                    Edge *edge = new Edge(m_c, v1, v2, coedges_f);
                    m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
                    edges[j] = edge;
                }
            }

            CoFace *face = new CoFace(m_c, edges, true);
            cofaces[i] = face;
            fnames[i] = face->getName();
            m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);
            group2->add(face);
            face->getGroups().push_back(group2);
        }

    /*----------------------------------------------------------------------------*/

        if (!moveStreamOntoFirst(s, "BLOCKS")) {
            std::string mess = "BLK read error: no BLOCKS keyword found";
            throw TkUtil::Exception(mess);
        }
        int nb_blocks;
        s >> nb_blocks;

        std::vector<std::vector<int>> b_nodes(nb_blocks,
                                              std::vector<int>(8));
        std::vector<std::vector<std::vector<int> > > blocks_faces(nb_blocks,
                                                                 std::vector<std::vector<int> >(6,
                                                                                                std::vector<int>()));

        for (int i = 0; i < nb_blocks; i++) {

            for (int j = 0; j < 8; j++) {
                s >> b_nodes[i][j];
            }

            std::string str;
            for (int f_i = 0; f_i < 6; f_i++) {
                std::vector<int> bi_faces;
                s >> str;
                while (str != "]") {
                    if (str == "[") {
                        s >> str;
                    } else {
                        int f_id;
                        f_id = std::stoi(str);
                        bi_faces.push_back(f_id);
                        s >> str;

                    }
                }
                blocks_faces[i][f_i] = bi_faces;
            }
        }

    /*----------------------------------------------------------------------------*/

        for (int i = 0; i < nb_blocks; ++i) {

            std::vector<Face *> faces;
            //faces.resize(6);

            for (int j = 0; j < 6; ++j) {
                std::vector<int> cofaces_in_bloc = blocks_faces[i][j];
                std::vector<CoFace *> cofaces_f;
                cofaces_f.resize(cofaces_in_bloc.size());

                if(cofaces_in_bloc.size() > 1){

                    int indice = 0;
                    std::vector<std::string> name_vertices;
                    for(auto cf : cofaces_in_bloc){

                        CoFace* coface = cofaces[cf];
                        cofaces_f[indice] = coface;
                        for(auto v : coface->getVertices()){
                            name_vertices.push_back(v->getName());
                        }
                        indice++;
                    }


                    std::vector<std::string> cleaned_names;

                    for(const auto& name : name_vertices){
                        if(std::count(name_vertices.begin(),name_vertices.end(), name) == 1){
                            cleaned_names.push_back(name);
                        }
                    }


                    std::vector<Vertex*> v_in_face;
                    v_in_face.resize(4);
                    indice = 0;
                    for(const auto& vname : cleaned_names) {
                        std::cout<<vname<<std::endl;
                        auto pos = std::find(vnames.begin(), vnames.end(), vname);
                        int iv1 = pos - vnames.begin();
                        v_in_face[indice] = vertices[iv1];
                        indice++;
                    }

                    Face* face = new Face(m_c,cofaces_f,v_in_face, true);
                    faces.push_back(face);
                    m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);

                }else{
                    Face* face = new Face(m_c,cofaces[cofaces_in_bloc[0]]);
                    faces.push_back(face);
                    m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);
                }
            }

            std::vector<Vertex*> vs;
            vs.resize(b_nodes[i].size());

            for(int j = 0; j < b_nodes[i].size(); j++){
                vs[j] = vertices[b_nodes[i][j]];
            }

            Block* block = new Block(m_c, faces, vs, true);
            m_icmd->addTopoInfoEntity(block, Internal::InfoCommand::CREATED);
            group3->add(block);
            block->getGroups().push_back(group3);


            //block->check();
        }


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

/*----------------------------------------------------------------------------*/
}
}
