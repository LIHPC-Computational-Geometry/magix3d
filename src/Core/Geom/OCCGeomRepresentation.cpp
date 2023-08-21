/*----------------------------------------------------------------------------*/
/** \file OCCGeomRepresentation.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 19/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/OCCDisplayRepresentationBuilder.h"
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Geom/EntityFactory.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Utils/MgxNumeric.h"
#include "Utils/MgxException.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/Log.h>
#include <TkUtil/UTF8String.h>
#include <cmath>
#include <TkUtil/Exception.h>
#include <algorithm>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
//inclusion de fichiers en-tête d'Open Cascade
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Curve.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Conic.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <GeomPlate_Surface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomProjLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <ShapeFix_FixSmallFace.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include<ShapeAnalysis_ShapeContents.hxx>
#include<BRepBuilderAPI_MakeVertex.hxx>

#include <BRepClass_FaceClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <GeomAPI_IntCS.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>

/*----------------------------------------------------------------------------*/
// utilisation du TopoDS_Shape::IsSame
//#define USE_ISSAME
// cela pose problème (ne fonctionne pas correctement) pour au moins le cas GTestMesh.testGMSH_1

//#define _DEBUG2

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
TDF_Label OCCGeomRepresentation::m_rootLabel=TDF_Label();
/*----------------------------------------------------------------------------*/
OCCGeomRepresentation::OCCGeomRepresentation(Internal::Context& c,
			const TopoDS_Shape& shape)
