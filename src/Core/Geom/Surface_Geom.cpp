/*----------------------------------------------------------------------------*/
#include "Geom/Surface.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Volume.h"
#include "Geom/OCCHelper.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomProjectImplementation.h"
#include "Group/Group2D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Type.hxx>
/*----------------------------------------------------------------------------*/
#include <list>
#include <string>
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
void Surface::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const
{
	TopoDS_Face face;
	if (m_occ_faces.size() == 1) {
		face = TopoDS::Face(m_occ_faces[0]);
	} else {
		uint idBest;
		std::tie(V2, idBest) = GeomProjectImplementation().project(this, P1);
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
    	pt = GeomProjectImplementation().project(this, pt).first;
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
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
