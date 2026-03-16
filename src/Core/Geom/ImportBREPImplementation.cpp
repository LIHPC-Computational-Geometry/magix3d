/*----------------------------------------------------------------------------*/
#include "Geom/ImportBREPImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopExp_Explorer.hxx>
#include <ShapeFix_Shape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportBREPImplementation::
ImportBREPImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
		const std::string& n, const bool createGroups)
: GeomImport(c,icmd,n, false, createGroups)
{
}
/*----------------------------------------------------------------------------*/
ImportBREPImplementation::~ImportBREPImplementation()
{}
/*----------------------------------------------------------------------------*/
void ImportBREPImplementation::readFile()
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
    TopoDS_Shape shape;
    BRepTools::Read(shape, file, builder);

    std::cout << "-> Après import" << std::endl;
    displayShapeInfo(shape);

    double tolerance = 1e-6;
    shape = mergeVerticesSafe(shape, tolerance);
    std::cout << "-> Après mergeVerticesSafe" << std::endl;
    displayShapeInfo(shape);

    shape = removeDegeneratedEdges(shape);
    std::cout << "-> Après removeDegeneratedEdges" << std::endl;
    displayShapeInfo(shape);

    //shape = fixShape(shape);
    //std::cout << "-> apres fix " << std::endl;
    //displayShapeInfo(shape);

    m_importedShapes.resize(1);
    m_importedShapes[0] = shape;
}

// Fonction pour afficher les informations de base sur une forme TopoDS_Shape
void ImportBREPImplementation::displayShapeInfo(const TopoDS_Shape& shape)
{
        TopTools_IndexedMapOfShape edgeMap;
        TopTools_IndexedMapOfShape faceMap;
        TopTools_IndexedMapOfShape vertexMap;
        TopExp::MapShapes(shape, TopAbs_EDGE, edgeMap);
        TopExp::MapShapes(shape, TopAbs_FACE, faceMap);
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertexMap);
        std::cout << "Vertices : " << vertexMap.Extent() << std::endl;
        std::cout << "Edges    : " << edgeMap.Extent() << std::endl;
        std::cout << "Faces    : " << faceMap.Extent() << std::endl;
}

TopoDS_Shape ImportBREPImplementation::mergeVerticesSafe(const TopoDS_Shape& shape, double tol)
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

TopoDS_Shape ImportBREPImplementation::removeDegeneratedEdges(const TopoDS_Shape& shape)
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

TopoDS_Shape ImportBREPImplementation::fixShape(const TopoDS_Shape& shape)
{
    Handle(ShapeFix_Shape) fix = new ShapeFix_Shape(shape);
    try { fix->Perform(); } 
    catch (Standard_Failure& e) { std::cout << e.GetMessageString() << std::endl; }

    return fix->Shape();
}

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
