/*----------------------------------------------------------------------------*/
#include "Geom/ImportBREPImplementationForPairing.h"
#include "Geom/OCCHelper.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomSplitImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopExp_Explorer.hxx>
#include <ShapeFix_Shape.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportBREPImplementationForPairing::
ImportBREPImplementationForPairing(Internal::Context& c, Internal::InfoCommand* icmd,
		const std::string& n)
: GeomModificationBaseClass(c)
, m_group_helper(*icmd, c.getGroupManager())
, m_icmd(icmd)
, m_filename(n)
{
}
/*----------------------------------------------------------------------------*/
ImportBREPImplementationForPairing::~ImportBREPImplementationForPairing()
{}
/*----------------------------------------------------------------------------*/
void ImportBREPImplementationForPairing::readFile()
{
    // check file extension
    std::string suffix = m_filename;
    int suffix_start = m_filename.find_last_of(".");
    suffix.erase(0,suffix_start+1);
    if (suffix != "brep")
        throw TkUtil::Exception (TkUtil::UTF8String ("Mauvaise extension de fichier BREP (.brep)", TkUtil::Charset::UTF_8));

    // read file
    std::ifstream file(m_filename);
    if (!file.is_open()) {
        throw TkUtil::Exception (TkUtil::UTF8String ("Impossible d'ouvrir ce fichier BREP", TkUtil::Charset::UTF_8));
    }
    
    BRep_Builder builder;
    BRepTools::Read(m_importedShape, file, builder);
    std::cout << "-> Après import" << std::endl;
    OCCHelper::displayShapeInfo(m_importedShape);

    //m_importedShape = OCCHelper::cleanShape(m_importedShape);
    //std::cout << "-> Après clean" << std::endl;
    //OCCHelper::displayShapeInfo(m_importedShape);

    //double tolerance = 1e-6;
    //m_importedShape = mergeVerticesSafe(m_importedShape, tolerance);
    //m_importedShape = removeDegeneratedEdges(m_importedShape);
    //std::cout << "-> Après removeDegeneratedEdges" << std::endl;
    //OCCHelper::displayShapeInfo(m_importedShape);
    //shape = fixShape(shape);
    //std::cout << "-> apres fix " << std::endl;
    //displayShapeInfo(shape);
}

TopoDS_Shape ImportBREPImplementationForPairing::mergeVerticesSafe(const TopoDS_Shape& shape, double tol)
{
    TopTools_IndexedMapOfShape vertexMap;
    TopExp::MapShapes(shape, TopAbs_VERTEX, vertexMap);

    Handle(ShapeBuild_ReShape) reshaper = new ShapeBuild_ReShape;
    std::vector<TopoDS_Vertex> masters;

    for (int i = 1; i <= vertexMap.Extent(); i++)
    {
        TopoDS_Vertex v = TopoDS::Vertex(vertexMap(i));
        gp_Pnt p;

        // Vérification si le vertex a un gp_Pnt
        try {
            p = BRep_Tool::Pnt(v);
        } 
        catch (Standard_Failure& e) 
        {
            std::cout << "Vertex sans gp_Pnt, ignoré: " << e.GetMessageString() << std::endl;
            continue;
        }

        bool merged = false;
        for (auto& m : masters)
        {
            gp_Pnt pm = BRep_Tool::Pnt(m);
            if (p.Distance(pm) <= tol)
            {
                reshaper->Replace(v, m);
                merged = true;
                break;
            }
        }

        if (!merged)
            masters.push_back(v);
    }

    return reshaper->Apply(shape);
}

TopoDS_Shape ImportBREPImplementationForPairing::removeDegeneratedEdges(const TopoDS_Shape& shape)
{
    Handle(ShapeBuild_ReShape) reshaper = new ShapeBuild_ReShape;

    for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
    {
        TopoDS_Edge e = TopoDS::Edge(ex.Current());

        TopoDS_Vertex v1 = TopExp::FirstVertex(e);
        TopoDS_Vertex v2 = TopExp::LastVertex(e);

        if (!v1.IsNull() && !v2.IsNull() && v1.IsSame(v2))
        {
            reshaper->Remove(e);
        }
    }

    return reshaper->Apply(shape);
}

TopoDS_Shape ImportBREPImplementationForPairing::fixShape(const TopoDS_Shape& shape)
{
    Handle(ShapeFix_Shape) fix = new ShapeFix_Shape(shape);
    try { fix->Perform(); } 
    catch (Standard_Failure& e) { std::cout << e.GetMessageString() << std::endl; }

    return fix->Shape();
}

