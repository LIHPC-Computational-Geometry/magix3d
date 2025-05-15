/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCHelper.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/OCCDisplayRepresentationBuilder.h"
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
/*----------------------------------------------------------------------------*/
#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include <memory>			// unique_ptr
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomEntity::GeomEntity(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp, GeomProperty* gprop)
: Internal::InternalEntity (ctx, prop, disp),
  m_geomProp(gprop), m_computedAreaIsUpToDate(false), m_computedArea(0)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity::~GeomEntity()
{
    if(m_geomProp!=0)
        delete m_geomProp;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getBounds (double bounds[6]) const
{
	Entity::getBounds (bounds);	// RAZ
	Utils::Math::Point	min, max;
	computeBoundingBox (min, max);
	bounds [0]	= min.getX ( );
	bounds [1]	= max.getX ( );
	bounds [2]	= min.getY ( );
	bounds [3]	= max.getY ( );
	bounds [4]	= min.getZ ( );
	bounds [5]	= max.getZ ( );
}
/*----------------------------------------------------------------------------*/
GeomProperty* GeomEntity::setGeomProperty(GeomProperty* prop)
{
    GeomProperty* old_rep=0;
    if(prop==0){
        throw   TkUtil::Exception(TkUtil::UTF8String ("Null property", TkUtil::Charset::UTF_8));
    }

    old_rep = m_geomProp;
    m_geomProp= prop;
    return old_rep;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
#ifdef _DEBUG2
	std::cout<<"getRepresentation appelé pour "<<getName()<<std::endl;
#endif

    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_GEOM)
        throw TkUtil::Exception("Invalid display type entity");

    GeomDisplayRepresentation* gdr = dynamic_cast<GeomDisplayRepresentation*>(&dr);

    auto add_representation = [&](const TopoDS_Shape& sh) { 
        OCCDisplayRepresentationBuilder builder(this, sh, gdr);
        builder.execute();
    };
    this->apply(add_representation);

	// applique le shrink sur la représentation
	Utils::Math::Point barycentre = getCenteredPosition();
	double shrink = dr.getShrink();
	std::vector<Utils::Math::Point>& pts = dr.getPoints();
	if (shrink != 1.0)
		for(unsigned int i=0;i<pts.size();i++)
			pts[i] = (barycentre + (pts[i] - barycentre) * shrink);
}
/*----------------------------------------------------------------------------*/
template<typename T>
void GeomEntity::
buildSerializedRepresentation(Utils::SerializedRepresentation& description, const std::string& title, 
    const std::set<T*, decltype(&Utils::Entity::compareEntity)> elements) const
{
    if (!elements.empty()){
        Utils::SerializedRepresentation	sr (title,TkUtil::NumericConversions::toStr(elements.size()));
        for (auto elt : elements)
            sr.addProperty (Utils::SerializedRepresentation::Property(elt->getName(),*elt));
        description.addPropertiesSet (sr);
    }
}
/*----------------------------------------------------------------------------*/
template<typename T>
void GeomEntity::
buildSerializedRepresentation(Utils::SerializedRepresentation& description, const std::string& title, 
    const std::vector<T*> elements) const
{
    if (!elements.empty()){
        Utils::SerializedRepresentation	sr (title,TkUtil::NumericConversions::toStr(elements.size()));
        for (auto elt : elements)
            sr.addProperty (Utils::SerializedRepresentation::Property(elt->getName(),*elt));
        description.addPropertiesSet (sr);
    }
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* GeomEntity::
getDescription (bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
			InternalEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ))

	Utils::SerializedRepresentation  relationsGeomDescription ("Relations géométriques", "");
    GetUpIncidentGeomEntitiesVisitor vup;
    GetDownIncidentGeomEntitiesVisitor vdown;
    GetAdjacentGeomEntitiesVisitor vadj;
    this->accept(vup);
    this->accept(vdown);
    this->accept(vadj);

	// On y ajoute les éléments géométriques en relation avec celui-ci :
	auto v = vdown.getVertices();
    v.insert(vadj.getVertices().begin(), vadj.getVertices().end());
    buildSerializedRepresentation(relationsGeomDescription, "Sommets", v);
    TkUtil::UTF8String	summary (TkUtil::Charset::UTF_8);
    for (auto vert : v)
        summary << vert->getName ( ) <<" ";
    description->setSummary(summary.ascii());

	auto c = vup.getCurves();
    c.insert(vdown.getCurves().begin(), vdown.getCurves().end());
    c.insert(vadj.getCurves().begin(), vadj.getCurves().end());
    buildSerializedRepresentation(relationsGeomDescription, "Courbes", c);

	auto s = vup.getSurfaces();
    s.insert(vdown.getSurfaces().begin(), vdown.getSurfaces().end());
    s.insert(vadj.getSurfaces().begin(), vadj.getSurfaces().end());
    buildSerializedRepresentation(relationsGeomDescription, "Surfaces", s);

    auto vol = vup.getVolumes();
    vol.insert(vadj.getVolumes().begin(), vadj.getVolumes().end());
    buildSerializedRepresentation(relationsGeomDescription, "Volumes", vol);

	description->addPropertiesSet (relationsGeomDescription);

	// la Topologie s'il y en a une
	if (!m_topo_entities.empty()){
	    Utils::SerializedRepresentation  topoDescription ("Relations topologiques", "");

	    std::vector<Topo::Vertex*> v;
        std::vector<Topo::CoEdge*> e;
        std::vector<Topo::CoFace*> f;
        std::vector<Topo::Block*> b;

        for (std::vector<Topo::TopoEntity* >::const_iterator iter = m_topo_entities.begin();
                iter != m_topo_entities.end(); ++iter){
            if ((*iter)->getDim() == 0)
                v.push_back(dynamic_cast<Topo::Vertex*>(*iter));
            else if ((*iter)->getDim() == 1)
                e.push_back(dynamic_cast<Topo::CoEdge*>(*iter));
            else if ((*iter)->getDim() == 2)
                f.push_back(dynamic_cast<Topo::CoFace*>(*iter));
            else if ((*iter)->getDim() == 3)
                b.push_back(dynamic_cast<Topo::Block*>(*iter));
        }

        buildSerializedRepresentation(topoDescription, "Sommets topologiques", v);
        buildSerializedRepresentation(topoDescription, "Arêtes topologiques", e);
        buildSerializedRepresentation(topoDescription, "Faces topologiques", f);
        buildSerializedRepresentation(topoDescription, "Blocs topologiques", b);

	    description->addPropertiesSet (topoDescription);
	} else {
        description->addProperty (
                Utils::SerializedRepresentation::Property (
                        "Relations topologiques", std::string("Aucune")));
	}

    // Les groupes s'il y en a
	std::vector<Group::GroupEntity*> grp;
	getGroups(grp);
    if (!grp.empty()){
        Utils::SerializedRepresentation  groupeDescription ("Relations vers des groupes", "");

        std::vector<Group::Group0D*> g0;
        std::vector<Group::Group1D*> g1;
        std::vector<Group::Group2D*> g2;
        std::vector<Group::Group3D*> g3;

        for (std::vector<Group::GroupEntity* >::const_iterator iter = grp.begin();
                iter != grp.end(); ++iter){
            if ((*iter)->getDim() == 0)
                g0.push_back(dynamic_cast<Group::Group0D*>(*iter));
            else if ((*iter)->getDim() == 1)
                g1.push_back(dynamic_cast<Group::Group1D*>(*iter));
            else if ((*iter)->getDim() == 2)
                g2.push_back(dynamic_cast<Group::Group2D*>(*iter));
            else if ((*iter)->getDim() == 3)
                g3.push_back(dynamic_cast<Group::Group3D*>(*iter));
        }

        buildSerializedRepresentation(groupeDescription, "Groupes 0D", g0);
        buildSerializedRepresentation(groupeDescription, "Groupes 1D", g1);
        buildSerializedRepresentation(groupeDescription, "Groupes 2D", g2);
        buildSerializedRepresentation(groupeDescription, "Groupes 3D", g3);

        description->addPropertiesSet (groupeDescription);
        groupeDescription.setSummary(TkUtil::NumericConversions::toStr(grp.size()));
    } else {
        description->addProperty (
                Utils::SerializedRepresentation::Property (
                        "Relations vers des groupes", std::string("Aucune")));
    }

