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
        CommandNewTopo(Internal::Context& c, std::vector<Topo::Vertex*> sommets, eTopoType topoType,
                       std::string groupName)
                : CommandEditTopo(c, "Nom de commande à définir")
                , m_groupName(groupName)
                , m_ni(0)
                , m_nj(0)
                , m_nk(0)
                ,m_vertices(sommets)
        {
            TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

            switch (sommets.size()) {
                case 2:
                    m_dim = 1;
                    break;
                case 4:
                    m_dim = 2;
                    break;
                case 8:
                    m_dim = 3;
                    break;
                default:
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo pour un nombre de sommets non prévu", TkUtil::Charset::UTF_8));
            }

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
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "CommandNewTopo::execute pour la commande " << getName ( )
                    << " de nom unique " << getUniqueName ( );

            if (m_dim == 0){
                createVertex();
            }
            else if (m_dim == 1){

                if(getCommonEdge(m_vertices[0], m_vertices[1]) != nullptr){
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo une arete existe déjà entre " + m_vertices[0]->getName() + " et " + m_vertices[1]->getName(), TkUtil::Charset::UTF_8));
                }
                else{
                    createCoEdge(m_vertices[0], m_vertices[1]);
                }
            }
            else if (m_dim == 3){
                // Utilise un service de création d'une topologie à partir d'un volume
                /*
                Internal::ServiceGeomToTopo g2t(getContext(), getGeomEntity(), m_extrem_vertices, &getInfoCommand());

                // convertion en un bloc structuré si cela est demandé
                if (isStructured() && g2t.convertBlockStructured(m_ni, m_nj, m_nk)){
                    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"Convertion en un bloc structuré impossible pour le volume "<<getGeomEntity()->getName();
                    throw TkUtil::Exception(message);
                }

                addCreatedBlock(g2t.getBlock());
                */
            }
            else if (m_dim == 2){
                // Utilise un service de création d'une topologie à partir d'une surface
                /*
                Internal::ServiceGeomToTopo g2t(getContext(), getGeomEntity(), m_extrem_vertices, &getInfoCommand());

                // convertion en une face commune structurés si cela est possible et demandé
                if (isStructured() && g2t.convertCoFaceStructured()){
                    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"Convertion en une face structurées impossible pour la surface "<<getGeomEntity()->getName();
                    throw TkUtil::Exception(message);
                }

                addCreatedCoFace(g2t.getCoFace());
                */
            }
            else {
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry imposible avec une géométrie autre qu'un Volume ou une Surface", TkUtil::Charset::UTF_8));
            }

            // suppression des parties internes de type save... qui n'ont pas lieu d'être
            // puisque l'on est en cours de création
            saveInternalsStats();
            deleteInternalsStats();

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

            Group::Group0D *group = getContext().getLocalGroupManager().getNewGroup0D(m_groupName,
                                                                                      &getInfoCommand());
            group->add(vtx);
            vtx->getGroupsContainer().add(group);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
        }
/*----------------------------------------------------------------------------*/
        CoEdge* CommandNewTopo::createCoEdge(Vertex* v0,Vertex* v1)
        {
            EdgeMeshingPropertyUniform emp(getContext().getLocalTopoManager().getDefaultNbMeshingEdges());
            CoEdge* coedge = new CoEdge(getContext(), &emp, v0, v1);

            Group::Group1D *group = getContext().getLocalGroupManager().getNewGroup1D(m_groupName,
                                                                                      &getInfoCommand());
            group->add(coedge);
            coedge->getGroupsContainer().add(group);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);
        }
