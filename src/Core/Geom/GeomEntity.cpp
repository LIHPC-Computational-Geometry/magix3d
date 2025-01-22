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
#include "Geom/GeomProperty.h"
#include "Geom/MementoGeomEntity.h"
#include "Geom/OCCDisplayRepresentationBuilder.h"
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Geom/OCCHelper.h"

#include "Topo/TopoEntity.h"
#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"

#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>

#include "Utils/Point.h"

#include <memory>			// unique_ptr

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomEntity::GeomEntity(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
        GeomProperty* gprop, const TopoDS_Shape& shape)
: Internal::InternalEntity (ctx, prop, disp),
  m_geomProp(gprop), m_computedAreaIsUpToDate(false), m_computedArea(0), m_shape(shape)
{
    if (useOCAF()){
		m_label = m_rootLabel.FindChild(getContext().nextUniqueId());
		TNaming_Builder tnb(m_label);
		tnb.Generated(m_shape);
	}
}
/*----------------------------------------------------------------------------*/
GeomEntity::~GeomEntity()
{
    if(m_geomProp!=0)
        delete m_geomProp;
}
/*----------------------------------------------------------------------------*/
bool GeomEntity::useOCAF() const
{
	return (getContext().getGeomKernel() == Internal::ContextIfc::WITHOCAF);
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
    //std::cout<<"GeomEntity::setFromMemento avec "<<getComputationalProperties().size()<<" GeomRepresentations remplacées par "<<mem.getGeomRepresentation().size()<<" pour "<<getName()<<std::endl;
    m_topo_entities = mem.getTopoEntities();
    m_geomProp = mem.getProperty();
    m_shape = mem.getShape();
    setFromSpecificMemento(mem);

    m_computedAreaIsUpToDate = false;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::createMemento(MementoGeomEntity& mem)
{
    mem.setTopoEntities(m_topo_entities);
    mem.setProperty(m_geomProp);
    mem.setShape(m_shape);
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
void GeomEntity::computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax,
                                               double tol) const
{
    Bnd_Box box;
    box.SetGap(tol);
    BRepBndLib::Add(m_shape,box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);
    pmin.setXYZ(xmin, ymin, zmin);
    pmax.setXYZ(xmax, ymax, zmax);
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

    TopoDS_Shape s = m_shape;
    OCCDisplayRepresentationBuilder builder(this, s,
                dynamic_cast<Geom::GeomDisplayRepresentation*>(&dr));
    builder.execute();

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
    bool isOCC = true;
    Utils::SerializedRepresentation  occGeomDescription (
    		"Propriétés OCC", "");
    // observation du HashCode retourné par OCC
    TopoDS_Shape sh1 = m_shape;
    int hc = sh1.HashCode(INT_MAX);
    occGeomDescription.addProperty (
            Utils::SerializedRepresentation::Property ("HashCode", (long int)hc));
    TDF_Label label = m_label;
    if (label.IsNull())
        occGeomDescription.addProperty (
                Utils::SerializedRepresentation::Property ("Label OCAF", std::string("non defini")));
    else {
        occGeomDescription.addProperty (
                Utils::SerializedRepresentation::Property ("OCAF - Label",(long int)label.Tag()));
        occGeomDescription.addProperty (
                Utils::SerializedRepresentation::Property ("OCAF - Depth",(long int)label.Depth()));
        occGeomDescription.addProperty (
                Utils::SerializedRepresentation::Property ("OCAF - NbChildren",(long int)label.NbChildren()));
        occGeomDescription.addProperty (
                Utils::SerializedRepresentation::Property ("OCAF - NbAttributes",(long int)label.NbAttributes()));
        occGeomDescription.addProperty (
                Utils::SerializedRepresentation::Property ("OCAF - Tag",(long int)label.Tag()));
    }
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
void GeomEntity::
getFacetedRepresentation(
        std::vector<gmds::math::Triangle >& AVec) const
{
    AVec.clear();
    TopoDS_Shape s = m_shape;
    OCCFacetedRepresentationBuilder builder(this, s);
    builder.execute(AVec);
}
/*----------------------------------------------------------------------------*/
void GeomEntity::
facetedRepresentationForwardOrient(
        GeomEntity* AEntityOrientation,
        std::vector<gmds::math::Triangle >* ATri) const
{
    TopoDS_Shape callerShape = m_shape;
    TopoDS_Shape entityOrientationShape = AEntityOrientation->getShape();

    /* si la shape est vide, on ne fait rien */
    if(m_shape.IsNull())
        throw TkUtil::Exception ("GeomEntity::facetedRepresentationForwardOrient "
                "shape est NULL.");

    TopExp_Explorer ex;
    for (ex.Init(callerShape, TopAbs_FACE); ex.More(); ex.Next()) {
    	if(OCCHelper::areEquals(TopoDS::Face(ex.Current()),TopoDS::Face(entityOrientationShape))) {

//    		TopoDS_Face face = TopoDS::Face(ex.Current());
//    		TopoDS_Face face_bis = TopoDS::Face(entityOrientationShape);
//
//    		TopAbs_Orientation orient = face_bis.Orientation();
//
//    		if(orient==TopAbs_FORWARD) {
//    			std::cout<<"TopAbs_FORWARD "<<std::endl;
//    		}
//    		if(orient==TopAbs_REVERSED) {
//    			std::cout<<"TopAbs_REVERSED "<<std::endl;
//    		}
//    		if(orient==TopAbs_INTERNAL) {
//    			std::cout<<"TopAbs_INTERNAL "<<std::endl;
//    		}
//    		if(orient==TopAbs_EXTERNAL) {
//    			std::cout<<"TopAbs_EXTERNAL "<<std::endl;
//    		}
//
//    		return (orient == TopAbs_FORWARD);

//    		AEntityOrientation
//    		 fillRepresentation(TopoDS::Face(ex.Current()),AVec);
    		ATri->clear();

    		OCCFacetedRepresentationBuilder builder(this, callerShape);
    		builder.execute(*ATri,entityOrientationShape);

    		return;
    	}
    }
    throw TkUtil::Exception ("GeomEntity::facetedRepresentationForwardOrient "
            "la shape que l'on doit orienter n'a pas été trouvée.");
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
TopoDS_Shape& GeomEntity::getShape()  {
    return m_shape;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::projectPointOn( Utils::Math::Point& P) const
{

	if(!m_shape.IsNull() && m_shape.ShapeType()==TopAbs_VERTEX)
	{
		gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(m_shape));
		P.setXYZ(pnt.X(), pnt.Y(), pnt.Z());
	}
	else
	{
		gp_Pnt pnt(P.getX(),P.getY(),P.getZ());
		TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
		BRepExtrema_DistShapeShape extrema(V, m_shape);
		bool isDone = extrema.IsDone();
		if(!isDone) {
			isDone = extrema.Perform();
		}

		if(!isDone){
			std::cerr<<"OCCGeomRepresentation::projectPointOn("<<P<<")\n";
			throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");

		}
		gp_Pnt pnt2 = extrema.PointOnShape2(1);
		P.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
	}
}
/*----------------------------------------------------------------------------*/
void GeomEntity::translate(const Utils::Math::Vector& V)
{
    gp_Trsf T;
    gp_Vec v(V.getX(),V.getY(),V.getZ());
    T.SetTranslation(v);
    BRepBuilderAPI_Transform translat(T);
    //on effectue la translation
    translat.Perform(m_shape);

    if(!translat.IsDone())
        throw TkUtil::Exception("Echec d'une translation!!");

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = translat.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomEntity::scale(const double F, const Point& center)
{
    Bnd_Box box;
    box.SetGap(Utils::Math::MgxNumeric::mgxDoubleEpsilon);
    BRepBndLib::Add(m_shape, box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);

    gp_Trsf T;
    T.SetScale(gp_Pnt(center.getX(), center.getY(), center.getZ()),F);
    BRepBuilderAPI_Transform scaling(T);

    //on effectue l'homotéthie
    scaling.Perform(m_shape);

    if(!scaling.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une homothétie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = scaling.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomEntity::scale(const double factorX,
        const double factorY,
        const double factorZ,
        const Point& center)
{
    Bnd_Box box;
    box.SetGap(Utils::Math::MgxNumeric::mgxDoubleEpsilon);
    BRepBndLib::Add(m_shape, box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);

    gp_GTrsf GT;
    GT.SetValue(1,1, factorX);
    GT.SetValue(2,2, factorY);
    GT.SetValue(3,3, factorZ);
	GT.SetValue(1,4, (1-factorX) * center.getX());
	GT.SetValue(2,4, (1-factorY) * center.getY());
    GT.SetValue(3,4, (1-factorZ) * center.getZ());

    BRepBuilderAPI_GTransform scaling(GT);

    //on effectue l'homothétie
    scaling.Perform(m_shape);

    if(!scaling.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une homothétie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = scaling.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomEntity::rotate(const Utils::Math::Point& P1,
        const Utils::Math::Point& P2, double Angle)
{
    gp_Trsf T;
    gp_Pnt p1(P1.getX(),P1.getY(),P1.getZ());

    gp_Dir dir( P2.getX()-P1.getX(),
                P2.getY()-P1.getY(),
                P2.getZ()-P1.getZ());

    gp_Ax1 axis(p1,dir);
    T.SetRotation(axis,Angle);
    BRepBuilderAPI_Transform rotat(T);
    //on effectue la rotation
    rotat.Perform(m_shape);

    if(!rotat.IsDone())
        throw TkUtil::Exception("Echec d'une rotation!!");

    //on stocke le résultat de la translation (maj de la shape interne)
    m_shape = rotat.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomEntity::mirror(const Utils::Math::Plane& plane)
{
	gp_Trsf T;
    Utils::Math::Point plane_pnt = plane.getPoint();
    Utils::Math::Vector plane_vec = plane.getNormal();

    gp_Ax2 A2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ()));
    T.SetMirror (A2);
    BRepBuilderAPI_Transform trans(T);

    trans.Perform(m_shape);

    if(!trans.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une symétrie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la transformation (maj de la shape interne)
    m_shape = trans.Shape();
}
/*----------------------------------------------------------------------------*/
double GeomEntity::getPrecision()
{
	if(m_shape.ShapeType()==TopAbs_VERTEX){
		return BRep_Tool::Tolerance(TopoDS::Vertex(m_shape));
	}
	else if(m_shape.ShapeType()==TopAbs_EDGE){
		return BRep_Tool::Tolerance(TopoDS::Edge(m_shape));
	}
	else if(m_shape.ShapeType()==TopAbs_FACE){
		return BRep_Tool::Tolerance(TopoDS::Face(m_shape));
	}
	// [EB] pas trouvé de précision pour un volume

	return 0.0;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
