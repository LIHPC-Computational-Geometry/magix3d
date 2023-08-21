/*----------------------------------------------------------------------------*/
/** \file OCCFacetedRepresentationBuilder.cpp
 *
 *  \author legoff
 *
 *  \date 08/07/2013
 */
/*----------------------------------------------------------------------------*/
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
//// inclusion de fichiers en-tête OCC
#include<Poly_Polygon3D.hxx>
#include<Poly_PolygonOnTriangulation.hxx>
#include<BRepBndLib.hxx>
#include<BRepMesh.hxx>
#include<BRepTools.hxx>
#include<BRep_Tool.hxx>
#include<TopExp.hxx>
#include<TopoDS.hxx>
#include<Geom_Curve.hxx>
#include<Geom_BSplineCurve.hxx>
#include<Geom_BezierCurve.hxx>
#include<Geom_OffsetCurve.hxx>
#include<Geom_Ellipse.hxx>
#include<Geom_Parabola.hxx>
#include<Geom_Hyperbola.hxx>
#include<Geom_TrimmedCurve.hxx>
#include<Geom_Circle.hxx>
#include<Geom_Line.hxx>
#include<Geom_Conic.hxx>
#include<TopoDS_Shape.hxx>
#include<TopoDS_Wire.hxx>
#include<TopoDS_Edge.hxx>
#include<TopoDS_Face.hxx>
#include<TopoDS_Iterator.hxx>
#include<TopExp_Explorer.hxx>
#include<Poly_Array1OfTriangle.hxx>
#include<Poly_Triangulation.hxx>
#include<TColgp_Array1OfPnt.hxx>
#include<TColStd_Array1OfInteger.hxx>
#include<GeomLProp_SLProps.hxx>
#include<gp_Trsf.hxx>
#include<TopTools_ListOfShape.hxx>
#include<TopTools_IndexedMapOfShape.hxx>
#include<TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include<ShapeAnalysis_ShapeContents.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
OCCFacetedRepresentationBuilder::
OCCFacetedRepresentationBuilder(const GeomEntity* caller,
                                TopoDS_Shape& shape)
