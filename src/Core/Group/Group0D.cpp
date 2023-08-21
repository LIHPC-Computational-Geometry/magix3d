/*----------------------------------------------------------------------------*/
/*
 * \file Group0D.cpp
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
#include "Group/Group0D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Vertex.h"
#include "Utils/SerializedRepresentation.h"

#include <TkUtil/NumericConversions.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group0D::typeNameGroup0D = "Group0D";
/*----------------------------------------------------------------------------*/
Group0D::Group0D(Internal::Context& ctx,
        const std::string & nom,
		   bool isDefaultGroup,
		   uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group0D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group0D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group0D::~Group0D()
{
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Group0D & g)
{
    o << g.getName()
      << " (uniqueId "<<g.getUniqueId()<<")"
      << (g.isDestroyed()?" (DETRUIT)":"");
    o << (g.isVisible()?" visible":" caché");
    if (g.getVertices().empty())
        o << " vide.";
    o << "\n";

    const std::vector<Geom::Vertex*>& vtx = g.getVertices();
    for (std::vector<Geom::Vertex*>::const_iterator iter1 = vtx.begin();
            iter1 != vtx.end(); ++iter1){
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
std::ostream & operator << (std::ostream & o, const Group0D & g)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << g;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
std::string Group0D::getInfos()
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << *this;
    return us.iso();
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> Group0D::getEntities() const
{
//	return Internal::entitiesFromTypedEntities (getVertices ( ));
return std::vector<Utils::Entity*> ( );
}
/*----------------------------------------------------------------------------*/
void Group0D::remove(Geom::Vertex* vtx, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_vertices.size() && vtx != m_vertices[i]; ++i)
        ;

    if (i!=m_vertices.size())
        m_vertices.erase(m_vertices.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<vtx->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group0D::remove(Topo::Vertex* vtx, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_topo_vertices.size() && vtx != m_topo_vertices[i]; ++i)
        ;

    if (i!=m_topo_vertices.size())
    	m_topo_vertices.erase(m_topo_vertices.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<vtx->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group0D::add(Geom::Vertex* vtx)
{
	if (find(vtx)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<vtx->getName();
        throw TkUtil::Exception(messErr);
	}

    m_vertices.push_back(vtx);
}
/*----------------------------------------------------------------------------*/
void Group0D::add(Topo::Vertex* vtx)
{
	if (find(vtx)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<vtx->getName();
        throw TkUtil::Exception(messErr);
	}

    m_topo_vertices.push_back(vtx);
}
/*----------------------------------------------------------------------------*/
bool Group0D::find(Geom::Vertex* vtx)
{
    uint i = 0;
    for (; i<m_vertices.size() && vtx != m_vertices[i]; ++i)
        ;

    return (i!=m_vertices.size());
}
/*----------------------------------------------------------------------------*/
bool Group0D::find(Topo::Vertex* vtx)
{
    uint i = 0;
    for (; i<m_topo_vertices.size() && vtx != m_topo_vertices[i]; ++i)
        ;

    return (i!=m_topo_vertices.size());
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group0D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    if (!m_vertices.empty()){
        Utils::SerializedRepresentation vertices ("Sommets géométriques",
                TkUtil::NumericConversions::toStr(m_vertices.size()));
        for (std::vector<Geom::Vertex*>::const_iterator its = m_vertices.begin( ); m_vertices.end( )!=its; its++)
            vertices.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (vertices);
    }

    if (!m_topo_vertices.empty()){
        Utils::SerializedRepresentation vertices ("Sommets topologiques",
                TkUtil::NumericConversions::toStr(m_vertices.size()));
        for (std::vector<Topo::Vertex*>::const_iterator its = m_topo_vertices.begin( ); m_topo_vertices.end( )!=its; its++)
            vertices.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (vertices);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
