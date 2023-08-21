/*----------------------------------------------------------------------------*/
/** \file Block.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string.h>
/*----------------------------------------------------------------------------*/
#include "Topo/TopoHelper.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/Block.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/CommandEditTopo.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
#include "Topo/FaceMeshingPropertyRotational.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"
#include "Topo/FaceMeshingPropertyDelaunayGMSH.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Topo/BlockMeshingPropertyOrthogonal.h"
#include "Topo/BlockMeshingPropertyRotational.h"
#include "Topo/BlockMeshingPropertyTransfinite.h"
#include "Topo/BlockMeshingPropertyDelaunayTetgen.h"
#ifdef USE_MESHGEMS
#include "Topo/BlockMeshingPropertyDelaunayMeshGems.h"
#endif	// USE_MESHGEMS

#include "Mesh/CommandCreateMesh.h"

#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Internal/InternalPreferences.h"

#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/Common.h"
#include "Utils/SerializedRepresentation.h"

#include "Geom/GeomEntity.h"

#include "Group/Group3D.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/Timer.h>
/*----------------------------------------------------------------------------*/
//#define _DEBUG_TIMER
//#define _DEBUG_SPLIT
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/

/// tableau des faces concernées par la coupe suivant la direction de cette dernière
static const uint tabIndFaceParDir[3][4] = {
        {2,4,3,5},
        {0,4,1,5},
        {0,2,1,3}};

/*----------------------------------------------------------------------------*/
const char* Block::typeNameTopoBlock = "TopoBlock";
/*----------------------------------------------------------------------------*/
std::string Block::toString(eFaceOnBlock ind)
{
    switch (ind){
    case i_min: return "i_min";
    case i_max: return "i_max";
    case j_min: return "j_min";
    case j_max: return "j_max";
    case k_min: return "k_min";
    case k_max: return "k_max";
    }
    return "inconnu";
}
/*----------------------------------------------------------------------------*/
std::string Block::toString(eDirOnBlock ind)
{
    switch (ind){
    case i_dir: return "i_dir";
    case j_dir: return "j_dir";
    case k_dir: return "k_dir";
    }
    return "inconnu";
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
Block::
Block(Internal::Context& ctx,
      std::vector<Face* > &faces,
      std::vector<Topo::Vertex* > &vertices,
      bool isStructured)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoBlock),
        ctx.newDisplayProperties(Utils::Entity::TopoBlock))
, m_topo_property(new BlockTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(new BlockMeshingData())
, m_save_mesh_data(0)
{
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "Block::Block(), dans le cas quelconque avec "<<faces.size()<<" faces";
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));

    if (isStructured){
        m_mesh_property = new BlockMeshingPropertyTransfinite();
    }
#ifdef USE_MESHGEMS
    else {
        //Changement de Netgen a Tetgen pour la valeur par defaut F. Ledoux 30/10/13
    	// Changement de Tetgen à MeshGems pour la valeur par défaut, EB le 3/5/2019
        m_mesh_property = new BlockMeshingPropertyDelaunayMeshGems();
    }
#else
    else
		m_mesh_property = new BlockMeshingPropertyDelaunayTetgen ( );   // Or Triton ???
#endif	// USE_MESHGEMS

#ifdef _DEBUG_MEMORY
    std::cout<<"Block::Block() avec les faces :";
    for (uint i=0; i<faces.size(); i++)
        std::cout<<" "<<(0!=faces[i]?faces[i]->getName():"(vide)");
    std::cout<<std::endl;
    std::cout<<"et avec les sommets :";
    for (uint i=0; i<vertices.size(); i++)
        std::cout<<" "<<(0!=vertices[i]?vertices[i]->getName():"(vide)");
    std::cout<<std::endl;
    for (uint i=0; i<vertices.size(); i++)
    	std::cout<<" "<<*vertices[i]<<std::endl;
#endif

    for (uint i=0; i<faces.size(); i++)
        if (0 == faces[i]){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            messErr << "La création d'un bloc ne peut se faire, la face d'indice "<<(short)i
                    <<" est nulle";
            throw TkUtil::Exception(messErr);
        }
    for (uint i=0; i<vertices.size(); i++)
        if (0 == vertices[i]){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            messErr << "La création d'un bloc ne peut se faire, le sommet d'indice "<<(short)i
                    <<" est nul";
            throw TkUtil::Exception(messErr);
        }

    m_topo_property->getFaceContainer().add(faces);
    m_topo_property->getVertexContainer().add(vertices);

    _init();
}
/*----------------------------------------------------------------------------*/
Block::
Block(Internal::Context& ctx, int ni, int nj, int nk,
        BlockMeshingProperty::meshLaw ml,
        BlockMeshingProperty::meshDirLaw md)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoBlock),
        ctx.newDisplayProperties(Utils::Entity::TopoBlock))
, m_topo_property(new BlockTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(new BlockMeshingData())
, m_save_mesh_data(0)
{
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "Block::Block(), dans le cas structuré\n";
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));

    // NB, il manque l'axe de rotation pour construire un BlockMeshingPropertyRotational
    if (ml == BlockMeshingProperty::directional)
        m_mesh_property =new BlockMeshingPropertyDirectional(md);
    else
        m_mesh_property =new BlockMeshingPropertyTransfinite();

    // bloc avec les sommets équivalents à ceux d'une boite de taille 1

    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(0,0,0)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(1,0,0)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(0,1,0)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(1,1,0)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(0,0,1)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(1,0,1)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(0,1,1)));
    m_topo_property->getVertexContainer().add(new Topo::Vertex(ctx, Utils::Math::Point(1,1,1)));

    // les arêtes (communes) dont on ne conserve pas de lien dans le bloc
    std::vector<CoEdge* > coedges;

    // une discretisation par défaut (découpage uniforme suivant le nombre de bras par défaut)
    // à moins qu'un nombre de bras ait été demandé
    EdgeMeshingPropertyUniform empI(ni?ni:ctx.getLocalTopoManager().getDefaultNbMeshingEdges());
    EdgeMeshingPropertyUniform empJ(nj?nj:ctx.getLocalTopoManager().getDefaultNbMeshingEdges());
    EdgeMeshingPropertyUniform empK(nk?nk:ctx.getLocalTopoManager().getDefaultNbMeshingEdges());

    coedges.push_back(new Topo::CoEdge(ctx, &empI, getVertex(0), getVertex(1)));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, getVertex(2), getVertex(3)));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, getVertex(4), getVertex(5)));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, getVertex(6), getVertex(7)));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, getVertex(0), getVertex(2)));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, getVertex(1), getVertex(3)));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, getVertex(4), getVertex(6)));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, getVertex(5), getVertex(7)));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, getVertex(0), getVertex(4)));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, getVertex(1), getVertex(5)));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, getVertex(2), getVertex(6)));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, getVertex(3), getVertex(7)));

    // création des Edges qui utilisent les CoEdges
    std::vector<Edge* > edges;
    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
        edges.push_back(new Edge(ctx, *iter));
    }

    // création des 6 faces
    CoFace* coface;

    coface = new Topo::CoFace(ctx, edges[4], edges[8], edges[6], edges[10]);
    m_topo_property->getFaceContainer().add(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[5], edges[9], edges[7], edges[11]);
    m_topo_property->getFaceContainer().add(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[0], edges[8], edges[2], edges[9]);
    m_topo_property->getFaceContainer().add(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[1], edges[10], edges[3], edges[11]);
    m_topo_property->getFaceContainer().add(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[0], edges[4], edges[1], edges[5]);
    m_topo_property->getFaceContainer().add(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[2], edges[6], edges[3], edges[7]);
    m_topo_property->getFaceContainer().add(new Topo::Face(ctx, coface));

    _init();

//    TkUtil::UTF8String   message2 (Charset::UTF_8);
//    message1 << "Block créé:\n";
//    message1 << *this;
//    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
}
/*----------------------------------------------------------------------------*/
void Block::
_init()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"Block::_init() de : "<<*this <<std::endl;
#endif

    // association remontante
    for (uint i=0; i<getNbFaces(); i++)
        getFace(i)->addBlock(this);
	getContext ( ).newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
Block::
Block(const Block& f)
: TopoEntity(f.getContext(), 0, 0)
, m_topo_property(0)
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(0)
, m_save_mesh_data(0)
{
    MGX_NOT_YET_IMPLEMENTED("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
Block::
~Block()
{
#ifdef _DEBUG
    if (m_topo_property == 0 || m_mesh_property == 0 || m_mesh_data == 0)
    	std::cerr<<"Serait-on passé deux fois dans le destructeur ? pour "<<getName()<<std::endl;
#endif

    delete m_topo_property;
    delete m_mesh_property;
    delete m_mesh_data;

#ifdef _DEBUG
    m_topo_property = 0;
    m_mesh_property = 0;
    m_mesh_data = 0;

    if (m_save_topo_property)
    	std::cerr<<"La sauvegarde du BlockTopoProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_property)
    	std::cerr<<"La sauvegarde du BlockMeshingProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_data)
    	std::cerr<<"La sauvegarde du BlockMeshingData a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* Block::
getVertex(bool cote_i, bool cote_j, bool cote_k)
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::getVertex(bool,bool,bool) n'est pas possible avec un bloc non structuré", TkUtil::Charset::UTF_8));

    uint indice = (cote_i?1:0) + (cote_j?2:0) + (cote_k?4:0);
    Topo::Vertex* vtx = getVertex(indice);
#ifdef _DEBUG2
    std::cout<<"Block::getVertex("<<(cote_i?"true":"false")<<", "<<(cote_j?"true":"false")<<", "<<(cote_k?"true":"false")<<") => "<<vtx->getName()<<std::endl;
#endif
    return vtx;

}
/*----------------------------------------------------------------------------*/
void Block::
getCoFaces(std::vector<CoFace* >& cofaces) const
{
    cofaces.clear();
    std::list<Topo::CoFace*> l_cf;
    for(uint i=0; i <getNbFaces();i++) {
        Face* face = getFace(i);
        const std::vector<CoFace* > & local_cofaces =
                face->getCoFaces();
        l_cf.insert(l_cf.end(), local_cofaces.begin(), local_cofaces.end());
    }
    l_cf.sort(Utils::Entity::compareEntity);
    l_cf.unique();

    cofaces.insert(cofaces.end(),l_cf.begin(),l_cf.end());
}
/*----------------------------------------------------------------------------*/
uint Block::
getNbCoFaces() const
{
    std::vector<CoFace* > cofaces;
    getCoFaces(cofaces);
    return cofaces.size();
}
/*----------------------------------------------------------------------------*/
void Block::
getEdges(std::vector<Edge* >& edges) const
{
    edges.clear();
    std::list<Topo::Edge*> l_e;
    for(uint i=0; i <getNbFaces();i++) {
        Face* face = getFace(i);
        for(uint j=0; j < face->getNbCoFaces();j++) {
            const std::vector<Edge* > & local_edges =
                    face->getCoFace(j)->getEdges();
            l_e.insert(l_e.end(), local_edges.begin(), local_edges.end());
        }
    }
    l_e.sort(Utils::Entity::compareEntity);
    l_e.unique();

    edges.insert(edges.end(),l_e.begin(),l_e.end());
}
/*----------------------------------------------------------------------------*/
uint Block::
getNbEdges() const
{
    std::vector<Edge* > edges;
    getEdges(edges);
    return edges.size();
}
/*----------------------------------------------------------------------------*/
void Block::getAllVertices(std::vector<Vertex* >& vertices, bool unique) const
{
    vertices.clear();
    if (unique){
    	std::list<Topo::Vertex*> l_v;
    	for(uint i=0; i <getNbFaces();i++) {
    		Face* face = getFace(i);
    		for(uint j=0; j < face->getNbCoFaces();j++) {
    			CoFace* coface = face->getCoFace(j);
    			for(uint k=0; k < coface->getNbEdges();k++) {
    				Edge* edge = coface->getEdge(k);
    				for(uint l=0; l < edge->getNbCoEdges(); l++){
    					CoEdge* coedge = edge->getCoEdge(l);

    					const std::vector<Vertex* > & local_vertices = coedge->getVertices();

    					l_v.insert(l_v.end(), local_vertices.begin(), local_vertices.end());
    				}
    			}
    		}
    	}

    	l_v.sort(Utils::Entity::compareEntity);
    	l_v.unique();

    	vertices.insert(vertices.end(),l_v.begin(),l_v.end());
    }
    else {
    	for(uint i=0; i <getNbFaces();i++) {
    		Face* face = getFace(i);
    		for(uint j=0; j < face->getNbCoFaces();j++) {
    			CoFace* coface = face->getCoFace(j);
    			for(uint k=0; k < coface->getNbEdges();k++) {
    				Edge* edge = coface->getEdge(k);
    				for(uint l=0; l < edge->getNbCoEdges(); l++){
    					CoEdge* coedge = edge->getCoEdge(l);

    					const std::vector<Vertex* > & local_vertices = coedge->getVertices();

    					vertices.insert(vertices.end(), local_vertices.begin(), local_vertices.end());
    				}
    			}
    		}
    	}
    }
}
/*----------------------------------------------------------------------------*/
void Block::getCoEdges(std::vector<CoEdge* >& coedges, bool unique) const
{
	if (unique){
		coedges.clear();
		std::list<Topo::CoEdge*> l_e;
		for(uint i=0; i <getNbFaces();i++) {
			Face* face = getFace(i);
			for(uint j=0; j < face->getNbCoFaces();j++) {
				CoFace* coface = face->getCoFace(j);
				for(uint k=0; k < coface->getNbEdges();k++) {
					const std::vector<CoEdge* > & local_coedges =
							coface->getEdge(k)->getCoEdges();
					l_e.insert(l_e.end(), local_coedges.begin(), local_coedges.end());
				}
			}
		}
		l_e.sort(Utils::Entity::compareEntity);
		l_e.unique();

		coedges.insert(coedges.end(),l_e.begin(),l_e.end());
	}
	else {
		coedges.clear();
		for(uint i=0; i <getNbFaces();i++) {
			Face* face = getFace(i);
			for(uint j=0; j < face->getNbCoFaces();j++) {
				CoFace* coface = face->getCoFace(j);
				for(uint k=0; k < coface->getNbEdges();k++) {
					const std::vector<CoEdge* > & local_coedges =
							coface->getEdge(k)->getCoEdges();
					coedges.insert(coedges.end(), local_coedges.begin(), local_coedges.end());
				}
			}
		}
	}

}
/*----------------------------------------------------------------------------*/
uint Block::
getNbCoEdges() const
{
    std::vector<CoEdge* > coedges;
    getCoEdges(coedges);
    return coedges.size();
}
/*----------------------------------------------------------------------------*/
void Block::
replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd)
{
    Topo::Vertex* v_tmp;

    for (uint i=0; i<getNbVertices(); i++)
        if (v1 == getVertex(i)){
            v_tmp = getVertex(i);
            saveBlockTopoProperty(icmd);
            m_topo_property->getVertexContainer().set(i, v2);
        }
    if (propagate_down)
    	for (uint j=0; j<getNbFaces(); j++)
    		getFace(j)->replace(v1, v2, propagate_up, propagate_down, icmd);

}
/*----------------------------------------------------------------------------*/
void Block::
replace(Face* f1, Face* f2, Internal::InfoCommand* icmd)
{
    for (uint i=0; i<getNbFaces(); i++)
        if (f1 == getFace(i)){
            saveBlockTopoProperty(icmd);
            m_topo_property->getFaceContainer().set(i, f2);

            f1->saveFaceTopoProperty(icmd);
            f2->saveFaceTopoProperty(icmd);
            f1->removeBlock(this);
            f2->addBlock(this);
        }
}
/*----------------------------------------------------------------------------*/
void Block::
replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd)
{
    for (uint i=0; i<getNbFaces(); i++)
    	getFace(i)->replace(e1, e2, icmd);
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_REPRESENTATION
void Block::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_TOPO)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

    if (isDestroyed() && (true == checkDestroyed)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Interdit d'afficher une entité détruite ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }

    TopoDisplayRepresentation* tdr =
            dynamic_cast<TopoDisplayRepresentation*>(&dr);
    CHECK_NULL_PTR_ERROR(tdr);
	double shrink = tdr->getShrink();

#ifdef _DEBUG_REPRESENTATION
    std::cout<<"getRepresentation pour "<<getName()<<" shrink = "<<shrink<<std::endl;
#endif

    std::vector<Utils::Math::Point>& points = tdr->getPoints();
    std::vector<size_t>& indicesFilaire = tdr->getCurveDiscretization();
    std::vector<size_t>& indicesSurf = tdr->getSurfaceDiscretization();

    // recherche du barycentre
    Utils::Math::Point barycentre = getBarycentre();

    points.clear();

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE)){
        indicesFilaire.clear();

        if (getNbVertices() == 8 && isStructured()){

            for (uint i=0; i<getNbVertices(); i++)
                points.push_back(barycentre + (getVertex(i)->getCoord() -  barycentre) * shrink);

            // construction des 12 arêtes en se servant du modèle de construction du bloc
            indicesFilaire.push_back(0);
            indicesFilaire.push_back(1);

            indicesFilaire.push_back(2);
            indicesFilaire.push_back(3);

            indicesFilaire.push_back(4);
            indicesFilaire.push_back(5);

            indicesFilaire.push_back(6);
            indicesFilaire.push_back(7);

            indicesFilaire.push_back(0);
            indicesFilaire.push_back(2);

            indicesFilaire.push_back(1);
            indicesFilaire.push_back(3);

            indicesFilaire.push_back(4);
            indicesFilaire.push_back(6);

            indicesFilaire.push_back(5);
            indicesFilaire.push_back(7);

            indicesFilaire.push_back(0);
            indicesFilaire.push_back(4);

            indicesFilaire.push_back(1);
            indicesFilaire.push_back(5);

            indicesFilaire.push_back(2);
            indicesFilaire.push_back(6);

            indicesFilaire.push_back(3);
            indicesFilaire.push_back(7);
        }
        else {
            // utilisation de toutes les arêtes et construction de la représentation filaire
            std::vector<Topo::Edge* > edges;
            getEdges(edges);

            for (std::vector<Topo::Edge* >::iterator iter = edges.begin();
                        iter != edges.end(); ++iter) {
                Topo::Edge* edge = *iter;
                if (edge->getNbVertices() == 2){
                    for (uint i=0; i<2; i++){
                        indicesFilaire.push_back(points.size());
                        points.push_back(barycentre + (edge->getVertex(i)->getCoord() -  barycentre) * shrink);
                    }
                }
            } // end for iter = edges.begin()
        } // end else if (getNbVertices() == 8)
    }
    else if (tdr->hasRepresentation(Utils::DisplayRepresentation::SOLID)) {

        // représentation à l'aide de 2 triangles par côté
        if (getNbVertices() == 8 && isStructured()){
            for (uint i=0; i<getNbVertices(); i++)
                points.push_back(barycentre + (getVertex(i)->getCoord() -  barycentre) * shrink);

            indicesSurf.push_back(0);
            indicesSurf.push_back(1);
            indicesSurf.push_back(5);

            indicesSurf.push_back(5);
            indicesSurf.push_back(4);
            indicesSurf.push_back(0);

            indicesSurf.push_back(4);
            indicesSurf.push_back(5);
            indicesSurf.push_back(7);

            indicesSurf.push_back(7);
            indicesSurf.push_back(6);
            indicesSurf.push_back(4);

            indicesSurf.push_back(6);
            indicesSurf.push_back(7);
            indicesSurf.push_back(3);

            indicesSurf.push_back(3);
            indicesSurf.push_back(2);
            indicesSurf.push_back(6);

            indicesSurf.push_back(2);
            indicesSurf.push_back(3);
            indicesSurf.push_back(1);

            indicesSurf.push_back(1);
            indicesSurf.push_back(0);
            indicesSurf.push_back(2);

            indicesSurf.push_back(0);
            indicesSurf.push_back(4);
            indicesSurf.push_back(6);

            indicesSurf.push_back(6);
            indicesSurf.push_back(2);
            indicesSurf.push_back(0);

            indicesSurf.push_back(5);
            indicesSurf.push_back(1);
            indicesSurf.push_back(3);

            indicesSurf.push_back(3);
            indicesSurf.push_back(7);
            indicesSurf.push_back(5);
        }
        else {
            // utilisation d'une représentation de chacune des faces (sans shrink)
            // que l'on reporte dans celle du block.
            for (uint i=0; i<getNbFaces(); i++){

                TopoDisplayRepresentation   tr (Utils::DisplayRepresentation::SOLID);
                tr.setShrink(1.0);
                getFace(i)->getRepresentation (tr, checkDestroyed);

                std::vector<Utils::Math::Point>& face_points = tr.getPoints();
                std::vector<size_t>& face_indicesSurf = tr.getSurfaceDiscretization();

                points.insert(points.end(), face_points.begin(), face_points.end());
                indicesSurf.insert(indicesSurf.end(), face_indicesSurf.begin(), face_indicesSurf.end());
            }

            // on applique ensuite un shrink pour l'ensemble des sommets
            for (uint i=0; i<points.size(); i++)
                points[i] = (barycentre + (points[i] -  barycentre) * shrink);
        }
    }

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWDISCRETISATIONTYPE)){
#ifdef _DEBUG_REPRESENTATION
    std::cout<<" cas SHOWDISCRETISATIONTYPE pour "<<getName()<<std::endl;
    std::cout<<*this<<std::endl;
#endif

    	std::vector<size_t>& indices = tdr->getCurveDiscretization();

    	// un point de départ pour la représentation
    	Utils::Math::Point orig = (barycentre + (getVertex(0)->getCoord() -  barycentre) * shrink);

    	// trois vecteurs pour orienter et placer dans l'espace la représentation
    	Utils::Math::Vector vect1;
    	Utils::Math::Vector vect2;
       	Utils::Math::Vector vect3;

       	// un ratio pour normaliser la représentation (basé sur la plus petite longueur suivant les 3 vecteurs)
    	double ratio = 1.0;

    	if (isStructured()){

    		std::vector<CoEdge* > iCoedges;
    		std::vector<CoEdge* > jCoedges;
    		std::vector<CoEdge* > kCoedges;
    		getOrientedCoEdges(iCoedges, jCoedges, kCoedges);
    		std::vector<CoEdge* > coedges_dirI;
    		std::vector<CoEdge* > coedges_dirJ;
    		std::vector<CoEdge* > coedges_dirK;
    		TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(1), iCoedges, coedges_dirI);
    		TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(2), jCoedges, coedges_dirJ);
    		TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(4), kCoedges, coedges_dirK);

    		// on va prendre la coedge1 et vect1 qui suivent le sens de la discrétisation (s'il y en a un)
    		CoEdge* coedge1=0;
    		CoEdge* coedge2=0;
    		CoEdge* coedge3=0;

    		if (m_mesh_property->getMeshLaw() == BlockMeshingProperty::directional
    				|| m_mesh_property->getMeshLaw() == BlockMeshingProperty::orthogonal){
    			BlockMeshingPropertyDirectional* mp = dynamic_cast<BlockMeshingPropertyDirectional*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			if (mp->getDir() == 1){
    				coedge1 = coedges_dirJ[0];
    				coedge2 = coedges_dirI[0];
    				coedge3 = coedges_dirK[0];
    			}
    			else if (mp->getDir() == 2){
    				coedge1 = coedges_dirK[0];
    				coedge2 = coedges_dirJ[0];
    				coedge3 = coedges_dirI[0];
    			}
    			else {
    				coedge1 = coedges_dirI[0];
    				coedge2 = coedges_dirJ[0];
    				coedge3 = coedges_dirK[0];
    			}
    		}
    		else if (m_mesh_property->getMeshLaw() == BlockMeshingProperty::rotational){
    			BlockMeshingPropertyRotational* mp = dynamic_cast<BlockMeshingPropertyRotational*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			if (mp->getDir() == 1){
    				coedge1 = coedges_dirJ[0];
    				coedge2 = coedges_dirI[0];
    				coedge3 = coedges_dirK[0];
    			}
    			else if (mp->getDir() == 2){
    				coedge1 = coedges_dirK[0];
    				coedge2 = coedges_dirJ[0];
    				coedge3 = coedges_dirI[0];
    			}
    			else {
    				coedge1 = coedges_dirI[0];
    				coedge2 = coedges_dirJ[0];
    				coedge3 = coedges_dirK[0];
    			}
    		}
    		else {
    			coedge1 = coedges_dirI[0];
    			coedge2 = coedges_dirJ[0];
    			coedge3 = coedges_dirK[0];
    		}


    		std::vector<Utils::Math::Point> points_coedge1;
    		std::vector<Utils::Math::Point> points_coedge2;
       		std::vector<Utils::Math::Point> points_coedge3;
    		coedge1->getPoints(points_coedge1);
    		coedge2->getPoints(points_coedge2);
    		coedge3->getPoints(points_coedge3);
