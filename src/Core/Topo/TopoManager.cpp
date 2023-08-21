/*----------------------------------------------------------------------------*/
/** \file TopoManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/12/2010
 *
 *
 *  Modified on: 23/02/2022
 *      Author: Simon C
 *      ajout de la fonction de création de blocs par extrusion de faces topologiques
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Utils/CommandManager.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Point.h"
#include "Utils/Rotation.h"
#include "Utils/TypeDedicatedNameManager.h"
#include "Utils/MgxException.h"

#include "Topo/TopoManager.h"
#include "Topo/TopoHelper.h"

#include "Topo/CommandNewTopoOnGeometry.h"
#include "Topo/CommandFuse2Vertices.h"
#include "Topo/CommandGlue2Blocks.h"
#include "Topo/CommandGlue2Topo.h"
#include "Topo/CommandNewTopoOGridOnGeometry.h"
#include "Topo/CommandSetEdgeMeshingProperty.h"
#include "Topo/CommandSetEdgeMeshingPropertyToParallelCoEdges.h"
#include "Topo/CommandSetFaceMeshingProperty.h"
#include "Topo/CommandSetBlockMeshingProperty.h"
#include "Topo/CommandSplitBlock.h"
#include "Topo/CommandSplitBlocks.h"
#include "Topo/CommandSplitBlocksWithOgrid.h"
#include "Topo/CommandSplitFacesWithOgrid.h"
#include "Topo/CommandExtendSplitBlock.h"
#include "Topo/CommandSplitFaces.h"
#include "Topo/CommandExtendSplitFace.h"
#include "Topo/CommandUnrefineBlock.h"
#include "Topo/CommandRefineAllCoEdges.h"
#include "Topo/CommandImportTopoMDL.h"
#include "Topo/CommandSetGeomAssociation.h"
#include "Topo/CommandMakeBlocksByRevol.h"
#include "Topo/CommandSetGeomAssociation.h"
#include "Topo/CommandProjectVerticesOnNearestGeomEntities.h"
#include "Topo/CommandProjectEdgesOnCurves.h"
#include "Topo/CommandProjectFacesOnSurfaces.h"
#include "Topo/CommandSnapVertices.h"
#include "Topo/CommandTranslateTopo.h"
#include "Topo/CommandRotateTopo.h"
#include "Topo/CommandScaleTopo.h"
#include "Topo/CommandMirrorTopo.h"
#include "Topo/CommandChangeVerticesLocation.h"
#include "Topo/CommandChangeVertexSameLocation.h"
#include "Topo/CommandSnapProjectedVertices.h"
#include "Topo/CommandSplitEdge.h"
#include "Topo/CommandChangeDefaultNbMeshingEdges.h"
#include "Topo/CommandSetNbMeshingEdges.h"
#include "Topo/CommandAlignVertices.h"
#include "Topo/CommandModificationTopo.h"
#include "Topo/CommandDestroyTopo.h"
#include "Topo/CommandReverseDirection.h"
#include "Topo/CommandDuplicateTopo.h"
#include "Topo/CommandReplaceTransfiniteMethod.h"
#include "Topo/CommandInsertHole.h"
#include <Topo/CommandFuseCoEdges.h>
#include <Topo/CommandFuse2Blocks.h>
#include "Topo/CommandExtrudeTopo.h"
#include "Topo/CommandExtrudeFace.h"

#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"

#include "Geom/GeomManager.h"
#include "Geom/GeomEntity.h"
#include "Geom/CommandNewBox.h"
#include "Geom/CommandNewCylinder.h"
#include "Geom/CommandNewCone.h"
#include "Geom/CommandNewHollowCylinder.h"
#include "Geom/CommandNewSphere.h"
#include "Geom/CommandNewHollowSphere.h"
#include "Geom/CommandNewSpherePart.h"
#include "Geom/CommandNewHollowSpherePart.h"
#include "Geom/CommandImportMDL.h"
#include "Geom/CommandGluing.h"
#include "Geom/CommandJoinCurves.h"
#include "Geom/CommandExtrudeRevolution.h"
#include "Geom/CommandRotation.h"
#include "Geom/CommandTranslation.h"
#include "Geom/CommandScaling.h"
#include "Geom/CommandMirroring.h"

#include "Mesh/CommandAddRemoveGroupName.h"

#include "Internal/CommandComposite.h"
#include "Internal/ImportMDLImplementation.h"
#include "Internal/Context.h"
#include "Internal/NameManager.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandChangeLengthUnit.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <Topo/CommandFuse2Edges.h>
#include <Topo/CommandFuse2EdgeList.h>
#include <Topo/CommandFuse2Faces.h>
#include <Topo/CommandFuse2FaceList.h>

#include <TkUtil/TraceLog.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
TopoManager::TopoManager(const std::string& name, Internal::ContextIfc* c)
:Topo::TopoManagerIfc (name, c)
, m_defaultNbMeshingEdges(10)
{
#ifdef _DEBUG_TIMER
	_cpuDuration = 0;
#endif
}
/*----------------------------------------------------------------------------*/
TopoManager::~TopoManager()
{
//#define _DEBUG_MEMORYedges
#ifdef _DEBUG_MEMORY
    std::cout<<"TopoManager::~TopoManager()"<<std::endl;
    std::cout<<" m_faces :";
    for (uint i=0;i<m_faces.getNb(); i++)
        std::cout<<" "<<m_faces.get(i)->getName();
    std::cout<<std::endl;
#endif

    clear();

#ifdef _DEBUG_TIMER
    std::cout<<"Temps cpu pour recherche des noms dans TopoManager : "
    		<<_cpuDuration/TkUtil::Timer::cpuPerSecond()
            <<" ("<<_cpuDuration<<" cycles cpu)"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void TopoManager::clear()
{
    m_blocks.deleteAndClear();
    m_faces.deleteAndClear();
    m_cofaces.deleteAndClear();
    m_edges.deleteAndClear();
    m_coedges.deleteAndClear();
    m_vertices.deleteAndClear();
    m_defaultNbMeshingEdges = 10;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getBlocks() const
{
    std::vector<std::string> entities_names;

    const std::vector<Block* >& entities_all = m_blocks.get();
    for(auto e: entities_all) {
        if(!e->isDestroyed()) {
            entities_names.push_back(e->getName());
        }
    }
    std::sort(entities_names.begin(), entities_names.end());

    return entities_names;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getCoFaces() const
{
    std::vector<std::string> entities_names;

    const std::vector<CoFace* >& entities_all = m_cofaces.get();
    for(auto e: entities_all) {
        if(!e->isDestroyed()) {
            entities_names.push_back(e->getName());
        }
    }
    std::sort(entities_names.begin(), entities_names.end());

    return entities_names;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getCoEdges() const
{
    std::vector<std::string> entities_names;

    const std::vector<CoEdge* >& entities_all = m_coedges.get();
    for(auto e: entities_all) {
        if(!e->isDestroyed()) {
            entities_names.push_back(e->getName());
        }
    }
    std::sort(entities_names.begin(), entities_names.end());

    return entities_names;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string>  TopoManager::getVertices() const
{
    std::vector<std::string> entities_names;

    const std::vector<Vertex* >& entities_all = m_vertices.get();
    for(auto e: entities_all) {
        if(!e->isDestroyed()) {
            entities_names.push_back(e->getName());
        }
    }
    std::sort(entities_names.begin(), entities_names.end());

    return entities_names;
}
/*----------------------------------------------------------------------------*/
void TopoManager::getBlocks(std::vector<Block* > &blocks, bool sort) const
{
    blocks.clear();

    if (sort){
        std::list<Topo::Block*> l_te;
        const std::vector<Block* >& blks = m_blocks.get();
        for (std::vector<Block* >::const_iterator iter = blks.begin();
                iter != blks.end(); ++iter)
            if (!(*iter)->isDestroyed())
                l_te.push_back(*iter);
        l_te.sort(Utils::Entity::compareEntity);
        l_te.unique();
        blocks.insert(blocks.end(), l_te.begin(), l_te.end());
    }
    else {
        const std::vector<Block* >& blks = m_blocks.get();
        for (std::vector<Block* >::const_iterator iter = blks.begin();
                iter != blks.end(); ++iter)
            if (!(*iter)->isDestroyed())
                blocks.push_back(*iter);
    }
}
/*----------------------------------------------------------------------------*/
void TopoManager::add(Block* b)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::add("<<b->getName()<<")"<<std::endl;
#endif

    m_blocks.add(b);
}
/*----------------------------------------------------------------------------*/
void TopoManager::remove(Block* b)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::remove("<<b->getName()<<")"<<std::endl;
#endif

    m_blocks.remove(b, true);

}
/*----------------------------------------------------------------------------*/
void TopoManager::add(Face* f)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::add("<<f->getName()<<")"<<std::endl;
#endif

    m_faces.add(f);
}
/*----------------------------------------------------------------------------*/
void TopoManager::remove(Face* f)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::remove("<<f->getName()<<")"<<std::endl;
#endif

    m_faces.remove(f, true);
}
/*----------------------------------------------------------------------------*/
void TopoManager::add(CoFace* f)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::add("<<f->getName()<<")"<<std::endl;
#endif

    m_cofaces.add(f);
}
/*----------------------------------------------------------------------------*/
void TopoManager::remove(CoFace* f)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::remove("<<f->getName()<<")"<<std::endl;
#endif

    m_cofaces.remove(f, true);
}
/*----------------------------------------------------------------------------*/
void TopoManager::add(Edge* ce)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::add("<<ce->getName()<<")"<<std::endl;
#endif

    m_edges.add(ce);
}
/*----------------------------------------------------------------------------*/
void TopoManager::remove(Edge* ce)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::remove("<<ce->getName()<<")"<<std::endl;
#endif

    m_edges.remove(ce, true);
}
/*----------------------------------------------------------------------------*/
void TopoManager::add(CoEdge* ce)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::add("<<ce->getName()<<")"<<std::endl;
#endif

    m_coedges.add(ce);
}
/*----------------------------------------------------------------------------*/
void TopoManager::remove(CoEdge* ce)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::remove("<<ce->getName()<<")"<<std::endl;
#endif

    m_coedges.remove(ce, true);
}
/*----------------------------------------------------------------------------*/
void TopoManager::add(Vertex* v)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::add("<<v->getName()<<")"<<std::endl;
#endif

    m_vertices.add(v);
}
/*----------------------------------------------------------------------------*/
void TopoManager::remove(Vertex* v)
{
#ifdef _DEBUG2
    std::cout<<"TopoManager::remove("<<v->getName()<<")"<<std::endl;
#endif

    m_vertices.remove(v, true);
}
/*----------------------------------------------------------------------------*/
Block* TopoManager::getBlock (const std::string& name, const bool exceptionIfNotFound) const
{
#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(true);
#endif
    Block* bloc = 0;

    if (!Block::isA(name)){
    	if (exceptionIfNotFound){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message <<"getBlock impossible, entité \""<<name<<"\" n'a pas un nom de bloc";
    		throw TkUtil::Exception(message);
    	}
    	else
    		return bloc;
    }

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::TopoBlock)->renameWithShiftingId(name);
    else
        new_name = name;

    const std::vector<Block* >& blocks = m_blocks.get();
    for (std::vector<Block* >::const_iterator iter = blocks.begin();
    		iter != blocks.end(); ++iter)
    	if (new_name == (*iter)->getName())
    		bloc = (*iter);

#ifdef _DEBUG_TIMER
	timer.stop();
	_cpuDuration += timer.cpuDuration();
#endif
    if (bloc != 0 && bloc->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getBlock trouve l'entité \""<<new_name<<"\", mais elle est détruite";
        throw Utils::IsDestroyedException(message);
    }

    if (exceptionIfNotFound && bloc == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getBlock impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return bloc;
}
/*----------------------------------------------------------------------------*/
std::string TopoManager::getLastBlock()
{
    const std::vector<Block* >& blocks = m_blocks.get();
#ifdef _DEBUG
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "TopoManager::getLastBlock ()\n";
    message << "m_blocks de taille:" << blocks.size() << "\n";
   for (uint i=0; i<m_blocks.getNb(); i++)
        message << " bloc "<<blocks[i]->getName()<<(blocks[i]->isDestroyed()?" détruit":" non détruit")<<"\n";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
#endif

    std::string nom("");
    if (m_blocks.empty())
        return nom;
    // recherche du dernier non détruit, en partant de la fin
    int i;
    for (i=blocks.size()-1; i>=0 && blocks[i]->isDestroyed(); i--)
    {}
    if (i>=0)
        nom = blocks[i]->getName();
    return nom;
}
/*----------------------------------------------------------------------------*/
int TopoManager::getNbBlocks() const
{
	const std::vector<Block* >& blocs = m_blocks.get();
	int nb = 0;
	for (uint i=0; i<blocs.size(); i++)
		if (!blocs[i]->isDestroyed())
			nb++;
	return nb;
}
/*----------------------------------------------------------------------------*/
int TopoManager::getNbFaces() const
{
	const std::vector<CoFace* >& cofaces = m_cofaces.get();
	int nb = 0;
	for (uint i=0; i<cofaces.size(); i++)
		if (!cofaces[i]->isDestroyed())
			nb++;
	return nb;
}
/*----------------------------------------------------------------------------*/
int TopoManager::getNbEdges() const
{
	const std::vector<CoEdge* >& coedges = m_coedges.get();
	int nb = 0;
	for (uint i=0; i<coedges.size(); i++)
		if (!coedges[i]->isDestroyed())
			nb++;
	return nb;
}
/*----------------------------------------------------------------------------*/
void TopoManager::getCoFaces(std::vector<Topo::CoFace* >& faces) const
{
    faces.clear();

    const std::vector<CoFace* >& cofaces = m_cofaces.get();

    std::list<Topo::CoFace*> l_te;
    for (std::vector<CoFace* >::const_iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter)
        if (!(*iter)->isDestroyed())
            l_te.push_back(*iter);
    l_te.sort(Utils::Entity::compareEntity);
    l_te.unique();
    faces.insert(faces.end(), l_te.begin(), l_te.end());
}
/*----------------------------------------------------------------------------*/
CoFace* TopoManager::getCoFace(const std::string& name, const bool exceptionIfNotFound) const
{
#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(true);
#endif
    CoFace* face = 0;

    if (!CoFace::isA(name)){
    	if (exceptionIfNotFound){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message <<"getCoEdge impossible, entité \""<<name<<"\" n'a pas un nom de face commune";
    		throw TkUtil::Exception(message);
    	}
    	else
    		return face;
    }

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::TopoCoFace)->renameWithShiftingId(name);
    else
        new_name = name;

    const std::vector<CoFace* >& cofaces = m_cofaces.get();
    for (std::vector<CoFace* >::const_iterator iter3 = cofaces.begin();
    		iter3 != cofaces.end(); ++iter3)
    	if (new_name == (*iter3)->getName())
    		face = (*iter3);

    #ifdef _DEBUG_TIMER
	timer.stop();
	_cpuDuration += timer.cpuDuration();
#endif

    if (face != 0 && face->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getCoFace trouve l'entité \""<<new_name<<"\", mais elle est détruite";
        throw Utils::IsDestroyedException(message);
    }

    if (exceptionIfNotFound && face == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getCoFace impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return face;
}
/*----------------------------------------------------------------------------*/
Face* TopoManager::getFace(const std::string& name, const bool exceptionIfNotFound) const
{
#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(true);
#endif
    Face* face = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::TopoFace)->renameWithShiftingId(name);
    else
        new_name = name;

    if (Face::isA(new_name)){
        const std::vector<Face* >& faces = m_faces.get();
        for (std::vector<Face* >::const_iterator iter2 = faces.begin();
                iter2 != faces.end() && face == 0; ++iter2){
            if (new_name == (*iter2)->getName())
                face = (*iter2);
        }
    }
    else if (exceptionIfNotFound){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getFace impossible, entité \""<<new_name<<"\" n'a pas un nom de face";
        throw TkUtil::Exception(message);
    }
#ifdef _DEBUG_TIMER
	timer.stop();
	_cpuDuration += timer.cpuDuration();
