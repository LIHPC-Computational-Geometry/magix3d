/*----------------------------------------------------------------------------*/
/** \file CommandNewTopo.cpp
 *
 *  \author Simon Calderan
 *
 *  \date 03/01/2025
 */
/*----------------------------------------------------------------------------*/
#include "Geom/EntityFactory.h"
#include "Topo/CommandNewTopo.h"
#include "Topo/TopoManager.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Utils/Common.h"
#include "Internal/ServiceGeomToTopo.h"
#include "Internal/Context.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Topo {
/*----------------------------------------------------------------------------*/
        CommandNewTopo::
        CommandNewTopo(Internal::Context& c, std::vector<Topo::Vertex*> sommets, eTopoType topoType, int dim,
                       std::string groupName)
                : CommandEditTopo(c, "Nom de commande à définir")
                , m_groupName(groupName)
                , m_ni(0)
                , m_nj(0)
                , m_nk(0)
                ,m_vertices(sommets)
                ,m_dim(dim)
        {
            TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

            switch (topoType){
                case UNSTRUCTURED_BLOCK:
                    //setStructured(false);
                    if (m_dim == 3)
                        comments << "Création d'un bloc topologique non structuré";
                    else if (m_dim == 2)
                        comments << "Création d'une face topologique non structurée";
                    break;
                case STRUCTURED_BLOCK:
                    //setStructured(true);
                    if (m_dim == 3)
                        comments << "Création d'un bloc topologique structuré";
                    else if (m_dim == 2)
                        comments << "Création d'une face topologique structurée";
                    else if (m_dim == 1)
                        comments << "Création d'une arête topologie structurée";
                    break;
                default:
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo pour un type de topologie non prévu", TkUtil::Charset::UTF_8));
            }

            setScriptComments(comments);
            setName(comments);
        }

        /*----------------------------------------------------------------------------*/
        CommandNewTopo::
        CommandNewTopo(Internal::Context& c, const Utils::Math::Point point,
                       std::string groupName)
                : CommandEditTopo(c, "Nom de commande à définir")
                , m_dim(0)
                , m_point(point)
                , m_groupName(groupName)
                , m_ni(0)
                , m_nj(0)
                , m_nk(0)
        {
            TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
            comments << "Création d'un sommet géométrique par coordonnées";


            setScriptComments(comments);
            setName(comments);
        }
/*----------------------------------------------------------------------------*/
CommandNewTopo::
        ~CommandNewTopo()
        {}
/*----------------------------------------------------------------------------*/
        void CommandNewTopo::
        internalExecute()
        {

            if(m_dim == 1 && m_vertices.size() != 2)
                throw TkUtil::Exception(TkUtil::UTF8String("CommandNewTopo nombre de sommets invalide pour créer une arete " + std::to_string(m_vertices.size()) + " au lieu de 2", TkUtil::Charset::UTF_8));
            if(m_dim == 2 && m_vertices.size() != 4)
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo nombre de sommets invalide pour créer une face " + std::to_string(m_vertices.size()) + " au lieu de 4", TkUtil::Charset::UTF_8));
            if(m_dim == 3 && m_vertices.size() != 8)
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo nombre de sommets invalide pour créer un bloc " + std::to_string(m_vertices.size()) + " au lieu de 8", TkUtil::Charset::UTF_8));


            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "CommandNewTopo::execute pour la commande " << getName ( )
                    << " de nom unique " << getUniqueName ( );

            for(auto v : m_vertices){
                v->saveVertexTopoProperty(&getInfoCommand());
            }

            if (m_dim == 0){
                createVertex();
            }
            else if (m_dim == 1){

                if(getCommonEdge(m_vertices[0], m_vertices[1]) != nullptr){
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo une arete existe déjà entre " + m_vertices[0]->getName() + " et " + m_vertices[1]->getName(), TkUtil::Charset::UTF_8));
                }
                else{
                    createCoEdge(m_vertices[0], m_vertices[1], m_groupName);
                }
            }
            else if (m_dim == 3){

                if(getCommonBlock(m_vertices[0], m_vertices[1], m_vertices[2], m_vertices[3],
                                  m_vertices[4], m_vertices[5], m_vertices[6], m_vertices[7]) != nullptr){
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo une face existe déjà entre "
                                                                 + m_vertices[0]->getName() + ", " + m_vertices[1]->getName()
                                                                 + ", " + m_vertices[2]->getName()+ " et " + m_vertices[3]->getName(), TkUtil::Charset::UTF_8));
                }else{
                    createBlock(m_vertices[0], m_vertices[1], m_vertices[2], m_vertices[3],
                                m_vertices[4], m_vertices[5], m_vertices[6], m_vertices[7],m_groupName);
                }

            }
            else if (m_dim == 2){

                if(getCommonFace(m_vertices[0], m_vertices[1], m_vertices[2], m_vertices[3]) != nullptr){
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo une face existe déjà entre "
                    + m_vertices[0]->getName() + ", " + m_vertices[1]->getName()
                    + ", " + m_vertices[2]->getName()+ " et " + m_vertices[3]->getName(), TkUtil::Charset::UTF_8));
                }else{
                    createFace(m_vertices[0], m_vertices[1], m_vertices[2], m_vertices[3], m_groupName);
                }
            }
            else {
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry imposible avec une géométrie autre qu'un Volume ou une Surface", TkUtil::Charset::UTF_8));
            }

            // suppression des parties internes de type save... qui n'ont pas lieu d'être
            // puisque l'on est en cours de création
            saveInternalsStats();
            //deleteInternalsStats();

            // suppression des entités temporaires
            cleanTemporaryEntities();

            // enregistrement des nouvelles entités dans le TopoManager
            registerToManagerCreatedEntities();

            log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
        }