#ifdef _DEBUG_REPRESENTATION
    std::cout<<"  coedge1 : "<<*coedge1;
    std::cout<<"    points_coedge1[0] = "<<points_coedge1[0]<<", points_coedge1[1] = "<<points_coedge1[1]
             <<", points_coedge1["<<points_coedge1.size()-2<<"] = "<<points_coedge1[points_coedge1.size()-2]
             <<", points_coedge1["<<points_coedge1.size()-1<<"] = "<<points_coedge1[points_coedge1.size()-1]
             <<std::endl;
    std::cout<<"  coedge2 : "<<*coedge2;
    std::cout<<"    points_coedge2[0] = "<<points_coedge2[0]<<", points_coedge2[1] = "<<points_coedge2[1]
             <<", points_coedge2["<<points_coedge2.size()-2<<"] = "<<points_coedge2[points_coedge2.size()-2]
             <<", points_coedge2["<<points_coedge2.size()-1<<"] = "<<points_coedge2[points_coedge2.size()-1]
             <<std::endl;
    std::cout<<"  coedge3 : "<<*coedge3;
    std::cout<<"    points_coedge3[0] = "<<points_coedge3[0]<<", points_coedge3[1] = "<<points_coedge3[1]
             <<", points_coedge3["<<points_coedge3.size()-2<<"] = "<<points_coedge3[points_coedge3.size()-2]
             <<", points_coedge3["<<points_coedge3.size()-1<<"] = "<<points_coedge3[points_coedge3.size()-1]
             <<std::endl;
#endif

    		if (coedge1->getVertex(0) == getVertex(0))
    			vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    		else if (coedge1->getVertex(1) == getVertex(0))
    			vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    					points_coedge1[points_coedge1.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<coedge1->getName();
    			throw TkUtil::Exception(messErr);
    		}

    		if (coedge2->getVertex(0) == getVertex(0))
    			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
    		else if (coedge2->getVertex(1) == getVertex(0))
    			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
    					points_coedge2[points_coedge2.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<coedge2->getName();
    			throw TkUtil::Exception(messErr);
    		}

    		if (coedge3->getVertex(0) == getVertex(0))
    			vect3 = Utils::Math::Vector(points_coedge3[0], points_coedge3[1]);
    		else if (coedge3->getVertex(1) == getVertex(0))
    			vect3 = Utils::Math::Vector(points_coedge3[points_coedge3.size()-1],
    					points_coedge3[points_coedge3.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<coedge3->getName();
    			throw TkUtil::Exception(messErr);
    		}

    		double lg1 = 0;
    		double lg2 = 0;
    		double lg3 = 0;
    		for (uint i=0; i<points_coedge1.size()-1; i++)
    			lg1 += (points_coedge1[i+1]-points_coedge1[i]).norme();
    		for (uint i=0; i<points_coedge2.size()-1; i++)
    			lg2 += (points_coedge2[i+1]-points_coedge2[i]).norme();
    		for (uint i=0; i<points_coedge3.size()-1; i++)
    			lg3 += (points_coedge3[i+1]-points_coedge3[i]).norme();

    		if (lg1<lg2)
    			ratio = lg1;
    		else
    			ratio = lg2;
    		if (lg3<ratio)
    			ratio = lg3;
    		ratio /= 7.0;

    		// normalisation des 3 vecteurs
    		vect1 /= vect1.norme();
    		vect2 /= vect2.norme();
    		vect3 /= vect3.norme();

#ifdef _DEBUG_REPRESENTATION
    		std::cout<<" ratio : "<<ratio<<std::endl;
    		std::cout<<" vect1 : "<<vect1<<std::endl;
    		std::cout<<" vect2 : "<<vect2<<std::endl;
    		std::cout<<" vect3 : "<<vect3<<std::endl;
#endif
    		// décalages pour faire apparaitre la courbure, si nécessaire
    		double dec2 = 0.0;
    		double dec3 = 0.0;
    		if (m_mesh_property->getMeshLaw() == BlockMeshingProperty::rotational){
    			BlockMeshingPropertyRotational* mp = dynamic_cast<BlockMeshingPropertyRotational*>(m_mesh_property);
    			CHECK_NULL_PTR_ERROR(mp);
    			Utils::Math::Point axis1, axis2;
    			mp->getAxis(axis1, axis2);

    			// on cherche à minimiser la distance entre l'axe et l'un des points
    			// avec l'une des options pour la direction
    			Utils::Math::Point option1 = (vect1*0.5+vect2*(0.5+0.1)+vect3*(0.5))*ratio+orig;
    			Utils::Math::Point option2 = (vect1*0.5+vect2*(0.5-0.1)+vect3*(0.5))*ratio+orig;
    			Utils::Math::Point option3 = (vect1*0.5+vect2*(0.5)+vect3*(0.5+0.1))*ratio+orig;
    			Utils::Math::Point option4 = (vect1*0.5+vect2*(0.5)+vect3*(0.5-0.1))*ratio+orig;

    			uint best_option = 1;
    			double dist = (axis1-option1).norme2();

    			if ((axis1-option2).norme2()<dist){
    				dist = (axis1-option2).norme2();
    				best_option = 2;
    			}
    			if ((axis1-option3).norme2()<dist){
    				dist = (axis1-option3).norme2();
    				best_option = 3;
    			}
    			if ((axis1-option4).norme2()<dist){
    				dist = (axis1-option4).norme2();
    				best_option = 4;
    			}

    			if (best_option == 1)
    				dec2 = +0.1;
    			else if (best_option == 2)
    				dec2 = -0.1;
    			else if (best_option == 3)
    				dec3 = +0.1;
    			else if (best_option == 4)
    				dec3 = -0.1;
    		}

    		// on dessine dans le repère vect1 vect2 vect3
    		points.push_back(vect1*0.4+vect2*0.4+vect3*0.2);
    		points.push_back(vect1*0.8+vect2*0.4+vect3*0.2);
    		points.push_back(vect1*0.4+vect2*0.8+vect3*0.2);
    		points.push_back(vect1*0.8+vect2*0.8+vect3*0.2);

    		points.push_back(vect1*0.4+vect2*0.4+vect3*1.0);
    		points.push_back(vect1*0.8+vect2*0.4+vect3*1.0);
    		points.push_back(vect1*0.4+vect2*0.8+vect3*1.0);
    		points.push_back(vect1*0.8+vect2*0.8+vect3*1.0);

    		points.push_back(vect1*0.4+vect2*0.2+vect3*0.4);
    		points.push_back(vect1*0.8+vect2*0.2+vect3*0.4);
    		points.push_back(vect1*0.4+vect2*0.2+vect3*0.8);
    		points.push_back(vect1*0.8+vect2*0.2+vect3*0.8);

    		points.push_back(vect1*0.4+vect2*1.0+vect3*0.4);
    		points.push_back(vect1*0.8+vect2*1.0+vect3*0.4);
    		points.push_back(vect1*0.4+vect2*1.0+vect3*0.8);
    		points.push_back(vect1*0.8+vect2*1.0+vect3*0.8);


    		points.push_back(vect1*0.2+vect2*(0.4+dec2)+vect3*(0.4+dec3));
    		points.push_back(vect1*0.4+vect2*0.4+vect3*0.4);
    		points.push_back(vect1*0.8+vect2*0.4+vect3*0.4);
    		points.push_back(vect1*1.0+vect2*(0.4+dec2)+vect3*(0.4+dec3));

    		points.push_back(vect1*0.2+vect2*(0.4+dec2)+vect3*(0.8+dec3));
    		points.push_back(vect1*0.4+vect2*0.4+vect3*0.8);
    		points.push_back(vect1*0.8+vect2*0.4+vect3*0.8);
    		points.push_back(vect1*1.0+vect2*(0.4+dec2)+vect3*(0.8+dec3));

    		points.push_back(vect1*0.2+vect2*(0.8+dec2)+vect3*(0.4+dec3));
    		points.push_back(vect1*0.4+vect2*0.8+vect3*0.4);
    		points.push_back(vect1*0.8+vect2*0.8+vect3*0.4);
    		points.push_back(vect1*1.0+vect2*(0.8+dec2)+vect3*(0.4+dec3));

    		points.push_back(vect1*0.2+vect2*(0.8+dec2)+vect3*(0.8+dec3));
    		points.push_back(vect1*0.4+vect2*0.8+vect3*0.8);
    		points.push_back(vect1*0.8+vect2*0.8+vect3*0.8);
    		points.push_back(vect1*1.0+vect2*(0.8+dec2)+vect3*(0.8+dec3));


    		indices.push_back(0); indices.push_back(17);
    		indices.push_back(1); indices.push_back(18);
    		indices.push_back(2); indices.push_back(25);
    		indices.push_back(3); indices.push_back(26);

    		indices.push_back(4); indices.push_back(21);
    		indices.push_back(5); indices.push_back(22);
    		indices.push_back(6); indices.push_back(29);
    		indices.push_back(7); indices.push_back(30);

    		indices.push_back(17); indices.push_back(21);
    		indices.push_back(18); indices.push_back(22);
    		indices.push_back(25); indices.push_back(29);
    		indices.push_back(26); indices.push_back(30);

    		indices.push_back(8); indices.push_back(17);
       		indices.push_back(9); indices.push_back(18);
       		indices.push_back(12); indices.push_back(25);
       		indices.push_back(13); indices.push_back(26);
    		indices.push_back(10); indices.push_back(21);
    		indices.push_back(11); indices.push_back(22);
    		indices.push_back(14); indices.push_back(29);
    		indices.push_back(15); indices.push_back(30);

       		indices.push_back(17); indices.push_back(25);
       		indices.push_back(18); indices.push_back(26);
    		indices.push_back(21); indices.push_back(29);
    		indices.push_back(22); indices.push_back(30);


    		indices.push_back(16); indices.push_back(17);
    		indices.push_back(17); indices.push_back(18);
    		indices.push_back(18); indices.push_back(19);

    		indices.push_back(20); indices.push_back(21);
    		indices.push_back(21); indices.push_back(22);
    		indices.push_back(22); indices.push_back(23);

    		indices.push_back(24); indices.push_back(25);
    		indices.push_back(25); indices.push_back(26);
    		indices.push_back(26); indices.push_back(27);

    		indices.push_back(28); indices.push_back(29);
    		indices.push_back(29); indices.push_back(30);
    		indices.push_back(30); indices.push_back(31);


    		// ajout des flêches
    		if (m_mesh_property->getMeshLaw() == BlockMeshingProperty::directional
    				|| m_mesh_property->getMeshLaw() == BlockMeshingProperty::orthogonal){
    			points.push_back(vect1*1.1+vect2*0.2+vect3*0.4);
    			points.push_back(vect1*1.1+vect2*0.6+vect3*0.4);
    			points.push_back(vect1*1.1+vect2*0.4+vect3*0.2);
    			points.push_back(vect1*1.1+vect2*0.4+vect3*0.6);
    			points.push_back(vect1*1.3+vect2*0.4+vect3*0.4);
    		}
    		else if (m_mesh_property->getMeshLaw() == BlockMeshingProperty::rotational){
    			if (dec2>0.0){
    				points.push_back(vect1*1.2+vect2*0.5+vect3*0.4);
    				points.push_back(vect1*1.0+vect2*0.7+vect3*0.4);
    				points.push_back(vect1*1.1+vect2*0.6+vect3*0.2);
    				points.push_back(vect1*1.1+vect2*0.6+vect3*0.6);
    				points.push_back(vect1*1.2+vect2*0.7+vect3*0.4);
    			} else if (dec2<0.0){
    				points.push_back(vect1*1.2+vect2*0.3+vect3*0.4);
    				points.push_back(vect1*1.0+vect2*0.1+vect3*0.4);
    				points.push_back(vect1*1.1+vect2*0.2+vect3*0.2);
    				points.push_back(vect1*1.1+vect2*0.2+vect3*0.6);
    				points.push_back(vect1*1.2+vect2*0.1+vect3*0.4);
    			} else if (dec3>0.0){
    				points.push_back(vect1*1.2+vect3*0.5+vect2*0.4);
    				points.push_back(vect1*1.0+vect3*0.7+vect2*0.4);
    				points.push_back(vect1*1.1+vect3*0.6+vect2*0.2);
    				points.push_back(vect1*1.1+vect3*0.6+vect2*0.6);
    				points.push_back(vect1*1.2+vect3*0.7+vect2*0.4);
    			} else if (dec3<0.0){
    				points.push_back(vect1*1.2+vect3*0.3+vect2*0.4);
    				points.push_back(vect1*1.0+vect3*0.1+vect2*0.4);
    				points.push_back(vect1*1.1+vect3*0.2+vect2*0.2);
    				points.push_back(vect1*1.1+vect3*0.2+vect2*0.6);
    				points.push_back(vect1*1.2+vect3*0.1+vect2*0.4);
    			}
    		}
    		if (points.size() > 32){
    			indices.push_back(19); indices.push_back(36);
    			indices.push_back(32); indices.push_back(36);
    			indices.push_back(33); indices.push_back(36);
    			indices.push_back(34); indices.push_back(36);
    			indices.push_back(35); indices.push_back(36);
    		}
    		if (m_mesh_property->getMeshLaw() == BlockMeshingProperty::orthogonal){
    			uint id = points.size();
    			points.push_back(vect1*0.5+vect2*0.4+vect3*0.4);
    			points.push_back(vect1*0.4+vect2*0.5+vect3*0.4);
    			points.push_back(vect1*0.4+vect2*0.4+vect3*0.5);
    			points.push_back(vect1*0.5+vect2*0.5+vect3*0.4);
    			points.push_back(vect1*0.4+vect2*0.5+vect3*0.5);
    			points.push_back(vect1*0.5+vect2*0.4+vect3*0.5);
    			//points.push_back(vect1*0.5+vect2*0.5+vect3*0.5);
    			indices.push_back(id+0); indices.push_back(id+3);
    			indices.push_back(id+1); indices.push_back(id+3);
    			indices.push_back(id+1); indices.push_back(id+4);
    			indices.push_back(id+2); indices.push_back(id+4);
    			indices.push_back(id+0); indices.push_back(id+5);
    			indices.push_back(id+2); indices.push_back(id+5);
//    			indices.push_back(id+3); indices.push_back(id+6);
//    			indices.push_back(id+4); indices.push_back(id+6);
//    			indices.push_back(id+5); indices.push_back(id+6);
    		}

    	} // end if (isStructured())
    	else {

    		// recherche de 3 vecteurs
    		std::vector<CoEdge* > coedges;
    		getCoEdges(coedges);
    		// on cherche les coedges reliées au sommet 0 du bloc
    		std::vector<CoEdge* > coedges_som0;
    		for (uint i=0; i<coedges.size(); i++)
    			if (coedges[i]->find(getVertex(0)))
    				coedges_som0.push_back(coedges[i]);

    		if (coedges_som0.size() == 1){
    			CoEdge* coedge1 = coedges_som0[0];
    			std::vector<Utils::Math::Point> points_coedge1;
    			coedge1->getPoints(coedge1->getMeshingProperty(), points_coedge1, true);
    			vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			vect2 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    			    						points_coedge1[points_coedge1.size()-2]);
    			vect3 = vect1*vect2;
    			double lg1 = 0;
    			for (uint i=0; i<points_coedge1.size()-1; i++)
    				lg1 += (points_coedge1[i+1]-points_coedge1[i]).norme();
    			ratio =  lg1/7.0;

    		} else if (coedges_som0.size() == 2){
    			CoEdge* coedge1 = coedges_som0[0];
    			CoEdge* coedge2 = coedges_som0[1];

    			std::vector<Utils::Math::Point> points_coedge1;
    			std::vector<Utils::Math::Point> points_coedge2;
    			coedge1->getPoints(coedge1->getMeshingProperty(), points_coedge1, true);
    			coedge2->getPoints(coedge2->getMeshingProperty(), points_coedge2, true);
    			if (coedge1->getVertex(0) == getVertex(0))
    				vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			else if (coedge1->getVertex(1) == getVertex(0))
    				vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    						points_coedge1[points_coedge1.size()-2]);
    			else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Erreur interne pour l'affichage de la discrétisation ("
    						<<getName()<<"), pb avec arête "<<coedge1->getName();
    				throw TkUtil::Exception(messErr);
    			}

        		if (coedge2->getVertex(0) == getVertex(0))
        			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
        		else if (coedge2->getVertex(1) == getVertex(0))
        			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
        					points_coedge2[points_coedge2.size()-2]);
        		else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        			messErr << "Erreur interne pour l'affichage de la discrétisation ("
        					<<getName()<<"), pb avec arête "<<coedge2->getName();
        			throw TkUtil::Exception(messErr);
        		}

        		// cas cycliques
        		if (coedge1->getVertex(0) == coedge1->getVertex(1)){
        			vect3 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
        			    						points_coedge1[points_coedge1.size()-2]);
        		} else if (coedge1->getVertex(0) == coedge1->getVertex(1)){
        			vect3 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
        					points_coedge2[points_coedge2.size()-2]);
        		} else
        			vect3 = vect1*vect2; // potentiellement du mauvais côté ...

        		double lg1 = 0;
        		double lg2 = 0;
        		for (uint i=0; i<points_coedge1.size()-1; i++)
        			lg1 += (points_coedge1[i+1]-points_coedge1[i]).norme();
        		for (uint i=0; i<points_coedge2.size()-1; i++)
        			lg2 += (points_coedge2[i+1]-points_coedge2[i]).norme();

        		if (lg1<lg2)
        			ratio = lg1;
        		else
        			ratio = lg2;
        		ratio /= 7.0;
    		} else if (coedges_som0.size() > 2){
    			CoEdge* coedge1 = coedges_som0[0];
    			CoEdge* coedge2 = coedges_som0[1];
    			CoEdge* coedge3 = coedges_som0[2];

    			std::vector<Utils::Math::Point> points_coedge1;
    			std::vector<Utils::Math::Point> points_coedge2;
    			std::vector<Utils::Math::Point> points_coedge3;
    			coedge1->getPoints(coedge1->getMeshingProperty(), points_coedge1, true);
    			coedge2->getPoints(coedge2->getMeshingProperty(), points_coedge2, true);
    			coedge3->getPoints(coedge3->getMeshingProperty(), points_coedge3, true);
    			if (coedge1->getVertex(0) == getVertex(0))
    				vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			else if (coedge1->getVertex(1) == getVertex(0))
    				vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    						points_coedge1[points_coedge1.size()-2]);
    			else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Erreur interne pour l'affichage de la discrétisation ("
    						<<getName()<<"), pb avec arête "<<coedge1->getName();
    				throw TkUtil::Exception(messErr);
    			}

        		if (coedge2->getVertex(0) == getVertex(0))
        			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
        		else if (coedge2->getVertex(1) == getVertex(0))
        			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
        					points_coedge2[points_coedge2.size()-2]);
        		else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        			messErr << "Erreur interne pour l'affichage de la discrétisation ("
        					<<getName()<<"), pb avec arête "<<coedge2->getName();
        			throw TkUtil::Exception(messErr);
        		}

        		if (coedge3->getVertex(0) == getVertex(0))
        			vect3 = Utils::Math::Vector(points_coedge3[0], points_coedge3[1]);
        		else if (coedge3->getVertex(1) == getVertex(0))
        			vect3 = Utils::Math::Vector(points_coedge3[points_coedge3.size()-1],
        					points_coedge3[points_coedge3.size()-2]);
        		else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        			messErr << "Erreur interne pour l'affichage de la discrétisation ("
        					<<getName()<<"), pb avec arête "<<coedge3->getName();
        			throw TkUtil::Exception(messErr);
        		}

        		double lg1 = 0;
        		double lg2 = 0;
        		double lg3 = 0;
        		for (uint i=0; i<points_coedge1.size()-1; i++)
        			lg1 += (points_coedge1[i+1]-points_coedge1[i]).norme();
        		for (uint i=0; i<points_coedge2.size()-1; i++)
        			lg2 += (points_coedge2[i+1]-points_coedge2[i]).norme();
        		for (uint i=0; i<points_coedge3.size()-1; i++)
        			lg3 += (points_coedge3[i+1]-points_coedge3[i]).norme();

        		if (lg1<lg2)
        			ratio = lg1;
        		else
        			ratio = lg2;
        		if (lg3<ratio)
        			ratio = lg3;
        		ratio /= 7.0;

    		} else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pas assez d'arêtes";
    			throw TkUtil::Exception(messErr);
    		}

    		// normalisation des 3 vecteurs
    		vect1 /= vect1.norme();
    		vect2 /= vect2.norme();
    		vect3 /= vect3.norme();

