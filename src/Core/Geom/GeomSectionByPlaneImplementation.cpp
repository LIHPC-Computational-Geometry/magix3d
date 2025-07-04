/*----------------------------------------------------------------------------*/
#include "Geom/GeomSectionByPlaneImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/EntityFactory.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Point.h"
#include "Utils/Plane.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepFeat_Gluer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
/*----------------------------------------------------------------------------*/
#include <map>
#include <set>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D
{
/*----------------------------------------------------------------------------*/
namespace Geom
{
/*----------------------------------------------------------------------------*/
GeomSectionByPlaneImplementation::GeomSectionByPlaneImplementation(
        Internal::Context& c,
        std::vector<GeomEntity*>& es, Utils::Math::Plane* p) :
        GeomModificationBaseClass(c), m_plane(p)
{
    m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());

}
/*----------------------------------------------------------------------------*/
GeomSectionByPlaneImplementation::~GeomSectionByPlaneImplementation()
{
}
/*----------------------------------------------------------------------------*/
void GeomSectionByPlaneImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomSectionByPlaneImplementation::perform(std::vector<GeomEntity*>& res)
{
	splitEntities(res);
}
/*----------------------------------------------------------------------------*/
void GeomSectionByPlaneImplementation::splitEntities(std::vector<GeomEntity*>& res)
{
#ifdef _DEBUG2
	std::cout<<"GeomSectionByPlaneImplementation::splitEntities ..."<<std::endl;
#endif
    //========================================================================
    // 1 - Définition du plan de coupe
    //========================================================================
    Utils::Math::Point plane_pnt = m_plane->getPoint();
    Utils::Math::Vector plane_vec = m_plane->getNormal();
    gp_Pln gp_plane(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
            gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ()));
    BRepBuilderAPI_MakeFace mkF(gp_plane);
    TopoDS_Face wf = mkF.Face();

    //========================================================================
    // 2 - Conservation du plan de coupe réduit aux objets coupés
    //========================================================================
    // On commence par fusionner toutes les entites a couper
#ifdef _DEBUG2
	std::cout<<"m_init_entities.size = "<<m_init_entities.size()<<std::endl;
#endif
    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s_fuse;
    getUniqueOCCShape(e1, s_fuse);
    for(unsigned int i=1;i<m_init_entities.size();i++){
        GeomEntity* e2 = m_init_entities[i];
        TopoDS_Shape s2;
        getUniqueOCCShape(e2, s2);

        BRepAlgoAPI_Fuse fuse_operator(s_fuse,s2);
        if(fuse_operator.IsDone())
            s_fuse = fuse_operator.Shape();
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union avant coupe", TkUtil::Charset::UTF_8));
    }
    // On recupere l'intersection de la surface wf et de l'union des entites à couper
    BRepAlgoAPI_Common common_operator(s_fuse,wf);
    if(common_operator.IsDone())
        m_restricted_section_tool = common_operator.Shape();
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'intersection avant coupe", TkUtil::Charset::UTF_8));

    //========================================================================
    // 3 - Decoupe des entités
    //========================================================================
    TopoDS_Shape s;
    BRepAlgoAPI_BuilderAlgo splitter;

    TopTools_ListOfShape list_of_arguments;
    for (unsigned int i = 0; i < m_init_entities.size(); i++) {
    	GeomEntity* ei = m_init_entities[i];
    	TopoDS_Shape si;
        getUniqueOCCShape(ei, si);
        list_of_arguments.Append(si);
    }
    list_of_arguments.Append(wf);
    splitter.SetArguments(list_of_arguments);
    splitter.Build();

    s = splitter.Shape();

    createGeomEntities(s,true);

    //=========================================================================
    //          Gestion des groupes de volumes (si un seul volume en entrée)
    //=========================================================================
    if(m_init_entities.size()==1){
        GeomEntity* ei= m_init_entities[0];
        std::vector<GeomEntity*>& rep_ei =  m_replacedEntities[ei];
        for(unsigned int j=0;j<m_newVolumes.size();j++)
            rep_ei.push_back(m_newVolumes[j]);
    }

    //    res.insert(res.end(),new_volumes.begin(),new_volumes.end());

    res.insert(res.end(), entities_new.begin(), entities_new.end());
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*> GeomSectionByPlaneImplementation::getEntitiesOnPlane()
{
    std::vector<GeomEntity*> planeEntities;
    planeEntities.clear();
    for (unsigned int i = 0; i < m_newEntities.size(); i++)
    {
        GeomEntity* e = m_newEntities[i];

        if (isOnPlane(e))
        {
            planeEntities.push_back(e);
        }
    }
    return planeEntities;
}
/*----------------------------------------------------------------------------*/
bool GeomSectionByPlaneImplementation::isOnPlane(GeomEntity* e)
{
    bool onPlane = true;

    if (e->getDim() == 3) //VOLUME
        onPlane = false;
    else if (e->getDim() == 2)
    { //SURFACE
        std::vector<Curve*> curves = dynamic_cast<Surface*>(e)->getCurves();
        for (unsigned int i = 0; i < curves.size() && onPlane; i++)
        {
            if (!isOnPlane(curves[i]))
                onPlane = false;
        }
    }
    else if (e->getDim() == 1)
    { //COURBE
        onPlane = (bool)isOnPlane(dynamic_cast<Curve*>(e));
    }
    else if (e->getDim() == 0)
    { //SOMMET
        onPlane = (bool)isOnPlane(dynamic_cast<Vertex*>(e));
    }
    return onPlane;
}

/*----------------------------------------------------------------------------*/
bool GeomSectionByPlaneImplementation::isOnPlane(Vertex* v)
{
	CHECK_NULL_PTR_ERROR(v);
    Utils::Math::Point p = v->getCenteredPosition();
    return isOnPlane(p);
}
/*----------------------------------------------------------------------------*/
bool GeomSectionByPlaneImplementation::isOnPlane(Curve* c)
{

    bool onPlane = true;
    auto vertices = c->getVertices();
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        Vertex* vi = vertices[i];
        if (!isOnPlane(vi))
            onPlane = false;
    }

    if (onPlane)
    {
        //on regarde deux points sur la courbe
        Utils::Math::Point p;
        c->getPoint(0.25, p);
        if (!isOnPlane(p))
            onPlane = false;
        c->getPoint(0.5, p);
        if (!isOnPlane(p))
            onPlane = false;
        c->getPoint(0.75, p);
        if (!isOnPlane(p))
            onPlane = false;
    }
    return onPlane;
}
/*----------------------------------------------------------------------------*/
bool GeomSectionByPlaneImplementation::isOnPlane(Utils::Math::Point& p)
{
    Utils::Math::Point planePnt = m_plane->getPoint();
    Utils::Math::Vector planeVec = m_plane->getNormal();
    Utils::Math::Vector vec(p, planePnt);
//    std::cerr<<"Point "<<p<<" -> ";
//    if (fabs(vec.dot(planeVec)) < Mgx3D::Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
//        std::cerr<<"ON "<<std::endl;
//    else
//        std::cerr<<"OUT"<<std::endl;
    return (fabs(vec.dot(planeVec))
            < Mgx3D::Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon);
}
/*----------------------------------------------------------------------------*/
void GeomSectionByPlaneImplementation::
getUniqueOCCShape(GeomEntity* ge, TopoDS_Shape& sh) const
{
    switch (ge->getDim())
    {
        case 0:
        {
            Vertex* v = dynamic_cast<Vertex*>(ge);
            sh = v->getOCCVertex();
            break;
        }
        case 1:
        {
            Curve* c = dynamic_cast<Curve*>(ge);
            if (c->getOCCEdges().size() != 1)
                throw TkUtil::Exception("Opération non réalisable sur une courbe composée : " + c->getName());
            sh = c->getOCCEdges()[0];
            break;
        }
        case 2:
        {
            Surface* s = dynamic_cast<Surface*>(ge);
            if (s->getOCCFaces().size() != 1)
                throw TkUtil::Exception("Opération non réalisable sur une surface composée : " + s->getName());
            sh = s->getOCCFaces()[0];
            break;
        }
        case 3:
        {
            Volume* v = dynamic_cast<Volume*>(ge);
            sh = v->getOCCShape();
            break;
        }
        default:
            throw TkUtil::Exception("Opération non réalisable sur cette entité : " + ge->getName());
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
