//
// Created by calderans on 4/3/25.
//

/*----------------------------------------------------------------------------*/
#include "Topo/ImportBlocksImplementation.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Topo/EdgeMeshingPropertyBigeometric.h"
#include "Topo/EdgeMeshingPropertyBeta.h"
#include "Topo/EdgeMeshingPropertyHyperbolic.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/EdgeMeshingPropertySpecificSize.h"
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
#include "Internal/InfoCommand.h"

/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/

namespace Mgx3D {
namespace Topo {

/*----------------------------------------------------------------------------*/

ImportBlocksImplementation::ImportBlocksImplementation(Internal::Context &c, Internal::InfoCommand *icmd,
                            const std::string &n, bool withGeom) :
    m_group_helper(*icmd, c.getGroupManager()),
    m_c(c),
    m_icmd(icmd),
    m_filename(n),
    m_geom(withGeom)
    {}

/*----------------------------------------------------------------------------*/
//ImportBlocksImplementation::~ImportBlocksImplementation() {}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::internalExecute() {

    // check file extension
    std::string suffix = m_filename;
    int suffix_start = m_filename.find_last_of('.');
    suffix.erase(0, suffix_start + 1);
    if (suffix != "mgx")
        throw TkUtil::Exception(
                TkUtil::UTF8String("Mauvaise extension de fichier (.mgxt)", TkUtil::Charset::UTF_8));

    std::ifstream s(m_filename.c_str(), std::ios::in);
    if (!s) {
        std::string mess = "Impossible to read file " + m_filename;
        throw TkUtil::Exception(mess);
    }
        Group::Group0D* group0 = getStdContext()->getGroupManager().getNewGroup<Group::Group0D>("Hors_Groupe_0D", m_icmd);
        Group::Group1D* group1 = getStdContext()->getGroupManager().getNewGroup<Group::Group1D>("Hors_Groupe_1D", m_icmd);
        Group::Group2D* group2 = getStdContext()->getGroupManager().getNewGroup<Group::Group2D>("Hors_Groupe_2D", m_icmd);
        Group::Group3D* group3 = getStdContext()->getGroupManager().getNewGroup<Group::Group3D>("Hors_Groupe_3D", m_icmd);

        int nb_edges;

        readNodes(s, group0);
        nb_edges = readEdges(s, group1);
        readFaces(s, group2);
        readDiscr(s, nb_edges);
        readBlocks(s, group3);

    if(m_geom)
        readAssociation(s);

