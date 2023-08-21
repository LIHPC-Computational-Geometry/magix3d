/*----------------------------------------------------------------------------*/
/** \file CommandExtrudeRevolution.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrudeRevolution.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomRevolImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExtrudeRevolution::
CommandExtrudeRevolution(Internal::Context& c,
        std::vector<GeomEntity*>& entities,
        const Utils::Math::Rotation& rot,  const bool keep)
:   CommandExtrusion(c, "Revolution"),
    m_entities(entities), m_axis1(rot.getAxis1()), m_axis2(rot.getAxis2()),
    m_angle(rot.getAngle()), m_keep(keep)
{
    if (m_angle<0.0 || m_angle>360.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));

    m_impl = new GeomRevolImplementation(c,m_entities,m_axis1,m_axis2,m_angle,m_keep);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Révolution de";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandExtrudeRevolution::~CommandExtrudeRevolution()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeRevolution::
internalExecute()
{
    m_impl->prePerform();
    m_impl->perform(m_createdEntities,m_v2v, m_v2v_opp,m_v2c,
            m_c2c, m_c2c_opp, m_c2s, m_s2s, m_s2s_opp, m_s2v);
    std::vector<GeomEntity*>& new_entities = m_impl->getNewEntities();
    std::vector<GeomEntity*>& rem_entities = m_impl->getRemovedEntities();
    for(int i=0;i<new_entities.size();i++){
        GeomEntity* ge = new_entities[i];
        getInfoCommand ( ).addGeomInfoEntity (ge, Internal::InfoCommand::CREATED);
        getContext().getLocalGeomManager().addEntity(ge);
    }
    for(int i=0;i<rem_entities.size();i++){
        getInfoCommand ( ).addGeomInfoEntity (rem_entities[i], Internal::InfoCommand::DELETED);
    }

    getInfoCommand().setDestroyAndUpdateConnectivity(rem_entities);

    // transmet les groupes du 2D vers le 3D
    groups2DTo3D();

    // ajoute les groupes PZ0 et PY0 suivant la rotation, et les sous-groupes associés
    if (m_angle == 90){
        prefixGroupsName("PZ0", m_v2v, m_c2c, m_s2s);
        prefixGroupsName("PY0", m_v2v_opp, m_c2c_opp, m_s2s_opp);
        addGroupOnAxis();
    }
    else if (m_angle == 180){
        prefixGroupsName("PZ0", m_v2v, m_c2c, m_s2s);
        prefixGroupsName("PZ0", m_v2v_opp, m_c2c_opp, m_s2s_opp);
        addGroupOnAxis();
    }
    else if (m_angle == 72){ // révolution d'un cinquième
        prefixGroupsName("PZ0", m_v2v, m_c2c, m_s2s);
        prefixGroupsName("P72", m_v2v_opp, m_c2c_opp, m_s2s_opp);
        addGroupOnAxis();
    }
    for(int i=0;i<new_entities.size();i++){
    	GeomEntity* ge = new_entities[i];
    	if (ge->getNbGroups() == 0)
    		// ajoute à un groupe par défaut
    		addToGroup(ge);
    }
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeRevolution::
addGroupOnAxis()
{
    // le groupe pour les courbes sur l'axe
    Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D("AXE", &getInfoCommand());

    // on commence par identifier les sommets sur l'axe, on les marque à 2, sinon 1
    std::map<Geom::Vertex* ,uint> filtre_vertices;

    // il semblerait que les sommets sur l'axe sont ceux de départ qui n'apparaissent pas dans m_v2v_opp
    for (std::map<Geom::Vertex* ,Geom::Vertex*>::iterator iter = m_v2v.begin();
            iter != m_v2v.end(); ++iter){
        Geom::Vertex* vtx1 = iter->first;
        Geom::Vertex* vtx2 = iter->second;
        Geom::Vertex* vtx3 = m_v2v_opp[vtx1];
        if (0 == vtx3)
            filtre_vertices[vtx1] = 2;
        else
            filtre_vertices[vtx1] = 1;
    }

    for (std::map<Geom::Curve*  ,Geom::Curve*>::iterator iter = m_c2c.begin();
            iter != m_c2c.end(); ++iter){
        Geom::Curve* crv1 = iter->first;
        Geom::Curve* crv2 = iter->second;
        std::vector<Vertex*> vertices;
        crv1->get(vertices);
        //std::cout<<"observe "<<crv1->getName()<<" entre "<<vertices[0]->getName()<<" et "<<vertices[1]->getName()<<std::endl;
        if (vertices.size()==2
                && filtre_vertices[vertices[0]]==2
                && filtre_vertices[vertices[1]]==2
                && crv1->isLinear())
        {
            //std::cout<<crv1->getName()<< " sur l'axe, et donne "<<crv2->getName()<<std::endl;
            crv2->add(grp);
            grp->add(crv2);
        }
    }

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


















