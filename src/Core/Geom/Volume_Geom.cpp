/*----------------------------------------------------------------------------*/
/** \file Volume.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 15/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <algorithm>	// CP : find sur Bull
#include <string.h>
#include <sys/types.h>
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/Shell.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/MementoGeomEntity.h"
#include "TkUtil/Exception.h"
#include "Group/Group3D.h"
#include "Internal/Context.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <BRepClass3d_SolidClassifier.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const char* Volume::typeNameGeomVolume = "GeomVolume";
/*----------------------------------------------------------------------------*/
/// fonction de tri géométrique utile pour ordonner les sommets (boite comme dans Bibop3D)
bool compareVertex(Vertex* v1, Vertex* v2)
{
    double z1 = v1->getZ();
    double z2 = v2->getZ();

    if (z1==z2){
        double y1 = v1->getY();
        double y2 = v2->getY();

        if (y1==y2){
            double x1 = v1->getX();
            double x2 = v2->getX();

            return (x1<x2);
        }
        else {
            return (y1<y2);
        }
    }
    else {
        return (z1<z2);
    }
}
/*----------------------------------------------------------------------------*/
Volume::Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
        GeomProperty* gprop, GeomRepresentation* compProp)
: GeomEntity(ctx, prop, disp, gprop,compProp)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity* Volume::clone(Internal::Context& c)
{
    return new Volume(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
            this->getComputationalProperty()->clone());
}