/*----------------------------------------------------------------------------*/
        void CommandNewTopo::createFace()
        {
            Vertex* v0 = m_vertices[0];
            Vertex* v1 = m_vertices[1];
            Vertex* v2 = m_vertices[2];
            Vertex* v3 = m_vertices[3];

            gmds::math::Triangle t012(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                    gmds::math::Point(v1->getX(),v1->getY(),v1->getZ()),
                                    gmds::math::Point(v2->getX(),v2->getY(),v2->getZ()));
            gmds::math::Triangle t023(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                    gmds::math::Point(v2->getX(),v2->getY(),v2->getZ()),
                                    gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()));

            double dot = t012.getNormal().dot(t023.getNormal());

            int orientation = 0;

            if(dot < 0){
                gmds::math::Triangle t013(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                          gmds::math::Point(v1->getX(),v1->getY(),v1->getZ()),
                                          gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()));
                gmds::math::Triangle t032(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                          gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()),
                                          gmds::math::Point(v2->getX(),v2->getY(),v2->getZ()));

                dot = t013.getNormal().dot(t032.getNormal());

                if(dot < 0 ){
                    gmds::math::Triangle t031(gmds::math::Point(v0->getX(),v0->getY(),v0->getZ()),
                                              gmds::math::Point(v1->getX(),v1->getY(),v1->getZ()),
                                              gmds::math::Point(v3->getX(),v3->getY(),v3->getZ()));

                    dot = t031.getNormal().dot(t012.getNormal());
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
                        e0 = createCoEdge(v0,v1);

                    e1 = getCommonEdge(v1,v2);
                    if(e1 == nullptr)
                        e1 = createCoEdge(v1,v2);

                    e2 = getCommonEdge(v2,v3);
                    if(e2 == nullptr)
                        e2 = createCoEdge(v2,v3);

                    e3 = getCommonEdge(v3,v0);
                    if(e3 == nullptr)
                        e3 = createCoEdge(v3,v0);
                    break;
                case 1:
                    e0 = getCommonEdge(v0,v1);
                    if(e0 == nullptr)
                        e0 = createCoEdge(v0,v1);

                    e1 = getCommonEdge(v1,v3);
                    if(e1 == nullptr)
                        e1 = createCoEdge(v1,v3);

                    e2 = getCommonEdge(v3,v2);
                    if(e2 == nullptr)
                        e2 = createCoEdge(v3,v2);

                    e3 = getCommonEdge(v2,v0);
                    if(e3 == nullptr)
                        e3 = createCoEdge(v2,v0);
                    break;
                case 2:
                    e0 = getCommonEdge(v0,v2);
                    if(e0 == nullptr)
                        e0 = createCoEdge(v0,v2);

                    e1 = getCommonEdge(v2,v1);
                    if(e1 == nullptr)
                        e1 = createCoEdge(v2,v1);

                    e2 = getCommonEdge(v1,v3);
                    if(e2 == nullptr)
                        e2 = createCoEdge(v1,v3);

                    e3 = getCommonEdge(v3,v0);
                    if(e3 == nullptr)
                        e3 = createCoEdge(v3,v0);
                    break;
            }

            Topo::Edge* edge0 = new Edge(getContext(), e0);
            Topo::Edge* edge1 = new Edge(getContext(), e1);
            Topo::Edge* edge2 = new Edge(getContext(), e2);
            Topo::Edge* edge3 = new Edge(getContext(), e3);

            std::vector<Edge*> edges = {edge0,edge1,edge2,edge3};

            Topo::CoFace* face = new CoFace(getContext(), edges);

            Group::Group2D *group = getContext().getLocalGroupManager().getNewGroup2D(m_groupName,
                                                                                      &getInfoCommand());
            group->add(face);
            face->getGroupsContainer().add(group);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(face, Internal::InfoCommand::CREATED);
        }
/*----------------------------------------------------------------------------*/
void CommandNewTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
    return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
Topo::CoEdge* CommandNewTopo::getCommonEdge(const Vertex* v1, const Vertex* v2) {

    for(auto v1_e : v1->getCoEdges()){
        for(auto v2_e : v2->getCoEdges()){
            if(v1_e->getName() == v2_e->getName())
                return v1_e;
        }
    }

    return nullptr;
}

/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