#endif

    if (face != 0 && face->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getFace trouve l'entité \""<<new_name<<"\", mais elle est détruite";
        throw Utils::IsDestroyedException(message);
        }

    if (exceptionIfNotFound && face == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getFace impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return face;
}
/*----------------------------------------------------------------------------*/
void TopoManager::getCoEdges(std::vector<Topo::CoEdge* >& edges) const
{
    edges.clear();

    const std::vector<CoEdge* >& coedges = m_coedges.get();

    std::list<Topo::CoEdge*> l_te;
    for (std::vector<CoEdge* >::const_iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        if (!(*iter)->isDestroyed())
            l_te.push_back(*iter);
    l_te.sort(Utils::Entity::compareEntity);
    l_te.unique();
    edges.insert(edges.end(), l_te.begin(), l_te.end());
}
/*----------------------------------------------------------------------------*/
CoEdge* TopoManager::getCoEdge(const std::string& name, const bool exceptionIfNotFound) const
{
#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(true);
#endif
    CoEdge* edge = 0;

    if (!CoEdge::isA(name)){
    	if (exceptionIfNotFound){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message <<"getCoEdge impossible, entité \""<<name<<"\" n'a pas un nom d'arête commune";
    		throw TkUtil::Exception(message);
    	}
    	else
    		return edge;
    }

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::TopoCoEdge)->renameWithShiftingId(name);
    else
        new_name = name;

    //std::cout<<"TopoManager::getCoEdge("<<name<<") recherche de "<<new_name<<std::endl;

    const std::vector<CoEdge* >& coedges = m_coedges.get();
    for (std::vector<CoEdge* >::const_iterator iter4 = coedges.begin();
    		iter4 != coedges.end(); ++iter4)

    	if (new_name == (*iter4)->getName())
    		edge = (*iter4);

#ifdef _DEBUG_TIMER
	timer.stop();
	_cpuDuration += timer.cpuDuration();
#endif

    if (edge != 0 && edge->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getCoEdge trouve l'entité \""<<new_name<<"\", mais elle est détruite";
        throw Utils::IsDestroyedException(message);
    }

    if (exceptionIfNotFound && edge == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getCoEdge impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return edge;
}
/*----------------------------------------------------------------------------*/
Edge* TopoManager::getEdge(const std::string& name, const bool exceptionIfNotFound) const
{
#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(true);
#endif
   Edge* edge = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::TopoEdge)->renameWithShiftingId(name);
    else
        new_name = name;

    if (Edge::isA(new_name)) {
        const std::vector<Edge* >& edges = m_edges.get();
        for (std::vector<Edge* >::const_iterator iter4 = edges.begin();
                iter4 != edges.end(); ++iter4)

            if (new_name == (*iter4)->getName())
                edge = (*iter4);
    }
    else if (exceptionIfNotFound){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getEdge impossible, entité \""<<new_name<<"\" n'a pas un nom d'arête";
        throw TkUtil::Exception(message);
    }

#ifdef _DEBUG_TIMER
	timer.stop();
	_cpuDuration += timer.cpuDuration();
#endif

    if (edge != 0 && edge->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getEdge trouve l'entité \""<<new_name<<"\", mais elle est détruite";
        throw Utils::IsDestroyedException(message);
    }

    if (exceptionIfNotFound && edge == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getEdge impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return edge;
}
/*----------------------------------------------------------------------------*/
void TopoManager::getVertices(std::vector<Topo::Vertex* >& vertices) const
{
    vertices.clear();

    const std::vector<Vertex* >& vtx = m_vertices.get();

    std::list<Topo::Vertex*> l_te;
    for (std::vector<Vertex* >::const_iterator iter = vtx.begin();
            iter != vtx.end(); ++iter)
        if (!(*iter)->isDestroyed())
            l_te.push_back(*iter);
    l_te.sort(Utils::Entity::compareEntity);
    l_te.unique();
    vertices.insert(vertices.end(), l_te.begin(), l_te.end());
}
/*----------------------------------------------------------------------------*/
Vertex* TopoManager::getVertex(const std::string& name, const bool exceptionIfNotFound) const
{
#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(true);
#endif
    Vertex* vertex = 0;

    if (!Vertex::isA(name)){
    	if (exceptionIfNotFound){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message <<"getVertex impossible, entité \""<<name<<"\" n'a pas un nom de sommet";
    		throw TkUtil::Exception(message);
    	}
    	else
    		return vertex;
    }

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated())
        new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::TopoVertex)->renameWithShiftingId(name);
    else
    	new_name = name;

    const std::vector<Vertex* >& vertices = m_vertices.get();
    for (std::vector<Vertex* >::const_iterator iter2 = vertices.begin();
    		iter2 != vertices.end(); ++iter2)

    	if (new_name == (*iter2)->getName())
    		vertex = (*iter2);

#ifdef _DEBUG_TIMER
	timer.stop();
	_cpuDuration += timer.cpuDuration();
