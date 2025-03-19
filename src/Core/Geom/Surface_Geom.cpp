/*----------------------------------------------------------------------------*/
/** \file Surface.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 25/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <string.h>
#include <sys/types.h>
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/Loop.h"
#include "Geom/OCCHelper.h"
#include "Group/Group2D.h"
#include "Geom/EntityFactory.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <ShapeAnalysis.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <Standard_Type.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const char* Surface::typeNameGeomSurface = "GeomSurface";

/*----------------------------------------------------------------------------*/
//void Surface::add(TopoDS_Edge e)
//{m_occ_edges.push_back(e);}
///*----------------------------------------------------------------------------*/
//std::vector<TopoDS_Edge> Surface::getEdges()
//{return m_occ_edges;}

/*----------------------------------------------------------------------------*/
Surface::Surface(Internal::Context& ctx, Utils::Property* prop,
        Utils::DisplayProperties* disp,
        GeomProperty* gprop, TopoDS_Face& shape)
:GeomEntity(ctx, prop, disp, gprop)
{
	m_occ_faces.push_back(shape);
}
/*----------------------------------------------------------------------------*/
Surface::Surface(Internal::Context& ctx, Utils::Property* prop,
        Utils::DisplayProperties* disp,
        GeomProperty* gprop, std::vector<TopoDS_Face>& shapes)
