/*----------------------------------------------------------------------------*/
/** \file GeomEntity.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 18/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCHelper.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/OCCDisplayRepresentationBuilder.h"
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Geom/MementoGeomEntity.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>

#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"

#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"

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
void GeomEntity::setFromMemento(MementoGeomEntity& mem)
{
    m_topo_entities = mem.getTopoEntities();
    m_geomProp = mem.getProperty();
    setFromSpecificMemento(mem);

    m_computedAreaIsUpToDate = false;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::createMemento(MementoGeomEntity& mem)
{
    mem.setTopoEntities(m_topo_entities);
    mem.setProperty(m_geomProp);
    createSpecificMemento(mem);
}
/*----------------------------------------------------------------------------*/
void GeomEntity::setFromSpecificMemento(MementoGeomEntity& mem)
{
	INTERNAL_ERROR(exc, TkUtil::UTF8String ("Méthode non surchargée.", TkUtil::Charset::UTF_8), "GeomEntity::setFromSpecificMemento")
	throw exc;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::createSpecificMemento(MementoGeomEntity& mem)
{
	INTERNAL_ERROR(exc, TkUtil::UTF8String ("Méthode non surchargée.", TkUtil::Charset::UTF_8), "GeomEntity::createSpecificMemento")
	throw exc;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getRefEntities(std::vector<GeomEntity*>& entities)
{
	INTERNAL_ERROR(exc, TkUtil::UTF8String ("Méthode non surchargée.", TkUtil::Charset::UTF_8), "GeomEntity::getRefEntities")
	throw exc;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::clearRefEntities(std::list<GeomEntity*>& vertices,
            std::list<GeomEntity*>& curves,
            std::list<GeomEntity*>& surfaces,
            std::list<GeomEntity*>& volumes)
{
	INTERNAL_ERROR(exc, TkUtil::UTF8String ("Méthode non surchargée.", TkUtil::Charset::UTF_8), "GeomEntity::clearRefEntities")
	throw exc;
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
Utils::SerializedRepresentation* GeomEntity::
getDescription (bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
			InternalEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ))


	Utils::SerializedRepresentation  relationsGeomDescription (
                                                "Relations géométriques", "");


	// On y ajoute les éléments géométriques en relation avec celuis-ci :
	std::vector<Vertex*>		v;
	get (v);
	if (!v.empty()){
		TkUtil::UTF8String	summary (TkUtil::Charset::UTF_8);
	    Utils::SerializedRepresentation	vertices ("Sommets",
	            TkUtil::NumericConversions::toStr(v.size()));
	    for (std::vector<Vertex*>::iterator itv = v.begin( ); v.end( )!=itv; itv++){
	        vertices.addProperty (
	                Utils::SerializedRepresentation::Property (
	                        (*itv)->getName ( ),  *(*itv)));
	        summary << (*itv)->getName ( ) <<" ";
	    }
	    relationsGeomDescription.addPropertiesSet (vertices);
	    description->setSummary(summary.ascii());
	}

	std::vector<Curve*>			c;
	get (c);
	if (!c.empty()){
	    Utils::SerializedRepresentation	curves ("Courbes",
                TkUtil::NumericConversions::toStr(c.size()));
	    for (std::vector<Curve*>::iterator itc = c.begin( ); c.end( )!=itc; itc++)
	        curves.addProperty (
	                Utils::SerializedRepresentation::Property (
	                        (*itc)->getName ( ), *(*itc)));
	    relationsGeomDescription.addPropertiesSet (curves);
	}

	std::vector<Surface*>		s;
	get (s);
	if (!s.empty()){
	    Utils::SerializedRepresentation	surfaces ("Surfaces",
                TkUtil::NumericConversions::toStr(s.size()));
	    for (std::vector<Surface*>::iterator its = s.begin( ); s.end( )!=its; its++)
	        surfaces.addProperty (
	                Utils::SerializedRepresentation::Property (
	                        (*its)->getName ( ), *(*its)));
	    relationsGeomDescription.addPropertiesSet (surfaces);
	}

	std::vector<Volume*>		vol;
	get (vol);
	if (!vol.empty()){
	    Utils::SerializedRepresentation	volumes ("Volumes",
                TkUtil::NumericConversions::toStr(vol.size()));
	    for (std::vector<Volume*>::iterator itv = vol.begin( ); vol.end( )!=itv;
	            itv++)
	        volumes.addProperty (
	                Utils::SerializedRepresentation::Property (
	                        (*itv)->getName ( ), *(*itv)));
	    relationsGeomDescription.addPropertiesSet (volumes);
	}

	description->addPropertiesSet (relationsGeomDescription);


	// la Topologie s'il y en a une
	if (!m_topo_entities.empty()){
	    Utils::SerializedRepresentation  topoDescription (
	            "Relations topologiques", "");

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

        if (!v.empty()){
            Utils::SerializedRepresentation  vertices ("Sommets topologiques",
                    TkUtil::NumericConversions::toStr(v.size()));
            for (std::vector<Topo::Vertex*>::iterator iter = v.begin( ); v.end( )!=iter; iter++)
                vertices.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            topoDescription.addPropertiesSet (vertices);
        }

        if (!e.empty()){
            Utils::SerializedRepresentation  coedges ("Arêtes topologiques",
                    TkUtil::NumericConversions::toStr(e.size()));
            for (std::vector<Topo::CoEdge*>::iterator iter = e.begin( ); e.end( )!=iter; iter++)
                coedges.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            topoDescription.addPropertiesSet (coedges);
        }

        if (!f.empty()){
            Utils::SerializedRepresentation  cofaces ("Faces topologiques",
                    TkUtil::NumericConversions::toStr(f.size()));
            for (std::vector<Topo::CoFace*>::iterator iter = f.begin( ); f.end( )!=iter; iter++)
                cofaces.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            topoDescription.addPropertiesSet (cofaces);
        }

        if (!b.empty()){
            Utils::SerializedRepresentation  blocks ("Blocs topologiques",
                    TkUtil::NumericConversions::toStr(b.size()));
            for (std::vector<Topo::Block*>::iterator iter = b.begin( ); b.end( )!=iter; iter++)
                blocks.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            topoDescription.addPropertiesSet (blocks);
        }

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
        Utils::SerializedRepresentation  groupeDescription (
                "Relations vers des groupes", "");

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

        if (!g0.empty()){
            Utils::SerializedRepresentation  groupe ("Groupes 0D",
                    TkUtil::NumericConversions::toStr(g0.size()));
            for (std::vector<Group::Group0D*>::iterator iter = g0.begin( ); g0.end( )!=iter; iter++)
                groupe.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            groupeDescription.addPropertiesSet (groupe);
        }

        if (!g1.empty()){
            Utils::SerializedRepresentation  groupe ("Groupes 1D",
                    TkUtil::NumericConversions::toStr(g1.size()));
            for (std::vector<Group::Group1D*>::iterator iter = g1.begin( ); g1.end( )!=iter; iter++)
                groupe.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            groupeDescription.addPropertiesSet (groupe);
        }

        if (!g2.empty()){
            Utils::SerializedRepresentation  groupe ("Groupes 2D",
                    TkUtil::NumericConversions::toStr(g2.size()));
            for (std::vector<Group::Group2D*>::iterator iter = g2.begin( ); g2.end( )!=iter; iter++)
                groupe.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            groupeDescription.addPropertiesSet (groupe);
        }

        if (!g3.empty()){
            Utils::SerializedRepresentation  groupe ("Groupes 3D",
                    TkUtil::NumericConversions::toStr(g3.size()));
            for (std::vector<Group::Group3D*>::iterator iter = g3.begin( ); g3.end( )!=iter; iter++)
                groupe.addProperty (
                        Utils::SerializedRepresentation::Property (
                                (*iter)->getName ( ),  *(*iter)));
            groupeDescription.addPropertiesSet (groupe);
        }

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
std::string GeomEntity::getSummary ( ) const
{
	std::vector<Vertex*>		v;
	get (v);
	TkUtil::UTF8String	summary (TkUtil::Charset::UTF_8);
	for (std::vector<Vertex*>::iterator itv = v.begin( ); v.end( )!=itv; itv++){
	        summary << (*itv)->getName ( ) <<" ";
	    }

	return summary.ascii ( );
}
/*----------------------------------------------------------------------------*/
GeomProperty::type GeomEntity::getGeomType ( ) const
{
	CHECK_NULL_PTR_ERROR (getGeomProperty ( ))
	return getGeomProperty ( )->getType ( );
}	// GeomEntity::getGeomType
/*----------------------------------------------------------------------------*/
void GeomEntity::add(Vertex* v)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::add(Curve* c)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::add(Surface* s)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::add(Volume* v)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::add(GeomEntity* e)
{
    switch(e->getDim()){
    case 0:
        add(dynamic_cast<Vertex*>(e));
        break;
    case 1:
        add(dynamic_cast<Curve*>(e));
        break;
    case 2:
        add(dynamic_cast<Surface*>(e));
        break;
    case 3:
        add(dynamic_cast<Volume*>(e));
        break;
    default:
        throw   TkUtil::Exception(TkUtil::UTF8String ("Invalid geometric cell dimension", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void GeomEntity::remove(Vertex* v)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::remove(Curve* c)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::remove(Surface* s)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::remove(Volume* v)
{}
/*----------------------------------------------------------------------------*/
void GeomEntity::remove(GeomEntity* e)
{
    switch(e->getDim()){
    case 0:
        remove(dynamic_cast<Vertex*>(e));
        break;
    case 1:
        remove(dynamic_cast<Curve*>(e));
        break;
    case 2:
        remove(dynamic_cast<Surface*>(e));
        break;
    case 3:
        remove(dynamic_cast<Volume*>(e));
        break;
    default:
        throw   TkUtil::Exception(TkUtil::UTF8String ("Invalid geometric cell dimension", TkUtil::Charset::UTF_8));
    }
}
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
