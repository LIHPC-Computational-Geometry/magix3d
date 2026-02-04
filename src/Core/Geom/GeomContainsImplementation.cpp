/*----------------------------------------------------------------------------*/
#include "Geom/GeomContainsImplementation.h"
#include "Geom/GeomProjectImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCHelper.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include <TopTools_IndexedMapOfShape.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Poly_Triangulation.hxx>
#include <Geom_Surface.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepBndLib.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
bool GeomContainsImplementation::
contains(Curve* c1, Curve* c2) const
{
    double c1_bounds[6];
    double c2_bounds[6];

    c1->getBounds(c1_bounds);
    c2->getBounds(c2_bounds);
    // Pourquoi prendre une aussi grande tolérance ?
    double local_tolX = 0.5*(c2_bounds[1]-c2_bounds[0]);
    double local_tolY = 0.5*(c2_bounds[3]-c2_bounds[2]);
    double local_tolZ = 0.5*(c2_bounds[5]-c2_bounds[4]);
    if(    ( c2_bounds[0] < c1_bounds[0] - local_tolX)||//minX
            ( c2_bounds[1] > c1_bounds[1] + local_tolX)||//maxX
            ( c2_bounds[2] < c1_bounds[2] - local_tolY)||//minY
            ( c2_bounds[3] > c1_bounds[3] + local_tolY)||//maxY
            ( c2_bounds[4] < c1_bounds[4] - local_tolZ)||//minZ
            ( c2_bounds[5] > c1_bounds[5] + local_tolZ) )//maxZ
    {
        return false;
    }

    // détermine un ordre de dimension pour la courbe à partir de sa boite englobante
    double length = 0.0;
    for (uint i=0; i<3; i++)
    	if (length<c1_bounds[i*2+1]-c1_bounds[i*2])
    		length=c1_bounds[i*2+1]-c1_bounds[i*2];

    //std::cout<<" length pour "<<getName()<<" : "<<length<<std::endl;

    double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*length;

    //===============================================================
    // EN FAIT IL FAUT TRAVAILLER AVEC LA SHAPES OCC, CAR LA SHAPE
    // TESTEE PEUT NE PAS ENCORE ETRE CONNECTEE TOPOLOGIQUEMENT
    // AVEC DES ENTITES M3D
    //===============================================================
	GeomProjectImplementation gpi;
	for (TopoDS_Shape shOther : c2->getOCCEdges())
	{
		//===============================================================
		// on teste les sommets
		//===============================================================
		TopTools_IndexedMapOfShape map_vertices;
		TopExp::MapShapes(shOther, TopAbs_VERTEX, map_vertices);
		for(unsigned int i=1; i<=map_vertices.Extent();i++)
		{
			TopoDS_Shape s_i = map_vertices.FindKey(i);
			TopoDS_Vertex v_i = TopoDS::Vertex(s_i);
			gp_Pnt p_i = BRep_Tool::Pnt(v_i);

			Utils::Math::Point pi(p_i.X(),p_i.Y(),p_i.Z());
			Utils::Math::Point proj_i = gpi.project(c1, pi).first;
			if(!Utils::Math::MgxNumeric::isNearlyZero(pi.length(proj_i),tol))
				return false;
		}

		//===============================================================
		// On teste les courbes (1er point au 1/3, 2eme au 2/3)
		//===============================================================
		TopTools_IndexedMapOfShape map_edges;
		TopExp::MapShapes(shOther, TopAbs_EDGE, map_edges);
		for(unsigned int i=1; i<=map_edges.Extent();i++)
		{
			TopoDS_Shape s_i = map_edges.FindKey(i);
			TopoDS_Edge e_i = TopoDS::Edge(s_i);
			BRepAdaptor_Curve curve_adaptor(e_i);
			double umin = curve_adaptor.FirstParameter();
			double umax = curve_adaptor.LastParameter();

			double u1 = umin+(umax-umin)/3.0;

			gp_Pnt pnt1 = curve_adaptor.Value(u1);
			Utils::Math::Point pi1(pnt1.X(),pnt1.Y(),pnt1.Z());

			Utils::Math::Point proj_i1 = gpi.project(c1, pi1).first;
			if(!Utils::Math::MgxNumeric::isNearlyZero(pi1.length(proj_i1),tol))
			{
				//            std::cout<<"Point  :"<<pi1<<std::endl;
				return false;
			}

			double u2 = umin+(umax-umin)*2.0/3.0;

			gp_Pnt pnt2 = curve_adaptor.Value(u2);
			Utils::Math::Point pi2(pnt2.X(),pnt2.Y(),pnt2.Z());

			Utils::Math::Point proj_i2 = gpi.project(c1, pi2).first;
			if(!Utils::Math::MgxNumeric::isNearlyZero(pi2.length(proj_i2),tol))
			{
				//            std::cout<<"Point  :"<<pi2<<std::endl;
				return false;
			}
		}
	}
    return true;
}
/*----------------------------------------------------------------------------*/
bool GeomContainsImplementation::
contains(Surface* s1, Surface* s2) const
{
    double s1_bounds[6];
    double s2_bounds[6];

    s1->getBounds(s1_bounds);
    s2->getBounds(s2_bounds);
    // Pourquoi prendre une aussi grande tolérance ?
    double local_tolX = 0.5*(s2_bounds[1]-s2_bounds[0]);
    double local_tolY = 0.5*(s2_bounds[3]-s2_bounds[2]);
    double local_tolZ = 0.5*(s2_bounds[5]-s2_bounds[4]);
    if(    ( s2_bounds[0] < s1_bounds[0] - local_tolX)||//minX
            ( s2_bounds[1] > s1_bounds[1] + local_tolX)||//maxX
            ( s2_bounds[2] < s1_bounds[2] - local_tolY)||//minY
            ( s2_bounds[3] > s1_bounds[3] + local_tolY)||//maxY
            ( s2_bounds[4] < s1_bounds[4] - local_tolZ)||//minZ
            ( s2_bounds[5] > s1_bounds[5] + local_tolZ) )//maxZ
    {
        return false;
    }

    // détermine un ordre de dimension pour la surface à partir de sa boite englobante
    double length = 0.0;
    for (uint i=0; i<3; i++)
    	if (length<s1_bounds[i*2+1]-s1_bounds[i*2])
    		length=s1_bounds[i*2+1]-s1_bounds[i*2];

    //std::cout<<" length pour "<<getName()<<" : "<<length<<std::endl;

    double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*length;

    if(s1->getArea() < s2->getArea())
        return false;

        //===============================================================
    // EN FAIT IL FAUT TRAVAILLER AVEC LA SHAPES OCC, CAR LA SHAPE
    // TESTEE PEUT NE PAS ENCORE ETRE CONNECTEE TOPOLOGIQUEMENT
    // AVEC DES ENTITES M3D
    //===============================================================
	GeomProjectImplementation gpi;
	for (auto shOther : s2->getOCCFaces()) {
    	//===============================================================
    	// on teste les sommets
    	//===============================================================
    	TopTools_IndexedMapOfShape map_vertices;
    	TopExp::MapShapes(shOther,TopAbs_VERTEX, map_vertices);
    	for(unsigned int i=1; i<=map_vertices.Extent();i++){
    		TopoDS_Shape s_i = map_vertices.FindKey(i);
    		TopoDS_Vertex v_i = TopoDS::Vertex(s_i);
    		gp_Pnt p_i = BRep_Tool::Pnt(v_i);
    		Utils::Math::Point pi(p_i.X(),p_i.Y(),p_i.Z());
    		Utils::Math::Point proj_i = gpi.project(s1, pi).first;
    		if(!Utils::Math::MgxNumeric::isNearlyZero(pi.length(proj_i),tol))
    			return false;

    	}
    	//===============================================================
    	// On teste les courbes (1er point au 1/3, 2eme au 2/3)
    	//===============================================================
    	TopTools_IndexedMapOfShape map_edges;
    	TopExp::MapShapes(shOther,TopAbs_EDGE, map_edges);
    	for(unsigned int i=1; i<=map_edges.Extent();i++){
    		TopoDS_Shape s_i = map_edges.FindKey(i);
    		TopoDS_Edge e_i = TopoDS::Edge(s_i);
    		BRepAdaptor_Curve curve_adaptor(e_i);
    		double umin = curve_adaptor.FirstParameter();
    		double umax = curve_adaptor.LastParameter();

            double u1 = umin+(umax-umin)/3.0;

            gp_Pnt pnt1 = curve_adaptor.Value(u1);
            Utils::Math::Point pi1(pnt1.X(),pnt1.Y(),pnt1.Z());
            Utils::Math::Point proj_i1 = gpi.project(s1, pi1).first;
            if(!Utils::Math::MgxNumeric::isNearlyZero(pi1.length(proj_i1),tol))
            {
    			//            std::cout<<"Point  :"<<pi1<<std::endl;
    			//            std::cout<<"Projete:"<<proj_i1<<std::endl;
    			//            std::cout<<"\t dist "<<pi1.length(proj_i1)<<std::endl;
    			//            std::cout<<"\t tol  "<<tol<<std::endl;
                return false;
            }

            double u2 = umin+(umax-umin)*2.0/3.0;

            gp_Pnt pnt2 = curve_adaptor.Value(u2);
            Utils::Math::Point pi2(pnt2.X(),pnt2.Y(),pnt2.Z());

            Utils::Math::Point proj_i2 = gpi.project(s1, pi2).first;
            if(!Utils::Math::MgxNumeric::isNearlyZero(pi2.length(proj_i2),tol))
            {
                //            std::cout<<"Point  :"<<pi2<<std::endl;
                return false;
            }
    	}
    	//===============================================================
    	// On teste la surface
    	//===============================================================
    	//maintenant on regardes si des points internes de ASurf sont ou pas sur la
    	//surface (*this). Les points internes en questions sont 3 points pris parmi
    	//ceux des triangles discrétisant la face
    	TopLoc_Location aLoc;
    	TopoDS_Face otherFace = TopoDS::Face(shOther);
    	Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(otherFace,aLoc);
    	Handle_Geom_Surface surf = BRep_Tool::Surface(otherFace);
    	// On récupère la transformation de la shape/face
    	if (aPoly.IsNull()){
    		OCCHelper::buildIncrementalBRepMesh(otherFace, 0.1);
    		// mesher.Perform();
    		aPoly = BRep_Tool::Triangulation(otherFace,aLoc);
    	}
    	if (aPoly.IsNull()){
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
    		return true;
    	}


    	int selectTriangleID=1;
    	if(nbTriInFace>2)
    		selectTriangleID= nbTriInFace/2;

    	const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();

    	// Get the triangle
    	//   std::cout<<"Triangle choisi : "<<selectTriangleID<<std::endl;
    	Standard_Integer N1,N2,N3;
    	Triangles(selectTriangleID).Get(N1,N2,N3);
    	gp_Pnt V1 = aPoly->Node(N1), V2 = aPoly->Node(N2), V3 = aPoly->Node(N3);
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
    		throw TkUtil::Exception (TkUtil::UTF8String ("Problème de projection dans Surface::contains", TkUtil::Charset::UTF_8));

    	//maintenant on projete sur la surface *this
    	Utils::Math::Point middle_point(res.X(), res.Y(), res.Z());
    	Utils::Math::Point proj_i = gpi.project(s1, middle_point).first;
    	if(!Utils::Math::MgxNumeric::isNearlyZero(middle_point.length(proj_i),tol))
    		return false;
    } // end for (uint j=0; j<loc_reps.size(); j++)

    return true;
}
/*----------------------------------------------------------------------------*/
bool GeomContainsImplementation::
contains(Volume* v1, Volume* v2) const
{
    //double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
    double v1_bounds[6];
    double v2_bounds[6];

    v1->getBounds(v1_bounds);
    v2->getBounds(v2_bounds);
    double local_tolX = 0.5*(v2_bounds[1]-v2_bounds[0]);
    double local_tolY = 0.5*(v2_bounds[3]-v2_bounds[2]);
    double local_tolZ = 0.5*(v2_bounds[5]-v2_bounds[4]);
        // Pourquoi prendre une aussi grande tolérance ?

    if(    ( v2_bounds[0] < v1_bounds[0] - local_tolX)||//minX
            ( v2_bounds[1] > v1_bounds[1] + local_tolX)||//maxX
            ( v2_bounds[2] < v1_bounds[2] - local_tolY)||//minY
            ( v2_bounds[3] > v1_bounds[3] + local_tolY)||//maxY
            ( v2_bounds[4] < v1_bounds[4] - local_tolZ)||//minZ
            ( v2_bounds[5] > v1_bounds[5] + local_tolZ) )//maxZ
    {
        return false;
    }

    if(v1->getArea()<v2->getArea())
        return false;

    //===============================================================
    // EN FAIT IL FAUT TRAVAILLER AVEC LA SHAPES OCC, CAR LA SHAPE
    // TESTEE PEUT NE PAS ENCORE ETRE CONNECTEE TOPOLOGIQUEMENT
    // AVEC DES ENTITES M3D
    //===============================================================
    // 1 seule représentation pour le volume
    TopoDS_Shape shOther = v2->getOCCShape();
    TopoDS_Shape sh = v1->getOCCShape();
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
            std::cerr<<"===== VOLUME_GEOM UNE SURFACE OCC D'AIRE "<<surf_area<<std::endl;
        }
        else{
            TopLoc_Location aLoc;
            Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(f_i,aLoc);
            Handle_Geom_Surface surf = BRep_Tool::Surface(f_i);
            // On récupère la transformation de la shape/face
            if (aPoly.IsNull()){
               OCCHelper::buildIncrementalBRepMesh(f_i, 0.1);
                aPoly = BRep_Tool::Triangulation(f_i,aLoc);
            }
            if (aPoly.IsNull()){
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
                return true;
            }


            int selectTriangleID=1;
            if(nbTriInFace>2)
                selectTriangleID= nbTriInFace/2;

            const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();

            // Get the triangle
            //   std::cout<<"Triangle choisi : "<<selectTriangleID<<std::endl;
            Standard_Integer N1,N2,N3;
            Triangles(selectTriangleID).Get(N1,N2,N3);
            gp_Pnt V1 = aPoly->Node(N1), V2 = aPoly->Node(N2), V3 = aPoly->Node(N3);
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
                throw TkUtil::Exception (TkUtil::UTF8String ("Problème de projection dans Surface::contains", TkUtil::Charset::UTF_8));

            gp_Pnt p_i   = res;
            classifier.Perform(p_i,1e-6);
            TopAbs_State result = classifier.State();
            if(result==TopAbs_OUT || result==TopAbs_UNKNOWN){
                const TkUtil::UTF8String message("\t ** une face OUT", TkUtil::Charset::UTF_8);
                v1->log(TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
                return false;
            }
        }
    }
    return true;
}

