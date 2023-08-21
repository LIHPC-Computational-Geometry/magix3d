/*----------------------------------------------------------------------------*/
/*
 * \file Group2D.cpp
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
#include "Group/Group2D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Surface.h"
#include "Utils/SerializedRepresentation.h"
#include "Mesh/Surface.h"

#include <TkUtil/NumericConversions.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group2D::typeNameGroup2D = "Group2D";
/*----------------------------------------------------------------------------*/
Group2D::Group2D(Internal::Context& ctx,
		const std::string & nom,
		bool isDefaultGroup,
		uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group2D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group2D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group2D::~Group2D()
{
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Group2D & g)
{
    o << g.getName()
      << " (uniqueId "<<g.getUniqueId()<<")"
      << (g.isDestroyed()?" (DETRUIT)":"");
    o << (g.isVisible()?" visible":" caché");
    if (g.getSurfaces().empty())
        o << " vide.";
    o << "\n";

    const std::vector<Geom::Surface*>& surf = g.getSurfaces();
    for (std::vector<Geom::Surface*>::const_iterator iter1 = surf.begin();
            iter1 != surf.end(); ++iter1){
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
std::ostream & operator << (std::ostream & o, const Group2D & g)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << g;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> Group2D::getEntities() const
{
//	return Internal::entitiesFromTypedEntities (getSurfaces ( ));
return std::vector<Utils::Entity*> ( );
}
/*----------------------------------------------------------------------------*/
std::string Group2D::getInfos()
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << *this;
    return us.iso();
}
/*----------------------------------------------------------------------------*/
void Group2D::remove(Geom::Surface* surf, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_surfaces.size() && surf != m_surfaces[i]; ++i)
        ;

    if (i!=m_surfaces.size())
        m_surfaces.erase(m_surfaces.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<surf->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group2D::remove(Topo::CoFace* coface, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_cofaces.size() && coface != m_cofaces[i]; ++i)
        ;

    if (i!=m_cofaces.size())
    	m_cofaces.erase(m_cofaces.begin()+i);
    else if(exceptionIfNotFound){
    	bool isInSurface = false;
    	if (coface->getGeomAssociation()){
    		Geom::Surface* surf = dynamic_cast<Geom::Surface*>(coface->getGeomAssociation());
    		if (surf && find(surf))
    			isInSurface = true;
    	}

		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<coface->getName();
        if (isInSurface)
         	messErr << "\nC'est la surface à laquelle est associée la face qui appartient à ce groupe";
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group2D::remove(Mesh::Surface* surf, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_mesh_surfaces.size() && surf != m_mesh_surfaces[i]; ++i)
        ;

    if (i!=m_mesh_surfaces.size())
    	m_mesh_surfaces.erase(m_mesh_surfaces.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<surf->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group2D::add(Geom::Surface* surf)
{
	if (find(surf)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<surf->getName();
        throw TkUtil::Exception(messErr);
	}

	if (!m_mesh_surfaces.empty()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà un maillage importé";
        throw TkUtil::Exception(messErr);
	}

	m_surfaces.push_back(surf);
}
/*----------------------------------------------------------------------------*/
void Group2D::add(Topo::CoFace* coface)
{
	if (find(coface)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<coface->getName();
        throw TkUtil::Exception(messErr);
	}
	if (coface->getGeomAssociation()){
		Geom::Surface* surf = dynamic_cast<Geom::Surface*>(coface->getGeomAssociation());
		if (surf && find(surf)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Le groupe "<<getName()<<" possède déjà "<<surf->getName()<<" à laquelle est associée "<<coface->getName();
			throw TkUtil::Exception(messErr);
		}
	}
	if (!m_mesh_surfaces.empty()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà un maillage importé";
        throw TkUtil::Exception(messErr);
	}

  m_cofaces.push_back(coface);
}
/*----------------------------------------------------------------------------*/
void Group2D::add(Mesh::Surface* surf)
{
	if (find(surf)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<surf->getName();
        throw TkUtil::Exception(messErr);
	}

	if (!m_surfaces.empty() || !m_cofaces.empty()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà une topologie, on ne peut y ajouter un maillage importé";
        throw TkUtil::Exception(messErr);
	}

   m_mesh_surfaces.push_back(surf);
}
/*----------------------------------------------------------------------------*/
bool Group2D::find(Geom::Surface* surf)
{
    uint i = 0;
    for (; i<m_surfaces.size() && surf != m_surfaces[i]; ++i)
        ;

    return (i!=m_surfaces.size());
}
/*----------------------------------------------------------------------------*/
bool Group2D::find(Topo::CoFace* coface)
{
    uint i = 0;
    for (; i<m_cofaces.size() && coface != m_cofaces[i]; ++i)
        ;

    return (i!=m_cofaces.size());
}
/*----------------------------------------------------------------------------*/
bool Group2D::find(Mesh::Surface* surf)
{
    uint i = 0;
    for (; i<m_mesh_surfaces.size() && surf != m_mesh_surfaces[i]; ++i)
        ;

    return (i!=m_mesh_surfaces.size());
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group2D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    if (!m_surfaces.empty()){
        Utils::SerializedRepresentation surfaces ("Surfaces géométriques",
                TkUtil::NumericConversions::toStr(m_surfaces.size()));
        for (std::vector<Geom::Surface*>::const_iterator its = m_surfaces.begin( ); m_surfaces.end( )!=its; its++)
            surfaces.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (surfaces);
    }

    if (!m_cofaces.empty()){
        Utils::SerializedRepresentation cofaces ("Faces topologiques",
                TkUtil::NumericConversions::toStr(m_cofaces.size()));
        for (std::vector<Topo::CoFace*>::const_iterator its = m_cofaces.begin( ); m_cofaces.end( )!=its; its++)
        	cofaces.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (cofaces);
    }

    // recherche du Mesh::Surface associé (suivant le nom)
    Mesh::Surface* msurf = getContext().getLocalMeshManager().getSurface(getName(), false);
    if (msurf){
        Utils::SerializedRepresentation surfaces ("Surface maillage", std::string("1"));
        surfaces.addProperty (
                Utils::SerializedRepresentation::Property (msurf->getName(), *msurf));

        description->addPropertiesSet (surfaces);
    } else {
        description->addProperty (
                Utils::SerializedRepresentation::Property (
                        "Surface maillage", std::string("Aucune")));
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
