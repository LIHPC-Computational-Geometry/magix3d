#include "Geom/OCCHelper.h"
#include "Utils/MgxNumeric.h"
#include "Utils/MgxException.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <ShapeFix_FixSmallFace.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeAnalysis.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
bool OCCHelper::
areSame(const TopoDS_Shape& sh1, const TopoDS_Shape& sh2)
{
#ifdef _DEBUG2
	std::cout<<"OCCHelper::areSame(...,...)"<<std::endl;
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
bool OCCHelper::
areEquals(const TopoDS_Solid& s1, const TopoDS_Solid& s2)
{
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
bool OCCHelper::
areEquals(const TopoDS_Face& f1, const TopoDS_Face& f2)
{
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
bool OCCHelper::
areEquals(const TopoDS_Edge& e1, const TopoDS_Edge& e2)
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
bool OCCHelper::
areEquals(const TopoDS_Wire& w1, const TopoDS_Wire& w2)
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
bool OCCHelper::
areEquals(const TopoDS_Vertex& v1, const TopoDS_Vertex& v2)
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
    std::cout<<"OCCHelper::areEquals avec "<<p1<<" et "<<p2<<std::endl;
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
buildIncrementalBRepMesh(const TopoDS_Shape& shape, const double& deflection)
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
void OCCHelper::
getPoint(const TopoDS_Edge& edge, const double& p, Utils::Math::Point& Pt, const bool in01)
{
    BRepAdaptor_Curve brepCurve(edge);
    gp_Pnt res;
    if (in01) {
        double f = brepCurve.FirstParameter();
        double l = brepCurve.LastParameter();

        //p dans [0,1] a positionner dans [f,l]
        if(f<l)
            res = brepCurve.Value(f+p*(l-f));
        else
            res = brepCurve.Value(l+p*(f-l));
    } else {
        res = brepCurve.Value(p);
    }
    Pt.setXYZ(res.X(), res.Y(), res.Z());
}
/*----------------------------------------------------------------------------*/
void OCCHelper::
tangent(const TopoDS_Edge& edge, const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{
	double first, last;
	Handle_Geom_Curve curv = BRep_Tool::Curve(edge, first, last);
	gp_Pnt pnt(P1.getX(),P1.getY(),P1.getZ());

    GeomAPI_ProjectPointOnCurve proj(pnt, curv, first, last);

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
void OCCHelper::
getIntersection(const TopoDS_Edge& edge, gp_Pln& plan_cut, Utils::Math::Point& Pt)
{
#ifdef _DEBUG_INTERSECTION
	std::cout<<"OCCHelper::getIntersection ..."<<std::endl;
#endif
	BRepBuilderAPI_MakeFace mkF(plan_cut);
	TopoDS_Face wf = mkF.Face();
	Handle_Geom_Surface surf = BRep_Tool::Surface(wf);

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
//#define _DEBUG_GETPARAMETER
double OCCHelper::
getParameter(const TopoDS_Edge& edge, const Utils::Math::Point& Pt, double& p)
{
    double distance = 0;
    gp_Pnt pnt(Pt.getX(),Pt.getY(),Pt.getZ());
    Standard_Real first, last;
    Handle_Geom_Curve curv = BRep_Tool::Curve(edge, first, last);
#ifdef _DEBUG_GETPARAMETER
    std::cout<<"OCCHelper::getParameterOnTopoDSEdge pour "<<Pt<<std::endl;
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
        	p = first;
        	nearest_pt = pnt_first;
        	distance = dist2;
        }
        dist2 = pnt.Distance(pnt_last);
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"dist2 = "<<dist2<<" pour pnt_last"<<std::endl;
#endif
        if (dist2<distance){
        	p = last;
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
void OCCHelper::
getParameters(const TopoDS_Edge& edge, double& first, double& last)
{
    BRepAdaptor_Curve curve_adaptor(edge);
    first = curve_adaptor.FirstParameter();
    last= curve_adaptor.LastParameter();
}
/*----------------------------------------------------------------------------*/
bool OCCHelper::
isTypeOf(const TopoDS_Edge& edge, const Handle_Standard_Type& type)
{
    Standard_Real first_param, last_param;
    Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
    if (!curve.IsNull() && curve->DynamicType() == type)
        return true;
    else
        return false;
}
/*----------------------------------------------------------------------------*/
bool OCCHelper::
isTypeOf(const TopoDS_Face& face, const Handle_Standard_Type& type)
{
    Handle_Geom_Surface surface = BRep_Tool::Surface(face);
    if (!surface.IsNull() && surface->DynamicType() == type)
        return true;
    else
        return false;
}
/*----------------------------------------------------------------------------*/
double OCCHelper::
getLength(const TopoDS_Edge& edge)
{
    if (BRep_Tool::Degenerated(edge)) {
        return 0.0;
    } else {
        //Fonctionne aussi bien pour une topoDS_Edge qu'un TopoDS_Wire
        BRepCheck_Analyzer anaAna(edge, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(edge, pb);
        return pb.Mass();
    }
}
/*----------------------------------------------------------------------------*/
double OCCHelper::
computeArea(const TopoDS_Face& face)
{
    BRepCheck_Analyzer anaAna(face, Standard_False);
    GProp_GProps pb;
    BRepGProp::SurfaceProperties(face, pb);
    return pb.Mass();
}
/*----------------------------------------------------------------------------*/
double OCCHelper::
computeArea(const TopoDS_Shape& volume)
{
    Standard_Boolean onlyClosed = Standard_True;
    Standard_Boolean isUseSpan = Standard_True;
    Standard_Real aDefaultTol = 1.e-7;
    Standard_Boolean CGFlag = Standard_False;
    Standard_Boolean IFlag = Standard_False;

    BRepCheck_Analyzer anaAna(volume, Standard_False);

    GProp_GProps pb;
    Standard_Real local_eps = BRepGProp::VolumePropertiesGK
        (volume, pb, aDefaultTol, onlyClosed, isUseSpan, CGFlag, IFlag);

    return pb.Mass();
}
/*----------------------------------------------------------------------------*/
void OCCHelper::
computeBoundingBox(const TopoDS_Shape& shape, Utils::Math::Point& pmin, Utils::Math::Point& pmax, double tol)
{
    Bnd_Box box;
    box.SetGap(tol);
    BRepBndLib::Add(shape, box);

    double xmin, ymin, zmin, xmax, ymax, zmax;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    pmin.setXYZ(xmin, ymin, zmin);
    pmax.setXYZ(xmax, ymax, zmax);
}
/*----------------------------------------------------------------------------*/
bool OCCHelper::
contains(const TopoDS_Shape& sh,const TopoDS_Shape& shOther)
{
#ifdef _DEBUG2
    	std::cout<<" OCCHelper::contains(...,...)"<<std::endl;
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
                buildIncrementalBRepMesh(f_i, 0.1);
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
TopoDS_Shape OCCHelper::
cleanShape(TopoDS_Shape& shape){
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
        shape = sfwf->Shape();
    }

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
    return shape;
}
/*----------------------------------------------------------------------------*/
void OCCHelper::
projectPointOn(const TopoDS_Face& shape, Utils::Math::Point& P)
{
    // in gmsh ShapeFix_Wire::FixReorder is called in the surface setup step.
    // Is it mandatory when meshing in the UV parametric space
    // and not in our case ?

    double umin, umax, vmin, vmax = 0.;
    TopoDS_Face face = TopoDS::Face(shape);
    ShapeAnalysis::GetFaceUVBounds(face, umin, umax, vmin, vmax);
    Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

    GeomAPI_ProjectPointOnSurf projector;
    gp_Pnt pnt(P.getX(),P.getY(),P.getZ());
    projector.Init(pnt, surface, umin, umax, vmin, vmax);
    bool isDone = false;
    if (projector.NbPoints() > 0) {
        gp_Pnt pnt2 = projector.NearestPoint();
        P.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
        isDone = true;
    }
    //    else {
    //        std::cerr<<"OCCGeomRepresentation::project ERROR "<<P<<" on surface " <<S<<std::endl;
    //        throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");
    //    }

    // this second projection in case the first one fails might not be necessary
    if(!isDone) {
        TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
        BRepExtrema_DistShapeShape extrema(V, shape);
        bool isDone = extrema.IsDone();
        if (!isDone) {
            isDone = extrema.Perform();
        }
        if (!isDone) {
            std::cerr<<"OCCHelper::projectPointOn("<<P<<")\n";
            throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");
        }
        gp_Pnt pnt2 = extrema.PointOnShape2(1);
        P.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
    }
}
/*----------------------------------------------------------------------------*/
void OCCHelper::
projectPointOn(const TopoDS_Vertex& shape, Utils::Math::Point& P)
{
    gp_Pnt pnt = BRep_Tool::Pnt(shape);
    P.setXYZ(pnt.X(), pnt.Y(), pnt.Z());
}
/*----------------------------------------------------------------------------*/
void OCCHelper::
projectPointOn(const TopoDS_Edge& shape, Utils::Math::Point& P)
{
    gp_Pnt pnt(P.getX(),P.getY(),P.getZ());
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
    BRepExtrema_DistShapeShape extrema(V, shape);
    bool isDone = extrema.IsDone();
    if (!isDone) {
        isDone = extrema.Perform();
    }
    if (!isDone) {
        std::cerr<<"OCCHelper::projectPointOn("<<P<<")\n";
        throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");
    }
    gp_Pnt pnt2 = extrema.PointOnShape2(1);
    P.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
}
/*----------------------------------------------------------------------------*/
void OCCHelper::
addShapeToLists(TopoDS_Shape& shape,
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
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/