#ifdef _DEBUG_REPRESENTATION
    		std::cout<<" ratio : "<<ratio<<std::endl;
    		std::cout<<" vect1 : "<<vect1<<std::endl;
    		std::cout<<" vect2 : "<<vect2<<std::endl;
    		std::cout<<" vect3 : "<<vect3<<std::endl;
#endif

    		points.push_back(vect1*0.3+vect2*0.3+vect3*0.3);
    		points.push_back(vect1*1.0+vect2*0.4+vect3*0.4);
    		points.push_back(vect1*0.5+vect2*1.0+vect3*0.8);
       		points.push_back(vect1*0.4+vect2*0.4+vect3*1.0);

       		indices.push_back(0); indices.push_back(1);
       		indices.push_back(0); indices.push_back(2);
       		indices.push_back(0); indices.push_back(3);
       		indices.push_back(1); indices.push_back(2);
       		indices.push_back(2); indices.push_back(3);
       		indices.push_back(1); indices.push_back(3);


    	} // end else if (isStructured())

    	// homothétie pour se mettre à l'échelle
        for (uint i=0; i<points.size(); i++)
        	points[i] = points[i]*ratio;

        // translation
        for (uint i=0; i<points.size(); i++)
        	points[i] += orig;

    } // end if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWDISCRETISATIONTYPE))

}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Block::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            TopoEntity::getDescription (alsoComputed));

    // les propriétés spécifiques à ce type d'entité

    Utils::SerializedRepresentation  topoProprietes ("Propriétés topologiques", "");

#ifdef _DEBUG
    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("En cours d'édition", isEdited()));
#endif

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Méthode de maillage", getMeshLawName()));

    /// ajoute la description des propriétés spécifiques à la méthode de discrétisation du bloc
    getBlockMeshingProperty()->addProperties(topoProprietes);

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Méthode maillage structurée", isStructured()));

    if (isStructured()){
        uint nbI, nbJ, nbK;
        try {
        	getNbMeshingEdges(nbI, nbJ, nbK, true);
        	topoProprietes.addProperty (
        			Utils::SerializedRepresentation::Property ("Nombre de maille 1ère direction", (long)nbI));
        	topoProprietes.addProperty (
        			Utils::SerializedRepresentation::Property ("Nombre de maille 2ème direction", (long)nbJ));
        	topoProprietes.addProperty (
        			Utils::SerializedRepresentation::Property ("Nombre de maille 3ème direction", (long)nbK));
        }
        catch(TkUtil::Exception& exc){
        	topoProprietes.addProperty (
        			Utils::SerializedRepresentation::Property ("Nombre de maille par direction", std::string("indisponible")));
        }
    }

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Est maillé", isMeshed()));

    if (isMeshed()){
        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de noeuds", (long)m_mesh_data->nodes().size()));

        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de polyèdres", (long)m_mesh_data->regions().size()));
    }


    description->addPropertiesSet (topoProprietes);


    // les relations vers les autres types d'entités topologiques
    // on cache l'existance des Edge et Face en mode release

    Utils::SerializedRepresentation  topoRelation ("Relations topologiques", "");

    std::vector<Topo::Vertex* > vtx;
    getVertices(vtx);

    Utils::SerializedRepresentation  vertices ("Sommets topologiques",
            TkUtil::NumericConversions::toStr(vtx.size()));
    for (std::vector<Topo::Vertex*>::iterator iter = vtx.begin( ); vtx.end( )!=iter; iter++)
        vertices.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (vertices);


    if (Internal::InternalPreferences::instance ( )._displayFace.getValue ( )){
    	std::vector<Face* > fa;
    	getFaces(fa);

    	Utils::SerializedRepresentation  faces ("Faces topologiques",
    			TkUtil::NumericConversions::toStr(fa.size()));

    	for (std::vector<Topo::Face*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
    		faces.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (faces);
    }
    else {
    	std::vector<CoFace* > fa;
    	getCoFaces(fa);

    	Utils::SerializedRepresentation  cofaces ("Faces topologiques",
    			TkUtil::NumericConversions::toStr(fa.size()));

    	for (std::vector<Topo::CoFace*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
    		cofaces.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (cofaces);
    }

    description->addPropertiesSet (topoRelation);

    std::vector<Group::Group3D*> grp;
    getGroups(grp);
    if (!grp.empty()){
    	Utils::SerializedRepresentation  groupe ("Relation vers les groupes",
    			TkUtil::NumericConversions::toStr(grp.size()));
    	for (std::vector<Group::Group3D*>::iterator iter = grp.begin( ); iter!=grp.end( ); ++iter)
    		groupe.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));

        description->addPropertiesSet (groupe);

    } // end if (!gr3d.empty())

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
bool Block::
structurable()
{
   if (!isStructured()) {
       if (getNbFaces() == 6){
           // il faut de plus que les faces soient structurables
           for (uint i=0; i<6; i++)
               if (!getFace(i)->structurable())
                   return false;
       }
       else
           return false;

   }
   return true;
}
/*----------------------------------------------------------------------------*/
void Block::
unstructure(Internal::InfoCommand* icmd)
{
    if (!isStructured())
        return;

#ifdef USE_MESHGEMS	
    BlockMeshingProperty* prop = new BlockMeshingPropertyDelaunayMeshGems();
    switchBlockMeshingProperty(icmd, prop);
    delete prop;
#else	// USE_MESHGEMS
	throw TkUtil::Exception (TkUtil::UTF8String ("Non structuration d'un bloc non prévue", TkUtil::Charset::UTF_8));
#endif	// USE_MESHGEMS

    // propage la non-structuration aux faces autant que possible
    for (uint i=0; i<6; i++)
        if (getFace(i)->unstructurable())
            getFace(i)->unstructure(icmd);
}
/*----------------------------------------------------------------------------*/
void Block::setStructured(Internal::InfoCommand* icmd, bool str)
{
    if (str)
        throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'un bloc non prévue", TkUtil::Charset::UTF_8));

    if (isStructured()){
#ifdef USE_MESHGEMS	
        BlockMeshingProperty* prop = new BlockMeshingPropertyDelaunayMeshGems();
        switchBlockMeshingProperty(icmd, prop);
        delete prop;
#else	// USE_MESHGEMS
		throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'un bloc non prévue", TkUtil::Charset::UTF_8));
#endif	// USE_MESHGEMS
    }
}
/*----------------------------------------------------------------------------*/
void Block::
structure(Internal::InfoCommand* icmd)
{
    if (isStructured())
        return;

    if (!structurable()) {
        throw TkUtil::Exception (TkUtil::UTF8String ("Strucuration d'un bloc impossible", TkUtil::Charset::UTF_8));
    }

//    TkUtil::UTF_8   message1 (Charset::UTF_8);
//    message1 << "Block::structure(), avant structuration:\n";
//    message1<<*this;
//    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));

    for (uint i=0; i<6; i++)
        getFace(i)->structure(icmd);

    // Réordonne les Faces comme pour Bibop3D
    // cela suppose que les sommets soit ordonnés
    std::vector<Face* > sorted_faces;

    // On tient compte du cas avec bloc à 5 ou 6 sommets
    std::vector<Topo::Vertex* > vertices;
    getHexaVertices(vertices);

    for (uint i=0; i<6; i++){
        sorted_faces.push_back(getFace(
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][0]],
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][1]],
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][2]],
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][3]]));

    }

    m_topo_property->getFaceContainer().clear();
    m_topo_property->getFaceContainer().add(sorted_faces);

    BlockMeshingProperty* prop = new BlockMeshingPropertyTransfinite();
    switchBlockMeshingProperty(icmd, prop);
    delete prop;

#ifdef _DEBUG2
	TkUtil::UTF8String	message2 (TkUtil::Charset::UTF_8);
    message2 << "Block::structure(), donne comme bloc:\n";
    message2<<*this;
    log (TkUtil::TraceLog (message2, TkUtil::Log::TRACE_4));
