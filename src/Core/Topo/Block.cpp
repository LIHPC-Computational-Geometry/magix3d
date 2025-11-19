#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string.h>
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Internal/InternalPreferences.h"
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
#include "Topo/FaceMeshingPropertyTransfinite.h"
#include "Topo/FaceMeshingPropertyDelaunayGMSH.h"
#include "Topo/BlockMeshingPropertyTransfinite.h"
#include "Topo/BlockMeshingPropertyDelaunayTetgen.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/Common.h"
#include "Utils/SerializedRepresentation.h"
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
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
    else{
		m_mesh_property = new BlockMeshingPropertyDelaunayTetgen ( );   // Or Triton ???
    }

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

    Utils::append(m_topo_property->getFaceContainer(), faces);
    Utils::append(m_topo_property->getVertexContainer(), vertices);

    _init();
}
/*----------------------------------------------------------------------------*/
Block::
Block(Internal::Context& ctx, int ni, int nj, int nk)
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

    m_mesh_property = new BlockMeshingPropertyTransfinite();

    // bloc avec les sommets équivalents à ceux d'une boite de taille 1
    Vertex* v0 = new Topo::Vertex(ctx, Utils::Math::Point(0,0,0));
    Vertex* v1 = new Topo::Vertex(ctx, Utils::Math::Point(1,0,0));
    Vertex* v2 = new Topo::Vertex(ctx, Utils::Math::Point(0,1,0));
    Vertex* v3 = new Topo::Vertex(ctx, Utils::Math::Point(1,1,0));
    Vertex* v4 = new Topo::Vertex(ctx, Utils::Math::Point(0,0,1));
    Vertex* v5 = new Topo::Vertex(ctx, Utils::Math::Point(1,0,1));
    Vertex* v6 = new Topo::Vertex(ctx, Utils::Math::Point(0,1,1));
    Vertex* v7 = new Topo::Vertex(ctx, Utils::Math::Point(1,1,1));

    m_topo_property->getVertexContainer().push_back(v0);
    m_topo_property->getVertexContainer().push_back(v1);
    m_topo_property->getVertexContainer().push_back(v2);
    m_topo_property->getVertexContainer().push_back(v3);
    m_topo_property->getVertexContainer().push_back(v4);
    m_topo_property->getVertexContainer().push_back(v5);
    m_topo_property->getVertexContainer().push_back(v6);
    m_topo_property->getVertexContainer().push_back(v7);

    // les arêtes (communes) dont on ne conserve pas de lien dans le bloc
    std::vector<CoEdge* > coedges;

    // une discretisation par défaut (découpage uniforme suivant le nombre de bras par défaut)
    // à moins qu'un nombre de bras ait été demandé
    EdgeMeshingPropertyUniform empI(ni?ni:ctx.getTopoManager().getDefaultNbMeshingEdges());
    EdgeMeshingPropertyUniform empJ(nj?nj:ctx.getTopoManager().getDefaultNbMeshingEdges());
    EdgeMeshingPropertyUniform empK(nk?nk:ctx.getTopoManager().getDefaultNbMeshingEdges());

    coedges.push_back(new Topo::CoEdge(ctx, &empI, v0, v1));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, v2, v3));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, v4, v5));
    coedges.push_back(new Topo::CoEdge(ctx, &empI, v6, v7));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, v0, v2));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, v1, v3));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, v4, v6));
    coedges.push_back(new Topo::CoEdge(ctx, &empJ, v5, v7));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, v0, v4));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, v1, v5));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, v2, v6));
    coedges.push_back(new Topo::CoEdge(ctx, &empK, v3, v7));

    // création des Edges qui utilisent les CoEdges
    std::vector<Edge* > edges;
    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
        edges.push_back(new Edge(ctx, *iter));
    }

    // création des 6 faces
    CoFace* coface;

    coface = new Topo::CoFace(ctx, edges[4], edges[8], edges[6], edges[10]);
    m_topo_property->getFaceContainer().push_back(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[5], edges[9], edges[7], edges[11]);
    m_topo_property->getFaceContainer().push_back(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[0], edges[8], edges[2], edges[9]);
    m_topo_property->getFaceContainer().push_back(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[1], edges[10], edges[3], edges[11]);
    m_topo_property->getFaceContainer().push_back(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[0], edges[4], edges[1], edges[5]);
    m_topo_property->getFaceContainer().push_back(new Topo::Face(ctx, coface));
    coface = new Topo::CoFace(ctx, edges[2], edges[6], edges[3], edges[7]);
    m_topo_property->getFaceContainer().push_back(new Topo::Face(ctx, coface));

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
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<faces.size(); i++)
        faces[i]->add(this);
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
    m_topo_property = 0;

    delete m_mesh_property;
    m_mesh_property = 0;

    delete m_mesh_data;
    m_mesh_data = 0;

    if (m_save_topo_property) {
        delete m_save_topo_property;
        m_save_topo_property = 0;
    }

    if (m_save_mesh_property) {
        delete m_save_mesh_property;
        m_save_mesh_property = 0;
    }

    if (m_save_mesh_data) {
        delete m_save_mesh_data;
        m_save_mesh_data = 0;
    }
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* Block::
getVertex(bool cote_i, bool cote_j, bool cote_k)
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::getVertex(bool,bool,bool) n'est pas possible avec un bloc non structuré", TkUtil::Charset::UTF_8));

    uint indice = (cote_i?1:0) + (cote_j?2:0) + (cote_k?4:0);
    Topo::Vertex* vtx = getVertices()[indice];
#ifdef _DEBUG2
    std::cout<<"Block::getVertex("<<(cote_i?"true":"false")<<", "<<(cote_j?"true":"false")<<", "<<(cote_k?"true":"false")<<") => "<<vtx->getName()<<std::endl;
