/*----------------------------------------------------------------------------*/
#include "Geom/GeomFuseImplementation.h"
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
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepFeat_Gluer.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
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

    Volume* e1 = dynamic_cast<Volume*>(m_init_entities[0]);
    TopoDS_Shape s1 = e1->getOCCShape();
    for(unsigned int i=1;i<m_init_entities.size();i++){
        Volume* e2 = dynamic_cast<Volume*>(m_init_entities[i]);
        TopoDS_Shape s2 = e2->getOCCShape();
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
}
/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::fuseSurfaces(std::vector<GeomEntity*>& res)
{
#ifdef _DEBUG2
	// [EB] non utilisé en dehors des google tests
	std::cout<<"GeomFuseImplementation::fuseSurfaces()"<<std::endl;
#endif

    Surface* e1 = dynamic_cast<Surface*>(m_init_entities[0]);
    if (e1->getOCCFaces().size() != 1) {
        throw TkUtil::Exception (TkUtil::UTF8String ("Fusion impossible pour une surface composée " + e1->getName(), TkUtil::Charset::UTF_8));
    }
    TopoDS_Shape s1 = e1->getOCCFaces()[0];
    for(unsigned int i=1;i<m_init_entities.size();i++){
        Surface* e2 = dynamic_cast<Surface*>(m_init_entities[i]);
        if (e2->getOCCFaces().size() != 1) {
            throw TkUtil::Exception (TkUtil::UTF8String ("Fusion impossible pour une surface composée " + e2->getName(), TkUtil::Charset::UTF_8));
        }
        TopoDS_Shape s2 = e2->getOCCFaces()[0];
        BRepAlgoAPI_Fuse fuse_operator(s1, s2);
        TopoDS_Shape s;
        if(fuse_operator.IsDone())
        {
            s = fuse_operator.Shape();
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
}

/*----------------------------------------------------------------------------*/
void GeomFuseImplementation::fuseCurves(std::vector<GeomEntity*>& res)
{
#ifdef _DEBUG
	// [EB] non utilisé
	std::cout<<"GeomFuseImplementation::fuseCurves()"<<std::endl;
#endif
    Curve* e1 = dynamic_cast<Curve*>(m_init_entities[0]);
    if (e1->getOCCEdges().size() != 1) {
        throw TkUtil::Exception (TkUtil::UTF8String ("Fusion impossible pour une courbe composée " + e1->getName(), TkUtil::Charset::UTF_8));
    }
    TopoDS_Shape s1 = e1->getOCCEdges()[0];
    for(unsigned int i=1;i<m_init_entities.size();i++){
        Curve* e2 = dynamic_cast<Curve*>(m_init_entities[i]);
        if (e2->getOCCEdges().size() != 1) {
            throw TkUtil::Exception (TkUtil::UTF8String ("Fusion impossible pour une courbe composée " + e2->getName(), TkUtil::Charset::UTF_8));
        }
        TopoDS_Shape s2 = e2->getOCCEdges()[0];
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
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