#endif
}
/*----------------------------------------------------------------------------*/
int Block::
check() const
{
#ifdef _DEBUG2
	std::cout<<"Block::check() pour le bloc "<<*this<<std::endl;
#endif

    if (isDestroyed()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    	messErr << "On utilise un bloc détruit : "<<getName();
    	throw TkUtil::Exception(messErr);
    }

    // on lance la vérification des faces
    for (uint i=0; i<getNbFaces(); i++)
        getFace(i)->check();

    if (getGeomAssociation() && getGeomAssociation()->isDestroyed()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur avec le bloc "<<getName()
                <<", il pointe sur "<<getGeomAssociation()->getName()
                <<" qui est détruit !";
        throw TkUtil::Exception(messErr);
    }

    if (isStructured()){
        // les faces doivent être structurées
        for (uint i=0; i<getNbFaces(); i++)
            if (!getFace(i)->isStructured()) {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec le bloc structuré "<<getName()
                        <<", sa face "<<getFace(i)->getName()
                        <<" n'est pas structurée";
                throw TkUtil::Exception(messErr);
            }

        // tableau qui donne les 2 directions par face pour les 2 arêtes obtenues à partir
        // des 3 premiers indices de TopoHelper::tabIndVtxByFaceOnBlock
        uint tabDirAretesFace[6][2] = {{2, 1}, {2, 1}, {2, 0}, {2, 0}, {1, 0}, {1, 0}};

        // construction d'un tableau avec les 8 sommets en duplicant les sommets en cas de dégénérescence
        std::vector<Topo::Vertex* > sommets;
        getHexaVertices(sommets);

        // calcul le nombre de bras pour chacune des directions
        uint nbBrasDir[3];
        getNbMeshingEdges(nbBrasDir[0], nbBrasDir[1], nbBrasDir[2]);

        // on vérifie que chacune des Face a le même nombre de bras que ce que l'on a trouvé pour le bloc
        for (uint iFace=0; iFace<getNbFaces(); iFace++){
            Topo::Vertex* v0 = sommets[TopoHelper::tabIndVtxByFaceOnBlock[iFace][0]];
            Topo::Vertex* v1 = sommets[TopoHelper::tabIndVtxByFaceOnBlock[iFace][1]];
            uint nb1, nb2;
            getFace(iFace)->getNbMeshingEdges(nb1, nb2);
            Face::eDirOnFace dirFace = getFace(iFace)->getDir(v0, v1);
            // on permute si nécessaire pour que le premier (nb1) corresponde à la première direction tabDirAretesFace
            if (dirFace == Face::j_dir){
                uint nb_tmp = nb1;
                nb1 = nb2;
                nb2 = nb_tmp;
            }

            if (nb1 != nbBrasDir[tabDirAretesFace[iFace][0]]){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec la face structurée "<<getFace(iFace)->getName()
                        << " dans le bloc " <<getName()
                        <<", le premier côté n'a pas le même nombre de bras: "
                        << (short)nb1
                        <<" (dans la face) et "<< (short)nbBrasDir[tabDirAretesFace[iFace][0]]
                        << " (dans le bloc)";
                throw TkUtil::Exception(messErr);
            }

            if (nb2 != nbBrasDir[tabDirAretesFace[iFace][1]]){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec la face structurée "<<getFace(iFace)->getName()
                        << " dans le bloc " <<getName()
                        <<", le deuxième côté n'a pas le même nombre de bras: "
                        << (short)nb2
                        <<" (dans la face) et "<< (short)nbBrasDir[tabDirAretesFace[iFace][1]]
                        << " (dans le bloc)";
                throw TkUtil::Exception(messErr);
            }
        } // end for (uint iFace=0; iFace<getNbFaces(); iFace++)

        return nbBrasDir[0]*nbBrasDir[1]*nbBrasDir[2];
    } // end if (isStructured())
    // sinon on ne vérifie rien pour le bloc
    return 0;
}
/*----------------------------------------------------------------------------*/
void Block::
getHexaVertices(std::vector<Topo::Vertex* >& sommets) const
{
    sommets.resize(8);

    if (getNbVertices() == 8){
        for (uint i=0; i<8; i++)
            sommets[i] = getVertex(i);
    } else if (getNbVertices() == 5){
        for (uint i=0; i<5; i++)
            sommets[i] = getVertex(i);
        sommets[5] = sommets[4];
        sommets[6] = sommets[4];
        sommets[7] = sommets[4];
    } else if (getNbVertices() == 6){
        for (uint i=0; i<4; i++)
            sommets[i] = getVertex(i);

        if (getFace(0)->getNbVertices() == 3 && getFace(1)->getNbVertices() == 3){
            sommets[4] = getVertex(4);
            sommets[5] = getVertex(5);
            sommets[6] = getVertex(4);
            sommets[7] = getVertex(5);
        } else if (getFace(2)->getNbVertices() == 3 && getFace(3)->getNbVertices() == 3){
            sommets[4] = getVertex(4);
            sommets[5] = getVertex(4);
            sommets[6] = getVertex(5);
            sommets[7] = getVertex(5);
        } else
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec Block::getHexaVertices(), nb de sommets non prévus dans une des faces", TkUtil::Charset::UTF_8));

    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec Block::getHexaVertices(), nb de sommets non prévus", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Face* Block::
getFace(Topo::Vertex* v0, Topo::Vertex* v1, Topo::Vertex* v2, Topo::Vertex* v3)
{
//    std::cout<<"Block::getFace("<<v0->getName()<<","
//            <<v1->getName()<<","<<v2->getName()<<","
//            <<v3->getName()<<")"<<std::endl;

    // on marque les 4 sommets (dont certains peuvent apparaitre plusieurs fois)
    std::map<Topo::Vertex*, uint> filtre_sommets;
    uint nbVtx = 1;
    filtre_sommets[v0] = 1;
    if (filtre_sommets[v1] == 0)
    	nbVtx += 1;
    filtre_sommets[v1] = 1;
    if (filtre_sommets[v2] == 0)
    	nbVtx += 1;
    filtre_sommets[v2] = 1;
    if (filtre_sommets[v3] == 0)
    	nbVtx += 1;
    filtre_sommets[v3] = 1;

    // recherche d'une face avec les nbVtx sommets de marqués
    Face* face_ok = 0;
    for (uint i=0; i<getNbFaces(); i++) {
        uint nb_marques = 0;
        const std::vector<Topo::Vertex* > & local_vertices = getFace(i)->getVertices();
        for (std::vector<Topo::Vertex* >::const_iterator iter2 = local_vertices.begin();
                iter2 != local_vertices.end(); ++iter2) {
            if (filtre_sommets[*iter2] == 1)
                nb_marques+=1;
        }
         if (nb_marques == nbVtx)
             face_ok = getFace(i);
    }
    if (!face_ok){
        std::cerr<<"Erreur avec Block::getFace("<<v0->getName()<<","
                <<v1->getName()<<","<<v2->getName()<<","
                <<v3->getName()<<")"<<std::endl;
        std::cerr<<*this;
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans Block::getFace(avec 4 sommets), on ne trouve pas de face", TkUtil::Charset::UTF_8));
    }
    return face_ok;
}
/*----------------------------------------------------------------------------*/
bool Block::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Block & b)
{
    o << b.getName() << " (uniqueId "<<b.getUniqueId()<<")"
      << (b.isStructured()?" (structuré) ":" (non structuré)");
    o << " (méthode "<<b.getMeshLawName()<<")";
    o << (b.isMeshed()?" (maillé)":" (non maillé)")
      << (b.isDestroyed()?" (DETRUIT)":"")
      << (b.isEdited()?" (EN COURS D'EDITION)":"");

    if (b.getGeomAssociation()){
        o << ", projetée sur " << b.getGeomAssociation()->getName()
        <<(b.getGeomAssociation()->isDestroyed()?" (DETRUIT)":"");

        std::vector<std::string> gn;
        b.getGeomAssociation()->getGroupsName(gn);
        if (!gn.empty()) {
            o << " (groupes:";
            for (size_t i=0; i<gn.size(); i++)
                o << " "<<gn[i];
            o << ")";
        }
        else {
            o << " (sans groupe)";
        }
    }
    else
        o << ", sans projection";

    o << " avec "<<(short)b.getNbFaces()<<" faces:\n";
    const std::vector<Topo::Face* > & faces = b.getFaces();
    for (uint i=0; i<faces.size();i++)
        o << " "<<faces[i]->getName()<<"\n";
//      o << *faces[i]<<"\n";
    o << "\n";

    o << b.getName()<< " bloc avec "<<(short)b.getNbVertices()<<" sommets:\n";
    const std::vector<Topo::Vertex* > & vertices = b.getVertices();
    for (uint i=0; i<vertices.size();i++){
        o <<"   "<<*vertices[i]<<"\n";
    }

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const Block & b)
{
    TkUtil::UTF8String us (TkUtil::Charset::UTF_8);
    us << b;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo Block::getInfos() const
{
	Topo::TopoInfo infos;
	infos.name = getName();
	infos.dimension = getDim();
	getVertices(infos.incident_vertices);
	getCoEdges(infos.incident_coedges);
	getCoFaces(infos.incident_cofaces);
	infos.geom_entity = getGeomAssociation();
	return infos;
}
/*----------------------------------------------------------------------------*/
//void Block::
//getEdges(eDirOnBlock dir, std::vector<Edge* > & edges) const
//{
//
//    if (!isStructured())
//        throw TkUtil::Exception("Block::getEdges(dir) n'est pas possible avec un bloc non structuré");
//
//    std::list<Topo::Edge*> l_e;
//
//    // attention, un bloc structuré a 5, 6 ou 8 sommets suivant les dégénérescences
//    if (dir == i_dir){
//        l_e.push_back(getFace(2)->getEdge(getVertex(0),getVertex(1)));
//        l_e.push_back(getFace(4)->getEdge(getVertex(0),getVertex(1)));
//        l_e.push_back(getFace(4)->getEdge(getVertex(2),getVertex(3)));
//        l_e.push_back(getFace(3)->getEdge(getVertex(2),getVertex(3)));
//
//        if (getNbFaces() == 6){
//            l_e.push_back(getFace(2)->getEdge(getVertex(4),getVertex(5)));
//            l_e.push_back(getFace(5)->getEdge(getVertex(4),getVertex(5)));
//            l_e.push_back(getFace(5)->getEdge(getVertex(6),getVertex(7)));
//            l_e.push_back(getFace(3)->getEdge(getVertex(6),getVertex(7)));
//        }
//        else if (getFace(2)->getNbVertices() == 4){
//            l_e.push_back(getFace(2)->getEdge(getVertex(4),getVertex(5)));
//            l_e.push_back(getFace(3)->getEdge(getVertex(4),getVertex(5)));
//        }
//
//    }
//    else if (dir == j_dir){
//        l_e.push_back(getFace(0)->getEdge(getVertex(0),getVertex(2)));
//        l_e.push_back(getFace(4)->getEdge(getVertex(0),getVertex(2)));
//        l_e.push_back(getFace(4)->getEdge(getVertex(1),getVertex(3)));
//        l_e.push_back(getFace(1)->getEdge(getVertex(1),getVertex(3)));
//
//        if (getNbFaces() == 6){
//            l_e.push_back(getFace(0)->getEdge(getVertex(4),getVertex(6)));
//            l_e.push_back(getFace(5)->getEdge(getVertex(4),getVertex(6)));
//            l_e.push_back(getFace(5)->getEdge(getVertex(5),getVertex(7)));
//            l_e.push_back(getFace(1)->getEdge(getVertex(5),getVertex(7)));
//        }
//        else if (getFace(0)->getNbVertices() == 4){
//            l_e.push_back(getFace(0)->getEdge(getVertex(4),getVertex(5)));
//            l_e.push_back(getFace(1)->getEdge(getVertex(4),getVertex(5)));
//        }
//
//    }
//    else if (dir == k_dir){
//
//        // construction d'un tableau avec les 8 sommets en duplicant les sommets en cas de dégénérescence
//        std::vector<Topo::Vertex* > sommets;
//        getHexaVertices(sommets);
//
//        if (getFace(0)->getNbVertices() == 3 && getFace(1)->getNbVertices() == 3) {
//            l_e.push_back(getFace(0)->getEdge(0));
//            l_e.push_back(getFace(0)->getEdge(2));
//            l_e.push_back(getFace(1)->getEdge(0));
//            l_e.push_back(getFace(1)->getEdge(2));
//        }
//        else {
//            l_e.push_back(getFace(0)->getEdge(sommets[0],sommets[4]));
//            l_e.push_back(getFace(0)->getEdge(sommets[2],sommets[6]));
//            l_e.push_back(getFace(1)->getEdge(sommets[1],sommets[5]));
//            l_e.push_back(getFace(1)->getEdge(sommets[3],sommets[7]));
//        }
//
//        if (getFace(2)->getNbVertices() == 3 && getFace(3)->getNbVertices() == 3) {
//            l_e.push_back(getFace(2)->getEdge(0));
//            l_e.push_back(getFace(2)->getEdge(2));
//            l_e.push_back(getFace(3)->getEdge(0));
//            l_e.push_back(getFace(3)->getEdge(2));
//        }
//        else {
//            l_e.push_back(getFace(2)->getEdge(sommets[0],sommets[4]));
//            l_e.push_back(getFace(2)->getEdge(sommets[1],sommets[5]));
//            l_e.push_back(getFace(3)->getEdge(sommets[2],sommets[6]));
//            l_e.push_back(getFace(3)->getEdge(sommets[3],sommets[7]));
//        }
//
//    }
//    else
//        throw TkUtil::Exception("Block::getEdges avec une direction non prévue");
//
//    l_e.sort(Utils::Entity::compareEntity);
//    l_e.unique();
//
//    edges.clear();
//    edges.insert(edges.end(),l_e.begin(),l_e.end());
//}
/*----------------------------------------------------------------------------*/
void Block::
getOrientedCoEdges(std::vector<CoEdge* > & iCoedges,
                   std::vector<CoEdge* > & jCoedges,
                   std::vector<CoEdge* > & kCoedges) const
{
//#define _DEBUG_getOrientedCoEdges
#ifdef _DEBUG_getOrientedCoEdges
	std::cout<<"Block::getOrientedCoEdges() en cours ...\n";
	std::cout<<*this<<std::endl;
#endif
    // vérification que l'on est bien dans un bloc structuré
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::getOrientedCoEdges n'est pas possible avec un bloc non structuré", TkUtil::Charset::UTF_8));

    // Comme pour Face::getOrientedCoEdges, on construit pour une Face et de proche en proche
    // on détermine si c'est dans une direction équivalente ou dans une troisième

    // on marque les CoEdges dans le bloc
    std::map<CoEdge*, uint> filtre_coedge;
    for(uint f=0; f < getNbFaces(); f++){
        Face* face = getFace(f);

        for(uint i=0; i < face->getNbCoFaces(); i++){
            CoFace* coface = face->getCoFace(i);

            for(uint j=0; j < coface->getNbEdges(); j++){
                Edge* edge = coface->getEdge(j);

                for(uint k=0; k < edge->getNbCoEdges(); k++){
                    CoEdge* coedge = edge->getCoEdge(k);
                    filtre_coedge[coedge] = 1;

                } // for(uint k=0; k < edge->getNbCoEdges(); k++)
            } // for(uint j=0; j < coface->getNbEdges(); j++)
        } // for(uint i=0; i < getNbCoFaces(); i++)
    } // for(uint f=0; f < getNbFaces(); f++)

    // tableau de 3 vecteurs de CoEdges pour le stockage temporaire
    std::vector<CoEdge* > tabCoedges[3];
    uint indDir1 = 0;
    uint indDir2 = 1;

    // ordre des faces pour un parcours avec des faces ayant une coedge en commun
    const uint tabIndFace[6] = {4, 0, 2, 1, 3, 5};
    for (uint i=0; i<getNbFaces(); i++){
        Face* face = getFace(tabIndFace[i]);

        std::vector<CoEdge* > iCoedges_face;
        std::vector<CoEdge* > jCoedges_face;
        face->getOrientedCoEdges(iCoedges_face, jCoedges_face);
#ifdef _DEBUG_getOrientedCoEdges
        std::cout<<"face->getOrientedCoEdges() pour la face "<<face->getName()<<std::endl;
        std::cout<<" iCoedges_face :";
        for (uint j=0; j<iCoedges_face.size(); j++)
        	std::cout<< " "<<iCoedges_face[j]->getName();
        std::cout<<std::endl;
        std::cout<<" jCoedges_face :";
        for (uint j=0; j<jCoedges_face.size(); j++)
        	std::cout<< " "<<jCoedges_face[j]->getName();
        std::cout<<std::endl;
#endif
        // il faut mettre à jour indDir1 et indDir2 en fonction de ce qui a déjà été vu
        if (i != 0) {
            bool dir1CommuneI = false;
            bool dir2CommuneI = false;
            bool dir1CommuneJ = false;
            bool dir2CommuneJ = false;

            for (std::vector<CoEdge* >::iterator iter = iCoedges_face.begin();
                    iter != iCoedges_face.end(); ++iter){
                if (filtre_coedge[*iter] == indDir1+2)
                    dir1CommuneI = true;
                else if (filtre_coedge[*iter] == indDir2+2)
                    dir2CommuneI = true;
            }

            for (std::vector<CoEdge* >::iterator iter = jCoedges_face.begin();
                    iter != jCoedges_face.end(); ++iter){
                if (filtre_coedge[*iter] == indDir1+2)
                    dir1CommuneJ = true;
                else if (filtre_coedge[*iter] == indDir2+2)
                    dir2CommuneJ = true;
            }

            // normallement, un seul des dir*Commune* doit être vrai
            uint indDir3 = 3 - (indDir1+indDir2);
            if (dir1CommuneI){
//std::cout<<"dir1CommuneI\n";
                indDir2 = indDir3;
            } else if (dir2CommuneI){
//std::cout<<"dir2CommuneI\n";
                indDir1 = indDir2;
                indDir2 = indDir3;
            } else if (dir1CommuneJ){
//std::cout<<"dir1CommuneJ\n";
                indDir2 = indDir1;
                indDir1 = indDir3;
            } else if (dir2CommuneJ){
//std::cout<<"dir2CommuneJ\n";
                indDir1 = indDir3;
            } else
                throw TkUtil::Exception (TkUtil::UTF8String ("Block::getOrientedCoEdges ne trouve pas d'arête commune entre 2 faces", TkUtil::Charset::UTF_8));

        } // if (i != 0)
//std::cout<<"indDir1 = "<<indDir1<<std::endl;
//std::cout<<"indDir2 = "<<indDir2<<std::endl;

        // on marque les CoEdges en fonction de la direction dans le bloc
        // et on les stocks dans le vecteur suivant la direction
        for (std::vector<CoEdge* >::iterator iter = iCoedges_face.begin();
                iter != iCoedges_face.end(); ++iter){
            if (filtre_coedge[*iter] == 1)
                tabCoedges[indDir1].push_back(*iter);
            filtre_coedge[*iter] = indDir1+2;
        }
        for (std::vector<CoEdge* >::iterator iter = jCoedges_face.begin();
                iter != jCoedges_face.end(); ++iter){
            if (filtre_coedge[*iter] == 1)
                tabCoedges[indDir2].push_back(*iter);
            filtre_coedge[*iter] = indDir2+2;
        }
    } // for (uint i=0; i<getNbFaces(); i++)

    // recherche de la correspondance entre direction dans bloc et les groupes de CoEdges
    std::vector<CoEdge* > coedges_between; // inutilisé
    for (uint i=0; i<3 && iCoedges.empty(); i++)
        if (TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(1), tabCoedges[i], coedges_between))
            iCoedges = tabCoedges[i];

    for (uint i=0; i<3 && jCoedges.empty(); i++)
        if (TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(2), tabCoedges[i], coedges_between))
            jCoedges = tabCoedges[i];

    for (uint i=0; i<3 && kCoedges.empty(); i++)
        if (TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(4), tabCoedges[i], coedges_between))
            kCoedges = tabCoedges[i];

    if (iCoedges.empty() || jCoedges.empty() || kCoedges.empty()){
        std::cout<<"....................................................\n";
        std::cout<<"Le bloc pour lequel il est fait un getOrientedCoEdges: "<<*this;
        std::cout<<"....................................................\n";

        throw TkUtil::Exception (TkUtil::UTF8String ("Block::getOrientedCoEdges ne trouve pas de groupe d'arêtes entre deux des sommets", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Block::eDirOnBlock Block::
getDir(CoEdge* ce)
{
    std::vector<Topo::CoEdge* > iCoedges[3];
    getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

    for (uint i=0; i<3; i++){
        std::vector<Topo::CoEdge* >::iterator iter;
        iter = find(iCoedges[i].begin(), iCoedges[i].end(), ce);
        if (iter != iCoedges[i].end())
            return (eDirOnBlock)i;
    }

    throw TkUtil::Exception (TkUtil::UTF8String ("L'arête n'est pas dans le bloc, on ne peut pas déterminer de direction", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Block::
getNbMeshingEdges(uint& nbI, uint& nbJ, uint& nbK, bool accept_error) const
{
    // vérification que l'on est bien dans un bloc structuré
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::getNbMeshingEdges n'est pas possible avec un bloc non structuré", TkUtil::Charset::UTF_8));

    // on utilise la recherche des discrétisations pour 2 faces orthogonales
    uint nbI_face0, nbJ_face0, nbI_face4, nbJ_face4;
    getFace(0)->getNbMeshingEdges(nbI_face0, nbJ_face0, accept_error);
    getFace(4)->getNbMeshingEdges(nbI_face4, nbJ_face4, accept_error);

    Face::eDirOnFace dirI = getFace(4)->getDir(getVertex(0), getVertex(1));
    Face::eDirOnFace dirK = getFace(0)->getDir(getVertex(0), getVertex(4));

    if (dirI == Face::i_dir){
        nbI = nbI_face4;
        nbJ = nbJ_face4;
    }
    else {
        nbI = nbJ_face4;
        nbJ = nbI_face4;
    }

    if (dirK == Face::i_dir)
        nbK = nbI_face0;
    else
        nbK = nbJ_face0;
}
/*----------------------------------------------------------------------------*/
void Block::
saveBlockMeshingProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_property == 0){
        	if (isMeshed()){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        		message << "On ne peut pas modifier un bloc alors qu'il est déjà maillé.";
        		throw TkUtil::Exception (message);
        	}
        	else
        		m_save_mesh_property = m_mesh_property->clone();
        }
    }
}
/*----------------------------------------------------------------------------*/
void Block::
switchBlockMeshingProperty(Internal::InfoCommand* icmd, BlockMeshingProperty* prop)
{
	if (isMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "On ne peut pas modifier un bloc ("<<getName()<<") alors qu'il est déjà maillé.";
		throw TkUtil::Exception (message);
	}

    if (icmd && m_save_mesh_property == 0)
        m_save_mesh_property = m_mesh_property;
    else
        delete m_mesh_property;

    m_mesh_property = prop->clone();
#ifdef _DEBUG_MEMORY
    std::cout<<"Block::switchCoFaceMeshingProperty() de "<<getName()<<", avec m_mesh_property en "<<m_mesh_property->getMeshLawName()<<std::endl;
#endif
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void Block::
saveBlockTopoProperty(Internal::InfoCommand* icmd)
{
 //   std::cout<<"Block::saveBlockTopoProperty pour "<<getName()<<std::endl;
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        //std::cout<<"change = "<<(change?"vrai":"faux")<<std::endl;
        if (change && m_save_topo_property == 0){
        	if (isMeshed()){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        		message << "On ne peut pas modifier un bloc alors qu'il est déjà maillé.";
        		throw TkUtil::Exception (message);
        	}
        	else
        		m_save_topo_property = m_topo_property->clone();
        }
    }
}
/*----------------------------------------------------------------------------*/
void Block::
saveBlockMeshingData(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_data == 0)
            m_save_mesh_data = m_mesh_data->clone();
    }
}
/*----------------------------------------------------------------------------*/
BlockMeshingProperty* Block::
setProperty(BlockMeshingProperty* prop)
{
    BlockMeshingProperty* tmp = m_mesh_property;
    m_mesh_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
BlockTopoProperty* Block::
setProperty(BlockTopoProperty* prop)
{
    //std::cout<<"Block::setProperty pour "<<getName()<<std::endl;
    BlockTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
BlockMeshingData* Block::
setProperty(BlockMeshingData* prop)
{
    BlockMeshingData* tmp = m_mesh_data;
    m_mesh_data = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Block::
saveInternals(CommandEditTopo* cet)
{
//    std::cout<<"Block::saveInternals pour "<<getName()<<std::endl;
    TopoEntity::saveInternals (cet);

    if (m_save_mesh_property) {
        cet->addBlockInfoMeshingProperty(this, m_save_mesh_property);
        m_save_mesh_property = 0;
    }
    if (m_save_topo_property) {
//        std::cout<<"m_save_topo_property avec "<<m_save_topo_property->getVertexContainer().size()<<" sommets"<<std::endl;
        cet->addBlockInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
void Block::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_mesh_data) {
        ccm->addBlockInfoMeshingData(this, m_save_mesh_data);
        m_save_mesh_data = 0;
    }
}
/*----------------------------------------------------------------------------*/
void Block::
fuse(Block* bloc_B,
        Internal::InfoCommand* icmd)
{
    // recherche des couples de sommets confondus
    std::map<Topo::Vertex*, Topo::Vertex*> corr_vertex_A_B;
    const std::vector<Topo::Vertex* > & vertices_A = getVertices();
    const std::vector<Topo::Vertex* > & vertices_B = bloc_B->getVertices();

    Vertex::findNearlyVertices(vertices_A, vertices_B, corr_vertex_A_B);

    // recherche de la face dont tous les sommets ont un sommet en correspondance
    Face* face_A = 0;
    bool faceATrouvee = false;
    for (uint i=0; i<getNbFaces() && !faceATrouvee; i++){
        Face* face = getFace(i);
        const std::vector<Topo::Vertex* > & vertices = face->getVertices();

        bool tousMarques = true;
        for (uint j=0; j<vertices.size() && tousMarques; j++)
            if (corr_vertex_A_B[vertices[j]] == 0)
                tousMarques = false;
        if (tousMarques){
            faceATrouvee = true;
            face_A = face;
        }
    }
//    if (faceATrouvee){
//        std::cout <<"Block::fuse trouve la face A " << *face_A;
//    } else
//        std::cout <<"Block::fuse ne trouve pas de face A\n";

    // idem dans le bloc voisin
    std::map<Topo::Vertex*, Topo::Vertex*> corr_vertex_B_A;
    for (std::map<Topo::Vertex*, Topo::Vertex*>::iterator iter = corr_vertex_A_B.begin();
            iter!=corr_vertex_A_B.end(); ++iter)
        corr_vertex_B_A[(*iter).second] = (*iter).first;

    Face* face_B = 0;
    bool faceBTrouvee = false;
    for (uint i=0; i<bloc_B->getNbFaces() && !faceBTrouvee; i++){
        Face* face = bloc_B->getFace(i);
        const std::vector<Topo::Vertex* > vertices = face->getVertices();

        bool tousMarques = true;
        for (uint j=0; j<vertices.size() && tousMarques; j++)
            if (corr_vertex_B_A[vertices[j]] == 0)
                tousMarques = false;
        if (tousMarques){
            faceBTrouvee = true;
            face_B = face;
        }
    }
//    if (faceBTrouvee){
//        std::cout <<"Block::fuse trouve la face B " << *face_B;
//    } else
//        std::cout <<"Block::fuse ne trouve pas de face B\n";

    if (face_A == 0)
    	throw TkUtil::Exception (TkUtil::UTF8String ("La fusion n'a pas pu se faire entre 2 blocs, on ne trouve pas de face en correspondance", TkUtil::Charset::UTF_8));

    // construction des vecteurs de correspondance
    const std::vector<Topo::Vertex* > & vertices_face_A = face_A->getVertices();
    std::vector<Topo::Vertex* > vertices_face_B;
    for (std::vector<Topo::Vertex* >::const_iterator iter = vertices_face_A.begin();
            iter != vertices_face_A.end(); ++iter){
        vertices_face_B.push_back(corr_vertex_A_B[*iter]);
    }

    // appel à la fusion des faces
    face_A->fuse(face_B, vertices_face_A, vertices_face_B, icmd);
}
/*----------------------------------------------------------------------------*/
void Block::extendSplit(CoEdge* arete,
		std::vector<Block* > & newBlocs,
		Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"Block::extendSplit("<<arete->getName()<<")\n";
    //std::cout<<*this;
    std::cout<<"On découpe le bloc "<<getName()<<std::endl;
#endif

#ifdef _DEBUG_TIMER
   TkUtil::Timer timer(true);
#endif

    // recherche d'une arête sur le bord du bloc

    // pour cela on cherche les arêtes du bloc
    std::vector<CoEdge* > iCoedges[3];
    getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"getOrientedCoEdges en  "<<timer.strDuration()<<std::endl;
#endif


    // recherche de la direction de l'arete
    eDirOnBlock dir = unknown;

    std::map<CoEdge*, uint> filtre_coedge;

    for (uint i=0; i<3; i++){
        for (std::vector<Topo::CoEdge* >::iterator iter = iCoedges[i].begin();
                iter != iCoedges[i].end(); ++iter){
            filtre_coedge[*iter] = i+1; // +1 pour se distinguer des arêtes en dehors du bloc
            if (*iter == arete)
                dir = (eDirOnBlock)i;
        } // end for iter
    } // end for i<3

    if (dir == unknown)
        throw TkUtil::Exception (TkUtil::UTF8String ("L'arête n'a pas été trouvée dans le bloc", TkUtil::Charset::UTF_8));

    // recherche dans une face du bloc et voisine de l'arête
    // une arête d'une direction différente
    std::map<CoFace*, uint> filtre_coface;
    std::vector<CoFace* > cofaces;
    getCoFaces(cofaces);
    for (std::vector<CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter)
        filtre_coface[*iter] = 1;

    arete->getCoFaces(cofaces);
    CoFace* coface = 0;
    for (std::vector<CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter)
        if (filtre_coface[*iter] == 1)
            coface = *iter;

    if (0 == coface)
           throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas de CoFace dans le bloc depuis l'arête", TkUtil::Charset::UTF_8));

    CoEdge* coedge = 0;
    std::vector<CoEdge* > coedges;
    uint marque_arete = dir + 1;
    coface->getCoEdges(coedges);
    for (std::vector<Topo::CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        if (filtre_coedge[*iter] != marque_arete)
            coedge = *iter;

    if (0 == coedge)
           throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête dans une autre direction et dans une même coface", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    if (coedge->getVertex(0) == arete->getVertex(0)
    		|| coedge->getVertex(0) == arete->getVertex(1))
    	split(coedge, 0.0, newBlocs, icmd);
    else if (coedge->getVertex(1) == arete->getVertex(0)
    		|| coedge->getVertex(1) == arete->getVertex(1))
    	split(coedge, 1.0, newBlocs, icmd);
    else
    	throw TkUtil::Exception (TkUtil::UTF8String ("On ne trouve pas de sommet commun entre l'arête sélectionnée et l'arête transverse", TkUtil::Charset::UTF_8));
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"split en "<<timer.strDuration()<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
void Block::
split(CoEdge* arete, double ratio,
        std::vector<Block* > & newBlocs,
        Internal::InfoCommand* icmd)
{
    // vérification que l'on est bien dans un bloc structuré
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::split n'est pas possible avec un bloc non structuré", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_SPLIT
    std::cout<<"Block::split("<<arete->getName()<<","<<ratio<<")\n";
    //std::cout<<*this;
    std::cout<<"On découpe le bloc "<<getName()<<std::endl;
#endif
#ifdef _DEBUG_TIMER
   TkUtil::Timer timer(true);
#endif

    // recherche de la direction suivant laquelle est utilisée l'arête
    eDirOnBlock dirSplit;

    std::vector<CoEdge* > iCoedges;
    std::vector<CoEdge* > jCoedges;
    std::vector<CoEdge* > kCoedges;

    getOrientedCoEdges(iCoedges, jCoedges, kCoedges);
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"getOrientedCoEdges en  "<<timer.strDuration()<<std::endl;
#endif

    std::vector<CoEdge* >::iterator iter;
    if ((iter = find(iCoedges.begin(), iCoedges.end(), arete)) != iCoedges.end())
        dirSplit = i_dir;
    else if ((iter = find(jCoedges.begin(), jCoedges.end(), arete)) != jCoedges.end())
        dirSplit = j_dir;
    else if ((iter = find(kCoedges.begin(), kCoedges.end(), arete)) != kCoedges.end())
        dirSplit = k_dir;
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Block::split ne retrouve pas la direction de l'arête dans le bloc", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_SPLIT
    std::cout<<"dirSplit = "<<dirSplit<<std::endl;
#endif

    // les faces communes autorisées pour la coupe
    std::vector<CoFace* > blk_cofaces;
    getCoFaces(blk_cofaces);

    // les Edges qui coupent le bloc en deux
    std::vector<Edge* > splitingEdges;

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
   // le découpage des faces communes (qui doivent former une boucle)
    TopoHelper::splitFaces(blk_cofaces, arete, ratio, 0, true, false, splitingEdges, icmd);
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<" TopoHelper::splitFaces en "<<timer.strDuration()<<std::endl;
#endif

    // les faces qui ne sont pas concernées par la coupe
    uint ind_face0 = dirSplit*2;
    Face* face0 = getFace(ind_face0);
    Face* face1 = (getNbVertices()!=8 && dirSplit==k_dir?0:getFace(dirSplit*2+1));
    Face* faceI[4];
    Face* newFaceI[4][2];
#ifdef _DEBUG_SPLIT
    std::cout<<"face0 : "<<face0->getName()<<std::endl;
#endif
    // nombre de Faces impactées par la coupe
    uint nbFacesCoupees = face0->getNbVertices();

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    for (uint i=0; i<nbFacesCoupees; i++){

        if (tabIndFaceParDir[dirSplit][i]<getNbFaces()){

            // la face que l'on coupe
            faceI[i] = getFace(tabIndFaceParDir[dirSplit][i]);

            // on découpe la face en deux
            faceI[i]->split(splitingEdges, newFaceI[i][0], newFaceI[i][1], icmd);

            // la coupe passe par un bord du bloc
            // on ne fait rien
            if (0 == newFaceI[i][0])
                return;
        }
        else {
            // cas de l'arête d'un prisme par laquelle passe la coupe
            faceI[i] = 0;
            newFaceI[i][0] = 0;
            newFaceI[i][1] = 0;
        }

    } // for (uint i=0; i<nbFacesCoupees; i++)
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<" split des faces en "<<timer.strDuration()<<std::endl;
#endif

    // il faut réordonner les 2 faces issues du découpage
    // pour cela on recherche un des sommets de face0 dans chacun des couples de faceI
    // on met en premier celle qui utilise l'un des sommets
    for (uint i=0; i<nbFacesCoupees; i++){

        if (0 == newFaceI[i][0])
            continue;

        Topo::Vertex* vtx;
        if (i == 0 && getNbVertices() != 8)
            // on évite le sommet du bloc s'il est dégénéré
            vtx = getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]);
        else
            vtx = getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][i]);
#ifdef _DEBUG_SPLIT
        std::cout <<"sommet "<<vtx->getName()<<" pour ordonner les 2 faces : "
                <<newFaceI[i][0]->getName()<<" et "<<newFaceI[i][1]->getName()<<std::endl;
#endif

        if (newFaceI[i][0]->find(vtx)){
            // rien à faire
        } else if (newFaceI[i][1]->find(vtx)) {
            // il faut permuter les 2 faces
            Face* face_tmp = newFaceI[i][1];
            newFaceI[i][1] = newFaceI[i][0];
            newFaceI[i][0] = face_tmp;
        } else
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Block::split ne trouve pas de sommet pour ordonner les 2 faces créées", TkUtil::Charset::UTF_8));
    }


    // les sommets de la face qui coupe le bloc en deux
    std::vector<Topo::Vertex* > splitingVertices;
    // NB il y a autant de sommets que de faces
    Face::eDirOnFace dirFaceSplit[4] = {Face::i_dir,Face::i_dir,Face::i_dir,Face::i_dir};

    for (uint i=0; i<nbFacesCoupees; i++){

        // on cherche la direction de la coupe dans la face
        // on se base sur les 2 sommets en contact avec la face inchangée
        Topo::Vertex* vtx1;
        Topo::Vertex* vtx2;
        if (getNbVertices() == 6 && ind_face0 == 0 && i == 3)
            vtx1 = getVertex(5);
        else
            vtx1 = getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][i]);

        if (getNbVertices() == 6 && nbFacesCoupees == 4 && ind_face0 == 0 && i == 2)
            vtx2 = getVertex(5);
        else
            vtx2 = getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][(i+1)%nbFacesCoupees]);

    	if (i>=4)
    		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Bloc::split echoue avec i trop grand", TkUtil::Charset::UTF_8));

        if (0 == newFaceI[i][0]){
            splitingVertices.push_back(vtx1);
#ifdef _DEBUG_SPLIT
            std::cout<<"vtx1 pour splitingVertices : "<<vtx1->getName()<<std::endl;
#endif
            }
        else {
            dirFaceSplit[i] = newFaceI[i][0]->getDir(vtx1, vtx2);
            // on prend l'autre direction, celle de la coupe
            if (dirFaceSplit[i] == Face::i_dir)
                dirFaceSplit[i] = Face::j_dir;
            else
                dirFaceSplit[i] = Face::i_dir;

            // le sommet opposé, donc sur la coupe
            Topo::Vertex* vtx = newFaceI[i][0]->getOppositeVertex(vtx1, dirFaceSplit[i]);
#ifdef _DEBUG_SPLIT
            std::cout<<"vtx pour splitingVertices : "<<vtx->getName()<<std::endl;
#endif
            splitingVertices.push_back(vtx);
        }
    }


    // Création des 3 ou 4 arêtes
    Edge* edgeI[4];
    for (uint i=0; i<nbFacesCoupees; i++){
        std::vector<CoEdge* > local_coedges;
        Topo::Vertex* vtx1 = splitingVertices[i];
        Topo::Vertex* vtx2 = splitingVertices[(i+1)%nbFacesCoupees];

        if (0 == newFaceI[i][0]) {
            if (dirSplit==i_dir) {
                if (!TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, jCoedges, local_coedges))
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Bloc::split echoue avec getCoEdgesBetweenVertices pour retrouver l'arête sommitale suivant j", TkUtil::Charset::UTF_8));
            }
            else if (dirSplit==j_dir){
                if (!TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, iCoedges, local_coedges))
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Bloc::split echoue avec getCoEdgesBetweenVertices pour retrouver l'arête sommitale suivant i", TkUtil::Charset::UTF_8));
            }
            else
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Bloc::split avec coupe suivant k et arête sommitale", TkUtil::Charset::UTF_8));


        }
        else {
            std::vector<CoEdge* > iCoedges_face;
            std::vector<CoEdge* > jCoedges_face;
            newFaceI[i][0]->getOrientedCoEdges(iCoedges_face, jCoedges_face);

            if (dirFaceSplit[i] == Face::j_dir){
                if (!TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, iCoedges_face, local_coedges))
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Bloc::split echoue avec getCoEdgesBetweenVertices pour la création des arêtes sur la coupe suivant j", TkUtil::Charset::UTF_8));
            }
            else
                if (!TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, jCoedges_face, local_coedges))
                    if (!TopoHelper::getCoEdgesBetweenVertices(vtx2, vtx1, jCoedges_face, local_coedges))
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Bloc::split echoue avec getCoEdgesBetweenVertices pour la création des arêtes sur la coupe suivant i", TkUtil::Charset::UTF_8));

        } // else (0 == newFaceI[i][0])

        if (icmd)
            for (uint j=0; j<local_coedges.size(); j++)
                local_coedges[j]->saveCoEdgeTopoProperty(icmd);

        edgeI[i] = new Topo::Edge(getContext(), vtx1, vtx2, local_coedges);

        if (icmd){
            for (uint j=0; j<local_coedges.size(); j++)
                icmd->addTopoInfoEntity(local_coedges[j], Internal::InfoCommand::DISPMODIFIED);
            icmd->addTopoInfoEntity(edgeI[i], Internal::InfoCommand::CREATED);
        }
    }

    // création de la face issue de la coupe
    CoFace* new_coface;
    if (nbFacesCoupees == 4)
        new_coface = new Topo::CoFace(getContext(), edgeI[0], edgeI[1], edgeI[2], edgeI[3]);
    else
        new_coface = new Topo::CoFace(getContext(), edgeI[0], edgeI[1], edgeI[2]);

