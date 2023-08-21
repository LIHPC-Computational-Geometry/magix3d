/*----------------------------------------------------------------------------*/
/** \file Surface.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 25/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
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
#include "Geom/MementoGeomEntity.h"
#include "Group/Group2D.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/FacetedSurface.h"
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
        GeomProperty* gprop, GeomRepresentation* compProp)
:GeomEntity(ctx, prop, disp, gprop,compProp)
{
}
/*----------------------------------------------------------------------------*/
Surface::Surface(Internal::Context& ctx, Utils::Property* prop,
        Utils::DisplayProperties* disp,
        GeomProperty* gprop, std::vector<GeomRepresentation*>& compProp)
:GeomEntity(ctx, prop, disp, gprop, compProp)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity* Surface::clone(Internal::Context& c)
{
	std::vector<GeomRepresentation*> newGeomRep;
	std::vector<GeomRepresentation*> oldGeomRep = this->getComputationalProperties();

	for (uint i=0; i<oldGeomRep.size(); i++)
		newGeomRep.push_back(oldGeomRep[i]->clone());

    return new Surface(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
			newGeomRep);
}
/*----------------------------------------------------------------------------*/
Surface::~Surface()
{
	//std::cout<<"Surface::~Surface() pour "<<getName()<<std::endl;
}
/*----------------------------------------------------------------------------*/
void Surface::setFromSpecificMemento(MementoGeomEntity& mem)
{
    m_curves = mem.getCurves();
    m_volumes = mem.getVolumes();
    m_groups  = mem.getGroups2D();
}
/*----------------------------------------------------------------------------*/
void Surface::createSpecificMemento(MementoGeomEntity& mem)
{
    mem.setCurves(m_curves);
    mem.setVolumes(m_volumes);
    mem.setGroups2D(m_groups);
}

/*----------------------------------------------------------------------------*/
void Surface::getRefEntities(std::vector<GeomEntity*>& entities)
{
    entities.clear();
    entities.insert(entities.end(),m_volumes.begin(),m_volumes.end());
    entities.insert(entities.end(),m_curves.begin(),m_curves.end());
}
/*----------------------------------------------------------------------------*/
void Surface::clearRefEntities(std::list<GeomEntity*>& vertices,
        std::list<GeomEntity*>& curves,
        std::list<GeomEntity*>& surfaces,
        std::list<GeomEntity*>& volumes)
{
    std::vector<Curve*> toRemoveC;
    std::vector<Volume*> toRemoveV;
    for(unsigned int i=0;i<m_curves.size();i++){
        GeomEntity *e = m_curves[i];
        std::list<GeomEntity*>::iterator res = std::find(curves.begin(),curves.end(),e);
        if(res!=curves.end())
            toRemoveC.push_back(dynamic_cast<Curve*>(e));
    }

    for(unsigned int i=0;i<m_volumes.size();i++){
        GeomEntity *e = m_volumes[i];
        std::list<GeomEntity*>::iterator res = std::find(volumes.begin(),volumes.end(),e);
        if(res!=volumes.end()){
            toRemoveV.push_back(dynamic_cast<Volume*>(e));
        }
    }

    for(unsigned int i=0;i<toRemoveC.size();i++)
        remove(toRemoveC[i]);
    for(unsigned int i=0;i<toRemoveV.size();i++)
        remove(toRemoveV[i]);
}

