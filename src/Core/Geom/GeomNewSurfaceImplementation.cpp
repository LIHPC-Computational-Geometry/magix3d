/*----------------------------------------------------------------------------*/
/*
 * GeomNewSurfaceImplementation.cpp
 *
 *  Created on: 4 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GeomNewSurfaceImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "Utils/MgxException.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pln.hxx>
/*----------------------------------------------------------------------------*/


#include <ShapeExtend_WireData.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeBuild_Edge.hxx>
#include <Geom_Line.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

GeomNewSurfaceImplementation::
GeomNewSurfaceImplementation(Internal::Context& c, std::vector<Curve*>& es)
:GeomModificationBaseClass(c)
{
    m_curves_param.insert(m_curves_param.end(),es.begin(),es.end());
    m_context = new ShapeBuild_ReShape;
}
/*----------------------------------------------------------------------------*/
GeomNewSurfaceImplementation::~GeomNewSurfaceImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomNewSurfaceImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    std::vector<GeomEntity*> entities;
    entities.resize(m_curves_param.size());
    for(unsigned int i=0;i<m_curves_param.size();i++)
        entities[i] = m_curves_param[i];
    init(entities);
}

/*----------------------------------------------------------------------------*/
TopoDS_Edge GeomNewSurfaceImplementation::
buildEdge(const TopoDS_Edge& aE1, const TopoDS_Edge& aE2)
{
    double max_tolerance = 10e-3;
    //=======================================================================
    // (1) Recuperation des sommets extremites à coller
    //=======================================================================

    ShapeAnalysis_Edge asae;
    TopoDS_Vertex aV11 = asae.FirstVertex(aE1);
    TopoDS_Vertex aV12 = asae.LastVertex(aE1);

    TopoDS_Vertex aV21 = asae.FirstVertex(aE2);
    TopoDS_Vertex aV22 = asae.LastVertex(aE2);

    gp_Pnt p11 = BRep_Tool::Pnt(aV11);
    gp_Pnt p12 = BRep_Tool::Pnt(aV12);
    gp_Pnt p21 = BRep_Tool::Pnt(aV21);
    gp_Pnt p22 = BRep_Tool::Pnt(aV22);

    gp_Pnt p1,p2;
    TopoDS_Vertex aV1, aV2;
    if (p11.Distance(p21)<=max_tolerance)
    {
        p1 = p11;
        p2 = p21;
        aV1 = aV11;
        aV2 = aV21;
    }
    else if (p11.Distance(p22)<=max_tolerance)
    {
        p1 = p11;
        p2 = p22;
        aV1 = aV11;
        aV2 = aV22;

    }
    if (p12.Distance(p21)<=max_tolerance)
    {
        p1 = p12;
        p2 = p21;
        aV1 = aV12;
        aV2 = aV21;

    }
    else  if (p12.Distance(p22)<=max_tolerance)
    {
        p1 = p12;
        p2 = p22;
        aV1 = aV12;
        aV2 = aV22;

    }
    else
       std::cerr<<"Ajnkljclscjlwchjjclkdxhcwlkhcls"<<std::endl;

    std::cerr<<"CREATION D'UNE ARETE entre ("
            <<p1.X()<<", "<<p1.Y()<<"), ("
            <<p2.X()<<", "<<p2.Y()<<")"<<std::endl;

    //=======================================================================
    // (2) Construction de sommets copies
    //=======================================================================
    BRepBuilderAPI_MakeVertex mkver1( p1 );
    TopoDS_Vertex newV1 = mkver1.Vertex();

    BRepBuilderAPI_MakeVertex mkver2( p2 );
    TopoDS_Vertex newV2 = mkver2.Vertex();
    BRep_Builder B;
    B.UpdateVertex ( newV1,Precision::Confusion());
    B.UpdateVertex ( newV2, Precision::Confusion());

    //=======================================================================
    // (3) Construction de l'arete de jonction
    //=======================================================================

    Standard_Boolean isBuild = Standard_False;
    TopoDS_Edge edge;
    B.MakeEdge ( edge );
    ShapeBuild_Edge sbe;

    gp_Vec v1 ( p1, p2 );
    Handle(Geom_Line) aLine = new Geom_Line ( p1, gp_Dir ( v1 ) );
    B.UpdateEdge ( edge, aLine, ::Precision::Confusion() );
    B.Range ( edge,0.0, v1.Magnitude());

    B.Add ( edge, newV1.Oriented ( TopAbs_FORWARD ) );
    B.Add ( edge, newV2.Oriented ( TopAbs_REVERSED ) );

    //=======================================================================
    // (3) Remplacement des anciens sommets par les nouveaux
    //=======================================================================

    m_context->Replace(aV1, newV1.Oriented (aV1.Orientation()));
    m_context->Replace(aV2, newV2.Oriented (aV2.Orientation()));

    return edge;
}
/*----------------------------------------------------------------------------*/
void GeomNewSurfaceImplementation::
updateWire (Handle(ShapeExtend_WireData)& AWireData)
{
}