:m_context(c), m_shape(shape), m_label()
{
    // on ne fait pas appel à BRepBuilderAPI_Copy ici car dans ce cas les shapes
    // "identiques" ne sont pas reconnues comme telles (différentes avec IsSame)

	if (useOCAF()){
		m_label = m_rootLabel.FindChild(m_context.nextUniqueId());
		TNaming_Builder tnb(m_label);
		tnb.Generated(m_shape);
	}
}
/*----------------------------------------------------------------------------*/
OCCGeomRepresentation::OCCGeomRepresentation(const OCCGeomRepresentation& rep)
: m_context(rep.m_context), m_shape(rep.m_shape)
{
    // on ne fait pas appel à BRepBuilderAPI_Copy ici car dans ce cas les shapes
    // "identiques" ne sont pas reconnues comme telles (différentes avec IsSame)

	if (useOCAF()){
		m_label = m_rootLabel.FindChild(m_context.nextUniqueId());
		TNaming_Builder tnb(m_label);
		tnb.Generated(m_shape);
	}
}
/*----------------------------------------------------------------------------*/
OCCGeomRepresentation::~OCCGeomRepresentation()
{}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::useOCAF() const
{
	return (m_context.getGeomKernel() == Internal::ContextIfc::WITHOCAF);
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape OCCGeomRepresentation::getShape()  {
    return m_shape;
}
/*----------------------------------------------------------------------------*/
TDF_Label OCCGeomRepresentation::getLabel() const
{
	return m_label;
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::addShapeToLists(TopoDS_Shape& shape,
        TopTools_IndexedMapOfShape& fmap,
        TopTools_IndexedMapOfShape& emap,
        TopTools_IndexedMapOfShape& vmap,
        TopTools_IndexedMapOfShape& somap,
        TopTools_IndexedMapOfShape& shmap,
        TopTools_IndexedMapOfShape& wmap)
{
  TopExp_Explorer exp0, exp1, exp2, exp3, exp4, exp5;
  for(exp0.Init(shape, TopAbs_SOLID); exp0.More(); exp0.Next()){
    TopoDS_Solid solid = TopoDS::Solid(exp0.Current());
    if(somap.FindIndex(TopoDS::Solid(exp0.Current())) < 1){
      somap.Add(TopoDS::Solid(exp0.Current()));

      for(exp1.Init(exp0.Current(), TopAbs_SHELL); exp1.More(); exp1.Next()){
        TopoDS_Shell shell = TopoDS::Shell(exp1.Current().Composed(exp0.Current().Orientation()));
        if(shmap.FindIndex(shell) < 1){
          shmap.Add(shell);

          for(exp2.Init(shell, TopAbs_FACE); exp2.More(); exp2.Next()){
            TopoDS_Face face = TopoDS::Face(exp2.Current().Composed(shell.Orientation()));
            if(fmap.FindIndex(face) < 1){
              fmap.Add(face);

              for(exp3.Init(exp2.Current(), TopAbs_WIRE); exp3.More(); exp3.Next()){
                TopoDS_Wire wire = TopoDS::Wire(exp3.Current().Composed(face.Orientation()));
                if(wmap.FindIndex(wire) < 1){
                  wmap.Add(wire);

                  for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
                    TopoDS_Edge edge = TopoDS::Edge(exp4.Current().Composed(wire.Orientation()));
                    if(emap.FindIndex(edge) < 1){
                      emap.Add(edge);

                      for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
                        TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
                        if(vmap.FindIndex(vertex) < 1)
                          vmap.Add(vertex);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Free Shells
  for(exp1.Init(exp0.Current(), TopAbs_SHELL, TopAbs_SOLID); exp1.More(); exp1.Next()){
    TopoDS_Shape shell = exp1.Current().Composed(exp0.Current().Orientation());
    if(shmap.FindIndex(shell) < 1){
      shmap.Add(shell);

      for(exp2.Init(shell, TopAbs_FACE); exp2.More(); exp2.Next()){
        TopoDS_Face face = TopoDS::Face(exp2.Current().Composed(shell.Orientation()));
        if(fmap.FindIndex(face) < 1){
          fmap.Add(face);

          for(exp3.Init(exp2.Current(), TopAbs_WIRE); exp3.More(); exp3.Next()){
            TopoDS_Wire wire = TopoDS::Wire(exp3.Current());
            if(wmap.FindIndex(wire) < 1){
              wmap.Add(wire);

              for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
                TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
                if(emap.FindIndex(edge) < 1){
                  emap.Add(edge);

                  for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
                    TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
                    if(vmap.FindIndex(vertex) < 1)
                      vmap.Add(vertex);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Free Faces
  for(exp2.Init(shape, TopAbs_FACE, TopAbs_SHELL); exp2.More(); exp2.Next()){
    TopoDS_Face face = TopoDS::Face(exp2.Current());
    if(fmap.FindIndex(face) < 1){
      fmap.Add(face);

      for(exp3.Init(exp2.Current(), TopAbs_WIRE); exp3.More(); exp3.Next()){
        TopoDS_Wire wire = TopoDS::Wire(exp3.Current());
        if(wmap.FindIndex(wire) < 1){
          wmap.Add(wire);

          for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
            TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
            if(emap.FindIndex(edge) < 1){
              emap.Add(edge);

              for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
                TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
                if(vmap.FindIndex(vertex) < 1)
                  vmap.Add(vertex);
              }
            }
          }
        }
      }
    }
  }

  // Free Wires
  for(exp3.Init(shape, TopAbs_WIRE, TopAbs_FACE); exp3.More(); exp3.Next()){
    TopoDS_Wire wire = TopoDS::Wire(exp3.Current());
    if(wmap.FindIndex(wire) < 1){
      wmap.Add(wire);

      for(exp4.Init(exp3.Current(), TopAbs_EDGE); exp4.More(); exp4.Next()){
        TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
        if(emap.FindIndex(edge) < 1){
          emap.Add(edge);

          for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
            TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
            if(vmap.FindIndex(vertex) < 1)
              vmap.Add(vertex);
          }
        }
      }
    }
  }

  // Free Edges
  for(exp4.Init(shape, TopAbs_EDGE, TopAbs_WIRE); exp4.More(); exp4.Next()){
    TopoDS_Edge edge = TopoDS::Edge(exp4.Current());
    if(emap.FindIndex(edge) < 1){
      emap.Add(edge);

      for(exp5.Init(exp4.Current(), TopAbs_VERTEX); exp5.More(); exp5.Next()){
        TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
        if(vmap.FindIndex(vertex) < 1)
          vmap.Add(vertex);
      }
    }
  }

  // Free Vertices
  for(exp5.Init(shape, TopAbs_VERTEX, TopAbs_EDGE); exp5.More(); exp5.Next()){
    TopoDS_Vertex vertex = TopoDS::Vertex(exp5.Current());
    if(vmap.FindIndex(vertex) < 1)
      vmap.Add(vertex);
  }

}
/*----------------------------------------------------------------------------*/
TopoDS_Shape OCCGeomRepresentation::cleanShape(TopoDS_Shape& shape){
    double tolerance= Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
    bool fixsmalledges= true;
    bool fixspotstripfaces = true;
    bool sewfaces=false;
    bool makesolids=false;
    bool connect=false;

    int nrc = 0, nrcs = 0;

    TopExp_Explorer e;
    for(e.Init(shape, TopAbs_COMPOUND); e.More(); e.Next()) nrc++;
    for(e.Init(shape, TopAbs_COMPSOLID); e.More(); e.Next()) nrcs++;

    double surfacecont = 0;

    TopTools_IndexedMapOfShape fmap, emap, vmap, somap, shmap, wmap;

    addShapeToLists(shape, fmap,emap,vmap,somap,shmap,wmap);

    for(int i = 1; i <= fmap.Extent(); i++){
        GProp_GProps system;
        BRepGProp::LinearProperties(fmap(i), system);
        surfacecont += system.Mass();
    }

#ifdef _DEBUG
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "Correction de la géométrie (tolerance="<<tolerance<<")";
    std::cout<<message<<std::endl;
#endif

    if(fixsmalledges){
#ifdef _DEBUG
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "- suppression des arêtes trop petites";
        std::cout<<message<<std::endl;
#endif

        Handle(ShapeFix_Wire) sfw;
        Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
        rebuild->Apply(shape);

        for(int i = 1; i <= fmap.Extent(); i++){
            TopExp_Explorer exp1;
            for(exp1.Init(fmap(i), TopAbs_WIRE); exp1.More(); exp1.Next()){
                TopoDS_Wire oldwire = TopoDS::Wire(exp1.Current());
                sfw = new ShapeFix_Wire(oldwire, TopoDS::Face(fmap(i)), tolerance);
                sfw->ModifyTopologyMode() = Standard_True;

                if(sfw->FixSmall(false, tolerance)){

                    std::cout<<"suppression dans un \"wire\" "<< wmap.FindIndex(oldwire)<<std::endl;

                    TopoDS_Wire newwire = sfw->Wire();
                    rebuild->Replace(oldwire, newwire, Standard_False);
                }
#ifdef _DEBUG
                if((sfw->StatusSmall(ShapeExtend_FAIL1)) ||
                        (sfw->StatusSmall(ShapeExtend_FAIL2)) ||
                        (sfw->StatusSmall(ShapeExtend_FAIL3)))
                    std::cout<<"Echec de la correction d'une petite arête dans un wire"<<wmap.FindIndex(oldwire)<<std::endl;
#endif
            }
        }
        shape = rebuild->Apply(shape);

        {
            Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
            rebuild->Apply(shape);
            TopExp_Explorer exp1;
            for(exp1.Init(shape, TopAbs_EDGE); exp1.More(); exp1.Next()){
                TopoDS_Edge edge = TopoDS::Edge(exp1.Current());
                if(vmap.FindIndex(TopExp::FirstVertex(edge)) ==
                        vmap.FindIndex(TopExp::LastVertex(edge))){
                    GProp_GProps system;
                    BRepGProp::LinearProperties(edge, system);
                    if(system.Mass() < tolerance){
#ifdef _DEBUG
                        std::cout <<"removing degenerated edge "<<emap.FindIndex(edge)<<std::endl;
#endif
                        rebuild->Remove(edge, false);
                    }
                }
            }
            shape = rebuild->Apply(shape);
        }

        Handle(ShapeFix_Wireframe) sfwf = new ShapeFix_Wireframe;
        sfwf->SetPrecision(tolerance);
        sfwf->Load(shape);

#ifdef _DEBUG
        if(sfwf->FixSmallEdges()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"- fixing wire frames";
            std::cout<<message<<std::endl;
            if(sfwf->StatusSmallEdges(ShapeExtend_OK)){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"no small edges found";
                std::cout<<message<<std::endl;
            }
            if(sfwf->StatusSmallEdges(ShapeExtend_DONE1)){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"some small edges fixed";
                std::cout<<message<<std::endl;
            }
            if(sfwf->StatusSmallEdges(ShapeExtend_FAIL1)){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"failed to fix some small edges";
                std::cout<<message<<std::endl;
            }
        }

        if(sfwf->FixWireGaps()){
            std::cout<<"- fixing wire gaps"<<std::endl;
            if(sfwf->StatusWireGaps(ShapeExtend_OK))
                std::cout<<"no gaps found"<<std::endl;
            if(sfwf->StatusWireGaps(ShapeExtend_DONE1))
                std::cout<<"some 2D gaps fixed"<<std::endl;
            if(sfwf->StatusWireGaps(ShapeExtend_DONE2))
                std::cout<<"some 3D gaps fixed"<<std::endl;
            if(sfwf->StatusWireGaps(ShapeExtend_FAIL1))
                std::cout<<"failed to fix some 2D gaps"<<std::endl;
            if(sfwf->StatusWireGaps(ShapeExtend_FAIL2))
                std::cout<<"failed to fix some 3D gaps"<<std::endl;
        }
#endif

        shape = sfwf->Shape();
    }
//
    if(fixspotstripfaces){
#ifdef _DEBUG
       std::cout<<"- fixing spot faces"<<std::endl;
#endif
        Handle(ShapeFix_FixSmallFace) sffsm = new ShapeFix_FixSmallFace;
        sffsm->Init(shape);
        sffsm->SetPrecision(tolerance);
       sffsm->FixSpotFace();
        shape = sffsm->FixShape();
    }
/*
    if(sewfaces){
        std::cout<<"- sewing faces"<<std::endl;


        TopExp_Explorer exp0;

        BRepOffsetAPI_Sewing sewedObj(tolerance);

        for(exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()){
            TopoDS_Face face = TopoDS::Face(exp0.Current());
            sewedObj.Add(face);
        }

        sewedObj.Perform();

        if(!sewedObj.SewedShape().IsNull())
            shape = sewedObj.SewedShape();
        else
            std::cout<<" not possible"<<std::endl;
    }

    if(makesolids){
        std::cout<<"- making solids"<<std::endl;

        TopExp_Explorer exp0;

        BRepBuilderAPI_MakeSolid ms;
        int count = 0;
        for(exp0.Init(shape, TopAbs_SHELL); exp0.More(); exp0.Next()){
            count++;
            ms.Add(TopoDS::Shell(exp0.Current()));
        }

        if(!count){
            std::cout<<" not possible (no shells)"<<std::endl;
        }
        else{
            BRepCheck_Analyzer ba(ms);
            if(ba.IsValid()){
                Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
                sfs->Init(ms);
                sfs->SetPrecision(tolerance);
                sfs->SetMaxTolerance(tolerance);
                sfs->Perform();
                shape = sfs->Shape();

                for(exp0.Init(shape, TopAbs_SOLID); exp0.More(); exp0.Next()){
                    TopoDS_Solid solid = TopoDS::Solid(exp0.Current());
                    TopoDS_Solid newsolid = solid;
                    BRepLib::OrientClosedSolid(newsolid);
                    Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
                    // rebuild->Apply(shape);
                    rebuild->Replace(solid, newsolid, Standard_False);
                    TopoDS_Shape newshape = rebuild->Apply(shape, TopAbs_COMPSOLID, 1);
                    // TopoDS_Shape newshape = rebuild->Apply(shape);
                    shape = newshape;
                }
            }
            else
                std::cout<<" not possible"<<std::endl;
        }
    }

    if(connect) {
        //        OCC_Connect connect;
        //        for(TopExp_Explorer p(shape,TopAbs_SOLID); p.More(); p.Next())
        //            connect.Add(p.Current());
        //        connect.Connect();
        //        shape=connect;
    }
*/
    return shape;
}
//{
//
//
//    Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
//    sfs->Init(sh);
//    sfs->SetPrecision   (10e-9);
//    sfs->SetMaxTolerance(10e-9);
//    sfs->Perform();
//
//
//    TopoDS_Shape first_result = sfs->Shape();
//
//    Handle(ShapeFix_Wireframe) sfw = new ShapeFix_Wireframe(first_result);
//    sfw->SetPrecision   (10e-9);
//    sfw->SetMaxTolerance(10e-9);
//
// //   sfw->DropSmallEdgesMode() = true;
//    sfw->FixSmallEdges();
//    sfw->FixWireGaps();
//
//    TopoDS_Shape result = sfw->Shape();
//
//    Handle(ShapeFix_Shell) sfsh = new ShapeFix_Shell;
//
//    if (result.ShapeType()==TopAbs_SHELL)
//    {
//        sfsh->Init(TopoDS::Shell(result));
//        sfsh->FixFaceOrientation(TopoDS::Shell(result));
//    }
//    else if (result.ShapeType()==TopAbs_SOLID)
//    {
//        sfsh->Init(BRepTools::OuterShell(TopoDS::Solid(result)));
//        sfsh->FixFaceOrientation(BRepTools::OuterShell(TopoDS::Solid(result)));
//    }
//    sfsh->SetPrecision   (10e-9);
//    sfsh->SetMaxTolerance(10e-9);
//    sfsh->Perform();
//
//    if (sfsh->Status(ShapeExtend_DONE))
//    {
//        result = sfsh->Shape();
//    }
//
//    return result;
//
//}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::areSame(const TopoDS_Shape& sh1,
        const TopoDS_Shape& sh2){
#ifdef _DEBUG2
	std::cout<<"OCCGeomRepresentation::areSame(...,...)"<<std::endl;
#endif
    //---------------------------------------------------------------------
    //test sur les boites englobantes des 2 objets
    //---------------------------------------------------------------------
    double gap = 0;
    Bnd_Box box1, box2;
    box1.SetGap(gap);
    box2.SetGap(gap);
    BRepBndLib::Add(sh1,box1);
    BRepBndLib::Add(sh2,box2);

    double xmin1,ymin1,zmin1,xmax1,ymax1,zmax1;
    double xmin2,ymin2,zmin2,xmax2,ymax2,zmax2;
    box1.Get(xmin1,ymin1,zmin1,xmax1,ymax1,zmax1);
    box2.Get(xmin2,ymin2,zmin2,xmax2,ymax2,zmax2);

    //pour chaque boite, on considere 1/10 de sa diagonale
    double diag1 = 0.1*sqrt((xmax1-xmin1)*(xmax1-xmin1) +
            (ymax1-ymin1)*(ymax1-ymin1) +(zmax1-zmin1)*(zmax1-zmin1) );
    double diag2 = 0.1*sqrt((xmax2-xmin2)*(xmax2-xmin2) +
            (ymax2-ymin2)*(ymax2-ymin2) +(zmax2-zmin2)*(zmax2-zmin2) );

    double tol = (diag1>diag2)?diag1:diag2;

    //la tolerance est supposee assez grande pour facilement eliminer les
    //cas distants et le test suivant est suffisant
    if(xmin1<xmin2-tol || xmax1>xmax2+tol || ymin1<ymin2-tol ||
       ymax1>ymax2+tol || zmin1<zmin2-tol || zmax1>zmax2+tol )
    {
#ifdef _DEBUG2
    	std::cout<<" Box 1 : ["<<xmin1<<" - "<<xmax1<<", "<<ymin1<<" - "<<ymax1<<", "<<zmin1<<" - "<<zmax1<<"]"<<std::endl;
    	std::cout<<" Box 2 : ["<<xmin2<<" - "<<xmax2<<", "<<ymin2<<" - "<<ymax2<<", "<<zmin2<<" - "<<zmax2<<"]"<<std::endl;
    	std::cout<<"\t different boxes (tol = "<<tol<<")"<<std::endl;
#endif
        return false;
    }
    //---------------------------------------------------------------------
    //test sur la position des sommets. Si les objets ont le même nombre
    // de sommets, alors ceux-ci doivent être positionnées à la même place
    //---------------------------------------------------------------------

    TopTools_IndexedMapOfShape map_sh1, map_sh2;
    TopExp::MapShapes(sh1,TopAbs_VERTEX, map_sh1);
    TopExp::MapShapes(sh2,TopAbs_VERTEX, map_sh2);
    if(map_sh1.Extent()!=map_sh2.Extent()){
#ifdef _DEBUG2
    	std::cout<<"Diff vertices "<<map_sh1.Extent()<<" vs "<<map_sh2.Extent()<<std::endl;
#endif
       return false;
    }

    for(int i1=1;i1<=map_sh1.Extent();i1++){
        TopoDS_Vertex v1 = TopoDS::Vertex(map_sh1(i1));
        bool hasACopy=false;
        for(int i2=1;i2<=map_sh2.Extent() && !hasACopy;i2++){
                TopoDS_Vertex v2 = TopoDS::Vertex(map_sh2(i2));
                if(areEquals(v1,v2))
                    hasACopy=true;
        }
        if(!hasACopy)
            return false;
    }

    // si on atteint cette ligne, c'est que tous les sommets de sh1
    // on bien une correspondance dans sh2 (injection). Par contre,
    // ce n'est peut être pas une bijection. Ils sont peut-être tous
    // associés au même de sh2. On fait donc la vérification inverse
    // (surjection)
    for(int i1=1;i1<=map_sh2.Extent();i1++){
        TopoDS_Vertex v1 = TopoDS::Vertex(map_sh2(i1));
        bool hasACopy=false;
        for(int i2=1;i2<=map_sh1.Extent() && !hasACopy;i2++){
                TopoDS_Vertex v2 = TopoDS::Vertex(map_sh1(i2));
                if(areEquals(v1,v2))
                    hasACopy=true;
        }
        if(!hasACopy)
            return false;
    }

#ifdef _DEBUG2
    std::cout<<"return True"<<std::endl;
#endif
    // meme boite englobante et bijection sur les noeuds
    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::areEquals(const TopoDS_Solid& s1, const TopoDS_Solid& s2){
#ifdef _DEBUG2
	std::cout<<"  areEquals(TopoDS_Solid) ??? IsEqual "
			<<(s1.IsEqual(s2)?"vrai":"faux")
			<<", IsSame "
			<<(s1.IsSame(s2)?"vrai":"faux")
			<<", IsPartner "
			<<(s1.IsPartner(s2)?"vrai":"faux")
			<<std::endl;
#endif
#ifdef USE_ISSAME
	return (s1.IsSame(s2));
#endif
    if(!areSame(s1,s2))
        return false;


    // meme sommet, mais meme faces aussi ?
    TopTools_IndexedMapOfShape map_sh1, map_sh2;
    TopExp::MapShapes(s1,TopAbs_FACE, map_sh1);
    TopExp::MapShapes(s2,TopAbs_FACE, map_sh2);


    if(map_sh1.Extent()!=map_sh2.Extent())
        return false;

    for(int i1=1;i1<=map_sh1.Extent();i1++){
        TopoDS_Face f1 = TopoDS::Face(map_sh1(i1));
        bool hasACopy=false;
        for(int i2=1;i2<=map_sh2.Extent() && !hasACopy;i2++){
                TopoDS_Face f2 = TopoDS::Face(map_sh2(i2));
                if(areEquals(f1,f2))
                    hasACopy=true;
        }
        if(!hasACopy)
            return false;
    }
    for(int i1=1;i1<=map_sh2.Extent();i1++){
        TopoDS_Face f1 = TopoDS::Face(map_sh2(i1));
        bool hasACopy=false;
        for(int i2=1;i2<=map_sh1.Extent() && !hasACopy;i2++){
            TopoDS_Face f2 = TopoDS::Face(map_sh1(i2));
            if(areEquals(f1,f2))
                hasACopy=true;
        }
        if(!hasACopy)
            return false;
    }
#ifdef _DEBUG2
    std::cout<<"  return true"<<std::endl;
#endif
    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::contains(const TopoDS_Shape& sh,const TopoDS_Shape& shOther)
{
#ifdef _DEBUG2
    	std::cout<<" OCCGeomRepresentation::contains(...,...)"<<std::endl;
#endif
   double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
    double my_bounds[6];
    double bounds[6];

    Bnd_Box my_box;
    my_box.SetGap(tol);
    BRepBndLib::Add(sh,my_box);

    my_box.Get(my_bounds[0],my_bounds[2], my_bounds[4],my_bounds[1], my_bounds[3],my_bounds[5]);

    Bnd_Box box;
    box.SetGap(tol);
    BRepBndLib::Add(shOther,box);
    box.Get(bounds[0],bounds[2],bounds[4],bounds[1],bounds[3],bounds[5]);


    double local_tolX = 0.5*(bounds[1]-bounds[0]);
    double local_tolY = 0.5*(bounds[3]-bounds[2]);
    double local_tolZ = 0.5*(bounds[5]-bounds[4]);
    if(    ( bounds[0] < my_bounds[0] - local_tolX)||//minX
            ( bounds[1] > my_bounds[1] + local_tolX)||//maxX
            ( bounds[2] < my_bounds[2] - local_tolY)||//minY
            ( bounds[3] > my_bounds[3] + local_tolY)||//maxY
            ( bounds[4] < my_bounds[4] - local_tolZ)||//minZ
            ( bounds[5] > my_bounds[5] + local_tolZ) )//maxZ
    {
#ifdef _DEBUG2
    	std::cout<<"  (boites englobantes différentes) return faux"<<std::endl;
#endif
    	return false;
    }



    Standard_Boolean onlyClosed = Standard_True;
    Standard_Boolean isUseSpan = Standard_True;
    Standard_Real aDefaultTol = 1.e-7;
    Standard_Boolean CGFlag = Standard_False;
    Standard_Boolean IFlag = Standard_False;

    BRepCheck_Analyzer anaAna(sh, Standard_False);
    GProp_GProps pb;
    Standard_Real local_eps =BRepGProp::VolumePropertiesGK (sh, pb, aDefaultTol,
                                                            onlyClosed, isUseSpan,
                                                            CGFlag, IFlag);

    double myArea = pb.Mass();

    BRepCheck_Analyzer anaAnaOther(shOther, Standard_False);
    GProp_GProps pbOther;
    Standard_Real local_epsOther =BRepGProp::VolumePropertiesGK (shOther, pbOther,
                                                            aDefaultTol, onlyClosed,
                                                            isUseSpan, CGFlag, IFlag);


    double otherArea = pbOther.Mass();
    if(myArea < otherArea * (1-Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)){
#ifdef _DEBUG2
    	std::cout<<setprecision(14)<<"  (vol my ("<<myArea<<") < other ("<<otherArea
    			<<")) return faux (< à "<<otherArea * (1-Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)<<")"<<std::endl;
#endif
        return false;
    }

    BRepClass3d_SolidClassifier classifier(sh);
    //===============================================================
    // on teste les sommets
    //===============================================================
    TopTools_IndexedMapOfShape map_vertices;
    TopExp::MapShapes(shOther,TopAbs_VERTEX, map_vertices);
    for(unsigned int i=1; i<=map_vertices.Extent();i++){
        TopoDS_Shape s_i = map_vertices.FindKey(i);
        TopoDS_Vertex v_i = TopoDS::Vertex(s_i);
        gp_Pnt p_i   = BRep_Tool::Pnt(v_i);

        classifier.Perform(p_i,1e-6);
        TopAbs_State result = classifier.State();
        if(result==TopAbs_OUT || result==TopAbs_UNKNOWN){
#ifdef _DEBUG2
        	std::cout<<"  (sommets différents) return faux"<<std::endl;
#endif
            return false;
        }

    }
    //===============================================================
    // On ne teste pas les courbes car cela pose de problèmes
    // numériques avec OCC
    //===============================================================

    //===============================================================
    // On teste les surfaces
    //===============================================================
    //maintenant on regardes si des points internes de ASurf sont ou pas sur la
    //surface (*this). Les points internes en questions sont 3 points pris parmi
    //ceux des triangles discrétisant la face
    TopTools_IndexedMapOfShape map_faces;
    TopExp::MapShapes(shOther,TopAbs_FACE, map_faces);
    for(unsigned int i=1; i<=map_faces.Extent();i++){
        TopoDS_Shape s_i = map_faces.FindKey(i);
        TopoDS_Face  f_i = TopoDS::Face(s_i);


        BRepCheck_Analyzer anaAna(f_i, Standard_False);

        GProp_GProps pb;
        BRepGProp::SurfaceProperties(f_i,pb);

        double surf_area = pb.Mass();
        if(surf_area<1e-4){
            std::cout<<"=====  OCC: UNE SURFACE OCC D'AIRE "<<surf_area<<std::endl;
        }
        else{
            TopLoc_Location aLoc;
            Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(f_i,aLoc);
            Handle_Geom_Surface surf = BRep_Tool::Surface(f_i);
            // On récupère la transformation de la shape/face
            if (aPoly.IsNull()){
                OCCGeomRepresentation::buildIncrementalBRepMesh(f_i, 0.1);
                aPoly = BRep_Tool::Triangulation(f_i,aLoc);
            }
            if (aPoly.IsNull()){
#ifdef _DEBUG2
            	std::cout<<"  (aPoly.IsNull()) return vrai"<<std::endl;
#endif
                return true;
            }
            gp_Trsf myTransf;
            Standard_Boolean identity = true;
            if (!aLoc.IsIdentity()) {
                identity = false;
                myTransf = aLoc.Transformation();
            }
            int nbTriInFace = aPoly->NbTriangles();
            if(nbTriInFace==0){
#ifdef _DEBUG2
            	std::cout<<"  (nbTriInFace==0) return vrai"<<std::endl;
#endif
                return true;
            }


            int selectTriangleID=1;
            if(nbTriInFace>2)
                selectTriangleID= nbTriInFace/2;

            const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
            const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();

            // Get the triangle
            //   std::cout<<"Triangle choisi : "<<selectTriangleID<<std::endl;
            Standard_Integer N1,N2,N3;
            Triangles(selectTriangleID).Get(N1,N2,N3);
            gp_Pnt V1 = Nodes(N1), V2 = Nodes(N2), V3 = Nodes(N3);
            // on positionne correctement les sommets
            if (!identity) {
                V1.Transform(myTransf);
                V2.Transform(myTransf);
                V3.Transform(myTransf);
            }
            //on calcule le point milieu a projeter
            gp_Pnt mp(  (V1.X()+V2.X()+V3.X())/3.0,
                    (V1.Y()+V2.Y()+V3.Y())/3.0,
                    (V1.Z()+V2.Z()+V3.Z())/3.0);
            //on le projete sur la surface de départ pour ne pas avoir
            //une erreur du à la déflection entre le triangle et la surface
            GeomAPI_ProjectPointOnSurf proj(mp,surf);
            proj.Perform(mp);

            gp_Pnt res;
            if(proj.NbPoints()!=0){
                res = proj.NearestPoint();
            }
            else
            	throw TkUtil::Exception(TkUtil::UTF8String ("Problème de projection dans Surface::contains", TkUtil::Charset::UTF_8));

            gp_Pnt p_i   = res;
            classifier.Perform(p_i,1e-6);
            TopAbs_State result = classifier.State();
            if(result==TopAbs_OUT || result==TopAbs_UNKNOWN){
#ifdef _DEBUG2
            	std::cout<<"  (une face OUT) return faux"<<std::endl;
#endif
                return false;
            }
        }
    }
#ifdef _DEBUG2
    std::cout<<"  (par défaut) return vrai"<<std::endl;
#endif
    return true;

}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::areEquals(const TopoDS_Face& f1,const TopoDS_Face& f2){
#ifdef _DEBUG2
	std::cout<<"  areEquals(TopoDS_Face) ??? IsEqual "
			<<(f1.IsEqual(f2)?"vrai":"faux")
			<<", IsSame "
			<<(f1.IsSame(f2)?"vrai":"faux")
			<<", IsPartner "
			<<(f1.IsPartner(f2)?"vrai":"faux")
			<<std::endl;
#endif
#ifdef USE_ISSAME
	return (f1.IsSame(f2));
#endif

    if(!areSame(f1,f2)){
#ifdef _DEBUG2
    	std::cout<<"Boites englobantes différentes"<<std::endl;
#endif
        return false;
    }
#ifdef _DEBUG2
    std::cout<<"Boites englobantes équivalentes, on continue"<<std::endl;
#endif

    // meme sommet, mais meme courbes aussi ?
    TopTools_IndexedMapOfShape map_sh1, map_sh2;
    TopExp::MapShapes(f1,TopAbs_EDGE, map_sh1);
    TopExp::MapShapes(f2,TopAbs_EDGE, map_sh2);


    // we compare the number of "non null" boundary curves
    int null_1 = 0, null_2 = 0;
    for(int i1=1;i1<=map_sh1.Extent();i1++){
        TopoDS_Edge e1 = TopoDS::Edge(map_sh1(i1));
        BRepCheck_Analyzer anaAna1(e1, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(e1,pb);

        if(pb.Mass()<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
            null_1++;
    }
    for(int i2=1;i2<=map_sh2.Extent();i2++){
        TopoDS_Edge e2 = TopoDS::Edge(map_sh2(i2));
        BRepCheck_Analyzer anaAna2(e2, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(e2,pb);

        if(pb.Mass()<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
            null_2++;
    }

    if(map_sh1.Extent()-null_1!=map_sh2.Extent()-null_2){
#ifdef _DEBUG2
        std::cout<<"NOT THE SAME NUMBER OF NON NULL BOUNDARY CURVES"<<std::endl;
        std::cout<<"("<<map_sh1.Extent()<<", "<<null_1<<"), ("<<map_sh2.Extent()<<", "<<null_2<<")"<<std::endl;
#endif
        return false;
    }

#ifdef _DEBUG2
    std::cout<<"Comparaison des courbes "<<map_sh1.Extent()<<" "<<map_sh2.Extent()<<std::endl;
#endif
    for(int i1=1;i1<=map_sh1.Extent();i1++){
        TopoDS_Edge e1 = TopoDS::Edge(map_sh1(i1));
        BRepCheck_Analyzer anaAna(e1, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(e1,pb);
        // si la courbe est de longueur nulle, on l'ignore
#ifdef _DEBUG2
        std::cout<<"courbe "<<i1<<std::endl;
#endif
        if(pb.Mass()>=Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
            bool hasACopy=false;
            for(int i2=1;i2<=map_sh2.Extent();i2++){//&& !hasACopy;i2++){
                TopoDS_Edge e2 = TopoDS::Edge(map_sh2(i2));
                if(areEquals(e1,e2))
                    hasACopy=true;
            }
            if(!hasACopy){
#ifdef _DEBUG2
                std::cout<<"\t une courbe est différente"<<std::endl;
#endif
                return false;
            }
        }
#ifdef _DEBUG2
        else std::cout<<"\t longueur nulle"<<std::endl;
#endif
    }

//    for(int i1=1;i1<=map_sh2.Extent();i1++){
//        TopoDS_Edge e1 = TopoDS::Edge(map_sh2(i1));
//        BRepCheck_Analyzer anaAna(e1, Standard_False);
//        GProp_GProps pb;
//        BRepGProp::LinearProperties(e1,pb);
//        // si la courbe est de longueur nulle, on l'ignore
//        if(pb.Mass()>=1e-10){
//            bool hasACopy=false;
//            for(int i2=1;i2<=map_sh1.Extent() && !hasACopy;i2++){
//                TopoDS_Edge e2 = TopoDS::Edge(map_sh1(i2));
//                if(areEquals(e1,e2))
//                    hasACopy=true;
//            }
//            if(!hasACopy)
//                return false;
//        }
//    }
#ifdef _DEBUG2
    std::cout<<"=========== CE SONT LES MEMES"<<std::endl;
#endif
    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::areEquals(const TopoDS_Edge& e1, const TopoDS_Edge& e2)
{
#ifdef _DEBUG2
	std::cout<<"  areEquals(TopoDS_Edge) ??? IsEqual "
			<<(e1.IsEqual(e2)?"vrai":"faux")
			<<", IsSame "
			<<(e1.IsSame(e2)?"vrai":"faux")
			<<", IsPartner "
			<<(e1.IsPartner(e2)?"vrai":"faux")
			<<std::endl;
#endif
#ifdef USE_ISSAME
	return (e1.IsSame(e2));
#endif

    if (BRep_Tool::Degenerated(e1) && BRep_Tool::Degenerated(e2)){
        TopTools_IndexedMapOfShape map_vertices1, map_vertices2;
        TopExp::MapShapes(e1,TopAbs_VERTEX, map_vertices1);
        TopExp::MapShapes(e2,TopAbs_VERTEX, map_vertices2);
        if(map_vertices1.Extent()!=map_vertices2.Extent())
            return false;
        //donc egaux
        if(map_vertices1.Extent()==1){
            TopoDS_Vertex v1= TopoDS::Vertex(map_vertices1.FindKey(1));
            TopoDS_Vertex v2= TopoDS::Vertex(map_vertices2.FindKey(1));
            return areEquals(v1,v2);
        }
        else
        	throw TkUtil::Exception(TkUtil::UTF8String ("Problème de comparaisons entre deux courbes dégénérées", TkUtil::Charset::UTF_8));
    }
    else if (BRep_Tool::Degenerated(e2))
                return false;
    else if (BRep_Tool::Degenerated(e1))
                return false;

    TopoDS_Vertex V1, V2, V3, V4;
    TopExp::Vertices( e1, V1, V2 );
    TopExp::Vertices( e2, V3, V4);

    gp_Pnt p1 = BRep_Tool::Pnt(V1);
    gp_Pnt p2 = BRep_Tool::Pnt(V2);
    gp_Pnt p3 = BRep_Tool::Pnt(V3);
    gp_Pnt p4 = BRep_Tool::Pnt(V4);
//    std::cout<<"courbe 1 P1 ("<<p1.X()<<", "<<p1.Y()<<", "<<p1.Z()<<") "<<std::endl;
//    std::cout<<"courbe 1 P2 ("<<p2.X()<<", "<<p2.Y()<<", "<<p2.Z()<<") "<<std::endl;
//    std::cout<<"courbe 2 P1 ("<<p3.X()<<", "<<p3.Y()<<", "<<p3.Z()<<") "<<std::endl;
//    std::cout<<"courbe 2 P2 ("<<p4.X()<<", "<<p4.Y()<<", "<<p4.Z()<<") "<<std::endl;

    if (!areEquals(V1,V4) && !areEquals(V1,V3))
        return false;

    if (!areEquals(V2,V4) && !areEquals(V2,V3))
        return false;




    // e1 and e2 ont les memes sommets. On compare sur des points
    // internes à la courbe.
    Handle(Geom_Curve) C1, C2;
    Standard_Real f1, l1, f2, l2, tol;
    C1 = BRep_Tool::Curve( e1, f1, l1);
    C2 = BRep_Tool::Curve( e2, f2, l2);

    // on utilise la tolérance max de e1 et e2
    tol = Max(BRep_Tool::Tolerance(e1),BRep_Tool::Tolerance(e2));
//    std::cout<<"("<<f1<<", "<<l1<<") -- ("<<f2<<", "<<l2<<") + tol "<<tol <<std::endl;
    //std::cout<<e1.Orientation()<<" "<<e2.Orientation()<<std::endl;


    double param = f1+(l1-f1)*0.5;

    gp_Pnt local_pnt = C1->Value( param );
    GeomAPI_ProjectPointOnCurve proj(local_pnt,C2,f2,l2);

//    gp_Pnt C1_first, C1_mid, C1_last, C2_first, C2_mid, C2_last;
//    C1_first = C1->Value(f1);
//    C1_mid = C1->Value(f1+(l1-f1)/2.0);
//    C1_last = C1->Value(l1);
//    C2_first = C2->Value(f2);
//    C2_mid = C2->Value(f2+(l2-f2)/2.0);
//    C2_last = C2->Value(l2);
//    std::cout<<"C1 - first point ("<<C1_first.X()<<", "<<C1_first.Y()<<", "<<C1_first.Z()<<")\n";
//    std::cout<<"C1 - mid point   ("<<C1_mid.X()<<", "<<C1_mid.Y()<<", "<<C1_mid.Z()<<")\n";
//    std::cout<<"C1 - last point  ("<<C1_last.X()<<", "<<C1_last.Y()<<", "<<C1_last.Z()<<")\n";
//    std::cout<<"C2 - first point ("<<C2_first.X()<<", "<<C2_first.Y()<<", "<<C2_first.Z()<<")\n";
//    std::cout<<"C2 - mid point   ("<<C2_mid.X()<<", "<<C2_mid.Y()<<", "<<C2_mid.Z()<<")\n";
//    std::cout<<"C2 - last point  ("<<C2_last.X()<<", "<<C2_last.Y()<<", "<<C2_last.Z()<<")\n";
//    std::cout<<"POINT A PROJETER ("<<local_pnt.X()<<", "<<local_pnt.Y()<<", "<<local_pnt.Z()<<")"<<std::endl;
    proj.Perform(local_pnt);


    if(proj.NbPoints()==0){
//        std::cout<<"aucun projete"<<std::endl;
        //message <<"OCCGeomRepresentation::areEquals(...) - Fail to compare curve of end points";
        return false;
       // throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe pour la comparaison entre 2 courbes!!");
    }
    else
    {
//        std::cout<<"NB POINTS : "<<proj.NbPoints()<<std::endl;
        bool oneWrongProjection = false;
        bool oneGoodProjection = false;
        int nb_proj = 0;
        for(int i_point=1; i_point<=proj.NbPoints();i_point++){
//            std::cout<<"\t point "<<i_point<<std::endl;
            gp_Pnt res = proj.Point(i_point);//NearestPoint();
            // on élimine les points qui ne sont pas dans le bon Range
            TopoDS_Vertex Vres = BRepBuilderAPI_MakeVertex(res);
            double param_res = proj.Parameter(i_point);

//            std::cout<<param_res<<" --> ("<<res.X()<<", "<<res.Y()<<", "<<res.Z()<<")"<<std::endl;
            // Maintenant on regarde si l'on est bien dans l'intervalle de la courbe
            // d'arrivée
            BRepAdaptor_Curve brepCurve(e2);
            double f= brepCurve.FirstParameter();
            double l= brepCurve.LastParameter();
//            std::cout<<"(FIRST,LAST) = ("<<f<<", "<<l<<")"<<std::endl;
//            if(e2.Orientation()==TopAbs_REVERSED)
//                std::cout<<"TOPO REVERSED"<<std::endl;
//            else if(e2.Orientation()==TopAbs_FORWARD)
//                std::cout<<"TOPO FORWARD"<<std::endl;

//            if(e2.Orientation()==TopAbs_REVERSED && f2>l2 && (param_res>f2 && param_res<l2) ){
//                std::cout<<" NON"<<std::endl;
//                continue;
//            }
//            else
            if( (param_res<f2 || param_res>l2)){
                //std::cout<<" NON 2"<<std::endl;
                continue;
            }
            //Arrivée ici, la projection est nécessairement pertinente
            if (res.Distance(local_pnt)> tol){
//                std::cout<<"POINT A PROJETER ("<<local_pnt.X()<<", "<<local_pnt.Y()<<", "<<local_pnt.Z()<<") SUR"<<std::endl;
//                std::cout<<i_point<<" => dist: "<<res.Distance(local_pnt)<<std::endl;
//                std::cout<<"TROP LOIN"<<std::endl;
                oneWrongProjection=true;
                nb_proj++;
            }
            else{
                oneGoodProjection = true;
                nb_proj++;
//                std::cout<<"\t projection OK"<<std::endl;
            }

        }

        if(oneGoodProjection){
//            std::cout<<"---------> EGAL"<<std::endl;

#ifdef _DEBUG2
    std::cout<<"=========== CE SONT LES MEMES"<<std::endl;
#endif
            return true;
        }
        else if(oneWrongProjection)
        {
//            std::cout<<"---------> DIFF"<<std::endl;
            return false;
        }
        else if(nb_proj==0)
        {
 //           std::cout<<"---------> DIFF"<<std::endl;
            return false;
        }
    }
#ifdef _DEBUG2
    std::cout<<"=========== CE SONT LES MEMES"<<std::endl;
#endif
    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::areEquals(const TopoDS_Wire& w1, const TopoDS_Wire& w2)
{
#ifdef _DEBUG2
	std::cout<<"  areEquals(TopoDS_Wire) ??? IsEqual "
			<<(w1.IsEqual(w2)?"vrai":"faux")
			<<", IsSame "
			<<(w1.IsSame(w2)?"vrai":"faux")
			<<", IsPartner "
			<<(w1.IsPartner(w2)?"vrai":"faux")
			<<std::endl;
#endif
#ifdef USE_ISSAME
	return (w1.IsSame(w2));
#endif

    TopTools_IndexedMapOfShape map1, map2;
    TopExp::MapShapes(w1,TopAbs_EDGE, map1);
    TopExp::MapShapes(w2,TopAbs_EDGE, map2);

    for(int i1=1;i1<=map1.Extent();i1++){
        TopoDS_Edge e1 = TopoDS::Edge(map1.FindKey(i1));
        bool isIn = false;
        for(int i2=1;i2<=map2.Extent() && !isIn;i2++){
            TopoDS_Edge e2 = TopoDS::Edge(map2.FindKey(i2));
            if(OCCGeomRepresentation::isIn(e2,e1))
                isIn=true;

        }
        if(!isIn)//e1 n'est contenu dans aucune arête de e2
            return false;
    }

    for(int i2=1;i2<=map2.Extent();i2++){
        TopoDS_Edge e2 = TopoDS::Edge(map2.FindKey(i2));
        bool isIn = false;
        for(int i1=1;i1<=map1.Extent();i1++){
            TopoDS_Edge e1 = TopoDS::Edge(map1.FindKey(i1));
            if(OCCGeomRepresentation::isIn(e1,e2))
                isIn=true;

        }
        if(!isIn)//e2 n'est contenu dans aucune arête de e1
            return false;
    }

    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::
isIn(const TopoDS_Edge& eComposite, const TopoDS_Edge& eComponent)
{
    if(BRep_Tool::Degenerated(eComposite) && BRep_Tool::Degenerated(eComponent))
    {
        TopTools_IndexedMapOfShape map_vertices1, map_vertices2;
        TopExp::MapShapes(eComponent,TopAbs_VERTEX, map_vertices1);
        TopExp::MapShapes(eComponent,TopAbs_VERTEX, map_vertices2);
        if(map_vertices1.Extent()!=map_vertices2.Extent())
            return false;
        //donc egaut
        if(map_vertices1.Extent()==1){
            TopoDS_Vertex v1= TopoDS::Vertex(map_vertices1.FindKey(1));
            TopoDS_Vertex v2= TopoDS::Vertex(map_vertices2.FindKey(1));
            return areEquals(v1,v2);
        }
        else
        	throw TkUtil::Exception(TkUtil::UTF8String ("Problème de comparaisons entre deux courbes dégénérées", TkUtil::Charset::UTF_8));
    }
    else if (BRep_Tool::Degenerated(eComponent))
        return false;
    else if (BRep_Tool::Degenerated(eComposite))
        return false;


    //On récupère les courbes géométriques*
    Handle(Geom_Curve) C1, C2;
    Standard_Real f1, l1, f2, l2, tol;
    C1 = BRep_Tool::Curve( eComposite, f1, l1);
    C2 = BRep_Tool::Curve( eComponent, f2, l2);

    // on utilise la tolérance max de e1 et e2
    tol = Max(BRep_Tool::Tolerance(eComposite),BRep_Tool::Tolerance(eComponent));
    double nbi=4;
    //on projete des points de C2 sur C1 s'ils sont tous sous la
    //tolérance, c'est bon
    for (int i=0; i<=nbi; i++) {

        double param = f2+(l2-f2)*i/nbi;

        gp_Pnt local_pnt = C2->Value( param );
        GeomAPI_ProjectPointOnCurve proj(local_pnt,C1);
        proj.Perform(local_pnt);

        if(proj.NbPoints()==0){
            return false;
        }
        else{
//            std::cout<<"NB POINTS : "<<proj.NbPoints()<<std::endl;
            for(int i_point=1; i_point<=proj.NbPoints();i_point++){
//                std::cout<<"\t point "<<i_point<<std::endl;
                gp_Pnt res = proj.NearestPoint();
                // on élimine les points qui ne sont pas dans le bon Range
                TopoDS_Vertex Vres = BRepBuilderAPI_MakeVertex(res);
                double param_res = proj.Parameter(i_point);
                if(eComposite.Orientation()==TopAbs_REVERSED && (param_res>f1 && param_res<l1) ){
                    continue;

                }
                else if( (param_res<f1 || param_res>l1)){
                  continue;
                }
//                std::cout<<" OUI"<<std::endl;
              if (res.Distance(local_pnt)> tol){ //TODO [FL] Attention, encore un probleme de tolerance
//                  std::cout<<"POINT A PROJETER ("<<local_pnt.X()<<", "<<local_pnt.Y()<<", "<<local_pnt.Z()<<") SUR"<<std::endl;
//                  std::cout<<i_point<<" => dist: "<<res.Distance(local_pnt)<<std::endl;
                  std::cout<<"TROP LOIN"<<std::endl;
                    return false;
              }
//              std::cout<<"\t projection OK"<<std::endl;
            }
        }

    }
//    std::cout<<"MEME COURBES"<<std::endl;
    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCGeomRepresentation::areEquals(const TopoDS_Vertex& v1,
        const TopoDS_Vertex& v2)
{
#ifdef _DEBUG2
	std::cout<<"  areEquals(TopoDS_Vertex) ??? IsEqual "
			<<(v1.IsEqual(v2)?"vrai":"faux")
			<<", IsSame "
			<<(v1.IsSame(v2)?"vrai":"faux")
			<<", IsPartner "
			<<(v1.IsPartner(v2)?"vrai":"faux")
			<<std::endl;
#endif
#ifdef USE_ISSAME
	return (v1.IsSame(v2));
#endif

    gp_Pnt gp1, gp2;
    gp1 = BRep_Tool::Pnt(v1);
    gp2 = BRep_Tool::Pnt(v2);

    Utils::Math::Point p1(gp1.X(),gp1.Y(),gp1.Z());
    Utils::Math::Point p2(gp2.X(),gp2.Y(),gp2.Z());
    bool res = Utils::Math::MgxNumeric::isNearlyZero(p1.length(p2),
                    Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon);
#ifdef _DEBUG2
    std::cout<<"OCCGeomRepresentation::areEquals avec "<<p1<<" et "<<p2<<std::endl;
    if (res)
    	std::cout<<"=========== CE SONT LES MEMES"<<std::endl;
    else
    	std::cout<<"différents"<<std::endl;
#endif

    return res;
}

/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::split(std::vector<Surface*>& surf,
        std::vector<Curve*  >& curv, std::vector<Vertex* >&  vert,Volume* owner)
{
	if (useOCAF()){
    	OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(owner->getComputationalProperty());
    	CHECK_NULL_PTR_ERROR(occ_rep);
		setRootLabel(occ_rep->getLabel());
	}
    /* on va explorer le solide OCC stocké en attribut et créer les entités de
     * dimension directement inférieure, c'est-à-dire les faces
     */
    TopExp_Explorer e;
//    if (m_shape.ShapeType()!=TopAbs_SOLID  && m_shape.ShapeType()!=TopAbs_SHELL)
//        throw TkUtil::Exception("Wrong OCC shape type!!!");

    /* on crée les faces */
    std::vector<TopoDS_Shape> OCCFaces;
    std::vector<Surface*>     Mgx3DSurfaces;
    std::vector<TopoDS_Shape> OCCCurves;
    std::vector<Curve *>      Mgx3DCurves;


    for(e.Init(m_shape, TopAbs_FACE); e.More(); e.Next())
    {

        TopoDS_Face F = TopoDS::Face(e.Current());

        Surface* s = newOCCSurface(F);

        // correspondance entre shapes OCC et géométries Mgx3D
        OCCFaces.push_back(F);
        Mgx3DSurfaces.push_back(s);
        // on crée le lien V->F
        owner->add(s);
        // on crée le lien F->V
        s->add(owner);
    }

    // maintenant que les faces sont créées, on crée les arêtes
    TopTools_IndexedDataMapOfShapeListOfShape map;
    TopExp::MapShapesAndAncestors(m_shape, TopAbs_EDGE, TopAbs_FACE, map);
    // on a ainsi toutes les arêtes dans map et pour chaque arête, on
    // connait les faces auxquelles elle appartient.

    TopTools_IndexedMapOfShape map_edges;
    TopExp::MapShapes(m_shape,TopAbs_EDGE, map_edges);
    TopTools_ListOfShape listFaces;


    for(int i = 1; i <= map_edges.Extent(); i++)
    {
        TopoDS_Edge E = TopoDS::Edge(map_edges(i));
        BRepCheck_Analyzer anaAna(E, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(E,pb);
        double res = pb.Mass();
        if(res<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Une courbe de taille nulle n'a pas ete creee pour l'entité géométrique "<<owner->getName();
            m_context.getLogDispatcher().log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));
            continue;
        }

        Curve* c = newOCCCurve(E);

        // correspondance entre shapes OCC et géométries Mgx3D
        OCCCurves.push_back(E);
        Mgx3DCurves.push_back(c);

        /* on récupère les faces contenant cette arête et déjà crées donc
         */
        listFaces = map.FindFromKey(E);
        TopTools_ListIteratorOfListOfShape it_faces;
        for(it_faces.Initialize(listFaces);it_faces.More();it_faces.Next()){
            TopoDS_Shape shape =  it_faces.Value();

           Surface *s= 0;
           bool not_find_shape = true;
           for(int i =0; i<OCCFaces.size() && not_find_shape; i++)
               if(shape.IsSame(OCCFaces[i])){
                   not_find_shape = false;
                   s = Mgx3DSurfaces[i];
               }

           // on crée le lien S->C
           s->add(c);
           // on crée le lien C->S
           c->add(s);

        }
    }

    // maintenant que les faces et les aretes sont créées, on crée les
    // sommets
    TopExp::MapShapesAndAncestors(m_shape, TopAbs_VERTEX, TopAbs_EDGE, map);
    // on a ainsi tous les sommets dans map et pour chaque sommet, on
    // connait les aretes auxquelles il appartient.

    /* on crée les labels contenants les sommets et pour chaque sommet,
     * on fait pointer une ref à partir des labels ayant les aretes
     * correspondantes */
    TopTools_IndexedMapOfShape map_vertices;
    TopExp::MapShapes(m_shape,TopAbs_VERTEX, map_vertices);
    TopTools_ListOfShape listEdges;

    for(int i = 1; i <= map_vertices.Extent(); i++)
    {
        TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
        // creation du sommet
        Vertex* v = newOCCVertex(V);

        /* on récupère les arêtes contenant ce sommet. Mais attention, ce nb
         * d'arêtes est trop important car des doublons existent.
         */
        listEdges = map.FindFromKey(V);

        TopTools_ListIteratorOfListOfShape it_edges;

        // ce vecteur nous sert à ne pas récupérer 2 fois le même sommet
        std::vector<bool> still_done;
        still_done.resize(Mgx3DCurves.size(),0);

        for(it_edges.Initialize(listEdges);it_edges.More();it_edges.Next()){
           TopoDS_Shape shape =  it_edges.Value();
           Curve *c= 0;
           bool not_find_shape = true;
           for(int i =0; i<OCCCurves.size() && not_find_shape; i++){
               if(shape.IsSame(OCCCurves[i]) && !still_done[i]){
                   not_find_shape = false;
                   c = Mgx3DCurves[i];
                   still_done[i] = true;
               }
           }
           // si on a trouvé la shape et qu'elle n'avait pas déjà été traitée
           if(!not_find_shape){
               // on crée le lien C->V
               c->add(v);
               // on crée le lien V->C
               v->add(c);
           }

        }
    }

    // on renseigne la fonction appelante
    owner->get(surf);
    owner->get(curv);
    owner->get(vert);

    if (useOCAF())
    	setRootLabel(EntityFactory::getOCAFRootLabel());
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::split(std::vector<Curve*  >& curv,
        std::vector<Vertex* >&  vert,Surface* owner)
{
	if (useOCAF()){
    	OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(owner->getComputationalProperty());
    	CHECK_NULL_PTR_ERROR(occ_rep);
		setRootLabel(occ_rep->getLabel());
	}

    /* on va explorer la face OCC stocké en attribut et créer les entités de
     * dimension directement inférieure, c'est-à-dire les courbes
     */
    TopExp_Explorer e;

    /* on crée les faces */
    std::vector<TopoDS_Shape> OCCCurves;
    std::vector<Curve *>      Mgx3DCurves;


    for(e.Init(m_shape, TopAbs_EDGE); e.More(); e.Next())
    {

        TopoDS_Edge E = TopoDS::Edge(e.Current());

        Curve* c = newOCCCurve(E);

        // correspondance entre shapes OCC et géométries Mgx3D
        OCCCurves.push_back(E);
        Mgx3DCurves.push_back(c);
        // on crée le lien V->F
        owner->add(c);
        // on crée le lien F->V
        c->add(owner);
    }


    // maintenant que les aretes sont créées, on crée les
    // sommets
    TopTools_IndexedDataMapOfShapeListOfShape map;
    TopExp::MapShapesAndAncestors(m_shape, TopAbs_VERTEX, TopAbs_EDGE, map);
    // on a ainsi tous les sommets dans map et pour chaque sommet, on
    // connait les aretes auxquelles il appartient.

    /* on crée les labels contenants les sommets et pour chaque sommet,
     * on fait pointer une ref à partir des labels ayant les aretes
     * correspondantes */
    TopTools_IndexedMapOfShape map_vertices;
    TopExp::MapShapes(m_shape,TopAbs_VERTEX, map_vertices);
    TopTools_ListOfShape listEdges;

    for(int i = 1; i <= map_vertices.Extent(); i++)
    {
        TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
        // creation du sommet
        Vertex* v = newOCCVertex(V);

        /* on récupère les arêtes contenant ce sommet. Mais attention, ce nb
         * d'arêtes est trop important car des doublons existent.
         */
        listEdges = map.FindFromKey(V);

        TopTools_ListIteratorOfListOfShape it_edges;

        // ce vecteur nous sert à ne pas récupérer 2 fois le même sommet
        std::vector<bool> still_done;
        still_done.resize(Mgx3DCurves.size(),0);

        for(it_edges.Initialize(listEdges);it_edges.More();it_edges.Next()){
           TopoDS_Shape shape =  it_edges.Value();
           Curve *c= 0;
           bool not_find_shape = true;
           for(int i =0; i<OCCCurves.size() && not_find_shape; i++){
               if(shape.IsSame(OCCCurves[i]) && !still_done[i]){
                   not_find_shape = false;
                   c = Mgx3DCurves[i];
                   still_done[i] = true;
               }
           }
           // si on a trouvé la shape et qu'elle n'avait pas déjà été traitée
           if(!not_find_shape){
               // on crée le lien C->V
               c->add(v);
               // on crée le lien V->C
               v->add(c);
           }

        }
    }

    // on renseigne la fonction appelante
    owner->get(curv);
    owner->get(vert);

    if (useOCAF())
    	setRootLabel(EntityFactory::getOCAFRootLabel());
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::split(std::vector<Vertex* >&  vert,Curve* owner)
{
#ifdef _DEBUG2
	std::cout<<"OCCGeomRepresentation::split avec vert.size () = "<<vert.size ()<<std::endl;
#endif

	if (useOCAF()){
    	OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(owner->getComputationalProperty());
    	CHECK_NULL_PTR_ERROR(occ_rep);
		setRootLabel(occ_rep->getLabel());
	}

    /* on va explorer la courbe OCC stockée en attribut et créer les entités de
     * dimension directement inférieure, c'est-à-dire les sommets
     */
	Vertex* v = 0;
	TopExp_Explorer e;
    for(e.Init(m_shape, TopAbs_VERTEX); e.More(); e.Next())
    {

        TopoDS_Vertex V = TopoDS::Vertex(e.Current());
        // on évite de mettre 2 fois le même sommet [EB]
        bool are_same = false;
        if (v){
        	OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(v->getComputationalProperty());
        	CHECK_NULL_PTR_ERROR(occ_rep);
        	TopoDS_Vertex Vprec = TopoDS::Vertex(occ_rep->getShape());
        	if (Vprec.IsSame(V))
        		are_same = true;
        }

        if (!are_same){
        	// création du nouveau sommet
        	v = newOCCVertex(V);

        	// on crée le lien C->V
        	owner->add(v);
        	// on crée le lien V->C
        	v->add(owner);
        }
    }

    // on renseigne la fonction appelante
    owner->get(vert);

    if (useOCAF())
     	setRootLabel(EntityFactory::getOCAFRootLabel());
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::project(Utils::Math::Point& P, const Curve* C)
{
    projectPointOn(P);
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::project(Utils::Math::Point& P, const Surface* S)
{
    projectPointOn(P);

//	double tol = 1.0e-7;
//	gp_Pnt aPnt(P.getX(),P.getY(),P.getZ());
//
//	// version Salomé d'une projection d'un point sur une surface
//	// 3 fois plus rapide que projectPointOn, mais pb avec tests unitaires
//
//	TopoDS_Face aFace = TopoDS::Face(m_shape);
//	Handle(Geom_Surface) surface = BRep_Tool::Surface(aFace);
//	double U1, U2, V1, V2;
//	//surface->Bounds(U1, U2, V1, V2);
//	BRepTools::UVBounds(aFace, U1, U2, V1, V2);
//
//	// projector
//	GeomAPI_ProjectPointOnSurf proj;
//	proj.Init(surface, U1, U2, V1, V2, tol);
//
//	//gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(anOriginal));
//	proj.Perform(aPnt);
//	if (!proj.IsDone()) {
//		Standard_ConstructionError::Raise
//		("Projection aborted : the algorithm failed");
//	}
//	int nbPoints = proj.NbPoints();
//	if (nbPoints < 1) {
//		Standard_ConstructionError::Raise("No solution found");
//	}
//
//	Quantity_Parameter U, V;
//	proj.LowerDistanceParameters(U, V);
//	gp_Pnt2d aProjPnt (U, V);
//
//	// classifier
//	BRepClass_FaceClassifier aClsf (aFace, aProjPnt, tol);
//	if (aClsf.State() != TopAbs_IN && aClsf.State() != TopAbs_ON) {
//		bool isSol = false;
//		double minDist = RealLast();
//		for (int i = 1; i <= nbPoints; i++) {
//			Quantity_Parameter Ui, Vi;
//			proj.Parameters(i, Ui, Vi);
//			aProjPnt = gp_Pnt2d(Ui, Vi);
//			aClsf.Perform(aFace, aProjPnt, tol);
//			if (aClsf.State() == TopAbs_IN || aClsf.State() == TopAbs_ON) {
//				isSol = true;
//				double dist = proj.Distance(i);
//				if (dist < minDist) {
//					minDist = dist;
//					U = Ui;
//					V = Vi;
//				}
//			}
//		}
//		if (!isSol) {
//			Standard_ConstructionError::Raise("No solution found");
//		}
//	}
//
//	gp_Pnt surfPnt = surface->Value(U, V);
//
//	//aShape = BRepBuilderAPI_MakeVertex(surfPnt).Shape();
//
//
//	P.setXYZ(surfPnt.X(), surfPnt.Y(), surfPnt.Z());
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::project(const Utils::Math::Point& P1, Utils::Math::Point& P2,
        const Curve* C)
{
    P2=P1;
    projectPointOn(P2);
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::project(const Utils::Math::Point& P1, Utils::Math::Point& P2,
                                       const Surface* S)
{
    P2=P1;
//    project(P2, S);
    projectPointOn(P2);
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::projectPointOn( Utils::Math::Point& P)
{

	if(m_shape.ShapeType()==TopAbs_VERTEX)
	{
		gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(m_shape));
		P.setXYZ(pnt.X(), pnt.Y(), pnt.Z());
	}
	else
	{
		gp_Pnt pnt(P.getX(),P.getY(),P.getZ());
		TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
		BRepExtrema_DistShapeShape extrema(V, m_shape);
		bool isDone = extrema.IsDone();
		if(!isDone) {
			isDone = extrema.Perform();
		}

		if(!isDone){
			std::cerr<<"OCCGeomRepresentation::projectPointOn("<<P<<")\n";
			throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");

		}
		gp_Pnt pnt2 = extrema.PointOnShape2(1);
		P.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
//		std::cout<<"OCCGeomRepresentation::projectPointOn("<<pnt.X()<<", "<<pnt.Y()<<", "<<pnt.Z()
//				<<") => "<<pnt2.X()<<", "<<pnt2.Y()<<", "<<pnt2.Z()<<" distance : "<<pnt.Distance(pnt2)<<std::endl;
	}
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S)
{
	//std::cout<<"OCCGeomRepresentation::normal avec m_shape "<<(m_shape.IsNull()?"vide":"non vide")<<std::endl;
	// projection pour en déduire les paramètres
	gp_Pnt pnt(P1.getX(),P1.getY(),P1.getZ());
	TopoDS_Vertex Vtx = BRepBuilderAPI_MakeVertex(pnt);
	BRepExtrema_DistShapeShape extrema(Vtx, m_shape);
	bool isDone = extrema.IsDone();
	if(!isDone) {
		isDone = extrema.Perform();
	}
	//std::cout<<"  NbSolution "<<extrema.NbSolution()<<std::endl;
	if(!isDone){
		std::cerr<<"OCCGeomRepresentation::normal("<<P1<<")\n";
		throw TkUtil::Exception("Echec d'une projection d'un point sur une surface (pour la normale)!!");
	}


	if(m_shape.ShapeType()!=TopAbs_FACE){
		std::cerr<<"OCCGeomRepresentation::normal("<<S->getName()<<")"<<std::endl;
		throw TkUtil::Exception("On ne peut calculer une normale sur autre chose qu'une FACE");
	}

	if (extrema.SupportTypeShape2(1) == BRepExtrema_IsInFace){
		//std::cout<<"  Solution sur la surface"<<std::endl;
		Standard_Real U, V;
		extrema.ParOnFaceS2(1, U, V);

		TopoDS_Face occ_f = TopoDS::Face(m_shape);
		Handle(Geom_Surface) brepSurface = BRep_Tool::Surface(occ_f);

		gp_Pnt out_pnt;
		gp_Vec du, dv;
		brepSurface->D1(U,V,out_pnt,du,dv);

		gp_Vec n=du.Crossed(dv);
		V2.setX(n.X());
		V2.setY(n.Y());
		V2.setZ(n.Z());
	}
	else if (extrema.SupportTypeShape2(1) == BRepExtrema_IsOnEdge){
		//std::cout<<"  Solution sur le bord de la surface"<<std::endl;

		V2.setX(0);
		V2.setY(0);
		V2.setZ(0);
		//throw  Utils::BadNormalException("C'est la tangente et non la normale pour OCC !");

//		TopoDS_Shape border = extrema.SupportOnShape2(1);
//		Standard_Real t;
//		extrema.ParOnEdgeS2(1,t);
//		gp_Pnt out_pnt;
//		gp_Vec du;
//		double first_local_param, last_local_param;
//		Handle(Geom_Curve) brepCurve = BRep_Tool::Curve(TopoDS::Edge(border), first_local_param, last_local_param);
//		brepCurve->D1(t, out_pnt, du);
//		V2.setX(du.X());
//		V2.setY(du.Y());
//		V2.setZ(du.Z());
	}
	else if (extrema.SupportTypeShape2(1) == BRepExtrema_IsVertex){
//		std::cout<<"OCCGeomRepresentation::normal("<<pnt.X()<<", "<<pnt.Y()<<", "<<pnt.Z()<<")"<<std::endl;
		V2.setX(0);
		V2.setY(0);
		V2.setZ(0);
		//throw  Utils::BadNormalException("Cas d'un point à une extrémité de surface, normale inconnue pour OCC");
	}
	//std::cout<<"OCCGeomRepresentation::normal("<<pnt.X()<<", "<<pnt.Y()<<", "<<pnt.Z()<<") => "<<V2<<std::endl;
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{
	TopoDS_Edge edge = TopoDS::Edge(m_shape);
	double first, last;
	Handle_Geom_Curve curv = BRep_Tool::Curve(edge, first, last);
	gp_Pnt pnt(P1.getX(),P1.getY(),P1.getZ());

    GeomAPI_ProjectPointOnCurve proj(pnt,curv, first, last);

    proj.Perform(pnt);

    // on va aussi observer les points aux extrémités
    gp_Pnt pnt_first = curv->Value(first);
    gp_Pnt pnt_last  = curv->Value(last);
    if(proj.NbPoints()!=0){
    	Standard_Real p = proj.LowerDistanceParameter();

        gp_Vec V1;
        gp_Pnt Pt;

        curv->D1(p, Pt, V1);
        V2.setXYZ(V1.X(), V1.Y(), V1.Z());
    }
    else {
    	throw  Utils::BadNormalException("Erreur interne, pas de point projeté sur la courbe pour obtenir la tangente");
    }
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::getPoint(const double& p, Utils::Math::Point& Pt,
        const bool in01)
{
    gp_Pnt res;
    if(m_shape.ShapeType()==TopAbs_EDGE){
        BRepAdaptor_Curve brepCurve(TopoDS::Edge(m_shape));
        if(in01)
        {
            double f= brepCurve.FirstParameter();
            double l= brepCurve.LastParameter();

            //p dans [0,1] a positionner dans [f,l]
            if(f<l)
                res = brepCurve.Value(f+p*(l-f));
            else
                res = brepCurve.Value(l+p*(f-l));
        }
        else
            res = brepCurve.Value(p);
    }
    else{
    	MGX_FORBIDDEN("cas d'un wire");
//        //cas du wire, on est entre 0 et 1 et on gère nous même
//        TopoDS_Wire w = TopoDS::Wire(m_shape);
//        //====================================================================
//        // Calcul de la longueur de la courbe
//        //====================================================================
//        BRepCheck_Analyzer anaAna(w, Standard_False);
//        GProp_GProps pb;
//        BRepGProp::LinearProperties(w,pb);
//        double total_length = pb.Mass();
//
//        //====================================================================
//        // on recupere toutes les courbes formant le wire
//        //====================================================================
//        TopTools_IndexedMapOfShape  mapE;
//        TopExp::MapShapes(w,TopAbs_EDGE, mapE);
//
//        //====================================================================
//        // on recupere le point de depart et de fin (qui peuvent etre
//        // confondus)
//        //====================================================================
//        std::vector<Vertex*> m3d_vertices;
//        crb->get(m3d_vertices);
//        Vertex* first_vertex  = m3d_vertices.front();
//        OCCGeomRepresentation* occ_rep =
//                dynamic_cast<OCCGeomRepresentation*>
//                (first_vertex->getComputationalProperty());
//        CHECK_NULL_PTR_ERROR(occ_rep);
//        TopoDS_Vertex v1 = TopoDS::Vertex(occ_rep->getShape());
//
//        Vertex* second_vertex = m3d_vertices.back();
//        occ_rep = dynamic_cast<OCCGeomRepresentation*>
//                (second_vertex->getComputationalProperty());
//        CHECK_NULL_PTR_ERROR(occ_rep);
//        TopoDS_Vertex v2 = TopoDS::Vertex(occ_rep->getShape());
//
//
//        //====================================================================
//        // En fonction du paramètre p, on cherche le tronçon, ou la courbe,
//        // dont le paramétrage contient p
//        //====================================================================
//        TopoDS_Vertex v_from = v1;
//        TopoDS_Vertex v_to;
//
//        // On cherche la courbe incidente a v_from et appartenant à w.
//        // Pour le moment, le cas  cyclique n'est pas géré, car on ne sait
//        // alors pas quelle courbe choisir pour démarrer?
//        if(OCCGeomRepresentation::areEquals(v1,v2))
//            throw TkUtil::Exception("Incapacité à déterminer un point à partir d'un paramètre pour une courbe composite cyclique");
//
//        TopoDS_Edge current_edge;
//        bool edge_found=false;
//        for(int i=1; i<=mapE.Extent() && !edge_found;i++)
//        {
//            TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
//            TopTools_IndexedMapOfShape  mapV;
//            TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
//            TopoDS_Vertex ce_v1, ce_v2;
//            if(mapV.Extent()>1){
//                ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
//                ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                gp_Pnt edge_pnt1 = BRep_Tool::Pnt(ce_v1);
//                gp_Pnt edge_pnt2 = BRep_Tool::Pnt(ce_v2);
//                if(OCCGeomRepresentation::areEquals(ce_v1,v_from)){
//                    edge_found = true;
//                    current_edge = ce;
//                    v_to = ce_v2;
//                }
//                else if(OCCGeomRepresentation::areEquals(ce_v2,v_from)){
//                    edge_found = true;
//                    current_edge = ce;
//                    v_to = ce_v1;
//                }
//            }
//        }
//        if(!edge_found)
//             throw TkUtil::Exception("Problème de topologie OCC (Wire,Edge,Vertex) dans OCCGeomRepresentation::getPoint");
//
//      //  std::cout<<"EDGE choisie "<<current_edge<<std::endl;
//        //ici on a la premiere courbe, le sommet de départ, le sommet suivant.
//        // On peut calculer les paramètres
//        double param_global_from, param_local_from;
//        double param_global_to  , param_local_to  ;
//        double prev_length=0;
//        BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
//        GProp_GProps pb_edge;
//        BRepGProp::LinearProperties(current_edge,pb_edge);
//        double current_length = pb_edge.Mass();
//
//        param_global_from=0; //premier sommet de la première courbe
//        //On déduit le paramètre param_global_to
//        param_global_to = (prev_length+current_length)/total_length;
//        while(p>param_global_to){
//            v_from = v_to; // on change de sommet de départ
//            //puis de courbe courante
//            bool edge_found=false;
//            prev_length+=current_length;
//            for(int i=1; i<=mapE.Extent() && !edge_found;i++)
//            {
//                TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
//                TopTools_IndexedMapOfShape  mapV;
//                TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
//                TopoDS_Vertex ce_v1, ce_v2;
//                if(mapV.Extent()>1){
//                    ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
//                    ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                    if(OCCGeomRepresentation::areEquals(ce_v1,v_from) &&
//                            ce!=current_edge)
//                    {
//                        edge_found = true;
//                        current_edge = ce;
//                        v_to = ce_v2;
//                    }
//                    else if(OCCGeomRepresentation::areEquals(ce_v2,v_from) &&
//                            ce!=current_edge)
//                    {
//                        edge_found = true;
//                        current_edge = ce;
//                        v_to = ce_v1;
//                    }
//                }
//            }
//            //prev_length, current_edge, v_from et v_to sont maintenant initialisés
//            BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
//            GProp_GProps pb_edge;
//            BRepGProp::LinearProperties(current_edge,pb_edge);
//            current_length = pb_edge.Mass();
//
//            param_global_from=prev_length/total_length;;
//            param_global_to = (prev_length+current_length)/total_length;
//        }
//        //====================================================================
//        // On est maintenant sur la bonne courbe
//        //====================================================================
//        // On récupère le paramétrage local
//        Handle(Geom_Curve) brepCurve = BRep_Tool::Curve(current_edge,
//                         param_local_from,//paramètre local à la courbe courante pour v_from
//                         param_local_to); //paramètre local à la courbe courante pour v_to
//
//        double t= (p-param_global_from)/(param_global_to-param_global_from);
//        double local_param;
//        if(param_local_from<param_local_to){
//            gp_Pnt first_curve_pnt = brepCurve->Value(param_local_from);
//            gp_Pnt from_pnt =BRep_Tool::Pnt(v_from);
//            if(from_pnt.Distance(first_curve_pnt)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
//                //courbe locale parcourue dans le même sens que la courbe composite
//                local_param = (1-t)*param_local_from + t*param_local_to;
//            else//courbe locale parcourue dans le sens inverse
//                local_param = (1-t)*param_local_to + t*param_local_from;
//        }
//        else{//sens inverse de la parametrisation OCC
//            local_param = (1-t)*param_local_to + t*param_local_from;
//        }
//        res = brepCurve->Value(local_param);
////        std::cout<<"Pour p="<<p<<" -> ("<<res.X()<<", "<<res.Y()<<", "<<res.Z()<<")"
////                <<" entre ("<<first_vertex->getX()<<", "<<first_vertex->getY()<<", "<<first_vertex->getZ()<<") et "
////                <<" ("<<second_vertex->getX()<<", "<<second_vertex->getY()<<", "<<second_vertex->getZ()<<")"
////                <<std::endl;
    }

    Pt.setXYZ(res.X(), res.Y(), res.Z());
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_INTERSECTION
void OCCGeomRepresentation::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt)
{
#ifdef _DEBUG_INTERSECTION
	std::cout<<"OCCGeomRepresentation::getIntersection ..."<<std::endl;
#endif
	BRepBuilderAPI_MakeFace mkF(plan_cut);
	TopoDS_Face wf = mkF.Face();
	Handle_Geom_Surface surf = BRep_Tool::Surface(wf);

	TopoDS_Edge edge = TopoDS::Edge(m_shape);
	double first, last;
	Handle_Geom_Curve curv = BRep_Tool::Curve(edge, first, last);

	bool isPeriodic = curv->IsPeriodic();
	Standard_Real period = 0;
	if (isPeriodic)
		period = curv->Period();

	 GeomAPI_IntCS Intersector(curv, surf);
	 uint pt_found = 0;
	 if (Intersector.IsDone()){
		 if (Intersector.NbPoints() == 1){
			 gp_Pnt pt = Intersector.Point (1);
			 for (uint j=0; j<3; j++)
				 Pt.setCoord(j,pt.Coord(j+1));
			 pt_found = 1;
		 }
		 else if (Intersector.NbPoints() > 1){
			 for (uint i=1; i<=Intersector.NbPoints(); i++){
				 gp_Pnt pt = Intersector.Point (i);
				 Quantity_Parameter U, V, W;
				 Intersector.Parameters(i, U, V, W);
#ifdef _DEBUG_INTERSECTION
				 std::cout<<" i = "<<i<<" W = "<<W<<" dans ["<<first<<" "<<last<<"] ?"<<std::endl;
				 std::cout<<"  isPeriodic = "<<isPeriodic<<", period = "<<period<<std::endl;
#endif
				 if ((W>=first && W<=last) || (isPeriodic && W+period>=first && W+period<=last)){
#ifdef _DEBUG_INTERSECTION
					 std::cout<<"  point accepté"<<std::endl;
#endif
					 for (uint j=0; j<3; j++)
						 Pt.setCoord(j,pt.Coord(j+1));
					 pt_found+=1;
				 }
			 }
		 }
	 }
	 else {
     	throw TkUtil::Exception(TkUtil::UTF8String ("Problème OCC lors de l'intersection entre courbe et un plan", TkUtil::Charset::UTF_8));
	 }
	 if (pt_found != 1) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		 message << "Problème OCC lors de l'intersection entre courbe et un plan, on obtient ";
		 message << (short)Intersector.NbPoints();
		 message <<" points dont "<<(short)pt_found<<" pourraient convenir\n";
		 for (uint i=1; i<=Intersector.NbPoints(); i++){
			 gp_Pnt pt = Intersector.Point (i);
			 message <<" i = "<<(short)i<<" "<<pt.X()<<","<<pt.Y()<<","<<pt.Z()<<"\n";
		 }
		 message<<" plan de coupe passe par point "<<plan_cut.Location().X()
				 <<", "<<plan_cut.Location().Y()
				 <<", "<<plan_cut.Location().Z()
				 <<" et a pour normale "<<plan_cut.Axis().Direction().X()
				 <<", "<<plan_cut.Axis().Direction().Y()
				 <<", "<<plan_cut.Axis().Direction().Z()
				 <<"\n";

		 throw TkUtil::Exception(message);
	 }

}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::getParameter(const Utils::Math::Point& Pt, double& p, const Curve* C)
{
    if(m_shape.ShapeType()==TopAbs_EDGE){
        getParameterOnTopoDSEdge(TopoDS::Edge(m_shape), Pt,p);
    }
    else if(m_shape.ShapeType()==TopAbs_WIRE)
    {
        //====================================================================
        // On recherche l'arête du wire sur laquelle le point P se projete car
        // le plus proche. On ne peut toutefois pas faire ce parcours dans n'
        // importe quel ordre. On va partir du point intial de la courbe C et
        // aller vers le point final en récoltant diverses informations
        // nécessaires pour la suite des calculs
        //====================================================================
        TopoDS_Wire w = TopoDS::Wire(m_shape);

        //====================================================================
        // Calcul de la longueur totale de la courbe
        //====================================================================
        BRepCheck_Analyzer anaAna(w, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(w,pb);
        double wire_length = pb.Mass();

        //====================================================================
        // on recupere toutes les courbes formant le wire
        //====================================================================
        TopTools_IndexedMapOfShape  mapE;
        TopExp::MapShapes(w,TopAbs_EDGE, mapE);

        //====================================================================
        // on recupere le point de depart et de fin (qui peuvent etre
        // confondus)
        //====================================================================
        std::vector<Vertex*> m3d_vertices;
        C->get(m3d_vertices);

        //récuperation du sommet initial
        Vertex* first_vertex  = m3d_vertices.front();
        OCCGeomRepresentation* occ_rep =
                dynamic_cast<OCCGeomRepresentation*>
                (first_vertex->getComputationalProperty());
        CHECK_NULL_PTR_ERROR(occ_rep);
        TopoDS_Vertex v1 = TopoDS::Vertex(occ_rep->getShape());

        //récuperation du sommet final
        Vertex* second_vertex = m3d_vertices.back();
        occ_rep = dynamic_cast<OCCGeomRepresentation*>
                (second_vertex->getComputationalProperty());
        CHECK_NULL_PTR_ERROR(occ_rep);
        TopoDS_Vertex v2 = TopoDS::Vertex(occ_rep->getShape());

        //====================================================================
        // On recherce maintenant l'arête sur laquelle se projète P en partant
        // de v_from.
        //====================================================================
        TopoDS_Vertex v_from = v1;
        TopoDS_Vertex v_to;
        // On cherche la courbe incidente a v_from et appartenant à w.
        // Pour le moment, le cas  cyclique n'est pas géré, car on ne sait
        // alors pas quelle courbe choisir pour démarrer?
        if(OCCGeomRepresentation::areEquals(v1,v2))
         	throw TkUtil::Exception(TkUtil::UTF8String ("Incapacité à déterminer un point à partir d'un paramètre pour une courbe composite cyclique", TkUtil::Charset::UTF_8));


        //On commence par récupérer la première arête
        TopoDS_Edge current_edge;
        bool edge_found=false;

        for(int i=1; i<=mapE.Extent() && !edge_found;i++)
        {
            TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
            TopTools_IndexedMapOfShape  mapV;
            TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
            TopoDS_Vertex ce_v1, ce_v2;
            if(mapV.Extent()>1){
                ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
                ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                gp_Pnt edge_pnt1 = BRep_Tool::Pnt(ce_v1);
//                gp_Pnt edge_pnt2 = BRep_Tool::Pnt(ce_v2);
//                std::cout<<"Courbe OCC  de ("<<edge_pnt1.X()<<", "<<edge_pnt1.Y()<<", "<<edge_pnt1.Z()<<") "
//                        <<" a ("<<edge_pnt2.X()<<", "<<edge_pnt2.Y()<<", "<<edge_pnt2.Z()<<") "
//                        <<std::endl;
                if(OCCGeomRepresentation::areEquals(ce_v1,v_from)){
//                    std::cout<<"\t POINT 1 RECHERCHE"<<std::endl;
                    edge_found = true;
                    current_edge = ce;
                    v_to = ce_v2;
                }
                else if(OCCGeomRepresentation::areEquals(ce_v2,v_from)){
 //                   std::cout<<"\t POINT 2 RECHERCHE"<<std::endl;
                    edge_found = true;
                    current_edge = ce;
                    v_to = ce_v1;
                }
            }

        }
        if(!edge_found)
        	throw TkUtil::Exception(TkUtil::UTF8String ("Problème de topologie OCC (Wire,Edge,Vertex) dans OCCGeomRepresentation::getPoint", TkUtil::Charset::UTF_8));

        //on a maintenant la courbe de départ orientée dans le sens
        //de parcours défini de v_from à v_to
        double param_global_from, param_global_to  ;
        double prev_length=0;
        BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
        GProp_GProps pb_edge;
        BRepGProp::LinearProperties(current_edge,pb_edge);
        double current_length = pb_edge.Mass();


        param_global_from=0; //premier sommet de la première courbe
        //On déduit le paramètre param_global_to
        param_global_to = (prev_length+current_length)/wire_length;

        //maintenant on parcourt tant que l'arête contenant P n'a
        //pas été trouvée
        double local_param_of_P;
        bool found_edge_containing_P=false;
        double local_param_of_P_trial=1000;
        try{
            local_param_of_P_trial =
                    getParameterOnTopoDSEdge(current_edge,Pt,local_param_of_P);
        }
        catch(TkUtil::Exception &e) {
            local_param_of_P_trial = 1000;
        }
//        std::cout<<"* from "<<param_global_from<<" to "<<param_global_to<<std::endl;

        while( local_param_of_P_trial > Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon )
        {
            v_from = v_to; // on change de sommet de départ
            //puis de courbe courante
            bool edge_found=false;

            prev_length+=current_length;
            for(int i=1; i<=mapE.Extent() && !edge_found;i++)
            {
                TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
                TopTools_IndexedMapOfShape  mapV;
                TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
                TopoDS_Vertex ce_v1, ce_v2;
                if(mapV.Extent()>1){
                    ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
                    ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                    gp_Pnt edge_pnt1 = BRep_Tool::Pnt(ce_v1);
//                    gp_Pnt edge_pnt2 = BRep_Tool::Pnt(ce_v2);
//                    std::cout<<"Courbe OCC  de ("<<edge_pnt1.X()<<", "<<edge_pnt1.Y()<<", "<<edge_pnt1.Z()<<") "
//                                         <<" a ("<<edge_pnt2.X()<<", "<<edge_pnt2.Y()<<", "<<edge_pnt2.Z()<<") "
//                                         <<std::endl;
                    if(OCCGeomRepresentation::areEquals(ce_v1,v_from) &&
                            ce!=current_edge)
                    {
//                        std::cout<<"\t POINT 1 RECHERCHE"<<std::endl;

                        edge_found = true;
                        current_edge = ce;
                        v_to = ce_v2;
                    }
                    else if(OCCGeomRepresentation::areEquals(ce_v2,v_from) &&
                            ce!=current_edge)
                    {
  //                      std::cout<<"\t POINT 2 RECHERCHE"<<std::endl;
                        edge_found = true;
                        current_edge = ce;
                        v_to = ce_v1;
                    }
                }
            }
            //prev_length, current_edge, v_from et v_to sont maintenant initialisés
            BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
            GProp_GProps pb_edge;
            BRepGProp::LinearProperties(current_edge,pb_edge);
            current_length = pb_edge.Mass();
//            std::cout<<"prev = "<<prev_length<<std::endl;
//            std::cout<<"curr = "<<current_length<<std::endl;
//            std::cout<<"total= "<<wire_length<<std::endl;
            param_global_from=prev_length/wire_length;
            param_global_to = (prev_length+current_length)/wire_length;
//            std::cout<<"- from "<<param_global_from<<" to "<<param_global_to<<std::endl;
            try{
                local_param_of_P_trial =
                        getParameterOnTopoDSEdge(current_edge,Pt,local_param_of_P);
            }
            catch(TkUtil::Exception &e) {
                local_param_of_P_trial = 1000;
            }

        }
        //====================================================================
        // on est sur l'arête contenant le point P
        // on a deja les coordonnées globales de v_from et v_to ainsi que le
        // paramètre local de P dans current_edge.
        // on récupère maintenant le paramétrage local à la courbe
        //====================================================================
        double first_local_param, last_local_param;
        //Parametre locaux de la courbe
        Handle(Geom_Curve) brepCurve = BRep_Tool::Curve(current_edge,
                first_local_param, // param. local à la courbe pour v_from
                last_local_param); // param. local à la courbe pour v_to

        //====================================================================
        double t= (local_param_of_P-first_local_param)/(last_local_param-first_local_param);
        double global_param;
        if(first_local_param<last_local_param){
            gp_Pnt first_curve_pnt = brepCurve->Value(first_local_param);
            gp_Pnt from_pnt =BRep_Tool::Pnt(v_from);
            if(from_pnt.Distance(first_curve_pnt)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
                //courbe locale parcourue dans le même sens que la courbe composite
                global_param = (1-t)*param_global_from + t*param_global_to;
            else//courbe locale parcourue dans le sens inverse
                global_param = (1-t)*param_global_to + t*param_global_from;
        }
        else{//sens inverse de la parametrisation OCC
            global_param = (1-t)*param_global_to + t*param_global_from;
        }
        p = global_param;
//        std::cout<<"Parametre : "<<p<<" pour "<<Pt<<std::endl;
//        gp_Pnt from_pnt =BRep_Tool::Pnt(v_from);
//        std::cout<<"\t entre ("<<from_pnt.X()<<", "<<from_pnt.Y()<<", "<<from_pnt.Z()<<")";
//        gp_Pnt to_pnt =BRep_Tool::Pnt(v_to);
//        std::cout<<" et ("<<to_pnt.X()<<", "<<to_pnt.Y()<<", "<<to_pnt.Z()<<")"<<std::endl;
//        std::cout<<"\t de param local ("<<first_local_param<<", "<<last_local_param<<")"<<std::endl;
//        std::cout<<"\t de param global("<<param_global_from<<", "<<param_global_to<<")"<<std::endl;
//        gp_Pnt first_curve_pnt = brepCurve->Value(first_local_param);
//        if(from_pnt.Distance(first_curve_pnt)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
//            std::cout<<"\t avec ORDRE DIRECT"<<std::endl;
//        else
//            std::cout<<"\t avec ORDRE INVERSE"<<std::endl;
//        std::cout<<"\t t = "<<t<<std::endl;
    }
    else
    	throw TkUtil::Exception(TkUtil::UTF8String ("Le paramétrage d'un point ne peut être demandé que sur une courbe!!", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_GETPARAMETER
double OCCGeomRepresentation::getParameterOnTopoDSEdge(
        const TopoDS_Edge& edge, const Utils::Math::Point& Pt, double& p)
{
    double distance = 0;
    gp_Pnt pnt(Pt.getX(),Pt.getY(),Pt.getZ());
    Standard_Real first, last;
    Handle_Geom_Curve curv = BRep_Tool::Curve(edge,first, last);
#ifdef _DEBUG_GETPARAMETER
    std::cout<<"OCCGeomRepresentation::getParameterOnTopoDSEdge pour "<<Pt<<std::endl;
    std::cout<<"first = "<<first<<std::endl;
    std::cout<<"last  = "<<last<<std::endl;
#endif

    GeomAPI_ProjectPointOnCurve proj(pnt,curv, first, last);

    proj.Perform(pnt);

    // on va aussi observer les points aux extrémités
    gp_Pnt pnt_first = curv->Value(first);
    gp_Pnt pnt_last  = curv->Value(last);
#ifdef _DEBUG_GETPARAMETER
    std::cout<<" => NbPoints = "<<proj.NbPoints()<<std::endl;
    std::cout<<"first = ("<<pnt_first.X()<<", "<<pnt_first.Y()<<", "<<pnt_first.Z()<<") pour param "<<first<<std::endl;
    std::cout<<"last = ("<<pnt_last.X()<<", "<<pnt_last.Y()<<", "<<pnt_last.Z()<<") pour param "<<last<<std::endl;
#endif
    if(proj.NbPoints()!=0){
        p = proj.LowerDistanceParameter();
        distance = proj.LowerDistance();
        gp_Pnt nearest_pt = proj.NearestPoint();

        double dist2 = pnt.Distance(pnt_first);
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"dist2 = "<<dist2<<" pour pnt_first"<<std::endl;
#endif
        if (dist2<distance){
        	p=first;
        	nearest_pt = pnt_first;
        	distance = dist2;
        }
        dist2 = pnt.Distance(pnt_last);
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"dist2 = "<<dist2<<" pour pnt_last"<<std::endl;
#endif
        if (dist2<distance){
        	p=last;
        	nearest_pt = pnt_last;
        	distance = dist2;
        }

#ifdef _DEBUG_GETPARAMETER
       for (uint i=1; i<=proj.NbPoints(); i++){
        	std::cout<<"i: "<<i<<", Parameter: "<<proj.Parameter(i)<<", Distance: "<<proj.Distance(i)<<std::endl;
        }
    	std::cout<<"retenu : distance "<<distance<<", param "<<p<<", nearest_pt "<<nearest_pt.X()<<", "<<nearest_pt.Y()<<", "<<nearest_pt.Z()<<std::endl;
#endif
    }
    else{
        distance = 0; // si sur un des points extrémités
        Utils::Math::Point pfirst(pnt_first.X(),pnt_first.Y(),pnt_first.Z());
        Utils::Math::Point plast(pnt_last.X(),pnt_last.Y(),pnt_last.Z());

        // calcul une tolérance relative à la dimension de la courbe (distance entre les 2 extrémités pour faire rapide)
        double length = (pfirst-plast).norme();
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"length = "<<length<<std::endl;
#endif
       double tol = Utils::Math::MgxNumeric::mgxParameterEpsilon;
        if (!Utils::Math::MgxNumeric::isNearlyZero(length))
        	tol*=length;

        if(Pt.isEpsilonEqual(pfirst, tol))
            p =first;
        else if(Pt.isEpsilonEqual(plast, tol))
            p =last;
        else{
        	/*std::cout<<"==================="<<std::endl;
            std::cout<<"On cherche les parametres de "<<Pt<<std::endl;
            std::cout<<"first: "<<first<<" -> "<<pfirst<<", sq. dist: "<<Pt.length2(pfirst)<<std::endl;
            std::cout<<"last : "<<first<<" -> "<<plast<<", sq. dist: "<<Pt.length2(plast)<<std::endl;
            std::cout<<"avec epsilon = "<<tol<<std::endl;
            std::cout<<"==================="<<std::endl;

            std::cout<<"OCCGeomRepresentation::getParameter() pour "<<C->getName()<<std::endl;
            std::cout<<"OCCGeomRepresentation::getParameter() pour "<<C->getName()<<" (uid "<<C->getUniqueId()<<")"<<std::endl;
            std::cout<<" que l'on trouve dans les volumes:";
            std::vector<Volume*> volumes;
            C->get(volumes);
            for (uint i=0; i<volumes.size(); i++)
            	std::cout<<" "<<volumes[i]->getName();
            std::cout<<std::endl;
            std::cout<<" que l'on trouve dans les surfaces:";
            std::vector<Surface*> surfaces;
            C->get(surfaces);
            for (uint i=0; i<surfaces.size(); i++)
            	std::cout<<" "<<surfaces[i]->getName();
            std::cout<<std::endl;
            std::cout<<" qui est référencée par topo:";
            std::vector<Topo::TopoEntity* > topos = C->getRefTopo();
            for (uint i=0; i<topos.size(); i++)
            	std::cout<<" "<<topos[i]->getName();
            std::cout<<std::endl;
            std::cout<<"Pt = ("<<pnt.X()<<", "<<pnt.Y()<<", "<<pnt.Z()<<")\n";
            std::cout<<"p = "<<p<<std::endl;
            std::cout<<"first = ("<<pnt_first.X()<<", "<<pnt_first.Y()<<", "<<pnt_first.Z()<<")\n";
            std::cout<<"last = ("<<pnt_last.X()<<", "<<pnt_last.Y()<<", "<<pnt_last.Z()<<")\n";
*/
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Echec de la récupération d'un paramétrage d'un point sur une courbe!!\n";
            message <<"===================\n";
            message<<"On cherche le parametre de "<<Pt<<"\n";
            message<<"first: "<<first<<" -> "<<pfirst<<", dist: "<<Pt.length(pfirst)<<"\n";
            message<<"last : "<<first<<" -> "<<plast<<", dist: "<<Pt.length(plast)<<"\n";
            message<<"avec epsilon = "<<tol<<"\n";
            message<<"===================\n";
            throw TkUtil::Exception(message);
        }
    }

    return distance;
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::getParameters(double& first, double& last)
{
    if(m_shape.ShapeType()==TopAbs_EDGE){
        BRepAdaptor_Curve curve_adaptor(TopoDS::Edge(m_shape));
        first = curve_adaptor.FirstParameter();
        last= curve_adaptor.LastParameter();

//        if(first!=0){
//            double step=0;
//            if(first<0){
//                step=-first; first=0; last+=step;
//            }
//            else{ //first>0
//                step=first; first=0; last-=step;
//            }
//        }
    }
    else
    { //cas du wire!!
        first = 0;
        last  = 1;
    }
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::translate(const Utils::Math::Vector& V)
{
    gp_Trsf T;
    gp_Vec v(V.getX(),V.getY(),V.getZ());
    T.SetTranslation(v);
    BRepBuilderAPI_Transform translat(T);
    //on effectue la translation
    translat.Perform(m_shape);

    if(!translat.IsDone())
        throw TkUtil::Exception("Echec d'une translation!!");

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = translat.Shape();
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::scale(const double F, const Point& center)
{
    Bnd_Box box;
    box.SetGap(Utils::Math::MgxNumeric::mgxDoubleEpsilon);
    BRepBndLib::Add(m_shape, box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);

    gp_Trsf T;
    T.SetScale(gp_Pnt(center.getX(), center.getY(), center.getZ()),F);
    BRepBuilderAPI_Transform scaling(T);

    //on effectue l'homotéthie
    scaling.Perform(m_shape);

    if(!scaling.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une homothétie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = scaling.Shape();
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::scale(const double factorX,
        const double factorY,
        const double factorZ)
{
    Bnd_Box box;
    box.SetGap(Utils::Math::MgxNumeric::mgxDoubleEpsilon);
    BRepBndLib::Add(m_shape, box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);

    gp_GTrsf T;
    T.SetValue(1,1, factorX);
    T.SetValue(2,2, factorY);
    T.SetValue(3,3, factorZ);
    BRepBuilderAPI_GTransform scaling(T);

    //on effectue l'homotéthie
    scaling.Perform(m_shape);

    if(!scaling.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une homothétie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = scaling.Shape();
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::rotate(const Utils::Math::Point& P1,
        const Utils::Math::Point& P2, double Angle)
{
    gp_Trsf T;
    gp_Pnt p1(P1.getX(),P1.getY(),P1.getZ());

    gp_Dir dir( P2.getX()-P1.getX(),
                P2.getY()-P1.getY(),
                P2.getZ()-P1.getZ());

    gp_Ax1 axis(p1,dir);
    T.SetRotation(axis,Angle);
    BRepBuilderAPI_Transform rotat(T);
    //on effectue la rotation
    rotat.Perform(m_shape);

    if(!rotat.IsDone())
        throw TkUtil::Exception("Echec d'une rotation!!");

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = rotat.Shape();
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::mirror(const Utils::Math::Plane& plane)
{
	gp_Trsf T;
    Utils::Math::Point plane_pnt = plane.getPoint();
    Utils::Math::Vector plane_vec = plane.getNormal();

    gp_Ax2 A2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ()));
    T.SetMirror (A2);
    BRepBuilderAPI_Transform trans(T);

    trans.Perform(m_shape);

    if(!trans.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une symétrie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la transformation (maj de la shape interne)
    m_shape = trans.Shape();
}
/*----------------------------------------------------------------------------*/
GeomRepresentation* OCCGeomRepresentation::clone() const
{
	if (useOCAF())
		setRootLabel(getLabel());

    GeomRepresentation* e= new OCCGeomRepresentation(*this);

    if (useOCAF())
    	setRootLabel(EntityFactory::getOCAFRootLabel());

    return e;
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::connectTopology( // [EB] OBSOLETE, cf GeomGluingImplementation::sewSurfaces
        Surface* surfM3D, TopoDS_Shape& surfOCC,
        std::vector<Vertex*>&  verticesM3D, std::vector<TopoDS_Shape>& verticesOCC,
        std::vector<Curve*>&  curvesM3D  , std::vector<TopoDS_Shape>& curvesOCC)
{

    /* on va explorer la surface OCC surfOCC */
    TopExp_Explorer e;
    if (surfOCC.ShapeType()!=TopAbs_FACE  && surfOCC.ShapeType()!=TopAbs_WIRE)
        throw TkUtil::Exception("Wrong OCC shape type!!!");

    /* on crée la connection surface->courbe et inversement */
    for(unsigned int i=0; i< curvesM3D.size();i++)
    {
        surfM3D->add(curvesM3D[i]);
        curvesM3D[i]->add(surfM3D);
    }

    connectV2C(surfOCC, verticesM3D,  verticesOCC, curvesM3D  ,  curvesOCC);

//    for(unsigned int i=0; i< curvesOCC.size();i++){
//        TopoDS_Edge e_i = TopoDS::Edge(curvesOCC[i]);
//        Curve* curve_i  = curvesM3D[i];
//        std::vector<Vertex*> curve_vertices;
//        curve_i->get(curve_vertices);
//        std::cout<<"Nb sommets pour la courbe "<<curve_i->getName()<<" = "<<curve_vertices.size()<<std::endl;
//    }
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::connectTopology( // [EB] OBSOLETE, cf GeomGluingImplementation::sewSurfaces
        Volume* volM3D, TopoDS_Shape& volOCC,
        std::vector<Vertex*>&  verticesM3D,  std::vector<TopoDS_Shape>& verticesOCC,
        std::vector<Curve*>&  curvesM3D  ,  std::vector<TopoDS_Shape>& curvesOCC,
        std::vector<Surface*>& surfacesM3D,   std::vector<TopoDS_Shape>& surfacesOCC)
{

    /* on va explorer le solide OCC volOCC */
    TopExp_Explorer e;
//    if (volOCC.ShapeType()!=TopAbs_SOLID  && volOCC.ShapeType()!=TopAbs_SHELL)
//        throw TkUtil::Exception("Wrong OCC shape type!!!");

    /* on crée la connection volume->surface et inversement */
    for(unsigned int i=0; i< surfacesM3D.size();i++){
        volM3D->add(surfacesM3D[i]);
        surfacesM3D[i]->add(volM3D);

        //face OCC correspondondante
        TopoDS_Face fi  = TopoDS::Face(surfacesOCC[i]);

        //pour la surface en question, on regarde ses courbes adjacentes

        TopTools_IndexedMapOfShape  mapE;
        TopExp::MapShapes(fi,TopAbs_EDGE, mapE);

        for(int j = 1; j <= mapE.Extent(); j++){
            TopoDS_Edge ej  = TopoDS::Edge(mapE(j));
            // on regarde si cette arete est la même qu'une autre créée avant
            // C'EST FORCEMENT LE CAS!!!!
            Curve *cj= 0;
            bool not_find_shape = true;
            for(int k =0; k<curvesOCC.size() && not_find_shape; k++)
            {
                TopoDS_Edge ek = TopoDS::Edge(curvesOCC[k]);
                if(areEquals(ej,ek)){
                    not_find_shape = false;
                    cj = curvesM3D[k];
                }
            }
            // on regarde si cette courbe est déjà dans le bord de la
            // surface
//            std::vector<Curve*> curves_SI;
//            surfacesM3D[i]->get(curves_SI);
//            bool alreadyIN = false;
//            for(unsigned int ic=0;ic<curves_SI.size();ic++){
//                if(cj==curves_SI[ic])
//                    alreadyIN=true;
//            }
            if(cj!=0){// && !alreadyIN){
                // on crée le lien S->C
                surfacesM3D[i]->add(cj);
                // on crée le lien C->S
                cj->add(surfacesM3D[i]);

            }
        }
    }

    // maintenant que les faces et les aretes sont créées, on crée les
    // sommets et on connecte
    connectV2C(volOCC, verticesM3D,  verticesOCC, curvesM3D  ,  curvesOCC);
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::connectV2C(
        TopoDS_Shape& shape,
        std::vector<Vertex*>&  verticesM3D,  std::vector<TopoDS_Shape>& verticesOCC,
        std::vector<Curve*>&  curvesM3D  ,  std::vector<TopoDS_Shape>& curvesOCC)
{
    TopTools_IndexedDataMapOfShapeListOfShape map;
    TopExp::MapShapesAndAncestors(shape, TopAbs_VERTEX, TopAbs_EDGE, map);

    // on a ainsi tous les sommets dans map et pour chaque sommet, on
    // connait les aretes auxquelles il appartient. Aretes qui sont
    // dans shape bien sûr.

    TopTools_IndexedMapOfShape map_edges;
    TopExp::MapShapes(shape,TopAbs_EDGE, map_edges);

    for(unsigned int i=0; i< curvesOCC.size();i++){
        TopoDS_Edge e_i = TopoDS::Edge(curvesOCC[i]);
        Curve* curve_i  = curvesM3D[i];

        //pour chaque courbe OCC, on recupere les sommets OCC
        TopTools_IndexedMapOfShape map_vertices;
        TopExp::MapShapes(e_i,TopAbs_VERTEX, map_vertices);
        if(map_vertices.Extent()==1){

            TopoDS_Vertex v = TopoDS::Vertex(map_vertices.FindKey(1));
            gp_Pnt p = BRep_Tool::Pnt(v);
//            std::cout<<"UN SEUL SOMMET POUR "<<curve_i->getName()
//                    <<" -> ("<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<")"<<std::endl;
        }
        for(unsigned int j=1; j <= map_vertices.Extent(); j++){
            TopoDS_Vertex vj = TopoDS::Vertex(map_vertices.FindKey(j));

            //on recherche alors l'indice du sommet OCC dans verticesOCC
            bool not_found = true;
            int index = -1;
            for(unsigned int k=0;k<verticesOCC.size() && not_found; k++){
                TopoDS_Vertex vk = TopoDS::Vertex(verticesOCC[k]);
                if(OCCGeomRepresentation::areEquals(vk,vj))
                {
                    not_found=false;
                    index = k;
                }
            }
            if(not_found)
            	throw TkUtil::Exception(TkUtil::UTF8String ("Probleme dans les connexions entre entités géométriques", TkUtil::Charset::UTF_8));


            // on connecte si ce sommet n'est pas deja incident a la courbe en question
            Vertex* vertex_k = verticesM3D[index];

            std::vector<Vertex*> curve_i_vertices;
            curve_i->get(curve_i_vertices);
            bool to_add = true;
            for(int k=0;k<curve_i_vertices.size() && to_add;k++)
            {
                if(vertex_k->getUniqueId()==curve_i_vertices[k]->getUniqueId())
                    to_add = false;
            }
            if(to_add){
                // on crée le lien C->V
                curve_i->add(vertex_k);
                // on crée le lien V->C
                vertex_k->add(curve_i);
            }
        }

    }
    //
    //    TopTools_IndexedMapOfShape map_vertices;
    //    TopExp::MapShapes(shape,TopAbs_VERTEX, map_vertices);
    //
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::connectTopology( // [EB] OBSOLETE, cf GeomGluingImplementation::sewSurfaces
        Curve* curve3D, TopoDS_Shape& curveOCC,
        std::vector<Vertex*>&  verticesM3D,
        std::vector<TopoDS_Shape>& verticesOCC)
{
    // on a une courbe M3D son homologue OCC
    // on récupère les sommets OCC incidents à la courbe OCC
//    std::cout<<"M3D vertices: ";
//    for(unsigned int i=0;i<verticesM3D.size();i++){
//        Vertex  *v = verticesM3D[i];
//        std::cout<<"-- "<<v->getName()<<" ("<<v->getX()<<", "<<v->getY()<<", "<<v->getZ()<<") --";
//    }
//    std::cout<<std::endl;
//    std::cout<<"OCC vertices: ";
//    for(unsigned int i=0;i<verticesOCC.size();i++){
//        TopoDS_Vertex v = TopoDS::Vertex(verticesOCC[i]);
//        gp_Pnt p = BRep_Tool::Pnt(v);
//
//        std::cout<<"== ("<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<") --";
//    }
//    std::cout<<std::endl;
    TopExp_Explorer e;
    for(e.Init(curveOCC, TopAbs_VERTEX); e.More(); e.Next())
    {

        TopoDS_Vertex v_occ = TopoDS::Vertex(e.Current());
        gp_Pnt p = BRep_Tool::Pnt(v_occ);

//        std::cout<<"\t Point ("<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<")"<<std::endl;
        Vertex* v_m3d=0;
        bool found_vertex = false;
        for(unsigned int i=0;i<verticesOCC.size()&&!found_vertex;i++){
            TopoDS_Vertex vi = TopoDS::Vertex(verticesOCC[i]);
            gp_Pnt pi = BRep_Tool::Pnt(vi);
//            std::cout<<"\t compare with ("<<pi.X()<<", "<<pi.Y()<<", "<<pi.Z()<<")"<<std::endl;
            if(areEquals(vi,v_occ)){
  //              std::cout<<"\t --> equal: "<<i<<std::endl;
                found_vertex=true;
                v_m3d=verticesM3D[i];
            }
//            else
//                std::cout<<"\t --> different"<<std::endl;
        }
        CHECK_NULL_PTR_ERROR(v_m3d);
//        if(!found_vertex)
//            std::cout<<"SOMMET NON TROUVE"<<std::endl;
        // on crée le lien C->V
        curve3D->add(v_m3d);
//        std::cout<<"Connexion de "<<curve3D->getName()<<" et "<<v_m3d->getName()<<")"<<std::endl;
        // on crée le lien V->C
        v_m3d->add(curve3D);
    }
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::connectTopology( // [EB] OBSOLETE, cf GeomGluingImplementation::sewSurfaces
        Volume* volM3D,
        std::vector<Vertex*>&  verticesM3D,
        std::vector<Curve*>&  curvesM3D   ,
        std::vector<Surface*>& surfacesM3D)
{
}
/*----------------------------------------------------------------------------*/
Surface* OCCGeomRepresentation::newOCCSurface(TopoDS_Face& f)
{
    return EntityFactory(m_context).newOCCSurface(f);
}
/*----------------------------------------------------------------------------*/
Vertex* OCCGeomRepresentation::newOCCVertex(TopoDS_Vertex& v)
{
    return EntityFactory(m_context).newOCCVertex(v);
}/*----------------------------------------------------------------------------*/
Curve* OCCGeomRepresentation::newOCCCurve(TopoDS_Edge& e)
{
    return EntityFactory(m_context).newOCCCurve(e);
}
/*----------------------------------------------------------------------------*/
double OCCGeomRepresentation::computeVolumeArea()
{
    Standard_Boolean onlyClosed = Standard_True;
    Standard_Boolean isUseSpan = Standard_True;
    Standard_Real aDefaultTol = 1.e-7;
    Standard_Boolean CGFlag = Standard_False;
    Standard_Boolean IFlag = Standard_False;

    BRepCheck_Analyzer anaAna(m_shape, Standard_False);

    GProp_GProps pb;
    Standard_Real local_eps =BRepGProp::VolumePropertiesGK (m_shape,
                                                            pb,
                                                            aDefaultTol,
                                                            onlyClosed,
                                                            isUseSpan,
                                                            CGFlag,
                                                            IFlag);

    double res = pb.Mass();

    return res;
}
/*----------------------------------------------------------------------------*/
double OCCGeomRepresentation::computeSurfaceArea()
{
    BRepCheck_Analyzer anaAna(m_shape, Standard_False);

    GProp_GProps pb;
    BRepGProp::SurfaceProperties(m_shape,pb);

    double res = pb.Mass();

    return res;
}
/*----------------------------------------------------------------------------*/
double OCCGeomRepresentation::computeCurveArea()
{
	// pour éviter un plantage dans LinearProperties pour le cas d'une arête dégénérée
	if(m_shape.ShapeType()==TopAbs_EDGE){
		TopoDS_Edge e = TopoDS::Edge(m_shape);
		if (BRep_Tool::Degenerated(e))
			return 0.0;
	}

    //Fonctionne aussi bien pour une topoDS_Edge qu'un TopoDS_Wire
    BRepCheck_Analyzer anaAna(m_shape, Standard_False);

    GProp_GProps pb;
    BRepGProp::LinearProperties(m_shape,pb);

    double res = pb.Mass();

    return res;
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::computeBoundingBox(Utils::Math::Point& pmin,
                                               Utils::Math::Point& pmax,
                                               double tol) const
{
    Bnd_Box box;
    box.SetGap(tol);
    BRepBndLib::Add(m_shape,box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);
    pmin.setXYZ(xmin, ymin, zmin);
    pmax.setXYZ(xmax, ymax, zmax);
}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::
buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
                           const GeomEntity* caller) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_GEOM)
        throw TkUtil::Exception("Invalid display type entity");

    TopoDS_Shape s = m_shape;
    OCCDisplayRepresentationBuilder builder(caller, s,
                dynamic_cast<Geom::GeomDisplayRepresentation*>(&dr));


    builder.execute();


}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::
getFacetedRepresentation(
        std::vector<gmds::math::Triangle >& AVec,
        const GeomEntity* caller) const
{
//    Utils::DisplayRepresentation dr = Utils::DisplayRepresentation::DISPLAY_GEOM;

    TopoDS_Shape s = m_shape;
    OCCFacetedRepresentationBuilder builder(caller, s);


    builder.execute(AVec);


}
/*----------------------------------------------------------------------------*/
void OCCGeomRepresentation::
facetedRepresentationForwardOrient(
        const GeomEntity* ACaller,
        const GeomEntity* AEntityOrientation,
        std::vector<gmds::math::Triangle>* ATri) const
{
    TopoDS_Shape callerShape = m_shape;

    OCCGeomRepresentation* occ_rep =
                      dynamic_cast<OCCGeomRepresentation*> (AEntityOrientation->getComputationalProperty());
    CHECK_NULL_PTR_ERROR(occ_rep);
    TopoDS_Shape entityOrientationShape = occ_rep->getShape();


    /* si la shape est vide, on ne fait rien */
    if(m_shape.IsNull())
        throw TkUtil::Exception ("OCCGeomRepresentation::facetedRepresentationForwardOrient "
                "shape est NULL.");

    TopExp_Explorer ex;

    for (ex.Init(callerShape, TopAbs_FACE); ex.More(); ex.Next()) {
    	if(areEquals(TopoDS::Face(ex.Current()),TopoDS::Face(entityOrientationShape))) {

//    		TopoDS_Face face = TopoDS::Face(ex.Current());
//    		TopoDS_Face face_bis = TopoDS::Face(entityOrientationShape);
//
//    		TopAbs_Orientation orient = face_bis.Orientation();
//
//    		if(orient==TopAbs_FORWARD) {
//    			std::cout<<"TopAbs_FORWARD "<<std::endl;
//    		}
//    		if(orient==TopAbs_REVERSED) {
//    			std::cout<<"TopAbs_REVERSED "<<std::endl;
//    		}
//    		if(orient==TopAbs_INTERNAL) {
//    			std::cout<<"TopAbs_INTERNAL "<<std::endl;
//    		}
//    		if(orient==TopAbs_EXTERNAL) {
//    			std::cout<<"TopAbs_EXTERNAL "<<std::endl;
//    		}
//
//    		return (orient == TopAbs_FORWARD);

//    		AEntityOrientation
//    		 fillRepresentation(TopoDS::Face(ex.Current()),AVec);
    		ATri->clear();

    		OCCFacetedRepresentationBuilder builder(ACaller,callerShape);
    		builder.execute(*ATri,entityOrientationShape);

    		return;
    	}
    }

    throw TkUtil::Exception ("OCCGeomRepresentation::facetedRepresentationForwardOrient "
            "la shape que l'on doit orienter n'a pas été trouvée.");
}
/*----------------------------------------------------------------------------*/
double OCCGeomRepresentation::
buildIncrementalBRepMesh(TopoDS_Shape& shape, const double& deflection)
{
    /* si la shape est vide, on ne fait rien */
    if(shape.IsNull())
        return 0;

    // on crée la représentation de la shape
#ifdef _DEBUG2
    Bnd_Box bounds;
    BRepBndLib::Add(shape, bounds);
    bounds.SetGap(0.0);
    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);

    Standard_Real dX = xMax - xMin;
    Standard_Real dY = yMax - yMin;
    Standard_Real dZ = zMax - zMin;

    Standard_Real deviation = 0.05;
    Standard_Real deflection_calc = (dX+dY+dZ)/300.0*deviation;
    //Standard_Real deflection = 0.01;

    /* utilisation du incrementalMesh plutot que du fastDiscret car ce dernier avait
     * des problèmes sur la discrétisation de courbes isolées */
    std::cout<<"buildIncrementalBRepMesh deflection_calc = "<<deflection_calc<<std::endl;
    std::cout<<"buildIncrementalBRepMesh deflection = "<<deflection<<std::endl;
#endif
    BRepMesh_IncrementalMesh mesher(shape, deflection, Standard_True); //,Standard_False, 0.01);

//    BRepMesh_FastDiscret::Parameters params;
//    params.Angle = 0.01;
//    params.Deflection = deflection;
//    //params.AdaptiveMin = Standard_True;
//    BRepMesh_FastDiscret mesher(bounds, params);
//    mesher.Perform(shape);

    return deflection;
}
/*----------------------------------------------------------------------------*/
double OCCGeomRepresentation::getPrecision()
{
	if(m_shape.ShapeType()==TopAbs_VERTEX){
		return BRep_Tool::Tolerance(TopoDS::Vertex(m_shape));
	}
	else if(m_shape.ShapeType()==TopAbs_EDGE){
		return BRep_Tool::Tolerance(TopoDS::Edge(m_shape));
	}
	else if(m_shape.ShapeType()==TopAbs_FACE){
		return BRep_Tool::Tolerance(TopoDS::Face(m_shape));
	}
	// [EB] pas trouvé de précision pour un volume

	return 0.0;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