/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Vertex*>& vertices) const
{
    std::list<Vertex*> l;
    vertices.clear();
    for(unsigned int i=0; i <m_curves.size();i++){
        Curve* c = m_curves[i];
        std::vector<Vertex*> local_vertices;
        c->get(local_vertices);
        l.insert(l.end(),local_vertices.begin(),local_vertices.end());
    }
    l.sort(Utils::Entity::compareEntity);
    l.unique();

    vertices.insert(vertices.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Curve*>& curves) const
{
    curves.clear();
    curves.insert(curves.end(),m_curves.begin(),m_curves.end());
}
/*----------------------------------------------------------------------------*/
void Surface::getTemporary(std::vector<Curve*>& curves) const
{
	OCCGeomRepresentation* occ_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

	if(occ_rep==0)
		throw TkUtil::Exception (TkUtil::UTF8String ("getParametricBounds nécessite une représentation OCC", TkUtil::Charset::UTF_8));

	TopoDS_Shape sh = occ_rep->getShape();

	if(sh.ShapeType()!=TopAbs_FACE)
		throw TkUtil::Exception (TkUtil::UTF8String ("getTemporary nécessite une face OCC", TkUtil::Charset::UTF_8));

	curves.clear();
	TopExp_Explorer exp0;
	for(exp0.Init(sh, TopAbs_EDGE); exp0.More(); exp0.Next()){
		TopoDS_Edge edge = TopoDS::Edge(exp0.Current());

		Curve*  c   = new Curve(getContext(),
				getContext().newProperty(Utils::Entity::GeomCurve),
				getContext().newDisplayProperties(Utils::Entity::GeomCurve),
				new GeomProperty(), new OCCGeomRepresentation(getContext(), edge));

		curves.push_back(c);
	}
}
/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Surface*>& surfaces) const
{
    std::list<Surface*> l;
    surfaces.clear();
    for(unsigned int i=0; i <m_curves.size();i++){
        Curve *c = m_curves[i];
        std::vector<Surface*> local_surfaces;
        c->get(local_surfaces);
        l.insert(l.end(),local_surfaces.begin(),local_surfaces.end());
    }

    l.sort(Utils::Entity::compareEntity);
    l.unique();
    l.remove(const_cast<Surface*>(this));
    surfaces.insert(surfaces.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Surface::get(std::vector<Volume*>& volumes) const
{
    volumes.clear();
    volumes.insert(volumes.end(),m_volumes.begin(),m_volumes.end());
}
/*----------------------------------------------------------------------------*/
void Surface::project(Utils::Math::Point& P) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->project(P,this);
	else {
		// on va prendre la projection la plus courte pour le cas composé
		Utils::Math::Point pInit = P;

		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[0]->project(P,this);
		Utils::Math::Point pBest = P;
		double norme2 = (P-pInit).norme2();
		for (uint i=1; i<reps.size(); i++){
			P = pInit;
			reps[i]->project(P,this);
			double dist = (P-pInit).norme2();
			if (dist<norme2){
				norme2 = dist;
				pBest = P;
			}
		}
		P = pBest;
	}
}
/*----------------------------------------------------------------------------*/
void Surface::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->project(P1,P2,this);
	else {
		// on va prendre la projection la plus courte pour le cas composé

		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[0]->project(P1,P2,this);
		Utils::Math::Point pBest = P2;
		double norme2 = (P2-P1).norme2();
		for (uint i=1; i<reps.size(); i++){
			reps[i]->project(P1,P2,this);
			double dist = (P2-P1).norme2();
			if (dist<norme2){
				norme2 = dist;
				pBest = P2;
			}
		}
		P2 = pBest;
	}
}
/*----------------------------------------------------------------------------*/
void Surface::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->normal(P1,V2,this);
	else {
		// comme pour la projection, on recherche la plus courte distance et on utilise cette sous-surface
//		std::cout<<"normale en "<<P1<<"pour "<<getName()<<std::endl;
		uint ind = 0;
		Utils::Math::Point P2;
		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[0]->project(P1,P2,this);
		Utils::Math::Point pBest = P2;
		double norme2 = (P2-P1).norme2();
//		std::cout<<"i "<<0<<" norme2 "<<norme2<<std::endl;
		for (uint i=1; i<reps.size(); i++){
			reps[i]->project(P1,P2,this);
			double dist = (P2-P1).norme2();
//			std::cout<<"i "<<i<<" dist "<<dist<<std::endl;
			if (dist<norme2){
				norme2 = dist;
				pBest = P2;
				ind = i;
			}
		}
//		std::cout<<"  normale pour ind "<<ind<<" en "<<pBest<<std::endl;
		reps[ind]->normal(pBest, V2, this);
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
void Surface::split(std::vector<Curve* >& curv,std::vector<Vertex* >&  vert)
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->split(curv,vert,this);
	else {
		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		for (uint i=0; i<reps.size(); i++)
			reps[i]->split(curv,vert,this);
	}
}
/*----------------------------------------------------------------------------*/
double Surface::computeArea() const
{
	double area = 0.0;
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	for (uint i=0; i<reps.size(); i++)
		area += reps[i]->computeSurfaceArea();

	return area;
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
	// on dit que ce n'est pas planaire dès que c'est composé
	if (getComputationalProperties().size()>1)
		return false;

    OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    // pas plan si autre que OCC
    if(rep!=0){
    	TopoDS_Shape sh = rep->getShape();

    	if(sh.ShapeType() == TopAbs_FACE){
    		TopoDS_Face face = TopoDS::Face(sh);
    		Handle_Geom_Surface surface = BRep_Tool::Surface(face);
    		if(surface->DynamicType()==STANDARD_TYPE(Geom_Plane))
    			return true;
    	}
    }

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
    std::vector<GeomRepresentation*> loc_reps = ASurf->getComputationalProperties();

    for (uint j=0; j<loc_reps.size(); j++){
    	OCCGeomRepresentation* occ_rep =
    			dynamic_cast<OCCGeomRepresentation*>(loc_reps[j]);
    	CHECK_NULL_PTR_ERROR(occ_rep);

    	TopoDS_Shape shOther = occ_rep->getShape();

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
    		OCCGeomRepresentation::buildIncrementalBRepMesh(otherFace, 0.1);
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
/*----------------------------------------------------------------------------*/
void Surface::
getParametricBounds(double& U1,double& U2,double& V1,double& V2) const
{
    OCCGeomRepresentation* occ_rep =
            dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    if(occ_rep==0)
        throw TkUtil::Exception (TkUtil::UTF8String ("getParametricBounds nécessite une représentation OCC", TkUtil::Charset::UTF_8));

    TopoDS_Shape sh = occ_rep->getShape();

    if(sh.ShapeType()!=TopAbs_FACE)
        throw TkUtil::Exception (TkUtil::UTF8String ("getParametricBounds nécessite une face OCC", TkUtil::Charset::UTF_8));

    Handle_Geom_Surface surf = BRep_Tool::Surface(TopoDS::Face(sh));

    //calcul des parametres par OCC
    surf->Bounds(U1,U2,V1,V2);
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
Utils::Math::Point Surface::getPoint(const double u, const double v) const
{

    double umin, umax, vmin, vmax;
    getParametricBounds(umin, umax, vmin, vmax);

    if(u<umin || u>umax)
        throw TkUtil::Exception (TkUtil::UTF8String ("Parametre u hors de la plage [UMIN, UMAX]", TkUtil::Charset::UTF_8));
    if(v<vmin || v>vmax)
        throw TkUtil::Exception (TkUtil::UTF8String ("Parametre v hors de la plage [VMIN, VMAX]", TkUtil::Charset::UTF_8));

    OCCGeomRepresentation* occ_rep =
            dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    if(occ_rep==0)
        throw TkUtil::Exception (TkUtil::UTF8String ("getParametricBounds nécessite une représentation OCC", TkUtil::Charset::UTF_8));

    TopoDS_Shape sh = occ_rep->getShape();

    if(sh.ShapeType()!=TopAbs_FACE)
        throw TkUtil::Exception (TkUtil::UTF8String ("getParametricBounds nécessite une face OCC", TkUtil::Charset::UTF_8));

    Handle_Geom_Surface surf = BRep_Tool::Surface(TopoDS::Face(sh));

    gp_Pnt occ_pnt = surf->Value(u,v);

     return Utils::Math::Point(occ_pnt.X(), occ_pnt.Y(), occ_pnt.Z());

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

    // précision OpenCascade ou autre
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	for (uint i=0; i<reps.size(); i++){
		TkUtil::UTF8String	precStr (TkUtil::Charset::UTF_8);
		precStr << reps[i]->getPrecision();
	    propertyGeomDescription.addProperty (
	    	        Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );
	}

	// recherche des infos pour le cas facétisé
	bool isFaceted = false;
	uint nbFaces = 0;
	if (reps.size() == 1){
		FacetedSurface* fs = dynamic_cast<FacetedSurface*>(reps[0]);
		if (fs){
			isFaceted = true;
			nbFaces = fs->getNbFaces();
		}
	}


    // on ajoute des infos du style: c'est un plan
	TkUtil::UTF8String	typeStr (TkUtil::Charset::UTF_8);
    if (isPlanar())
    	typeStr<<"plan";
    else if (getComputationalProperties().size()>1)
    	typeStr<<"composée";
    else if (isFaceted)
    	typeStr<<"facétisée";
    else
    	typeStr<<"quelconque";

    propertyGeomDescription.addProperty (
    		Utils::SerializedRepresentation::Property ("Type", typeStr.ascii()) );

    if (isFaceted){
		TkUtil::UTF8String	nbStr (TkUtil::Charset::UTF_8);
    	nbStr<<(long int)nbFaces;

    	propertyGeomDescription.addProperty (
    			Utils::SerializedRepresentation::Property ("Nb polygones", nbStr.ascii()) );
    }

    description->addPropertiesSet (propertyGeomDescription);

	return description.release ( );
}
/*------------------------------------------------------------------------*/
GeomOrientation Surface::orientation() const
{
	OCCGeomRepresentation* this_rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();

	TopoDS_Face f = TopoDS::Face(this_shape);
	GeomOrientation value;
	switch(f.Orientation())
	{
	case(TopAbs_FORWARD):
		  value = GEOM_FORWARD;
	break;
	case(TopAbs_REVERSED):
		  value = GEOM_REVERSED;
	break;
	case(TopAbs_INTERNAL):
		  value = GEOM_INTERNAL;
	break;
	case(TopAbs_EXTERNAL):
		  value = GEOM_EXTERNAL;
	break;
	};
	return value;
}

/*------------------------------------------------------------------------*/
void Surface::bounds(double& AUMin, double& AUMax,
		double& AVMin, double& AVMax) const
{
	OCCGeomRepresentation* this_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();

	TopoDS_Face f = TopoDS::Face(this_shape);
	Handle_Geom_Surface surf = BRep_Tool::Surface(f);
	surf->Bounds(AUMin,AUMax,AVMin,AVMax);
}
/*------------------------------------------------------------------------*/
void Surface::d2(const double& AU, const double& AV,
		Utils::Math::Point& AP,
		Utils::Math::Vector& ADU,
		Utils::Math::Vector& ADV,
		Utils::Math::Vector& ADUU,
		Utils::Math::Vector& ADUV,
		Utils::Math::Vector& ADVV) const
{
	OCCGeomRepresentation* this_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();

	TopoDS_Face f = TopoDS::Face(this_shape);
	Handle_Geom_Surface surf = BRep_Tool::Surface(f);
	gp_Pnt pnt;
	gp_Vec du, dv, duu, dvv, duv;
	surf->D2(AU,AV,pnt,du,dv,duu,dvv,duv);

	//We get the location
	AP.setXYZ(pnt.X(),pnt.Y(),pnt.Z());

	//We get the first derivatives
	ADU.setX(du.X());
	ADU.setY(du.Y());
	ADU.setZ(du.Z());

	ADV.setX(dv.X());
	ADV.setY(dv.Y());
	ADV.setZ(dv.Z());

	//We get the second derivatives
	ADUU.setX(duu.X());
	ADUU.setY(duu.Y());
	ADUU.setZ(duu.Z());

	ADVV.setX(dvv.X());
	ADVV.setY(dvv.Y());
	ADVV.setZ(dvv.Z());

	ADUV.setX(duv.X());
	ADUV.setY(duv.Y());
	ADUV.setZ(duv.Z());
}
/*----------------------------------------------------------------------------*/
bool Surface::needLowerDimensionalEntityModification()
{
    std::vector<GeomRepresentation*> reps = getComputationalProperties();
    if (reps.size() == 1) {
        FacetedSurface *fs = dynamic_cast<FacetedSurface *>(reps[0]);
        if (fs)
            return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