/*----------------------------------------------------------------------------*/
Volume::~Volume()
{}
/*----------------------------------------------------------------------------*/
void Volume::setFromSpecificMemento(MementoGeomEntity& mem)
{
    m_surfaces = mem.getSurfaces();
    m_groups  = mem.getGroups3D();
}
/*----------------------------------------------------------------------------*/
void Volume::createSpecificMemento(MementoGeomEntity& mem)
{
    mem.setSurfaces(m_surfaces);
    mem.setGroups3D(m_groups);
}
/*----------------------------------------------------------------------------*/
void Volume::getRefEntities(std::vector<GeomEntity*>& entities)
{
    entities.clear();
    entities.insert(entities.end(),m_surfaces.begin(),m_surfaces.end());
}
/*----------------------------------------------------------------------------*/
void Volume::clearRefEntities(std::list<GeomEntity*>& vertices,
        std::list<GeomEntity*>& curves,
        std::list<GeomEntity*>& surfaces,
        std::list<GeomEntity*>& volumes)
{
    std::vector<Surface*> toRemoveS;
    for(unsigned int i=0;i<m_surfaces.size();i++){
        GeomEntity *e = m_surfaces[i];
        std::list<GeomEntity*>::iterator res = std::find(surfaces.begin(),surfaces.end(),e);
        if(res!=surfaces.end())
            toRemoveS.push_back(dynamic_cast<Surface*>(e));
    }
    for(unsigned int i=0;i<toRemoveS.size();i++)
        remove(toRemoveS[i]);
}
/*----------------------------------------------------------------------------*/
void Volume::get(std::vector<Vertex*>& vertices) const
{
    std::list<Vertex*> l;
    vertices.clear();
    for(unsigned int i=0; i <m_surfaces.size();i++){
        Surface* s = m_surfaces[i];
        std::vector<Vertex*> local_vertices;
        s->get(local_vertices);
        l.insert(l.end(),local_vertices.begin(),local_vertices.end());
    }

    l.sort(Utils::Entity::compareEntity);
    l.unique();

    vertices.insert(vertices.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Volume::getGeomSorted(std::vector<Vertex*>& vertices) const
{
    std::list<Vertex*> l;
    vertices.clear();
    for(unsigned int i=0; i <m_surfaces.size();i++){
        Surface* s = m_surfaces[i];
        std::vector<Vertex*> local_vertices;
        s->get(local_vertices);
        l.insert(l.end(),local_vertices.begin(),local_vertices.end());
    }

    // on fait un tri suivant les positions géométriques
    //std::cout<<"Volume::get lance un sort pour "<<l.size()<<" Geom::Vertex"<<std::endl;
    l.sort(compareVertex);
    l.unique();

    vertices.insert(vertices.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Volume::get(std::vector<Curve*>& curves) const
{
    std::list<Curve*> l;
    curves.clear();
    for(unsigned int i=0; i <m_surfaces.size();i++){
        Surface* s = m_surfaces[i];
        std::vector<Curve*> local_curves;
        s->get(local_curves);
        l.insert(l.end(),local_curves.begin(),local_curves.end());
    }

    l.sort(Utils::Entity::compareEntity);
    l.unique();

    curves.insert(curves.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Volume::get(std::vector<Surface*>& surfaces) const
{
    surfaces.clear();
    surfaces.insert(surfaces.end(),m_surfaces.begin(),m_surfaces.end());
}
/*----------------------------------------------------------------------------*/
void Volume::get(std::vector<Volume*>& volumes) const
{
    std::list<Volume*> l;
    volumes.clear();
    for(unsigned int i=0; i <m_surfaces.size();i++){
        Surface* s = m_surfaces[i];
        std::vector<Volume*> local_vol;
        s->get(local_vol);
        l.insert(l.end(),local_vol.begin(),local_vol.end());
    }

    l.sort(Utils::Entity::compareEntity);
    l.unique();
    l.remove(const_cast<Volume*>(this));
    volumes.insert(volumes.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Volume::project(Utils::Math::Point& P) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("Volume::project n'est pas disponible", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Volume::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("Volume::project n'est pas disponible", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Volume::add(Surface* s)
{
    m_surfaces.push_back(s);
}
/*----------------------------------------------------------------------------*/
void Volume::remove(Surface* s)
{
#ifdef _DEBUG2
	std::cout<<"Volume::remove ("<<s->getName()<<") dans "<<getName()<<"\n qui contient les surfaces :"<<std::endl;
	for (uint i=0; i<m_surfaces.size(); i++)
		std::cout<<" "<<m_surfaces[i]->getName();
	std::cout<<std::endl;
#endif

    std::vector<Surface*>::iterator it= m_surfaces.begin();
    while((it!=m_surfaces.end()) && (*it!=s))
        it++;

    if(it!=m_surfaces.end())
        m_surfaces.erase(it);
}
/*----------------------------------------------------------------------------*/
void Volume::split(std::vector<Surface*>& surf,
        std::vector<Curve*  >& curv,
        std::vector<Vertex* >&  vert)
{
    getComputationalProperty()->split(surf,curv,vert,this);

}
/*----------------------------------------------------------------------------*/
double Volume::computeArea() const
{
    return getComputationalProperty()->computeVolumeArea();
}
/*----------------------------------------------------------------------------*/
bool Volume::contains(Volume* vol) const
{

    //double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
    double my_bounds[6];
    double bounds[6];

    getBounds(my_bounds);
    vol->getBounds(bounds);
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

    double myArea    = getArea();
    double otherArea = vol->getArea();

    if(myArea<otherArea)
        return false;
    //===============================================================
    // EN FAIT IL FAUT TRAVAILLER AVEC LA SHAPES OCC, CAR LA SHAPE
    // TESTEE PEUT NE PAS ENCORE ETRE CONNECTEE TOPOLOGIQUEMENT
    // AVEC DES ENTITES M3D
    //===============================================================
    OCCGeomRepresentation* rep =
            dynamic_cast<OCCGeomRepresentation*>(vol->getComputationalProperty());
    CHECK_NULL_PTR_ERROR(rep);
    TopoDS_Shape shOther = rep->getShape();

    OCCGeomRepresentation* my_rep =
                dynamic_cast<OCCGeomRepresentation*>(this->getComputationalProperty());
    CHECK_NULL_PTR_ERROR(my_rep);
    TopoDS_Shape sh = my_rep->getShape();

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
               OCCGeomRepresentation::buildIncrementalBRepMesh(f_i, 0.1);
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

            gp_Pnt p_i   = res;
            classifier.Perform(p_i,1e-6);
            TopAbs_State result = classifier.State();
            if(result==TopAbs_OUT || result==TopAbs_UNKNOWN){
                std::cerr<<"\t ** une face OUT"<<std::endl;
                return false;
            }
        }
    }
    return true;
}
/*----------------------------------------------------------------------------*/
void Volume::addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const
{
    for(unsigned int i=0; i <m_surfaces.size();i++){
        l_entity.push_back(m_surfaces[i]);
        m_surfaces[i]->addAllDownLevelEntity(l_entity);
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Volume::
getCenteredPosition() const
{
    // on prend le barycentre des points aux centres des surfaces
    // ce qui est plus juste pour le cas d'une demi sphère creuse par exemple
    Utils::Math::Point pt;

    for (uint i=0; i<m_surfaces.size(); i++)
        pt += m_surfaces[i]->getCenteredPosition();
    pt /= (double)m_surfaces.size();
    return pt;
}
/*----------------------------------------------------------------------------*/
bool Volume::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
void Volume::add(Group::Group3D* grp)
{
    m_groups.push_back(grp);
}
/*----------------------------------------------------------------------------*/
void Volume::remove(Group::Group3D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    if (i!=m_groups.size())
        m_groups.erase(m_groups.begin()+i);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas de groupe), avec Volume::remove", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool Volume::find(Group::Group3D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    return (i!=m_groups.size());
}
/*----------------------------------------------------------------------------*/
void Volume::getGroupsName (std::vector<std::string>& gn) const
{
    gn.clear();
    for (uint i = 0; i<m_groups.size(); ++i)
        gn.push_back(m_groups[i]->getName());
}
/*----------------------------------------------------------------------------*/
void Volume::getGroups(std::vector<Group::GroupEntity*>& grp) const
{
    grp.insert(grp.end(), m_groups.begin(), m_groups.end());
}
/*----------------------------------------------------------------------------*/
void Volume::setGroups(std::vector<Group::GroupEntity*>& grp)
{
	m_groups.clear();
	for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin(); iter != grp.end(); iter++){
		Group::Group3D* g3 = dynamic_cast<Group::Group3D*>(*iter);
		if (g3 == 0)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec conversion en groupe local dans setGroups", TkUtil::Charset::UTF_8));
		m_groups.push_back(g3);
	}
}
/*----------------------------------------------------------------------------*/
int Volume::getNbGroups() const
{
    return m_groups.size();
}
/*----------------------------------------------------------------------------*/
void Volume::setDestroyed(bool b)
{
    if (isDestroyed() == b)
        return;

    // supprime la relation du groupe vers le volume en cas de destruction
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
void Volume::
get(std::vector<Topo::Block*>& blocs) const
{
	const std::vector<Topo::TopoEntity*>& topo_entities = getRefTopo();
	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topo_entities.begin();
			iter != topo_entities.end(); ++iter)
		if ((*iter)->getDim() == 3)
			blocs.push_back(dynamic_cast<Topo::Block*>(*iter));
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Volume::getDescription (bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
			GeomEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ))

    Utils::SerializedRepresentation  propertyGeomDescription (
                                                "Propriétés géométriques", "");

	if (true == alsoComputed)
	{
		// récupération du volume
		TkUtil::UTF8String	volStr (TkUtil::Charset::UTF_8);
		volStr<<getArea();

		propertyGeomDescription.addProperty (
			Utils::SerializedRepresentation::Property ("Volume", volStr.ascii()) );
	}

    // pour afficher les spécifictés de certaines propriétés
    getGeomProperty()->addDescription(propertyGeomDescription);


    description->addPropertiesSet (propertyGeomDescription);

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
