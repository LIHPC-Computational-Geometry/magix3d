/*----------------------------------------------------------------------------*/
/*
 * GeomSectionImplementation.cpp
 *
 *  Created on: 7 oct. 2013
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
#include "Geom/GeomSectionImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS.hxx>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepFeat_Gluer.hxx>
#include <BRep_Builder.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomSectionImplementation::
GeomSectionImplementation(Internal::Context& c, std::vector<GeomEntity*>& es,
        GeomEntity* t)
:GeomModificationBaseClass(c)
{

   m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());
   m_tool = t;
}
/*----------------------------------------------------------------------------*/
GeomSectionImplementation::~GeomSectionImplementation()
{}


/*----------------------------------------------------------------------------*/
void GeomSectionImplementation::prePerform()
{
    /* m_ref_entities[i] contient toutes les shapes et sous-shapes
     * passées en argument de l'opération en cours de traitement.
     *
     * On vide toutes les relations topologiques ENTRE ENTITES de m_ref_entities
     * pour les reconstruire de manière identique que l'on ait créé ou
     * modifié des entités
     */
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomSectionImplementation::perform(std::vector<GeomEntity*>& res)
{
    /* m_ref_entities[i] contient toutes les shapes et sous-shapes
     * passées en argument de l'opération en cours de traitement.
     *
     * On vide toutes les relations topologiques ENTRE ENTITES de m_ref_entities
     * pour les reconstruire de manière identique que l'on ait créé ou
     * modifié des entités
     */
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================


    int dim_ref = m_init_entities[0]->getDim();
    if(dim_ref==3){
    	sectionVolumes(res);
    }
    else if(dim_ref==2)
        sectionSurfaces(res);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("La section géométrique ne s'applique qu'aux volumes ou surfaces", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
void GeomSectionImplementation::sectionVolumes(std::vector<GeomEntity*>& res)
{
    //========================================================================
    //1 - Recuperation de la surface de coupe
    //========================================================================
    TopoDS_Shape tool_shape;
    getOCCShape(m_tool,tool_shape);
    TopoDS_Face tool_face = TopoDS::Face(tool_shape);

    //========================================================================
    // 2 - Conservation du plan de coupe réduit aux objets coupés
    //========================================================================
    // On commence par fusionner toutes les entites a couper
    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s_fuse;
    getOCCShape(e1, s_fuse);
    for(unsigned int i=1;i<m_init_entities.size();i++){
        GeomEntity* e2 = m_init_entities[i];
        TopoDS_Shape s2;
        getOCCShape(e2, s2);

        BRepAlgoAPI_Fuse fuse_operator(s_fuse,s2);
        if(fuse_operator.IsDone())
            s_fuse = fuse_operator.Shape();
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union avant coupe", TkUtil::Charset::UTF_8));
    }
    // On recupere l'intersection de la surface wf et de l'union des entites à
    // couper
    BRepAlgoAPI_Common common_operator(s_fuse,tool_face);
    if(common_operator.IsDone())
        m_restricted_section_tool = common_operator.Shape();
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'intersection avant coupe", TkUtil::Charset::UTF_8));

    //========================================================================
    //3 - Decoupe des entités
    //========================================================================
    TopoDS_Shape s;
    BRepAlgoAPI_BuilderAlgo splitter;

    TopTools_ListOfShape list_of_arguments;
    for (unsigned int i = 0; i < m_init_entities.size(); i++) {
        GeomEntity* ei = m_init_entities[i];
        TopoDS_Shape si;
        getOCCShape(ei, si);
        list_of_arguments.Append(si);
    }
    list_of_arguments.Append(m_restricted_section_tool);

    splitter.SetArguments(list_of_arguments);
    splitter.Build();
    s = splitter.Shape();

    createGeomEntities(s,true);

    res.insert(res.end(), entities_new.begin(), entities_new.end());
}
/*----------------------------------------------------------------------------*/
void GeomSectionImplementation::sectionSurfaces(std::vector<GeomEntity*>& res)
{
    //========================================================================
    //1 - Recuperation de la surface de coupe
    //========================================================================
    TopoDS_Shape tool_shape;
    getOCCShape(m_tool,tool_shape);
    TopoDS_Face tool_face = TopoDS::Face(tool_shape);

    //========================================================================
    // 2 - Conservation du plan de coupe réduit aux objets coupés
    //========================================================================
    // On commence par fusionner toutes les entites a couper

    GeomEntity* e1 = m_init_entities[0];
//    TopoDS_Shape s_fuse;
//    getOCCShape(e1, s_fuse);
//    for(unsigned int i=1;i<m_init_entities.size();i++){
//        GeomEntity* e2 = m_init_entities[i];
//        TopoDS_Shape s2;
//        getOCCShape(e2, s2);
//
//        BRepAlgoAPI_Fuse fuse_operator(s_fuse,s2);
//        if(fuse_operator.IsDone())
//            s_fuse = fuse_operator.Shape();
//        else
//            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union avant coupe", TkUtil::Charset::UTF_8);
//    }
//    // On recupere l'intersection de la surface wf et de l'union des entites à
//    // couper
//    BRepAlgoAPI_Common common_operator(s_fuse,tool_face);
//    if(common_operator.IsDone())
//        m_restricted_section_tool = common_operator.Shape();
//    else
//        throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'intersection avant coupe", TkUtil::Charset::UTF_8);

    //========================================================================
    //3 - Decoupe des entités
    //========================================================================
    TopoDS_Shape s;
    BRepAlgoAPI_BuilderAlgo splitter;
    TopTools_ListOfShape list_of_arguments;
    for (unsigned int i = 0; i < m_init_entities.size(); i++) {
        GeomEntity* ei = m_init_entities[i];
        TopoDS_Shape si;
        getOCCShape(ei, si);
        list_of_arguments.Append(si);
    }
    list_of_arguments.Append(tool_face);
    splitter.Build();
    s = splitter.Shape();


    //createGeomEntitiesBut(s,tool_shape);
    createGeomEntities(s,true);

    res.insert(res.end(), entities_new.begin(), entities_new.end());
}

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