#ifdef _DEBUG_SPLIT
    std::cout<<"new_coface pour couper le bloc: "<<*new_coface;
#endif
    if (icmd)
        icmd->addTopoInfoEntity(new_coface, Internal::InfoCommand::CREATED);
    // la face est attachée uniquement à un bloc
    Face* new_face_1 = new Topo::Face(getContext(), new_coface);
    Face* new_face_2 = new Topo::Face(getContext(), new_coface);
    if (icmd){
        icmd->addTopoInfoEntity(new_face_1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(new_face_2, Internal::InfoCommand::CREATED);
    }

    // il faut créer les 2 blocs à partir des 6 faces (la dernière pouvant être dégénérée
    std::vector<Face* > faces_1;
    std::vector<Face* > faces_2;

    std::vector<Topo::Vertex* > vertices_1;
    std::vector<Topo::Vertex* > vertices_2;

    // les nouveaux blocs dépendent du type de bloc au départ
    if (getNbVertices() == 8){
        // cas du bloc hexaédrique
        vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]));
        vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]));
        vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][3]));
        vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]));
        vertices_1.push_back(splitingVertices[0]);
        vertices_1.push_back(splitingVertices[1]);
        vertices_1.push_back(splitingVertices[3]);
        vertices_1.push_back(splitingVertices[2]);

        faces_1.push_back(newFaceI[3][0]);
        faces_1.push_back(newFaceI[1][0]);
        faces_1.push_back(newFaceI[0][0]);
        faces_1.push_back(newFaceI[2][0]);
        faces_1.push_back(face0);
        faces_1.push_back(new_face_1);


        vertices_2.push_back(splitingVertices[0]);
        vertices_2.push_back(splitingVertices[1]);
        vertices_2.push_back(splitingVertices[3]);
        vertices_2.push_back(splitingVertices[2]);
        Topo::Vertex* vtx;
        vtx = newFaceI[0][1]->getOppositeVertex(splitingVertices[0], dirFaceSplit[0]);
        vertices_2.push_back(vtx);
        vtx = newFaceI[1][1]->getOppositeVertex(splitingVertices[1], dirFaceSplit[1]);
        vertices_2.push_back(vtx);
        vtx = newFaceI[3][1]->getOppositeVertex(splitingVertices[3], dirFaceSplit[3]);
        vertices_2.push_back(vtx);
        vtx = newFaceI[2][1]->getOppositeVertex(splitingVertices[2], dirFaceSplit[2]);
        vertices_2.push_back(vtx);

        faces_2.push_back(newFaceI[3][1]);
        faces_2.push_back(newFaceI[1][1]);
        faces_2.push_back(newFaceI[0][1]);
        faces_2.push_back(newFaceI[2][1]);
        faces_2.push_back(new_face_2);
        faces_2.push_back(face1);


    } else if (getNbVertices() == 6){
        // cas du bloc avec dégénérescence en prisme

        if (nbFacesCoupees == 3){
            // cas avec une coupe des 3 faces quadrangulaires
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]));
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]));
            vertices_1.push_back(splitingVertices[2]);
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]));
            vertices_1.push_back(splitingVertices[0]);

            faces_1.push_back(face0);
            faces_1.push_back(new_face_1);
            faces_1.push_back(newFaceI[0][0]);
            faces_1.push_back(newFaceI[2][0]);
            faces_1.push_back(newFaceI[1][0]);


            vertices_2.push_back(splitingVertices[1]);
            vertices_2.push_back(newFaceI[1][1]->getOppositeVertex(splitingVertices[1], dirFaceSplit[1]));
            vertices_2.push_back(splitingVertices[2]);
            vertices_2.push_back(newFaceI[2][1]->getOppositeVertex(splitingVertices[2], dirFaceSplit[2]));
            vertices_2.push_back(splitingVertices[0]);
            vertices_2.push_back(newFaceI[0][1]->getOppositeVertex(splitingVertices[0], dirFaceSplit[0]));

            faces_2.push_back(new_face_2);
            faces_2.push_back(face1);
            faces_2.push_back(newFaceI[0][1]);
            faces_2.push_back(newFaceI[2][1]);
            faces_2.push_back(newFaceI[1][1]);
        }
        else if (0 == newFaceI[3][0]) {
            // cas de la coupe des 2 faces triangulaires, du quad à la base, et on suit l'arête somitale
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]));
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]));
            vertices_1.push_back(splitingVertices[2]);
            vertices_1.push_back(splitingVertices[0]);
            vertices_1.push_back(splitingVertices[3]);

            faces_1.push_back(face0);
            faces_1.push_back(new_face_1);
            faces_1.push_back(newFaceI[0][0]);
            faces_1.push_back(newFaceI[2][0]);
            faces_1.push_back(newFaceI[1][0]);


            vertices_2.push_back(splitingVertices[1]);
            vertices_2.push_back(newFaceI[1][1]->getOppositeVertex(splitingVertices[1], dirFaceSplit[1]));
            vertices_2.push_back(splitingVertices[2]);
            vertices_2.push_back(newFaceI[2][1]->getOppositeVertex(splitingVertices[2], dirFaceSplit[2]));
            vertices_2.push_back(splitingVertices[0]);
            vertices_2.push_back(splitingVertices[3]);

            faces_2.push_back(new_face_2);
            faces_2.push_back(face1);
            faces_2.push_back(newFaceI[0][1]);
            faces_2.push_back(newFaceI[2][1]);
            faces_2.push_back(newFaceI[1][1]);
        }
        else {
            // cas avec la coupe parallèle à la base, donc on coupe 2 tri et 2 quad
            faces_1.push_back(newFaceI[3][0]);
            faces_1.push_back(newFaceI[1][0]);
            faces_1.push_back(newFaceI[0][0]);
            faces_1.push_back(newFaceI[2][0]);
            faces_1.push_back(face0);
            faces_1.push_back(new_face_1);

            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]));
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]));
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][3]));
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]));
            vertices_1.push_back(splitingVertices[0]);
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(splitingVertices[3]);
            vertices_1.push_back(splitingVertices[2]);

            faces_2.push_back(newFaceI[3][1]);
            faces_2.push_back(newFaceI[1][1]);
            faces_2.push_back(newFaceI[0][1]);
            faces_2.push_back(newFaceI[2][1]);
            faces_2.push_back(new_face_2);
            if (0 != face1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Block::split face1 devrait être nul pour le prisme coupé // base", TkUtil::Charset::UTF_8));

            vertices_2.push_back(splitingVertices[0]);
            vertices_2.push_back(splitingVertices[1]);
            vertices_2.push_back(splitingVertices[3]);
            vertices_2.push_back(splitingVertices[2]);

            // Pour le cas dégénéré il faut ne prendre les sommets qu'une fois
            Topo::Vertex* vtx;
            vtx = newFaceI[0][1]->getOppositeVertex(splitingVertices[0], dirFaceSplit[0]);
            vertices_2.push_back(vtx); // le 4ème sommet

            vtx = newFaceI[1][1]->getOppositeVertex(splitingVertices[1], dirFaceSplit[1]);
            if (vtx != vertices_2[4])
                vertices_2.push_back(vtx);

            vtx = newFaceI[3][1]->getOppositeVertex(splitingVertices[3], dirFaceSplit[3]);
            if (vtx != vertices_2[4] && (vertices_2.size() == 5 || vtx != vertices_2[5]))
                vertices_2.push_back(vtx);
        }

    } else if (getNbVertices() == 5){
        // cas du bloc avec dégénérescence en pyramide

        if (nbFacesCoupees == 4) {
            // cas avec la coupe parallèle à la base, donc on coupe 4 quad
            faces_1.push_back(newFaceI[3][0]);
            faces_1.push_back(newFaceI[1][0]);
            faces_1.push_back(newFaceI[0][0]);
            faces_1.push_back(newFaceI[2][0]);
            faces_1.push_back(face0);
            faces_1.push_back(new_face_1);

            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]));
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]));
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][3]));
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]));
            vertices_1.push_back(splitingVertices[0]);
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(splitingVertices[3]);
            vertices_1.push_back(splitingVertices[2]);

            faces_2.push_back(newFaceI[3][1]);
            faces_2.push_back(newFaceI[1][1]);
            faces_2.push_back(newFaceI[0][1]);
            faces_2.push_back(newFaceI[2][1]);
            faces_2.push_back(new_face_2);
            if (0 != face1)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Block::split face1 devrait être nul pour le pyramide coupée // base", TkUtil::Charset::UTF_8));

            vertices_2.push_back(splitingVertices[0]);
            vertices_2.push_back(splitingVertices[1]);
            vertices_2.push_back(splitingVertices[3]);
            vertices_2.push_back(splitingVertices[2]);

            // Pour le cas dégénéré il faut ne prendre les sommets qu'une fois
            Topo::Vertex* vtx;
            vtx = newFaceI[0][1]->getOppositeVertex(splitingVertices[0], dirFaceSplit[0]);
            vertices_2.push_back(vtx); // le 4ème sommet

        }
        else {
            // cas avec la coupe de la base (quad) et de 2 tri
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]));
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]));
            vertices_1.push_back(splitingVertices[2]);
            vertices_1.push_back(getVertex(TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]));

            faces_1.push_back(face0);
            faces_1.push_back(new_face_1);
            faces_1.push_back(newFaceI[0][0]);
            faces_1.push_back(newFaceI[2][0]);
            faces_1.push_back(newFaceI[1][0]);


            vertices_2.push_back(splitingVertices[1]);
            vertices_2.push_back(newFaceI[1][1]->getOppositeVertex(splitingVertices[1], dirFaceSplit[1]));
            vertices_2.push_back(splitingVertices[2]);
            vertices_2.push_back(newFaceI[2][1]->getOppositeVertex(splitingVertices[2], dirFaceSplit[2]));
            vertices_2.push_back(splitingVertices[0]);

            faces_2.push_back(new_face_2);
            faces_2.push_back(face1);
            faces_2.push_back(newFaceI[0][1]);
            faces_2.push_back(newFaceI[2][1]);
            faces_2.push_back(newFaceI[1][1]);
        }

    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Block::split découpe un bloc avec un nombre de sommets non prévu", TkUtil::Charset::UTF_8));


    // sauvegarde les relation avant la création du bloc (pour les faces // nouvelle face qui coupe)
    for (std::vector<Face* >::iterator iter = faces_1.begin();
            iter != faces_1.end(); ++iter)
        (*iter)->saveFaceTopoProperty(icmd);
    for (std::vector<Face* >::iterator iter = faces_2.begin();
            iter != faces_2.end(); ++iter)
        (*iter)->saveFaceTopoProperty(icmd);

    Block* block_1 = new Topo::Block(getContext(), faces_1, vertices_1, true);
    Block* block_2 = new Topo::Block(getContext(), faces_2, vertices_2, true);

    newBlocs.push_back(block_1);
    newBlocs.push_back(block_2);

    block_1->setGeomAssociation(getGeomAssociation());
    block_2->setGeomAssociation(getGeomAssociation());

    Utils::Container<Group::Group3D>& groups = getGroupsContainer();
    for (uint i=0; i<groups.size(); i++){
    	Group::Group3D* gr = groups.get(i);
    	gr->add(block_1);
    	gr->add(block_2);
    	block_1->getGroupsContainer().add(gr);
    	block_2->getGroupsContainer().add(gr);
    }

    if (icmd){
        icmd->addTopoInfoEntity(block_1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(block_2, Internal::InfoCommand::CREATED);
    }

    // le type de maillage
#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    if (getMeshLaw() == BlockMeshingProperty::directional){
        // pour le cas d'un découpage unidirectionnel,
        // on recherche une direction acceptable dans les 2 blocs
#ifdef _DEBUG_SPLIT
        std::cout<<" cas d'un bloc ("<<getName()<<") avec MeshLaw à "<<getMeshLawName()<<std::endl;
#endif
        block_1->selectBasicMeshLaw(icmd);
        block_2->selectBasicMeshLaw(icmd);
    }
    else {
        // on clone la méthode de discrétiasation
        block_1->switchBlockMeshingProperty(icmd, getBlockMeshingProperty());
        block_2->switchBlockMeshingProperty(icmd, getBlockMeshingProperty());
    }
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<" sélection du type de maillage en "<<timer.strDuration()<<std::endl;
#endif
    // copie le lien sur la géométrie
    block_1->setGeomAssociation(getGeomAssociation());
    block_2->setGeomAssociation(getGeomAssociation());

    free(icmd);