TopoDS_Shape ImportBREPImplementationForPairing::cleanShape(const TopoDS_Shape& shape)
{
    Handle(ShapeBuild_ReShape) reshaper = new ShapeBuild_ReShape;
    
    TopTools_IndexedMapOfShape occSolidMap;   
    TopExp::MapShapes(shape, TopAbs_SOLID, occSolidMap);

    TopTools_IndexedMapOfShape occFaceMap;
    TopExp::MapShapes(shape, TopAbs_FACE, occFaceMap);

    for (int i = 1; i<= occSolidMap.Extent(); i++)
    {
        TopoDS_Solid solid = TopoDS::Solid(occSolidMap(i));

        TopTools_IndexedMapOfShape solidFacesMap;
        TopExp::MapShapes(solid, TopAbs_FACE, solidFacesMap);
        for(int j = 1; j <= solidFacesMap.Extent(); ++j)
        {
            TopoDS_Face face = TopoDS::Face(solidFacesMap(j));
             if (occFaceMap.Contains(face)) {
                // La face est présente dans la map
                std::cout << "La face est présente dans la map." << std::endl;
            } else {
                // La face n'est pas présente dans la map
                std::cout << "La face n'est pas présente dans la map." << std::endl;
            }
        }
    }

    return shape;
}


void ImportBREPImplementationForPairing::perform(std::vector<GeomEntity*>& res)
{
    //Recuperation de la liste des shapes traduites depuis le fichier
    readFile();

    // nombre de solides non fermés
    uint nb_solide_non_ferme = 0;

    unsigned long id_solid =0;
    unsigned long id_shell =0;
    unsigned long id_face  =0;
    unsigned long id_wire  =0;
    unsigned long id_edge  =0;
    unsigned long id_vertex=0;

    bool testVolumicProperties = true;
    bool onlySolidsAndFaces = false;

    std::cout << "dans ImportBREPImplementationForPairing " << std::endl;
    OCCHelper::displayShapeInfo(m_importedShape);

    TopTools_IndexedMapOfShape occSolidMap;
    TopTools_IndexedMapOfShape occFaceMap;
    TopTools_IndexedMapOfShape occWireMap;
    TopTools_IndexedMapOfShape occEdgeMap;
    TopTools_IndexedMapOfShape occVertexMap;

    TopExp::MapShapes(m_importedShape, TopAbs_VERTEX, occVertexMap);
    TopExp::MapShapes(m_importedShape, TopAbs_EDGE, occEdgeMap);
    TopExp::MapShapes(m_importedShape, TopAbs_WIRE, occWireMap);
    TopExp::MapShapes(m_importedShape, TopAbs_FACE, occFaceMap);
    TopExp::MapShapes(m_importedShape, TopAbs_SOLID, occSolidMap);

    std::map<int, Vertex*> verticesMap;
    std::vector<Vertex*> vertices;
    std::vector<Curve*> curves;
    std::vector<Surface*> surfaces;
    std::vector<Volume*> volumes;

    /*
    GeomSplitImplementation gsi(m_context);

    for (int i = 1; i<= occSolidMap.Extent(); i++)
    {
        TopoDS_Solid s = TopoDS::Solid(occSolidMap(i));
        Volume* volume = EntityFactory(m_context).newOCCVolume(s);
        std::vector<Surface*> surfs;
        std::vector<Curve*  > curvs;
        std::vector<Vertex* > verts;
        gsi.split(volume, surfs, curvs, verts);
        store(volume);
        volumes.push_back(volume);
        for(unsigned int i=0;i<surfs.size();i++){
            std::cout << "Surface " << surfs[i]->getName() << std::cout;
        	store(surfs[i]);
        	m_group_helper.addToGroup("", surfs[i]);
        }
        for(unsigned int i=0;i<curvs.size();i++){
        	store(curvs[i]);
        	m_group_helper.addToGroup("", curvs[i]);
        }
        for(unsigned int i=0;i<verts.size();i++){
        	store(verts[i]);
        	m_group_helper.addToGroup("", verts[i]);
        }
    }
    */

    for (int i = 1; i <= occVertexMap.Extent(); ++i)
    {
        const TopoDS_Vertex& v = TopoDS::Vertex(occVertexMap(i));
        Vertex* vertex = 0;
        for (const auto& pair : verticesMap)
        {
            const TopoDS_Vertex& refVertex = TopoDS::Vertex(occVertexMap(pair.first));
            if (v.IsSame(refVertex))
            {
                vertex = pair.second;
                break;
            }
        }
        if (vertex == 0)
        {
            vertex = EntityFactory(m_context).newOCCVertex(const_cast<TopoDS_Vertex&>(v));
            store(vertex);
            vertices.push_back(vertex);
        }
        verticesMap[i] = vertex;
    }

    std::cout << "On a créé " << vertices.size() << " vertices" << std::endl;

    for (int i = 1; i <= occEdgeMap.Extent(); ++i)
    {
        TopoDS_Edge e = TopoDS::Edge(occEdgeMap(i));
        Curve* curve = EntityFactory(m_context).newOCCCurve(e);

        TopTools_IndexedMapOfShape edgeVerticesMap;
        TopExp::MapShapes(e, TopAbs_VERTEX, edgeVerticesMap);
        for(int j = 1; j <= edgeVerticesMap.Extent(); ++j)
        {
            TopoDS_Vertex v = TopoDS::Vertex(edgeVerticesMap(j));
            auto index = occVertexMap.FindIndex(v);
            if (index > 0)
                curve->add(vertices[index-1]);
            else
                std::cout << "Vertex non trouvé dans le map." << std::endl;
        }
        store(curve);
        curves.push_back(curve);
    }

    std::cout << "On a créé " << curves.size() << " curves" << std::endl;

    /*
    for (int i = 1; i <= occWireMap.Extent(); ++i)
    {
        TopoDS_Wire w = TopoDS::Wire(occWireMap(i));

        TopTools_IndexedMapOfShape wireEdgesMap;
        std::vector<TopoDS_Edge> edges;
        TopExp::MapShapes(w, TopAbs_EDGE, wireEdgesMap);
        for(int j = 1; j <= wireEdgesMap.Extent(); ++j)
            edges.push_back(TopoDS::Edge(wireEdgesMap(j)));

        Point p1;
        Point p2;
        Curve* curve = EntityFactory(m_context).newOCCCompositeCurve(edges, p1, p2);

        for(int j = 1; j <= edgeVerticesMap.Extent(); ++j)
        {
            TopoDS_Vertex v = TopoDS::Vertex(edgeVerticesMap(j));
            auto index = occVertexMap.FindIndex(v);
            if (index > 0)
                curve->add(vertices[index-1]);
            else
                std::cout << "Vertex non trouvé dans le map." << std::endl;
        }

        store(curve);
        curves.push_back(curve);
    }
    */
    
    for (int i = 1; i <= occFaceMap.Extent(); ++i)
    {
        TopoDS_Face f = TopoDS::Face(occFaceMap(i));
        Surface* surface = EntityFactory(m_context).newOCCSurface(f);

        TopTools_IndexedMapOfShape faceEdgesMap;
        TopExp::MapShapes(f, TopAbs_EDGE, faceEdgesMap);
        for(int j = 1; j <= faceEdgesMap.Extent(); ++j)
        {
            TopoDS_Edge e = TopoDS::Edge(faceEdgesMap(j));
            auto index = occEdgeMap.FindIndex(e);
            if (index > 0)
                surface->add(curves[index-1]);
            else
                std::cout << "Edge non trouvé dans le map." << std::endl;
        }

        store(surface);
        surfaces.push_back(surface);
    }

    for (int i = 1; i <= occSolidMap.Extent(); ++i)
    {
        TopoDS_Solid s = TopoDS::Solid(occSolidMap(i));
        Volume* volume = EntityFactory(m_context).newOCCVolume(s);

        TopTools_IndexedMapOfShape volumeFacesMap;
        TopExp::MapShapes(s, TopAbs_FACE, volumeFacesMap);
        for(int j = 1; j <= volumeFacesMap.Extent(); ++j)
        {
            TopoDS_Face f = TopoDS::Face(volumeFacesMap(j));
            auto index = occFaceMap.FindIndex(f);
            if (index > 0)
                volume->add(surfaces[index-1]);
            else
                std::cout << "Face non trouvée dans le map." << std::endl;
        }

        store(volume);
        volumes.push_back(volume);
    }

    std::cout << "On a créé " << volumes.size() << " volumes" << std::endl;

    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    if (nb_solide_non_ferme == 0)
        message << "Importation sans aucun volume non fermé";
    else
        message << "Importation avec au moins 1 volume non fermé ("<<(short)nb_solide_non_ferme<<")";
    getContext().getLogDispatcher().log (TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
}

void ImportBREPImplementationForPairing::store(Volume* v)
{
	m_newEntities.push_back(v);
	m_newVolumes.push_back(v);
	m_icmd->addGeomInfoEntity (v, Internal::InfoCommand::CREATED);
	getContext().getGeomManager().addEntity(v);
}
/*----------------------------------------------------------------------------*/
void ImportBREPImplementationForPairing::store(Surface* s)
{
	m_newEntities.push_back(s);
	m_newSurfaces.push_back(s);
	m_icmd->addGeomInfoEntity (s, Internal::InfoCommand::CREATED);
	getContext().getGeomManager().addEntity(s);
}
/*----------------------------------------------------------------------------*/
void ImportBREPImplementationForPairing::store(Curve* c)
{
	m_newEntities.push_back(c);
	m_newCurves.push_back(c);
	m_icmd->addGeomInfoEntity (c, Internal::InfoCommand::CREATED);
	getContext().getGeomManager().addEntity(c);
}
/*----------------------------------------------------------------------------*/
void ImportBREPImplementationForPairing::store(Vertex* v)
{
	m_newEntities.push_back(v);
	m_newVertices.push_back(v);
	m_icmd->addGeomInfoEntity (v, Internal::InfoCommand::CREATED);
	getContext().getGeomManager().addEntity(v);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
