/*
 * GeomNewVolumeImplementation.cpp
 *
 *  Created on: 3 nov. 2014
 *      Author: ledouxf
 */

#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/GeomNewVolumeImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>
/*----------------------------------------------------------------------------*/

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

GeomNewVolumeImplementation::
GeomNewVolumeImplementation(Internal::Context& c, std::vector<Surface*>& es)
:GeomModificationBaseClass(c)
{
    m_surfaces_param.insert(m_surfaces_param.end(),es.begin(),es.end());
}
/*----------------------------------------------------------------------------*/
GeomNewVolumeImplementation::~GeomNewVolumeImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomNewVolumeImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    std::vector<GeomEntity*> entities;
    entities.resize(m_surfaces_param.size());
    for(unsigned int i=0;i<m_surfaces_param.size();i++)
        entities[i] = m_surfaces_param[i];
    init(entities);
}
/*----------------------------------------------------------------------------*/
void GeomNewVolumeImplementation::perform(std::vector<GeomEntity*>& res)
{
    BRep_Builder B;
    TopoDS_Compound compound;
    B.MakeCompound(compound);

    for(unsigned int i=0; i<m_surfaces_param.size();i++)
    {
        Surface* ei = m_surfaces_param[i];
        TopoDS_Shape si;
        getOCCShape(ei, si);
        B.Add(compound,si);
    }

    /*ShHealOper_Sewing shellMaker(compound, 1e-4);

    shellMaker.Perform();

    TopoDS_Shape s1 = shellMaker.GetResultShape();

    if (s1.ShapeType()==TopAbs_SHELL){
        BRepBuilderAPI_MakeSolid mkSolid;
        mkSolid.Add(TopoDS::Shell(s1));
        TopoDS_Shape result = mkSolid.Shape();
        createGeomEntities(result,true);
    }
    else*/
        throw TkUtil::Exception (TkUtil::UTF8String ("Impossible de crer un volume a partir de ces surfaces", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