/*----------------------------------------------------------------------------*/
void GeomNewSurfaceImplementation::perform(std::vector<GeomEntity*>& res)
{
//{
//    double tolerance =0.1;
//    double max_tolerance = 0.1;
//    bool mode_fix_gaps_curves = true;
//
//    //====================================================================
//    //      (1) Récupération de la séquence d'arêtes
//    //====================================================================
//    Handle(TopTools_HSequenceOfShape) init_seq_edges = new TopTools_HSequenceOfShape();
//
//    for(unsigned int i=0; i<m_curves_param.size();i++)
//    {
//        Curve* ei = m_curves_param[i];
////        std::cerr<<"Courbe "<<ei->getName()<<std::endl;
//        TopoDS_Shape si;
//        getOCCShape(ei, si);
//        init_seq_edges->Append(TopoDS::Edge(si));
//    }
//
//    Handle(TopTools_HSequenceOfShape) wires = new TopTools_HSequenceOfShape();
//    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(init_seq_edges,tolerance,false, wires);
//
//    for(int i=1;i<=wires->Length();i++)
//    {
//        //std::cerr<<"Wire "<<i<<std::endl;
//        TopoDS_Wire wi = TopoDS::Wire(wires->Value(i));
////        {
////            TopTools_IndexedMapOfShape map_vertices, map_edges, map_faces, map_wires;
////            TopExp::MapShapes(wi,TopAbs_VERTEX, map_vertices);
////            TopExp::MapShapes(wi,TopAbs_EDGE, map_edges);
////            TopExp::MapShapes(wi,TopAbs_WIRE, map_wires);
////            TopExp::MapShapes(wi,TopAbs_FACE, map_faces);
////            std::cerr<<"Nb sommets : "<<map_vertices.Extent()<<std::endl;
////            BRep_Tool t;
////            for(int k=1;k<=map_vertices.Extent();k++){
////                TopoDS_Vertex v = TopoDS::Vertex(map_vertices.FindKey(k));
////                std::cerr<<"\t tolerance vertex "<<k<<": "<<t.Tolerance(v)<<std::endl;
////            }
////
////            std::cerr<<"Nb aretes  : "<<map_edges.Extent()<<std::endl;
////            for(int k=1;k<=map_edges.Extent();k++){
////                TopoDS_Edge e = TopoDS::Edge(map_edges.FindKey(k));
////                std::cerr<<"\t tolerance edge "<<k<<": "<<t.Tolerance(e)<<std::endl;
////            }
////            std::cerr<<"Nb contours: "<<map_wires.Extent()<<std::endl;
////            std::cerr<<"Nb faces   : "<<map_faces.Extent()<<std::endl;
////        }
//
//        Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire();
//        sfw->Load(wi);
//        sfw->Perform();
//        sfw->FixReorder();
//
//        sfw->SetMaxTolerance(Precision::Confusion());
//        sfw->ClosedWireMode()=Standard_True;
//        sfw->FixConnected(tolerance);//Precision::Confusion());
//        sfw->FixClosed(tolerance);//(Precision::Confusion());
//
//        ShapeFix_ShapeTolerance shTol;
//        BRepBuilderAPI_MakeWire mkWire;
//        for(int i_edge=1; i_edge<=sfw->NbEdges();i_edge++)
//        {
//            TopoDS_Edge ei = sfw->WireData()->Edge(i_edge);
//            shTol.SetTolerance(ei,tolerance,TopAbs_VERTEX);
//            mkWire.Add(ei);
//        }
//
//        wires->ChangeValue(i)=mkWire.Wire();
//    }
//
//
//    Handle(TopTools_HSequenceOfShape) out_wires = new TopTools_HSequenceOfShape();
//    ShapeAnalysis_FreeBounds::ConnectWiresToWires(wires,tolerance,false, out_wires);
//
////    std::cerr<<"NB OUT Wires = "<<out_wires->Length()<<std::endl;
//    for(int i=1;i<=out_wires->Length();i++)
//    {
////        std::cerr<<"OUT Wire "<<i<<std::endl;
//        TopoDS_Wire wi = TopoDS::Wire(out_wires->Value(i));
//
//        Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire();
//        sfw->Load(wi);
//        sfw->Perform();
//        sfw->FixReorder();
//
//        sfw->SetMaxTolerance(Precision::Confusion());
//        sfw->ClosedWireMode()=Standard_True;
//        sfw->FixConnected(tolerance);//Precision::Confusion());
//        sfw->FixClosed(tolerance);//(Precision::Confusion());
//
//        ShapeFix_ShapeTolerance shTol;
//        BRepBuilderAPI_MakeWire mkWire;
//        for(int i_edge=1; i_edge<=sfw->NbEdges();i_edge++)
//        {
//            TopoDS_Edge ei = sfw->WireData()->Edge(i_edge);
//            shTol.SetTolerance(ei,tolerance,TopAbs_VERTEX);
//            mkWire.Add(ei);
//        }
//
//        out_wires->ChangeValue(i)=mkWire.Wire();
//    }
//
//    Handle(TopTools_HSequenceOfShape) out_wires2 = new TopTools_HSequenceOfShape();
//    ShapeAnalysis_FreeBounds::ConnectWiresToWires(out_wires,tolerance,false, out_wires2);
//
//    //====================================================================
//    //      (2) Transformation en un contour "ouvert"
//    //====================================================================
////    std::cerr<<"NB OUT Wires 2 = "<<out_wires2->Length()<<std::endl;
//
//    TopoDS_Shape result = out_wires2->Value(1);
//    {
//        TopTools_IndexedMapOfShape map_vertices, map_edges, map_faces, map_wires;
//        TopExp::MapShapes(result,TopAbs_VERTEX, map_vertices);
//        TopExp::MapShapes(result,TopAbs_EDGE, map_edges);
//        TopExp::MapShapes(result,TopAbs_WIRE, map_wires);
//        TopExp::MapShapes(result,TopAbs_FACE, map_faces);
////        std::cerr<<"Nb sommets : "<<map_vertices.Extent()<<std::endl;
////        BRep_Tool t;
////        for(int k=1;k<=map_vertices.Extent();k++){
////            TopoDS_Vertex v = TopoDS::Vertex(map_vertices.FindKey(k));
////            std::cerr<<"\t tolerance vertex "<<k<<": "<<t.Tolerance(v)<<std::endl;
////        }
////
////        std::cerr<<"Nb aretes  : "<<map_edges.Extent()<<std::endl;
////        for(int k=1;k<=map_edges.Extent();k++){
////            TopoDS_Edge e = TopoDS::Edge(map_edges.FindKey(k));
////            std::cerr<<"\t tolerance edge "<<k<<": "<<t.Tolerance(e)<<std::endl;
////        }
////        std::cerr<<"Nb contours: "<<map_wires.Extent()<<std::endl;
////        std::cerr<<"Nb faces   : "<<map_faces.Extent()<<std::endl;
//    }
////    //    GEOMAlgo_Splitter splitter;
////    //    for(unsigned int i=1; i<=seq.Length();i++)
////    //    {
////    //
////    //
//////        TopoDS_Shape si = seq.Value(i);
//////        splitter.AddShape (si);
//////    }
//////    splitter.Perform();
//////    result = splitter.Shape();
////
//   //MAINTENANT, ON FAIT CE QUI ETAIT FAIT AVANT
//  // result =  newSurface(result);
//
//   //EntityFactory(m_context).newOCCSurface(TopoDS::Face(result));
//   createGeomEntities(result,true);
//}
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape GeomNewSurfaceImplementation::newSurface(TopoDS_Shape& AShape)
{
    //================================================================
    //      1 - Création d'un contour
    //================================================================
    BRepBuilderAPI_MakeWire mk_wire;

    TopExp_Explorer ex;
    ex.Init(AShape, TopAbs_EDGE);
    for (; ex.More(); ex.Next())
    {
        TopoDS_Edge current_edge = TopoDS::Edge(ex.Current());
        mk_wire.Add(current_edge);
    }

    mk_wire.Build();
    TopoDS_Wire w = mk_wire.Wire();

    //================================================================
    //      2 - Création du plan support du contour
    //================================================================
    /* A partir des points des courbes, on cree un plan support de la face.
     * Si on a plus de 3 points, on vérifie la coplanarité. Si elle n'est
     * pas là, on lance une exception.
     */
    TopTools_IndexedMapOfShape map_vertices;
    TopExp::MapShapes(AShape,TopAbs_VERTEX, map_vertices);
    std::vector<Utils::Math::Point> vec_vertices;

    for(unsigned int i=1; i<=map_vertices.Extent();i++){
        TopoDS_Shape s_i = map_vertices.FindKey(i);
        TopoDS_Vertex v_i = TopoDS::Vertex(s_i);
        gp_Pnt p_i = BRep_Tool::Pnt(v_i);

        Utils::Math::Point pi(p_i.X(),p_i.Y(),p_i.Z());
        vec_vertices.push_back(pi);
    }

    // par defaut, on rajoute dans les points à considérer, le milieu de toutes les courbes
    // qui ne sont pas des segments. Cela évite entre autres des problèmes avec les
    // demi-cercles.
    for(unsigned int i=0; i<m_curves_param.size();i++)
    {

        Curve* ei = m_curves_param[i];
        Utils::Math::Point p_middle;
        ei->getPoint(0.5,p_middle,true);
        vec_vertices.push_back(p_middle);
    }

    gp_Pln surface_plane;
    if(vec_vertices.size()<3){
        // Au moins l'une des deux courbes n'est pas un segment
        throw Utils::BuildingException(TkUtil::UTF8String ("Creation d'une surface plane réduite à un segment", TkUtil::Charset::UTF_8));
    }
    else
    {
        Utils::Math::Point v0 = vec_vertices[0];
        Utils::Math::Point v1 = vec_vertices[1];
        gp_Pnt p0(v0.getX(),v0.getY(),v0.getZ());
        gp_Pnt p1(v1.getX(),v1.getY(),v1.getZ());

        gp_Vec v01(p0,p1);
        int pnt_index=2;
        gp_Vec plane_normal;
        bool find_third_point = false;
        while(!find_third_point && pnt_index<vec_vertices.size()){
            Utils::Math::Point v2 = vec_vertices[pnt_index];
            gp_Pnt p2(v2.getX(),v2.getY(),v2.getZ());
            gp_Vec v02(p0,p2);

            plane_normal= v01.Crossed(v02);
            if(plane_normal.Magnitude()==0) //vecteur nul problème
                    pnt_index++;
            else
                find_third_point=true;
        }
        if(!find_third_point)
            throw Utils::BuildingException(TkUtil::UTF8String ("Creation d'une surface plane: Les sommets sont colinéaires", TkUtil::Charset::UTF_8));

        surface_plane = gp_Pln(p0,gp_Dir(plane_normal));

        if(vec_vertices.size()>3){
            /* on regarde si les autres sommets appartiennent au plan que l'on vient
             * de definir. Sinon, on renvoit une exception.
             */
            for(unsigned int j=3;j<vec_vertices.size();j++){
                Utils::Math::Point v = vec_vertices[j];
                gp_Pnt p(v.getX(),v.getY(),v.getZ());
                if(surface_plane.SquareDistance(p)>Utils::Math::MgxNumeric::mgxDoubleEpsilon)
                    throw Utils::BuildingException(TkUtil::UTF8String ("Creation d'une surface plane: Les sommets ne sont pas coplanaires", TkUtil::Charset::UTF_8));
            }
        }
    }
    //================================================================
    //      3 - Création de la face à partir du contour et du plan
    //================================================================
    TopoDS_Shape aFace;
    BRepBuilderAPI_MakeFace MF(surface_plane,w,true);


    if(MF.Error()==BRepBuilderAPI_FaceDone){
        aFace   = MF.Shape();
    }
    else
        throw Utils::BuildingException(TkUtil::UTF8String ("Error during surface creation (no shape)", TkUtil::Charset::UTF_8));

    return aFace;
}

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

