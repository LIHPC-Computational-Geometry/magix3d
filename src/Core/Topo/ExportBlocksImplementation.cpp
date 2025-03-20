//
// Created by calderans on 3/17/25.
//

#include "Topo/ExportBlocksImplementation.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
ExportBlocksCGNSImplementation::
ExportBlocksCGNSImplementation(Internal::Context& c, const std::string& n)
: m_context(c), m_filename(n)
{}
/*----------------------------------------------------------------------------*/
ExportBlocksCGNSImplementation::
~ExportBlocksCGNSImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportBlocksCGNSImplementation::perform(Internal::InfoCommand* icmd)
{

    init();
    Mgx3D::Topo::TopoManagerIfc& tm = m_context.getTopoManager();

    std::vector<Topo::Vertex*> vertices;
    tm.getVertices(vertices);
    *m_stream  << "POINTS " << vertices.size() << "\n";

    auto blk_node_id = 0;
    for(auto v : vertices){
        Utils::Math::Point p = v->getCoord();
        *m_stream << p.getX() <<  " " << p.getY() << " " << p.getZ() << "\n";
        m_node_ids_mapping[v->getName()] = blk_node_id++;
    }
    *m_stream  <<"\n\n";

    std::vector<CoEdge*> edges;
    tm.getCoEdges(edges);
    *m_stream  << "EDGES " << edges.size() << "\n";

    auto blk_edge_id = 0;
    for(auto e : edges){
        int n0 = m_node_ids_mapping[e->getVertex(0)->getName()];
        int n1 = m_node_ids_mapping[e->getVertex(1)->getName()];
        *m_stream << n0 <<  " " << n1 << "\n";
        m_edge_ids_mapping[e->getName()] = blk_edge_id++;
    }
    *m_stream  << "Discr " << edges.size() << "\n";
    for(auto e : edges){

        CoEdgeMeshingProperty* prop = e->getMeshingProperty();
        switch (prop->getMeshLaw()) {
            case CoEdgeMeshingProperty::uniforme:
                *m_stream << 0 <<  " " << prop->getNbEdges() << "\n";
                break;
            case CoEdgeMeshingProperty::geometrique:
                *m_stream << 1 <<  " " << prop->getNbEdges() << " ";
                *m_stream << dynamic_cast<EdgeMeshingPropertyGeometric*>(prop)->getRatio() << "\n";
                break;
            case CoEdgeMeshingProperty::bigeometrique:
                *m_stream << 2 <<  " " << prop->getNbEdges() << "\n";
                break;
            case CoEdgeMeshingProperty::hyperbolique:
                *m_stream << 3 <<  " " << prop->getNbEdges() << "\n";
                break;
            case CoEdgeMeshingProperty::specific_size:
                *m_stream << 4 <<  " " << prop->getNbEdges() << "\n";
                break;
            case CoEdgeMeshingProperty::interpolate:
                *m_stream << 5 <<  " " << prop->getNbEdges() << "\n";
                break;
            case CoEdgeMeshingProperty::globalinterpolate:

                *m_stream << 6 <<  " " << prop->getNbEdges() << " [ ";
                for(auto e_name : dynamic_cast<EdgeMeshingPropertyGlobalInterpolate*>(prop)->getFirstCoEdges()){
                    *m_stream << m_edge_ids_mapping[e_name] << " ";
                }
                *m_stream << "] ";
                for(auto e_name : dynamic_cast<EdgeMeshingPropertyGlobalInterpolate*>(prop)->getSecondCoEdges()){
                    *m_stream << m_edge_ids_mapping[e_name] << " ";
                }
                *m_stream << "] " << "\n";
                break;
            case CoEdgeMeshingProperty::tabulated:
                *m_stream << 7 <<  " " << prop->getNbEdges() << "\n";
                break;
            case CoEdgeMeshingProperty::beta_resserrement:
                *m_stream << 8 <<  " " << prop->getNbEdges() << "\n";
                break;
            default:
                std::string s ="Arete avec une méthode de discrétisation inconnue";
                throw TkUtil::Exception(TkUtil::UTF8String (s));
        }
    }
    *m_stream  <<"\n\n";

    std::vector<CoFace*> faces;
    tm.getCoFaces(faces);
    *m_stream  << "FACES " << faces.size() << "\n";
    auto blk_face_id = 0;
    for(auto f : faces){

        m_face_ids_mapping[f->getName()] = blk_face_id++;
        Edge* e0 = f->getEdge(0);
        Edge* e1 = f->getEdge(1);
        Edge* e2 = f->getEdge(2);
        Edge* e3 = f->getEdge(3);

        *m_stream << "[ ";

        if(f->getVertex(0)->getName() == e0->getVertex(0)->getName()) {
            for (int i = 0; i < e0->getCoEdges().size(); i++)
                *m_stream << m_edge_ids_mapping[e0->getCoEdge(i)->getName()]<<" ";
        }else if(f->getVertex(0)->getName() == e0->getVertex(1)->getName()){
            for (int i = e0->getCoEdges().size()-1; i >= 0 ; i--)
                *m_stream << m_edge_ids_mapping[e0->getCoEdge(i)->getName()]<<" ";
        }
        *m_stream << "] [ ";

        if(f->getVertex(1)->getName() == e1->getVertex(0)->getName()) {
            for (int i = 0; i < e1->getCoEdges().size(); i++)
                *m_stream << m_edge_ids_mapping[e1->getCoEdge(i)->getName()]<<" ";
        }else if(f->getVertex(1)->getName() == e1->getVertex(1)->getName()){
            for (int i = e1->getCoEdges().size()-1; i >= 0 ; i--)
                *m_stream << m_edge_ids_mapping[e1->getCoEdge(i)->getName()]<<" ";
        }
        *m_stream << "] [ ";

        if(f->getVertex(2)->getName() == e2->getVertex(0)->getName()) {
            for (int i = 0; i < e2->getCoEdges().size(); i++)
                *m_stream << m_edge_ids_mapping[e2->getCoEdge(i)->getName()]<<" ";
        }else if(f->getVertex(2)->getName() == e2->getVertex(1)->getName()){
            for (int i = e2->getCoEdges().size()-1; i >= 0 ; i--)
                *m_stream << m_edge_ids_mapping[e2->getCoEdge(i)->getName()]<<" ";
        }
        *m_stream << "] [ ";

        if(f->getVertex(3)->getName() == e3->getVertex(0)->getName()) {
            for (int i = 0; i < e3->getCoEdges().size(); i++)
                *m_stream << m_edge_ids_mapping[e3->getCoEdge(i)->getName()]<<" ";
        }else if(f->getVertex(3)->getName() == e3->getVertex(1)->getName()){
            for (int i = e3->getCoEdges().size()-1; i >= 0 ; i--)
                *m_stream << m_edge_ids_mapping[e3->getCoEdge(i)->getName()]<<" ";
        }
        *m_stream << "]\n";
    }
    *m_stream << "\n";
    std::vector<Block*> blocks;
    tm.getBlocks(blocks);
    *m_stream  << "BLOCKS " << blocks.size() << "\n";
    for(auto b : blocks){
        int v0 = m_node_ids_mapping[b->getVertex(0)->getName()];
        int v1 = m_node_ids_mapping[b->getVertex(1)->getName()];
        int v2 = m_node_ids_mapping[b->getVertex(2)->getName()];
        int v3 = m_node_ids_mapping[b->getVertex(3)->getName()];
        int v4 = m_node_ids_mapping[b->getVertex(4)->getName()];
        int v5 = m_node_ids_mapping[b->getVertex(5)->getName()];
        int v6 = m_node_ids_mapping[b->getVertex(6)->getName()];
        int v7 = m_node_ids_mapping[b->getVertex(7)->getName()];

        Face* f0 = b->getFace(0);
        Face* f1 = b->getFace(1);
        Face* f2 = b->getFace(2);
        Face* f3 = b->getFace(3);
        Face* f4 = b->getFace(4);
        Face* f5 = b->getFace(5);

        *m_stream << v0 << " " << v1 << " " << v2 << " " << v3 << " " << v4 << " ";
        *m_stream << v5 << " " << v6 << " " << v7 << " [ ";
        for (int i = 0; i < f0->getCoFaces().size(); i++)
            *m_stream << m_face_ids_mapping[f0->getCoFace(i)->getName()]<<" ";
        *m_stream << "] [ ";
        for (int i = 0; i < f1->getCoFaces().size(); i++)
            *m_stream << m_face_ids_mapping[f1->getCoFace(i)->getName()]<<" ";
        *m_stream << "] [ ";
        for (int i = 0; i < f2->getCoFaces().size(); i++)
            *m_stream << m_face_ids_mapping[f2->getCoFace(i)->getName()]<<" ";
        *m_stream << "] [ ";
        for (int i = 0; i < f3->getCoFaces().size(); i++)
            *m_stream << m_face_ids_mapping[f3->getCoFace(i)->getName()]<<" ";
        *m_stream << "] [ ";
        for (int i = 0; i < f4->getCoFaces().size(); i++)
            *m_stream << m_face_ids_mapping[f4->getCoFace(i)->getName()]<<" ";
        *m_stream << "] [ ";
        for (int i = 0; i < f5->getCoFaces().size(); i++)
            *m_stream << m_face_ids_mapping[f5->getCoFace(i)->getName()]<<" ";
        *m_stream << "]\n";
    }

    m_stream->close();
}
/*----------------------------------------------------------------------------*/
void ExportBlocksCGNSImplementation::init()
{
    m_stream= new std::ofstream(m_filename, std::ios::out);
    if (!m_stream){
        std::string s ="Impossible to create a Blocking File: "+m_filename;
        throw TkUtil::Exception (TkUtil::UTF8String (s));
    }

    *m_stream << "# Mgx DataFile Version 0.1\n";
    *m_stream << "Generated by Mgx Writer\n\n";
    *m_stream << "ASCII\n";
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/