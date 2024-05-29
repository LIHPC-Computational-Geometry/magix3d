/*----------------------------------------------------------------------------*/
/** \file GeomCommonImplementation.cpp
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
#include "Geom/GeomCommonImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
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
    GeomEntity* e1 = m_init_entities[0];
    TopoDS_Shape s1;
    getOCCShape(e1, s1);
    for(unsigned int i=1;i<m_init_entities.size();i++){
        GeomEntity* e2 = m_init_entities[i];
        TopoDS_Shape s2;
        getOCCShape(e2, s2);

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
//    std::cerr<<"Nb new vertices = "<<m_newVertices.size()<<std::endl;
//    std::cerr<<"Nb new curves   = "<<m_newCurves.size()<<std::endl;
//    std::cerr<<"Nb new surfaces = "<<m_newSurfaces.size()<<std::endl;
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

//    /* mise à jour des entités géométriques et des connectivités */
//    res.insert(res.end(),new_volumes.begin(),new_volumes.end());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