#endif

    if (vertex != 0 && vertex->isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getVertex trouve l'entité \""<<new_name<<"\", mais elle est détruite";
        throw Utils::IsDestroyedException(message);
    }

    if (exceptionIfNotFound && vertex == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getVertex impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

    return vertex;
}
/*----------------------------------------------------------------------------*/
TopoEntity* TopoManager::getEntity(const std::string& name, const bool exceptionIfNotFound) const
{
	TopoEntity*	entity	= getVertex (name, false);
	if (0 == entity)
		entity	= getCoEdge (name, false);
	if (0 == entity)
		entity	= getCoFace (name, false);
	if (0 == entity)
		entity	= getBlock (name, false);

	if ((0 == entity) && (true == exceptionIfNotFound)){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"getEntity impossible, entité \""<<name<<"\" n'a pas été trouvée dans le TopoManager";
        throw TkUtil::Exception(message);
    }

return entity;
}
/*----------------------------------------------------------------------------*/
std::string TopoManager::getVertexAt(const Point& pt1) const
{
	// il pourrait y en avoir aucun ou plusieurs, on n'en veut qu'un
	std::vector<Vertex*> selected;

	const std::vector<Vertex* >& all = m_vertices.get();
	for (std::vector<Vertex*>::const_iterator iter = all.begin();
			iter != all.end(); ++iter)
		if ((*iter)->getCoord() == pt1)
			selected.push_back(*iter);

	if (selected.size() == 1)
		return selected[0]->getName();
	else {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"getVertexAt impossible, on trouve "<<selected.size()<<" sommets à la position "<<pt1;
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
std::string TopoManager::getEdgeAt(const Point& pt1, const Point& pt2) const
{
	// il pourrait y en avoir aucune ou plusieurs, on n'en veut qu'un
	std::vector<CoEdge*> selected;

	const std::vector<CoEdge* >& all = m_coedges.get();
	for (std::vector<CoEdge*>::const_iterator iter = all.begin();
			iter != all.end(); ++iter)
		if ((*iter)->getVertex(0)->getCoord() == pt1 && (*iter)->getVertex(1)->getCoord() == pt2)
			selected.push_back(*iter);

	if (selected.size() == 1)
		return selected[0]->getName();
	else {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"getEdgeAt impossible, on trouve "<<selected.size()<<" arêtes au couple de positions "
				<<pt1<<", "<<pt2;
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
std::string TopoManager::getFaceAt(std::vector<Point>& pts) const
{
#ifdef _DEBUG2
	std::cout<<"getFaceAt avec les pts";
	for (uint i=0; i<pts.size(); i++)
		std::cout<<" "<<pts[i];
	std::cout<<std::endl;
#endif
	// il pourrait y en avoir aucune ou plusieurs, on n'en veut qu'une
	std::vector<CoFace*> cofaces;

	const std::vector<CoFace* >& all = m_cofaces.get();
	for (std::vector<CoFace*>::const_iterator iter = all.begin();
	            iter != all.end(); ++iter){
		std::vector<Topo::Vertex*> vertices;
		(*iter)->getAllVertices(vertices);
		uint i;
		if (vertices.size() != pts.size())
			continue;
		for (i=0; i<vertices.size() && vertices[i]->getCoord() == pts[i]; i++){
		}
#ifdef _DEBUG2
		std::cout<<"Pour "<<(*iter)->getName()<<" vertices.size() = "<<vertices.size()<<" et i = "<<i<<std::endl;
#endif

		if (i == vertices.size())
			cofaces.push_back(*iter);
	} // end for iter = m_cofaces.begin()

	if (cofaces.size() == 1)
		return cofaces[0]->getName();
	else {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"getFaceAt impossible, on trouve "<<cofaces.size()
				<<" faces avec ces "
				<<pts.size()<<" positions";
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
std::string TopoManager::getBlockAt(std::vector<Point>& pts) const
{
	// il pourrait y en avoir aucune ou plusieurs, on n'en veut qu'une
	std::vector<Block*> blocks;

	const std::vector<Block* >& all = m_blocks.get();
	for (std::vector<Block*>::const_iterator iter = all.begin();
	            iter != all.end(); ++iter){
		std::vector<Topo::Vertex*> vertices;
		(*iter)->getAllVertices(vertices);
		uint i;
		if (vertices.size() != pts.size())
			continue;
		for (i=0; i<vertices.size() && vertices[i]->getCoord() == pts[i]; i++){
		}

		if (i == vertices.size())
			blocks.push_back(*iter);
	} // end for iter = m_blocks.begin()

	if (blocks.size() == 1)
		return blocks[0]->getName();
	else {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message <<"getFaceAt impossible, on trouve "<<blocks.size()
				<<" blocs avec ces "
				<<pts.size()<<" positions";
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newTopoOnGeometry(std::string ne)
{
	return newTopoOnGeometry(getLocalContext().getGeomManager().getEntity(ne));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newTopoOnGeometry(Geom::GeomEntity* ge)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newTopoOnGeometry("<<ge->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), ge,
    		CommandNewTopoOnGeometry::ASSOCIATED_TOPO);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newTopoOnGeometry (\""<<ge->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);

    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newStructuredTopoOnGeometry(std::string ne)
{
    if (!ne.empty())
        return newStructuredTopoOnGeometry(getLocalContext().getGeomManager().getEntity(ne));
    else
        return newStructuredTopoOnGeometry(0);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newStructuredTopoOnGeometry(Geom::GeomEntity* ge)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newStructuredTopoOnGeometry("<<(ge?ge->getName():"\"\"")<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), ge,
    		CommandNewTopoOnGeometry::STRUCTURED_BLOCK);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newStructuredTopoOnGeometry (\""<<(ge?ge->getName():"")<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);

    return cmdResult;
}
/*----------------------------------------------------------------------------*/
 Mgx3D::Internal::M3DCommandResultIfc*
 TopoManager::newStructuredTopoOnSurface(std::string ne, std::vector<std::string>& ve)
 {
	 Geom::Surface* surf = getLocalContext().getGeomManager().getSurface(ne, true);
	 std::vector<Geom::Vertex*> vertices;
	 for (uint i=0; i<ve.size(); i++)
		 vertices.push_back(getLocalContext().getGeomManager().getVertex(ve[i], true));

	 return newStructuredTopoOnSurface(surf, vertices);
 }
 /*----------------------------------------------------------------------------*/
 Mgx3D::Internal::M3DCommandResultIfc*
 TopoManager::newStructuredTopoOnSurface(Geom::Surface* surf, std::vector<Geom::Vertex*>& vertices)
 {
	 Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), surf, vertices);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
	 cmd << getContextAlias() << "." << "getTopoManager().newStructuredTopoOnSurface (\""<<surf->getName()
			 <<"\", [";
	 for(unsigned int i=0;i<vertices.size();i++){
		 if(i!=0)
			 cmd << ", ";
		 cmd << "\""<< vertices[i]->getName()<<"\"";
	 }
	 cmd << "])";

	 command->setScriptCommand(cmd);

	 getCommandManager().addCommand(command, Utils::Command::DO);

	 Internal::M3DCommandResultIfc*  cmdResult   =
			 new Internal::M3DCommandResult (*command);

	 return cmdResult;
 }
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newUnstructuredTopoOnGeometry(std::string ne)
{
    return newUnstructuredTopoOnGeometry(getLocalContext().getGeomManager().getEntity(ne));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newUnstructuredTopoOnGeometry(Geom::GeomEntity* ge)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newUnstructuredTopoOnGeometry("<<ge->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), ge,
    		CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newUnstructuredTopoOnGeometry (\""
            <<ge->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newFreeTopoOnGeometry(std::string ne)
{
    if (!ne.empty())
        return newFreeTopoOnGeometry(getLocalContext().getGeomManager().getEntity(ne));
    else
        return newFreeTopoOnGeometry(0);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newFreeTopoOnGeometry(Geom::GeomEntity* ge)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newFreeTopoOnGeometry("<<(ge?ge->getName():"\"\"")<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), ge,
    		CommandNewTopoOnGeometry::FREE_BLOCK);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newFreeTopoOnGeometry (\""
            <<(ge?ge->getName():"")<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newFreeTopoInGroup(std::string ng, int dim)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newFreeTopoInGroup("<<ng<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), ng, dim);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newFreeTopoInGroup (\""
            <<ng<<"\", "<< (short)dim <<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newInsertionTopoOnGeometry(std::string ne)
{
    return newInsertionTopoOnGeometry(getLocalContext().getGeomManager().getEntity(ne));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::newInsertionTopoOnGeometry(Geom::GeomEntity* ge)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newInsertionTopoOnGeometry("<<ge->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOnGeometry* command = new Topo::CommandNewTopoOnGeometry(getLocalContext(), ge,
    		CommandNewTopoOnGeometry::INSERTION_BLOCK);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newInsertionTopoOnGeometry (\""
            <<ge->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newBoxWithTopo(const Utils::Math::Point& pmin, const Utils::Math::Point& pmax,
		bool meshStructured, std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newBoxWithTopo ("<<pmin<<", "<<pmax;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande de création de la géométrie
    Geom::CommandNewBox *commandGeom = new Geom::CommandNewBox(getLocalContext(),pmin,pmax,groupName);

    // creation de la commande de création de la topologie
    // avec un volume à récupérer dans la commande
    // true <=> structuration demandée
    Topo::CommandNewTopoOnGeometry* commandTopo =
    		new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom,
    				meshStructured?CommandNewTopoOnGeometry::STRUCTURED_BLOCK
    						:CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Création d'une boite avec une topologie");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newBoxWithTopo ("
            <<pmin.getScriptCommand()<<", "
            <<pmax.getScriptCommand()<<", "
            <<(meshStructured?"True":"False");
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newBoxWithTopo(const Utils::Math::Point& pmin, const Utils::Math::Point& pmax,
		const int ni, const int nj, const int nk,
		std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newBoxWithTopo ("<<pmin<<", "<<pmax<<", "<<(short)ni<<", "<<(short)nj<<", "<<(short)nk;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande de création de la géométrie
    Geom::CommandNewBox *commandGeom = new Geom::CommandNewBox(getLocalContext(),pmin,pmax,groupName);

    // creation de la commande de création de la topologie
    // avec un volume à récupérer dans la commande
    // true <=> structuration demandée
    Topo::CommandNewTopoOnGeometry* commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, ni, nj, nk);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Création d'une boite avec une topologie");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newBoxWithTopo ("
            <<pmin.getScriptCommand()<<", "
            <<pmax.getScriptCommand()<<", "
            <<(short)ni<<", "<<(short)nj<<", "<<(short)nk;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newCylinderWithTopo(const Point& pcentre, const double& dr,
        const Vector& dv, const double& da,
        bool meshStructured, const double& rat,
        const int naxe, const int ni, const int nr,
        std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
     message << "TopoManager::newCylinderWithTopo("<<pcentre<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (dr)<<", "<<dv<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (da)<<", "
             <<(meshStructured?"True":"False")<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (rat)<<", "
	         <<(long)naxe<<", "<<(long)ni<<", "<<(long)nr;
     if (!groupName.empty())
         message<<", "<<groupName;
     message<<")";
     log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

     // creation de la commande de création de la géométrie
     Geom::CommandNewCylinder *commandGeom = new Geom::CommandNewCylinder(getLocalContext(),pcentre, dr, dv, da, groupName);

     // creation de la commande de création de la topologie
     // avec un volume à récupérer dans la commande
     // true <=> structuration demandée
     Topo::CommandCreateTopo* commandTopo=0;

     if (meshStructured) {
         CommandNewTopoOGridOnGeometry* commandTopoOGrid = new Topo::CommandNewTopoOGridOnGeometry(getLocalContext(), commandGeom, rat);
         commandTopoOGrid->setNaxe(naxe);
         commandTopoOGrid->setNi(ni);
         commandTopoOGrid->setNr(nr);
         commandTopo = commandTopoOGrid;
     }
     else
         commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

     // creation de la commande composite
     Internal::CommandComposite *command =
             new Internal::CommandComposite(getLocalContext(), "Création d'un cylindre avec une topologie");
     command->addCommand(commandGeom);
     command->addCommand(commandTopo);

     // trace dans le script
     TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
     cmd << getContextAlias() << "." << "getTopoManager().newCylinderWithTopo ("
             <<pcentre.getScriptCommand()<<", "
             << Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
             <<dv.getScriptCommand()<<", "
             <<Utils::Math::MgxNumeric::userRepresentation (da)<<", "
             <<(meshStructured?"True":"False")<<", "
             <<Utils::Math::MgxNumeric::userRepresentation (rat)<<", "
             <<(long)naxe<<", "
             <<(long)ni<<", "
             <<(long)nr;
     if (!groupName.empty())
         cmd<<", \""<<groupName<<"\"";
     cmd<<")";
     command->setScriptCommand(cmd);

     // on passe au gestionnaire de commandes qui exécute la commande en // ou non
     // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
     getCommandManager().addCommand(command, Utils::Command::DO);

     Internal::M3DCommandResultIfc*  cmdResult   =
                                     new Internal::M3DCommandResult (*command);
     return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newHollowCylinderWithTopo(const Point& pcentre, const double& dr_int, const double& dr_ext,
        const Vector& dv, const double& da,
        bool meshStructured,
        const int naxe, const int ni, const int nr,
        std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
     message << "TopoManager::newHollowCylinderWithTopo("<<pcentre<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (dr_int)<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (dr_ext)<<", "
     	 	 <<dv<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (da)<<", "
             <<(meshStructured?"True":"False")<<", "
	         <<(long)naxe<<", "<<(long)ni<<", "<<(long)nr;
     if (!groupName.empty())
         message<<", "<<groupName;
     message<<")";
     log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

     // creation de la commande de création de la géométrie
     Geom::CommandNewHollowCylinder *commandGeom =
    		 new Geom::CommandNewHollowCylinder(getLocalContext(),pcentre, dr_int, dr_ext, dv, da, groupName);

     // creation de la commande de création de la topologie
     // avec un volume à récupérer dans la commande
     // true <=> structuration demandée
     Topo::CommandCreateTopo* commandTopo=0;

     if (meshStructured) {
         CommandNewTopoOGridOnGeometry* commandTopoOGrid =
        		 new Topo::CommandNewTopoOGridOnGeometry(getLocalContext(), commandGeom, 0.0);
         commandTopoOGrid->setNaxe(naxe);
         commandTopoOGrid->setNi(ni);
         commandTopoOGrid->setNr(nr);
         commandTopo = commandTopoOGrid;
     }
     else
         commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

     // creation de la commande composite
     Internal::CommandComposite *command =
             new Internal::CommandComposite(getLocalContext(), "Création d'un cylindre creux avec une topologie");
     command->addCommand(commandGeom);
     command->addCommand(commandTopo);

     // trace dans le script
     TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
     cmd << getContextAlias() << "." << "getTopoManager().newHollowCylinderWithTopo ("
             <<pcentre.getScriptCommand()<<", "
             << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
             << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
             <<dv.getScriptCommand()<<", "
             <<Utils::Math::MgxNumeric::userRepresentation (da)<<", "
             <<(meshStructured?"True":"False")<<", "
             <<(long)naxe<<", "
             <<(long)ni<<", "
             <<(long)nr;
     if (!groupName.empty())
         cmd<<", \""<<groupName<<"\"";
     cmd<<")";
     command->setScriptCommand(cmd);

     // on passe au gestionnaire de commandes qui exécute la commande en // ou non
     // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
     getCommandManager().addCommand(command, Utils::Command::DO);

     Internal::M3DCommandResultIfc*  cmdResult   =
                                     new Internal::M3DCommandResult (*command);
     return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newConeWithTopo(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da,
			bool meshStructured, const double& rat,
            const int naxe, const int ni, const int nr,
			std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
     message << "TopoManager::newConeWithTopo("
	         <<Utils::Math::MgxNumeric::userRepresentation (dr1)<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (dr2)<<", "
			 <<dv<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (da)<<", "
             <<(meshStructured?"True":"False")<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (rat)<<", "
             <<(long)naxe<<", "<<(long)ni<<", "<<(long)nr;
     if (!groupName.empty())
         message<<", "<<groupName;
     message<<")";
     log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

     // creation de la commande de création de la géométrie
     Geom::CommandNewCone *commandGeom = new Geom::CommandNewCone(getLocalContext(), dr1, dr2, dv, da, groupName);

     // creation de la commande de création de la topologie
     // avec un volume à récupérer dans la commande
     // true <=> structuration demandée
     Topo::CommandCreateTopo* commandTopo=0;

     if (meshStructured) {
         CommandNewTopoOGridOnGeometry* commandTopoOGrid = new Topo::CommandNewTopoOGridOnGeometry(getLocalContext(), commandGeom, rat);
         commandTopoOGrid->setNaxe(naxe);
         commandTopoOGrid->setNi(ni);
         commandTopoOGrid->setNr(nr);
         commandTopo = commandTopoOGrid;
     }
     else
         commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

     // creation de la commande composite
     Internal::CommandComposite *command =
             new Internal::CommandComposite(getLocalContext(), "Création d'un cône avec une topologie");
     command->addCommand(commandGeom);
     command->addCommand(commandTopo);

     // trace dans le script
     TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
     cmd << getContextAlias() << "." << "getTopoManager().newConeWithTopo ("
             << Utils::Math::MgxNumeric::userRepresentation (dr1) <<", "
             << Utils::Math::MgxNumeric::userRepresentation (dr2) <<", "
             <<dv.getScriptCommand()<<", "
             <<Utils::Math::MgxNumeric::userRepresentation (da)<<", "
             <<(meshStructured?"True":"False")<<", "
	         <<Utils::Math::MgxNumeric::userRepresentation (rat)<<", "
             <<(long)naxe<<", "
             <<(long)ni<<", "
             <<(long)nr;
     if (!groupName.empty())
         cmd<<", \""<<groupName<<"\"";
     cmd<<")";
     command->setScriptCommand(cmd);

     // on passe au gestionnaire de commandes qui exécute la commande en // ou non
     // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
     getCommandManager().addCommand(command, Utils::Command::DO);

     Internal::M3DCommandResultIfc*  cmdResult   =
                                     new Internal::M3DCommandResult (*command);
     return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newSphereWithTopo(const Point& pcentre,  const double& dr,
                            const  Utils::Portion::Type& dt,
                            bool meshStructured, const double& rat,
                            const int ni, const int nr,
                            std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newSphereWithTopo("<<pcentre<<", "
	        <<Utils::Math::MgxNumeric::userRepresentation (dr)<<", "
            <<Utils::Portion::getName(dt)<<", "
            <<(meshStructured?"True":"False")<<", "
	        <<Utils::Math::MgxNumeric::userRepresentation (rat)<<", "<<(long)ni<<", "<<(long)nr;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande de création de la géométrie
    Geom::CommandNewSphere *commandGeom = new Geom::CommandNewSphere(getLocalContext(),pcentre, dr, dt, groupName);

    // creation de la commande de création de la topologie
    // avec un volume à récupérer dans la commande
    // true <=> structuration demandée
    Topo::CommandCreateTopo* commandTopo=0;

    if (meshStructured){
        CommandNewTopoOGridOnGeometry* commandTopoOGrid = new Topo::CommandNewTopoOGridOnGeometry(getLocalContext(), commandGeom, rat);
        commandTopoOGrid->setNi(ni);
        commandTopoOGrid->setNr(nr);
        commandTopo = commandTopoOGrid;
    }
    else
        commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Création d'une sphère avec une topologie");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newSphereWithTopo ("
            <<pcentre.getScriptCommand()<<", "
            << Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
            <<Utils::Portion::getName(dt)<<", "
            <<(meshStructured?"True":"False")<<", "
            <<Utils::Math::MgxNumeric::userRepresentation (rat)<<", "
            <<(long)ni<<", "
            <<(long)nr;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newHollowSphereWithTopo(const Point& pcentre,  const double& dr_int,  const double& dr_ext,
                            const  Utils::Portion::Type& dt,
                            bool meshStructured,
                            const int ni, const int nr,
                            std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newSphereWithTopo("<<pcentre<<", "
	        <<Utils::Math::MgxNumeric::userRepresentation (dr_int)<<", "
	        <<Utils::Math::MgxNumeric::userRepresentation (dr_ext)<<", "
            <<Utils::Portion::getName(dt)<<", "
            <<(meshStructured?"True":"False")<<", "
	        <<(long)ni<<", "<<(long)nr;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande de création de la géométrie
    Geom::CommandNewHollowSphere *commandGeom =
    		new Geom::CommandNewHollowSphere(getLocalContext(),pcentre, dr_int, dr_ext, dt, groupName);

    // creation de la commande de création de la topologie
    // avec un volume à récupérer dans la commande
    // true <=> structuration demandée
    Topo::CommandCreateTopo* commandTopo=0;

    if (meshStructured){
        CommandNewTopoOGridOnGeometry* commandTopoOGrid =
        		new Topo::CommandNewTopoOGridOnGeometry(getLocalContext(), commandGeom, 0.0);
        commandTopoOGrid->setNi(ni);
        commandTopoOGrid->setNr(nr);
        commandTopo = commandTopoOGrid;
    }
    else
        commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Création d'une sphère creuse avec une topologie");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newHollowSphereWithTopo ("
            <<pcentre.getScriptCommand()<<", "
            << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
            <<Utils::Portion::getName(dt)<<", "
            <<(meshStructured?"True":"False")<<", "
            <<(long)ni<<", "
            <<(long)nr;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::
newSpherePartWithTopo(const double& dr,
		const double& angleY,
		const double& angleZ,
		const int ni,
		const int nj,
		const int nk,
		std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newSpherePartWithTopo("
    		<<Utils::Math::MgxNumeric::userRepresentation (dr)
    		<<", "<<Utils::Math::MgxNumeric::userRepresentation (angleY)
    		<<", "<<Utils::Math::MgxNumeric::userRepresentation (angleZ)
    		<<", "<<(long)ni<<", "<<(long)nj<<", "<<(long)nk;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande de création de la géométrie
    Geom::CommandNewSpherePart *commandGeom = new Geom::CommandNewSpherePart(getLocalContext(),dr, angleY, angleZ, groupName);

    // creation de la commande de création de la topologie
    // avec un volume à récupérer dans la commande
    Topo::CommandCreateTopo* commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, ni, nj, nk);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Création d'une aiguille pleine avec une topologie");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newSpherePartWithTopo ("
            << Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (angleY) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (angleZ) <<", "
            <<(long)ni<<", "
            <<(long)nj<<", "
            <<(long)nk;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::
newHollowSpherePartWithTopo(const double& dr_int,
		const double& dr_ext,
		const double& angleY,
		const double& angleZ,
		const int ni,
		const int nj,
		const int nk,
		std::string groupName)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newHollowSpherePartWithTopo("
    		<<Utils::Math::MgxNumeric::userRepresentation (dr_int)
    		<<", "<<Utils::Math::MgxNumeric::userRepresentation (angleY)
    		<<", "<<Utils::Math::MgxNumeric::userRepresentation (angleZ)
    		<<", "<<(long)ni<<", "<<(long)nj<<", "<<(long)nk;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande de création de la géométrie
    Geom::CommandNewHollowSpherePart *commandGeom = new Geom::CommandNewHollowSpherePart(getLocalContext(),dr_int, dr_ext, angleY, angleZ, groupName);

    // creation de la commande de création de la topologie
    // avec un volume à récupérer dans la commande
    Topo::CommandCreateTopo* commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom, ni, nj, nk);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Création d'une aiguille creuse avec une topologie");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newHollowSpherePartWithTopo ("
            << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (angleY) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (angleZ) <<", "
            <<(long)ni<<", "
            <<(long)nj<<", "
            <<(long)nk;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
newIJBoxesWithTopo(int ni, int nj, bool alternateStruture)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newIJBoxesWithTopo ("<<(int64_t)ni<<", "<<(int64_t)nj<<", "
            <<(alternateStruture?"True":"False")<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande composite
    TkUtil::UTF8String titreCmd(TkUtil::Charset::UTF_8);
    titreCmd << "Création d'une grille de "
            <<(int64_t)ni<<" X "<<(int64_t)nj<<" boites avec une topologie";
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(),titreCmd);
    std::vector<Topo::CommandCreateTopo*> topo_cmds;
    std::vector<Geom::CommandCreateGeom*> geom_cmds;
    bool structured = true;
    Utils::Math::Point pmin(0,0,0);
    Utils::Math::Point pmax(1,1,1);
    double increment = 1.0;
    for (uint j=0; j<nj; j++){
        pmin.setX(0);
        pmax.setX(1);
        for (uint i=0; i<ni; i++){
            TkUtil::UTF8String gname("B", TkUtil::Charset::UTF_8);
            gname<<(int64_t)i<<"."<<(int64_t)j;
            Geom::CommandNewBox *commandGeom = new Geom::CommandNewBox(getLocalContext(),pmin,pmax,gname);
            Topo::CommandNewTopoOnGeometry* commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom,
            		(structured?CommandNewTopoOnGeometry::STRUCTURED_BLOCK:CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK));

            command->addCommand(commandGeom);
            geom_cmds.push_back(commandGeom);
            command->addCommand(commandTopo);
            topo_cmds.push_back(commandTopo);

            // pour alterner les blocs structurés/non-structurés
            if (alternateStruture)
                structured = !structured;

            // décalage de la boite
            pmin.setX(pmin.getX()+increment);
            pmax.setX(pmax.getX()+increment);
        } // for (uint i=0; i<ni; i++)
        pmin.setY(pmin.getY()+increment);
        pmax.setY(pmax.getY()+increment);
    } // for (uint j=0; j<nj; j++)

    // commande de fusion (glue) géométrique de tous les volumes
    // (dont on ne dispose que de la commande de création)
    if (geom_cmds.size()>1){
        Geom::CommandEditGeom *commandGeom = new Geom::CommandGluing(getLocalContext(),geom_cmds);
        command->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        command->addCommand(commandTopo);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newIJBoxesWithTopo ("
            <<(int64_t)ni<<", "<<(int64_t)nj<<", "
            <<(alternateStruture?"True":"False")<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::newIJKBoxesWithTopo(int ni, int nj, int nk, bool alternateStruture)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::newIJKBoxesWithTopo ("<<(int64_t)ni<<", "<<(int64_t)nj<<", "<<(int64_t)nk<<", "
            <<(alternateStruture?"True":"False")<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // creation de la commande composite
    TkUtil::UTF8String titreCmd(TkUtil::Charset::UTF_8);
    titreCmd << "Création d'une grille de "
            <<(int64_t)ni<<" X "<<(int64_t)nj<<" X "<<(int64_t)nk<<" boites avec une topologie";
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(),titreCmd);
    std::vector<Topo::CommandCreateTopo*> topo_cmds;
    std::vector<Geom::CommandCreateGeom*> geom_cmds;
    bool structured = true;
    Utils::Math::Point pmin(0,0,0);
    Utils::Math::Point pmax(1,1,1);
    double increment = 1.0;
    for (uint k=0; k<nk; k++){
        pmin.setY(0);
        pmax.setY(1);
        for (uint j=0; j<nj; j++){
            pmin.setX(0);
            pmax.setX(1);
            for (uint i=0; i<ni; i++){
				TkUtil::UTF8String	gname ("B", TkUtil::Charset::UTF_8);
                gname<<(int64_t)i<<"."<<(int64_t)j<<"."<<(int64_t)k;
                Geom::CommandNewBox *commandGeom = new Geom::CommandNewBox(getLocalContext(),pmin,pmax,gname);
                Topo::CommandNewTopoOnGeometry* commandTopo = new Topo::CommandNewTopoOnGeometry(getLocalContext(), commandGeom,
                		(structured?CommandNewTopoOnGeometry::STRUCTURED_BLOCK:CommandNewTopoOnGeometry::UNSTRUCTURED_BLOCK));

                command->addCommand(commandGeom);
                geom_cmds.push_back(commandGeom);
                command->addCommand(commandTopo);
                topo_cmds.push_back(commandTopo);

                // pour alterner les blocs structurés/non-structurés
                if (alternateStruture)
                    structured = !structured;

                // décalage de la boite
                pmin.setX(pmin.getX()+increment);
                pmax.setX(pmax.getX()+increment);
            } // for (uint i=0; i<ni; i++)
            pmin.setY(pmin.getY()+increment);
            pmax.setY(pmax.getY()+increment);
        } // for (uint j=0; j<nj; j++)
        pmin.setZ(pmin.getZ()+increment);
        pmax.setZ(pmax.getZ()+increment);
    } // for (uint k=0; k<nk; k++)

    // commande de fusion (glue) géométrique de tous les volumes
    // (dont on ne dispose que de la commande de création)
    if (geom_cmds.size()>1){
        Geom::CommandEditGeom *commandGeom = new Geom::CommandGluing(getLocalContext(),geom_cmds);
        command->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        command->addCommand(commandTopo);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newIJKBoxesWithTopo ("
            <<(int64_t)ni<<", "<<(int64_t)nj<<", "<<(int64_t)nk<<", "
            <<(alternateStruture?"True":"False")<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::newTopoOGridOnGeometry(std::string ne, const double& rat)
{
    return newTopoOGridOnGeometry(getLocalContext().getGeomManager().getEntity(ne), rat);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::newTopoOGridOnGeometry(Geom::GeomEntity* ge, const double& rat)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::newTopoOGridOnGeometry("<<ge->getName()<<","<<rat<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandNewTopoOGridOnGeometry* command = new Topo::CommandNewTopoOGridOnGeometry(getLocalContext(), ge, rat);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().newTopoOGridOnGeometry (\""
            <<ge->getName()<<"\","<<Utils::Math::MgxNumeric::userRepresentation (rat)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
void TopoManager::destroy(std::vector<std::string>& topo_entities_names, bool propagate)
{
    std::vector<TopoEntity*> topo_entities;
    TopoManager::getTopoEntities(topo_entities_names, topo_entities, "destroy");

    destroy (topo_entities, propagate);
}
/*----------------------------------------------------------------------------*/
void TopoManager::destroy(std::vector<Topo::TopoEntity*>& ve, bool propagate)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::destroy([";
    for (uint i=0; i<ve.size(); i++){
    	if (i)
    		message <<", ";
    	message << ve[i]->getName();
    }
    message <<"],"<<(propagate?"avec propagation":"sans propagation")<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandDestroyTopo* command = new Topo::CommandDestroyTopo(getLocalContext(), ve, propagate);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().destroy ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i]->getName();
    }
    cmd << "\"], "<<(propagate?"True":"False")<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::copy(std::vector<std::string>& vb, std::string vo)
{
	std::vector<Topo::Block* > blocs;
	for (std::vector<std::string>::iterator iter = vb.begin();
			iter != vb.end(); ++iter){
		blocs.push_back(getBlock(*iter));
	}

	Geom::Volume* vol = getContext().getGeomManager().getVolume(vo, false);

	return copy(blocs, vol);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::copy(std::vector<Topo::Block*>& vb, Geom::Volume* vo)
{
    Topo::CommandDuplicateTopo* command = new Topo::CommandDuplicateTopo(getLocalContext(),
    		vb, vo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().copy ([\"";
    for (uint i=0; i<vb.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vb[i]->getName();
    }
    if (vo)
    	cmd<<"\"], \""<<vo->getName()<<"\")";
    else
    	cmd<<"\"], \"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::extract(std::vector<std::string>& vb, const std::string ng)
{
	std::vector<Topo::Block* > blocs;
	for (std::vector<std::string>::iterator iter = vb.begin();
			iter != vb.end(); ++iter){
		blocs.push_back(getBlock(*iter));
	}

	return extract(blocs, ng);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::extract(std::vector<Topo::Block*>& vb, const std::string ng)
{
    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), std::string("Extraction de blocs dans ")+ng);

    std::vector<Topo::TopoEntity*> te;
    te.insert(te.end(), vb.begin(), vb.end());

    Topo::CommandDuplicateTopo* command1 = new Topo::CommandDuplicateTopo(getLocalContext(), vb, ng);

    Topo::CommandDestroyTopo* command2 = new Topo::CommandDestroyTopo(getLocalContext(), te, true);

    command->addCommand(command1);
    command->addCommand(command2);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().extract ([\"";
    for (uint i=0; i<vb.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vb[i]->getName();
    }
    cmd<<"\"], \""<<ng<<"\")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::insertHole(std::vector<std::string>& face_names)
{
    std::vector<CoFace*> cofaces;
    for (std::vector<std::string>::iterator iter = face_names.begin();
            iter != face_names.end(); ++iter)
    	cofaces.push_back(getCoFace(*iter));

    return insertHole(cofaces);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::insertHole(std::vector<CoFace*>& cofaces)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::insertHole("<<cofaces.size()<<" faces)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandInsertHole* command = new Topo::CommandInsertHole(getLocalContext(), cofaces);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().insertHole ([\"";
    for (uint i=0; i<cofaces.size(); i++){
       	if (i)
       		cmd <<"\", \"";
       	cmd << cofaces[i]->getName();
       }
       cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Edges(std::string na, std::string nb)
{
    return fuse2Edges(getCoEdge(na), getCoEdge(nb));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Edges(CoEdge* edge_A, CoEdge* edge_B)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::fuse2Edges("<<edge_A->getName()<<","<<edge_B->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandFuse2Edges* command = new Topo::CommandFuse2Edges(getLocalContext(), edge_A, edge_B);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuse2Edges (\""
            <<edge_A->getName()<<"\",\""<<edge_B->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::fuse2EdgeList(std::vector<std::string>& coedge_names1, std::vector<std::string>& coedge_names2)
{
    std::vector<Topo::CoEdge* > coedges1;
    for (std::vector<std::string>::iterator iter = coedge_names1.begin();
            iter != coedge_names1.end(); ++iter){
        coedges1.push_back(getCoEdge(*iter));
    }
    std::vector<Topo::CoEdge* > coedges2;
    for (std::vector<std::string>::iterator iter = coedge_names2.begin();
            iter != coedge_names2.end(); ++iter){
        coedges2.push_back(getCoEdge(*iter));
    }
    return fuse2EdgeList(coedges1, coedges2);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::fuse2EdgeList(std::vector<Topo::CoEdge* > &coedges1, std::vector<Topo::CoEdge* > &coedges2)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::fuse2EdgeList([";
    for (uint i=0; i<coedges1.size(); i++){
        if (i)
            message <<", ";
        message << coedges1[i]->getName();
    }
    message <<"], [";
    for (uint i=0; i<coedges2.size(); i++){
        if (i)
            message <<", ";
        message << coedges2[i]->getName();
    }
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandFuse2EdgeList* command = new Topo::CommandFuse2EdgeList(getLocalContext(),
    		coedges1, coedges2);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuse2EdgeList ([\"";
    for (uint i=0; i<coedges1.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << coedges1[i]->getName();
    }
    cmd <<"\"], [\"";
    for (uint i=0; i<coedges2.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << coedges2[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Faces(std::string na, std::string nb)
{
    return fuse2Faces(getCoFace(na), getCoFace(nb));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Faces(CoFace* face_A, CoFace* face_B)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::fuse2Faces("<<face_A->getName()<<","<<face_B->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandFuse2Faces* command = new Topo::CommandFuse2Faces(getLocalContext(), face_A, face_B);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuse2Faces (\""
            <<face_A->getName()<<"\",\""<<face_B->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::fuse2FaceList(std::vector<std::string>& coface_names1, std::vector<std::string>& coface_names2)
{
    std::vector<Topo::CoFace* > cofaces1;
    for (std::vector<std::string>::iterator iter = coface_names1.begin();
            iter != coface_names1.end(); ++iter){
        cofaces1.push_back(getCoFace(*iter));
    }
    std::vector<Topo::CoFace* > cofaces2;
    for (std::vector<std::string>::iterator iter = coface_names2.begin();
            iter != coface_names2.end(); ++iter){
        cofaces2.push_back(getCoFace(*iter));
    }
    return fuse2FaceList(cofaces1, cofaces2);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::fuse2FaceList(std::vector<Topo::CoFace* > &cofaces1, std::vector<Topo::CoFace* > &cofaces2)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::fuse2FaceList([";
    for (uint i=0; i<cofaces1.size(); i++){
        if (i)
            message <<", ";
        message << cofaces1[i]->getName();
    }
    message <<"], [";
    for (uint i=0; i<cofaces2.size(); i++){
        if (i)
            message <<", ";
        message << cofaces2[i]->getName();
    }
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandFuse2FaceList* command = new Topo::CommandFuse2FaceList(getLocalContext(),
    		cofaces1, cofaces2);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuse2FaceList ([\"";
    for (uint i=0; i<cofaces1.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces1[i]->getName();
    }
    cmd <<"\"], [\"";
    for (uint i=0; i<cofaces2.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces2[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::glue2Blocks(std::string na, std::string nb)
{
    return glue2Blocks(getBlock(na), getBlock(nb));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::glue2Blocks(Block* bl_A, Block* bl_B)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::glue2Blocks("<<bl_A->getName()<<","<<bl_B->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandGlue2Blocks* command = new Topo::CommandGlue2Blocks(getLocalContext(), bl_A, bl_B);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().glue2Blocks (\""
            <<bl_A->getName()<<"\",\""<<bl_B->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::glue2Topo(
		std::string volName1, std::string volName2)
{
	return glue2Topo(getLocalContext().getLocalGeomManager().getVolume(volName1, true),
			getLocalContext().getLocalGeomManager().getVolume(volName2, true));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::glue2Topo(
		Geom::Volume* vol1, Geom::Volume* vol2)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::glue2Topo("<<vol1->getName()<<","<<vol2->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandGlue2Topo* command = new Topo::CommandGlue2Topo(getLocalContext(), vol1, vol2);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().glue2Topo (\""
            <<vol1->getName()<<"\",\""<<vol2->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Vertices(std::string na, std::string nb)
{
    return fuse2Vertices(getVertex(na), getVertex(nb));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Vertices(Topo::Vertex* vtx_A, Topo::Vertex* vtx_B)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::fuse2Vertices("<<vtx_A->getName()<<","<<vtx_B->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandFuse2Vertices* command = new Topo::CommandFuse2Vertices(getLocalContext(), vtx_A, vtx_B);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuse2Vertices (\""
            <<vtx_A->getName()<<"\",\""<<vtx_B->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlock(std::string nbloc, std::string narete, const double& ratio)
{
    return splitBlock(getBlock(nbloc), getCoEdge(narete), ratio);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlock(Block* bloc, CoEdge* arete, const double& ratio)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitBlock("<<bloc->getName()<<","<<arete->getName()<<","<<ratio<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitBlock* command = new Topo::CommandSplitBlock(getLocalContext(), bloc, arete, ratio);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitBlock (\""
            <<bloc->getName()<<"\",\""<<arete->getName()<<"\","<<Utils::Math::MgxNumeric::userRepresentation (ratio)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlock(Block* bloc, CoEdge* arete, const Point& pt)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitBlock("<<bloc->getName()<<","<<arete->getName()<<","<<pt<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitBlock* command = new Topo::CommandSplitBlock(getLocalContext(), bloc, arete, pt);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitBlock (\""
            <<bloc->getName()<<"\",\""<<arete->getName()<<"\","<<pt.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const double& ratio)
{
    std::vector<Topo::Block* > blocs;
    for (std::vector<std::string>::iterator iter = blocs_names.begin();
            iter != blocs_names.end(); ++iter)
        blocs.push_back(getBlock(*iter));

    if (blocs.size() > 1)
    	return splitBlocks(blocs, getCoEdge(narete), ratio);
    else if (blocs.size() == 1)
    	return splitBlock(blocs[0], getCoEdge(narete), ratio);
    else
    	throw TkUtil::Exception (TkUtil::UTF8String ("Le découpage de blocs ne peut se faire sans aucun bloc", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitBlocks(std::vector<Topo::Block* > &blocs, CoEdge* arete, const double& ratio)
{
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitBlocks ([\"";
    for (uint i=0; i<blocs.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << blocs[i]->getName();
    }
    cmd <<"\"], \""<<arete->getName()
    	<<"\","<<Utils::Math::MgxNumeric::userRepresentation (ratio)<<")";

    Topo::CommandSplitBlocks* command = new Topo::CommandSplitBlocks(getLocalContext(), blocs, arete, ratio);

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitAllBlocks(std::string narete, const double& ratio)
{
	return splitAllBlocks(getCoEdge(narete), ratio);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitAllBlocks(CoEdge* arete, const double& ratio)
{
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitAllBlocks (\""<<arete->getName()
    	<<"\","<<Utils::Math::MgxNumeric::userRepresentation (ratio)<<")";

    Topo::CommandSplitBlocks* command = new Topo::CommandSplitBlocks(getLocalContext(), arete, ratio);

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const Point& pt)
{
    std::vector<Topo::Block* > blocs;
    for (std::vector<std::string>::iterator iter = blocs_names.begin();
            iter != blocs_names.end(); ++iter)
        blocs.push_back(getBlock(*iter));

    if (blocs.size() >= 1)
    	return splitBlocks(blocs, getCoEdge(narete), pt);
    else
    	throw TkUtil::Exception (TkUtil::UTF8String ("Le découpage de blocs ne peut se faire sans aucun bloc", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitBlocks(std::vector<Topo::Block* > &blocs, CoEdge* arete, const Point& pt)
{
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitBlocks ([\"";
    for (uint i=0; i<blocs.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << blocs[i]->getName();
    }
    cmd <<"\"], \""<<arete->getName()
    	<<"\","<<pt.getScriptCommand()<<")";

    Topo::CommandSplitBlocks* command = new Topo::CommandSplitBlocks(getLocalContext(), blocs, arete, pt);

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitAllBlocks(std::string narete, const Point& pt)
{
	return splitAllBlocks(getCoEdge(narete), pt);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::splitAllBlocks(CoEdge* arete, const Point& pt)
{
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitAllBlocks (\""<<arete->getName()
    	<<"\","<<pt.getScriptCommand()<<")";

    Topo::CommandSplitBlocks* command = new Topo::CommandSplitBlocks(getLocalContext(), arete, pt);

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::extendSplitBlock(std::string nbloc, std::string narete)
{
    return extendSplitBlock(getBlock(nbloc), getCoEdge(narete));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::extendSplitBlock(Block* bloc, CoEdge* arete)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::extendSplitBlock("<<bloc->getName()<<","<<arete->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandExtendSplitBlock* command = new Topo::CommandExtendSplitBlock(getLocalContext(), bloc, arete);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().extendSplitBlock (\""
            <<bloc->getName()<<"\",\""<<arete->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlocksWithOgridV2(std::vector<std::string> &blocs_names,
        std::vector<std::string> &cofaces_names,
        const double& ratio_ogrid, int nb_bras)
{
    std::vector<Topo::Block* > blocs;
    for (std::vector<std::string>::iterator iter = blocs_names.begin();
            iter != blocs_names.end(); ++iter){
        blocs.push_back(getBlock(*iter));
    }
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
        cofaces.push_back(getCoFace(*iter));
    }
    return splitBlocksWithOgrid(blocs, cofaces, ratio_ogrid, nb_bras, false, false);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlocksWithOgrid(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio_ogrid, int nb_bras)
{
    std::vector<Topo::Block* > blocs;
    for (std::vector<std::string>::iterator iter = blocs_names.begin();
            iter != blocs_names.end(); ++iter){
        blocs.push_back(getBlock(*iter));
    }
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
        cofaces.push_back(getCoFace(*iter));
    }
    return splitBlocksWithOgrid(blocs, cofaces, ratio_ogrid, nb_bras, false, true);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlocksWithOgrid_old(std::vector<std::string> &blocs_names,
        std::vector<std::string> &cofaces_names,
        const double& ratio_ogrid, int nb_bras)
{
    std::vector<Topo::Block* > blocs;
    for (std::vector<std::string>::iterator iter = blocs_names.begin();
            iter != blocs_names.end(); ++iter){
        blocs.push_back(getBlock(*iter));
    }
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
        cofaces.push_back(getCoFace(*iter));
    }
    return splitBlocksWithOgrid(blocs, cofaces, ratio_ogrid, nb_bras, true, true);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitBlocksWithOgrid(std::vector<Topo::Block* > &blocs,
        std::vector<Topo::CoFace* > &cofaces,
        const double& ratio_ogrid, int nb_bras,
		bool create_internal_vertices,
		bool propagate_neighbor_block)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitBlocksWithOgrid([";
    for (uint i=0; i<blocs.size(); i++){
        if (i)
            message <<", ";
        message << blocs[i]->getName();
    }
    message <<"], [";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            message <<", ";
        message << cofaces[i]->getName();
    }
    message <<"],"<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<","<<(short)nb_bras<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitBlocksWithOgrid* command = new Topo::CommandSplitBlocksWithOgrid(getLocalContext(),
            blocs, cofaces, ratio_ogrid, nb_bras, create_internal_vertices, propagate_neighbor_block);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    if (propagate_neighbor_block)
    	cmd << getContextAlias() << "." << "getTopoManager().splitBlocksWithOgrid ([\"";
    else if (create_internal_vertices)
    	cmd << getContextAlias() << "." << "getTopoManager().splitBlocksWithOgrid_old ([\"";
    else
    	cmd << getContextAlias() << "." << "getTopoManager().splitBlocksWithOgridV2 ([\"";

    for (uint i=0; i<blocs.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << blocs[i]->getName();
    }
    cmd <<"\"], [";
    if (!cofaces.empty())
        cmd <<"\"";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces[i]->getName();
    }
    if (!cofaces.empty())
        cmd <<"\"";
    cmd << "], ";
    cmd <<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<", "<<(short)nb_bras<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFacesWithOgrid(std::vector<std::string> &cofaces_names,
        std::vector<std::string> &coedges_names,
        const double& ratio_ogrid, int nb_bras)
{
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
        cofaces.push_back(getCoFace(*iter));
    }
    std::vector<Topo::CoEdge* > coedges;
    for (std::vector<std::string>::iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter){
    	coedges.push_back(getCoEdge(*iter));
    }
    return splitFacesWithOgrid(cofaces, coedges, ratio_ogrid, nb_bras);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFacesWithOgrid(std::vector<Topo::CoFace*> &cofaces,
        std::vector<Topo::CoEdge*> &coedges,
        const double& ratio_ogrid, int nb_bras)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitFacesWithOgrid([";
    for (uint i=0; i<coedges.size(); i++){
        if (i)
            message <<", ";
        message << coedges[i]->getName();
    }
    message <<"], [";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            message <<", ";
        message << cofaces[i]->getName();
    }
    message <<"],"<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<","<<(short)nb_bras<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFacesWithOgrid* command = new Topo::CommandSplitFacesWithOgrid(getLocalContext(),
            cofaces, coedges, ratio_ogrid, nb_bras);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
   	cmd << getContextAlias() << "." << "getTopoManager().splitFacesWithOgrid ([\"";

    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces[i]->getName();
    }
    cmd <<"\"], [";
    if (!coedges.empty())
        cmd <<"\"";
    for (uint i=0; i<coedges.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << coedges[i]->getName();
    }
    if (!coedges.empty())
        cmd <<"\"";
    cmd << "], ";
    cmd <<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<", "<<(short)nb_bras<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const double& ratio_dec, const double& ratio_ogrid)
{
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
        cofaces.push_back(getCoFace(*iter));
    }
    return splitFaces(cofaces, getCoEdge(narete), ratio_dec, ratio_ogrid);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFaces(std::vector<Topo::CoFace*  > &cofaces, CoEdge* arete, const double& ratio_dec, const double& ratio_ogrid)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitFaces([";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            message <<", ";
        message << cofaces[i]->getName();
    }
    message <<"],"<<arete->getName()<<","<<Utils::Math::MgxNumeric::userRepresentation (ratio_dec)<<","<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFaces* command = new Topo::CommandSplitFaces(getLocalContext(), cofaces, arete, ratio_dec, ratio_ogrid);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitFaces ([\"";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces[i]->getName();
    }
    cmd << "\"], \"";
    cmd << arete->getName() <<"\", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_dec)<<", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const Point& pt, const double& ratio_ogrid)
{
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
        cofaces.push_back(getCoFace(*iter));
    }
    return splitFaces(cofaces, getCoEdge(narete), pt, ratio_ogrid);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFaces(std::vector<Topo::CoFace*  > &cofaces, CoEdge* arete, const Point& pt, const double& ratio_ogrid)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitFaces([";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            message <<", ";
        message << cofaces[i]->getName();
    }
    message <<"],"<<arete->getName()<<","<<pt<<","<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFaces* command = new Topo::CommandSplitFaces(getLocalContext(), cofaces, arete, pt, ratio_ogrid);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitFaces ([\"";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces[i]->getName();
    }
    cmd << "\"], \"";
    cmd << arete->getName() <<"\", "<<pt.getScriptCommand()<<", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFace(std::string coface_name, std::string narete, const double& ratio_dec, bool project_on_meshing_edges)
{
    return splitFace(getCoFace(coface_name), getCoEdge(narete), ratio_dec, project_on_meshing_edges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFace(Topo::CoFace* coface, CoEdge* coedge, const double& ratio_dec, bool project_on_meshing_edges)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);

    message <<"TopoManager::splitFace("<<coface->getName()<<", "<< coedge->getName() <<", "<< ratio_dec<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFaces* command = new Topo::CommandSplitFaces(getLocalContext(), coface, coedge, ratio_dec, project_on_meshing_edges);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitFace (\"";
    cmd << coface->getName()<<"\", \""<< coedge->getName() <<"\", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_dec);
    if (project_on_meshing_edges)
    	cmd << ", True)";
    else
    	cmd << ", False)";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFace(std::string coface_name, std::string narete, const Point& pt, bool project_on_meshing_edges)
{
    return splitFace(getCoFace(coface_name), getCoEdge(narete), pt, project_on_meshing_edges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitFace(Topo::CoFace* coface, CoEdge* coedge, const Point& pt, bool project_on_meshing_edges)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);

    message <<"TopoManager::splitFace("<<coface->getName()<<", "<< coedge->getName() <<", "<< pt<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFaces* command = new Topo::CommandSplitFaces(getLocalContext(), coface, coedge, pt, project_on_meshing_edges);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitFace (\"";
    cmd << coface->getName()<<"\", \""<< coedge->getName() <<"\", "<<pt.getScriptCommand();
    if (project_on_meshing_edges)
    	cmd << ", True)";
    else
    	cmd << ", False)";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::extendSplitFace(std::string coface_name, std::string nsommet)
{
	return extendSplitFace(getCoFace(coface_name), getVertex(nsommet));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::extendSplitFace(Topo::CoFace* coface, Vertex* vertex)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);

    message <<"TopoManager::extendSplitFace("<<coface->getName()<<", "<< vertex->getName() <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandExtendSplitFace* command = new Topo::CommandExtendSplitFace(getLocalContext(), coface, vertex);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().extendSplitFace (\"";
    cmd << coface->getName()<<"\", \""
    	<< vertex->getName() <<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitAllFaces(std::string narete, const double& ratio_dec, const double& ratio_ogrid)
{
    return splitAllFaces(getCoEdge(narete), ratio_dec, ratio_ogrid);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitAllFaces(CoEdge* coedge, const double& ratio_dec, const double& ratio_ogrid)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitAllFaces(";
    message <<coedge->getName()<<","<<Utils::Math::MgxNumeric::userRepresentation (ratio_dec)<<","<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFaces* command = new Topo::CommandSplitFaces(getLocalContext(), coedge, ratio_dec, ratio_ogrid);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitAllFaces (\"";
    cmd << coedge->getName() <<"\", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_dec)<<", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitAllFaces(std::string narete, const Point& pt, const double& ratio_ogrid)
{
    return splitAllFaces(getCoEdge(narete), pt, ratio_ogrid);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitAllFaces(CoEdge* coedge, const Point& pt, const double& ratio_ogrid)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::splitAllFaces(";
    message <<coedge->getName()<<","<<pt<<","<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitFaces* command = new Topo::CommandSplitFaces(getLocalContext(), coedge, pt, ratio_ogrid);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitAllFaces (\"";
    cmd << coedge->getName() <<"\", "<<pt.getScriptCommand()<<", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_ogrid)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitEdge(std::string coedge_name, const double& ratio_dec)
{
   return splitEdge(getCoEdge(coedge_name), ratio_dec);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitEdge(CoEdge* coedge, const double& ratio_dec)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);

    message <<"TopoManager::splitEdge("<<coedge->getName()<<", "<< Utils::Math::MgxNumeric::userRepresentation (ratio_dec)<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitEdge* command = new Topo::CommandSplitEdge(getLocalContext(), coedge, ratio_dec);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitEdge (\"";
    cmd << coedge->getName()<<"\", "<<Utils::Math::MgxNumeric::userRepresentation (ratio_dec)<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitEdge(std::string coedge_name, const Point& pt)
{
   return splitEdge(getCoEdge(coedge_name), pt);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::splitEdge(CoEdge* coedge, const Point& pt)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);

    message <<"TopoManager::splitEdge("<<coedge->getName()<<", "<< pt<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSplitEdge* command = new Topo::CommandSplitEdge(getLocalContext(), coedge, pt);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().splitEdge (\"";
    cmd << coedge->getName()<<"\", "<<pt.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapVertices(std::string nom1, std::string nom2, bool project_on_first)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
     message <<"TopoManager::snapVertices(";
     message <<nom1<<","<<nom2
             <<(project_on_first?", avec projection sur le premier":", avec placement au milieu des sommets")<<")";
     log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

     // trace pour le script
     TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
     cmd << getContextAlias() << "." << "getTopoManager().snapVertices (\""<<nom1<<"\", \""<<nom2<<"\", ";
     cmd << (project_on_first?"True":"False")<<")";

     Vertex* ve1 = 0;
     Vertex* ve2 = 0;
     CoEdge* ce1 = 0;
     CoEdge* ce2 = 0;
     CoFace* cf1 = 0;
     CoFace* cf2 = 0;

     if ((ve1 = TopoManager::getVertex (nom1, false)) != 0){
         if ((ve2 = TopoManager::getVertex (nom2, false)) != 0){
             return snapVertices(ve1, ve2, project_on_first, cmd);
         }
         else
             throw TkUtil::Exception (TkUtil::UTF8String ("Si la première entité est un sommet, la deuxième doit l'être aussi", TkUtil::Charset::UTF_8));
     }
     else if ((ce1 = TopoManager::getCoEdge (nom1, false)) != 0){
         if ((ce2 = TopoManager::getCoEdge (nom2, false)) != 0){
             return snapVertices(ce1, ce2, project_on_first, cmd);
         }
         else
             throw TkUtil::Exception (TkUtil::UTF8String ("Si la première entité est une arête, la deuxième doit l'être aussi", TkUtil::Charset::UTF_8));
     }
     else if ((cf1 = TopoManager::getCoFace (nom1, false)) != 0){
         if ((cf2 = TopoManager::getCoFace (nom2, false)) != 0){
             return snapVertices(cf1, cf2, project_on_first, cmd);
         }
         else
             throw TkUtil::Exception (TkUtil::UTF8String ("Si la première entité est une arête, la deuxième doit l'être aussi", TkUtil::Charset::UTF_8));
     }
     else
         throw TkUtil::Exception (TkUtil::UTF8String ("snapVertices ne peut se faire que pour des sommets, arêtes ou faces", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapVertices(Topo::Vertex* ve1, Topo::Vertex* ve2,
		bool project_on_first,
		TkUtil::UTF8String& scriptCommand)
{
    std::vector<Topo::Vertex* > vertices1;
    std::vector<Topo::Vertex* > vertices2;

    vertices1.push_back(ve1);
    vertices2.push_back(ve2);

    return snapVertices(vertices1, vertices2, project_on_first, scriptCommand);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapVertices(Topo::CoEdge* ce1, Topo::CoEdge* ce2,
		bool project_on_first,
		TkUtil::UTF8String& scriptCommand)
{
    std::vector<Topo::Vertex* > vertices1;
    std::vector<Topo::Vertex* > vertices2;

    ce1->getVertices(vertices1);
    ce2->getVertices(vertices2);

    return snapVertices(vertices1, vertices2, project_on_first, scriptCommand);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapVertices(Topo::CoFace* cf1, Topo::CoFace* cf2,
		bool project_on_first,
		TkUtil::UTF8String& scriptCommand)
{
    std::vector<Topo::Vertex* > vertices1;
    std::vector<Topo::Vertex* > vertices2;

    cf1->getVertices(vertices1);
    cf2->getVertices(vertices2);

    return snapVertices(vertices1, vertices2, project_on_first, scriptCommand);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapVertices(std::vector<Topo::Vertex* > vertices1, std::vector<Topo::Vertex* > vertices2,
		bool project_on_first,
		TkUtil::UTF8String& scriptCommand)
{
     // la commande avec les sommets dans n'importe quel ordre
     Topo::CommandSnapVertices* command = new Topo::CommandSnapVertices(getLocalContext(),
             vertices1, vertices2, project_on_first);

     command->setScriptCommand(scriptCommand);

     getCommandManager().addCommand(command, Utils::Command::DO);

     Internal::M3DCommandResultIfc*  cmdResult   =
             new Internal::M3DCommandResult (*command);
     return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::makeBlocksByRevol(std::vector<std::string>& coedges_names, const  Utils::Portion::Type& dt)
{
    std::vector<CoEdge*> coedges;
    for (std::vector<std::string>::iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter)
        coedges.push_back(getCoEdge(*iter));

    return makeBlocksByRevol(coedges, dt, 1.0);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::makeBlocksByRevolWithRatioOgrid(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt, const double& ratio_ogrid)
{
    std::vector<CoEdge*> coedges;
    for (std::vector<std::string>::iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter)
        coedges.push_back(getCoEdge(*iter));

    return makeBlocksByRevol(coedges, dt, ratio_ogrid);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::makeBlocksByRevol(std::vector<CoEdge*>& coedges, const  Utils::Portion::Type& dt, const double& ratio_ogrid)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::makeBlocksByRevol([";
    for (uint i=0; i<coedges.size(); i++){
    	if (i)
    		message <<", ";
    	message << coedges[i]->getName();
    }
    message <<"],"<<Utils::Portion::getName(dt)<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

	if (dt != Utils::Portion::DEMI
			&& dt != Utils::Portion::TIERS
			&& dt != Utils::Portion::QUART
			&& dt != Utils::Portion::CINQUIEME
			&& dt != Utils::Portion::SIXIEME
			&& dt != Utils::Portion::ENTIER)
        throw TkUtil::Exception (TkUtil::UTF8String ("On ne peut faire la révolution que pour sixième, cinquième, quart, tiers, demi ou plein", TkUtil::Charset::UTF_8));

    // recherche de toutes les entités géométriques en relation avec les arêtes
    // on se limite aux surfaces
	// pour le cas sans sélection d'ogrid, on prend toutes les surfaces existantes
    std::vector<Geom::GeomEntity*> geom_entities;

    // pour ne prendre les coface qu'une fois
    std::map<CoFace*, uint> filtre_coface;
    // idem avec les surfaces
    std::map<Geom::GeomEntity*, uint> filtre_surf;

    if (coedges.empty()){
    	std::vector<Geom::Surface*> surfs = getLocalContext().getLocalGeomManager().getSurfacesObj();
    	geom_entities.insert(geom_entities.end(), surfs.begin(), surfs.end());
    }

    for (uint i=0; i<coedges.size(); i++){
    	CoEdge* coedge = coedges[i];

        std::vector<CoFace* > cofaces_dep;
        std::vector<CoFace* > cofaces_suiv;

        coedge->getCoFaces(cofaces_dep);

        for (std::vector<CoFace* >::iterator iter = cofaces_dep.begin();
                iter != cofaces_dep.end(); ++iter){
            Geom::GeomEntity* ge = (*iter)->getGeomAssociation();
            if (ge && filtre_surf[ge] == 0){
                geom_entities.push_back(ge);
                filtre_surf[ge] = 1;
            }
            filtre_coface[*iter] = 1;
        }

        do {

            for (std::vector<CoFace* >::iterator iter1 = cofaces_dep.begin();
                    iter1 != cofaces_dep.end(); ++iter1){
                std::vector<CoFace* > cofaces_vois;
                (*iter1)->getNeighbour(cofaces_vois);

                for (std::vector<CoFace* >::iterator iter2 = cofaces_vois.begin();
                        iter2 != cofaces_vois.end(); ++iter2){
                    if (filtre_coface[*iter2] == 0){
                        filtre_coface[*iter2] = 1;
                        cofaces_suiv.push_back(*iter2);

                        Geom::GeomEntity* ge = (*iter2)->getGeomAssociation();
                        if (ge && filtre_surf[ge] == 0){
                            geom_entities.push_back(ge);
                            filtre_surf[ge] = 1;
                        }
                    }
                }
            }

            cofaces_dep = cofaces_suiv;
            cofaces_suiv.clear();

        } while (!cofaces_dep.empty());


    } // end for i<coedges.size()

#ifdef _DEBUG2
    std::cout<<"makeBlocksByRevol geom_entities:";
    for (std::vector<Geom::GeomEntity*>::iterator iter = geom_entities.begin();
    		iter != geom_entities.end(); ++iter)
    	std::cout<<" "<<(*iter)->getName();
    std::cout<<std::endl;
#endif

    // pour le moment, rotation / axe des X
    Utils::Math::Point axis1(0,0,0);
    Utils::Math::Point axis2(1,0,0);

    // révolution de la géométrie sans conservation des données 2D initiales
    Geom::CommandExtrudeRevolution *commandGeom =
               new Geom::CommandExtrudeRevolution(getLocalContext(), geom_entities, Utils::Math::Rotation(axis1, axis2, dt), false);

    Topo::CommandMakeBlocksByRevol* commandTopo =
            new Topo::CommandMakeBlocksByRevol(getLocalContext(), commandGeom, coedges, dt, ratio_ogrid);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Construction Topo et Geom 3D avec o-grid par révolution");
    command->addCommand(commandGeom);
    command->addCommand(commandTopo);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    if (ratio_ogrid==1.0)
    	cmd << getContextAlias() << "." << "getTopoManager().makeBlocksByRevol ([";
    else
    	cmd << getContextAlias() << "." << "getTopoManager().makeBlocksByRevolWithRatioOgrid ([";
    for (uint i=0; i<coedges.size(); i++){
    	if (i)
    		cmd <<", ";
    	cmd << "\""<<coedges[i]->getName()<< "\"";
    }
    cmd <<"], "<<Utils::Portion::getName(dt);
    if (ratio_ogrid!=1.0)
    	cmd <<", "<<ratio_ogrid;
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::makeBlocksByExtrude(std::vector<std::string>& cofaces_names, const  Utils::Math::Vector& dv)
{
    std::vector<TopoEntity*> cofaces;
    for (std::vector<std::string>::iterator iter = cofaces_names.begin(); iter != cofaces_names.end(); ++iter)
        cofaces.push_back(getCoFace(*iter));

    return makeBlocksByExtrude(cofaces, dv);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::makeBlocksByExtrude(std::vector<TopoEntity*>& cofaces, const  Utils::Math::Vector& dv)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::makeBlocksByExtrude([";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            message <<", ";
        message << cofaces[i]->getName();
    }
    message <<"],"<<dv<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandExtrudeFace* command = new CommandExtrudeFace(getLocalContext(), cofaces, dv);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().makeBlocksByExtrude ([";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            cmd <<", ";
        cmd << "\""<<cofaces[i]->getName()<< "\"";
    }
    cmd <<"],";
    cmd << dv << ")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<std::string> &coedges_names)
{
    std::vector<CoEdge*> coedges;
    for (std::vector<std::string>::iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter)
        coedges.push_back(getCoEdge(*iter));

    return setMeshingProperty(emp, coedges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<CoEdge*>& coedges)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setMeshingProperty(EdgeMeshingProperty("<<(long)emp.getNbEdges()<<","<<emp.getMeshLawName()
                    <<"),"<<coedges.size()<<" arêtes)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetEdgeMeshingProperty* command = new Topo::CommandSetEdgeMeshingProperty(getLocalContext(), emp, coedges);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << emp.getScriptCommandBegin()<<"\n";
    cmd << getContextAlias() << "." << "getTopoManager().setMeshingProperty ("
            <<emp.getScriptCommandRef()<<",[\"";
    for (uint i=0; i<coedges.size(); i++){
       	if (i)
       		cmd <<"\", \"";
       	cmd << coedges[i]->getName();
       }
       cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setAllMeshingProperty(CoEdgeMeshingProperty& emp)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setAllMeshingProperty(EdgeMeshingProperty("<<(long)emp.getNbEdges()<<","<<emp.getMeshLawName()
                    <<"), toutes les arêtes)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    std::vector<Topo::CoEdge* > coedges;
    getCoEdges(coedges);

    Topo::CommandSetEdgeMeshingProperty* command = new Topo::CommandSetEdgeMeshingProperty(getLocalContext(), emp, coedges);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << emp.getScriptCommandBegin()<<"\n";
    cmd << getContextAlias() << "." << "getTopoManager().setAllMeshingProperty ("
            <<emp.getScriptCommandRef()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::setParallelMeshingProperty(CoEdgeMeshingProperty& emp, std::string coedge_name)
{
	return setParallelMeshingProperty(emp, getCoEdge(coedge_name));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::setParallelMeshingProperty(CoEdgeMeshingProperty& emp, Topo::CoEdge* coedge)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setMeshingProperty(EdgeMeshingProperty("<<(long)emp.getNbEdges()<<","<<emp.getMeshLawName()
                    <<"),"<<coedge->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetEdgeMeshingPropertyToParallelCoEdges* command = new Topo::CommandSetEdgeMeshingPropertyToParallelCoEdges(getLocalContext(), emp, coedge);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << emp.getScriptCommandBegin()<<"\n";
    cmd << getContextAlias() << "." << "getTopoManager().setParallelMeshingProperty ("
        << emp.getScriptCommandRef()<<",\""<<coedge->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::reverseDirection(std::vector<std::string> &coedges_names)
{
    std::vector<CoEdge*> coedges;
    for (std::vector<std::string>::iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter)
        coedges.push_back(getCoEdge(*iter));

    return reverseDirection(coedges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::reverseDirection(std::vector<CoEdge*>& coedges)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::reverseDirection pour "<<coedges.size()<<" arêtes";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandReverseDirection* command = new Topo::CommandReverseDirection(getLocalContext(), coedges);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().reverseDirection ([\"";
    for (uint i=0; i<coedges.size(); i++){
       	if (i)
       		cmd <<"\", \"";
       	cmd << coedges[i]->getName();
       }
       cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<std::string> &face_names)
{
    std::vector<CoFace*> cofaces;
    for (std::vector<std::string>::iterator iter = face_names.begin();
            iter != face_names.end(); ++iter)
    	cofaces.push_back(getCoFace(*iter));

    return setMeshingProperty(emp, cofaces);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<CoFace*>& cofaces)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setMeshingProperty(EdgeMeshingProperty("<<emp.getMeshLawName()
                    <<"),"<<cofaces.size()<<" faces)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetFaceMeshingProperty* command = new Topo::CommandSetFaceMeshingProperty(getLocalContext(), emp, cofaces);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setMeshingProperty ("
            <<emp.getScriptCommand()<<",[\"";
    for (uint i=0; i<cofaces.size(); i++){
       	if (i)
       		cmd <<"\", \"";
       	cmd << cofaces[i]->getName();
       }
       cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setAllMeshingProperty(CoFaceMeshingProperty& emp)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setAllMeshingProperty(FaceMeshingProperty("<<emp.getMeshLawName()
                    <<"), toutes les faces)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    std::vector<Topo::CoFace* > cofaces;
    getCoFaces(cofaces);

    Topo::CommandSetFaceMeshingProperty* command = new Topo::CommandSetFaceMeshingProperty(getLocalContext(), emp, cofaces);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setAllMeshingProperty ("
            <<emp.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setMeshingProperty(BlockMeshingProperty& emp, std::vector<std::string> &bloc_names)
{
    std::vector<Block*> blocks;
    for (std::vector<std::string>::iterator iter = bloc_names.begin();
            iter != bloc_names.end(); ++iter)
    	blocks.push_back(getBlock(*iter));

    return setMeshingProperty(emp, blocks);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setMeshingProperty(BlockMeshingProperty& emp, std::vector<Block*>& blocks)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setMeshingProperty(EdgeMeshingProperty("<<emp.getMeshLawName()
                    <<"),"<<blocks.size()<<" blocs)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetBlockMeshingProperty* command = new Topo::CommandSetBlockMeshingProperty(getLocalContext(), emp, blocks);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setMeshingProperty ("
            <<emp.getScriptCommand()<<",[\"";
    for (uint i=0; i<blocks.size(); i++){
       	if (i)
       		cmd <<"\", \"";
       	cmd << blocks[i]->getName();
       }
       cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setAllMeshingProperty(BlockMeshingProperty& emp)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setAllMeshingProperty(FaceMeshingProperty("<<emp.getMeshLawName()
                    <<"), toutes les faces)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    std::vector<Topo::Block* > blocks;
    getBlocks(blocks);

    Topo::CommandSetBlockMeshingProperty* command = new Topo::CommandSetBlockMeshingProperty(getLocalContext(), emp, blocks);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setAllMeshingProperty ("
            <<emp.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}


/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, std::string ed)
{
    return setEdgeMeshingProperty(emp, getCoEdge(ed, true));
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty&
TopoManager::getEdgeMeshingProperty(std::string ed)
{
    CoEdgeMeshingProperty* property = getCoEdge(ed, true)->getMeshingProperty ( );
    CHECK_NULL_PTR_ERROR (property)
    return *property;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, CoEdge* ed)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setEdgeMeshingProperty(EdgeMeshingProperty("<<(long)emp.getNbEdges()<<","<<emp.getMeshLawName()
                    <<"),"<<ed->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetEdgeMeshingProperty* command = new Topo::CommandSetEdgeMeshingProperty(getLocalContext(), emp, ed);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << emp.getScriptCommandBegin()<<"\n";
    cmd << getContextAlias() << "." << "getTopoManager().setEdgeMeshingProperty ("
            <<emp.getScriptCommandRef()<<",\""<<ed->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setFaceMeshingProperty(CoFaceMeshingProperty& emp, std::string cf)
{
    return setFaceMeshingProperty(emp, getCoFace(cf, true));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setFaceMeshingProperty(CoFaceMeshingProperty& emp, CoFace* cf)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setFaceMeshingProperty(FaceMeshingProperty("<<emp.getMeshLawName()
                    <<"),"<<cf->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetFaceMeshingProperty* command = new Topo::CommandSetFaceMeshingProperty(getLocalContext(), emp, cf);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setFaceMeshingProperty ("
            <<emp.getScriptCommand()<<",\""<<cf->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setBlockMeshingProperty(BlockMeshingProperty& emp, std::string bl)
{
    return setBlockMeshingProperty(emp, getBlock(bl, true));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setBlockMeshingProperty(BlockMeshingProperty& emp, Block* bl)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setBlockMeshingProperty(BlockMeshingProperty("<<emp.getMeshLawName()
                    <<"),"<<bl->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetBlockMeshingProperty* command = new Topo::CommandSetBlockMeshingProperty(getLocalContext(), emp, bl);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setBlockMeshingProperty ("
            <<emp.getScriptCommand()<<",\""<<bl->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::replaceTransfiniteByDirectionalMeshMethodAsPossible()
{
    Topo::CommandReplaceTransfiniteMethod* command = new Topo::CommandReplaceTransfiniteMethod(getLocalContext());

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().replaceTransfiniteByDirectionalMeshMethodAsPossible()";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::unrefine(std::string nbloc, std::string narete, int ratio)
{
    return unrefine(getBlock(nbloc), getCoEdge(narete), ratio);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::unrefine(Block* bloc, CoEdge* arete, int ratio)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::unrefine("<<bloc->getName()<<","<<arete->getName()<<","<<(short)ratio<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandUnrefineBlock* command = new Topo::CommandUnrefineBlock(getLocalContext(), bloc, arete, ratio);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().unrefine (\""
            <<bloc->getName()<<"\",\""<<arete->getName()<<"\","<<(short)ratio<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::refine(int ratio)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::refine("<<(short)ratio<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandRefineAllCoEdges* command = new Topo::CommandRefineAllCoEdges(getLocalContext(), ratio);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().refine ("<<(short)ratio<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::importMDL(std::string n, const bool all, const bool useAreaName,
		std::string prefixName,int deg_min, int deg_max)
{
#ifdef USE_MDLPARSER
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::importMDL ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // création de l'importateur de modélisation/topo MDL
    Internal::ImportMDLImplementation* impl = new Internal::ImportMDLImplementation(getLocalContext(), n, all,
    		useAreaName, deg_min, deg_max);
    if (!prefixName.empty())
    	impl->setPrefix(prefixName);

    // creation de la commande composite
    Internal::CommandComposite *command =
            new Internal::CommandComposite(getLocalContext(), "Import Mdl (géom et topo)");

    Utils::Unit::lengthUnit luMdl = impl->getLengthUnit();
    Utils::Unit::lengthUnit luCtx = getContext().getLengthUnit();

    if (luCtx == Utils::Unit::undefined){
    	// si l'unité n'est pas définie, on se met dans celle du mdl
    	 Internal::CommandChangeLengthUnit* commandCU =
    			 new Internal::CommandChangeLengthUnit(getLocalContext(), luMdl);
    	command->addCommand(commandCU);
    }
    else if (luMdl != luCtx) {
    	// cas où l'unité est définie, mais différente dans le MDL
    	double factor = Utils::Unit::computeFactor(luMdl, luCtx);
    	impl->setScale(factor);
    }

    //creation de la commande d'importation de la géométrie
    Geom::CommandImportMDL *commandGeom = new Geom::CommandImportMDL(getLocalContext(), impl);
    //creation de la commande d'importation de la topologie
    Topo::CommandImportTopoMDL *commandTopo = new Topo::CommandImportTopoMDL(getLocalContext(), impl);

    command->addCommand(commandGeom);
    command->addCommand(commandTopo);


    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().importMDL(\""<<n<<"\","
            <<(all?"True":"False")
			<<", "
			<<(useAreaName?"True":"False")
			<<", "
			<<"\""<<prefixName<<"\""
			<<", "
			<<(short)deg_min
			<<", "
			<<(short)deg_max
			<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    // on peut détruire l'importateur de MDL
    delete impl;

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("ImportMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER    
}
/*----------------------------------------------------------------------------*/
std::vector<std::vector<Topo::CoEdge*> > TopoManager::
getFusableEdgesObj()
{
    // Récupérations de toutes les faces communes
    std::vector<Topo::CoFace* > all_cofaces;
    getCoFaces(all_cofaces);

    // On ne conserve que les CoFaces dans une topo 2D (non utilisées par une Face)
    std::vector<Topo::CoFace* > cofaces;
    for (std::vector<Topo::CoFace* >::iterator iter = all_cofaces.begin();
            iter != all_cofaces.end(); ++iter)
        if ((*iter)->getNbFaces() == 0)
            cofaces.push_back(*iter);

    // On fait une recherche des groupes de CoEdges partagées par les même CoFaces
    std::vector<std::vector<Topo::CoEdge*> > l_coedges;

    // pour ce faire, on colorie les CoEdges (repérées par l'unique id)
    std::map<uint, uint> coedge_color;
    uint color = 0; // la couleur pour les prochaines coedges vues

    for (std::vector<Topo::CoFace* >::iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){
        Topo::CoFace* coface = *iter1;
        std::vector<Edge* > edges;
        coface->getEdges(edges);

        for (std::vector<Topo::Edge* >::iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){
            Topo::Edge* edge = *iter2;
            std::vector<CoEdge* > coedges;
            edge->getCoEdges(coedges);
            // on change de couleur à chaque nouvelle arête
            color += 1;

            uint prec_color = 0;
            for (std::vector<Topo::CoEdge* >::iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3){
                Topo::CoEdge* coedge = *iter3;

                if (coedge_color[coedge->getUniqueId()] == prec_color) {
                    coedge_color[coedge->getUniqueId()] = color;
                }
                else {
                    if (iter3 != coedges.begin())
                        color += 1;

                    prec_color = coedge_color[coedge->getUniqueId()];
                    coedge_color[coedge->getUniqueId()] = color;
                }
            }
        }
    }

    // après coloration, on recherche les groupes de même couleur
    // on remet à 0 les couleurs des groupes sélectionnés pour éviter de les prendre 2 fois
    for (std::vector<Topo::CoFace* >::iterator iter1 = cofaces.begin();
            iter1 != cofaces.end(); ++iter1){
        Topo::CoFace* coface = *iter1;
        std::vector<Edge* > edges;
        coface->getEdges(edges);

        for (std::vector<Topo::Edge* >::iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){
            Topo::Edge* edge = *iter2;
            std::vector<CoEdge* > coedges;
            edge->getCoEdges(coedges);

            uint prec_color = 0;
            std::vector<CoEdge*> fusesableCoedges;

            for (std::vector<Topo::CoEdge* >::iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3){
                Topo::CoEdge* coedge = *iter3;

                if (coedge_color[coedge->getUniqueId()] == prec_color) {
                    if (prec_color != 0){
                        fusesableCoedges.push_back(coedge);
                        coedge_color[coedge->getUniqueId()] = 0;
                    }
                }
                else {
                    if (iter3 != coedges.begin())
                        color += 1;

                    prec_color = coedge_color[coedge->getUniqueId()];
                    coedge_color[coedge->getUniqueId()] = 0;

                    // on commence une nouvelle liste
                    if (fusesableCoedges.size() > 1)
                        l_coedges.push_back(fusesableCoedges);
                    fusesableCoedges.clear();
                    if (prec_color != 0)
                        fusesableCoedges.push_back(coedge);
                }
            }
            if (fusesableCoedges.size() > 1)
                l_coedges.push_back(fusesableCoedges);
        }
    }

    return l_coedges;
}
/*----------------------------------------------------------------------------*/
std::vector<std::vector<std::string> > TopoManager::
getFusableEdges()
{
    std::vector<std::vector<Topo::CoEdge*> > fusable = getFusableEdgesObj();
    std::vector<std::vector<std::string> > fusableNames;

    for (std::vector<std::vector<Topo::CoEdge*> >::iterator iter1=fusable.begin();
            iter1!=fusable.end(); ++iter1){
        std::vector<std::string> names;
        for (std::vector<Topo::CoEdge*>::iterator iter2=(*iter1).begin();
                iter2!=(*iter1).end(); ++iter2){
            names.push_back((*iter2)->getName());
        }
        fusableNames.push_back(names);
    }
    return fusableNames;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getInvalidEdges() const
{
    std::vector<std::string> invalidEdges;

    // on prend toutes les CoEdges disponibles
    std::vector<Topo::CoEdge* > coedges;
    getCoEdges(coedges);

    for (std::vector<Topo::CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
        try {
            (*iter)->check();
        }
        catch (const TkUtil::Exception& exc)
        {
        	invalidEdges.push_back((*iter)->getName());
        }
    }

    return invalidEdges;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
fuseEdges(std::vector<std::string> &coedges_names)
{
    std::vector<CoEdge*> coedges;
    for (std::vector<std::string>::iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter)
        coedges.push_back(getCoEdge(*iter));
    return fuseEdges(coedges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
fuseEdges(std::vector<CoEdge*> &coedges)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::fuseEdges (";
    for (uint i=0; i<coedges.size(); i++){
        if (i)
            message <<", ";
        message << coedges[i]->getName();
    }
    message << ")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // récupération des courbes sur lesquelles se font les projections
    std::vector<Geom::GeomEntity*> courbes;
    for (uint i=0; i<coedges.size(); i++)
    	if (coedges[i]->getGeomAssociation())
    		courbes.push_back(coedges[i]->getGeomAssociation());

    // si c'est toujours la même courbe alors il n'y a rien à faire au niveau géométrique
    if (courbes.size()){
    	bool diff = false;
    	Geom::GeomEntity* c0 = courbes[0];
    	for (uint i=1; i<courbes.size(); i++)
    		if (courbes[i] != c0)
    			diff = true;
    	if (!diff)
    		courbes.clear();
    }

    Internal::CommandInternal *command = 0;
    if (courbes.size()){
    	// creation de la commande de modification/création de la géométrie
    	Geom::CommandJoinCurves* commandGeom = new Geom::CommandJoinCurves(getLocalContext(), courbes);

    	// création de la commande de modification de la projection des arêtes
    	std::vector<Topo::TopoEntity*> topoEntities;
    	topoEntities.insert(topoEntities.end(), coedges.begin(), coedges.end());
    	Topo::CommandSetGeomAssociation* commandTopo1 = new Topo::CommandSetGeomAssociation(getLocalContext(), topoEntities, commandGeom);

    	// creation de la commande de modification de la topologie
    	Topo::CommandFuseCoEdges* commandTopo2 = new Topo::CommandFuseCoEdges(getLocalContext(), coedges);

    	// creation de la commande composite

    	Internal::CommandComposite* commandCompo = new Internal::CommandComposite(getLocalContext(), "Fusion d'arêtes et des courbes associées");
    	commandCompo->addCommand(commandGeom);
    	commandCompo->addCommand(commandTopo1);
    	commandCompo->addCommand(commandTopo2);
    	command = commandCompo;
    }
    else {
    	// creation de la commande de modification de la topologie
    	command = new Topo::CommandFuseCoEdges(getLocalContext(), coedges);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuseEdges ([\"";
    for (uint i=0; i<coedges.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << coedges[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Blocks(std::string na, std::string nb)
{
    return fuse2Blocks(getBlock(na), getBlock(nb));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::fuse2Blocks(Block* bl_A, Block* bl_B)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::fuse2Blocks("<<bl_A->getName()<<","<<bl_B->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandFuse2Blocks* command = new Topo::CommandFuse2Blocks(getLocalContext(), bl_A, bl_B);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().fuse2Blocks (\""
            <<bl_A->getName()<<"\",\""<<bl_B->getName()<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
setGeomAssociation(std::vector<std::string> & topo_entities_names,
		std::string geom_entity_name,
		bool move_vertices)
{
    std::vector<TopoEntity*> topo_entities;
    TopoManager::getTopoEntities(topo_entities_names, topo_entities, "setGeomAssociation");

    Geom::GeomEntity* geom_entity = 0;
    if (!geom_entity_name.empty())
        geom_entity = getLocalContext().getGeomManager().getEntity(geom_entity_name, true);

    return setGeomAssociation(topo_entities, geom_entity, move_vertices);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
setGeomAssociation(std::vector<TopoEntity*> & topo_entities,
		Geom::GeomEntity* geom_entity,
		bool move_vertices)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::setGeomAssociation ([";
    for (uint i=0; i<topo_entities.size(); i++){
        if (i)
            message <<", ";
        message << topo_entities[i]->getName();
    }
    message << "], "<< (geom_entity?geom_entity->getName():"") <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    // création de la commande
    Topo::CommandSetGeomAssociation* command =
            new Topo::CommandSetGeomAssociation(getLocalContext(), topo_entities, geom_entity, move_vertices);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setGeomAssociation ([\"";
    for (uint i=0; i<topo_entities.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << topo_entities[i]->getName();
    }
    cmd << "\"], \"" << (geom_entity?geom_entity->getName():"") << "\", "<<(move_vertices?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectVerticesOnNearestGeomEntities(std::vector<std::string> & vertices_names,
		std::vector<std::string> & geom_entities_names,
		bool move_vertices)
{
	std::vector<Vertex*> vertices;
	std::vector<Geom::GeomEntity*> geom_entities;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    for (std::vector<std::string>::const_iterator iter = geom_entities_names.begin();
            iter != geom_entities_names.end(); ++iter){
    	Geom::GeomEntity* ge = getLocalContext().getGeomManager().getEntity(*iter, true);
    	geom_entities.push_back(ge);
    }

    return projectVerticesOnNearestGeomEntities(vertices, geom_entities, move_vertices);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectVerticesOnNearestGeomEntities(std::vector<Vertex*> & vertices,
		std::vector<Geom::GeomEntity*> & geom_entities,
		bool move_vertices)
{

    // création de la commande
    Topo::CommandProjectVerticesOnNearestGeomEntities* command =
            new Topo::CommandProjectVerticesOnNearestGeomEntities(getLocalContext(), vertices, geom_entities, move_vertices);


    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().projectVerticesOnNearestGeomEntities ([\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"], [\"";
    for (uint i=0; i<geom_entities.size(); i++){
    	if (i)
    		cmd <<"\", \"";
    	cmd << geom_entities[i]->getName();
    }
    cmd << "\"], "<<(move_vertices?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectEdgesOnCurves(std::vector<std::string> & coedges_names)
{
	std::vector<CoEdge*> coedges;

    for (std::vector<std::string>::const_iterator iter = coedges_names.begin();
            iter != coedges_names.end(); ++iter){
        coedges.push_back(getCoEdge(*iter, true));
    }

    return projectEdgesOnCurves(coedges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectEdgesOnCurves(std::vector<CoEdge*> & coedges)
{
	if (coedges.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Aucune arête sélectionnée pour la projection", TkUtil::Charset::UTF_8));

	// création de la commande
    Topo::CommandProjectEdgesOnCurves* command =
            new Topo::CommandProjectEdgesOnCurves(getLocalContext(), coedges);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().projectEdgesOnCurves ([\"";
    for (uint i=0; i<coedges.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << coedges[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectAllEdgesOnCurves()
{
    // création de la commande
    Topo::CommandProjectEdgesOnCurves* command =
            new Topo::CommandProjectEdgesOnCurves(getLocalContext());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().projectAllEdgesOnCurves ()";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectFacesOnSurfaces(std::vector<std::string> & cofaces_names)
{
	std::vector<CoFace*> cofaces;

    for (std::vector<std::string>::const_iterator iter = cofaces_names.begin();
            iter != cofaces_names.end(); ++iter){
    	cofaces.push_back(getCoFace(*iter, true));
    }

    return projectFacesOnSurfaces(cofaces);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectFacesOnSurfaces(std::vector<CoFace*> & cofaces)
{
	if (cofaces.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Aucune face sélectionnée pour la projection", TkUtil::Charset::UTF_8));

	// création de la commande
    Topo::CommandProjectFacesOnSurfaces* command =
            new Topo::CommandProjectFacesOnSurfaces(getLocalContext(), cofaces);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().projectFacesOnSurfaces ([\"";
    for (uint i=0; i<cofaces.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << cofaces[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
projectAllFacesOnSurfaces()
{
    // création de la commande
    Topo::CommandProjectFacesOnSurfaces* command =
            new Topo::CommandProjectFacesOnSurfaces(getLocalContext());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().projectAllFacesOnSurfaces ()";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
translate(std::vector<std::string>& ve, const Vector& dp,
        const bool withGeom)
{
    std::vector<TopoEntity*> entities;
    TopoManager::getTopoEntities(ve, entities, "translate");

      return translate(entities, dp, withGeom);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
translate(std::vector<TopoEntity*>& ve, const Vector& dp,
        const bool withGeom)
{
    if (ve.empty()){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionnée pour la translation", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::translate ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i]->getName();
    }
    message << "], "<< dp ;
    message <<(withGeom?", avec":", sans")<<" translation de la géométrie)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Internal::CommandInternal* command = 0;

    if (withGeom){

        Internal::CommandComposite* commandCompo =
                new Internal::CommandComposite(getLocalContext(), "Translation d'une topologie avec sa géométrie");

        std::vector<Geom::GeomEntity*> geomEntities = Topo::TopoHelper::getGeomEntities(ve);

        Geom::CommandTranslation *commandGeom = new Geom::CommandTranslation(getLocalContext(), geomEntities, dp);
        commandCompo->addCommand(commandGeom);

        Topo::CommandTranslateTopo* commandTopo = new Topo::CommandTranslateTopo(getLocalContext(), ve, dp);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new Topo::CommandTranslateTopo(getLocalContext(), ve, dp);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().translate ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i]->getName();
    }
    cmd << "\"], " << dp.getScriptCommand() << ", "<<(withGeom?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
rotate(std::vector<std::string>& ve,
		const Utils::Math::Rotation& rot,
		const bool withGeom)
{
	std::vector<TopoEntity*> entities;
	TopoManager::getTopoEntities(ve, entities, "rotate");

	return rotate(entities, rot, withGeom);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::
rotate(std::vector<TopoEntity*>& ve,
		const Utils::Math::Rotation& rot,
		const bool withGeom)
{
    if (ve.empty()){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la rotation", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::rotate ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i]->getName();
    }
    message << "], "<<  rot;
    message <<(withGeom?", avec":", sans")<<" rotation de la géométrie)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Internal::CommandInternal* command = 0;

    if (withGeom){

        Internal::CommandComposite* commandCompo =
                new Internal::CommandComposite(getLocalContext(), "Rotation d'une topologie avec sa géométrie");

        // constitution des entités géométriques à modifier
        std::vector<Geom::GeomEntity*> geomEntities = Topo::TopoHelper::getGeomEntities(ve);

        Geom::CommandRotation* commandGeom =
                new Geom::CommandRotation(getLocalContext(), geomEntities, rot);
        Topo::CommandRotateTopo* commandTopo =
                new Topo::CommandRotateTopo(getLocalContext(), ve, rot);

        commandCompo->addCommand(commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new Topo::CommandRotateTopo(getLocalContext(), ve,  rot);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().rotate ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i]->getName();
    }
    cmd << "\"], " << rot.getScriptCommand() << ", "<<(withGeom?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::scale(std::vector<std::string>& ve,
        const double& facteur,
        const bool withGeom)
{
    std::vector<TopoEntity*> entities;
    TopoManager::getTopoEntities(ve, entities, "scale");

    return scale(entities, facteur, Utils::Math::Point(0,0,0), withGeom);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::scale(std::vector<std::string>& ve,
        const double& facteur,
        const Utils::Math::Point& pcentre,
        const bool withGeom)
{
    std::vector<TopoEntity*> entities;
    TopoManager::getTopoEntities(ve, entities, "scale");

    return scale(entities, facteur, pcentre, withGeom);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::scale(std::vector<TopoEntity*>& ve,
        const double& facteur,
        const Point& pcentre,
        const bool withGeom)
{
    if (ve.empty()){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour l'homothétie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::scale ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i]->getName();
    }
    message << "], "<< Utils::Math::MgxNumeric::userRepresentation (facteur) ;
    message <<(withGeom?", avec":", sans")<<" homothétie de la géométrie)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Internal::CommandInternal* command = 0;

    if (withGeom){

        Internal::CommandComposite* commandCompo =
                new Internal::CommandComposite(getLocalContext(), "Homothétie d'une topologie avec sa géométrie");

        // constitution des entités géométriques à modifier
        std::vector<Geom::GeomEntity*> geomEntities = Topo::TopoHelper::getGeomEntities(ve);

        Geom::CommandScaling* commandGeom = new Geom::CommandScaling(getLocalContext(), geomEntities, facteur, pcentre);
        Topo::CommandScaleTopo* commandTopo = new Topo::CommandScaleTopo(getLocalContext(), ve, facteur, pcentre);

        commandCompo->addCommand(commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new Topo::CommandScaleTopo(getLocalContext(), ve, facteur, pcentre);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().scale ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i]->getName();
    }
    cmd << "\"], " << Utils::Math::MgxNumeric::userRepresentation (facteur);
    if (!(pcentre == Utils::Math::Point(0,0,0)))
    	cmd <<", "<<pcentre.getScriptCommand();
    cmd<<", "<<(withGeom?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::scale(std::vector<std::string>& ve,
		const double factorX,
		const double factorY,
		const double factorZ,
        const bool withGeom)
{
    std::vector<TopoEntity*> entities;
    TopoManager::getTopoEntities(ve, entities, "scale");

    return scale(entities, factorX, factorY, factorZ, withGeom);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::scale(std::vector<TopoEntity*>& ve,
		const double factorX,
		const double factorY,
		const double factorZ,
        const bool withGeom)
{
    if (ve.empty()){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour l'homothétie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::scale ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i]->getName();
    }
    message << "], "<< factorX<<", "<<factorY<<", "<<factorZ;
    message <<(withGeom?", avec":", sans")<<" homothétie de la géométrie)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Internal::CommandInternal* command = 0;

    if (withGeom){

        Internal::CommandComposite* commandCompo =
                new Internal::CommandComposite(getLocalContext(), "Homothétie d'une topologie avec sa géométrie");

        // constitution des entités géométriques à modifier
        std::vector<Geom::GeomEntity*> geomEntities = Topo::TopoHelper::getGeomEntities(ve);

        Geom::CommandScaling* commandGeom = new Geom::CommandScaling(getLocalContext(), geomEntities, factorX, factorY, factorZ);
        Topo::CommandScaleTopo* commandTopo = new Topo::CommandScaleTopo(getLocalContext(), ve, factorX, factorY, factorZ);

        commandCompo->addCommand(commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new Topo::CommandScaleTopo(getLocalContext(), ve, factorX, factorY, factorZ);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().scale ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i]->getName();
    }
    cmd << "\"], " << Utils::Math::MgxNumeric::userRepresentation (factorX)
        <<", "<<Utils::Math::MgxNumeric::userRepresentation (factorY)
        <<", "<<Utils::Math::MgxNumeric::userRepresentation (factorZ);
    cmd<<", "<<(withGeom?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::mirror(std::vector<std::string>& ve,
    		Utils::Math::Plane* plane,
    		const bool withGeom)
{
    std::vector<TopoEntity*> entities;
    TopoManager::getTopoEntities(ve, entities, "mirror");

    return mirror(entities, plane, withGeom);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
TopoManager::mirror(std::vector<TopoEntity*>& ve,
			Utils::Math::Plane* plane,
    		const bool withGeom)
{
    if (ve.empty()){
    	throw TkUtil::Exception (TkUtil::UTF8String ("Aucune entité sélectionné pour la ymétrie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::mirror ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i]->getName();
    }
    message << "], "<<  *plane;
    message <<(withGeom?", avec":", sans")<<" symétrie de la géométrie)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Internal::CommandInternal* command = 0;

    if (withGeom){

        Internal::CommandComposite* commandCompo =
                new Internal::CommandComposite(getLocalContext(), "Symétrie d'une topologie avec sa géométrie");

        // constitution des entités géométriques à modifier
        std::vector<Geom::GeomEntity*> geomEntities = Topo::TopoHelper::getGeomEntities(ve);

        Geom::CommandMirroring* commandGeom =
                new Geom::CommandMirroring(getLocalContext(), geomEntities, plane);
        Topo::CommandMirrorTopo* commandTopo =
                new Topo::CommandMirrorTopo(getLocalContext(), ve, plane);

        commandCompo->addCommand(commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new Topo::CommandMirrorTopo(getLocalContext(), ve,  plane);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().mirror ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i]->getName();
    }
    cmd << "\"], " << plane->getScriptCommand() << ", "<<(withGeom?"True":"False") <<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexLocation(std::vector<std::string>& vertices_names,
        const bool changeX,
        const double& xPos,
        const bool changeY,
        const double& yPos,
        const bool changeZ,
        const double& zPos,
		std::string sysCoordName)
{
    std::vector<Vertex*> vertices;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    if (vertices.empty()){
    	TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    	message <<"setVertexLocation n'a pas de sommet";
    	throw TkUtil::Exception(message);
    }

    CoordinateSystem::SysCoord* rep = 0;
    if (!sysCoordName.empty())
    	rep = getLocalContext().getSysCoordManager().getSysCoord(sysCoordName, true);

    return setVertexLocation(vertices, changeX, xPos, changeY, yPos, changeZ, zPos, rep);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexLocation(std::vector<Vertex*>& vertices,
        const bool changeX,
        const double& xPos,
        const bool changeY,
        const double& yPos,
        const bool changeZ,
        const double& zPos,
		CoordinateSystem::SysCoord* rep)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::setVertexLocation ([";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            message <<", ";
        message << vertices[i]->getName();
    }
    message << "]";
    if (changeX)
        message << ", modifie x : "<<Utils::Math::MgxNumeric::userRepresentation (xPos);
    if (changeY)
        message << ", modifie y : "<<Utils::Math::MgxNumeric::userRepresentation (yPos);
    if (changeZ)
        message << ", modifie z : "<<Utils::Math::MgxNumeric::userRepresentation (zPos);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandChangeVerticesLocation* command =
            new Topo::CommandChangeVerticesLocation(getLocalContext(),
            		vertices, changeX, xPos, changeY, yPos, changeZ, zPos,
					CommandChangeVerticesLocation::cartesian, rep);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setVertexLocation ([\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"]";
    cmd << (changeX?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (xPos);
    cmd << (changeY?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (yPos);
    cmd << (changeZ?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (zPos);
    if (rep)
    	cmd<<", \""<<rep->getName()<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexSphericalLocation(std::vector<std::string>& vertices_names,
		const bool changeRho,
		const double& rhoPos,
		const bool changeTheta,
		const double& thetaPos,
		const bool changePhi,
		const double& phiPos,
		std::string sysCoordName)
{
    std::vector<Vertex*> vertices;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    if (vertices.empty()){
    	TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    	message <<"setVertexSphericalLocation n'a pas de sommet";
    	throw TkUtil::Exception(message);
    }

    CoordinateSystem::SysCoord* rep = 0;
    if (!sysCoordName.empty())
    	rep = getLocalContext().getSysCoordManager().getSysCoord(sysCoordName, true);

    return setVertexSphericalLocation(vertices, changeRho, rhoPos, changeTheta, thetaPos, changePhi, phiPos, rep);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexSphericalLocation(std::vector<Vertex*>& vertices,
		const bool changeRho,
		const double& rhoPos,
		const bool changeTheta,
		const double& thetaPos,
		const bool changePhi,
		const double& phiPos,
		CoordinateSystem::SysCoord* rep)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::setVertexSphericalLocation ([";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            message <<", ";
        message << vertices[i]->getName();
    }
    message << "]";
    if (changeRho)
        message << ", modifie Rho : "<<Utils::Math::MgxNumeric::userRepresentation (rhoPos);
    if (changeTheta)
        message << ", modifie Theta : "<<Utils::Math::MgxNumeric::userRepresentation (thetaPos);
    if (changePhi)
        message << ", modifie Phi : "<<Utils::Math::MgxNumeric::userRepresentation (phiPos);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandChangeVerticesLocation* command =
            new Topo::CommandChangeVerticesLocation(getLocalContext(),
            		vertices, changeRho, rhoPos, changeTheta, thetaPos, changePhi, phiPos,
					CommandChangeVerticesLocation::spherical, rep);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setVertexSphericalLocation ([\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"]";
    cmd << (changeRho?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (rhoPos);
    cmd << (changeTheta?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (thetaPos);
    cmd << (changePhi?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (phiPos);
    if (rep)
    	cmd<<", \""<<rep->getName()<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexCylindricalLocation(std::vector<std::string>& vertices_names,
        const bool changeRho,
        const double& rhoPos,
        const bool changePhi,
        const double& phiPos,
		const bool changeZ,
		const double& zPos,
		std::string sysCoordName)
{
    std::vector<Vertex*> vertices;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    if (vertices.empty()){
    	TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    	message <<"setVertexCylindricalLocation n'a pas de sommet";
    	throw TkUtil::Exception(message);
    }

    CoordinateSystem::SysCoord* rep = 0;
    if (!sysCoordName.empty())
    	rep = getLocalContext().getSysCoordManager().getSysCoord(sysCoordName, true);

    return setVertexCylindricalLocation(vertices, changeRho, rhoPos, changePhi, phiPos, changeZ, zPos, rep);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexCylindricalLocation(std::vector<Vertex*>& vertices,
		const bool changeRho,
		        const double& rhoPos,
		        const bool changePhi,
		        const double& phiPos,
				const bool changeZ,
				const double& zPos,
				CoordinateSystem::SysCoord* rep)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::setVertexCylindricalLocation ([";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            message <<", ";
        message << vertices[i]->getName();
    }
    message << "]";
    if (changeRho)
        message << ", modifie Rho : "<<Utils::Math::MgxNumeric::userRepresentation (rhoPos);
    if (changePhi)
        message << ", modifie Phi : "<<Utils::Math::MgxNumeric::userRepresentation (phiPos);
    if (changeZ)
        message << ", modifie z : "<<Utils::Math::MgxNumeric::userRepresentation (zPos);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandChangeVerticesLocation* command =
            new Topo::CommandChangeVerticesLocation(getLocalContext(),
            		vertices, changeRho, rhoPos, changePhi, phiPos, changeZ, zPos,
					CommandChangeVerticesLocation::cylindrical, rep);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setVertexCylindricalLocation ([\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"]";
    cmd << (changeRho?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (rhoPos);
    cmd << (changePhi?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (phiPos);
    cmd << (changeZ?", True, ":", False, ") << Utils::Math::MgxNumeric::userRepresentation (zPos);
    if (rep)
    	cmd<<", \""<<rep->getName()<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexSameLocation(std::string vertex_name,
		std::string target_name)
{
	Vertex* vtx = TopoManager::getVertex (vertex_name, true);
	Vertex* target = TopoManager::getVertex (target_name, true);
	return setVertexSameLocation(vtx, target);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setVertexSameLocation(Vertex* vtx,
		Vertex* target)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::setVertexSameLocation ("<<vtx->getName()<<", "<<target->getName()<<")";

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandChangeVertexSameLocation* command =
            new Topo::CommandChangeVertexSameLocation(getLocalContext(), vtx, target);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setVertexSameLocation (\""<<vtx->getName()<<"\", \""<<target->getName()<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapProjectedVertices(std::vector<std::string>& vertices_names)
{
    std::vector<Vertex*> vertices;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    return snapProjectedVertices(vertices);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapProjectedVertices(std::vector<Vertex*>& vertices)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::snapProjectedVertices ([";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            message <<", ";
        message << vertices[i]->getName();
    }
    message << "]";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSnapProjectedVertices* command =
            new Topo::CommandSnapProjectedVertices(getLocalContext(), vertices);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().snapProjectedVertices ([\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::snapAllProjectedVertices()
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::snapAllProjectedVertices ()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSnapProjectedVertices* command =
            new Topo::CommandSnapProjectedVertices(getLocalContext());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().snapAllProjectedVertices ()";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::alignVertices(std::vector<std::string>& vertices_names)
{
    std::vector<Vertex*> vertices;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    return alignVertices(vertices);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::alignVertices(std::vector<Vertex*>& vertices)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::alignVertices ([";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            message <<", ";
        message << vertices[i]->getName();
    }
    message << "]";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandAlignVertices* command =
            new Topo::CommandAlignVertices(getLocalContext(), vertices);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().alignVertices ([\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::alignVertices(
		const Point& p1, const Point& p2,
		std::vector<std::string>& vertices_names)
{
    std::vector<Vertex*> vertices;

    for (std::vector<std::string>::const_iterator iter = vertices_names.begin();
            iter != vertices_names.end(); ++iter){
        Vertex* vtx = TopoManager::getVertex (*iter, true);
        vertices.push_back(vtx);
    }

    return alignVertices(p1, p2, vertices);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::alignVertices(
		const Point& p1, const Point& p2,
		std::vector<Vertex*>& vertices)
{
    Topo::CommandAlignVertices* command =
            new Topo::CommandAlignVertices(getLocalContext(), p1, p2, vertices);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().alignVertices ("
    		<< p1.getScriptCommand()<<", "
    		<< p2.getScriptCommand()<<", [\"";
    for (uint i=0; i<vertices.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << vertices[i]->getName();
    }
    cmd << "\"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo TopoManager::getInfos(const std::string& name, int dim) const
{
    switch(dim){
    case(3):
	{
    	Block* bl = TopoManager::getBlock (name, false);
    	if (bl)
    		return bl->getInfos();
    	else {
    		TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    		message <<"getInfos impossible, entité \""<<name<<"\" n'a pas été trouvée dans le TopoManager";
    		throw TkUtil::Exception(message);
    	}
	}
    break;
    case(2):
    {
        CoFace* cf = 0;
        Face* fa = 0;
        if ((cf = TopoManager::getCoFace (name, false)) != 0)
            return cf->getInfos();
        else if ((fa = TopoManager::getFace (name, false)) != 0)
            return fa->getInfos();
        else {
            TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
            message <<"getInfos impossible, entité \""<<name<<"\" n'a pas été trouvée dans le TopoManager";
            throw TkUtil::Exception(message);
        }
    }
    break;
    case(1):
    {
        CoEdge* ce = 0;
        Edge* ed = 0;
        if ((ce = TopoManager::getCoEdge (name, false)) != 0)
            return ce->getInfos();
        else if ((ed = TopoManager::getEdge (name, false)) != 0)
            return ed->getInfos();
        else {
            TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
            message <<"getInfos impossible, entité \""<<name<<"\" n'a pas été trouvée dans le TopoManager";
            throw TkUtil::Exception(message);
        }
    }
    break;
    case(0):
	{
    	Vertex* vtx = TopoManager::getVertex (name, false);
    	if (vtx)
    		return vtx->getInfos();
    	else {
    		TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    		message <<"getInfos impossible, entité \""<<name<<"\" n'a pas été trouvée dans le TopoManager";
    		throw TkUtil::Exception(message);
    	}
	}
    break;
    default:
        throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour TopoManager::getInfos", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
std::string TopoManager::getDirOnBlock(const std::string& aname, const std::string& bname) const
{
    Block* bloc = TopoManager::getBlock (bname, true);
    CoEdge* coedge = TopoManager::getCoEdge (aname, true);

    Block::eDirOnBlock dir = bloc->getDir(coedge);

    return Block::toString(dir);
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point TopoManager::getCoord(const std::string& name) const
{
	Vertex* vtx = TopoManager::getVertex (name, true);
	return vtx->getCoord();
}
/*----------------------------------------------------------------------------*/
void TopoManager::getTopoEntities(std::vector<std::string> & topo_entities_names,
         std::vector<TopoEntity*> & topo_entities,
         const char* nom_fonction)
{
    Block* bl = 0;
    CoFace* cf = 0;
    CoEdge* ce = 0;
    Vertex* ve = 0;

    for (std::vector<std::string>::iterator iter = topo_entities_names.begin();
            iter != topo_entities_names.end(); ++iter){
        std::string& name = *iter;
        if ((bl = TopoManager::getBlock (name, false)) != 0)
            topo_entities.push_back(bl);
        else if ((cf = TopoManager::getCoFace (name, false)) != 0)
            topo_entities.push_back(cf);
        else if ((ce = TopoManager::getCoEdge (name, false)) != 0)
            topo_entities.push_back(ce);
        else if ((ve = TopoManager::getVertex (name, false)) != 0)
            topo_entities.push_back(ve);
        else {
            TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
            message <<nom_fonction<<" avec \""<<name<<"\" ne peut se faire, l'entité n'a pas été trouvée";
            throw TkUtil::Exception(message);
        }
    }
}
/*----------------------------------------------------------------------------*/
int TopoManager::getNbMeshingEdges(const std::string& name) const
{
	CoEdge* coedge = getCoEdge(name, false);
	if (coedge)
		return (int)coedge->getNbMeshingEdges();
	else
		return 0;
}
/*----------------------------------------------------------------------------*/
int TopoManager::getDefaultNbMeshingEdges()
{
    return m_defaultNbMeshingEdges;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setDefaultNbMeshingEdges(int nb)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setDefaultNbMeshingEdges("<<(unsigned long)nb<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandChangeDefaultNbMeshingEdges* command =
            new Topo::CommandChangeDefaultNbMeshingEdges(getLocalContext(), nb);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setDefaultNbMeshingEdges ("<<(unsigned long)nb<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getBorderFaces() const
{
    std::vector<std::string> borderFaces;

    // on prend toutes les CoFaces disponibles
    std::vector<Topo::CoFace* > cofaces;
    getCoFaces(cofaces);

    for (std::vector<Topo::CoFace* >::iterator iter = cofaces.begin();
    		iter != cofaces.end(); ++iter)
    	if ((*iter)->getNbBlocks() == 1)
    		borderFaces.push_back((*iter)->getName());

    return borderFaces;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getFacesWithoutBlock() const
{
    std::vector<std::string> facesWB;

    // on prend toutes les CoFaces disponibles
    std::vector<Topo::CoFace* > cofaces;
    getCoFaces(cofaces);

    for (std::vector<Topo::CoFace* >::iterator iter = cofaces.begin();
    		iter != cofaces.end(); ++iter)
    	if ((*iter)->getNbBlocks() == 0)
    		facesWB.push_back((*iter)->getName());

    return facesWB;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getSemiConformalFaces() const
{
	std::vector<Topo::CoFace* > cofaces;
	getCoFaces(cofaces);

	// recherches parmis ces cofaces celles qui sont reliées à deux faces et pour lesquelles
	// il y a une semi-conformité
	std::vector<std::string> semiConf;
	for (uint i=0; i<cofaces.size(); i++){
		CoFace* coface = cofaces[i];
		bool isSemiConf = false;
		if (coface->getNbFaces() == 2 && coface->isStructured())
			for (uint j=0; j<2; j++){
				Face* face = coface->getFace(j);
				for (uint dir=0; dir<2; dir++)
					if (face->getRatio(coface,dir) != 1)
						isSemiConf = true;
			} // end for j
		if (isSemiConf)
			semiConf.push_back(coface->getName());
	} // end for i

	return semiConf;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getInvalidFaces() const
{
    std::vector<std::string> invalidFaces;

    // on prend toutes les CoFaces disponibles
    std::vector<Topo::CoFace* > cofaces;
    getCoFaces(cofaces);

    for (std::vector<Topo::CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter){
        try {
            (*iter)->check();
        }
        catch (const TkUtil::Exception& exc)
        {
            invalidFaces.push_back((*iter)->getName());
        }
    }

    return invalidFaces;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getUnstructuredFaces() const
{
    std::vector<std::string> unstructuredFaces;

    // on prend toutes les CoFaces disponibles
    std::vector<Topo::CoFace* > cofaces;
    getCoFaces(cofaces);

    for (std::vector<Topo::CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter){
        if (!(*iter)->isStructured())
        	unstructuredFaces.push_back((*iter)->getName());
    }

    return unstructuredFaces;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getTransfiniteMeshLawFaces() const
{
    std::vector<std::string> selectedFaces;

    // on prend toutes les CoFaces disponibles
    std::vector<Topo::CoFace* > cofaces;
    getCoFaces(cofaces);

    for (std::vector<Topo::CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter){
        if ((*iter)->getMeshLaw() == CoFaceMeshingProperty::transfinite)
        	selectedFaces.push_back((*iter)->getName());
    }

    return selectedFaces;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getInvalidBlocks() const
{
    std::vector<std::string> invalidBlocks;

    // on prend tous les blocs disponibles
    std::vector<Topo::Block* > blocs;
    getBlocks(blocs);

    for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter){
        try {
            (*iter)->check();
        }
        catch (const TkUtil::Exception& exc)
        {
        	invalidBlocks.push_back((*iter)->getName());
        }
    }

    return invalidBlocks;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getVisibleBlocks() const
{
    std::vector<std::string> visibleBlocks;

    std::vector<Topo::Block* > blocs;
    getBlocks(blocs);

    for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter){
    	if ((*iter)->isVisible())
    		visibleBlocks.push_back((*iter)->getName());
    }

    return visibleBlocks;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getUnstructuredBlocks() const
{
    std::vector<std::string> unstructuredBlocks;

    std::vector<Topo::Block* > blocs;
    getBlocks(blocs);

    for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter){
    	if (!(*iter)->isStructured())
    		unstructuredBlocks.push_back((*iter)->getName());
    }

    return unstructuredBlocks;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getTransfiniteMeshLawBlocks() const
{
    std::vector<std::string> selectedBlocks;

    std::vector<Topo::Block* > blocs;
    getBlocks(blocs);

    for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter){
    	if ((*iter)->getMeshLaw() == BlockMeshingProperty::transfinite)
    		selectedBlocks.push_back((*iter)->getName());
    }

    return selectedBlocks;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setNbMeshingEdges(std::string edge_name, int nb, std::vector<std::string>& frozed_edges_names)
{
    CoEdge* coedge = getCoEdge(edge_name);
    std::vector<CoEdge*> frozed_coedges;
    for (uint i=0; i<frozed_edges_names.size(); i++)
        frozed_coedges.push_back(getCoEdge(frozed_edges_names[i]));

    return setNbMeshingEdges(coedge, nb, frozed_coedges);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* TopoManager::setNbMeshingEdges(CoEdge* coedge, int nb, std::vector<CoEdge*>& frozed_coedges)
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message <<"TopoManager::setNbMeshingEdges("<<coedge->getName()<<", "<<(unsigned long)nb<<", [";
    for (uint i=0; i<frozed_coedges.size(); i++){
        if (i)
            message <<", ";
        message << frozed_coedges[i]->getName();
    }
    message <<"])";

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));

    Topo::CommandSetNbMeshingEdges* command = new Topo::CommandSetNbMeshingEdges(getLocalContext(), coedge, nb, frozed_coedges);

    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setNbMeshingEdges (\""
            <<coedge->getName()<<"\", "<<(unsigned long)nb<<", [";
    for (uint i=0; i<frozed_coedges.size(); i++){
        if (i)
            cmd <<", ";
        cmd << "\""<<frozed_coedges[i]->getName()<<"\"";
    }
    cmd <<"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::addToGroup ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i];
    }
    message << "], "<<(short)dim<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));


    Mesh::CommandAddRemoveGroupName* command = 0;

    switch(dim){
    case(0):{
        // reconstitue le vecteur de sommets
        std::vector<Vertex*> vertices;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            vertices.push_back(getVertex(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), vertices, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    case(1):{
        // reconstitue le vecteur de coedges
        std::vector<CoEdge*> coedges;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	coedges.push_back(getCoEdge(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), coedges, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    case(2):{
        // reconstitue le vecteur de surfaces
        std::vector<CoFace*> cofaces;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	cofaces.push_back(getCoFace(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), cofaces, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    case(3):{
        // reconstitue le vecteur de blocs
        std::vector<Block*> blocks;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	blocks.push_back(getBlock(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), blocks, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    default:{
        TkUtil::Exception ("Dimension erronée");
    }
    break;
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().addToGroup ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i];
    }
    cmd << "\"], "<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::removeFromGroup ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i];
    }
    message << "], "<<(short)dim<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));


    Mesh::CommandAddRemoveGroupName* command = 0;

    switch(dim){
    case(0):{
        // reconstitue le vecteur de sommets
        std::vector<Vertex*> vertices;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            vertices.push_back(getVertex(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), vertices, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    case(1):{
        // reconstitue le vecteur de coedges
        std::vector<CoEdge*> coedges;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	coedges.push_back(getCoEdge(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), coedges, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    case(2):{
        // reconstitue le vecteur de cofaces
        std::vector<CoFace*> cofaces;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	cofaces.push_back(getCoFace(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), cofaces, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    case(3):{
        // reconstitue le vecteur de blocks
        std::vector<Block*> blocks;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	blocks.push_back(getBlock(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), blocks, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    default:{
        TkUtil::Exception ("Dimension erronée");
    }
    break;
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().removeFromGroup ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i];
    }
    cmd << "\"], "<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* TopoManager::setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{

    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "TopoManager::setGroup ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i];
    }
    message << "], "<<(short)dim<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));


    Mesh::CommandAddRemoveGroupName* command = 0;

    switch(dim){
    case(0):{
        // reconstitue le vecteur de sommets
        std::vector<Vertex*> vertices;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            vertices.push_back(getVertex(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), vertices, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    case(1):{
        // reconstitue le vecteur de coedges
        std::vector<CoEdge*> coedges;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	coedges.push_back(getCoEdge(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), coedges, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    case(2):{
        // reconstitue le vecteur de cofaces
        std::vector<CoFace*> cofaces;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	cofaces.push_back(getCoFace(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), cofaces, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    case(3):{
        // reconstitue le vecteur de blocks
        std::vector<Block*> blocks;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
        	blocks.push_back(getBlock(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), blocks, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    default:{
        TkUtil::Exception ("Dimension erronée");
    }
    break;
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getTopoManager().setGroup ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i];
    }
    cmd << "\"], "<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoManager::getCommonEdges(const std::string& face1, const std::string& face2, int dim) const
{
	std::vector<std::string> listeCoEdges;
	std::vector<CoEdge*> coedges;
	switch(dim){
	case(2):{
		CoFace* coface1 = getCoFace(face1, true);
		CoFace* coface2 = getCoFace(face2, true);
		coedges = TopoHelper::getCommonCoEdges(coface1, coface2);
	}
	break;
	case(3):{
		Block* bloc1 = getBlock(face1, true);
		Block* bloc2 = getBlock(face2, true);
		coedges = TopoHelper::getCommonCoEdges(bloc1, bloc2);
	}
	break;
	default:{
		TkUtil::Exception ("Dimension erronée");
	}
	}
	for (uint i=0; i<coedges.size(); i++)
		listeCoEdges.push_back(coedges[i]->getName());
	return listeCoEdges;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
