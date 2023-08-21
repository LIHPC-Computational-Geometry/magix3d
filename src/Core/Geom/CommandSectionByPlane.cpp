/*
 * CommandSectionByPlane.cpp
 *
 *  Created on: 24 june 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandSectionByPlane.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomSectionByPlaneImplementation.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Group/GroupManager.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Volume;
/*----------------------------------------------------------------------------*/
CommandSectionByPlane::
CommandSectionByPlane(Internal::Context& c,std::vector<GeomEntity*>& entities,
        Utils::Math::Plane* p,
        std::string planeGroupName)
: CommandEditGeom(c, "Section par un plan",""),
  m_entities(entities), m_tool(p), m_planeName(planeGroupName)
{
    validate();

    m_impl = new GeomSectionByPlaneImplementation(c, m_entities,p);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Section par un plan de";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	comments << " suivant "<<*m_tool;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSectionByPlane::~CommandSectionByPlane()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandSectionByPlane::validate()
{
#ifdef _DEBUG2
    std::cout<<"CommandSectionByPlane::validate() m_entities.size() = "<<m_entities.size()<<std::endl;
#endif
    int nbDim2=0;
    int nbDim3=0;
    for(unsigned int i=0;i<m_entities.size();i++)
    {
        int dim_i =m_entities[i]->getDim();
        if(dim_i==2)
            nbDim2++;
        else if (dim_i==3)
            nbDim3++;
    }
    if(nbDim2!=m_entities.size() && nbDim3!=m_entities.size())
        throw TkUtil::Exception(TkUtil::UTF8String ("La coupe par un plan ne s'applique qu'à un ensemble de volumes ou un ensemble de surfaces", TkUtil::Charset::UTF_8));

    if(nbDim2==m_entities.size()){
        short nbSurfConnected2Vol=0;
        std::vector<GeomEntity*> validEntities;
        for(unsigned int i=0;i<m_entities.size();i++)
        {
            std::vector<Volume*> vols;
            m_entities[i]->get(vols);
            if(!vols.empty())
            {
                nbSurfConnected2Vol++;
            }
            else validEntities.push_back(m_entities[i]);

        }
        m_entities = validEntities;
        if(nbSurfConnected2Vol!=0)
        	throw TkUtil::Exception(TkUtil::UTF8String ("Surfaces non traitées car connectées à des volumes\n(=> couper les volumes)", TkUtil::Charset::UTF_8));
    }

    if (nbDim2)
        setDimensionGroup(1);
    else if (nbDim3)
        setDimensionGroup(2);
#ifdef _DEBUG2
    std::cout<<"  en sortie ==> m_entities.size() = "<<m_entities.size()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSectionByPlane::removeNonIntersectedEntities()
{
    std::vector<GeomEntity*> cleanEntities;
    for(unsigned int i=0;i<m_entities.size();i++)
    {
        GeomEntity* ei = m_entities[i];
        double bounds[6];
        ei->getBounds(bounds);

        double xmin = bounds[0];
        double xmax = bounds[1];
        double ymin = bounds[2];
        double ymax = bounds[3];
        double zmin = bounds[4];
        double zmax = bounds[5];

        // si tous les sommets de la BB sont du même côté du plan de coupe
        // on ne traite pas cet objet geometrique
        Utils::Math::Point p1(xmin,ymin,zmin);
        Utils::Math::Point p2(xmax,ymin,zmin);
        Utils::Math::Point p3(xmax,ymax,zmin);
        Utils::Math::Point p4(xmin,ymax,zmin);
        Utils::Math::Point p5(xmin,ymin,zmax);
        Utils::Math::Point p6(xmax,ymin,zmax);
        Utils::Math::Point p7(xmax,ymax,zmax);
        Utils::Math::Point p8(xmin,ymax,zmax);

        Utils::Math::Point plane_pnt = m_tool->getPoint();
        Utils::Math::Vector plane_vec = m_tool->getNormal();

        Utils::Math::Vector v[8];
        v[0]=Utils::Math::Vector(plane_pnt,p1);
        v[1]=Utils::Math::Vector(plane_pnt,p2);
        v[2]=Utils::Math::Vector(plane_pnt,p3);
        v[3]=Utils::Math::Vector(plane_pnt,p4);
        v[4]=Utils::Math::Vector(plane_pnt,p5);
        v[5]=Utils::Math::Vector(plane_pnt,p6);
        v[6]=Utils::Math::Vector(plane_pnt,p7);
        v[7]=Utils::Math::Vector(plane_pnt,p8);

        int sidePos=0, sideNeg=0;
        for(short j=0;j<8;j++){
            if(plane_vec.dot(v[j])>0.0)
                sidePos++;
            else
                sideNeg++;
        }
        if(sidePos!=0 && sideNeg!=0)
            cleanEntities.push_back(ei);

    }
    if(cleanEntities.size()!=m_entities.size()){
#ifdef _DEBUG2
    std::cout<<"CommandSectionByPlane::removeNonIntersectedEntities() m_entities.size() = "
    		<<m_entities.size()<<" et cleanEntities.size() = "<<cleanEntities.size()<<std::endl;
#endif
       m_entities.clear();
        m_entities.insert(m_entities.end(),cleanEntities.begin(),cleanEntities.end());
    }
}
/*----------------------------------------------------------------------------*/
void CommandSectionByPlane::internalSpecificExecute()
{
#ifdef _DEBUG2
    std::cout<<"CommandSectionByPlane::internalSpecificExecute() avec m_planeName = "<<m_planeName
    		<<" m_entities.size() = "<<m_entities.size()<<std::endl;
#endif

    if(m_entities.empty())
        return;

    removeNonIntersectedEntities();
    m_impl->perform(m_createdEntities);

// [EB] inutile (et fait double emploi) depuis utilisation de CommandEditGeom::updateGroups
    //on place dans le groupe les entites intersectees
    GeomSectionByPlaneImplementation* impl = dynamic_cast<GeomSectionByPlaneImplementation*>(m_impl);
    CHECK_NULL_PTR_ERROR(impl);
    std::vector<GeomEntity*> planeEntities = impl->getEntitiesOnPlane();
    if (!m_planeName.empty())
    	for(unsigned int i=0;i<planeEntities.size();i++) {
    		GeomEntity* e = planeEntities[i];
    		if(e->getDim()==0){
    			Vertex *v = dynamic_cast<Vertex*>(e);
    			CHECK_NULL_PTR_ERROR(v);
    			Group::Group0D* group = getContext().getLocalGroupManager().getNewGroup0D(m_planeName, &getInfoCommand());
    			v->add(group);
    			group->add(v);
    			getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
    		}
    		else if(e->getDim()==1){
    			Curve *c = dynamic_cast<Curve*>(e);
    			CHECK_NULL_PTR_ERROR(c);
    			Group::Group1D* group = getContext().getLocalGroupManager().getNewGroup1D(m_planeName, &getInfoCommand());
    			c->add(group);
    			group->add(c);
    			getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
    		}
    		if(e->getDim()==2){
    			Surface *s = dynamic_cast<Surface*>(e);
    			CHECK_NULL_PTR_ERROR(s);
    			Group::Group2D* group = getContext().getLocalGroupManager().getNewGroup2D(m_planeName, &getInfoCommand());
    			s->add(group);
    			group->add(s);
    			getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
    		}
    	}

}

/*----------------------------------------------------------------------------*/
void CommandSectionByPlane::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