#ifdef _DEBUG
    Utils::SerializedRepresentation  occGeomDescription ("Propriétés OCC", "");
    // observation du HashCode retourné par OCC

    auto add_description = [&](const TopoDS_Shape& sh) {
        int hc = sh.HashCode(INT_MAX);
        occGeomDescription.addProperty (Utils::SerializedRepresentation::Property ("HashCode", (long int)hc));
    };
    this->apply(add_description);
   	description->addPropertiesSet (occGeomDescription);
#endif

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
GeomProperty::type GeomEntity::getGeomType ( ) const
{
	CHECK_NULL_PTR_ERROR (getGeomProperty ( ))
	return getGeomProperty ( )->getType ( );
}	// GeomEntity::getGeomType
/*----------------------------------------------------------------------------*/
void GeomEntity::addRefTopo(Topo::TopoEntity* te)
{
#ifdef _DEBUG2
    std::cout<<"GeomEntity::addRefTopo("<<(te?te->getName():"0")<<") à "<<getName()<<std::endl;
#endif

    // recherche si l'entité topologique ne serait pas déjà référencée
    uint i = 0;
    for (; i<m_topo_entities.size() && te != m_topo_entities[i]; ++i)
        ;

    // elle est déjà présente, on ne fait rien
    if (i != m_topo_entities.size())
        return;

    m_topo_entities.push_back(te);
}
/*----------------------------------------------------------------------------*/
void GeomEntity::removeRefTopo(Topo::TopoEntity* te)
{
#ifdef _DEBUG2
    std::cout<<"GeomEntity::removeRefTopo("<<(te?te->getName():"0")<<") à "<<getName()<<std::endl;
#endif
    uint i = 0;
    for (; i<m_topo_entities.size() && te != m_topo_entities[i]; ++i)
        ;

    // on la retire si elle est présente
    if (i!=m_topo_entities.size())
        m_topo_entities.erase(m_topo_entities.begin()+i);
//    else {
//        std::cout<<"m_topo_entities : ";
//        for(int i=0;i<m_topo_entities.size();i++)
//            std::cout<<" "<<m_topo_entities[i]->getName();
//        std::cout<<std::endl;
//        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas d'entité), avec GeomEntity::removeRefTopo", TkUtil::Charset::UTF_8));
//    }
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getRefTopo(std::vector<Topo::TopoEntity* >& vte) const
{
    vte.clear();
    vte.insert(vte.end(), m_topo_entities.begin(), m_topo_entities.end());
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getGroupsName (std::vector<std::string>& gn) const
{
    MGX_FORBIDDEN("getGroupsName est à redéfinir dans les classes dérivées");
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getGroups(std::vector<Group::GroupEntity*>& grp) const
{
    MGX_FORBIDDEN("getGroups est à redéfinir dans les classes dérivées");
}
/*----------------------------------------------------------------------------*/
void GeomEntity::setGroups(std::vector<Group::GroupEntity*>& grp)
{
    MGX_FORBIDDEN("setGroups est à redéfinir dans les classes dérivées");
}
/*----------------------------------------------------------------------------*/
int GeomEntity::getNbGroups() const
{
    MGX_FORBIDDEN("getNbGroups est à redéfinir dans les classes dérivées");
    return 0;
}
/*----------------------------------------------------------------------------*/
double GeomEntity::getArea() const
{
	//std::cout<<"GeomEntity::getArea() pour "<<getName()<<std::endl;
	if (!m_computedAreaIsUpToDate){
		m_computedAreaIsUpToDate = true;
		//std::cout<<" => computeArea()...\n";
		m_computedArea = computeArea();
	}
	//std::cout<<"return m_computedArea = "<< m_computedArea<<std::endl;
	return m_computedArea;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
