/*----------------------------------------------------------------------------*/
#include "Geom/GeomCommonImplementation.h"
#include "Geom/Volume.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomCommonImplementation::
GeomCommonImplementation(Internal::Context& c,std::vector<GeomEntity*> es)
: GeomModificationBaseClass(c)
{
    m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());

}
/*----------------------------------------------------------------------------*/
GeomCommonImplementation::~GeomCommonImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomCommonImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomCommonImplementation::perform(std::vector<GeomEntity*>& res)
{
	commonVolumes(res);
}
/*----------------------------------------------------------------------------*/
void GeomCommonImplementation::commonVolumes(std::vector<GeomEntity*>& res)
{
    Volume* e1 = dynamic_cast<Volume*>(m_init_entities[0]);
    TopoDS_Shape s1 = e1->getOCCShape();
    for(unsigned int i=1;i<m_init_entities.size();i++){
        Volume* e2 = dynamic_cast<Volume*>(m_init_entities[i]);
        TopoDS_Shape s2 = e2->getOCCShape();

        BRepAlgoAPI_Common Common_operator(s1, s2);
        TopoDS_Shape s;
        if(Common_operator.IsDone())
        {
            s = Common_operator.Shape();
            ShapeAnalysis_ShapeContents cont;
            cont.Clear();
            cont.Perform(s);

//            if (cont.NbSolids()<1){
//                throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'intersection", TkUtil::Charset::UTF_8);
            //}
        }
        else
          throw TkUtil::Exception (TkUtil::UTF8String ("Problème OCC lors de l'intersection", TkUtil::Charset::UTF_8));

//        for (unsigned int i = 0; i < m_init_entities.size(); i++) {
//            GeomEntity* ei = m_init_entities[i];
//            TopoDS_Shape si;
//            getOCCShape(ei, si);
//            TopTools_ListOfShape genSh;
//            genSh = Common_operator.Generated(si) ;
//            TopTools_ListOfShape modSh;
//            modSh = Common_operator.Modified(si);
//            std::cerr<<"COM - NB Shapes generated from "<<ei->getName()<<" : "<<genSh.Extent()<<std::endl;
//            std::cerr<<"COM - NB Shapes modified from  "<<ei->getName()<<" : "<<modSh.Extent()<<std::endl;
//        }

        s1=s;
    }

    createGeomEntities(s1);

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
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
