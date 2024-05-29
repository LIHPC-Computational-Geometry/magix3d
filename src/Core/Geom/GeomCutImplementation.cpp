/*----------------------------------------------------------------------------*/
/** \file GeomCutImplementation.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <map>
#include <set>
#include <algorithm>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomCutImplementation.h"
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
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS.hxx>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
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

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomCutImplementation::
GeomCutImplementation(Internal::Context& c, GeomEntity* e,
        std::vector<GeomEntity*> es)
: GeomModificationBaseClass(c)
{
    m_entities_param.push_back(e);
    m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());
}
/*----------------------------------------------------------------------------*/
GeomCutImplementation::~GeomCutImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomCutImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================

    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomCutImplementation::perform(std::vector<GeomEntity*>& res)
{
	cutVolumes(res);
}
/*----------------------------------------------------------------------------*/
void GeomCutImplementation::cutVolumes(std::vector<GeomEntity*>& res)
{
    //entite a conserver
    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s1;
    getOCCShape(e1, s1);



    GeomEntity* e2 = m_init_entities[1];

    TopoDS_Shape cutting_assembly;

    getOCCShape(e2, cutting_assembly);
    for(unsigned int i=2;i<m_init_entities.size();i++){
        GeomEntity* e3 = m_init_entities[i];
        TopoDS_Shape s3;
        getOCCShape(e3, s3);

        BRepAlgoAPI_Fuse fuse_operator(cutting_assembly, s3);
        TopoDS_Shape s;
        if(fuse_operator.IsDone())
        {
            s= fuse_operator.Shape();
            ShapeAnalysis_ShapeContents cont;
            cont.Clear();
            cont.Perform(s);

            if (cont.NbSolids()<1){
                throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union des formes à supprimer: Aucun volume généré", TkUtil::Charset::UTF_8));
            }
        }
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union des formes à supprimer: Un objet de type BRepAlgoAPI_Fuse n'a pas fonctionné", TkUtil::Charset::UTF_8));

        cutting_assembly = s;
    }


    BRepAlgoAPI_Cut Cut_operator(s1, cutting_assembly);

    TopoDS_Shape s;

    if(Cut_operator.IsDone())
    {
        s = Cut_operator.Shape();
        ShapeAnalysis_ShapeContents cont;
        cont.Clear();
        cont.Perform(s);
    }
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de la différence", TkUtil::Charset::UTF_8));

    s1 = s;

    createGeomEntities(s1,true);

//    {
//    	// parcourir les surfaces associées au volume avant la coupe (Shape1),
//    	// et afficher les HashCode des entités  Modified et autres associés aux surfaces
//    	const TopoDS_Shape& shape1 = Cut_operator.Shape1();
//    	std::cout<<" volume shape1 HashCode "<<shape1.HashCode(INT_MAX)<<std::endl;
//    	TopExp_Explorer ShapeExplorer(shape1, TopAbs_FACE);
//    	for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
//    		const TopoDS_Shape& Root = ShapeExplorer.Current ();
//    		std::cout<<"  surface HashCode "<< Root.HashCode(INT_MAX)<<std::endl;
//			if (Cut_operator.IsDeleted (Root))
//				std::cout<<"   IsDeleted"<<std::endl;
//
//    		const TopTools_ListOfShape& Shapes = Cut_operator.Modified (Root);
//    		TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
//    		for (;ShapesIterator.More (); ShapesIterator.Next ()) {
//    			const TopoDS_Shape& newShape = ShapesIterator.Value ();
//    			std::cout<<"    modifiedShape HashCode "<< newShape.HashCode(INT_MAX)<<std::endl;
//    			if (!Root.IsSame (newShape))
//    				std::cout<<"      IsSame"<<std::endl;
//    			if (!Root.IsEqual (newShape))
//    				std::cout<<"      IsEqual"<<std::endl;
//
//
//
//    		}
//    	}
//    }
//
//    {
//    	// parcourir les surfaces associées au volume qui coupe et disparait (Shape2)
//    	// et afficher les HashCode
//    	const TopoDS_Shape& shape2 = Cut_operator.Shape2();
//    	std::cout<<" volume shape2 HashCode "<<shape2.HashCode(INT_MAX)<<std::endl;
//    	TopExp_Explorer ShapeExplorer(shape2, TopAbs_FACE);
//    	for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
//    		const TopoDS_Shape& F = ShapeExplorer.Current ();
//    		std::cout<<"  surface HashCode "<< F.HashCode(INT_MAX)<<std::endl;
//
//    		if (Cut_operator.IsDeleted (F))
//    			std::cout<<"   IsDeleted"<<std::endl;
//
//    		const TopTools_ListOfShape& modified = Cut_operator.Modified (F);
//			if (!modified.IsEmpty()) {
//
//				TopTools_ListIteratorOfListOfShape itr(modified);
//				for (; itr.More (); itr.Next ()) {
//					const TopoDS_Shape& newShape = itr.Value ();
//					std::cout<<"    modifiedShape HashCode "<< newShape.HashCode(INT_MAX)<<std::endl;
//				}
//			}
//    	}
//    }
//
//
//    {
//    	// parcourir les surfaces associées au volume résultant (Shape),
//    	// et afficher les HashCode des surfaces finales
//    	TopoDS_Shape newS1 = Cut_operator.Shape();
//    	std::cout<<" volume newS1 HashCode "<<newS1.HashCode(INT_MAX)<<std::endl;
//    	TopExp_Explorer ShapeExplorer(newS1, TopAbs_FACE);
//    	for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
//    		const TopoDS_Shape& Root = ShapeExplorer.Current ();
//    		std::cout<<"  surface HashCode "<< Root.HashCode(INT_MAX)<<std::endl;
//    	}
//    }

    //Pour tous les volumes créés, on indique qu'ils proviennent du volume initial découpé
    std::vector<GeomEntity*>& replaceE1 = m_replacedEntities[e1];
    for(unsigned int i=0;i<m_newVolumes.size();i++){
        replaceE1.push_back(m_newVolumes[i]);
    }

//    std::map<GeomEntity*, std::vector<GeomEntity*> >::iterator it = m_replacedEntities.begin();
//    while(it!=m_replacedEntities.end()){
//        GeomEntity* oldE = it->first;
//        std::vector<GeomEntity*> newE = it->second;
//        std::cout<<oldE->getName()<<" replaced by"<<std::endl;
//        for(unsigned int i=0;i<newE.size();i++)
//            std::cout<<"\t "<<newE[i]->getName()<<std::endl;
//        it++;
//    }

//    // on copie les groupes de la première entité (histoire de ne pas être sans rien)
//    for(unsigned int i=0;i<new_volumes.size();i++)
//        copyGroups(m_init_entities[0], new_volumes[i]);

//    res.insert(res.end(),new_volumes.begin(),new_volumes.end());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
