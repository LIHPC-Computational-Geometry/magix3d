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
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry pour un nombre de sommets non prévu", TkUtil::Charset::UTF_8));
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
                    throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry pour un type de topologie non prévu", TkUtil::Charset::UTF_8));
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
                createCoEdge();
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
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
        }
/*----------------------------------------------------------------------------*/
        void CommandNewTopo::createCoEdge()
        {
            Vertex* v0 = m_vertices[0];
            Vertex* v1 = m_vertices[1];

            bool commonEdgeExist = false;

            for(auto v0_e : v0->getCoEdges()){
                for(auto v1_e : v1->getCoEdges()){
                    commonEdgeExist = v0_e->getName() == v1_e->getName() || commonEdgeExist;
                }
            }

            if(commonEdgeExist){
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopo une arete existe déjà entre " + v0->getName() + " et " + v1->getName(), TkUtil::Charset::UTF_8));
            }

            EdgeMeshingPropertyUniform emp(getContext().getLocalTopoManager().getDefaultNbMeshingEdges());
            CoEdge* coedge = new CoEdge(getContext(), &emp, m_vertices[0], m_vertices[1]);

            Group::Group1D *group = getContext().getLocalGroupManager().getNewGroup1D(m_groupName,
                                                                                      &getInfoCommand());
            group->add(coedge);
            getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);

            getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);
        }
/*----------------------------------------------------------------------------*/
        void CommandNewTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
        {
            return getPreviewRepresentationNewCoedges(dr);
        }
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

