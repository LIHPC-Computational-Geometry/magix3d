/*----------------------------------------------------------------------------*/
/** \file OCCDisplayRepresentationBuilder.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 02/12/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/OCCDisplayRepresentationBuilder.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <TkUtil/WarningLog.h>
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
// inclusion de fichiers en-tête OCC
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
#include <TColStd_Array1OfReal.hxx>
#include<TColStd_Array1OfInteger.hxx>
#include<GeomLProp_SLProps.hxx>
#include<gp_Trsf.hxx>
#include<TopTools_ListOfShape.hxx>
#include<TopTools_IndexedMapOfShape.hxx>
#include<TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include<ShapeAnalysis_ShapeContents.hxx>

#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <HatchGen_Domain.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
static Standard_Integer PlotCount = 0;
static Standard_Real IsoRatio = 1.001;
static Standard_Integer MaxPlotCount = 5;

//#define _DEBUG_ISO
/*----------------------------------------------------------------------------*/
OCCDisplayRepresentationBuilder::
OCCDisplayRepresentationBuilder(const GeomEntity* caller,
                                TopoDS_Shape& shape,
                                Geom::GeomDisplayRepresentation* rep)
:m_entity(caller),m_shape(shape), m_rep(rep)
{}
/*----------------------------------------------------------------------------*/
OCCDisplayRepresentationBuilder::~OCCDisplayRepresentationBuilder()
{}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::execute()
{
    /* si la shape est vide, on ne fait rien */
    if(m_shape.IsNull())
        return;

// [EB] retiré car perturbe les opérations booléennes
    // on crée la représentation de la shape
//    double deflection = OCCGeomRepresentation::buildIncrementalBRepMesh(m_shape);
//    m_rep->setDeflection(deflection);

    if (m_shape.ShapeType()==TopAbs_COMPSOLID ||
            m_shape.ShapeType()==TopAbs_SOLID  ||
            m_shape.ShapeType()==TopAbs_SHELL ||
            m_shape.ShapeType()==TopAbs_FACE)
        buildSurfaceRepresentation();
    else if(m_shape.ShapeType()==TopAbs_EDGE ||
            m_shape.ShapeType()==TopAbs_WIRE)
        buildCurveRepresentation();
    else
        buildVertexRepresentation();

    /* on retire la triangulation de la shape OCC */
//    BRepTools::Clean(m_shape);
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::buildVertexRepresentation()
{
    computeVertices();
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::buildCurveRepresentation()
{
    if(m_rep->hasRepresentation(Utils::DisplayRepresentation::WIRE) )
         buildCurves();
    if(m_rep->hasRepresentation(Utils::DisplayRepresentation::ISOCURVE) )
         buildIsoCurves();
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::buildSurfaceRepresentation()
{
      if(m_rep->hasRepresentation(Utils::DisplayRepresentation::WIRE) )
              buildCurves();
      if(m_rep->hasRepresentation(Utils::DisplayRepresentation::ISOCURVE) )
              buildIsoCurves();
      if(m_rep->hasRepresentation(Utils::DisplayRepresentation::SOLID) )
              buildSurfaces();
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::buildCurves()
{
	//std::cout<<"OCCDisplayRepresentationBuilder::buildCurves() pour "<<m_entity->getName()<<std::endl;

    try {

        //        // on récupère une liste indexée des arêtes composant l'objet
        //        TopTools_IndexedMapOfShape M;
        //        TopExp::MapShapes(m_shape, TopAbs_EDGE, M);
        //
        //        // On construit la map inverse edge->face
        //        TopTools_IndexedDataMapOfShapeListOfShape edge2Face;
        //        TopExp::MapShapesAndAncestors(m_shape, TopAbs_EDGE, TopAbs_FACE, edge2Face);
        //
        //        for (int i=0; i<M.Extent(); i++)
        //        {
        //            const TopoDS_Edge& aEdge = TopoDS::Edge(M(i+1));
        //        }

        computeEdges();
        /* on retire la triangulation de la shape OCC*/
    }
    catch (...){
		TkUtil::UTF8String	warningText (TkUtil::Charset::UTF_8);
        warningText<<"Impossible de créer une représentation de courbes "
                <<"pour l'entité "<<m_entity->getName();

        m_entity->log (IN_UTIL WarningLog (warningText));
    }
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::buildIsoCurves()
{
	//std::cout<<"OCCDisplayRepresentationBuilder::buildIsoCurves() pour "<<m_entity->getName()<<std::endl;
    try {
        computeIsoEdges();
    }
    catch (...){
		TkUtil::UTF8String	warningText (TkUtil::Charset::UTF_8);
        warningText<<"Impossible de créer une représentation d'iso-courbes "
                <<"pour l'entité "<<m_entity->getName();

        m_entity->log (IN_UTIL WarningLog (warningText));

    }

}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::buildSurfaces()
{
	//std::cout<<"OCCDisplayRepresentationBuilder::buildSurfaces() pour "<<m_entity->getName()<<std::endl;
    try {
        computeFaces();
    }
    catch (...){
		TkUtil::UTF8String	warningText (TkUtil::Charset::UTF_8);
        warningText<<"Impossible de créer une représentation surfacique "
                <<"pour l'entité "<<m_entity->getName();

        m_entity->log (IN_UTIL WarningLog (warningText));
    }
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::computeVertices()
{
	//std::cout<<"OCCDisplayRepresentationBuilder::computeVertices() pour "<<m_entity->getName()<<std::endl;
    // define vertices
    std::vector<Utils::Math::Point>& rep_points = m_rep->getPoints();

    TopExp_Explorer ex;
    for (ex.Init(m_shape, TopAbs_VERTEX); ex.More(); ex.Next()) {
        // get the shape
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(ex.Current());
        gp_Pnt occ_pnt = BRep_Tool::Pnt(aVertex);
        Utils::Math::Point pnt(occ_pnt.X(), occ_pnt.Y(), occ_pnt.Z());
        rep_points.push_back(pnt);
    }
}
/*----------------------------------------------------------------------------*/
bool OCCDisplayRepresentationBuilder::MoveToISO(Utils::Math::Point& pnt)
{
	// TODO Algo à corriger pour éviter les points à l'infini
#ifdef _DEBUG_ISO
	std::cout<<"OCCDisplayRepresentationBuilder::MoveToISO() pour "<<m_entity->getName()<<", Point : "<<pnt<<std::endl;
#endif
    //ajout du point
	if (pnt.norme2()<1.e38){
		std::vector<Utils::Math::Point>& rep_points = m_rep->getPoints();
		rep_points.push_back(pnt);
		return false;
	}
	else {
#ifdef _DEBUG_ISO
		std::cout<<" on ne l'ajoute pas !"<<std::endl;
#endif
		return true;
	}
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::AddToISO(Utils::Math::Point& pnt)
{
#ifdef _DEBUG_ISO
	std::cout<<"OCCDisplayRepresentationBuilder::AddToISO() pour "<<m_entity->getName()<<", Point : "<<pnt<<std::endl;
#endif

    if (pnt.norme2()<1.e38){

    	std::vector<size_t>& rep_edges= m_rep->getCurveDiscretization();
        std::vector<Utils::Math::Point>& rep_points = m_rep->getPoints();

    	//ajout du point
    	rep_points.push_back(pnt);

    	//ajout de l'arete
    	rep_edges.push_back(rep_points.size()-1);
    	rep_edges.push_back(rep_points.size()-2);
    }
#ifdef _DEBUG_ISO
	else
		std::cout<<" on ne l'ajoute pas !"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_EDGES
void OCCDisplayRepresentationBuilder::computeEdges()
{
#ifdef _DEBUG_EDGES
	std::cout<<"OCCDisplayRepresentationBuilder::computeEdges() pour "<<m_entity->getName()<<std::endl;
#endif

    std::vector<Utils::Math::Point>& rep_points = m_rep->getPoints();

    std::vector<size_t>& rep_edges= m_rep->getCurveDiscretization();

    if(m_shape.ShapeType()<TopAbs_EDGE){ // toutes les entités de dimension supérieure
        // on récupère une liste indexée des arêtes composant l'objet
        TopTools_IndexedMapOfShape M;
        TopExp::MapShapes(m_shape, TopAbs_EDGE, M);

        // On construit la map inverse edge->face
        TopTools_IndexedDataMapOfShapeListOfShape edge2Face;
        TopExp::MapShapesAndAncestors(m_shape, TopAbs_EDGE, TopAbs_FACE, edge2Face);

        for (int ie=0; ie<M.Extent(); ie++)
        {
            const TopoDS_Edge& aEdge = TopoDS::Edge(M(ie+1));

            // On récupère la transformation de la shape/face
            gp_Trsf myTransf;
            Standard_Boolean identity = true;
            TopLoc_Location aLoc;

            // On triangule l'arête
            Handle(Poly_PolygonOnTriangulation) aEdgePoly;
            Standard_Integer index = 1;
            Handle(Poly_Triangulation) T;
            BRep_Tool::PolygonOnTriangulation(aEdge, aEdgePoly, T, aLoc, index);

            Handle(Poly_Polygon3D) aPoly;
            if(aEdgePoly.IsNull()){
//                std::cout<<"-> Pas de edge poly"<<std::endl;
                aPoly = BRep_Tool::Polygon3D(aEdge, aLoc);
            }

            Standard_Integer nbNodesInEdge;

            // Si la triangulation a réussi?
            if(!aEdgePoly.IsNull()){
//                std::cout<<" cas !aEdgePoly.IsNull()"<<std::endl;
                if (!aLoc.IsIdentity()) {
  //                  std::cout<<"!aLoc.IsIdentity() pour une shape"<<std::endl;
                    identity = false;
                    myTransf = aLoc.Transformation();
    //                std::cout<<" cas !aLoc.IsIdentity()"<<std::endl;
                }

                // on récupère la triangulation de l'arête et on remplit la discretisation
                nbNodesInEdge= aEdgePoly->NbNodes();

                const TColStd_Array1OfInteger& NodeIDs = aEdgePoly->Nodes();
                const TColgp_Array1OfPnt& Nodes = T->Nodes();
                gp_Pnt V;
                for (Standard_Integer i=0;i < nbNodesInEdge;i++) {
                    V = Nodes(NodeIDs(i+1));
                    if(!identity)
                        V.Transform(myTransf);

                    Utils::Math::Point P((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
                    rep_points.push_back(P);
                    if(i!=0){
                        rep_edges.push_back(rep_points.size()-1);
                        rep_edges.push_back(rep_points.size()-2);
                    }
                }
            }
            else if (!aPoly.IsNull()) {
       //         std::cout<<" cas !aPoly.IsNull()"<<std::endl;
                if (!aLoc.IsIdentity()) {
         //           std::cout<<"!aLoc.IsIdentity() pour une shape"<<std::endl;
                    identity = false;
                    myTransf = aLoc.Transformation();
           //         std::cout<<" cas !aLoc.IsIdentity()"<<std::endl;
                }
                // on récupère la triangulation de l'arête et on remplit la discretisation
                nbNodesInEdge= aPoly->NbNodes();

                const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
             //   std::cout<<" points";
                gp_Pnt V;
                for (Standard_Integer i=0;i < nbNodesInEdge;i++) {
                    V = Nodes(i+1);
                    if(!identity)
                        V.Transform(myTransf);
                    Utils::Math::Point P((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
               //     std::cout<<" "<<P;
                    rep_points.push_back(P);
                    if(i!=0){
                        rep_edges.push_back(rep_points.size()-1);
                        rep_edges.push_back(rep_points.size()-2);
                    }
                }
            }
            else    {
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
//                std::cout<<"first, last = "<<first<<", "<<last<<std::endl;
//                std::cout<<"curve "<<curv<<std::endl;
                if (curv){
                	int nb_steps= m_rep->getNbPts()-1;
                	double step= (last-first)/nb_steps;
                	//            std::cout<<"\t step: "<<step<<std::endl;
                	for(int i =0;i<=nb_steps; i++){
                		gp_Pnt V = curv->Value(first+i*step);
                		Utils::Math::Point P((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
                		rep_points.push_back(P);
                		if(i!=0){
                			rep_edges.push_back(rep_points.size()-1);
                			rep_edges.push_back(rep_points.size()-2);
                		}
                	}
                }
                else {
					TkUtil::UTF8String	warningText (TkUtil::Charset::UTF_8);
                	warningText << "Pas de triangulation disponible pour afficher une courbe de l'entité "
                			<< m_entity->getName();
                	m_entity->log (IN_UTIL WarningLog (warningText));
                }
            }
        }

    }//(m_shape.ShapeType()==TopAbs_FACE)
    else if(m_shape.ShapeType()==TopAbs_EDGE)
    {
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

            const TColStd_Array1OfInteger& NodeIDs = aEdgePoly->Nodes();
            const TColgp_Array1OfPnt& Nodes = T->Nodes();
            gp_Pnt V;
            for (Standard_Integer i=0;i < nbNodesInEdge;i++) {
                V = Nodes(NodeIDs(i+1));
                if(!identity)
                    V.Transform(myTransf);

                Utils::Math::Point P((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
                rep_points.push_back(P);
                if(i!=0){
                    rep_edges.push_back(rep_points.size()-1);
                    rep_edges.push_back(rep_points.size()-2);
                }
            }
        }
        // Méthode supprimée [EB] car elle ne fonctionne pas correctement dans certains cas (cf pbAffichageCrb.py)
//        else if (!aPoly.IsNull())
//        {
//            Standard_Integer nbNodesInEdge;
//
//#ifdef _DEBUG_EDGES
//            std::cout<<" cas !aPoly.IsNull()"<<std::endl;
//#endif
//            if (!aLoc.IsIdentity()) {
//#ifdef _DEBUG_EDGES
//            	std::cout<<"!aLoc.IsIdentity() pour une shape"<<std::endl;
//#endif
//            	identity = false;
//            	myTransf = aLoc.Transformation();
//            }
//            // on récupère la triangulation de l'arête et on remplit la discretisation
//            nbNodesInEdge= aPoly->NbNodes();
//
//            const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
//            //   std::cout<<" points";
//            gp_Pnt V;
//            for (Standard_Integer i=0;i < nbNodesInEdge;i++) {
//            	V = Nodes(i+1);
//            	if(!identity)
//            		V.Transform(myTransf);
//            	Utils::Math::Point P((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
//            	//     std::cout<<" "<<P;
//            	rep_points.push_back(P);
//            	if(i!=0){
//            		rep_edges.push_back(rep_points.size()-1);
//            		rep_edges.push_back(rep_points.size()-2);
//            	}
//            }
//
//        }
        else {
#ifdef _DEBUG_EDGES
        	std::cout<<" cas aEdgePoly.IsNull(), utilisation de curv->Value"<<std::endl;
#endif
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
            int nb_steps= m_rep->getNbPts()-1;
#ifdef _DEBUG_EDGES
            std::cout<<"  nb_steps "<<nb_steps<<std::endl;
#endif
            double step= (last-first)/nb_steps;
//            std::cout<<"\t step: "<<step<<std::endl;
            if (!curv.IsNull())
            	for(int i =0;i<=nb_steps; i++){
            		gp_Pnt V = curv->Value(first+i*step);
            		Utils::Math::Point P((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
            		rep_points.push_back(P);
            		if(i!=0){
            			rep_edges.push_back(rep_points.size()-1);
            			rep_edges.push_back(rep_points.size()-2);
            		}
            	}

        } // end else
    }

}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::
computeIsoEdges()
{
	//std::cout<<"OCCDisplayRepresentationBuilder::computeIsoEdges() pour "<<m_entity->getName()<<std::endl;

    TopTools_IndexedMapOfShape map_faces;
    TopExp::MapShapes(m_shape,TopAbs_FACE, map_faces);
    for(int i=1; i<=map_faces.Extent();i++)
    {
        TopoDS_Face f = TopoDS::Face(map_faces(i));

        //!!! C'est le fait de verifier l'orientation qui permet de prendre
        //l'interieur de la surface a tout les coups!!!!
        f.Orientation (TopAbs_FORWARD);

        computeIsoEdges(f);
    }
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::
computeIsoEdges(const TopoDS_Face& AFace)
{
	//std::cout<<"OCCDisplayRepresentationBuilder::computeIsoEdges(AFace) pour "<<m_entity->getName()<<std::endl;
//TODO [FL] computeIsoEdges
    Standard_Real IntersectorConfusion = 1.e-10 ; // -8 ;
    Standard_Real IntersectorTangency  = 1.e-10 ; // -8 ;
    Standard_Real HatcherConfusion2d   = 1.e-8 ;
    Standard_Real HatcherConfusion3d   = 1.e-8 ;
    int NbIsos = 1;

    Geom2dHatch_Hatcher aHatcher (Geom2dHatch_Intersector (IntersectorConfusion,
                                                           IntersectorTangency),
                                  HatcherConfusion2d,
                                  HatcherConfusion3d,
                                  Standard_True,
                                  Standard_False);

    Standard_Real myInfinite,myUMin,myUMax,myVMin,myVMax;

    myInfinite = 1e38;

    Standard_Integer myNbDom;
    TColStd_Array1OfReal myUPrm(1, NbIsos),myVPrm(1, NbIsos);
    TColStd_Array1OfInteger myUInd(1, NbIsos),myVInd(1, NbIsos);

    myUInd.Init(0);
    myVInd.Init(0);

    //-----------------------------------------------------------------------
    // Les mins et max en (u,v) peuvent être infinis
    //-----------------------------------------------------------------------

    BRepTools::UVBounds (AFace, myUMin, myUMax, myVMin, myVMax) ;

    Standard_Boolean InfiniteUMin = Precision::IsNegativeInfinite (myUMin) ;
    Standard_Boolean InfiniteUMax = Precision::IsPositiveInfinite (myUMax) ;
    Standard_Boolean InfiniteVMin = Precision::IsNegativeInfinite (myVMin) ;
    Standard_Boolean InfiniteVMax = Precision::IsPositiveInfinite (myVMax) ;
    if (InfiniteUMin && InfiniteUMax)
    {
      myUMin = - myInfinite ;
      myUMax =   myInfinite ;
    }
    else if (InfiniteUMin)
    {
      myUMin = myUMax - myInfinite ;
    }
    else if (InfiniteUMax)
    {
      myUMax = myUMin + myInfinite ;
    }

    if (InfiniteVMin && InfiniteVMax)
    {
      myVMin = - myInfinite ;
      myVMax =   myInfinite ;
    }
    else if (InfiniteVMin)
    {
      myVMin = myVMax - myInfinite ;
    }
    else if (InfiniteVMax)
    {
      myVMax = myVMin + myInfinite ;
    }
    //-----------------------------------------------------------------------
    // On retrouve les aretes et on les passe au Hatcher
    //-----------------------------------------------------------------------

    TopExp_Explorer ExpEdges ;
    for (ExpEdges.Init (AFace, TopAbs_EDGE); ExpEdges.More();  ExpEdges.Next())
    {
        const TopoDS_Edge& TopologicalEdge = TopoDS::Edge (ExpEdges.Current()) ;
        Standard_Real U1, U2 ;
        const Handle(Geom2d_Curve) PCurve =
                BRep_Tool::CurveOnSurface (TopologicalEdge, AFace, U1, U2) ;

        if ( PCurve.IsNull() )
            return; //TODO [FL] Un peu fort non?

        if ( U1==U2)
            return; //TODO [FL] Un peu fort non?

        //-- Test if a TrimmedCurve is necessary
        if( Abs(PCurve->FirstParameter()-U1)<= Precision::PConfusion() &&
                Abs(PCurve->LastParameter()-U2)<= Precision::PConfusion())
        {
            aHatcher.AddElement (PCurve, TopologicalEdge.Orientation()) ;
      }
      else
      {
          if (!PCurve->IsPeriodic())
          {
              Handle (Geom2d_TrimmedCurve) TrimPCurve = Handle(Geom2d_TrimmedCurve)::DownCast (PCurve);
              if (!TrimPCurve.IsNull()){
                  if (TrimPCurve->BasisCurve()->FirstParameter() - U1 > Precision::PConfusion() ||
                          TrimPCurve->BasisCurve()->FirstParameter() - U2 > Precision::PConfusion() ||
                          U1 - TrimPCurve->BasisCurve()->LastParameter()  > Precision::PConfusion() ||
                          U2 - TrimPCurve->BasisCurve()->LastParameter()  > Precision::PConfusion()){
                      aHatcher.AddElement (PCurve, TopologicalEdge.Orientation());
                      return;
                  }
              }
              else
              {
                  if (PCurve->FirstParameter() - U1 > Precision::PConfusion()){
                      U1 = PCurve->FirstParameter();
                  }
                  if (PCurve->FirstParameter() - U2 > Precision::PConfusion()){
                      U2 = PCurve->FirstParameter();
                  }
                  if (U1 - PCurve->LastParameter() > Precision::PConfusion()){
                      U1 = PCurve->LastParameter();
                  }
                  if (U2 - PCurve->LastParameter() > Precision::PConfusion()){
                      U2 = PCurve->LastParameter();
                  }
              }
          }

          // if U1 and U2 coincide-->do nothing
          if (Abs (U1 - U2) <= Precision::PConfusion())
              continue;

          Handle (Geom2d_TrimmedCurve) TrimPCurve = new Geom2d_TrimmedCurve (PCurve, U1, U2) ;
          aHatcher.AddElement (TrimPCurve, TopologicalEdge.Orientation()) ;
      }
    }


    //-----------------------------------------------------------------------
    // Loading and trimming the hatchings.
    //-----------------------------------------------------------------------

    Standard_Integer IIso ;
    Standard_Real DeltaU = Abs (myUMax - myUMin) ;
    Standard_Real DeltaV = Abs (myVMax - myVMin) ;
    Standard_Real confusion = Min (DeltaU, DeltaV) * HatcherConfusion3d ;
    aHatcher.Confusion3d (confusion) ;

    Standard_Real StepU = DeltaU / (Standard_Real) NbIsos ;
    if (StepU > confusion)
    {
      Standard_Real UPrm = myUMin + StepU / 2. ;
      gp_Dir2d Dir (0., 1.) ;
      for (IIso = 1 ; IIso <= NbIsos ; IIso++)
      {
        myUPrm(IIso) = UPrm ;
        gp_Pnt2d Ori (UPrm, 0.) ;
        Geom2dAdaptor_Curve HCur (new Geom2d_Line (Ori, Dir)) ;
        myUInd(IIso) = aHatcher.AddHatching (HCur) ;
        UPrm += StepU ;
      }
    }

    Standard_Real StepV = DeltaV / (Standard_Real) NbIsos ;
    if (StepV > confusion)
    {
      Standard_Real VPrm = myVMin + StepV / 2. ;
      gp_Dir2d Dir (1., 0.) ;
      for (IIso = 1 ; IIso <= NbIsos ; IIso++)
      {
        myVPrm(IIso) = VPrm ;
        gp_Pnt2d Ori (0., VPrm) ;
        Geom2dAdaptor_Curve HCur (new Geom2d_Line (Ori, Dir)) ;
        myVInd(IIso) = aHatcher.AddHatching (HCur) ;
        VPrm += StepV ;
      }
    }

    //-----------------------------------------------------------------------
    // Calcul.
    //-----------------------------------------------------------------------

    aHatcher.Trim() ;

    myNbDom = 0 ;
    for (IIso = 1 ; IIso <= NbIsos ; IIso++)
    {
      Standard_Integer Index ;

      Index = myUInd(IIso) ;
      if (Index != 0)
      {
        if (aHatcher.TrimDone (Index) && !aHatcher.TrimFailed (Index))
        {
          aHatcher.ComputeDomains (Index);
          if (aHatcher.IsDone (Index))
            myNbDom = myNbDom + aHatcher.NbDomains (Index) ;
        }
      }

      Index = myVInd(IIso) ;
      if (Index != 0)
      {
        if (aHatcher.TrimDone (Index) && !aHatcher.TrimFailed (Index))
        {
          aHatcher.ComputeDomains (Index);
          if (aHatcher.IsDone (Index))
              myNbDom = myNbDom + aHatcher.NbDomains (Index) ;
        }
      }
    }

    //-----------------------------------------------------------------------
    // Maintenant au crée les isolines pour vtk
    //-----------------------------------------------------------------------


    Standard_Integer pt_start_idx = 0;

    for (Standard_Integer UIso = myUPrm.Lower() ; UIso <= myUPrm.Upper() ; UIso++)
    {
      Standard_Integer UInd = myUInd.Value (UIso) ;
      if (UInd != 0)
      {
        Standard_Real UPrm = myUPrm.Value (UIso) ;
        if (!aHatcher.IsDone (UInd))
        {
          std::cout << "Calcul de isoline :: iso U de paramètre " << UPrm;
          switch (aHatcher.Status (UInd))
          {
            case HatchGen_NoProblem:
            {
                std::cerr << " Aucun problème" << std::endl;
              break;
            }
            case HatchGen_TrimFailure:
            {
                std::cerr << " Trim Echec" << std::endl;
              break;
            }
            case HatchGen_TransitionFailure:
            {
                std::cerr << " Transition Echec" << std::endl;
              break;
            }
            case HatchGen_IncoherentParity:
            {
                std::cerr << " Problème de parité" << std::endl;
              break;
            }
            case HatchGen_IncompatibleStates:
            {
                std::cerr << " Etats incompatibles " << std::endl;
              break;
            }
          }
        }
        else
        {
#ifdef _DEBUG_ISO
        	std::cout<<"DrawIso GeomAbs_IsoU"<<std::endl;
#endif
          Standard_Integer NbDom = aHatcher.NbDomains (UInd) ;
          for (Standard_Integer IDom = 1 ; IDom <= NbDom ; IDom++)
          {
            const HatchGen_Domain& Dom = aHatcher.Domain (UInd, IDom) ;
            Standard_Real V1 = Dom.HasFirstPoint()  ? Dom.FirstPoint().Parameter()  : myVMin - myInfinite ;
            Standard_Real V2 = Dom.HasSecondPoint() ? Dom.SecondPoint().Parameter() : myVMax + myInfinite ;
            DrawIso(AFace,GeomAbs_IsoU, UPrm, V1, V2, pt_start_idx);
          }
        }
      }
    }

    for (Standard_Integer VIso = myVPrm.Lower() ; VIso <= myVPrm.Upper() ; VIso++)
    {
      Standard_Integer VInd = myVInd.Value (VIso) ;
      if (VInd != 0)
      {
        Standard_Real VPrm = myVPrm.Value (VIso) ;
        if (!aHatcher.IsDone (VInd))
        {
          cout << "DBRep_IsoBuilder:: V iso of parameter: " << VPrm;
          switch (aHatcher.Status (VInd))
          {
            case HatchGen_NoProblem:
            {
                cerr << " No Problem" << endl;
              break;
            }
            case HatchGen_TrimFailure:
            {
                cerr << " Trim Failure" << endl;
              break;
            }
            case HatchGen_TransitionFailure:
            {
                cerr << " Transition Failure" << endl;
              break;
            }
            case HatchGen_IncoherentParity:
            {
                cerr << " Incoherent Parity" << endl;
              break;
            }
            case HatchGen_IncompatibleStates:
            {
                cerr << " Incompatible States" << endl;
              break;
            }
          }
        }
        else
        {
#ifdef _DEBUG_ISO
        	std::cout<<"DrawIso GeomAbs_IsoU"<<std::endl;
#endif
        	Standard_Integer NbDom = aHatcher.NbDomains (VInd) ;
        	for (Standard_Integer IDom = 1 ; IDom <= NbDom ; IDom++)
        	{
        		const HatchGen_Domain& Dom = aHatcher.Domain (VInd, IDom) ;
        		Standard_Real U1 = Dom.HasFirstPoint()  ? Dom.FirstPoint().Parameter()  : myVMin - myInfinite ;
        		Standard_Real U2 = Dom.HasSecondPoint() ? Dom.SecondPoint().Parameter() : myVMax + myInfinite ;
        		DrawIso(AFace,GeomAbs_IsoV, VPrm, U1, U2,pt_start_idx) ;
        	}
        }
      }
    }
}

/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::DrawIso(
        const TopoDS_Face& AFace,
        GeomAbs_IsoType T,
        Standard_Real Par, Standard_Real T1, Standard_Real T2,
        Standard_Integer& startidx)
{
	//std::cout<<"OCCDisplayRepresentationBuilder::DrawIso(...) pour "<<m_entity->getName()<<std::endl;
    int discretiso = 30;
    Standard_Boolean halt = Standard_False;
    Standard_Integer j,myDiscret = discretiso;
    Standard_Real U1,U2,V1,V2,stepU=0.,stepV=0.;
    gp_Pnt P;
    TopLoc_Location l;

    const Handle(Geom_Surface)& S = BRep_Tool::Surface(TopoDS::Face(AFace),l);
    if (!S.IsNull())
    {
        BRepAdaptor_Surface S(TopoDS::Face(AFace),Standard_False);

        GeomAbs_SurfaceType SurfType = S.GetType();

        GeomAbs_CurveType CurvType = GeomAbs_OtherCurve;

        Standard_Integer Intrv, nbIntv;
        Standard_Integer nbUIntv = S.NbUIntervals(GeomAbs_CN);
        Standard_Integer nbVIntv = S.NbVIntervals(GeomAbs_CN);
        TColStd_Array1OfReal TI(1,Max(nbUIntv, nbVIntv)+1);


        if (T == GeomAbs_IsoU)
        {
            S.VIntervals(TI, GeomAbs_CN);
            V1 = Max(T1, TI(1));
            V2 = Min(T2, TI(2));
            U1 = Par;
            U2 = Par;
            stepU = 0;
            nbIntv = nbVIntv;
        }
        else
        {
            S.UIntervals(TI, GeomAbs_CN);
            U1 = Max(T1, TI(1));
            U2 = Min(T2, TI(2));
            V1 = Par;
            V2 = Par;
            stepV = 0;
            nbIntv = nbUIntv;
        }

        S.D0(U1,V1,P);
        Utils::Math::Point p(P.X(),P.Y(),P.Z());
        if (MoveToISO(p))
        	return;

        for (Intrv = 1; Intrv <= nbIntv; Intrv++)
        {

            if (TI(Intrv) <= T1 && TI(Intrv + 1) <= T1)
                continue;
            if (TI(Intrv) >= T2 && TI(Intrv + 1) >= T2)
                continue;
            if (T == GeomAbs_IsoU)
            {
                V1 = Max(T1, TI(Intrv));
                V2 = Min(T2, TI(Intrv + 1));
                stepV = (V2 - V1) / myDiscret;
            }
            else
            {
                U1 = Max(T1, TI(Intrv));
                U2 = Min(T2, TI(Intrv + 1));
                stepU = (U2 - U1) / myDiscret;
            }

            switch (SurfType) {
            //-------------GeomAbs_Plane---------------
            case GeomAbs_Plane :
                break;
                //----GeomAbs_Cylinder   GeomAbs_Cone------
            case GeomAbs_Cylinder :
            case GeomAbs_Cone :
                if (T == GeomAbs_IsoV)
                {
#ifdef _DEBUG_ISO
                	std::cout<<"GeomAbs_Cone ou GeomAbs_Cylinder"<<std::endl;
#endif
                    for (j = 1; j < myDiscret; j++)
                    {
                        U1 += stepU;
                        V1 += stepV;
                        S.D0(U1,V1,P);
                        Utils::Math::Point p(P.X(),P.Y(),P.Z());
                        AddToISO(p);
                    }
                }
                break;
                //---GeomAbs_Sphere   GeomAbs_Torus--------
                //GeomAbs_BezierSurface GeomAbs_BezierSurface
            case GeomAbs_Sphere :
            case GeomAbs_Torus :
            case GeomAbs_OffsetSurface :
            case GeomAbs_OtherSurface :
            {
#ifdef _DEBUG_ISO
            	std::cout<<"GeomAbs_Sphere, GeomAbs_Torus, GeomAbs_OffsetSurface ou GeomAbs_OtherSurface"<<std::endl;
#endif
                for (j = 1; j < myDiscret; j++)
                {
                    U1 += stepU;
                    V1 += stepV;
                    S.D0(U1,V1,P);
                    Utils::Math::Point p(P.X(),P.Y(),P.Z());
                    AddToISO(p);
                }
            }
                break;
                //-------------GeomAbs_BSplineSurface------
            case GeomAbs_BezierSurface :
            case GeomAbs_BSplineSurface :
                for (j = 1; j <= myDiscret/2; j++)
                {
                    PlotCount=0;
                    PlotIso ( S, T, U1, V1, (T == GeomAbs_IsoV) ? stepU*2. : stepV*2., halt);
                    U1 += stepU*2.;
                    V1 += stepV*2.;
                }
                break;
                //-------------GeomAbs_SurfaceOfExtrusion--
                //-------------GeomAbs_SurfaceOfRevolution-
            case GeomAbs_SurfaceOfExtrusion :
            case GeomAbs_SurfaceOfRevolution :
                if ((T == GeomAbs_IsoV && SurfType == GeomAbs_SurfaceOfRevolution) ||
                        (T == GeomAbs_IsoU && SurfType == GeomAbs_SurfaceOfExtrusion))
                {
                    if (SurfType == GeomAbs_SurfaceOfExtrusion)
                        break;
#ifdef _DEBUG_ISO
                    std::cout<<"GeomAbs_SurfaceOfExtrusion ou GeomAbs_SurfaceOfRevolution, GeomAbs_IsoV ou GeomAbs_IsoU"<<std::endl;
#endif
                    for (j = 1; j < myDiscret; j++)
                    {
                        U1 += stepU;
                        V1 += stepV;
                        S.D0(U1,V1,P);
                        Utils::Math::Point p(P.X(),P.Y(),P.Z());
                        AddToISO(p);
                    }
                }
                else
                {
#ifdef _DEBUG_ISO
                    std::cout<<"GeomAbs_SurfaceOfExtrusion ou GeomAbs_SurfaceOfRevolution, ni GeomAbs_IsoV ni GeomAbs_IsoU"<<std::endl;
#endif
                    CurvType = (S.BasisCurve())->GetType();
                    switch (CurvType) {
                    case GeomAbs_Line :
                        break;
                    case GeomAbs_Circle :
                    case GeomAbs_Ellipse :
                        for (j = 1; j < myDiscret; j++)
                        {
                            U1 += stepU;
                            V1 += stepV;
                            S.D0(U1,V1,P);
                            Utils::Math::Point p(P.X(),P.Y(),P.Z());
                            AddToISO(p);
                        }
                        break;
                    case GeomAbs_Parabola :
                    case GeomAbs_Hyperbola :
                    case GeomAbs_BezierCurve :
                    case GeomAbs_BSplineCurve :
                    case GeomAbs_OtherCurve :
                        for (j = 1; j <= myDiscret/2; j++)
                        {
                            PlotCount=0;
                            PlotIso ( S, T, U1, V1,(T == GeomAbs_IsoV) ? stepU*2. : stepV*2., halt);
                            U1 += stepU*2.;
                            V1 += stepV*2.;
                        }
                        break;
                    }
                }
            }
        }
#ifdef _DEBUG_ISO
        std::cout<<"pour tous"<<std::endl;
#endif
        S.D0(U2,V2,P);
        Utils::Math::Point pnt(P.X(),P.Y(),P.Z());
        AddToISO(pnt);
    }
}
/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::PlotIso (BRepAdaptor_Surface& S,
        GeomAbs_IsoType T,
        Standard_Real& U,
        Standard_Real& V,
        Standard_Real Step,
        Standard_Boolean& halt)
{
#ifdef _DEBUG_ISO
	std::cout<<"OCCDisplayRepresentationBuilder::PlotIso(...) pour "<<m_entity->getName()<<std::endl;
#endif
    ++PlotCount;

    gp_Pnt Pl, Pr, Pm;

    if (T == GeomAbs_IsoU)
    {
        S.D0(U, V, Pl);
        S.D0(U, V + Step/2., Pm);
        S.D0(U, V + Step, Pr);
    }
    else
    {
        S.D0(U, V, Pl);
        S.D0(U + Step/2., V, Pm);
        S.D0(U + Step, V, Pr);
    }

    if (PlotCount > MaxPlotCount)
    {
        Utils::Math::Point p(Pr.X(),Pr.Y(),Pr.Z());
        AddToISO(p);
        return;
    }

    if (Pm.Distance(Pl) + Pm.Distance(Pr) <= IsoRatio*Pl.Distance(Pr))
    {
        Utils::Math::Point p(Pr.X(),Pr.Y(),Pr.Z());
        AddToISO(p);
    }
    else if (T == GeomAbs_IsoU)
    {
        PlotIso ( S, T, U, V, Step/2, halt);
        Standard_Real aLocalV = V + Step/2 ;
        PlotIso ( S, T, U, aLocalV , Step/2, halt);
    }
    else
    {
        PlotIso ( S, T, U, V, Step/2, halt);
        Standard_Real aLocalU = U + Step/2 ;
        PlotIso ( S, T, aLocalU , V, Step/2, halt);
    }
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_FACES
void OCCDisplayRepresentationBuilder::computeFaces()
{
#ifdef _DEBUG_FACES
	std::cout<<"OCCDisplayRepresentationBuilder::computeFaces() pour "<<m_entity->getName()<<std::endl;
#endif
    TopExp_Explorer ex;
    int i = 1;
    if(m_shape.ShapeType()==TopAbs_FACE){
        fillRepresentation(TopoDS::Face(m_shape));
    }
    else{
        for (ex.Init(m_shape, TopAbs_FACE); ex.More(); ex.Next(),i++) {
            // on récupère la face et on la maille
            fillRepresentation(TopoDS::Face(ex.Current()));

        } // for (ex.Init(m_shape, TopAbs_FACE); ex.More(); ex.Next(),i++)
    }
}

/*----------------------------------------------------------------------------*/
void OCCDisplayRepresentationBuilder::fillRepresentation(
        const TopoDS_Face& aFace)
{
#ifdef _DEBUG_FACES
	std::cout<<"OCCDisplayRepresentationBuilder::fillRepresentation(aFace) pour "<<m_entity->getName()<<std::endl;
#endif

    std::vector<Utils::Math::Point>& rep_points = m_rep->getPoints();
    std::vector<size_t>& rep_triangles= m_rep->getSurfaceDiscretization();

    TopLoc_Location aLoc;
//    BRepTools::Clean(aFace);
    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if (aPoly.IsNull()){
#ifdef _DEBUG_FACES
    	std::cout<<"cas avec aPoly.IsNull()..."<<std::endl;
#endif
        TopoDS_Face sh = aFace;
        OCCGeomRepresentation::cleanShape(sh);
        BRepTools::Clean(sh);
        OCCGeomRepresentation::buildIncrementalBRepMesh(sh, m_rep->getDeflection());
        aPoly = BRep_Tool::Triangulation(sh,aLoc);
        if (aPoly.IsNull()){
			TkUtil::UTF8String	warningText (TkUtil::Charset::UTF_8);
            warningText << "Triangulation vide pour une face  de l'entité "
                    << m_entity->getName();
            m_entity->log (IN_UTIL WarningLog (warningText));
            return;
        }
    }



    // On récupère la transformation de la shape/face
    gp_Trsf myTransf;
    Standard_Boolean identity = true;
    if (!aLoc.IsIdentity()) {
        identity = false;
        myTransf = aLoc.Transformation();
    }

    int i;
    int nbNodesInFace = aPoly->NbNodes();
    int nbTriInFace = aPoly->NbTriangles();

#ifdef _DEBUG_FACES
    std::cout<<"nbNodesInFace = "<<nbNodesInFace<<std::endl;
    std::cout<<"nbTriInFace = "<<nbTriInFace<<std::endl;
#endif


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
        Utils::Math::Point P1((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
        Utils::Math::Point P2((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
        Utils::Math::Point P3((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));
        rep_points.push_back(P1);
        rep_points.push_back(P2);
        rep_points.push_back(P3);
        int nb_points = rep_points.size();
        rep_triangles.push_back(nb_points-3);
        rep_triangles.push_back(nb_points-2);
        rep_triangles.push_back(nb_points-1);
    } // for (i=1;i<=nbTriInFace;i++)

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

