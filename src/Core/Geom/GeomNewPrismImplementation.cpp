/*----------------------------------------------------------------------------*/
/*
 * GeomNewPrismImplementation.cpp
 *
 *  Created on: 22 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <map>
#include <set>
#include <algorithm>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomNewPrismImplementation.h"
#include "Geom/PropertyPrism.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS.hxx>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepFeat_Gluer.hxx>
#include <BRep_Builder.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeFix_Shape.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomNewPrismImplementation::
GeomNewPrismImplementation(Internal::Context& c, PropertyPrism* prop)
:GeomModificationBaseClass(c),m_prop(prop)
{
    std::vector<GeomEntity*> es;
    es.push_back(m_prop->getBase());
    init(es);
}
/*----------------------------------------------------------------------------*/
GeomNewPrismImplementation::~GeomNewPrismImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomNewPrismImplementation::prePerform()
{}
/*----------------------------------------------------------------------------*/
void GeomNewPrismImplementation::perform(std::vector<GeomEntity*>& res)
{
    GeomEntity* base = m_prop->getBase();
    Utils::Math::Vector paxis = m_prop->getAxis();
    gp_Vec Vx(paxis.getX(), paxis.getY(), paxis.getZ());

    if (base->getDim() == 1) {
        Curve* curve = dynamic_cast<Curve*>(base);
        if (curve->getOCCEdges().size() != 1) 
            throw TkUtil::Exception (TkUtil::UTF8String ("L'extrusion n'est pas possible sur une courbe composée : " + curve->getName(), TkUtil::Charset::UTF_8));

        TopoDS_Edge sh = curve->getOCCEdges()[0];
        BRepPrimAPI_MakePrism builder(sh, Vx);
        if (!builder.IsDone ())
            throw TkUtil::Exception (TkUtil::UTF8String ("OCC n'arrive pas à effectuer la création du prisme par extrusion", TkUtil::Charset::UTF_8));

        // on a obtenu la face souhaitee en OCC
        TopoDS_Face res_face = TopoDS::Face(builder.Shape());
        createGeomEntities(res_face);
    } else if (base->getDim() == 2){
        Surface* surf = dynamic_cast<Surface*>(base);
        if (surf->getOCCFaces().size() != 1) 
            throw TkUtil::Exception (TkUtil::UTF8String ("L'extrusion n'est pas possible sur une face composée : " + surf->getName(), TkUtil::Charset::UTF_8));

        TopoDS_Face sh = surf->getOCCFaces()[0];
        BRepPrimAPI_MakePrism builder(sh, Vx);
        if (!builder.IsDone ())
            throw TkUtil::Exception (TkUtil::UTF8String ("OCC n'arrive pas à effectuer la création du prisme par extrusion", TkUtil::Charset::UTF_8));

        // on a obtenu le volume souhaite en OCC
        TopoDS_Solid res_vol = TopoDS::Solid(builder.Shape());
        createGeomEntities(res_vol);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Impossible de créer un prisme à partir de cette base", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