:GeomEntity(ctx, prop, disp, gprop)
, m_occ_faces(shapes)
{
}
/*----------------------------------------------------------------------------*/
void Surface::apply(std::function<void(const TopoDS_Shape&)> const& lambda) const
{
	for (auto sh : m_occ_faces)
		lambda(sh);
}
/*----------------------------------------------------------------------------*/
void Surface::applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda)
{
	for (int i=0 ; i<m_occ_faces.size() ; ++i) {
		m_occ_faces[i] = TopoDS::Face(lambda(m_occ_faces[i]));
	}
}
/*----------------------------------------------------------------------------*/
GeomEntity* Surface::clone(Internal::Context& c)
{
    return new Surface(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
			m_occ_faces);
}
/*----------------------------------------------------------------------------*/
Surface::~Surface()
{
	//std::cout<<"Surface::~Surface() pour "<<getName()<<std::endl;
}
/*----------------------------------------------------------------------------*/
uint Surface::project(Utils::Math::Point& P) const
{
	Utils::Math::Point P2;
	int idBest = project(P, P2);
	P = P2;
    return idBest;
}
/*----------------------------------------------------------------------------*/
uint Surface::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
	P2 = P1;
	OCCHelper::projectPointOn(m_occ_faces[0], P2);
	Utils::Math::Point pBest = P2;
	uint idBest = 0;
	double norme2 = (P2-P1).norme2();
	for (uint i=1; i<m_occ_faces.size(); i++){
		P2 = P1;
		OCCHelper::projectPointOn(m_occ_faces[i], P2);
		double dist = (P2-P1).norme2();
		if (dist<norme2){
			norme2 = dist;
			pBest = P2;
        	idBest = i;
		}
	}
	P2 = pBest;
    return idBest;
}
/*----------------------------------------------------------------------------*/
void Surface::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const
{
	TopoDS_Face face;
	if (m_occ_faces.size() == 1) {
		face = TopoDS::Face(m_occ_faces[0]);
	} else {
		uint idBest = project(P1, V2);
		face = TopoDS::Face(m_occ_faces[idBest]);
	}

	// projection pour en déduire les paramètres
	gp_Pnt pnt(P1.getX(),P1.getY(),P1.getZ());
	TopoDS_Vertex Vtx = BRepBuilderAPI_MakeVertex(pnt);
	BRepExtrema_DistShapeShape extrema(Vtx, face);
	bool isDone = extrema.IsDone();
	if(!isDone) {
		isDone = extrema.Perform();
	}
	//std::cout<<"  NbSolution "<<extrema.NbSolution()<<std::endl;
	if(!isDone){
		std::cerr<<"Surface::normal("<<P1<<")\n";
		throw TkUtil::Exception("Echec d'une projection d'un point sur une surface (pour la normale)!!");
	}

	if (extrema.SupportTypeShape2(1) == BRepExtrema_IsInFace){
		//std::cout<<"  Solution sur la surface"<<std::endl;
		Standard_Real U, V;
		extrema.ParOnFaceS2(1, U, V);

		Handle(Geom_Surface) brepSurface = BRep_Tool::Surface(face);

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
	}
	else if (extrema.SupportTypeShape2(1) == BRepExtrema_IsVertex){
		V2.setX(0);
		V2.setY(0);
		V2.setZ(0);
	}
}
/*----------------------------------------------------------------------------*/
void Surface::add(Volume* v)
{
    m_volumes.push_back(v);
}
/*----------------------------------------------------------------------------*/
void Surface::remove(Volume* v)
{
    std::vector<Volume*>::iterator it= m_volumes.begin();
    while(it!=m_volumes.end() && *it!=v)
        it++;

    if(it!=m_volumes.end())
        m_volumes.erase(it);
}
/*----------------------------------------------------------------------------*/
void Surface::add(Curve* c)
{
    m_curves.push_back(c);
}
/*----------------------------------------------------------------------------*/
void Surface::remove(Curve* c)
{
    std::vector<Curve*>::iterator it= m_curves.begin();
    while(it!=m_curves.end() && *it!=c)
        it++;

    if(it!=m_curves.end())
        m_curves.erase(it);
}
/*----------------------------------------------------------------------------*/
double Surface::computeArea() const
{
	double area = 0.0;
	for (auto rep : m_occ_faces)
		area += OCCHelper::computeArea(TopoDS::Face(rep));
	return area;
}
/*----------------------------------------------------------------------------*/
void Surface::computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const
{
	if (m_occ_faces.empty())
		return;

    OCCHelper::computeBoundingBox(m_occ_faces[0], pmin, pmax);
	for (uint i=1; i<m_occ_faces.size(); i++){
		Utils::Math::Point p1,p2;
        OCCHelper::computeBoundingBox(m_occ_faces[i], pmin, pmax);
		for (uint j=0; j<3; j++){
			double c1 = pmin.getCoord(j);
			double c2 = p1.getCoord(j);
			if (c2<c1)
				pmin.setCoord(j,c2);
		}
		for (uint j=0; j<3; j++){
			double c1 = pmax.getCoord(j);
			double c2 = p2.getCoord(j);
			if (c2>c1)
				pmin.setCoord(j,c2);
		}
	}
}
/*----------------------------------------------------------------------------*/
void Surface::addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const
{
    for(unsigned int i=0; i <m_curves.size();i++){
        l_entity.push_back(m_curves[i]);
        m_curves[i]->addAllDownLevelEntity(l_entity);
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Surface::
getCenteredPosition() const
{
    // on prend le barycentre des points aux centres des courbes
    // ce qui est plus juste pour le cas d'un demi disque par exemple
    Utils::Math::Point pt;

    if (m_curves.size()){
    	for (uint i=0; i<m_curves.size(); i++)
    		pt += m_curves[i]->getCenteredPosition();
    	pt /= (double)m_curves.size();

    	project(pt);
    }
    return pt;
}
/*----------------------------------------------------------------------------*/
bool Surface::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
void Surface::add(Group::Group2D* grp)
{
    m_groups.push_back(grp);
}
/*----------------------------------------------------------------------------*/
void Surface::remove(Group::Group2D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    if (i!=m_groups.size())
        m_groups.erase(m_groups.begin()+i);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas de groupe), avec Surface::remove", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool Surface::find(Group::Group2D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    return (i!=m_groups.size());
}
/*----------------------------------------------------------------------------*/
void Surface::getGroupsName (std::vector<std::string>& gn) const
{
    gn.clear();
    for (uint i = 0; i<m_groups.size(); ++i)
        gn.push_back(m_groups[i]->getName());
}
/*----------------------------------------------------------------------------*/
void Surface::getGroups(std::vector<Group::GroupEntity*>& grp) const
{
    grp.insert(grp.end(), m_groups.begin(), m_groups.end());
}
/*----------------------------------------------------------------------------*/
void Surface::setGroups(std::vector<Group::GroupEntity*>& grp)
{
	m_groups.clear();
	for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin(); iter != grp.end(); iter++){
		Group::Group2D* g2 = dynamic_cast<Group::Group2D*>(*iter);
		if (g2 == 0)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec conversion en groupe local dans setGroups", TkUtil::Charset::UTF_8));
		m_groups.push_back(g2);
	}
}
/*----------------------------------------------------------------------------*/
int Surface::getNbGroups() const
{
    return m_groups.size();
}
/*----------------------------------------------------------------------------*/
void Surface::setDestroyed(bool b)
{
    if (isDestroyed() == b)
        return;

    // supprime la relation du groupe vers le Surface en cas de destruction
    if (b)
        for (uint i = 0; i<m_groups.size(); ++i)
            m_groups[i]->remove(this);
    else
        // et inversement en cas de ressurection
        for (uint i = 0; i<m_groups.size(); ++i)
            m_groups[i]->add(this);

    Entity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
bool Surface::isPlanar() const
{
	if (m_occ_faces.size() == 1)
		return OCCHelper::isTypeOf(TopoDS::Face(m_occ_faces[0]), STANDARD_TYPE(Geom_Plane));
	else
		// courbe composée : on dit que ce n'est pas une ellipse
		return false;
}
/*----------------------------------------------------------------------------*/
bool Surface::contains(Surface* ASurf) const
{
    double my_bounds[6];
    double bounds[6];

    getBounds(my_bounds);
    ASurf->getBounds(bounds);
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
        return false;
    }

    // détermine un ordre de dimension pour la surface à partir de sa boite englobante
    double length = 0.0;
    for (uint i=0; i<3; i++)
    	if (length<my_bounds[i*2+1]-my_bounds[i*2])
    		length=my_bounds[i*2+1]-my_bounds[i*2];

    //std::cout<<" length pour "<<getName()<<" : "<<length<<std::endl;

    double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*length;

    double myArea    = getArea();
    double otherArea = ASurf->getArea();

    if(myArea<otherArea)
        return false;
    //===============================================================
    // EN FAIT IL FAUT TRAVAILLER AVEC LA SHAPES OCC, CAR LA SHAPE
    // TESTEE PEUT NE PAS ENCORE ETRE CONNECTEE TOPOLOGIQUEMENT
    // AVEC DES ENTITES M3D
    //===============================================================
	for (auto shOther : ASurf->getOCCFaces()) {
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
    		Utils::Math::Point proj_i;
    		project(pi,proj_i);
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

            Utils::Math::Point proj_i1;
            project(pi1,proj_i1);
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

            Utils::Math::Point proj_i2;
            project(pi2,proj_i2);
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
    		throw TkUtil::Exception (TkUtil::UTF8String ("Problème de projection dans Surface::contains", TkUtil::Charset::UTF_8));

    	//maintenant on projete sur la surface *this
    	Utils::Math::Point middle_point(res.X(), res.Y(), res.Z());
    	Utils::Math::Point proj_i;
    	project(middle_point,proj_i);
    	if(!Utils::Math::MgxNumeric::isNearlyZero(middle_point.length(proj_i),tol))
    		return false;
    } // end for (uint j=0; j<loc_reps.size(); j++)

    return true;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Surface::getPoint(const double u, const double v) const
{
	for (auto sh : m_occ_faces) {
		// check parametric bounds
		Handle_Geom_Surface surf = BRep_Tool::Surface(TopoDS::Face(sh));
		double umin, umax, vmin, vmax;
		surf->Bounds(umin, umax, vmin, vmax);
		if(u>=umin && u<=umax && v>=vmin && v<=vmax) {
			gp_Pnt occ_pnt = surf->Value(u,v);
			return Utils::Math::Point(occ_pnt.X(), occ_pnt.Y(), occ_pnt.Z());
		}
	}

    throw TkUtil::Exception (TkUtil::UTF8String ("Parametre u hors de la plage [UMIN, UMAX]", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Topo::CoFace*>& cofaces)
{
	const std::vector<Topo::TopoEntity* >& topos = getRefTopo();

	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topos.begin();
			iter != topos.end(); ++iter)
		if ((*iter)->getDim() == 2){
			Topo::CoFace* coface = dynamic_cast<Topo::CoFace*>(*iter);
			if (coface)
				cofaces.push_back(coface);
		}
}
/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Topo::CoEdge*>& coedges)
{
	const std::vector<Topo::TopoEntity* >& topos = getRefTopo();

	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topos.begin();
			iter != topos.end(); ++iter)
		if ((*iter)->getDim() == 1){
			Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(*iter);
			if (coedge)
				coedges.push_back(coedge);
		}
}
/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Topo::Vertex*>& vertices)
{
	const std::vector<Topo::TopoEntity* >& topos = getRefTopo();

	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topos.begin();
			iter != topos.end(); ++iter)
		if ((*iter)->getDim() == 0){
			Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(*iter);
			if (vertex)
				vertices.push_back(vertex);
		}
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Surface::getDescription (bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
			GeomEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ))

    Utils::SerializedRepresentation  propertyGeomDescription (
                                                "Propriétés géométriques", "");

	if (true == alsoComputed)
	{
		//recuperation de l'aire
		TkUtil::UTF8String	volStr (TkUtil::Charset::UTF_8);
		volStr<<getArea();

		propertyGeomDescription.addProperty (
	        Utils::SerializedRepresentation::Property ("Aire", volStr.ascii()) );
	}


#ifdef _DEBUG		// Issue#111
    // précision OpenCascade ou autre
	for (uint i=0; i<m_occ_faces.size(); i++){
		TkUtil::UTF8String	precStr (TkUtil::Charset::UTF_8);
		precStr << BRep_Tool::Tolerance(m_occ_faces[i]);
	    propertyGeomDescription.addProperty (
	    	        Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );
	}
#endif	// _DEBUG

    // on ajoute des infos du style: c'est un plan
	TkUtil::UTF8String	typeStr (TkUtil::Charset::UTF_8);
    if (isPlanar())
    	typeStr<<"plan";
    else if (m_occ_faces.size()>1)
    	typeStr<<"composée";
    else
    	typeStr<<"quelconque";

    propertyGeomDescription.addProperty (
    		Utils::SerializedRepresentation::Property ("Type", typeStr.ascii()) );

    description->addPropertiesSet (propertyGeomDescription);

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