    s.close();
}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::readNodes(std::ifstream &str, Group::Group0D* group){
    if (!moveStreamOntoFirst(str, "POINTS")) {
        std::string mess = "BLK read error: no POINTS keyword found";
        throw TkUtil::Exception(mess);
    }
    int nb_nodes;
    std::string coord_type;
    str >> nb_nodes;

    std::vector<double> x, y, z;
    x.resize(nb_nodes);
    y.resize(nb_nodes);
    z.resize(nb_nodes);

    for (int i = 0; i < nb_nodes; i++) {
        str >> x[i] >> y[i] >> z[i];
    }


    m_vertices.resize(nb_nodes);
    m_vnames.resize(nb_nodes);
    for (int i = 0; i < nb_nodes; i++) {
        Vertex *vtx = new Vertex(m_c, Utils::Math::Point(x[i], y[i], z[i]));
        m_vertices[i] = vtx;
        m_vnames[i] = vtx->getName();
        m_icmd->addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
        m_group_helper.addToGroup("Hors_Groupe_0D", vtx);
    }
}
/*----------------------------------------------------------------------------*/
int ImportBlocksImplementation::readEdges(std::ifstream &str, Group::Group1D* group){
            if (!moveStreamOntoFirst(str, "EDGES")) {
                std::string mess = "BLK read error: no EDGES keyword found";
                throw TkUtil::Exception(mess);
            }
            int nb_edges;
            str >> nb_edges;

            std::vector<int> n0, n1;
            n0.resize(nb_edges);
            n1.resize(nb_edges);

            for (int i = 0; i < nb_edges; i++) {
                str >> n0[i] >> n1[i];
            }

            m_coedges.resize(nb_edges);
            m_enames.resize(nb_edges);
            for (int i = 0; i < nb_edges; i++) {
                Vertex *v0 = m_vertices[n0[i]];
                Vertex *v1 = m_vertices[n1[i]];
                EdgeMeshingPropertyUniform* emp = new EdgeMeshingPropertyUniform(10);
                CoEdge *edge = new CoEdge(m_c, emp, v0, v1);
                m_coedges[i] = edge;
                m_enames[i] = edge->getName();
                m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
                m_group_helper.addToGroup("Hors_Groupe_1D", edge);
            }
            return nb_edges;
        }
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::readFaces(std::ifstream &str, Group::Group2D* group){
            if (!moveStreamOntoFirst(str, "FACES")) {
                std::string mess = "BLK read error: no FACES keyword found";
                throw TkUtil::Exception(mess);
            }
            int nb_faces;
            str >> nb_faces;

    //std::vector<std::vector<int>> faces_edges;
    //faces_edges.resize(nb_faces);
    std::vector<std::vector<std::vector<int> > > faces_edges(nb_faces,
                                                             std::vector<std::vector<int> >(4,
                                                                                            std::vector<int>()));
    for (int i = 0; i < nb_faces; i++) {

                std::string current;
                for (int e_i = 0; e_i < 4; e_i++) {
                    std::vector<int> fi_edges;
                    str >> current;
                    while (current != "]") {
                        if (current == "[") {
                            str >> current;
                        } else {
                            int e_id;
                            e_id = std::stoi(current);
                            fi_edges.push_back(e_id);
                            str >> current;

                        }
                    }
                    faces_edges[i][e_i] = fi_edges;
                }
            }

            m_cofaces.resize(nb_faces);
            m_fnames.resize(nb_faces);
            for (int i = 0; i < nb_faces; ++i) {

                std::vector<Edge *> edges;
                edges.resize(4);

                for (int j = 0; j < 4; ++j) {
                    //Ici on créer les aretes de la face

                    std::vector<int> coedges_in_face = faces_edges[i][j];
                    std::vector<CoEdge *> coedges_f;
                    coedges_f.resize(coedges_in_face.size());
                    coedges_f[0] = m_coedges[coedges_in_face[0]];
                    bool v1changed = false;
                    std::string v1Edge = m_coedges[coedges_in_face[0]]->getVertices()[0]->getName();
                    std::string v2Edge = m_coedges[coedges_in_face[0]]->getVertices()[1]->getName();

                    for (int i_e = 1; i_e < coedges_in_face.size(); i_e++) {
                        coedges_f[i_e] = m_coedges[coedges_in_face[i_e]];
                        std::string v1current = m_coedges[coedges_in_face[i_e]]->getVertices()[0]->getName();
                        std::string v2current = m_coedges[coedges_in_face[i_e]]->getVertices()[1]->getName();

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
                    auto pos = std::find(m_vnames.begin(), m_vnames.end(), v1Edge);
                    iv1 = pos-m_vnames.begin();
                    pos = std::find(m_vnames.begin(), m_vnames.end(), v2Edge);
                    iv2 = pos-m_vnames.begin();
                    if (v1changed) {
                        Vertex *v1 = m_vertices[iv2];
                        Vertex *v2 = m_vertices[iv1];

                        Edge *edge = new Edge(m_c, v1, v2, coedges_f);
                        m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
                        edges[j] = edge;
                    }else{
                        Vertex *v1 = m_vertices[iv1];
                        Vertex *v2 = m_vertices[iv2];

                        Edge *edge = new Edge(m_c, v1, v2, coedges_f);
                        m_icmd->addTopoInfoEntity(edge, Internal::InfoCommand::CREATED);
                        edges[j] = edge;
                    }
                }

                CoFace *face = new CoFace(m_c, edges, true);
                m_cofaces[i] = face;
                m_fnames[i] = face->getName();
                m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);
                m_group_helper.addToGroup("Hors_Groupe_2D", face);
            }
}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::readBlocks(std::ifstream &str, Group::Group3D* group){
            if (!moveStreamOntoFirst(str, "BLOCKS")) {
                std::string mess = "BLK read error: no BLOCKS keyword found";
                throw TkUtil::Exception(mess);
            }
            int nb_blocks;
            str >> nb_blocks;

            std::vector<std::vector<int>> b_nodes(nb_blocks,
                                                  std::vector<int>(8));
            std::vector<std::vector<std::vector<int> > > blocks_faces(nb_blocks,
                                                                      std::vector<std::vector<int> >(6,
                                                                                                     std::vector<int>()));

            m_bnames.resize(nb_blocks);
            m_blocks.resize(nb_blocks);
            for (int i = 0; i < nb_blocks; i++) {

                for (int j = 0; j < 8; j++) {
                    str >> b_nodes[i][j];
                }

                std::string current;
                for (int f_i = 0; f_i < 6; f_i++) {
                    std::vector<int> bi_faces;
                    str >> current;
                    while (current != "]") {
                        if (current == "[") {
                            str >> current;
                        } else {
                            int f_id;
                            f_id = std::stoi(current);
                            bi_faces.push_back(f_id);
                            str >> current;
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

                            CoFace* coface = m_cofaces[cf];
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
                            auto pos = std::find(m_vnames.begin(), m_vnames.end(), vname);
                            int iv1 = pos - m_vnames.begin();
                            v_in_face[indice] = m_vertices[iv1];
                            indice++;
                        }

                        Face* face = new Face(m_c,cofaces_f,v_in_face, true);
                        faces.push_back(face);
                        m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);

                    }else{
                        Face* face = new Face(m_c,m_cofaces[cofaces_in_bloc[0]]);
                        faces.push_back(face);
                        m_icmd->addTopoInfoEntity(face, Internal::InfoCommand::CREATED);
                    }
                }

                std::vector<Vertex*> vs;
                vs.resize(b_nodes[i].size());

                for(int j = 0; j < b_nodes[i].size(); j++){
                    vs[j] = m_vertices[b_nodes[i][j]];
                }

                Block* block = new Block(m_c, faces, vs, true);
                m_icmd->addTopoInfoEntity(block, Internal::InfoCommand::CREATED);
                m_group_helper.addToGroup("Hors_Groupe_3D", block);
                m_bnames[i] = block->getName();
                m_blocks[i] = block;
            }
}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::readDiscr(std::ifstream &str,const int &nbEdges){
            if (!moveStreamOntoFirst(str, "Discr")) {
                std::string mess = "BLK read error: no Discr keyword found";
                throw TkUtil::Exception(mess);
            }
            int nb_discr;
            str >> nb_discr;
            if (nbEdges != nb_discr) {
                std::string mess = "BLK read error: number of discretization info different from number of edges";
                throw TkUtil::Exception(mess);
            }
            std::vector<CoEdgeMeshingProperty*> emps;
            emps.resize(nb_discr);
            int disc_type;
            int nb;
            for (int i = 0; i < nb_discr; i++) {
                str >> disc_type;
                if (disc_type == 0) {
                    str >> nb;
                    emps[i] = new EdgeMeshingPropertyUniform(nb);
                }
                else if (disc_type == 1) {
                    double raison;
                    str >> nb >> raison;
                    emps[i] = new EdgeMeshingPropertyGeometric(nb, raison);
                }
                else if (disc_type == 2) {
                    double r1, sp1, r2, sp2;
                    str >> nb >> r1 >> sp1 >> r2 >> sp2;
                    emps[i] = new EdgeMeshingPropertyBigeometric(nb, r1, sp1, r2, sp2);
                }
                else if (disc_type == 3) {
                    double sp1, sp2;
                    str >> nb >> sp1 >> sp2;
                    emps[i] = new EdgeMeshingPropertyHyperbolic(nb, sp1, sp2);
                }
                else if (disc_type == 4) {
                    double size;
                    str >> nb >> size;
                    emps[i] = new EdgeMeshingPropertySpecificSize(size);
                }
                else if (disc_type == 5) {
                    str >> nb;
                    std::string current;
                    std::vector<int> edges;
                    int type;
                    str >> type;
                    if(type == 0){
                        str >> current;
                        while (current != "]") {
                            if (current == "[") {
                                str >> current;
                            } else {
                                int e_id;
                                e_id = std::stoi(current);
                                edges.push_back(e_id);
                                str >> current;

                            }
                        }
                        std::vector<std::string> coedgesNames;
                        coedgesNames.resize(edges.size());
                        int indice = 0;
                        for (auto e_id : edges) {
                            coedgesNames[indice] = m_enames[e_id];
                            indice++;
                        }

                        emps[i] = new EdgeMeshingPropertyInterpolate(nb, coedgesNames);
                    }
                    else{
                        std::string current;
                        str >> current;
                        std::string cofaceName;
                        str >> cofaceName;
                        str >> current;

                        emps[i] = new EdgeMeshingPropertyInterpolate(nb, cofaceName);
                    }
                }
                else if (disc_type == 6) {
                    str >> nb;
                    std::vector<std::vector<int>> interpolate_edges(2,std::vector<int>());

                    std::string current;
                    for (int e_i = 0; e_i < 2; e_i++) {
                        std::vector<int> edges;
                        str >> current;
                        while (current != "]") {
                            if (current == "[") {
                                str >> current;
                            } else {
                                int e_id;
                                e_id = std::stoi(current);
                                edges.push_back(e_id);
                                str >> current;

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
                        firstCoedgesNames[indice] = m_enames[e_id];
                        indice++;
                    }
                    indice = 0;
                    for (auto e_id : interpolate_edges[1]) {
                        secondCoedgesNames[indice] = m_enames[e_id];
                        indice++;
                    }
                    emps[i] = new EdgeMeshingPropertyGlobalInterpolate(nb, firstCoedgesNames,secondCoedgesNames);
                }
                    /*else if (disc_type == 7) {
                    double x,y,z;
                    s >> nb >> x >> y >> z;
                    std::vector<double> tabulation = {x,y,z};
                    emps[i] = new EdgeMeshingPropertyTabulated(tabulation);
                    }*/
                else if (disc_type == 8) {
                    double beta;
                    str >> nb >> beta;
                    emps[i] = new EdgeMeshingPropertyBeta(nb, beta);
                }
                else {
                    std::string mess = "BLK read error: type of discretization not supported";
                    throw TkUtil::Exception(mess);
                }
            }

            for (int i=0; i<m_coedges.size(); i++) {
                m_coedges[i]->setProperty(emps[i]);
            }
}
/*----------------------------------------------------------------------------*/
void ImportBlocksImplementation::readAssociation(std::ifstream &str){

    int id, dim, geom_id;
    std::string g_name;
    int nb_assoc;



    if(!moveStreamOntoFirst(str, "GEOM_Nodes")){
        throw TkUtil::Exception(
                TkUtil::UTF8String("Erreur lecteur BLK : Pas d'associations géométriques trouvées pour les sommets.",
                                   TkUtil::Charset::UTF_8));
    }else{
        str >> nb_assoc;
        for (int i = 0; i < nb_assoc; ++i) {
            str >> id; str >> dim; str >> geom_id;

            g_name = findGeom(dim, geom_id);
            Vertex* v = m_vertices[id];
            Geom::GeomEntity* ge = getStdContext()->getGeomManager().getEntity(g_name, false);
            if(ge != nullptr){
                v->setGeomAssociation(ge);
            }else{
                m_warning += v->getName()+" ";
            }
        }
    }


    if(!moveStreamOntoFirst(str, "GEOM_Edges")){
        throw TkUtil::Exception(
                TkUtil::UTF8String("Erreur lecteur BLK : Pas d'associations géométriques trouvées pour les aretes.",
                                   TkUtil::Charset::UTF_8));
    }else {
        str >> nb_assoc;
        for (int i = 0; i < nb_assoc; ++i) {
            str >> id;
            str >> dim;
            str >> geom_id;

            g_name = findGeom(dim, geom_id);
            CoEdge *e = m_coedges[id];
            Geom::GeomEntity *ge = getStdContext()->getGeomManager().getEntity(g_name, false);
            if (ge != nullptr) {
                e->setGeomAssociation(ge);
            } else {
                m_warning += e->getName() + " ";
            }
        }
    }

    if(!moveStreamOntoFirst(str, "GEOM_Faces")){
        throw TkUtil::Exception(
                TkUtil::UTF8String("Erreur lecteur BLK : Pas d'associations géométriques trouvées pour les faces.",
                                   TkUtil::Charset::UTF_8));
    }else {
        str >> nb_assoc;
        for (int i = 0; i < nb_assoc; ++i) {
            str >> id;
            str >> dim;
            str >> geom_id;

            g_name = findGeom(dim, geom_id);
            CoFace *f = m_cofaces[id];
            Geom::GeomEntity *ge = getStdContext()->getGeomManager().getEntity(g_name, false);
            if (ge != nullptr) {
                f->setGeomAssociation(ge);
            } else {
                m_warning += f->getName() + " ";
            }
        }
    }

    if(!moveStreamOntoFirst(str, "GEOM_Blocks")) {
        throw TkUtil::Exception(
                TkUtil::UTF8String("Erreur lecteur BLK : Pas d'associations géométriques trouvées pour les blocs.",
                                   TkUtil::Charset::UTF_8));
    }else {
        str >> nb_assoc;
        for (int i = 0; i < nb_assoc; ++i) {
            str >> id;
            str >> dim;
            str >> geom_id;

            g_name = findGeom(dim, geom_id);
            Block *b = m_blocks[id];
            Geom::GeomEntity *ge = getStdContext()->getGeomManager().getEntity(g_name, false);
            if (ge != nullptr) {
                b->setGeomAssociation(ge);
            } else {
                m_warning += b->getName() + " ";
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
std::string ImportBlocksImplementation::findGeom(int dim, int id){

    std::stringstream geom_name;

    if(dim == 0){
        geom_name << "Pt" << std::setw(4) << std::setfill('0') << id;
    }else if(dim == 1){
        geom_name << "Crb" << std::setw(4) << std::setfill('0') << id;
    }else if(dim == 2){
        geom_name << "Surf" << std::setw(4) << std::setfill('0') << id;
    }else if(dim == 3){
        geom_name << "Vol" << std::setw(4) << std::setfill('0') << id;
    }else {
        std::string mess = "BLK read error: "+std::to_string(dim)+"D geometry not supported for association.";
        throw TkUtil::Exception(mess);
    }

    return geom_name.str();
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
std::string ImportBlocksImplementation::getWarning(){
    return m_warning;
}
/*----------------------------------------------------------------------------*/
}
}