#endif
    return vtx;

}
/*----------------------------------------------------------------------------*/
std::vector<CoFace*> Block::
getCoFaces() const
{
    Utils::EntitySet<CoFace*> cofaces(Utils::Entity::compareEntity);
    for(Face* face : getFaces()) {
        const std::vector<CoFace* >& local_cofaces = face->getCoFaces();
        cofaces.insert(local_cofaces.begin(), local_cofaces.end());
    }
    return Utils::toVect(cofaces);
}
/*----------------------------------------------------------------------------*/
std::vector<Edge*> Block::
getEdges() const
{
    Utils::EntitySet<Topo::Edge*> edges(Utils::Entity::compareEntity);
    for(Face* face : getFaces()) {
        for(CoFace* coface : face->getCoFaces()) {
            const std::vector<Edge* >& local_edges = coface->getEdges();
            edges.insert(local_edges.begin(), local_edges.end());
        }
    }
    return Utils::toVect(edges);
}
/*----------------------------------------------------------------------------*/
std::vector<Vertex*> Block::
getAllVertices() const
{
    Utils::EntitySet<Vertex*> vertices(Utils::Entity::compareEntity);
    for(Face* face : getFaces()) {
        for(CoFace* coface : face->getCoFaces()) {
            for(Edge* edge : coface->getEdges()) {
                for(CoEdge* coedge : edge->getCoEdges()){
                    const std::vector<Vertex* >& local_vertices = coedge->getVertices();
                    vertices.insert(local_vertices.begin(), local_vertices.end());
                }
            }
        }
    }
    return Utils::toVect(vertices);
}
/*----------------------------------------------------------------------------*/
std::vector<CoEdge*> Block::
getCoEdges() const
{
    Utils::EntitySet<Topo::CoEdge*> coedges(Utils::Entity::compareEntity);
    for(Face* face : getFaces()) {
        for(CoFace* coface : face->getCoFaces()) {
            for(Edge* edge : coface->getEdges()) {
                const std::vector<CoEdge* >& local_coedges = edge->getCoEdges();
                coedges.insert(local_coedges.begin(), local_coedges.end());
            }
        }
    }
    return Utils::toVect(coedges);
}
/*----------------------------------------------------------------------------*/
void Block::
replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd)
{
    std::vector<Vertex*> vertices = getVertices();
    for (uint i=0; i<vertices.size(); i++)
        if (v1 == vertices[i]) {
            saveBlockTopoProperty(icmd);
            m_topo_property->getVertexContainer()[i] = v2;
        }
    if (propagate_down) {
        std::vector<Face*> faces = getFaces();
    	for (uint j=0; j<faces.size(); j++)
    		faces[j]->replace(v1, v2, propagate_up, propagate_down, icmd);
    }
}
/*----------------------------------------------------------------------------*/
void Block::
replace(Face* f1, Face* f2, Internal::InfoCommand* icmd)
{
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<faces.size(); i++)
        if (f1 == faces[i]){
            saveBlockTopoProperty(icmd);
            m_topo_property->getFaceContainer()[i] = f2;

            f1->saveFaceTopoProperty(icmd);
            f2->saveFaceTopoProperty(icmd);
            f1->remove(this);
            f2->add(this);
        }
}
/*----------------------------------------------------------------------------*/
void Block::
replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd)
{
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<faces.size(); i++)
    	faces[i]->replace(e1, e2, icmd);
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
    std::vector<Vertex*> vertices = getVertices();
    std::vector<Face*> faces = getFaces();

    // recherche du barycentre
    Utils::Math::Point barycentre = getBarycentre();

    points.clear();

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE)){
        indicesFilaire.clear();

        if (vertices.size() == 8 && isStructured()){

            for (uint i=0; i<vertices.size(); i++)
                points.push_back(barycentre + (vertices[i]->getCoord() -  barycentre) * shrink);

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
            std::vector<Topo::Edge* > edges = getEdges();

            for (std::vector<Topo::Edge* >::iterator iter = edges.begin();
                        iter != edges.end(); ++iter) {
                const std::vector<Vertex* >& edge_vertices = (*iter)->getVertices();
                if (edge_vertices.size() == 2){
                    for (uint i=0; i<2; i++){
                        indicesFilaire.push_back(points.size());
                        points.push_back(barycentre + (edge_vertices[i]->getCoord() -  barycentre) * shrink);
                    }
                }
            } // end for iter = edges.begin()
        } // end else if (getNbVertices() == 8)
    }
    else if (tdr->hasRepresentation(Utils::DisplayRepresentation::SOLID)) {

        // représentation à l'aide de 2 triangles par côté
        if (vertices.size() == 8 && isStructured()){
            for (uint i=0; i<vertices.size(); i++)
                points.push_back(barycentre + (vertices[i]->getCoord() -  barycentre) * shrink);

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
            for (uint i=0; i<faces.size(); i++){

                TopoDisplayRepresentation   tr (Utils::DisplayRepresentation::SOLID);
                tr.setShrink(1.0);
                faces[i]->getRepresentation (tr, checkDestroyed);

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
    	Utils::Math::Point orig = (barycentre + (vertices[0]->getCoord() -  barycentre) * shrink);

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
    		TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[1], iCoedges, coedges_dirI);
    		TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[2], jCoedges, coedges_dirJ);
    		TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[4], kCoedges, coedges_dirK);

    		// on va prendre la coedge1 et vect1 qui suivent le sens de la discrétisation (s'il y en a un)
    		CoEdge* coedge1 = coedges_dirI[0];
    		CoEdge* coedge2 = coedges_dirJ[0];
    		CoEdge* coedge3 = coedges_dirK[0];

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

		    const std::vector<Topo::Vertex* >& coedge1_vertices = coedge1->getVertices();
            if (coedge1_vertices[0] == vertices[0])
    			vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    		else if (coedge1_vertices[1] == vertices[0])
    			vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    					points_coedge1[points_coedge1.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<coedge1->getName();
    			throw TkUtil::Exception(messErr);
    		}

		    const std::vector<Topo::Vertex* >& coedge2_vertices = coedge2->getVertices();
    		if (coedge2_vertices[0] == vertices[0])
    			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
    		else if (coedge2_vertices[1] == vertices[0])
    			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
    					points_coedge2[points_coedge2.size()-2]);
    		else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr << "Erreur interne pour l'affichage de la discrétisation ("
    					<<getName()<<"), pb avec arête "<<coedge2->getName();
    			throw TkUtil::Exception(messErr);
    		}

		    const std::vector<Topo::Vertex* >& coedge3_vertices = coedge3->getVertices();
    		if (coedge3_vertices[0] == vertices[0])
    			vect3 = Utils::Math::Vector(points_coedge3[0], points_coedge3[1]);
    		else if (coedge3_vertices[1] == vertices[0])
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


    		points.push_back(vect1*0.2+vect2*(0.4)+vect3*(0.4));
    		points.push_back(vect1*0.4+vect2*0.4+vect3*0.4);
    		points.push_back(vect1*0.8+vect2*0.4+vect3*0.4);
    		points.push_back(vect1*1.0+vect2*(0.4)+vect3*(0.4));

    		points.push_back(vect1*0.2+vect2*(0.4)+vect3*(0.8));
    		points.push_back(vect1*0.4+vect2*0.4+vect3*0.8);
    		points.push_back(vect1*0.8+vect2*0.4+vect3*0.8);
    		points.push_back(vect1*1.0+vect2*(0.4)+vect3*(0.8));

    		points.push_back(vect1*0.2+vect2*(0.8)+vect3*(0.4));
    		points.push_back(vect1*0.4+vect2*0.8+vect3*0.4);
    		points.push_back(vect1*0.8+vect2*0.8+vect3*0.4);
    		points.push_back(vect1*1.0+vect2*(0.8)+vect3*(0.4));

    		points.push_back(vect1*0.2+vect2*(0.8)+vect3*(0.8));
    		points.push_back(vect1*0.4+vect2*0.8+vect3*0.8);
    		points.push_back(vect1*0.8+vect2*0.8+vect3*0.8);
    		points.push_back(vect1*1.0+vect2*(0.8)+vect3*(0.8));


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
    		if (points.size() > 32){
    			indices.push_back(19); indices.push_back(36);
    			indices.push_back(32); indices.push_back(36);
    			indices.push_back(33); indices.push_back(36);
    			indices.push_back(34); indices.push_back(36);
    			indices.push_back(35); indices.push_back(36);
    		}
    	} // end if (isStructured())
    	else {

    		// recherche de 3 vecteurs
    		std::vector<CoEdge* > coedges = getCoEdges();
    		// on cherche les coedges reliées au sommet 0 du bloc
    		std::vector<CoEdge* > coedges_som0;
    		for (uint i=0; i<coedges.size(); i++)
    			if (Utils::contains(vertices[0], coedges[i]->getVertices()))
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

                const std::vector<Topo::Vertex* >& coedge1_vertices = coedge1->getVertices();
    			if (coedge1_vertices[0] == vertices[0])
    				vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			else if (coedge1_vertices[1] == vertices[0])
    				vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    						points_coedge1[points_coedge1.size()-2]);
    			else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Erreur interne pour l'affichage de la discrétisation ("
    						<<getName()<<"), pb avec arête "<<coedge1->getName();
    				throw TkUtil::Exception(messErr);
    			}

    		    const std::vector<Topo::Vertex* >& coedge2_vertices = coedge2->getVertices();
        		if (coedge2_vertices[0] == vertices[0])
        			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
        		else if (coedge2_vertices[1] == vertices[0])
        			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
        					points_coedge2[points_coedge2.size()-2]);
        		else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        			messErr << "Erreur interne pour l'affichage de la discrétisation ("
        					<<getName()<<"), pb avec arête "<<coedge2->getName();
        			throw TkUtil::Exception(messErr);
        		}

        		// cas cycliques
        		if (coedge1_vertices[0] == coedge1_vertices[1]){
        			vect3 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
        			    						points_coedge1[points_coedge1.size()-2]);
        		} else if (coedge1_vertices[0] == coedge1_vertices[1]){
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

    		    const std::vector<Topo::Vertex* >& coedge1_vertices = coedge1->getVertices();
    			if (coedge1_vertices[0] == vertices[0])
    				vect1 = Utils::Math::Vector(points_coedge1[0], points_coedge1[1]);
    			else if (coedge1_vertices[1] == vertices[0])
    				vect1 = Utils::Math::Vector(points_coedge1[points_coedge1.size()-1],
    						points_coedge1[points_coedge1.size()-2]);
    			else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    				messErr << "Erreur interne pour l'affichage de la discrétisation ("
    						<<getName()<<"), pb avec arête "<<coedge1->getName();
    				throw TkUtil::Exception(messErr);
    			}

		        const std::vector<Topo::Vertex* >& coedge2_vertices = coedge2->getVertices();
        		if (coedge2_vertices[0] == vertices[0])
        			vect2 = Utils::Math::Vector(points_coedge2[0], points_coedge2[1]);
        		else if (coedge2_vertices[1] == vertices[0])
        			vect2 = Utils::Math::Vector(points_coedge2[points_coedge2.size()-1],
        					points_coedge2[points_coedge2.size()-2]);
        		else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        			messErr << "Erreur interne pour l'affichage de la discrétisation ("
        					<<getName()<<"), pb avec arête "<<coedge2->getName();
        			throw TkUtil::Exception(messErr);
        		}

    		    const std::vector<Topo::Vertex* >& coedge3_vertices = coedge3->getVertices();
        		if (coedge3_vertices[0] == vertices[0])
        			vect3 = Utils::Math::Vector(points_coedge3[0], points_coedge3[1]);
        		else if (coedge3_vertices[1] == vertices[0])
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

    std::vector<Topo::Vertex* > vtx = getVertices();

    Utils::SerializedRepresentation  vertices ("Sommets topologiques",
            TkUtil::NumericConversions::toStr(vtx.size()));
    for (std::vector<Topo::Vertex*>::iterator iter = vtx.begin( ); vtx.end( )!=iter; iter++)
        vertices.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (vertices);


    if (Internal::InternalPreferences::instance ( )._displayFace.getValue ( )){
    	std::vector<Face* > fa = getFaces();

    	Utils::SerializedRepresentation  faces ("Faces topologiques",
    			TkUtil::NumericConversions::toStr(fa.size()));

    	for (std::vector<Topo::Face*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
    		faces.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (faces);
    }
    else {
    	std::vector<CoFace* > fa = getCoFaces();

    	Utils::SerializedRepresentation  cofaces ("Faces topologiques",
    			TkUtil::NumericConversions::toStr(fa.size()));

    	for (std::vector<Topo::CoFace*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
    		cofaces.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (cofaces);
    }

    description->addPropertiesSet (topoRelation);

    std::vector<Group::Group3D*> grp = getGroups();
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
       std::vector<Face*> faces = getFaces();
       if (faces.size() == 6){
           // il faut de plus que les faces soient structurables
           for (uint i=0; i<6; i++)
               if (!faces[i]->structurable())
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

	throw TkUtil::Exception (TkUtil::UTF8String ("Non structuration d'un bloc non prévue", TkUtil::Charset::UTF_8));

    // propage la non-structuration aux faces autant que possible
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<6; i++)
        if (faces[i]->unstructurable())
            faces[i]->unstructure(icmd);
}
/*----------------------------------------------------------------------------*/
void Block::setStructured(Internal::InfoCommand* icmd, bool str)
{
    if (str)
        throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'un bloc non prévue", TkUtil::Charset::UTF_8));

    if (isStructured()){
		throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'un bloc non prévue", TkUtil::Charset::UTF_8));
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

    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<6; i++)
        faces[i]->structure(icmd);

    // Réordonne les Faces comme pour Bibop3D
    // cela suppose que les sommets soit ordonnés
    std::vector<Face* > sorted_faces;

    // On tient compte du cas avec bloc à 5 ou 6 sommets
    std::vector<Topo::Vertex* > vertices = getHexaVertices();

    for (uint i=0; i<6; i++){
        sorted_faces.push_back(getFace(
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][0]],
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][1]],
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][2]],
        		vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][3]]));

    }

    m_topo_property->getFaceContainer().clear();
    Utils::append(m_topo_property->getFaceContainer(), sorted_faces);

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
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<faces.size(); i++)
        faces[i]->check();

    if (getGeomAssociation() && getGeomAssociation()->isDestroyed()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur avec le bloc "<<getName()
                <<", il pointe sur "<<getGeomAssociation()->getName()
                <<" qui est détruit !";
        throw TkUtil::Exception(messErr);
    }

    if (isStructured()){
        // les faces doivent être structurées
        for (uint i=0; i<faces.size(); i++)
            if (!faces[i]->isStructured()) {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec le bloc structuré "<<getName()
                        <<", sa face "<<faces[i]->getName()
                        <<" n'est pas structurée";
                throw TkUtil::Exception(messErr);
            }

        // tableau qui donne les 2 directions par face pour les 2 arêtes obtenues à partir
        // des 3 premiers indices de TopoHelper::tabIndVtxByFaceOnBlock
        uint tabDirAretesFace[6][2] = {{2, 1}, {2, 1}, {2, 0}, {2, 0}, {1, 0}, {1, 0}};

        // construction d'un tableau avec les 8 sommets en duplicant les sommets en cas de dégénérescence
        std::vector<Topo::Vertex* > sommets = getHexaVertices();

        // calcul le nombre de bras pour chacune des directions
        uint nbBrasDir[3];
        getNbMeshingEdges(nbBrasDir[0], nbBrasDir[1], nbBrasDir[2]);

        // on vérifie que chacune des Face a le même nombre de bras que ce que l'on a trouvé pour le bloc
        for (uint iFace=0; iFace<faces.size(); iFace++){
            Topo::Vertex* v0 = sommets[TopoHelper::tabIndVtxByFaceOnBlock[iFace][0]];
            Topo::Vertex* v1 = sommets[TopoHelper::tabIndVtxByFaceOnBlock[iFace][1]];
            uint nb1, nb2;
            faces[iFace]->getNbMeshingEdges(nb1, nb2);
            Face::eDirOnFace dirFace = faces[iFace]->getDir(v0, v1);
            // on permute si nécessaire pour que le premier (nb1) corresponde à la première direction tabDirAretesFace
            if (dirFace == Face::j_dir){
                uint nb_tmp = nb1;
                nb1 = nb2;
                nb2 = nb_tmp;
            }

            if (nb1 != nbBrasDir[tabDirAretesFace[iFace][0]]){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec la face structurée "<<faces[iFace]->getName()
                        << " dans le bloc " <<getName()
                        <<", le premier côté n'a pas le même nombre de bras: "
                        << (short)nb1
                        <<" (dans la face) et "<< (short)nbBrasDir[tabDirAretesFace[iFace][0]]
                        << " (dans le bloc)";
                throw TkUtil::Exception(messErr);
            }

            if (nb2 != nbBrasDir[tabDirAretesFace[iFace][1]]){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec la face structurée "<<faces[iFace]->getName()
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
std::vector<Vertex*> Block::
getHexaVertices() const
{
    std::vector<Vertex*> vertices = getVertices();
    std::vector<Vertex*> sommets(8);
    if (vertices.size() == 8){
        for (uint i=0; i<8; i++)
            sommets[i] = vertices[i];
    } else if (vertices.size() == 5){
        for (uint i=0; i<5; i++)
            sommets[i] = vertices[i];
        sommets[5] = sommets[4];
        sommets[6] = sommets[4];
        sommets[7] = sommets[4];
    } else if (vertices.size() == 6){
        for (uint i=0; i<4; i++)
            sommets[i] = vertices[i];

        std::vector<Face*> faces = getFaces();
        if (faces[0]->getVertices().size() == 3 && faces[1]->getVertices().size() == 3){
            sommets[4] = vertices[4];
            sommets[5] = vertices[5];
            sommets[6] = vertices[4];
            sommets[7] = vertices[5];
        } else if (faces[2]->getVertices().size() == 3 && faces[3]->getVertices().size() == 3){
            sommets[4] = vertices[4];
            sommets[5] = vertices[4];
            sommets[6] = vertices[5];
            sommets[7] = vertices[5];
        } else
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec Block::getHexaVertices(), nb de sommets non prévus dans une des faces", TkUtil::Charset::UTF_8));

    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec Block::getHexaVertices(), nb de sommets non prévus", TkUtil::Charset::UTF_8));

    return sommets;
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
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<faces.size(); i++) {
        uint nb_marques = 0;
        const std::vector<Topo::Vertex* > & local_vertices = faces[i]->getVertices();
        for (std::vector<Topo::Vertex* >::const_iterator iter2 = local_vertices.begin();
                iter2 != local_vertices.end(); ++iter2) {
            if (filtre_sommets[*iter2] == 1)
                nb_marques+=1;
        }
         if (nb_marques == nbVtx)
             face_ok = faces[i];
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

        Group::GroupManager& gm = b.getContext().getGroupManager();
        std::vector<Group::GroupEntity*> gn = gm.getGroupsFor(b.getGeomAssociation());
        if (!gn.empty()) {
            o << " (groupes:";
            for (size_t i=0; i<gn.size(); i++)
                o << " "<<gn[i]->getName();
            o << ")";
        }
        else {
            o << " (sans groupe)";
        }
    }
    else
        o << ", sans projection";

    const std::vector<Topo::Face*>& faces = b.getFaces();
    o << " avec "<<(short)faces.size()<<" faces:\n";
    for (uint i=0; i<faces.size();i++)
        o << " "<<faces[i]->getName()<<"\n";
//      o << *faces[i]<<"\n";
    o << "\n";

    const std::vector<Topo::Vertex*> & vertices = b.getVertices();
    o << b.getName()<< " bloc avec "<<(short)vertices.size()<<" sommets:\n";
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
    infos._groups = Utils::toNames(getGroups());
	if (getGeomAssociation() != 0)
		infos.geom_entity = getGeomAssociation()->getName();

    const std::vector<Vertex*>& vertices = getVertices();
    infos._vertices = Utils::toNames(vertices);

    std::vector<CoEdge*> coedges = getCoEdges();
    infos._coedges = Utils::toNames(coedges);
    
    std::vector<CoFace*> cofaces = getCoFaces();
    infos._cofaces = Utils::toNames(cofaces);

    return infos;
}
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
    const std::vector<Topo::Face*>& faces = getFaces();
    for(Face* face : faces)
        for(CoFace* coface : face->getCoFaces())
            for(Edge* edge : coface->getEdges())
                for(CoEdge* coedge : edge->getCoEdges())
                    filtre_coedge[coedge] = 1;

    // tableau de 3 vecteurs de CoEdges pour le stockage temporaire
    std::vector<CoEdge* > tabCoedges[3];
    uint indDir1 = 0;
    uint indDir2 = 1;

    // ordre des faces pour un parcours avec des faces ayant une coedge en commun
    const uint tabIndFace[6] = {4, 0, 2, 1, 3, 5};
    for (uint i=0; i<faces.size(); i++){
        Face* face = faces[tabIndFace[i]];

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
    const std::vector<Vertex*>& vertices = getVertices();
    std::vector<CoEdge* > coedges_between; // inutilisé
    for (uint i=0; i<3 && iCoedges.empty(); i++)
        if (TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[1], tabCoedges[i], coedges_between))
            iCoedges = tabCoedges[i];

    for (uint i=0; i<3 && jCoedges.empty(); i++)
        if (TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[2], tabCoedges[i], coedges_between))
            jCoedges = tabCoedges[i];

    for (uint i=0; i<3 && kCoedges.empty(); i++)
        if (TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[4], tabCoedges[i], coedges_between))
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
    const std::vector<Face*>& faces = getFaces();
    faces[0]->getNbMeshingEdges(nbI_face0, nbJ_face0, accept_error);
    faces[4]->getNbMeshingEdges(nbI_face4, nbJ_face4, accept_error);

    const std::vector<Vertex*>& vertices = getVertices();
    Face::eDirOnFace dirI = faces[4]->getDir(vertices[0], vertices[1]);
    Face::eDirOnFace dirK = faces[0]->getDir(vertices[0], vertices[4]);

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
        if (m_save_topo_property == 0){
        	if (change && isMeshed()){
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
    const std::vector<Face*>& faces = getFaces();
    for (uint i=0; i<faces.size() && !faceATrouvee; i++){
        Face* face = faces[i];
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
    const std::vector<Face*>& Bfaces = bloc_B->getFaces();
    for (uint i=0; i<Bfaces.size() && !faceBTrouvee; i++){
        Face* face = Bfaces[i];
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
    std::vector<CoFace* > cofaces = getCoFaces();
    for (std::vector<CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter)
        filtre_coface[*iter] = 1;

    cofaces = arete->getCoFaces();
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
    for (Topo::CoEdge* ce : coface->getCoEdges())
        if (filtre_coedge[ce] != marque_arete)
            coedge = ce;

    if (0 == coedge)
           throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas d'arête dans une autre direction et dans une même coface", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    const std::vector<Vertex*>& ce_vertices = coedge->getVertices();
    const std::vector<Vertex*>& ar_vertices = arete->getVertices();
    if (ce_vertices[0] == ar_vertices[0]
    		|| ce_vertices[0] == ar_vertices[1])
    	split(coedge, 0.0, newBlocs, icmd);
    else if (ce_vertices[1] == ar_vertices[0]
    		|| ce_vertices[1] == ar_vertices[1])
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
    std::vector<CoFace* > blk_cofaces = getCoFaces();

    // les Edges qui coupent le bloc en deux
    std::vector<Edge* > splitingEdges;

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
   // le découpage des faces communes (qui doivent former une boucle)
    TopoHelper::splitFaces2D(blk_cofaces, arete, ratio, 0, true, false, splitingEdges, icmd);
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<" TopoHelper::splitFaces en "<<timer.strDuration()<<std::endl;
#endif

    // les faces qui ne sont pas concernées par la coupe
    uint ind_face0 = dirSplit*2;
    const std::vector<Face*>& faces = getFaces();
    Face* face0 = faces[ind_face0];
    const std::vector<Vertex*>& vertices = getVertices();
    Face* face1 = (vertices.size()!=8 && dirSplit==k_dir?0:faces[dirSplit*2+1]);
    Face* faceI[4];
    Face* newFaceI[4][2];
#ifdef _DEBUG_SPLIT
    std::cout<<"face0 : "<<face0->getName()<<std::endl;
#endif
    // nombre de Faces impactées par la coupe
    uint nbFacesCoupees = face0->getVertices().size();

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    for (uint i=0; i<nbFacesCoupees; i++){

        if (tabIndFaceParDir[dirSplit][i]<faces.size()){

            // la face que l'on coupe
            faceI[i] = faces[tabIndFaceParDir[dirSplit][i]];

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
        if (i == 0 && vertices.size() != 8)
            // on évite le sommet du bloc s'il est dégénéré
            vtx = vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]];
        else
            vtx = vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][i]];
#ifdef _DEBUG_SPLIT
        std::cout <<"sommet "<<vtx->getName()<<" pour ordonner les 2 faces : "
                <<newFaceI[i][0]->getName()<<" et "<<newFaceI[i][1]->getName()<<std::endl;
#endif

        if (Utils::contains(vtx, newFaceI[i][0]->getVertices())){
            // rien à faire
        } else if (Utils::contains(vtx, newFaceI[i][1]->getVertices())) {
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
        if (vertices.size() == 6 && ind_face0 == 0 && i == 3)
            vtx1 = vertices[5];
        else
            vtx1 = vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][i]];

        if (vertices.size() == 6 && nbFacesCoupees == 4 && ind_face0 == 0 && i == 2)
            vtx2 = vertices[5];
        else
            vtx2 = vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][(i+1)%nbFacesCoupees]];

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
    if (vertices.size() == 8){
        // cas du bloc hexaédrique
        vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]]);
        vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]]);
        vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][3]]);
        vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]]);
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


    } else if (vertices.size() == 6){
        // cas du bloc avec dégénérescence en prisme

        if (nbFacesCoupees == 3){
            // cas avec une coupe des 3 faces quadrangulaires
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]]);
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]]);
            vertices_1.push_back(splitingVertices[2]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]]);
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
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]]);
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]]);
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

            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][3]]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]]);
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

    } else if (vertices.size() == 5){
        // cas du bloc avec dégénérescence en pyramide

        if (nbFacesCoupees == 4) {
            // cas avec la coupe parallèle à la base, donc on coupe 4 quad
            faces_1.push_back(newFaceI[3][0]);
            faces_1.push_back(newFaceI[1][0]);
            faces_1.push_back(newFaceI[0][0]);
            faces_1.push_back(newFaceI[2][0]);
            faces_1.push_back(face0);
            faces_1.push_back(new_face_1);

            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][3]]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]]);
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
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][1]]);
            vertices_1.push_back(splitingVertices[1]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][2]]);
            vertices_1.push_back(splitingVertices[2]);
            vertices_1.push_back(vertices[TopoHelper::tabIndVtxByFaceOnBlock[ind_face0][0]]);

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

    for (Group::Group3D* gr : m_topo_property->getGroupsContainer()){
    	gr->add(block_1);
    	gr->add(block_2);
    	block_1->add(gr);
    	block_2->add(gr);
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
    // on clone la méthode de discrétiasation
    block_1->switchBlockMeshingProperty(icmd, getBlockMeshingProperty());
    block_2->switchBlockMeshingProperty(icmd, getBlockMeshingProperty());
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
    const std::vector<Face*>& faces = getFaces();
    for(Face* face : faces){
        for(CoFace* coface : face->getCoFaces()){
            // la CoFace est-elle entre 2 blocs ?
            bool coFaceInterne = false;
            if (coface->getFaces().size() == 2){
                filtre_cofaces[coface] = 2;
                coFaceInterne = true;
            }
            else {
                filtre_cofaces[coface] = 1;
                coFaceInterne = false;
            }

            for(Edge* edge : coface->getEdges())
                for(CoEdge* coedge : edge->getCoEdges())
                    if (coFaceInterne)
                        filtre_coedges[coedge] = 2;
                    else if (filtre_coedges[coedge] != 2)
                        filtre_coedges[coedge] = 1;
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
    const std::vector<Vertex*>& vertices = getVertices();
    for (uint f=0; f<4; f++)
        if (tabIndFaceParDir[dir][f]<faces.size()){
            Face* face = faces[tabIndFaceParDir[dir][f]];

            // on recherche la direction touchée par le déraffinement
            uint indFace = f;
            if (f == 2)
                indFace = 1;

            Face::eDirOnFace dirLoc = face->getDir(vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir][indFace][0]],
                    vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir][indFace][1]]);

            for(uint i=0; i < face->getCoFaces().size(); i++){
                CoFace* coface = face->getCoFaces()[i];
                if (filtre_cofaces[coface] == 2){
                    int old_ratio = face->getRatio(coface, dirLoc);
                    face->saveFaceMeshingProperty(icmd);
                    face->setRatio(coface, old_ratio*ratio, dirLoc);
                }
                else {
                    for (Edge* edge : coface->getEdges()){
                        std::vector<CoEdge* > coedges = edge->getCoEdges();
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

                                if (edge->getCoFaces().size() != 1){
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

    // suppression des relations remontantes des faces vers ce block
    for (Face* face : getFaces()) {
        face->saveFaceTopoProperty(icmd);
        if (Utils::contains(this, face->getBlocks()))
            face->remove(this);
    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
void Block::
degenerateFaceInVertex(uint id, Internal::InfoCommand* icmd)
{
    // la face qui disparait
    Face* face = getFaces()[id];

    if (face->getCoFaces().size() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Block::degenerateFaceInEdge pour une face composée", TkUtil::Charset::UTF_8));

    // la coface qui disparait
     CoFace* coface = face->getCoFaces()[0];

     if (coface->getVertices().size() != 4)
         throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Block::degenerateFaceInVertex pour une face avec autre chose que 4 sommets", TkUtil::Charset::UTF_8));

    // sauvegarde pour undo ou retour en arrière en cas d'échec
    saveBlockTopoProperty(icmd);

    // permutation de la topologie pour que la face id soit en k_max
    permuteToKmaxFace(id, icmd);

    // les fusions de sommets des arêtes (qui disparaissent de la face)

    const std::vector<Edge* >& edges = coface->getEdges();
    Edge* a0 = edges[0];
    Edge* a2 = edges[2];
    a0->collapse(icmd);
    a2->collapse(icmd);
    Edge* a1 = edges[0];
    Edge* a3 = edges[1];
    a1->merge(a3, icmd, false);
    a1->collapse(icmd);

    coface->free(icmd);
    face->free(icmd);
    Utils::remove(face, m_topo_property->getFaceContainer());

    // suppression des sommets de la liste
    m_topo_property->getVertexContainer().resize(5);

    // des fois que ...
    if (getVertices()[4]->isDestroyed())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Block::degenerateFaceInVertex, le sommet conservé est détruit", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Block::
degenerateFaceInEdge(uint id, Topo::Vertex* v1, Topo::Vertex* v2,
            Internal::InfoCommand* icmd)
{
	// comme pour degenerateFaceInVertex, mais sans le dernier collapse

	// la face qui disparait
    Face* face = getFaces()[id];

	if (face->getCoFaces().size() != 1)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Block::degenerateFaceInEdge pour une face composée", TkUtil::Charset::UTF_8));

	// la coface qui disparait
	CoFace* coface = face->getCoFaces()[0];

	// sauvegarde pour undo ou retour en arrière en cas d'échec
	saveBlockTopoProperty(icmd);

	// permutation de la topologie pour que la face id soit en k_max
	permuteToKmaxFace(id, icmd);

	// les fusions de sommets des arêtes (qui disparaissent de la face)
	Edge* a0 = coface->getEdge(v1,v2);
	Edge* a2 = coface->getOppositeEdge(a0);
	a0->collapse(icmd);
	a2->collapse(icmd);

    const std::vector<Edge* > edges = coface->getEdges();
	Edge* a1 = edges[0];
	Edge* a3 = edges[1];
	a1->merge(a3, icmd, false);

	coface->free(icmd);
	face->free(icmd);
	Utils::remove(face, m_topo_property->getFaceContainer());

	// suppression des sommets de la liste
	// tout d'abord, on évite de se retrouver avec 2 sommets identiques dans la liste restante
    const std::vector<Vertex*> vertices = getVertices();
	if (vertices[4] == vertices[5])
		m_topo_property->getVertexContainer()[5] = vertices[6];
	m_topo_property->getVertexContainer().resize(6);

	// des fois que ...
	if (vertices[4]->isDestroyed() || vertices[5]->isDestroyed())
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

    if (getVertices().size() != 8)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace n'est possible qu'avec un bloc structuré à 8 sommets", TkUtil::Charset::UTF_8));

    if (getFaces().size() != 6)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace n'est possible qu'avec un bloc structuré à 6 faces", TkUtil::Charset::UTF_8));

    // un filtre sur les arêtes pour passer des sommets vers les aretes sans sortir du bloc ou de la face
    std::map<Edge*, uint> filtre_aretes;
    // un filtre sur les sommets pour éviter de trouver les edges qui sont toutes en double depuis l'utilisation des CoEdge
    std::map<Topo::Vertex*, uint> filtre_sommets;

    // les arêtes du bloc
    for (Edge* edge : getEdges())
        filtre_aretes[edge] = 1;

    // les arêtes de la face à mettre en k_max,
    // en tenant compte des arêtes qui s'appuient sur les coedges, dans d'autres faces
    // donc on passe par toutes les coedges
    Face* face_id = getFaces()[id];
    for (CoEdge* coedge : face_id->getCoEdges())
        for (Edge* e : coedge->getEdges())
            filtre_aretes[e] = 3;

    // les arêtes de la face à mettre en k_max, strictement dans la face
    for (Edge* edge : face_id->getEdges())
        filtre_aretes[edge] = 2;

    for (Topo::Vertex* v : face_id->getVertices())
        filtre_sommets[v] = 2;

    // on se donne un sommet de départ (l'indice 7 car on veut mettre la face en k_max)
    Topo::Vertex* p7 = face_id->getVertices()[0];

    // recherche de 2 arêtes marqués dans la face parmis les arêtes reliées au sommet de départ
    std::vector<Edge* > aretesTriedre;
    for (Edge* p7_edge : p7->getEdges())
        if (filtre_aretes[p7_edge] == 2)
            aretesTriedre.push_back(p7_edge);

    if (aretesTriedre.size() != 2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace ne trouve pas 2 arêtes dans la face depuis un des points", TkUtil::Charset::UTF_8));

    // recherche d'1 arête marquée dans le bloc mais hors de la face
    for (Edge* p7_edge : p7->getEdges())
        if (filtre_aretes[p7_edge] == 1){
            Topo::Vertex* som_op = p7_edge->getOppositeVertex(p7);
            if (filtre_sommets[som_op] != 2){
                aretesTriedre.push_back(p7_edge);
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
    Edge* arete7 = face_id->getOppositeEdge(face_id->getEdge(p7, p5));
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

    for (Topo::Vertex* vtx : somDep) {
        // recherche d'1 arête marqué dans le bloc mais hors de la face
        Edge* arete = 0;

        for (Edge* vtx_edge : vtx->getEdges())
            if (filtre_aretes[vtx_edge] == 1)
                arete = vtx_edge;

        if (!arete)
               throw TkUtil::Exception (TkUtil::UTF8String ("Block::permuteToKmaxFace ne trouve pas 1 arête dans le bloc, hors de la face, depuis un sommet donné", TkUtil::Charset::UTF_8));

        somOpp.push_back(arete->getOppositeVertex(vtx));
#ifdef _DEBUG_PERM
        std::cout<<" getOppositeVertex dans l'arete "<<arete->getName()
                << " pour sommet "<<vtx->getName()
                <<" donne sommet "<<somOpp.back()->getName()<<std::endl;
#endif
    }

#ifdef _DEBUG_PERM
    std::cout<<"p0 = "<<somOpp[0]->getName()<<std::endl;
    std::cout<<"p1 = "<<somOpp[1]->getName()<<std::endl;
    std::cout<<"p2 = "<<somOpp[2]->getName()<<std::endl;
#endif
    // les sommets réordonnés
    m_topo_property->getVertexContainer()[0] = somOpp[0];
    m_topo_property->getVertexContainer()[1] = somOpp[1];
    m_topo_property->getVertexContainer()[2] = somOpp[2];
    m_topo_property->getVertexContainer()[3] = p3;
    m_topo_property->getVertexContainer()[4] = p4;
    m_topo_property->getVertexContainer()[5] = p5;
    m_topo_property->getVertexContainer()[6] = p6;
    m_topo_property->getVertexContainer()[7] = p7;

    // Réordonne les Faces
    std::vector<Face* > sorted_faces;
    const std::vector<Vertex*>& vertices = getVertices();
    for (uint i=0; i<6; i++){
        sorted_faces.push_back(getFace(
                vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][0]],
                vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][1]],
                vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][2]],
                vertices[TopoHelper::tabIndVtxByFaceOnBlock[i][3]]));

    }

    m_topo_property->getFaceContainer().clear();
    Utils::append(m_topo_property->getFaceContainer(), sorted_faces);

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
    std::vector<Vertex*> vertices = getVertices();
	if (7 != vertices.size())
		throw TkUtil::Exception (TkUtil::UTF8String ("Block::reverseOrder7Vertices n'est pas possible avec autre chose qu'un bloc à 7 sommets", TkUtil::Charset::UTF_8));

    std::vector<Face*> faces = getFaces();
	if (6 != faces.size())
		throw TkUtil::Exception (TkUtil::UTF8String ("Block::reverseOrder7Vertices n'est pas possible avec autre chose qu'un bloc à 6 côtés", TkUtil::Charset::UTF_8));

	// identification de l'indice du sommet à la dégénérescence
	// c'est le sommet commun aux deux faces triangulaires

	// les faces triangulaires
	std::vector<Face*> faces_tri;
	for (uint i=0; i<faces.size(); i++)
		if (3 == faces[i]->getVertices().size())
			faces_tri.push_back(faces[i]);

	if (2 != faces_tri.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "La fonction reverseOrder7Vertices ne trouve pas 2 faces triangulaires dans "<<getName()
				<<" mais en trouve "<<(short)faces_tri.size();
		throw TkUtil::Exception(messErr);
	}

	Vertex* vtx_deg = TopoHelper::getCommonVertex(faces_tri[0], faces_tri[1]);
	uint ind_deg = Utils::getIndexOf(vtx_deg, vertices);

	// identification des indices des 2 sommets opposés à ce sommet à la dégénérescence
	// il s'agit des sommets opposés au sommet deg / face quadrangulaires

	// les faces quad qui contiennent vtx_deg
	std::vector<Face*> faces_quad;
	for (uint i=0; i<faces.size(); i++)
		if (4 == faces[i]->getVertices().size()){
			std::vector<Vertex*> vertices = faces[i]->getVertices();

			if (std::find(vertices.begin(), vertices.end(), vtx_deg) != vertices.end())
				faces_quad.push_back(faces[i]);
		}



	// 4 cas possibles suivant indices des sommets
	if (2 != faces_quad.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "La fonction reverseOrder7Vertices ne trouve pas 2 faces quadrangulaires en relation avec le sommet "
				<<vtx_deg->getName()<<" dans "<<getName()<<" mais en trouve "<<(short)faces_quad.size();
		throw TkUtil::Exception(messErr);
	}

    auto fa1_vertices = faces_quad[0]->getVertices();
    int if1 = Utils::getIndexOf(vtx_deg, fa1_vertices);
	Vertex* vtx_opp1 = fa1_vertices[(if1+2)%4];
	uint ind_opp1 = Utils::getIndexOf(vtx_opp1, vertices);

    auto fa2_vertices = faces_quad[1]->getVertices();
    int if2 = Utils::getIndexOf(vtx_deg, fa2_vertices);
    Vertex* vtx_opp2 = fa2_vertices[(if2+2)%4];
	uint ind_opp2 = Utils::getIndexOf(vtx_opp2, vertices);

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
	getBlockTopoProperty()->getVertexContainer() = loc_vtx;

	// idem avec les faces
	getBlockTopoProperty()->getFaceContainer() = loc_faces;

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

    std::vector<Topo::Face* > faces = getFaces();
    for (std::vector<Topo::Face* >::iterator iter1 = faces.begin();
            iter1 != faces.end(); ++iter1){
        Topo::Face* face = *iter1;

        Topo::Block* bloc_opp = face->getOppositeBlock(this);

        for (uint j=0; j<face->getCoFaces().size(); j++){
            Topo::CoFace* coface = face->getCoFaces()[j];

            // cas où on affecte une méthode structurée
            if (new_ppty->getMeshLaw() == BlockMeshingProperty::transfinite) {
                FaceMeshingPropertyTransfinite* prop = new FaceMeshingPropertyTransfinite();
                CoFaceMeshingProperty* old_prop = coface->setProperty(prop);
                delete old_prop;
            } else if (new_ppty->getMeshLaw() == BlockMeshingProperty::delaunayTetgen){
                // si méthode non structuré, l'affecte à la face si le bloc voisin n'est pas structuré
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
    std::vector<Face*> faces = getFaces();
    for (uint i=0; i<faces.size(); i++){
        Face* face = faces[i];

        // recherche de la présence de ratios sur les cofaces
        bool haveRatio = false;
        for (CoFace* coface : face->getCoFaces()){
            uint ratio_i = face->getRatio(coface, 0);
            uint ratio_j = face->getRatio(coface, 1);

            if (ratio_i > 1 || ratio_j > 1)
                haveRatio = true;
        }

        if (haveRatio){
#ifdef _DEBUG2
           std::cout<<face->getName()<<" possède au moins un ratio"<<std::endl;
#endif
            // construction d'une table pour la face qui pour une coedge donne la doirection dans la face
            std::vector<CoEdge* > dirCoedges[2];
            face->getOrientedCoEdges(dirCoedges[0], dirCoedges[1]);
            std::map<CoEdge*,short> coedge2dir;
            for (uint dir=0; dir<2; dir++)
                for (CoEdge* coedge : dirCoedges[dir])
                    coedge2dir[coedge] = dir;

            for (CoFace* coface : face->getCoFaces()){
                uint ratio_dir[2];
                for (uint dir=0; dir<2; dir++)
                    ratio_dir[dir] = face->getRatio(coface, dir);

                for (uint dir=0; dir<2; dir++){
                    for (uint cote=0; cote<2; cote++){
                        uint ind = cote*2 + dir;
                        // on évite la 4ème arête d'une coface dégénérée
                        const std::vector<Edge* > cf_edges = coface->getEdges();
                        if (ind < cf_edges.size()){
                            Edge* edge =  cf_edges[ind];

                            for (CoEdge* coedge : edge->getCoEdges()){
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
                            } // end for coedge
                        } // end if (ind < coface->getNbEdges())
                    } // end for cote<2
                } // end for dir<2
            } // end for coface

        } // end if (haveRatio)
        else {
            // on évite ici l'appel à getOrientedCoEdges qui coûte un peu

            // recherche des ratios sur les coedges
            for (CoFace* coface : face->getCoFaces())
                for (Edge* edge : coface->getEdges())
                    for (CoEdge* coedge : edge->getCoEdges())
                        if (ratios[coedge] == 0){
                            uint ratio = edge->getRatio(coedge);
                            ratios[coedge] = ratio;
#ifdef _DEBUG2
                            std::cout<<coedge->getName()<<" a pour ratio "<<ratio<<std::endl;
#endif
                        }
        } // end else / if (haveRatio)
    } // end for i<getNbFaces()
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Block::getBarycentre() const
{
	Utils::Math::Point barycentre;

	std::vector<Topo::Vertex* > vertices = getVertices();
	for (uint i=0; i<vertices.size(); i++)
		barycentre += vertices[i]->getCoord();
	barycentre /= (double)vertices.size();
	return barycentre;
}
/*----------------------------------------------------------------------------*/
void Block::getGroupsName (std::vector<std::string>& gn) const
{
    TopoEntity::getGroupsName(gn);

    for (auto gr : m_topo_property->getGroupsContainer())
        gn.push_back(gr->getName());
}
/*----------------------------------------------------------------------------*/
void Block::add(Group::Group3D* grp)
{
    m_topo_property->getGroupsContainer().push_back(grp);
}
/*----------------------------------------------------------------------------*/
void Block::remove(Group::Group3D* grp)
{
    Utils::remove(grp, m_topo_property->getGroupsContainer());
}
/*----------------------------------------------------------------------------*/
int Block::getNbGroups() const
{
    return m_topo_property->getGroupsContainer().size();
}
/*----------------------------------------------------------------------------*/
std::vector<Group::Group3D*> Block::getGroups() const
{
    return m_topo_property->getGroupsContainer();
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
    auto groups = m_topo_property->getGroupsContainer();
#ifdef _DEBUG2
    std::cout<<"Les groupes:";
    for  (uint i=0; i<groups.size(); i++)
		 std::cout<<" "<<groups.get(i)->getName();
    std::cout<<std::endl;
#endif


     if (b)
    	 for (uint i=0; i<groups.size(); i++){
    		 Group::Group3D* gr = groups[i];
    		 gr->remove(this);
    	 }
     else
    	 for (uint i=0; i<groups.size(); i++){
    		 Group::Group3D* gr = groups[i];
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

        std::vector<Vertex*> vertices = getVertices();
		for (uint i=0;i<vertices.size(); i++)
			res -= vertices[i]->getNbInternalMeshingNodes();

		std::vector<CoEdge*> coedges = getCoEdges();
		for (uint i=0;i<coedges.size(); i++)
			res -= coedges[i]->getNbInternalMeshingNodes();

		std::vector<CoFace*> cofaces = getCoFaces();
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
