/*----------------------------------------------------------------------------*/
/*
 * \file Group1D.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
#include "Group/Group1D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Curve.h"
#include "Utils/SerializedRepresentation.h"

#include <TkUtil/NumericConversions.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group1D::typeNameGroup1D = "Group1D";
/*----------------------------------------------------------------------------*/
Group1D::Group1D(Internal::Context& ctx,
        const std::string & nom,
		   bool isDefaultGroup, uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group1D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group1D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group1D::~Group1D()
{
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Group1D & g)
{
    o << g.getName()
      << " (uniqueId "<<g.getUniqueId()<<")"
      << (g.isDestroyed()?" (DETRUIT)":"");
    o << (g.isVisible()?" visible":" caché");
    if (g.getCurves().empty())
        o << " vide.";
    o << "\n";

    const std::vector<Geom::Curve*>& crv = g.getCurves();
    for (std::vector<Geom::Curve*>::const_iterator iter1 = crv.begin();
            iter1 != crv.end(); ++iter1){
        const std::vector<Topo::TopoEntity* >& topo = (*iter1)->getRefTopo();

        o<<"  "<<(*iter1)->getName()<<" ->";
        for (std::vector<Topo::TopoEntity* >::const_iterator iter2 = topo.begin();
                iter2 != topo.end(); ++iter2)
            o<<" "<<(*iter2)->getName();
        o << "\n";
    }

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const Group1D & g)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << g;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
std::string Group1D::getInfos()
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << *this;
    return us.iso();
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> Group1D::getEntities() const
{
//	return Internal::entitiesFromTypedEntities (getCurves ( ));
return std::vector<Utils::Entity*> ( );
}
/*----------------------------------------------------------------------------*/
void Group1D::remove(Geom::Curve* crv, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_curves.size() && crv != m_curves[i]; ++i)
        ;

    if (i!=m_curves.size())
        m_curves.erase(m_curves.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<crv->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group1D::remove(Topo::CoEdge* coedge, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_coedges.size() && coedge != m_coedges[i]; ++i)
        ;

    if (i!=m_coedges.size())
    	m_coedges.erase(m_coedges.begin()+i);
    else if(exceptionIfNotFound){
    	bool isInCurve = false;
    	if (coedge->getGeomAssociation()){
    		Geom::Curve* crv = dynamic_cast<Geom::Curve*>(coedge->getGeomAssociation());
    		if (crv && find(crv))
    			isInCurve = true;
    	}
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<coedge->getName();
        if (isInCurve)
         	messErr << "\nC'est la courbe à laquelle est associée l'arête qui appartient à ce groupe";
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group1D::add(Geom::Curve* crv)
{
	if (find(crv)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<crv->getName();
        throw TkUtil::Exception(messErr);
	}

	m_curves.push_back(crv);
}
/*----------------------------------------------------------------------------*/
void Group1D::add(Topo::CoEdge* coedge)
{
	if (find(coedge)){
	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<coedge->getName();
        throw TkUtil::Exception(messErr);
	}
	if (coedge->getGeomAssociation()){
		Geom::Curve* crv = dynamic_cast<Geom::Curve*>(coedge->getGeomAssociation());
		if (crv && find(crv)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Le groupe "<<getName()<<" possède déjà "<<crv->getName()<<" à laquelle est associée "<<coedge->getName();
			throw TkUtil::Exception(messErr);
		}
	}
	m_coedges.push_back(coedge);
}
/*----------------------------------------------------------------------------*/
bool Group1D::find(Geom::Curve* crv)
{
    uint i = 0;
    for (; i<m_curves.size() && crv != m_curves[i]; ++i)
        ;

    return (i!=m_curves.size());
}
/*----------------------------------------------------------------------------*/
bool Group1D::find(Topo::CoEdge* coedge)
{
    uint i = 0;
    for (; i<m_coedges.size() && coedge != m_coedges[i]; ++i)
        ;

    return (i!=m_coedges.size());
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group1D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    if (!m_curves.empty()){
        Utils::SerializedRepresentation curves ("Courbes géométriques",
                TkUtil::NumericConversions::toStr(m_curves.size()));
        for (std::vector<Geom::Curve*>::const_iterator its = m_curves.begin( ); m_curves.end( )!=its; its++)
            curves.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (curves);
    }

    if (!m_coedges.empty()){
        Utils::SerializedRepresentation coedges ("Arêtes topologiques",
                TkUtil::NumericConversions::toStr(m_coedges.size()));
        for (std::vector<Topo::CoEdge*>::const_iterator its = m_coedges.begin( ); m_coedges.end( )!=its; its++)
        	coedges.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (coedges);
    }

   return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
