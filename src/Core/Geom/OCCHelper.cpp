#include "Geom/OCCHelper.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Point.h"

#include <TkUtil/Exception.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
bool OCCHelper::areEquals(const TopoDS_Face& f1,const TopoDS_Face& f2){
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
bool OCCHelper::areEquals(const TopoDS_Edge& e1, const TopoDS_Edge& e2)
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
bool OCCHelper::areEquals(const TopoDS_Wire& w1, const TopoDS_Wire& w2)
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
            if(OCCHelper::isIn(e2,e1))
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
            if(OCCHelper::isIn(e1,e2))
                isIn=true;

        }
        if(!isIn)//e2 n'est contenu dans aucune arête de e1
            return false;
    }

    return true;
}
/*----------------------------------------------------------------------------*/
bool OCCHelper::areEquals(const TopoDS_Vertex& v1,
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
bool OCCHelper::
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
double OCCHelper::
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
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/