/*----------------------------------------------------------------------------*/
        void CommandNewTopo::createVertex()
        {
            Vertex* vtx = new Vertex(getContext(), m_point);

            Group::Group0D *group = getContext().getGroupManager().getNewGroup0D(m_groupName,
                                                                                      &getInfoCommand());
            group->add(vtx);
            vtx->getGroupsContainer().add(group);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
        }
/*----------------------------------------------------------------------------*/
        CoEdge* CommandNewTopo::createCoEdge(Vertex* v0,Vertex* v1, std::string groupName)
        {

            EdgeMeshingPropertyUniform emp(getContext().getTopoManager().getDefaultNbMeshingEdges());
            CoEdge* coedge = new CoEdge(getContext(), &emp, v0, v1);


            Group::Group1D *group = getContext().getGroupManager().getNewGroup1D(groupName,
                                                                                      &getInfoCommand());
            group->add(coedge);
            coedge->getGroupsContainer().add(group);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);


            return coedge;
        }
/*----------------------------------------------------------------------------*/
        Topo::CoFace* CommandNewTopo::createFace(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3, std::string groupName)
        {

            gmds::math::Triangle t012(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                    gmds::math::Point(v1->getX(),v1->getY(),v1->getZ()),
                                    gmds::math::Point(v2->getX(),v2->getY(),v2->getZ()));
            gmds::math::Triangle t023(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                    gmds::math::Point(v2->getX(),v2->getY(),v2->getZ()),
                                    gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()));

            double dot = t012.getNormal().dot(t023.getNormal());

            gmds::math::VectorND<3, gmds::TCoord> vec012 = t012.getNormal();
            gmds::math::VectorND<3, gmds::TCoord> vec023 = t023.getNormal();

            int orientation = 0;

            if(dot < 0){
                gmds::math::Triangle t013(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                          gmds::math::Point(v1->getX(),v1->getY(),v1->getZ()),
                                          gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()));
                gmds::math::Triangle t032(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                          gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()),
                                          gmds::math::Point(v2->getX(),v2->getY(),v2->getZ()));

                dot = t013.getNormal().dot(t032.getNormal());

                gmds::math::VectorND<3, gmds::TCoord> vec013 = t013.getNormal();
                gmds::math::VectorND<3, gmds::TCoord> vec032 = t032.getNormal();

                if(dot < 0 ){
                    gmds::math::Triangle t031(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                              gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()),
                                              gmds::math::Point(v1->getX(),v1->getY(),v1->getZ()));

                    dot = t031.getNormal().dot(t012.getNormal());

                    gmds::math::VectorND<3, gmds::TCoord> vec031 = t031.getNormal();

                    if(dot < 0){
                        throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo impossible d'ordonner les sommets pour créer une face", TkUtil::Charset::UTF_8));
                    }
                    else{
                        orientation = 2; //cas les sommets 1 et 3 sont inversés
                    }
                } else{
                    orientation = 1; //cas les sommets 1 et 2 sont inversés
                }
            }

            CoEdge* e0;
            CoEdge* e1;
            CoEdge* e2;
            CoEdge* e3;

            switch (orientation) {
                case 0:
                    e0 = getCommonEdge(v0,v1);
                    if(e0 == nullptr)
                        e0 = createCoEdge(v0,v1,"");
                    else
                        e0->saveCoEdgeTopoProperty(&getInfoCommand());

                    e1 = getCommonEdge(v1,v2);
                    if(e1 == nullptr)
                        e1 = createCoEdge(v1,v2,"");
                    else
                        e1->saveCoEdgeTopoProperty(&getInfoCommand());

                    e2 = getCommonEdge(v2,v3);
                    if(e2 == nullptr)
                        e2 = createCoEdge(v2,v3,"");
                    else
                        e2->saveCoEdgeTopoProperty(&getInfoCommand());

                    e3 = getCommonEdge(v3,v0);
                    if(e3 == nullptr)
                        e3 = createCoEdge(v3,v0,"");
                    else
                        e3->saveCoEdgeTopoProperty(&getInfoCommand());

                    break;
                case 1:
                    e0 = getCommonEdge(v0,v1);
                    if(e0 == nullptr)
                        e0 = createCoEdge(v0,v1,"");
                    else
                        e0->saveCoEdgeTopoProperty(&getInfoCommand());

                    e1 = getCommonEdge(v1,v3);
                    if(e1 == nullptr)
                        e1 = createCoEdge(v1,v3,"");
                    else
                        e1->saveCoEdgeTopoProperty(&getInfoCommand());

                    e2 = getCommonEdge(v3,v2);
                    if(e2 == nullptr)
                        e2 = createCoEdge(v3,v2,"");
                    else
                        e2->saveCoEdgeTopoProperty(&getInfoCommand());

                    e3 = getCommonEdge(v2,v0);
                    if(e3 == nullptr)
                        e3 = createCoEdge(v2,v0,"");
                    else
                        e3->saveCoEdgeTopoProperty(&getInfoCommand());

                    break;
                case 2:
                    e0 = getCommonEdge(v0,v2);
                    if(e0 == nullptr)
                        e0 = createCoEdge(v0,v2,"");
                    else
                        e0->saveCoEdgeTopoProperty(&getInfoCommand());

                    e1 = getCommonEdge(v2,v1);
                    if(e1 == nullptr)
                        e1 = createCoEdge(v2,v1,"");
                    else
                        e1->saveCoEdgeTopoProperty(&getInfoCommand());

                    e2 = getCommonEdge(v1,v3);
                    if(e2 == nullptr)
                        e2 = createCoEdge(v1,v3,"");
                    else
                        e2->saveCoEdgeTopoProperty(&getInfoCommand());

                    e3 = getCommonEdge(v3,v0);
                    if(e3 == nullptr)
                        e3 = createCoEdge(v3,v0,"");
                    else
                        e3->saveCoEdgeTopoProperty(&getInfoCommand());

                    break;
            }

            Topo::Edge* edge0 = new Edge(getContext(), e0);
            getInfoCommand().addTopoInfoEntity(edge0,Internal::InfoCommand::CREATED);
            Topo::Edge* edge1 = new Edge(getContext(), e1);
            getInfoCommand().addTopoInfoEntity(edge1,Internal::InfoCommand::CREATED);
            Topo::Edge* edge2 = new Edge(getContext(), e2);
            getInfoCommand().addTopoInfoEntity(edge2,Internal::InfoCommand::CREATED);
            Topo::Edge* edge3 = new Edge(getContext(), e3);
            getInfoCommand().addTopoInfoEntity(edge3,Internal::InfoCommand::CREATED);

            std::vector<Edge*> edges = {edge0,edge1,edge2,edge3};

            Topo::CoFace* face = new CoFace(getContext(), edges, true);

            Group::Group2D *group = getContext().getGroupManager().getNewGroup2D(groupName,
                                                                                      &getInfoCommand());
            group->add(face);
            face->getGroupsContainer().add(group);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(face, Internal::InfoCommand::CREATED);

            return face;
        }