/*----------------------------------------------------------------------------*/

bool GeomContainsImplementation::
contains(const TopoDS_Shape& sh,const TopoDS_Shape& shOther) const
{
#ifdef _DEBUG2
    	std::cout<<" OCCHelper::contains(...,...)"<<std::endl;
#endif

    /* On pourrait utiliser une méthode opencascade du type
    BRepAlgoAPI_Common common(innerShape, outerShape);
    TopoDS_Shape result = common.Shape();
    return (!result.IsNull() && result.IsSame(innerShape));
    */

    Utils::Math::Point min_bound, max_bound;
    OCCHelper::computeBoundingBox(sh, min_bound, max_bound);

    Utils::Math::Point min_other_bound, max_other_bound;
    OCCHelper::computeBoundingBox(shOther, min_other_bound, max_other_bound);

    double local_tolX = 0.5*(max_bound.getX()-min_bound.getX());
    double local_tolY = 0.5*(max_bound.getY()-min_bound.getY());
    double local_tolZ = 0.5*(max_bound.getZ()-min_bound.getZ());
    if(    ( min_other_bound.getX() < min_bound.getX() - local_tolX)||//minX
            ( max_other_bound.getX() > max_bound.getX() + local_tolX)||//maxX
            ( min_other_bound.getY() < min_bound.getY() - local_tolY)||//minY
            ( max_other_bound.getY() > max_bound.getY() + local_tolY)||//maxY
            ( min_other_bound.getZ() < min_bound.getZ() - local_tolZ)||//minZ
            ( max_other_bound.getZ() > max_bound.getZ() + local_tolZ) )//maxZ
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
                OCCHelper::buildIncrementalBRepMesh(f_i, 0.1);
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

            // Get the triangle
            //   std::cout<<"Triangle choisi : "<<selectTriangleID<<std::endl;
            Standard_Integer N1,N2,N3;
            Triangles(selectTriangleID).Get(N1,N2,N3);
            gp_Pnt V1 = aPoly->Node(N1), V2 = aPoly->Node(N2), V3 = aPoly->Node(N3);
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
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