#ifdef _DEBUG_SPLIT
//    std::cout<<"Block::split() donne comme blocs : \n";
//    std::cout<<"....................................................\n";
//    std::cout<<*block_1;
//    std::cout<<"....................................................\n";
//    std::cout<<*block_2;
//    std::cout<<"....................................................\n";
    block_1->check();
    block_2->check();
#endif
}
/*----------------------------------------------------------------------------*/
void Block::
unrefine(eDirOnBlock dir,
            int ratio,
            Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
    std::cout<<"....................................................\n";
    std::cout<<"Block::unrefine("<<dir<<","<<ratio<<") pour le bloc suivant:\n";
    std::cout<<*this;
    std::cout<<"....................................................\n";
#endif

    if (ratio == 1)
        return;

    // recherche des CoEdges concernées par le déraffinement (ceux pour dir)
    std::vector<Topo::CoEdge* > iCoedges[3];
    getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

    // filtre sur les CoEdges et sur les CoFaces, à un pour ceux dans le bloc
    // on met à 2 la marque pour les CoFace et CoEdge reliés à 2 blocs
    std::map<CoEdge*, uint> filtre_coedges;
    std::map<CoFace*, uint> filtre_cofaces;
    for(uint f=0; f < getNbFaces(); f++){
        Face* face = getFace(f);

        for(uint i=0; i < face->getNbCoFaces(); i++){
            CoFace* coface = face->getCoFace(i);
            // la CoFace est-elle entre 2 blocs ?
            bool coFaceInterne = false;
            if (coface->getNbFaces() == 2){
                filtre_cofaces[coface] = 2;
                coFaceInterne = true;
            }
            else {
                filtre_cofaces[coface] = 1;
                coFaceInterne = false;
            }

            for(uint j=0; j < coface->getNbEdges(); j++){
                Edge* edge = coface->getEdge(j);

                for(uint k=0; k < edge->getNbCoEdges(); k++){
                    CoEdge* coedge = edge->getCoEdge(k);
                    if (coFaceInterne) {
                        filtre_coedges[coedge] = 2;
                    }
                    else {
                        if (filtre_coedges[coedge] != 2)
                            filtre_coedges[coedge] = 1;
                    }
                } // for(uint k=0; k < edge->getNbCoEdges(); k++)
            } // for(uint j=0; j < coface->getNbEdges(); j++)
        } // for(uint i=0; i < getNbCoFaces(); i++)
    } // for(uint f=0; f < getNbFaces(); f++)

    for (uint i=0; i<3; i++)
        if (i == dir){
            // vérification que la division n'a pas de reste pour les CoEdges touchées par le déraffinement
            for (std::vector<Topo::CoEdge* >::iterator iter = iCoedges[i].begin();
                    iter != iCoedges[i].end(); ++iter){
                CoEdge* coedge = *iter;

                int reste = coedge->getNbMeshingEdges()%ratio;
                if (reste) {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                    messErr << "Erreur avec l'arête "<<coedge->getName()
                            <<", sa discrétisation est de "<<(short)coedge->getNbMeshingEdges()
                            << ", on ne peut pas se permettre d'avoir un reste, le déraffinement est de "<< (short)ratio;
                    throw TkUtil::Exception(messErr);
                }
            }
        }
        else
            // on remet à 0 le filtre pour les CoEdges des directions qui ne sont pas celle du déraffinement
            for (std::vector<Topo::CoEdge* >::iterator iter = iCoedges[i].begin();
                    iter != iCoedges[i].end(); ++iter)
                filtre_coedges[*iter] = 0;


    // on parcours les CoFaces concernées par la direction
    // et si elles sont marquées à 2, on affecte le raffinement au niveau de la face
    // si elles sont marquées à 1, on traite les Edges:
    //    pour les CoEdges marquées à 2, on déraffine l'arête
    //    pour les CoEdges marquées à 1, on modifie la discrétisation
    for (uint f=0; f<4; f++)
        if (tabIndFaceParDir[dir][f]<getNbFaces()){
            Face* face = getFace(tabIndFaceParDir[dir][f]);

            // on recherche la direction touchée par le déraffinement
            uint indFace = f;
            if (f == 2)
                indFace = 1;

            Face::eDirOnFace dirLoc = face->getDir(getVertex(TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir][indFace][0]),
                    getVertex(TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir][indFace][1]));

            for(uint i=0; i < face->getNbCoFaces(); i++){
                CoFace* coface = face->getCoFace(i);
                if (filtre_cofaces[coface] == 2){
                    int old_ratio = face->getRatio(coface, dirLoc);
                    face->saveFaceMeshingProperty(icmd);
                    face->setRatio(coface, old_ratio*ratio, dirLoc);
                }
                else {
                    std::vector<Edge* > edges;
                    coface->getEdges(edges);

                    for (std::vector<Edge* >::iterator iter1=edges.begin();
                            iter1!=edges.end(); ++iter1){
                        Edge* edge = *iter1;
                        std::vector<CoEdge* > coedges;
                        edge->getCoEdges(coedges);
                        for (std::vector<CoEdge* >::iterator iter2=coedges.begin();
                                                    iter2!=coedges.end(); ++iter2){
                            CoEdge* coedge = *iter2;

                            if (filtre_coedges[coedge] == 1){
                                coedge->saveCoEdgeMeshingProperty(icmd);
                                CoEdgeMeshingProperty* cemp = coedge->getMeshingProperty();
                                int nbEdges = cemp->getNbEdges() / ratio;
                                cemp->setNbEdges(nbEdges);
                                // on ne veut plus modifier cette arête commune
                                filtre_coedges[coedge] = 0;

                            } else if (filtre_coedges[coedge] == 2){

                                if (edge->getNbCoFaces() != 1){
                                    // on duplique l'arête
                                    Edge* new_edge = edge->clone();
                                    if (icmd)
                                        icmd->addTopoInfoEntity(new_edge, Internal::InfoCommand::CREATED);

                                    // on la remplace dans la coface et on met à jour les relations
                                    coface->replace(edge, new_edge, icmd);

                                    int old_ratio = new_edge->getRatio(coedge);
                                    new_edge->setRatio(coedge, ratio * old_ratio);
                                }
                                else {
                                    int old_ratio = edge->getRatio(coedge);
                                    edge->saveEdgeMeshingProperty(icmd);
                                    edge->setRatio(coedge, ratio * old_ratio);
                                }
                            }
                        } // for iter2
                    } // for iter1
                } // end else / if (filtre_cofaces[coface] == 2)
            } // for i
        } // end if (tabIndFaceParDir[dir][i]<getNbFaces())