/*----------------------------------------------------------------------------*/
void CommandNewTopo::createBlock(Mgx3D::Topo::Vertex *v0, Mgx3D::Topo::Vertex *v1, Mgx3D::Topo::Vertex *v2,
                                 Mgx3D::Topo::Vertex *v3, Mgx3D::Topo::Vertex *v4, Mgx3D::Topo::Vertex *v5,
                                 Mgx3D::Topo::Vertex *v6, Mgx3D::Topo::Vertex *v7, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo la construction des blocs n'est pas prise en compte", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
    return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
Topo::CoEdge* CommandNewTopo::getCommonEdge(const Vertex* v1, const Vertex* v2) {

    std::vector<CoEdge*> test1 = v1->getCoEdges();
            std::vector<CoEdge*> test2 = v2->getCoEdges();

    for(auto v1_e : v1->getCoEdges()){
        for(auto v2_e : v2->getCoEdges()){
            if(v1_e->getName() == v2_e->getName())
                return v1_e;
        }
    }

    return nullptr;
}
/*----------------------------------------------------------------------------*/
Topo::CoFace* CommandNewTopo::getCommonFace(const Vertex* v1, const Vertex* v2, const Vertex* v3, const Vertex* v4) {

    std::vector<CoFace*> v1_cf;
    std::vector<CoFace*> v2_cf;
    std::vector<CoFace*> v3_cf;
    std::vector<CoFace*> v4_cf;

    v1->getCoFaces(v1_cf);
    v2->getCoFaces(v2_cf);
    v3->getCoFaces(v3_cf);
    v4->getCoFaces(v4_cf);

    for(auto v1_f : v1_cf){
        for(auto v2_f : v2_cf){
            for(auto v3_f : v3_cf) {
                for (auto v4_f: v4_cf) {
                    if (v1_f->getName() == v2_f->getName() &&
                        v1_f->getName() == v3_f->getName() &&
                        v1_f->getName() == v4_f->getName())
                        return v1_f;
                }
            }
        }
    }

    return nullptr;
}

Topo::Block* CommandNewTopo::getCommonBlock(const Mgx3D::Topo::Vertex *v0, const Mgx3D::Topo::Vertex *v1,
                                            const Mgx3D::Topo::Vertex *v2, const Mgx3D::Topo::Vertex *v3,
                                            const Mgx3D::Topo::Vertex *v4, const Mgx3D::Topo::Vertex *v5,
                                            const Mgx3D::Topo::Vertex *v6, const Mgx3D::Topo::Vertex *v7) {


}
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

