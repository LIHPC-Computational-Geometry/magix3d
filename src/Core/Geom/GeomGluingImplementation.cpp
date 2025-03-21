/*----------------------------------------------------------------------------*/
/*
 * GeomSewingImplementation.cpp
 *
 *  Created on: 1 déc. 2011
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
#include "Geom/GeomGluingImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
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
#include <TopoDS_Builder.hxx>
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
#include <ShapeConstruct.hxx>
#include <Geom_Curve.hxx>
#include <GeomConvert.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepAlgo_Image.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomGluingImplementation::
GeomGluingImplementation(Internal::Context& c, std::vector<GeomEntity*>& es)
:GeomModificationBaseClass(c)
{
    m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());
}
/*----------------------------------------------------------------------------*/
GeomGluingImplementation::~GeomGluingImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomGluingImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomGluingImplementation::perform(std::vector<GeomEntity*>& res)
{
    /* m_ref_entities[i] contient toutes les shapes et sous-shapes
     * passées en argument de l'opération en cours de traitement.
     *
     * On vide toutes les relations topologiques ENTRE ENTITES de m_ref_entities
     * pour les reconstruire de manière identique que l'on ait créé ou
     * modifié des entités
     */

    int dim_ref = m_init_entities[0]->getDim();
    if(dim_ref==3){
    	sewVolumes(res);
    }

    res.insert(res.end(),entities_new.begin(),entities_new.end());
}
/*----------------------------------------------------------------------------*/
void GeomGluingImplementation::sewVolumes(std::vector<GeomEntity*>& res)
{
	//on conserve dans un vecteur les shapes OCC des objets M3D initiaux
	BRepAlgoAPI_BuilderAlgo splitter;
	TopTools_ListOfShape list_of_arguments;
	for(unsigned int i=0;i<m_init_entities.size();i++){
		Volume* ei = dynamic_cast<Volume*>(m_init_entities[i]);
		TopoDS_Shape si = ei->getOCCShape();
		list_of_arguments.Append(si);
	}
	splitter.SetArguments(list_of_arguments);
	splitter.Build();
	TopoDS_Shape s1 = splitter.Shape();

	createGeomEntities(s1,true);
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
