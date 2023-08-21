/*----------------------------------------------------------------------------*/
/** \file CommandEditTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 16/12/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/InfoCommand.h"
#include "Topo/CommandEditTopo.h"
#include "Topo/TopoManager.h"
#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
#include "Topo/FaceMeshingPropertyOrthogonal.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"

#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"

#include "Utils/Command.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
//#define _DEBUG_CANCEL
//#define _DEBUG_PREVIEW
/*----------------------------------------------------------------------------*/
CommandEditTopo::CommandEditTopo(Internal::Context& c, std::string name)
: Internal::CommandInternal (c, name)
, m_isPreview(false)
{}
/*----------------------------------------------------------------------------*/
CommandEditTopo::~CommandEditTopo()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandEditTopo::~CommandEditTopo() pour la commande "<<getScriptComments()<<std::endl;
#endif
    deleteInternalsStats();
    if (!getContext().isFinished())
        deleteCreated();
}
/*----------------------------------------------------------------------------*/
TopoManager& CommandEditTopo::getTopoManager()
{
    return getContext().getLocalTopoManager();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
postExecute(bool hasError)
{
#ifdef _DEBUG_MEMORY
	std::cout<<"CommandEditTopo::postExecute(hasError = "<<(hasError?"true":"false")<<")"<<std::endl;
#endif
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
internalUndo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // permute toutes les propriétés internes avec leur sauvegarde
    permInternalsStats();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
internalRedo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    // permute toutes les propriétés internes avec leur sauvegarde
    permInternalsStats();

}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
saveInternalsStats()
{
#ifdef _DEBUG_CANCEL
    std::cout<<"CommandEditTopo::saveInternalsStats()"<<std::endl;
#endif
    Internal::InfoCommand& icmd = getInfoCommand();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        te->saveInternals(this);
#ifdef _DEBUG_CANCEL
        std::cout<<"saveInternals pour "<<te->getName()<<std::endl;
#endif
    }
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::permInternalsStats()
{
//    std::cout<<"CommandEditTopo::permInternalsStats()"<<std::endl;
#define PERM_PROPERTY(T,L) \
    for (std::vector <T>::iterator iter = L.begin(); iter != L.end(); ++iter) \
		(*iter).m_property = (*iter).m_entity->setProperty((*iter).m_property);

    PERM_PROPERTY (TopoPropertyInfo,          m_topo_property_info);
    PERM_PROPERTY (VertexGeomPropertyInfo,    m_vertex_geom_property_info);
    PERM_PROPERTY (VertexTopoPropertyInfo,    m_vertex_topo_property_info);
    PERM_PROPERTY (CoEdgeTopoPropertyInfo,    m_coedge_topo_property_info);
    PERM_PROPERTY (CoEdgeMeshingPropertyInfo, m_coedge_mesh_property_info);
    PERM_PROPERTY (EdgeTopoPropertyInfo,      m_edge_topo_property_info);
    PERM_PROPERTY (EdgeMeshingPropertyInfo,   m_edge_mesh_property_info);
    PERM_PROPERTY (CoFaceTopoPropertyInfo,    m_coface_topo_property_info);
    PERM_PROPERTY (CoFaceMeshingPropertyInfo, m_coface_mesh_property_info);
    PERM_PROPERTY (FaceTopoPropertyInfo,      m_face_topo_property_info);
    PERM_PROPERTY (FaceMeshingPropertyInfo,   m_face_mesh_property_info);
    PERM_PROPERTY (BlockTopoPropertyInfo,     m_block_topo_property_info);
    PERM_PROPERTY (BlockMeshingPropertyInfo,  m_block_mesh_property_info);

#undef PERM_PROPERTY
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::cancelInternalsStats()
{
#ifdef _DEBUG_CANCEL
    std::cout<<"CommandEditTopo::cancelInternalsStats() ..."<<std::endl;
#endif
    saveInternalsStats();
    permInternalsStats();
    deleteInternalsStats();

    getInfoCommand().permCreatedDeleted();
    deleteMarkedDeleted();
    getInfoCommand().clear();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::deleteInternalsStats()
{
//    std::cout<<"CommandEditTopo::deleteInternalsStats()"<<std::endl;

#define DELETE_PROPERTY(T,L) \
    for (std::vector <T>::iterator iter = L.begin(); iter != L.end(); ++iter) { \
       delete (*iter).m_property; \
    } \
    L.clear();

    DELETE_PROPERTY (TopoPropertyInfo,          m_topo_property_info);
    DELETE_PROPERTY (VertexTopoPropertyInfo,    m_vertex_topo_property_info);
    DELETE_PROPERTY (VertexGeomPropertyInfo,    m_vertex_geom_property_info);
    DELETE_PROPERTY (CoEdgeTopoPropertyInfo,    m_coedge_topo_property_info);
    DELETE_PROPERTY (CoEdgeMeshingPropertyInfo, m_coedge_mesh_property_info);
    DELETE_PROPERTY (EdgeTopoPropertyInfo,      m_edge_topo_property_info);
    DELETE_PROPERTY (EdgeMeshingPropertyInfo,   m_edge_mesh_property_info);
    DELETE_PROPERTY (CoFaceTopoPropertyInfo,    m_coface_topo_property_info);
    DELETE_PROPERTY (CoFaceMeshingPropertyInfo, m_coface_mesh_property_info);
    DELETE_PROPERTY (FaceTopoPropertyInfo,      m_face_topo_property_info);
    DELETE_PROPERTY (FaceMeshingPropertyInfo,   m_face_mesh_property_info);
    DELETE_PROPERTY (BlockTopoPropertyInfo,     m_block_topo_property_info);
    DELETE_PROPERTY (BlockMeshingPropertyInfo,  m_block_mesh_property_info);

#undef DELETE_PROPERTY

//#ifdef _DEBUG
//    std::cout<<"CommandEditTopo::deleteInternalsStats() FIN "<<std::endl;
//#endif
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::deleteCreated()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandEditTopo::deleteCreated() avec InfoCommand:"<<getInfoCommand()<<std::endl;
#endif
    // on détruit les entités marquées comme détruites
    // elles avaient été créées avec la commande
    Internal::InfoCommand& icmd = getInfoCommand();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        Internal::InfoCommand::type t = iter_tei->second;
        //te->setDestroyed(true);
        //std::cout<<"te : "<<te->getName()<<", t = "<<Internal::InfoCommand::type2String(t)<<std::endl;
        if (t == Internal::InfoCommand::DELETED){

            if (te->getType() == Utils::Entity::TopoBlock){
                Block* bloc = dynamic_cast<Block*>(te);
                if (bloc == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::deleteCreated", TkUtil::Charset::UTF_8));
                getTopoManager().remove(bloc);
            } else if (te->getType() == Utils::Entity::TopoFace){
                Face* face = dynamic_cast<Face*>(te);
                if (face == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::deleteCreated", TkUtil::Charset::UTF_8));
                getTopoManager().remove(face);
            } else if (te->getType() == Utils::Entity::TopoCoFace){
                CoFace* coface = dynamic_cast<CoFace*>(te);
                if (coface == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::deleteCreated", TkUtil::Charset::UTF_8));
                getTopoManager().remove(coface);
            } else if (te->getType() == Utils::Entity::TopoEdge){
                Edge* edge = dynamic_cast<Edge*>(te);
                if (edge == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::deleteCreated", TkUtil::Charset::UTF_8));
                getTopoManager().remove(edge);
            } else if (te->getType() == Utils::Entity::TopoCoEdge){
                CoEdge* coedge = dynamic_cast<CoEdge*>(te);
                if (coedge == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::deleteCreated", TkUtil::Charset::UTF_8));
                getTopoManager().remove(coedge);
            } else if (te->getType() == Utils::Entity::TopoVertex){
                Vertex* vertex = dynamic_cast<Vertex*>(te);
                if (vertex == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::deleteCreated", TkUtil::Charset::UTF_8));
                getTopoManager().remove(vertex);
            }

            te->clearDependancy();
            delete te;
        } // end if (t == Internal::InfoCommand::DELETED)
    } // end for iter
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
deleteMarkedDeleted()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandEditTopo::deleteMarkedDeleted() avec InfoCommand:"<<getInfoCommand()<<std::endl;
#endif
    // on détruit les entités marquées comme détruites
    // elles avaient été créées avec la commande
    Internal::InfoCommand& icmd = getInfoCommand();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        Internal::InfoCommand::type t = iter_tei->second;
        //std::cout<<"te : "<<te->getName()<<", t = "<<Internal::InfoCommand::type2String(t)<<std::endl;
        if (t == Internal::InfoCommand::DELETED){
            delete te;
        } // end if (t == Internal::InfoCommand::DELETED)
    } // end for iter
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
cleanTemporaryEntities()
{
    // on utilise un vecteur pour stocker les entités à supprimer de la map
    // sinon le parcours est modifié (on rate des éléments)
    std::vector<Topo::TopoEntity*> toDelete;

    Internal::InfoCommand& icmd = getInfoCommand();
    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>& topo_entities_info = icmd.getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        Internal::InfoCommand::type& t = iter_tei->second;

        if (te->isDestroyed() && t == Internal::InfoCommand::NONE){
            //std::cout<<"cleanTemporaryEntities() pour entité "<<te->getName()<<" (uid "<< te->getUniqueId()<<")"<<std::endl;
            toDelete.push_back(te);
            delete te;
        }
    }

    for (uint i=0; i<toDelete.size(); i++)
        topo_entities_info.erase(toDelete[i]);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addTopoInfoProperty(TopoEntity* te, TopoProperty* tp)
{
    TopoPropertyInfo ti = {te,tp};
    m_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addVertexInfoTopoProperty(Vertex* te, VertexTopoProperty* tp)
{
    VertexTopoPropertyInfo ti = {te,tp};
    m_vertex_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addVertexInfoGeomProperty(Vertex* te, VertexGeomProperty* tp)
{
    VertexGeomPropertyInfo ti = {te,tp};
    m_vertex_geom_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addCoEdgeInfoTopoProperty(CoEdge* te, CoEdgeTopoProperty* tp)
{
    CoEdgeTopoPropertyInfo ti = {te,tp};
    m_coedge_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addCoEdgeInfoMeshingProperty(CoEdge* te, CoEdgeMeshingProperty* tp)
{
    CoEdgeMeshingPropertyInfo ti = {te,tp};
    m_coedge_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addEdgeInfoTopoProperty(Edge* te, EdgeTopoProperty* tp)
{
    EdgeTopoPropertyInfo ti = {te,tp};
    m_edge_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addEdgeInfoMeshingProperty(Edge* te, EdgeMeshingProperty* tp)
{
    EdgeMeshingPropertyInfo ti = {te,tp};
    m_edge_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addCoFaceInfoTopoProperty(CoFace* te, CoFaceTopoProperty* tp)
{
#ifdef _DEBUG2
	std::cout<<"addCoFaceInfoTopoProperty pour "<<te->getName()<<std::endl;
#endif
    CoFaceTopoPropertyInfo ti = {te,tp};
    m_coface_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addCoFaceInfoMeshingProperty(CoFace* te, CoFaceMeshingProperty* tp)
{
    CoFaceMeshingPropertyInfo ti = {te,tp};
    m_coface_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addFaceInfoTopoProperty(Face* te, FaceTopoProperty* tp)
{
    FaceTopoPropertyInfo ti = {te,tp};
    m_face_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addFaceInfoMeshingProperty(Face* te, FaceMeshingProperty* tp)
{
    FaceMeshingPropertyInfo ti = {te,tp};
    m_face_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addBlockInfoTopoProperty(Block* te, BlockTopoProperty* tp)
{
    BlockTopoPropertyInfo ti = {te,tp};
    m_block_topo_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
addBlockInfoMeshingProperty(Block* te, BlockMeshingProperty* tp)
{
    BlockMeshingPropertyInfo ti = {te,tp};
    m_block_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
registerToManagerCreatedEntities()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandEditTopo::registerToManagerCreatedEntities() avec :"<<std::endl;
    std::cout<<getInfoCommand()<<std::endl;
#endif

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>& tie = getInfoCommand().getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter = tie.begin();
            iter != tie.end(); ++iter){
        Topo::TopoEntity* te = iter->first;
        Internal::InfoCommand::type t = iter->second;

        // on ne s'intéresse qu'au entités de type "utilisateur"
        // on ne prend pas les Face et Edge, mais seulement les Block, CoFace, CoEdge et Vertex
        if (t == Internal::InfoCommand::CREATED){
            if (te->getType() == Utils::Entity::TopoBlock){
                Block* bloc = dynamic_cast<Block*>(te);
                if (bloc == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::registerToManagerCreatedEntities", TkUtil::Charset::UTF_8));
                getTopoManager().add(bloc);
            } else if (te->getType() == Utils::Entity::TopoFace){
                Face* face = dynamic_cast<Face*>(te);
                if (face == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::registerToManagerCreatedEntities", TkUtil::Charset::UTF_8));
                getTopoManager().add(face);
            } else if (te->getType() == Utils::Entity::TopoCoFace){
                CoFace* coface = dynamic_cast<CoFace*>(te);
                if (coface == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::registerToManagerCreatedEntities", TkUtil::Charset::UTF_8));
                getTopoManager().add(coface);
            } else if (te->getType() == Utils::Entity::TopoEdge){
                Edge* edge = dynamic_cast<Edge*>(te);
                if (edge == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::registerToManagerCreatedEntities", TkUtil::Charset::UTF_8));
                getTopoManager().add(edge);
            } else if (te->getType() == Utils::Entity::TopoCoEdge){
                CoEdge* coedge = dynamic_cast<CoEdge*>(te);
                if (coedge == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::registerToManagerCreatedEntities", TkUtil::Charset::UTF_8));
                getTopoManager().add(coedge);
            } else if (te->getType() == Utils::Entity::TopoVertex){
                Vertex* vertex = dynamic_cast<Vertex*>(te);
                if (vertex == 0)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandEditTopo::registerToManagerCreatedEntities", TkUtil::Charset::UTF_8));
                getTopoManager().add(vertex);
            }
        } // end if (t == Internal::InfoCommand::CREATED)
    } // end for iter

}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
updateMeshLaw(std::list<Topo::CoFace*>& l_f, std::list<Topo::Block*>& l_b)
{
#ifdef _DEBUG2
    uint nb_cofaces = 0;
    uint nb_blocs = 0;
#endif
   for(std::list<Topo::CoFace*>::iterator iter=l_f.begin(); iter != l_f.end(); ++iter){
        Topo::CoFace* coface = *iter;
        if (coface->isDestroyed())
            continue;
#ifdef _DEBUG2
       nb_cofaces += 1;
#endif
       // cas d'une méthode structurée directionnelle ou rotationnelle
       if (coface->getMeshLaw() < CoFaceMeshingProperty::transfinite){

           // on cherche une méthode la plus basique possible
           coface->selectBasicMeshLaw(&getInfoCommand(), true);

       } // end if (coface->getMeshLaw() <= rotJ)
   } // end for iter=l_f.begin()

    // il est préférable de le faire sur les blocs après les faces communes
    // c'est pour cela que tout est dans une unique méthode
    for(std::list<Topo::Block*>::iterator iter=l_b.begin(); iter != l_b.end(); ++iter){
        Topo::Block* bloc = *iter;
        if (bloc->isDestroyed())
            continue;
#ifdef _DEBUG2
       nb_blocs += 1;
#endif
        if (bloc->getMeshLaw() < BlockMeshingProperty::transfinite){

            // on cherche une méthode la plus basique possible
            bloc->selectBasicMeshLaw(&getInfoCommand(), true);

        } // end if (bloc->getMeshLaw() <= rotK)
    } // end for iter=l_b.begin()
#ifdef _DEBUG2
    std::cout<<"CommandEditTopo::updateMeshLaw avec "<<nb_cofaces<<" cofaces et "<<nb_blocs<<" blocs"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandEditTopo::countNbCoEdgesByVertices n'a pas été redéfinie", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
previewBegin()
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"previewBegin"<<std::endl;
#endif
    // cas où la commande doit être détuite, il faut en créer une autre ...
    if (m_isPreview){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "La commande " << getName ( )
    	        << "  ne peut faire qu'un seul preview";
    	throw TkUtil::Exception(message);
    }

    getContext().getNameManager().getInternalStats(m_name_manager_before);


    bool            hasError    = false;

    // effectue la commande
    try {
    	internalExecute();
        if (Command::CANCELED == getStatus ( ))
        	hasError    = true;
    }
    catch (...)
    {
#ifdef _DEBUG_PREVIEW
    	std::cout<<"hasError => true"<<std::endl;
    	std::cout<<" avec comme message: "<<getErrorMessage()<<std::endl;
#endif
    	hasError    = true;
    }

    // fait le ménage si nécessaire
    postExecute(hasError);
    if (hasError){
        previewEnd();
        throw TkUtil::Exception (TkUtil::UTF8String ("Commande en erreur lors du preview", TkUtil::Charset::UTF_8));
    }

    // on s'assure de ne pas la relancer une seconde fois
    m_isPreview = true;


#ifdef _DEBUG_PREVIEW
    std::cout<<"==== arêtes créées en sortie de previewBegin ===="<<std::endl;
    Internal::InfoCommand& icmd = getInfoCommand();
    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
    		iter_tei != topo_entities_info.end(); ++iter_tei){
    	Topo::TopoEntity* te = iter_tei->first;
    	Internal::InfoCommand::type t = iter_tei->second;
    	if (t == Internal::InfoCommand::CREATED && te->getType() == Utils::Entity::TopoCoEdge){
    		std::cout << " arête "<<te->getName()<<std::endl;
    	}
    }
    std::cout<<"================================================="<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
previewEnd()
{
    // annulation de la commande
    CommandEditTopo::internalUndo();

    // remet les id comme au départ de la commande
    getContext().getNameManager().setInternalStats(m_name_manager_before);
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
getPreviewRepresentationTopoModif(Utils::DisplayRepresentation& dr)
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"getPreviewRepresentationTopoModif"<<std::endl;
#endif

    // stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié (avant la commande)
    std::map<Topo::Vertex*, uint> nb_coedges_by_vertex;
    countNbCoEdgesByVertices(nb_coedges_by_vertex);

    previewBegin();

    // parcours les arêtes nouvelles et ajoute leur représentation
    Internal::InfoCommand& icmd = getInfoCommand();
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indices = dr.getCurveDiscretization();
    points.clear();
    indices.clear();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
    		iter_tei != topo_entities_info.end(); ++iter_tei){
    	Topo::TopoEntity* te = iter_tei->first;
    	Internal::InfoCommand::type t = iter_tei->second;
    	if (t == Internal::InfoCommand::CREATED && te->getType() == Utils::Entity::TopoCoEdge){
    		CoEdge* coedge = dynamic_cast<CoEdge*>(te);
    		CHECK_NULL_PTR_ERROR(coedge);
#ifdef _DEBUG_PREVIEW
    		std::cout<<" getPreviewRepresentation coedge "<<coedge->getName()<<std::endl;
#endif
    		// on regarde si l'un des sommets a son nombre d'arêtes qui n'a pas changé
    		bool nb_change = true;
    		std::vector<Topo::Vertex* > vertices;
    		coedge->getVertices(vertices);
    		for (uint i=0; i<vertices.size(); i++)
    			if (vertices[i]->getNbCoEdges() == nb_coedges_by_vertex[vertices[i]]){
    				nb_change = false;
    				//std::cout<<"  "<<vertices[i]->getName()<<" avait "<<nb_coedges_by_vertex[vertices[i]]<<" arêtes et en a désormais "<<vertices[i]->getNbCoEdges()<<std::endl;
    			}

    		// on prend en compte les arêtes dont les 2 sommets on un nombre d'arêtes qui ont changé
    		if (nb_change){
#ifdef _DEBUG_PREVIEW
    			std::cout<<"  On ajoute l'arête "<<coedge->getName()<<std::endl;
#endif
   			for (uint i=0; i<vertices.size(); i++){
    				indices.push_back(points.size());
    				points.push_back(vertices[i]->getCoord());
    			}
    		}
    	}
    } // end for iter_tei

    previewEnd();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
getPreviewRepresentationNewCoedges(Utils::DisplayRepresentation& dr)
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"getPreviewRepresentationNewCoedges"<<std::endl;
#endif

	previewBegin();

    // parcours les arêtes créées et ajoute leur représentation
    Internal::InfoCommand& icmd = getInfoCommand();
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indices = dr.getCurveDiscretization();
    points.clear();
    indices.clear();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
    		iter_tei != topo_entities_info.end(); ++iter_tei){
    	Topo::TopoEntity* te = iter_tei->first;
    	Internal::InfoCommand::type t = iter_tei->second;
    	if (t == Internal::InfoCommand::CREATED && te->getType() == Utils::Entity::TopoCoEdge){
    		CoEdge* coedge = dynamic_cast<CoEdge*>(te);
    		CHECK_NULL_PTR_ERROR(coedge);
    		std::vector<Topo::Vertex* > vertices;
    		coedge->getVertices(vertices);
#ifdef _DEBUG_PREVIEW
    		std::cout<<"  On ajoute l'arête "<<coedge->getName()<<std::endl;
#endif
    		for (uint i=0; i<vertices.size(); i++){
    			indices.push_back(points.size());
    			points.push_back(vertices[i]->getCoord());
    		}
    	}
    } // end for iter_tei

    previewEnd();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
getPreviewRepresentationCoedgeDisplayModified(Utils::DisplayRepresentation& dr)
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"getPreviewRepresentationCoedgeDisplayModified"<<std::endl;
#endif

	previewBegin();

    // parcours les arêtes modifiées et ajoute leur représentation
    Internal::InfoCommand& icmd = getInfoCommand();
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indices = dr.getCurveDiscretization();
    points.clear();
    indices.clear();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
    		iter_tei != topo_entities_info.end(); ++iter_tei){
    	Topo::TopoEntity* te = iter_tei->first;
    	Internal::InfoCommand::type t = iter_tei->second;
    	if (t == Internal::InfoCommand::DISPMODIFIED && te->getType() == Utils::Entity::TopoCoEdge){
    		CoEdge* coedge = dynamic_cast<CoEdge*>(te);
    		CHECK_NULL_PTR_ERROR(coedge);
    		std::vector<Topo::Vertex* > vertices;
    		coedge->getVertices(vertices);
#ifdef _DEBUG_PREVIEW
    		std::cout<<"  On ajoute l'arête "<<coedge->getName()<<std::endl;
#endif
   		for (uint i=0; i<vertices.size(); i++){
    			indices.push_back(points.size());
    			points.push_back(vertices[i]->getCoord());
    		}
    	}
    } // end for iter_tei

    previewEnd();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
getPreviewRepresentationDeletedTopo(Utils::DisplayRepresentation& dr)
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"getPreviewRepresentationDeletedTopo"<<std::endl;
#endif

	previewBegin();

    // parcours les arêtes modifiées et ajoute leur représentation
    Internal::InfoCommand& icmd = getInfoCommand();
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indices = dr.getCurveDiscretization();
    points.clear();
    indices.clear();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
    		iter_tei != topo_entities_info.end(); ++iter_tei){
    	Topo::TopoEntity* te = iter_tei->first;
    	Internal::InfoCommand::type t = iter_tei->second;
    	if (t == Internal::InfoCommand::DELETED){
    		te->getRepresentation(dr, true);
    	}
    } // end for iter_tei

    previewEnd();
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::setOrthogonalCoEdges(CoFace* coface, FaceMeshingPropertyOrthogonal* mp)
{
	if (mp->getDir() == 0){
		// cas orthogonalité suivant I
		if (mp->getSide() == 0){
			setOrthogonalCoEdges(coface->getEdge(1), coface->getVertex(1), mp->getNbLayers());
			// pour cas non dégénéré avec 3 sommets
			if (coface->getNbVertices() == 4)
				setOrthogonalCoEdges(coface->getEdge(3), coface->getVertex(0), mp->getNbLayers());
		}
		else {
			setOrthogonalCoEdges(coface->getEdge(1), coface->getVertex(2), mp->getNbLayers());
			// pour cas non dégénéré avec 3 sommets
			if (coface->getNbVertices() == 4)
				setOrthogonalCoEdges(coface->getEdge(3), coface->getVertex(3), mp->getNbLayers());
		}

	} else {
		// cas orthogonalité suivant J
		if (mp->getSide() == 0){
			setOrthogonalCoEdges(coface->getEdge(0), coface->getVertex(1), mp->getNbLayers());
			setOrthogonalCoEdges(coface->getEdge(2), coface->getVertex(2), mp->getNbLayers());
		}
		else {
			setOrthogonalCoEdges(coface->getEdge(0), coface->getVertex(0), mp->getNbLayers());
			setOrthogonalCoEdges(coface->getEdge(2), coface->getVertex(3), mp->getNbLayers());
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::setOrthogonalCoEdges(Edge* edge, Vertex* vertex, int nbLayers)
{
	if (edge->getNbCoEdges() == 1){
		CoEdge* coedge = edge->getCoEdge(0);

		CoEdgeMeshingProperty *cemp = coedge->getMeshingProperty()->clone();
		bool sens = (coedge->getVertex(0)==vertex);
		cemp->setOrthogonal(nbLayers, sens);
		if ((*cemp) != (*coedge->getMeshingProperty()))
			coedge->switchCoEdgeMeshingProperty(&getInfoCommand(), cemp);
		delete cemp;
	}
	else {
		MGX_NOT_YET_IMPLEMENTED("Propagation orthogonalité pour plusieurs arêtes sur bord de face");
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"Propagation orthogonalité pour arête "<<edge->getName()<<" sur bord d'une face n'est pas implémentée";
		message <<"Cette arête est composée de plusieurs arêtes communes";
		getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
	}
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
duplicate(std::vector<CoFace*>& cofaces_dep,
        std::map<Vertex*, uint>& filtre_vertex,
        std::map<CoEdge*, uint>& filtre_coedge,
        std::map<CoFace*, uint>& filtre_coface,
        std::vector<CoFace*>& cofaces_arr)
{
#ifdef _DEBUG_REVOL
	std::cout<<"duplicate ..."<<std::endl;
#endif
    // les maps de correspondance entre les entités de départ et d'arrivé pour éviter de dupliquer plusieurs fois
    std::map<Vertex*, Vertex*> corr_vertex;
    std::map<std::string, CoEdge*> corr_coedge;
    std::map<Edge*, Edge*> corr_edge;
    std::map<CoFace*, CoFace*> corr_coface;

    for (std::vector<CoFace*>::iterator iter1 = cofaces_dep.begin();
            iter1 != cofaces_dep.end(); ++iter1){

        CoFace* newCoface = corr_coface[*iter1];

        if (!newCoface) {
#ifdef _DEBUG_REVOL
            std::cout<<"== duplique "<<(*iter1)->getName()<<std::endl;
#endif
            std::vector<Edge* > edges_dep;
            std::vector<Edge* > edges_arr;
            (*iter1)->getEdges(edges_dep);

            for (std::vector<Edge* >::iterator iter2 = edges_dep.begin();
                    iter2 != edges_dep.end(); ++iter2){

                Edge* newEdge = corr_edge[*iter2];
                if (!newEdge){

                    std::vector<CoEdge* > coedges_dep;
                    std::vector<CoEdge* > coedges_arr;
                    (*iter2)->getCoEdges(coedges_dep);

                    for (std::vector<CoEdge* >::iterator iter3 = coedges_dep.begin();
                            iter3 != coedges_dep.end(); ++iter3){

                        CoEdge* newCoEdge = corr_coedge[(*iter3)->getName()];
                        if (!newCoEdge){
                            const std::vector<Vertex* > & vertices_dep = (*iter3)->getVertices();
                            std::vector<Vertex* > vertices_arr;

                            for (std::vector<Vertex* >::const_iterator iter4 = vertices_dep.begin();
                                    iter4 != vertices_dep.end(); ++iter4){

                                Vertex* newVertex = corr_vertex[*iter4];
                                if (!newVertex){
                                    if (filtre_vertex[*iter4] >= 10)
                                        // cas sur l'axe, on conserve l'entité
                                        newVertex = *iter4;
                                    else {
                                        newVertex = (*iter4)->clone();
                                        filtre_vertex[newVertex] = filtre_vertex[*iter4];
                                        getInfoCommand().addTopoInfoEntity(newVertex, Internal::InfoCommand::CREATED);
                                    }
                                    corr_vertex[*iter4] = newVertex;
#ifdef _DEBUG_REVOL
                                    std::cout<<"Corr_vertex "<<(*iter4)->getName()<<" => "<<newVertex->getName()<<std::endl;
#endif
                                } // end if (!newVertex)

                                vertices_arr.push_back(newVertex);

                            } // end for vertices_dep

                            if (filtre_coedge[*iter3] >= 20)
                                // cas sur l'axe, on conserve l'entité
                                newCoEdge = *iter3;
                            else {
                                newCoEdge = new Topo::CoEdge(getContext(),
                                        (*iter3)->getMeshingProperty(),
                                        vertices_arr[0], vertices_arr[1]);
#ifdef _DEBUG_REVOL
                                std::cout<<" Création de la CoEdge "<<newCoEdge->getName()<<" à partir de "<<(*iter3)->getName()<<std::endl;
#endif
                                newCoEdge->setGeomAssociation((*iter3)->getGeomAssociation());
                                filtre_coedge[newCoEdge] = filtre_coedge[*iter3];
                                getInfoCommand().addTopoInfoEntity(newCoEdge, Internal::InfoCommand::CREATED);
                            }
                            corr_coedge[(*iter3)->getName()] = newCoEdge;
#ifdef _DEBUG_REVOL
                            std::cout<<"corr_coedge "<<(*iter3)->getName()<<" => "<<newCoEdge->getName()<<std::endl;
#endif
                        } // end if (!newCoEdge)

                        coedges_arr.push_back(newCoEdge);

                    } // end for coedges_dep

                    // on duplique même si on est sur l'axe
                    newEdge = new Topo::Edge(getContext(),
                            corr_vertex[(*iter2)->getVertex(0)],
                            corr_vertex[(*iter2)->getVertex(1)],
                            coedges_arr);
                    // reprise de la semi-conformité
                    for (std::vector<CoEdge* >::iterator iter3 = coedges_dep.begin();
                    		iter3 != coedges_dep.end(); ++iter3){
                    	CoEdge* newCoEdge = corr_coedge[(*iter3)->getName()];
                    	uint ratio = (*iter2)->getRatio(*iter3);
                    	if (ratio != 1)
                    		newEdge->setRatio(newCoEdge, ratio);
                    }
                    getInfoCommand().addTopoInfoEntity(newEdge, Internal::InfoCommand::CREATED);
                    corr_edge[*iter2] = newEdge;

                } // end if (!newEdge)

                edges_arr.push_back(newEdge);

            } // end for edges_dep

            // on utilise les sommets de la face de départ pour ordonner ceux de la face d'arrivée
            const std::vector<Vertex* > & vertices_dep = (*iter1)->getVertices();
            std::vector<Vertex* > vertices_arr;
            for (std::vector<Vertex* >::const_iterator iter4 = vertices_dep.begin();
                    iter4 != vertices_dep.end(); ++iter4)
                vertices_arr.push_back(corr_vertex[*iter4]);

            newCoface = new Topo::CoFace(getContext(), edges_arr, vertices_arr,
                    (*iter1)->isStructured(), (*iter1)->hasHole());
#ifdef _DEBUG_REVOL
            std::cout<<" Création de la CoFace "<<newCoface->getName()<<" par copie"<<std::endl;
#endif

            newCoface->setGeomAssociation((*iter1)->getGeomAssociation());

            CoFaceMeshingProperty* prop = (*iter1)->getCoFaceMeshingProperty();
            CoFaceMeshingProperty* oldProp = newCoface->setProperty(prop->clone());
            delete oldProp;

            filtre_coface[newCoface] = filtre_coface[*iter1];
            getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);
            corr_coface[*iter1] = newCoface;
#ifdef _DEBUG_REVOL
            std::cout<<"corr_coface "<<(*iter1)->getName()<<" => "<<newCoface->getName()<<std::endl;
#endif
            // mise à jour des arêtes interpolées
            std::vector<CoEdge* > coedges;
            newCoface->getCoEdges(coedges, false);
            for (uint i=0; i<coedges.size(); i++)
            	if (filtre_coedge[coedges[i]] < 20) // cas hors de l'axe
            		updateCoEdgesInterpolateNames(coedges[i], corr_coedge,
            				(*iter1)->getName(), newCoface->getName());

        } // end if (!newCoface)

        cofaces_arr.push_back(newCoface);

    } // end for cofaces_dep

}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
updateGeomAssociation(std::vector<CoFace*>& cofaces,
		std::map<Geom::Vertex*,Geom::Vertex*>& V2V,
		std::map<Geom::Curve*,Geom::Curve*>& C2C,
		std::map<Geom::Surface*,Geom::Surface*>& S2S,
		bool onlyIfDestroyed)
{
#ifdef _DEBUG2
    std::cout<<"updateGeomAssociation(V2V, C2C, S2S)"<<std::endl;
    for (std::map<Geom::Vertex*,Geom::Vertex*>::iterator iter = V2V.begin(); iter != V2V.end(); ++iter)
    	std::cout<<iter->first->getName()<<" => "<< (iter->second?iter->second->getName():"0")<<std::endl;
    for (std::map<Geom::Curve*,Geom::Curve*>::iterator iter = C2C.begin(); iter != C2C.end(); ++iter)
    	std::cout<<iter->first->getName()<<" => "<< (iter->second?iter->second->getName():"0")<<std::endl;
    for (std::map<Geom::Surface*,Geom::Surface*>::iterator iter = S2S.begin(); iter != S2S.end(); ++iter)
    	std::cout<<iter->first->getName()<<" => "<< (iter->second?iter->second->getName():"0")<<std::endl;

#endif
   // on marque les différentes entités concernées pour ne les toucher qu'une fois
    std::map<Vertex*, uint> filtre_vertex;
    std::map<CoEdge*, uint> filtre_coedge;

    for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){

        if (getInfoCommand().addTopoInfoEntity(*iter1,Internal::InfoCommand::DISPMODIFIED))
            (*iter1)->saveTopoProperty();

        Geom::GeomEntity* ge = (*iter1)->getGeomAssociation();
        Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
        if (surf)
            (*iter1)->setGeomAssociation(S2S[surf]);

        std::vector<CoEdge* > coedges;
        (*iter1)->getCoEdges(coedges);

        for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                iter3 != coedges.end(); ++iter3){

            if (filtre_coedge[*iter3] == 0){
                filtre_coedge[*iter3] = 1;

                if (getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DISPMODIFIED))
                    (*iter3)->saveTopoProperty();

                Geom::GeomEntity* ge = (*iter3)->getGeomAssociation();
#ifdef _DEBUG2
                   std::cout<<" traite"<<*(*iter3)<<std::endl;
#endif

                // on laisse les relation vers une entité qui n'est pas détruite (cas sur l'axe par ex)
                if (ge==0 || (onlyIfDestroyed && !ge->isDestroyed()))
                    continue;

                Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
                if (crv){
                    (*iter3)->setGeomAssociation(C2C[crv]);
#ifdef _DEBUG2
                   std::cout<<"  ->setGeomAssociation("<<(C2C[crv]?C2C[crv]->getName():"0")<<")"<<std::endl;
#endif

                }
                else {
                    Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
                    if (surf)
                        (*iter3)->setGeomAssociation(S2S[surf]);
                }

                std::vector<Vertex* > vertices;
                (*iter3)->getVertices(vertices);
                getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DISPMODIFIED);

                for (std::vector<Vertex* >::iterator iter4 = vertices.begin();
                        iter4 != vertices.end(); ++iter4){
                    Geom::GeomEntity* ge = (*iter4)->getGeomAssociation();
#ifdef _DEBUG2
                    std::cout<<" traite"<<*(*iter4)<<std::endl;
                    std::cout<<(*iter4)->getName()<<" filtre_vertex="<<filtre_vertex[*iter4];
                    if (ge)
                    	std::cout<<" ge="<<ge->getName()<<(ge->isDestroyed()?" est détruit":" n'est pas détruit")<<std::endl;
                    else
                    	std::cout<<" ge=rien"<<std::endl;
#endif
                    if (filtre_vertex[*iter4] == 0 && ge && (ge->isDestroyed() || !onlyIfDestroyed)){
                        filtre_vertex[*iter4] = 1;

                        if (getInfoCommand().addTopoInfoEntity(*iter4,Internal::InfoCommand::DISPMODIFIED))
                            (*iter4)->saveTopoProperty();

                        Geom::Vertex* vtx = dynamic_cast<Geom::Vertex*>(ge);
                        if (vtx)
                            (*iter4)->setGeomAssociation(V2V[vtx]);
                        else {
                            Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
                            if (crv)
                                (*iter4)->setGeomAssociation(C2C[crv]);
                            else {
                                Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
                                if (surf)
                                    (*iter4)->setGeomAssociation(S2S[surf]);
                            }
                        }
                    } // end if (filtre_vertex[*iter4] == 0 && ge)
                } // end for vertices
            } // end if (filtre_coedge[*iter3] == 0)
        } // end for coedges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
updateGeomAssociation(std::vector<CoFace*>& cofaces,
            std::map<Geom::Vertex*,Geom::Curve*>& V2C,
            std::map<Geom::Curve*,Geom::Surface*>& C2S,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge)
{
//    std::cout<<"updateGeomAssociation(V2C, C2S)..."<<std::endl;

    // on marque les différentes entités concernées pour ne les toucher qu'une fois
    std::map<Vertex*, uint> filtre_vertex_vu;
    std::map<CoEdge*, uint> filtre_coedge_vu;

    for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){

        if (getInfoCommand().addTopoInfoEntity(*iter1,Internal::InfoCommand::DISPMODIFIED))
            (*iter1)->saveTopoProperty();

        (*iter1)->setGeomAssociation(0);

        std::vector<CoEdge* > coedges;
        (*iter1)->getCoEdges(coedges);

        for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                iter3 != coedges.end(); ++iter3){

            if (filtre_coedge_vu[*iter3] == 0){
                filtre_coedge_vu[*iter3] = 1;

                if (getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DISPMODIFIED))
                    (*iter3)->saveTopoProperty();

                std::vector<Vertex* > vertices;
                (*iter3)->getVertices(vertices);
                getInfoCommand().addTopoInfoEntity(*iter3,Internal::InfoCommand::DISPMODIFIED);

                for (std::vector<Vertex* >::iterator iter4 = vertices.begin();
                        iter4 != vertices.end(); ++iter4){
                    Geom::GeomEntity* ge = (*iter4)->getGeomAssociation();

                    // on laisse les relation vers une entité qui n'est pas détruite (cas sur l'axe par ex)
                    if (ge==0 || !ge->isDestroyed())
                        continue;

                    if (filtre_vertex_vu[*iter4] == 0 && filtre_vertex[*iter4]%5 == 1){
                        filtre_vertex_vu[*iter4] = 1;
                        // les sommets sur l'ogrid ont été traité à part
                    }
                    else if (filtre_vertex_vu[*iter4] == 0 && ge){
                        filtre_vertex_vu[*iter4] = 1;

                        if (getInfoCommand().addTopoInfoEntity(*iter4,Internal::InfoCommand::DISPMODIFIED))
                            (*iter4)->saveTopoProperty();

                        Geom::Vertex* vtx = dynamic_cast<Geom::Vertex*>(ge);
                        if (vtx)
                            (*iter4)->setGeomAssociation(V2C[vtx]);
                        else {
                            Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);
                            if (crv){
                                //std::cout<<"updateGeomAssociation(C2V) setGeomAssociation(...) pour "<<(*iter4)->getName()<<std::endl;
                                (*iter4)->setGeomAssociation(C2S[crv]);
                            }
                            else {
                                Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
                                if (surf){
                                    //std::cout<<"updateGeomAssociation(V2C) setGeomAssociation(0) pour "<<(*iter4)->getName()<<std::endl;
                                    (*iter4)->setGeomAssociation(0);
                                }
                            }
                        }
                    } // end else if (filtre_vertex[*iter4] == 0 && ge)
                } // end for vertices

                // suppression des projection pour les arêtes sur l'ogrid
                if (filtre_coedge[*iter3]%10 == 1){
                    (*iter3)->setGeomAssociation(0);
                    //std::cout<<"Suppression de la projection pour "<<(*iter3)->getName()<<std::endl;
                }
                else {
                    Geom::GeomEntity* ge = (*iter3)->getGeomAssociation();
                    Geom::Curve* crv = dynamic_cast<Geom::Curve*>(ge);

                    // on laisse les relation vers une entité qui n'est pas détruite (cas sur l'axe par ex)
                    if (ge && ge->isDestroyed()){
                        if (crv)
                            (*iter3)->setGeomAssociation(C2S[crv]);
                        else {
                            Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
                            if (surf)
                                (*iter3)->setGeomAssociation(0);
                        }
                    }
                }

            } // end if (filtre_coedge[*iter3] == 0)
        } // end for coedges
    } // end for cofaces

}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::
updateCoEdgesInterpolateNames(CoEdge* coedge, std::map<std::string, CoEdge*>& corr_coedge,
		std::string oldCofaceName, std::string newCofaceName)
{
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"CommandEditTopo::updateCoEdgesInterpolateNames pour "<<coedge->getName()<<std::endl;
#endif
	if (coedge->getMeshingProperty()->getMeshLaw() != CoEdgeMeshingProperty::interpolate)
		return;
	EdgeMeshingPropertyInterpolate* empi = dynamic_cast<EdgeMeshingPropertyInterpolate*>(coedge->getMeshingProperty());
	if (empi == 0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandEditTopo::updateCoEdgesInterpolateNames pb avec dynamic_cast", TkUtil::Charset::UTF_8));

	if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list){
		std::vector<std::string> coedgesNames = empi->getCoEdges();
		std::vector<std::string> newCoedgesNames;
		for (uint i=0; i<coedgesNames.size(); i++){
#ifdef _DEBUG_INTERPOLATE
			std::cout<<" coedgesNames["<<i<<"] : "<<coedgesNames[i]<<std::endl;
#endif
			CoEdge* new_coedge = corr_coedge[coedgesNames[i]];
			if (new_coedge){
				newCoedgesNames.push_back(new_coedge->getName());
#ifdef _DEBUG_INTERPOLATE
				std::cout<<" à remplacer : "<<coedgesNames[i]<<" -> "<<new_coedge->getName()<<std::endl;
#endif
			}
		}
		if (newCoedgesNames.size())
			empi->setCoEdges(newCoedgesNames);
	}
	else if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coface
			&& oldCofaceName == empi->getCoFace()) {
#ifdef _DEBUG_INTERPOLATE
			std::cout<<" oldCofaceName : "<<oldCofaceName<<" => newCofaceName : "<<newCofaceName<<std::endl;
#endif
		empi->setCoFace(newCofaceName);
	}
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::mergeCoEdges(Vertex* vtx, std::map<CoEdge*,uint>& keep_coedges)
{
	std::map<Vertex*, CoEdge*> filtre_vertices;

	std::vector<CoEdge*> coedges;
	vtx->getCoEdges(coedges);
	for (uint i=0; i<coedges.size(); i++){
		CoEdge* coedge = coedges[i];
		Vertex* opp_vtx = coedge->getOppositeVertex(vtx);
		CoEdge* other_coedge = filtre_vertices[opp_vtx];
		if (other_coedge){
			if (keep_coedges[other_coedge]){
				other_coedge->merge(coedge, &getInfoCommand());
#ifdef _DEBUG
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message <<"Fusion des arêtes "<<other_coedge->getName()<<" (conservée) et "<<coedge->getName();
				getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
#endif
			}
			else {
				coedge->merge(other_coedge, &getInfoCommand());
#ifdef _DEBUG
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message <<"Fusion des arêtes "<<coedge->getName()<<" (conservée) et "<<other_coedge->getName();
				getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
#endif
			}
		}
		else {
			filtre_vertices[opp_vtx] = coedge;
		}
	} // end for i
}
/*----------------------------------------------------------------------------*/
void CommandEditTopo::mergeCoFaces(Vertex* vtx, std::map<CoFace*,uint>& keep_cofaces)
{
#ifdef _DEBUG_MERGE
	std::cout<<"CommandEditTopo::mergeCoFaces("<<vtx->getName()<<")"<<std::endl;
#endif

	std::vector<CoFace*> cofaces;
	vtx->getCoFaces(cofaces);
	for (uint i=0; i<cofaces.size(); i++){
		CoFace* coface = cofaces[i];

		if (coface->getNbBlocks() == 1){

			std::map<CoFace*, uint> filtre_cofaces;
#ifdef _DEBUG_MERGE
	std::cout<<"observation de "<<coface->getName()<<std::endl;
#endif

			// pour chacun des sommets, marque les cofaces adjacentes
			std::vector<Vertex*> vertices;
			coface->getVertices(vertices);

			for (uint j=0; j<vertices.size(); j++){
#ifdef _DEBUG_MERGE
	std::cout<<" sommet "<<vertices[j]->getName()<<std::endl;
#endif

				std::vector<CoFace*> loc_cofaces;
				vertices[j]->getCoFaces(loc_cofaces);

				for (uint k=0; k<loc_cofaces.size(); k++){
					filtre_cofaces[loc_cofaces[k]] += 1;
#ifdef _DEBUG_MERGE
	std::cout<<"   coface "<<loc_cofaces[k]->getName()<<" marquée à "<<filtre_cofaces[loc_cofaces[k]]<<std::endl;
#endif
				}

			} // end for j

			// recherche la liste des cofaces ayant été vues autant de fois que de sommets
			std::list<CoFace*> selected_cofaces;
			for (std::map<CoFace*, uint>::const_iterator iter = filtre_cofaces.begin();
					iter != filtre_cofaces.end(); ++iter)
				if (iter->second == vertices.size()){
					selected_cofaces.push_back(iter->first);
#ifdef _DEBUG_MERGE
	std::cout<<" selected_cofaces "<<iter->first->getName()<<std::endl;
#endif
				}
			if (selected_cofaces.size() == 2){
				selected_cofaces.sort(Utils::Entity::compareEntity);
				std::list<CoFace*>::iterator iter = selected_cofaces.begin();
				CoFace* coface1 = *iter;
				iter++;
				CoFace* coface2 = *iter;

				// on vérifie que les sommets communs sont aux extrémités des faces et pas internes à un côté
				std::map<Vertex*, uint> filtre_vertices;
				coface1->getVertices(vertices);
				for (uint j=0; j<vertices.size(); j++)
					filtre_vertices[vertices[j]] = 1;
				uint nb_communs = 0;
				coface2->getVertices(vertices);
				for (uint j=0; j<vertices.size(); j++)
					nb_communs += filtre_vertices[vertices[j]];
#ifdef _DEBUG_MERGE
	std::cout<<" nb_communs "<<nb_communs<<std::endl;
#endif
				if (nb_communs == vertices.size()){
					if (keep_cofaces[coface2]){
						coface2->merge(coface1, &getInfoCommand());
#ifdef _DEBUG
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message <<"Fusion des faces "<<coface2->getName()<<" (conservée) et "<<coface1->getName();
						getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
#endif
					}
					else {
						coface1->merge(coface2, &getInfoCommand());
#ifdef _DEBUG
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message <<"Fusion des faces "<<coface1->getName()<<" (conservée) et "<<coface2->getName();
						getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
#endif
					}
				}
			}

		} // end if (coface->getNbBlocks() == 1)
	} // end for i
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
