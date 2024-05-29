/*----------------------------------------------------------------------------*/
/** \file GeomFuseImplementation.cpp
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
#include "Geom/GeomFuseImplementation.h"
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

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomFuseImplementation::
GeomFuseImplementation(Internal::Context& c,std::vector<GeomEntity*> es)
: GeomModificationBaseClass(c)
{
    m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());

}
/*----------------------------------------------------------------------------*/
GeomFuseImplementation::~GeomFuseImplementation()
{

}
/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::perform(std::vector<GeomEntity*>& res)
{
    if(m_init_entities[0]->getDim()==3){
    	fuseVolumes(res);
    }
    else if(m_init_entities[0]->getDim()==2)
    	// [EB] non utilisé
        fuseSurfaces(res);
    else if(m_init_entities[0]->getDim()==1)
    	// [EB] non utilisé
        fuseCurves(res);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Les entités à fusionner doivent être des volumes, des surfaces ou des courbes", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::fuseVolumes(std::vector<GeomEntity*>& res)
{
#ifdef _DEBUG2
	std::cout<<"GeomFuseImplementation::fuseVolumes()"<<std::endl;
#endif

    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s1;
    getOCCShape(e1, s1);
    for(unsigned int i=1;i<m_init_entities.size();i++){
        GeomEntity* e2 = m_init_entities[i];
        TopoDS_Shape s2;
        getOCCShape(e2, s2);

        BRepAlgoAPI_Fuse fuse_operator(s1, s2);
        TopoDS_Shape s;
        if(fuse_operator.IsDone())
        {
            s= fuse_operator.Shape();
            ShapeAnalysis_ShapeContents cont;
            cont.Clear();
            cont.Perform(s);

            if (cont.NbSolids()<1){
                throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union: Aucun volume généré", TkUtil::Charset::UTF_8));
            }
        }
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union: Un objet de type BRepAlgoAPI_Fuse n'a pas fonctionné", TkUtil::Charset::UTF_8));

        s1 = s;
    }

//    if(new_volumes.size()==0)
//        throw TkUtil::Exception (TkUtil::UTF8String ("L'union de volumes n'a pas généré de volumes!", TkUtil::Charset::UTF_8));
    createGeomEntities(s1);
//    createGeomEntities(fuse_operator);


    //=========================================================================
    //          Gestion des groupes de volumes
    //=========================================================================
    for(unsigned int i=0;i<m_init_entities.size();i++){
        GeomEntity* ei= m_init_entities[i];
        std::vector<GeomEntity*>& rep_ei =  m_replacedEntities[ei];
        for(unsigned int j=0;j<m_newVolumes.size();j++)
            rep_ei.push_back(m_newVolumes[j]);
    }
    //=========================================================================
//    std::map<GeomEntity*, std::vector<GeomEntity*> >::iterator it = m_replacedEntities.begin();
//    while(it!=m_replacedEntities.end()){
//        GeomEntity* oldE = it->first;
//        std::vector<GeomEntity*> newE = it->second;
//        std::cout<<oldE->getName()<<" replaced by"<<std::endl;
//        for(unsigned int i=0;i<newE.size();i++)
//            std::cout<<"\t "<<newE[i]->getName()<<std::endl;
//        it++;
//    }

//    res.insert(res.end(),new_volumes.begin(),new_volumes.end());
}
/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::fuseSurfaces(std::vector<GeomEntity*>& res)
{
#ifdef _DEBUG2
	// [EB] non utilisé en dehors des google tests
	std::cout<<"GeomFuseImplementation::fuseSurfaces()"<<std::endl;
#endif

    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s1;
    getOCCShape(e1, s1);
    for(unsigned int i=1;i<m_init_entities.size();i++){
        GeomEntity* e2 = m_init_entities[i];
        TopoDS_Shape s2;
        getOCCShape(e2, s2);


        BRepAlgoAPI_Fuse fuse_operator(s1, s2);
        TopoDS_Shape s;
        if(fuse_operator.IsDone())
        {
            s= fuse_operator.Shape();
            ShapeAnalysis_ShapeContents cont;
            cont.Clear();
            cont.Perform(s);

            if (cont.NbFaces()<1){
                throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union: Aucun surface générée", TkUtil::Charset::UTF_8));
            }
        }
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union: Un objet de type BRepAlgoAPI_Fuse n'a pas fonctionné", TkUtil::Charset::UTF_8));

        s1 = s;
    }
//    if(new_volumes.size()==0)
//        throw TkUtil::Exception (TkUtil::UTF8String ("L'union de volumes n'a pas généré de volumes!", TkUtil::Charset::UTF_8));
    createGeomEntities(s1);


    //=========================================================================
    //          Gestion des groupes de surfaces
    //=========================================================================
    for(unsigned int i=0;i<m_init_entities.size();i++){
        GeomEntity* ei= m_init_entities[i];
        std::vector<GeomEntity*>& rep_ei =  m_replacedEntities[ei];
        for(unsigned int j=0;j<m_newSurfaces.size();j++)
            rep_ei.push_back(m_newSurfaces[j]);
    }
    //=========================================================================
//    std::map<GeomEntity*, std::vector<GeomEntity*> >::iterator it = m_replacedEntities.begin();
//    while(it!=m_replacedEntities.end()){
//        GeomEntity* oldE = it->first;
//        std::vector<GeomEntity*> newE = it->second;
//        std::cout<<oldE->getName()<<" replaced by"<<std::endl;
//        for(unsigned int i=0;i<newE.size();i++)
//            std::cout<<"\t "<<newE[i]->getName()<<std::endl;
//        it++;
//    }

//    res.insert(res.end(),new_volumes.begin(),new_volumes.end());
}

/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::fuseCurves(std::vector<GeomEntity*>& res)
{
#ifdef _DEBUG
	// [EB] non utilisé
	std::cout<<"GeomFuseImplementation::fuseCurves()"<<std::endl;
#endif
    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s1;
    getOCCShape(e1, s1);
    for(unsigned int i=1;i<m_init_entities.size();i++){
        GeomEntity* e2 = m_init_entities[i];
        TopoDS_Shape s2;
        getOCCShape(e2, s2);

        BRepAlgoAPI_Fuse fuse_operator(s1, s2);
        TopoDS_Shape s;
        if(fuse_operator.IsDone())
        {
            s= fuse_operator.Shape();
            ShapeAnalysis_ShapeContents cont;
            cont.Clear();
            cont.Perform(s);

            if (cont.NbC0Curves()<1){
                throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union: Aucune courbe générée", TkUtil::Charset::UTF_8));
            }
        }
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'union: Un objet de type BRepAlgoAPI_Fuse n'a pas fonctionné", TkUtil::Charset::UTF_8));

        s1 = s;
    }

    createGeomEntities(s1);
//    //=========================================================================
//    std::map<GeomEntity*, std::vector<GeomEntity*> >::iterator it = m_replacedEntities.begin();
//    while(it!=m_replacedEntities.end()){
//        GeomEntity* oldE = it->first;
//        std::vector<GeomEntity*> newE = it->second;
//        std::cout<<oldE->getName()<<" replaced by"<<std::endl;
//        for(unsigned int i=0;i<newE.size();i++)
//            std::cout<<"\t "<<newE[i]->getName()<<std::endl;
//        it++;
//    }

//    res.insert(res.end(),new_curves.begin(),new_curves.end());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