:m_entity(caller),m_shape(shape)
{}
/*----------------------------------------------------------------------------*/
OCCFacetedRepresentationBuilder::~OCCFacetedRepresentationBuilder()
{}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::execute(
        std::vector<gmds::math::Triangle >& AVec)
{
    /* si la shape est vide, on ne fait rien */
    if(m_shape.IsNull())
        throw TkUtil::Exception (TkUtil::UTF8String ("OCCFacetedRepresentationBuilder::execute "
                "shape est NULL.",
				TkUtil::Charset::UTF_8));

    OCCGeomRepresentation::buildIncrementalBRepMesh(m_shape, 0.01);

    if (m_shape.ShapeType()==TopAbs_COMPSOLID ||
            m_shape.ShapeType()==TopAbs_SOLID  ||
            m_shape.ShapeType()==TopAbs_SHELL ||
            m_shape.ShapeType()==TopAbs_FACE)
        buildSurfaceRepresentation(AVec);
    else if(m_shape.ShapeType()==TopAbs_EDGE ||
            m_shape.ShapeType()==TopAbs_WIRE)
        buildCurveRepresentation(AVec);
    else
        buildVertexRepresentation(AVec);
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::execute(
        std::vector<gmds::math::Triangle >& AVec,
        TopoDS_Shape& AShape)
{
    /* si la shape est vide, on ne fait rien */
    if(m_shape.IsNull())
        throw TkUtil::Exception (TkUtil::UTF8String ("OCCFacetedRepresentationBuilder::execute "
                "shape est NULL.",
				TkUtil::Charset::UTF_8));

    OCCGeomRepresentation::buildIncrementalBRepMesh(m_shape, 0.01);

    if (m_shape.ShapeType()==TopAbs_COMPSOLID ||
            m_shape.ShapeType()==TopAbs_SOLID  ||
            m_shape.ShapeType()==TopAbs_SHELL)
        buildSurfaceRepresentation(AVec,AShape);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("OCCFacetedRepresentationBuilder::execute "
                "ne peut être appelé que pour un volume.",
				TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildVertexRepresentation(
		std::vector<gmds::math::Triangle >& AVec)
{
    computeVertices(AVec);
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildCurveRepresentation(
		std::vector<gmds::math::Triangle >& AVec)
{
    buildCurves(AVec);
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildSurfaceRepresentation(
        std::vector<gmds::math::Triangle >& AVec)
{
    buildSurfaces(AVec);
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildSurfaceRepresentation(
        std::vector<gmds::math::Triangle >& AVec,
        TopoDS_Shape& AShape)
{
    buildSurfaces(AVec,AShape);
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildCurves(
		std::vector<gmds::math::Triangle >& AVec)
{
    try {
        computeEdges(AVec);
    }
    catch (...){
        throw TkUtil::Exception(TkUtil::UTF8String (
				"OCCFacetedRepresentationBuilder::buildCurves "
                "Impossible de créer une représentation de courbes!",
				TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildSurfaces(
        std::vector<gmds::math::Triangle >& AVec)
{
    try {
        computeFaces(AVec);
    }
    catch (...){
        throw TkUtil::Exception(TkUtil::UTF8String (
				"OCCFacetedRepresentationBuilder::buildSurfaces "
                "Impossible de créer une représentation facétisée de surfaces!",
				TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::buildSurfaces(
        std::vector<gmds::math::Triangle >& AVec,
        TopoDS_Shape& AShape)
{
    try {
        computeFaces(AVec,AShape);
    }
    catch (...){
        throw TkUtil::Exception(TkUtil::UTF8String (
				"OCCFacetedRepresentationBuilder::buildSurfaces "
                "Impossible de créer une représentation facétisée de surfaces!",
				TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::computeVertices(
		std::vector<gmds::math::Triangle >& AVec)
{
	AVec.clear();

    TopExp_Explorer ex;
    for (ex.Init(m_shape, TopAbs_VERTEX); ex.More(); ex.Next()) {
        // get the shape
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(ex.Current());
        gp_Pnt occ_pnt = BRep_Tool::Pnt(aVertex);

        gmds::math::Point pnt(occ_pnt.X(),occ_pnt.Y(),occ_pnt.Z());
        gmds::math::Triangle tri(pnt,pnt,pnt);
        AVec.push_back(tri);
    }
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::computeEdges(
		std::vector<gmds::math::Triangle >& AVec)
{
	AVec.clear();

	const TopoDS_Edge& aEdge = TopoDS::Edge(m_shape);

	// On récupère la transformation de la shape/face
	gp_Trsf myTransf;
	Standard_Boolean identity = true;
	TopLoc_Location aLoc;

	// On triangule l'arête
	Handle(Poly_PolygonOnTriangulation) aEdgePoly;
	Standard_Integer index = 1;
	Handle(Poly_Triangulation) T;
	BRep_Tool::PolygonOnTriangulation(aEdge, aEdgePoly, T, aLoc, index);
	//        Handle(Poly_Polygon3D) aPoly;
	//        if(aEdgePoly.IsNull())
	//            aPoly = BRep_Tool::Polygon3D(aEdge, aLoc);

	// Si la triangulation a réussi?
	if(!aEdgePoly.IsNull()){
#ifdef _DEBUG_EDGES
		std::cout<<" cas !aEdgePoly.IsNull()"<<std::endl;
#endif
		if (!aLoc.IsIdentity()) {
#ifdef _DEBUG_EDGES
			std::cout<<"!aLoc.IsIdentity() pour une shape"<<std::endl;
#endif
			identity = false;
			myTransf = aLoc.Transformation();
		}

		// on récupère la triangulation de l'arête et on remplit la discretisation
		int nbNodesInEdge= aEdgePoly->NbNodes();

		std::vector<gmds::math::Point> gmdsPoints(nbNodesInEdge);

		const TColStd_Array1OfInteger& NodeIDs = aEdgePoly->Nodes();
		const TColgp_Array1OfPnt& Nodes = T->Nodes();
		gp_Pnt V;
		for (Standard_Integer i=0;i < nbNodesInEdge;i++) {
			V = Nodes(NodeIDs(i+1));
			if(!identity)
				V.Transform(myTransf);

			gmdsPoints[i] = gmds::math::Point(V.X(),V.Y(),V.Z());
		}

		for (Standard_Integer i=1;i < nbNodesInEdge;i++) {
			gmds::math::Triangle tri(gmdsPoints[i-1],gmdsPoints[i],gmdsPoints[i-1]);
			AVec.push_back(tri);
		}
	}

	else {
		TopTools_IndexedMapOfShape M;
		TopExp::MapShapes(aEdge, TopAbs_VERTEX, M);

		for (int ie=0; ie<M.Extent(); ie++)
		{
			const TopoDS_Vertex& v= TopoDS::Vertex(M(ie+1));
			gp_Pnt pnt = BRep_Tool::Pnt(v);

			//                std::cout<<ie<<" -> ("<<pnt.X()<<", "<<pnt.Y()<<", "<<pnt.Z()<<")"<<std::endl;
		}
		Standard_Real first, last;
		Handle_Geom_Curve curv = BRep_Tool::Curve(aEdge,first, last);
		//            std::cout<<"first, last = "<<first<<", "<<last<<std::endl;
		//            std::cout<<"curve "<<curv<<std::endl;
		int nb_steps= 100;

		std::vector<gmds::math::Point> gmdsPoints(nb_steps+1);

		double step= (last-first)/nb_steps;
		//            std::cout<<"\t step: "<<step<<std::endl;
		for(int i =0;i<=nb_steps; i++){
			gp_Pnt V = curv->Value(first+i*step);
			gmdsPoints[i] = gmds::math::Point(V.X(),V.Y(),V.Z());
		}

		for (int i =0;i<nb_steps; i++){
			gmds::math::Triangle tri(gmdsPoints[i],gmdsPoints[i+1],gmdsPoints[i]);
			AVec.push_back(tri);
		}

	}
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::computeFaces(
        std::vector<gmds::math::Triangle >& AVec)
{
    AVec.clear();

    TopExp_Explorer ex;
    int i = 1;
    if(m_shape.ShapeType()==TopAbs_FACE){
        fillRepresentation(TopoDS::Face(m_shape),AVec);
    }
    else{
        for (ex.Init(m_shape, TopAbs_FACE); ex.More(); ex.Next(),i++) {
            // on récupère la face et on la maille
            fillRepresentation(TopoDS::Face(ex.Current()),AVec);

        } // for (ex.Init(m_shape, TopAbs_FACE); ex.More(); ex.Next(),i++)
    }
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::computeFaces(
        std::vector<gmds::math::Triangle >& AVec,
        TopoDS_Shape& AShape)
{
    AVec.clear();

    TopExp_Explorer ex;

    for (ex.Init(m_shape, TopAbs_FACE); ex.More(); ex.Next()) {
        // on récupère la face et on la maille
    	if (OCCGeomRepresentation::areEquals(TopoDS::Face(AShape),TopoDS::Face(ex.Current()))) {
    		fillRepresentation(TopoDS::Face(ex.Current()),AVec);
    	}
    } // for (ex.Init(m_shape, TopAbs_FACE); ex.More(); ex.Next(),i++)
}
/*----------------------------------------------------------------------------*/
void OCCFacetedRepresentationBuilder::fillRepresentation(
        const TopoDS_Face& aFace,
        std::vector<gmds::math::Triangle >& AVec)
{
//    std::cout<<"OCCFacetedRepresentationBuilder::fillRepresentation"<<std::endl;
//    std::vector<Utils::Math::Point>* rep_points = m_rep->getPoints();
//    std::vector<size_t>* rep_triangles= m_rep->getSurfaceDiscretization();
//
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if (aPoly.IsNull()) {
        throw TkUtil::Exception(TkUtil::UTF8String (
				"OCCFacetedRepresentationBuilder::fillRepresentation"
                "Triangulation vide pour une face!!!",
				TkUtil::Charset::UTF_8));
    }



    // On récupère transformation de la shape/face
    gp_Trsf myTransf;
    Standard_Boolean identity = true;
    if (!aLoc.IsIdentity()) {
        identity = false;
        myTransf = aLoc.Transformation();
    }

    int i;
    int nbNodesInFace = aPoly->NbNodes();
    int nbTriInFace = aPoly->NbTriangles();


    // check orientation
    TopAbs_Orientation orient = aFace.Orientation();

    // on parcourt la triangulation pour remplir notre représentation
    const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
    const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
    for (i=1;i<=nbTriInFace;i++) {
        // Get the triangle
        Standard_Integer N1,N2,N3;
        Triangles(i).Get(N1,N2,N3);

        // on oriente si nécessaire les triangles
        if ( orient != TopAbs_FORWARD ) {
            Standard_Integer tmp = N1;
            N1 = N2;
            N2 = tmp;
        }

        gp_Pnt V1 = Nodes(N1);
        gp_Pnt V2 = Nodes(N2);
        gp_Pnt V3 = Nodes(N3);

        // on positionne correctement les sommets
        if (!identity) {
            V1.Transform(myTransf);
            V2.Transform(myTransf);
            V3.Transform(myTransf);
        }


        // on met à jour les sommets
        gmds::math::Point P1((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
        gmds::math::Point P2((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
        gmds::math::Point P3((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));
        gmds::math::Triangle triangle(P1,P2,P3);
        AVec.push_back(triangle);
    } // for (i=1;i<=nbTriInFace;i++)

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

