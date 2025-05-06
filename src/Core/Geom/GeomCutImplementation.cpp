/*----------------------------------------------------------------------------*/
#include "Geom/GeomCutImplementation.h"
#include "Geom/Volume.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepFeat_Gluer.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
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
    Volume* e1 = dynamic_cast<Volume*>(m_init_entities[0]);
    TopoDS_Shape s1 = e1->getOCCShape();

    Volume* e2 = dynamic_cast<Volume*>(m_init_entities[1]);
    TopoDS_Shape cutting_assembly = e2->getOCCShape();
    for(unsigned int i=2;i<m_init_entities.size();i++){
        Volume* e3 = dynamic_cast<Volume*>(m_init_entities[i]);
        TopoDS_Shape s3 = e3->getOCCShape();

        BRepAlgoAPI_Fuse fuse_operator(cutting_assembly, s3);
        TopoDS_Shape s;
        if(fuse_operator.IsDone())
        {
            s = fuse_operator.Shape();
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

    //Pour tous les volumes créés, on indique qu'ils proviennent du volume initial découpé
    std::vector<GeomEntity*>& replaceE1 = m_replacedEntities[e1];
    for(unsigned int i=0;i<m_newVolumes.size();i++){
        replaceE1.push_back(m_newVolumes[i]);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