#ifdef _DEBUG2
    std::cout<<"....................................................\n";
    std::cout<<"Block::unrefine(), le bloc obtenu: \n";
    std::cout<<*this;
    std::cout<<"....................................................\n";
#endif

}
/*----------------------------------------------------------------------------*/
void Block::
free(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
    std::cout<<"Block::free() pour "<<getName()<<std::endl;
#endif

    saveBlockTopoProperty(icmd);

    // on retire la relation avec la géométrie
    if (icmd && getGeomAssociation()
             && icmd->getTopoInfoEntity()[this] != Internal::InfoCommand::NONE){
        saveTopoProperty();
        setGeomAssociation(0);
    }

    // on met au courant la commande de la destruction
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);

    setDestroyed(true);

    // suppression des relation remontante des faces vers ce block
    for (uint i=0; i<getNbFaces(); i++) {
        getFace(i)->saveFaceTopoProperty(icmd);
        getFace(i)->removeBlock(this, false);
    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
void Block::
degenerateFaceInVertex(uint id, Internal::InfoCommand* icmd)
{
    // la face qui disparait
    Face* face = getFace(id);

    if (face->getNbCoFaces() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Block::degenerateFaceInEdge pour une face composée", TkUtil::Charset::UTF_8));

    // la coface qui disparait
     CoFace* coface = face->getCoFace(0);

     if (coface->getNbVertices() != 4)
         throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Block::degenerateFaceInVertex pour une face avec autre chose que 4 sommets", TkUtil::Charset::UTF_8));

    // sauvegarde pour undo ou retour en arrière en cas d'échec
    saveBlockTopoProperty(icmd);

    // permutation de la topologie pour que la face id soit en k_max
    permuteToKmaxFace(id, icmd);

    // les fusions de sommets des arêtes (qui disparaissent de la face)

    Edge* a0 = coface->getEdge(0);
    Edge* a2 = coface->getEdge(2);
    a0->collapse(icmd);
    a2->collapse(icmd);
    Edge* a1 = coface->getEdge(0);
    Edge* a3 = coface->getEdge(1);
    a1->merge(a3, icmd, false);
    a1->collapse(icmd);

    coface->free(icmd);
    face->free(icmd);
    m_topo_property->getFaceContainer().remove(face, true);

    // suppression des sommets de la liste
    m_topo_property->getVertexContainer().resize(5);

    // des fois que ...
    if (getVertex(4)->isDestroyed())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Block::degenerateFaceInVertex, le sommet conservé est détruit", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Block::
degenerateFaceInEdge(uint id, Topo::Vertex* v1, Topo::Vertex* v2,
            Internal::InfoCommand* icmd)
{
	// comme pour degenerateFaceInVertex, mais sans le dernier collapse

	// la face qui disparait
	Face* face = getFace(id);

	if (face->getNbCoFaces() != 1)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Block::degenerateFaceInEdge pour une face composée", TkUtil::Charset::UTF_8));

	// la coface qui disparait
	CoFace* coface = face->getCoFace(0);

	// sauvegarde pour undo ou retour en arrière en cas d'échec
	saveBlockTopoProperty(icmd);

	// permutation de la topologie pour que la face id soit en k_max
	permuteToKmaxFace(id, icmd);

	// les fusions de sommets des arêtes (qui disparaissent de la face)
	Edge* a0 = coface->getEdge(v1,v2);
	Edge* a2 = coface->getOppositeEdge(a0);
	a0->collapse(icmd);
	a2->collapse(icmd);

	Edge* a1 = coface->getEdge(0);
	Edge* a3 = coface->getEdge(1);
	a1->merge(a3, icmd, false);

	coface->free(icmd);
	face->free(icmd);
	m_topo_property->getFaceContainer().remove(face, true);

	// suppression des sommets de la liste
	// tout d'abord, on évite de se retrouver avec 2 sommets identiques dans la liste restante
	if (getVertex(4) == getVertex(5))
		m_topo_property->getVertexContainer().set(5, getVertex(6));
	m_topo_property->getVertexContainer().resize(6);

	// des fois que ...
	if (getVertex(4)->isDestroyed() || getVertex(5)->isDestroyed())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Block::degenerateFaceInEdge, l'un des sommets conservés est détruit", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Block::
permuteToKmaxFace(uint id, Internal::InfoCommand* icmd)
{
//#define _DEBUG_PERM
#ifdef _DEBUG_PERM
    std::cout<<"Block::permuteToKmaxFace("<<id<<") pour bloc : "<<*this<<std::endl;
#endif

    // cas avec rien à faire
    if (id == 5)
        return;

    // prérequis
    if (id > 5)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace n'est pas possible avec un indice > 5", TkUtil::Charset::UTF_8));

    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace n'est possible qu'avec un bloc structuré", TkUtil::Charset::UTF_8));

    if (getNbVertices() != 8)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace n'est possible qu'avec un bloc structuré à 8 sommets", TkUtil::Charset::UTF_8));

    if (getNbFaces() != 6)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace n'est possible qu'avec un bloc structuré à 6 faces", TkUtil::Charset::UTF_8));

    // un filtre sur les arêtes pour passer des sommets vers les aretes sans sortir du bloc ou de la face
    std::map<Edge*, uint> filtre_aretes;
    // un filtre sur les sommets pour éviter de trouver les edges qui sont toutes en double depuis l'utilisation des CoEdge
    std::map<Topo::Vertex*, uint> filtre_sommets;

    // mémorisation de la direction à l'aide d'un couple de sommets
    Topo::Vertex* dirVertex1;
    Topo::Vertex* dirVertex2;
    if (getMeshLaw() == BlockMeshingProperty::directional
    		|| getMeshLaw() == BlockMeshingProperty::orthogonal){
    	BlockMeshingPropertyDirectional* prop =
    			dynamic_cast<Topo::BlockMeshingPropertyDirectional*>(getBlockMeshingProperty());
    	CHECK_NULL_PTR_ERROR(prop);
    	dirVertex1 = getVertex(0);
    	uint dir = prop->getDir();
    	if (dir == 0)
    		dirVertex2 = getVertex(1);
    	else if (dir == 1)
    		dirVertex2 = getVertex(2);
    	else if (dir == 2)
    		dirVertex2 = getVertex(4);
    }

    // les arêtes du bloc
    std::vector<Edge* > edges;
    getEdges(edges);
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter){
        filtre_aretes[*iter] = 1;
    }

    // les arêtes de la face à mettre en k_max,
    // en tenant compte des arêtes qui s'appuient sur les coedges, dans d'autres faces
    // donc on passe par toutes les coedges
    std::vector<CoEdge* > coedges;
    getFace(id)->getCoEdges(coedges);
    for (std::vector<CoEdge* >::iterator iter1 = coedges.begin();
                iter1 != coedges.end(); ++iter1){
        (*iter1)->getEdges(edges);
        for (std::vector<Edge* >::iterator iter = edges.begin();
                iter != edges.end(); ++iter)
            filtre_aretes[*iter] = 3;
    }

    // les arêtes de la face à mettre en k_max, strictement dans la face
    getFace(id)->getEdges(edges);
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        filtre_aretes[*iter] = 2;


    std::vector<Topo::Vertex* > sommets;
    getFace(id)->getVertices(sommets);
    for (std::vector<Topo::Vertex* >::iterator iter = sommets.begin();
            iter != sommets.end(); ++iter)
        filtre_sommets[*iter] = 2;

    // on se donne un sommet de départ (l'indice 7 car on veut mettre la face en k_max)
    Topo::Vertex* p7 = getFace(id)->getVertex(0);

    // recherche de 2 arêtes marqués dans la face parmis les arêtes reliées au sommet de départ
    std::vector<Edge* > aretesTriedre;
    p7->getEdges(edges);
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter){
        if (filtre_aretes[*iter] == 2){
            aretesTriedre.push_back(*iter);
        }
    }

    if (aretesTriedre.size() != 2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace ne trouve pas 2 arêtes dans la face depuis un des points", TkUtil::Charset::UTF_8));

    // recherche d'1 arête marquée dans le bloc mais hors de la face
    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        if (filtre_aretes[*iter] == 1){
            Topo::Vertex* som_op = (*iter)->getOppositeVertex(p7);
            if (filtre_sommets[som_op] != 2){
                aretesTriedre.push_back(*iter);
                filtre_sommets[som_op] = 2; // pour ne pas prendre 2 fois cette arête
            }
        }

    if (aretesTriedre.size() != 3)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace ne trouve pas 1 arête dans le bloc tout en étant hors de la face", TkUtil::Charset::UTF_8));

    // vérification de l'ordre des arêtes, il faut qu'elles forment un trièdre
    Topo::Vertex* p5 = aretesTriedre[0]->getOppositeVertex(p7);
    Topo::Vertex* p6 = aretesTriedre[1]->getOppositeVertex(p7);
    Topo::Vertex* p3 = aretesTriedre[2]->getOppositeVertex(p7);

    double prod_mixte = Utils::Math::mixte(
            Utils::Math::Vector(p7->getCoord(),p5->getCoord()),
            Utils::Math::Vector(p7->getCoord(),p6->getCoord()),
            Utils::Math::Vector(p7->getCoord(),p3->getCoord()));
    // il faut faire une permutation
    if (prod_mixte<0.0){
        Topo::Vertex* p_tmp = p6;
        p6 = p5;
        p5 = p_tmp;
    }
#ifdef _DEBUG_PERM
    std::cout<<"p7 = "<<p7->getName()<<std::endl;
    std::cout<<"p5 = "<<p5->getName()<<std::endl;
    std::cout<<"p6 = "<<p6->getName()<<std::endl;
    std::cout<<"p3 = "<<p3->getName()<<std::endl;
#endif

    // recherche de p4 (opposé à p7, dans la face)
    Edge* arete7 = getFace(id)->getOppositeEdge(getFace(id)->getEdge(p7, p5));
    Topo::Vertex* p4 = arete7->getOppositeVertex(p6);
#ifdef _DEBUG_PERM
    std::cout<<"p4 = "<<p4->getName()<<std::endl;
#endif

    // recherche des sommets opposés à p4, p5 et p6
    std::vector<Topo::Vertex* > somDep;
    std::vector<Topo::Vertex* > somOpp;
    somDep.push_back(p4);
    somDep.push_back(p5);
    somDep.push_back(p6);

    for (std::vector<Topo::Vertex* >::iterator iterSom = somDep.begin();
            iterSom != somDep.end(); ++iterSom) {
        // recherche d'1 arête marqué dans le bloc mais hors de la face
        Edge* arete = 0;
        (*iterSom)->getEdges(edges);

        for (std::vector<Edge* >::iterator iter = edges.begin();
                iter != edges.end(); ++iter)
            if (filtre_aretes[*iter] == 1)
                arete = *iter;

        if (!arete)
               throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace ne trouve pas 1 arête dans le bloc, hors de la face, depuis un sommet donné", TkUtil::Charset::UTF_8));

        somOpp.push_back(arete->getOppositeVertex(*iterSom));
#ifdef _DEBUG_PERM
        std::cout<<" getOppositeVertex dans l'arete "<<arete->getName()
                << " pour sommet "<<(*iterSom)->getName()
                <<" donne sommet "<<somOpp.back()->getName()<<std::endl;
#endif
    }

#ifdef _DEBUG_PERM
    std::cout<<"p0 = "<<somOpp[0]->getName()<<std::endl;
    std::cout<<"p1 = "<<somOpp[1]->getName()<<std::endl;
    std::cout<<"p2 = "<<somOpp[2]->getName()<<std::endl;
#endif
    // les sommets réordonnés
    m_topo_property->getVertexContainer().set(0, somOpp[0]);
    m_topo_property->getVertexContainer().set(1, somOpp[1]);
    m_topo_property->getVertexContainer().set(2, somOpp[2]);
    m_topo_property->getVertexContainer().set(3, p3);
    m_topo_property->getVertexContainer().set(4, p4);
    m_topo_property->getVertexContainer().set(5, p5);
    m_topo_property->getVertexContainer().set(6, p6);
    m_topo_property->getVertexContainer().set(7, p7);

    // Réordonne les Faces
    std::vector<Face* > sorted_faces;
    for (uint i=0; i<6; i++){
        sorted_faces.push_back(getFace(
                getVertex(TopoHelper::tabIndVtxByFaceOnBlock[i][0]),
                getVertex(TopoHelper::tabIndVtxByFaceOnBlock[i][1]),
                getVertex(TopoHelper::tabIndVtxByFaceOnBlock[i][2]),
                getVertex(TopoHelper::tabIndVtxByFaceOnBlock[i][3])));

    }

    m_topo_property->getFaceContainer().clear();
    m_topo_property->getFaceContainer().add(sorted_faces);

    // on recherche la direction initiale
    if (getMeshLaw() == BlockMeshingProperty::directional){
        BlockMeshingPropertyDirectional* prop = new BlockMeshingPropertyDirectional(dirVertex1->getCoord(), dirVertex2->getCoord());
        prop->initDir(this);
        switchBlockMeshingProperty(icmd, prop);
        delete prop;
    }
#ifdef _DEBUG_PERM
    std::cout<<" ==> Block : "<<*this<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Block::reverseOrder7Vertices()
{
//#define _DEBUG_REVERSE
#ifdef _DEBUG_REVERSE
	std::cout<<"Block::reverseOrder7Vertices() pour bloc : "<<*this<<std::endl;
#endif

	if (7 != getNbVertices())
		throw TkUtil::Exception (TkUtil::UTF8String ("Block::reverseOrder7Vertices n'est pas possible avec autre chose qu'un bloc à 7 sommets", TkUtil::Charset::UTF_8));

	if (6 != getNbFaces())
		throw TkUtil::Exception (TkUtil::UTF8String ("Block::reverseOrder7Vertices n'est pas possible avec autre chose qu'un bloc à 6 côtés", TkUtil::Charset::UTF_8));

	// identification de l'indice du sommet à la dégénérescence
	// c'est le sommet commun aux deux faces triangulaires

	// les faces triangulaires
	std::vector<Face*> faces_tri;
	for (uint i=0; i<getNbFaces(); i++)
		if (3 == getFace(i)->getNbVertices())
			faces_tri.push_back(getFace(i));

	if (2 != faces_tri.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "La fonction reverseOrder7Vertices ne trouve pas 2 faces triangulaires dans "<<getName()
				<<" mais en trouve "<<(short)faces_tri.size();
		throw TkUtil::Exception(messErr);
	}

	Vertex* vtx_deg = TopoHelper::getCommonVertex(faces_tri[0], faces_tri[1]);
	uint ind_deg = getIndex(vtx_deg);

	// identification des indices des 2 sommets opposés à ce sommet à la dégénérescence
	// il s'agit des sommets opposés au sommet deg / face quadrangulaires

	// les faces quad qui contiennent vtx_deg
	std::vector<Face*> faces_quad;
	for (uint i=0; i<getNbFaces(); i++)
		if (4 == getFace(i)->getNbVertices()){
			std::vector<Vertex*> vertices = getFace(i)->getVertices();

			if (std::find(vertices.begin(), vertices.end(), vtx_deg) != vertices.end())
				faces_quad.push_back(getFace(i));
		}



	// 4 cas possibles suivant indices des sommets
	if (2 != faces_quad.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "La fonction reverseOrder7Vertices ne trouve pas 2 faces quadrangulaires en relation avec le sommet "
				<<vtx_deg->getName()<<" dans "<<getName()<<" mais en trouve "<<(short)faces_quad.size();
		throw TkUtil::Exception(messErr);
	}

	Vertex* vtx_opp1 = faces_quad[0]->getVertex((faces_quad[0]->getIndex(vtx_deg)+2) %4);
	Vertex* vtx_opp2 = faces_quad[1]->getVertex((faces_quad[1]->getIndex(vtx_deg)+2) %4);
	uint ind_opp1 = getIndex(vtx_opp1);
	uint ind_opp2 = getIndex(vtx_opp2);

#ifdef _DEBUG_REVERSE
	std::cout<<"vtx_deg: "<<vtx_deg->getName()<<std::endl;
	std::cout<<"vtx_opp1: "<<vtx_opp1->getName()<<std::endl;
	std::cout<<"vtx_opp2: "<<vtx_opp2->getName()<<std::endl;
	std::cout<<"ind_deg = "<<ind_deg<<std::endl;
	std::cout<<"ind_opp1 = "<<ind_opp1<<std::endl;
	std::cout<<"ind_opp2 = "<<ind_opp2<<std::endl;
#endif

	std::vector<Vertex*> loc_vtx = getVertices();
	std::vector<Face*> loc_faces = getFaces();
	Vertex* vtx; // sommet pour les permutations
	Face* face;  // face pour les permutations

	if (4 == ind_deg){

		if ((1 == ind_opp1 && 3 == ind_opp2) || (3 == ind_opp1 && 1 == ind_opp2)){
			// permutation sommets 1-3
			vtx = loc_vtx[1]; loc_vtx[1] = loc_vtx[3]; loc_vtx[3] = vtx;
			// permutation sommets 2-5
			vtx = loc_vtx[2]; loc_vtx[2] = loc_vtx[5]; loc_vtx[5] = vtx;
			// permutation sommets 0-6
			vtx = loc_vtx[0]; loc_vtx[0] = loc_vtx[6]; loc_vtx[6] = vtx;

			// permutation faces 0-5
			face = loc_faces[0]; loc_faces[0] = loc_faces[5]; loc_faces[5] = face;
			// permutation faces 1-4
			face = loc_faces[1]; loc_faces[1] = loc_faces[4]; loc_faces[4] = face;
			// permutation faces 2-3
			face = loc_faces[2]; loc_faces[2] = loc_faces[3]; loc_faces[3] = face;

		}
		else if ((2 == ind_opp1 && 3 == ind_opp2) || (3 == ind_opp1 && 2 == ind_opp2)){
			// permutation sommets 2-3
			vtx = loc_vtx[2]; loc_vtx[2] = loc_vtx[3]; loc_vtx[3] = vtx;
			// permutation sommets 1-5
			vtx = loc_vtx[1]; loc_vtx[1] = loc_vtx[5]; loc_vtx[5] = vtx;
			// permutation sommets 0-6
			vtx = loc_vtx[0]; loc_vtx[0] = loc_vtx[6]; loc_vtx[6] = vtx;

			// permutation faces 0-1
			face = loc_faces[0]; loc_faces[0] = loc_faces[1]; loc_faces[1] = face;
			// permutation faces 2-5
			face = loc_faces[2]; loc_faces[2] = loc_faces[5]; loc_faces[5] = face;
			// permutation faces 3-4
			face = loc_faces[3]; loc_faces[3] = loc_faces[4]; loc_faces[4] = face;

		}
		else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "La fonction reverseOrder7Vertices ne peut se faire pour "<<getName()
			        <<", les sommets opposés à la dégénérescence ne sont pas à une position prévue: "
			        <<(short)ind_opp1<<" et "<<(short)ind_opp2
			        <<" pour le cas avec dégénérescence en position "<<(short)ind_deg;
			throw TkUtil::Exception(messErr);
		}
	}
	else if (5 == ind_deg){
		if ((0 == ind_opp1 && 2 == ind_opp2) || (2 == ind_opp1 && 0 == ind_opp2)){
			// permutation sommets 0-2
			vtx = loc_vtx[0]; loc_vtx[0] = loc_vtx[2]; loc_vtx[2] = vtx;
			// permutation sommets 3-4
			vtx = loc_vtx[3]; loc_vtx[3] = loc_vtx[4]; loc_vtx[4] = vtx;
			// permutation sommets 1-6
			vtx = loc_vtx[1]; loc_vtx[1] = loc_vtx[6]; loc_vtx[6] = vtx;

			// permutation faces 0-4
			face = loc_faces[0]; loc_faces[0] = loc_faces[4]; loc_faces[4] = face;
			// permutation faces 2-3
			face = loc_faces[2]; loc_faces[2] = loc_faces[3]; loc_faces[3] = face;
			// permutation faces 1-5
			face = loc_faces[1]; loc_faces[1] = loc_faces[5]; loc_faces[5] = face;

		}
		else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "La fonction reverseOrder7Vertices ne peut se faire pour "<<getName()
			        <<", les sommets opposés à la dégénérescence ne sont pas à une position prévue: "
			        <<(short)ind_opp1<<" et "<<(short)ind_opp2
			        <<" pour le cas avec dégénérescence en position "<<(short)ind_deg;
			throw TkUtil::Exception(messErr);
		}
	}
	else if (6 == ind_deg){
		if ((0 == ind_opp1 && 1 == ind_opp2) || (1 == ind_opp1 && 0 == ind_opp2)){
			// permutation sommets 0-1
			vtx = loc_vtx[0]; loc_vtx[0] = loc_vtx[1]; loc_vtx[1] = vtx;
			// permutation sommets 2-5
			vtx = loc_vtx[2]; loc_vtx[2] = loc_vtx[5]; loc_vtx[5] = vtx;
			// permutation sommets 3-4
			vtx = loc_vtx[3]; loc_vtx[3] = loc_vtx[4]; loc_vtx[4] = vtx;

			// permutation faces 0-1
			face = loc_faces[0]; loc_faces[0] = loc_faces[1]; loc_faces[1] = face;
			// permutation faces 2-4
			face = loc_faces[2]; loc_faces[2] = loc_faces[4]; loc_faces[4] = face;
			// permutation faces 3-5
			face = loc_faces[3]; loc_faces[3] = loc_faces[5]; loc_faces[5] = face;

		}
		else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "La fonction reverseOrder7Vertices ne peut se faire pour "<<getName()
			        <<", les sommets opposés à la dégénérescence ne sont pas à une position prévue: "
			        <<(short)ind_opp1<<" et "<<(short)ind_opp2
			        <<" pour le cas avec dégénérescence en position "<<(short)ind_deg;
			throw TkUtil::Exception(messErr);
		}
	}
	else {
		TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
		messErr << "La fonction reverseOrder7Vertices ne peut se faire pour "<<getName()
				<<", le sommet dégénéré n'est pas en fin de liste mais en position "<<(short)ind_deg;
		throw TkUtil::Exception(messErr);
	}

	// met en place la nouvelle liste de sommets réordonnés
	getBlockTopoProperty()->getVertexContainer().clear();
	getBlockTopoProperty()->getVertexContainer().add(loc_vtx);

	// idem avec les faces
	getBlockTopoProperty()->getFaceContainer().clear();
	getBlockTopoProperty()->getFaceContainer().add(loc_faces);

#ifdef _DEBUG_REVERSE
	std::cout<<" bloc après reverseOrder7Vertices : "<<*this<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Block::
setMeshLaw(BlockMeshingProperty* new_ppty)
{
    BlockMeshingProperty* old_ppty = setProperty(new_ppty);
    delete old_ppty;

    // recherche des arêtes qui suivent la direction du maillage
    // si direction il y a
    std::vector<Topo::CoEdge* > iCoedges[3];
    eDirOnBlock dir = unknown;
    if (new_ppty->getMeshLaw() == BlockMeshingProperty::directional
            || new_ppty->getMeshLaw() == BlockMeshingProperty::rotational){
        getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);
        dir = (eDirOnBlock)new_ppty->getDir();
    }

    std::vector<Topo::Face* > faces;
    getFaces(faces);
    for (std::vector<Topo::Face* >::iterator iter1 = faces.begin();
            iter1 != faces.end(); ++iter1){
        Topo::Face* face = *iter1;

        Topo::Block* bloc_opp = face->getOppositeBlock(this);

        for (uint j=0; j<face->getNbCoFaces(); j++){
            Topo::CoFace* coface = face->getCoFace(j);

            // cas où on affecte une méthode structurée
            if (new_ppty->isStructured()){

                if (new_ppty->getMeshLaw() == BlockMeshingProperty::transfinite) {
                    FaceMeshingPropertyTransfinite* prop = new FaceMeshingPropertyTransfinite();
                    CoFaceMeshingProperty* old_prop = coface->setProperty(prop);
                    delete old_prop;
                }
                else {
                    // il faut retrouver la direction par rapport à la face
                    // et l'affecter si possible
                    _setDirectionMeshLawToFace(new_ppty, iCoedges[dir], coface);
                }
            }
            // si méthode non structuré, l'affecte à la face si le bloc voisin n'est pas structuré
            else if (new_ppty->getMeshLaw() == BlockMeshingProperty::delaunayTetgen){
                // pas de bloc structuré comme voisin
                if (0 == bloc_opp || !bloc_opp->isStructured()) {
                    FaceMeshingPropertyDelaunayGMSH* prop = new FaceMeshingPropertyDelaunayGMSH();
                    CoFaceMeshingProperty* old_prop = coface->setProperty(prop);
                    delete old_prop;
                }
            }
            else {
                throw TkUtil::Exception (TkUtil::UTF8String ("Block::setMeshLaw, cas non prévu (cas avec propagation, méthode non-structurée)", TkUtil::Charset::UTF_8));
            }
        } // end for j=0; j<face->getNbCoFace()

    } // end for iter1 = faces.begin();
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MESH_LAW
void Block::
_setDirectionMeshLawToFace(BlockMeshingProperty* new_ppty,
        std::vector<Topo::CoEdge* >& dirCoedges,
        Topo::CoFace* coface)
{
#ifdef _DEBUG_MESH_LAW
    std::cout<<"_setDirectionMeshLawToFace("<<new_ppty->getMeshLawName()<<", "<<coface->getName()<<")"<<std::endl;
#endif


    // recherche de la direction (ce qui exclu les cas autres que directional ou rotational)
    eDirOnBlock dir = (eDirOnBlock)new_ppty->getDir();;

    // on marque les arêtes communes du bloc qui suivent la direction
    std::map<CoEdge*, uint> filtre_aretes;
#ifdef _DEBUG_MESH_LAW
    std::cout<<" suivant les arêtes : ";
#endif
    for (std::vector<CoEdge* >::iterator iter = dirCoedges.begin();
            iter != dirCoedges.end(); ++iter){
        filtre_aretes[*iter] = 1;
#ifdef _DEBUG_MESH_LAW
        std::cout<<" "<<(*iter)->getName();
#endif
    }
#ifdef _DEBUG_MESH_LAW
    std::cout<<std::endl;
#endif

    std::vector<Topo::CoEdge* > iCoedges[2];
    coface->getOrientedCoEdges(iCoedges[0], iCoedges[1]);

    // on cherche les arêtes pour chacunes des directions
    for (uint i=0; i<2; i++)
        for (std::vector<CoEdge* >::iterator iter = iCoedges[i].begin();
                iter != iCoedges[i].end(); ++iter)
            if (filtre_aretes[*iter] == 1){

                if (new_ppty->getMeshLaw() == BlockMeshingProperty::directional){
                    FaceMeshingPropertyDirectional* prop =
                            new FaceMeshingPropertyDirectional(i==0?CoFaceMeshingProperty::dir_i:CoFaceMeshingProperty::dir_j);
                    CoFaceMeshingProperty* old_prop = coface->setProperty(prop);
                    delete old_prop;
                }
                else if (new_ppty->getMeshLaw() == BlockMeshingProperty::rotational){
                    BlockMeshingPropertyRotational* bmp = dynamic_cast<BlockMeshingPropertyRotational*>(new_ppty);
                    CHECK_NULL_PTR_ERROR(bmp);
                    Utils::Math::Point axis1;
                    Utils::Math::Point axis2;
                    bmp->getAxis(axis1, axis2);
                    FaceMeshingPropertyRotational* prop =
                            new FaceMeshingPropertyRotational(i==0?CoFaceMeshingProperty::dir_i:CoFaceMeshingProperty::dir_j,
                            		axis1, axis2);
                    CoFaceMeshingProperty* old_prop = coface->setProperty(prop);
                    delete old_prop;
                }
            }

    // il se peut que l'on ne trouve pas d'arête de marquée,
    // c'est que l'on est orthogonal, on ne fait rien.
}
/*----------------------------------------------------------------------------*/
int Block::
getNbRegions()
{
    if (isMeshed())
        return regions().size();

    if (isStructured()) {
        uint nbBrasI = 0;
        uint nbBrasJ = 0;
        uint nbBrasK = 0;

        getNbMeshingEdges(nbBrasI, nbBrasJ, nbBrasK);

        return nbBrasI*nbBrasJ*nbBrasK;
    }
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::getNbRegions() n'est pas possible avec un bloc non structuré (à moins de le mailler)", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
bool Block::
isEdited() const
{
    return TopoEntity::isEdited()
    || m_save_topo_property != 0
    || m_save_mesh_property != 0
    || m_save_mesh_data != 0;
}
/*----------------------------------------------------------------------------*/
void Block::
getRatios(std::map<CoEdge*,uint> &ratios)
{
    for (uint i=0; i<getNbFaces(); i++){
        Face* face = getFace(i);

        // recherche de la présence de ratios sur les cofaces
        bool haveRatio = false;
        for (uint j=0; j<face->getNbCoFaces(); j++){
            CoFace* coface = face->getCoFace(j);

            uint ratio_i = face->getRatio(coface, 0);
            uint ratio_j = face->getRatio(coface, 1);

            if (ratio_i > 1 || ratio_j > 1)
                haveRatio = true;
        } // end for j<face->getNbCofaces()

        if (haveRatio){
#ifdef _DEBUG2
           std::cout<<face->getName()<<" possède au moins un ratio"<<std::endl;
#endif
            // construction d'une table pour la face qui pour une coedge donne la doirection dans la face
            std::vector<CoEdge* > dirCoedges[2];
            face->getOrientedCoEdges(dirCoedges[0], dirCoedges[1]);
            std::map<CoEdge*,short> coedge2dir;
            for (uint dir=0; dir<2; dir++)
                for (std::vector<CoEdge* >::iterator iter = dirCoedges[dir].begin();
                        iter != dirCoedges[dir].end(); ++iter)
                    coedge2dir[*iter] = dir;

            for (uint j=0; j<face->getNbCoFaces(); j++){
                CoFace* coface = face->getCoFace(j);

                uint ratio_dir[2];
                for (uint dir=0; dir<2; dir++)
                    ratio_dir[dir] = face->getRatio(coface, dir);

                for (uint dir=0; dir<2; dir++){
                    for (uint cote=0; cote<2; cote++){
                        uint ind = cote*2 + dir;
                        // on évite la 4ème arête d'une coface dégénérée
                        if (ind < coface->getNbEdges()){
                            Edge* edge =  coface->getEdge(ind);

                            for (uint l=0; l<edge->getNbCoEdges(); l++){
                                CoEdge* coedge = edge->getCoEdge(l);
                                uint ratio = edge->getRatio(coedge);
                                // cas d'une coedge non encore vue
                                if (ratios[coedge] == 0){
                                    ratios[coedge] = ratio*ratio_dir[coedge2dir[coedge]];
#ifdef _DEBUG2
                                    std::cout<<coedge->getName()<<" a pour ratio "
                                            <<ratio<<"x"<<ratio_dir[coedge2dir[coedge]]
                                            <<"="<<ratio*ratio_dir[coedge2dir[coedge]]<<std::endl;
#endif
                                } // end if (ratios[coedge] == 0)
                            } // end for l<edge->getNbCoEdges()
                        } // end if (ind < coface->getNbEdges())
                    } // end for cote<2
                } // end for dir<2

            } // end for j<face->getNbCofaces()

        } // end if (haveRatio)
        else {
            // on évite ici l'appel à getOrientedCoEdges qui coûte un peu

            // recherche des ratios sur les coedges
            for (uint j=0; j<face->getNbCoFaces(); j++){
                CoFace* coface = face->getCoFace(j);
                for (uint k=0; k<coface->getNbEdges(); k++){
                    Edge* edge =  coface->getEdge(k);
                    for (uint l=0; l<edge->getNbCoEdges(); l++){
                        CoEdge* coedge = edge->getCoEdge(l);

                        if (ratios[coedge] == 0){
                            uint ratio = edge->getRatio(coedge);
                            ratios[coedge] = ratio;
#ifdef _DEBUG2
                            std::cout<<coedge->getName()<<" a pour ratio "<<ratio<<std::endl;
#endif
                        }
                    } // end for l<edge->getNbCoEdges()
                } /// end for k<coface->getNbEdges()
            } // end for j<face->getNbCofaces()
        } // end else / if (haveRatio)
    } // end for i<getNbFaces()
}
/*----------------------------------------------------------------------------*/
void Block::
selectBasicMeshLaw(Internal::InfoCommand* icmd, bool forceCompute)
{
#ifdef _DEBUG2
    std::cout<<"selectBasicMeshLaw pour "<<getName()<<" cas d'un bloc avec MeshLaw à "<<getMeshLawName()<<std::endl;
#endif

    if (!forceCompute && getMeshLaw() == BlockMeshingProperty::directional){
#ifdef _DEBUG2
        std::cout<<"  on conserve "<<getMeshLawName()<< " pour "<<getName()<<std::endl;
#endif
        return;
    }

    std::vector<Topo::CoEdge* > iCoedges[3];
    getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

    std::vector<Topo::Vertex* > sommets;
    getHexaVertices(sommets);

    // la direction sélectionnée, à définir
    BlockMeshingProperty::meshDirLaw dirLaw = BlockMeshingProperty::dir_undef;

    // une des 3 directions convient-elle ?
    for (uint j=0; j<3; j++){
        std::vector<std::vector<CoEdge* > > coedges_dirs;
        for (uint k=0; k<4; k++){
            std::vector<CoEdge* > coedges_dir1;

            TopoHelper::getCoEdgesBetweenVertices(sommets[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[j][k][0]],
                    sommets[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[j][k][1]],
                    iCoedges[j],
                    coedges_dir1);
            coedges_dirs.push_back(coedges_dir1);

        } // end for k
        if (TopoHelper::isUnidirectionalMeshable(coedges_dirs))
            dirLaw = (BlockMeshingProperty::meshDirLaw)(j+1);
    } // end for j

    if (dirLaw != BlockMeshingProperty::dir_undef){
        saveBlockMeshingProperty(icmd);
        BlockMeshingPropertyDirectional* mp = new BlockMeshingPropertyDirectional(dirLaw);
        switchBlockMeshingProperty(icmd, mp);
        delete mp;

#ifdef _DEBUG2
        std::cout<<"  on attribue "<<getMeshLawName()<< " à "<<getName()<<std::endl;
#endif
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Block::getBarycentre() const
{
	Utils::Math::Point barycentre;

	std::vector<Topo::Vertex* > vertices;
	getVertices(vertices);
	for (uint i=0; i<vertices.size(); i++)
		barycentre += vertices[i]->getCoord();
	barycentre /= (double)vertices.size();
	return barycentre;
}
/*----------------------------------------------------------------------------*/
void Block::getGroupsName (std::vector<std::string>& gn, bool byGeom, bool byTopo) const
{
	if (byGeom)
		TopoEntity::getGroupsName(gn, byGeom, byTopo);

	if (byTopo)
		for (uint i = 0; i<m_topo_property->getGroupsContainer().size(); ++i)
		        gn.push_back(m_topo_property->getGroupsContainer().get(i)->getName());
}
/*----------------------------------------------------------------------------*/
void Block::setDestroyed(bool b)
{
#ifdef _DEBUG2
    std::cout<<"Block::setDestroyed("<<b<<") de "<<getName()<<std::endl;
#endif
    // cas sans changement
    if (isDestroyed() == b)
        return;

    // on retire la relation depuis les groupes
     Utils::Container<Group::Group3D>& groups = getGroupsContainer();
#ifdef _DEBUG2
    std::cout<<"Les groupes:";
    for  (uint i=0; i<groups.size(); i++)
		 std::cout<<" "<<groups.get(i)->getName();
    std::cout<<std::endl;
#endif


     if (b)
    	 for (uint i=0; i<groups.size(); i++){
    		 Group::Group3D* gr = groups.get(i);
    		 gr->remove(this);
    	 }
     else
    	 for (uint i=0; i<groups.size(); i++){
    		 Group::Group3D* gr = groups.get(i);
    		 gr->add(this);
    	 }

    TopoEntity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
unsigned long Block::getNbInternalMeshingNodes()
{
	if (isStructured()){
		uint nbI;
		uint nbJ;
		uint nbK;
		getNbMeshingEdges(nbI, nbJ, nbK);

		return (nbI-1)*(nbJ-1)*(nbK-1);
	}
	else {
		unsigned short res = m_mesh_data->nodes().size();
		for (uint i=0;i<getNbVertices(); i++)
			res -= getVertex(i)->getNbInternalMeshingNodes();

		std::vector<CoEdge*> coedges;
		getCoEdges(coedges);
		for (uint i=0;i<coedges.size(); i++)
			res -= coedges[i]->getNbInternalMeshingNodes();

		std::vector<CoFace*> cofaces;
		getCoFaces(cofaces);
		for (uint i=0;i<cofaces.size(); i++)
			res -= cofaces[i]->getNbInternalMeshingNodes();

		return res;
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
