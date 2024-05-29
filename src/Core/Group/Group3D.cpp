/*----------------------------------------------------------------------------*/
/*
 * \file Group3D.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
#include "Group/Group3D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Volume.h"
#include "Mesh/Volume.h"
#include "Utils/SerializedRepresentation.h"

#include <TkUtil/NumericConversions.h>
#include <TkUtil/WarningLog.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group3D::typeNameGroup3D = "Group3D";
/*----------------------------------------------------------------------------*/
Group3D::Group3D(Internal::Context& ctx,
        const std::string & nom,
		   bool isDefaultGroup, uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group3D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group3D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group3D::~Group3D()
{
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Group3D & g)
{
    o << g.getName()
      << " (uniqueId "<<g.getUniqueId()<<")"
      << (g.isDestroyed()?" (DETRUIT)":"");
    o << (g.isVisible()?" visible":" caché");
    if (g.getVolumes().empty())
        o << " vide.";
    o << "\n";

    const std::vector<Geom::Volume*>& vol = g.getVolumes();
    for (std::vector<Geom::Volume*>::const_iterator iter1 = vol.begin();
            iter1 != vol.end(); ++iter1){
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
std::ostream & operator << (std::ostream & o, const Group3D & g)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << g;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> Group3D::getEntities() const
{
	std::vector<Utils::Entity*> result1 = Internal::entitiesFromTypedEntities (getVolumes ( ));
	std::vector<Utils::Entity*> result2 = Internal::entitiesFromTypedEntities (getBlocks ( ));
	result1.insert(result1.end(), result2.begin(), result2.end());
	return result1;
}
/*----------------------------------------------------------------------------*/
std::string Group3D::getInfos()
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << *this;
    return us.iso();
}
/*----------------------------------------------------------------------------*/
void Group3D::remove(Geom::Volume* vol, const bool exceptionIfNotFound)
{
	//std::cout<<"Group3D::remove("<<vol->getName()<<") à "<<getName()<<std::endl;

    uint i = 0;
    for (; i<m_volumes.size() && vol != m_volumes[i]; ++i)
        ;

    if (i!=m_volumes.size())
        m_volumes.erase(m_volumes.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<vol->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group3D::remove(Topo::Block* bloc, const bool exceptionIfNotFound)
{
	//std::cout<<"Group3D::remove("<<vol->getName()<<") à "<<getName()<<std::endl;

    uint i = 0;
    for (; i<m_blocks.size() && bloc != m_blocks[i]; ++i)
        ;

    if (i!=m_blocks.size())
    	m_blocks.erase(m_blocks.begin()+i);
    else if(exceptionIfNotFound){
    	bool isInVolume = false;
    	if (bloc->getGeomAssociation()){
    		Geom::Volume* vol = dynamic_cast<Geom::Volume*>(bloc->getGeomAssociation());
    		if (vol && find(vol))
    			isInVolume = true;
    	}
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<bloc->getName();
        if (isInVolume)
        	messErr << "\nC'est le volume auquel est associé le bloc qui appartient à ce groupe";
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group3D::remove(Mesh::Volume* vol, const bool exceptionIfNotFound)
{
	//std::cout<<"Group3D::remove("<<vol->getName()<<") à "<<getName()<<std::endl;

    uint i = 0;
    for (; i<m_mesh_volumes.size() && vol != m_mesh_volumes[i]; ++i)
        ;

    if (i!=m_mesh_volumes.size())
    	m_mesh_volumes.erase(m_mesh_volumes.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<vol->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group3D::remove(CoordinateSystem::SysCoord* rep, const bool exceptionIfNotFound)
{
    uint i = 0;
    for (; i<m_sys_coord.size() && rep != m_sys_coord[i]; ++i)
        ;

    if (i!=m_sys_coord.size())
    	m_sys_coord.erase(m_sys_coord.begin()+i);
    else if(exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<rep->getName();
        throw TkUtil::Exception(messErr);
    }
}
/*----------------------------------------------------------------------------*/
void Group3D::add(Geom::Volume* vol)
{
	if (find(vol)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<vol->getName();
        throw TkUtil::Exception(messErr);
	}

	if (!m_mesh_volumes.empty()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà un maillage importé";
        throw TkUtil::Exception(messErr);
	}

	//std::cout<<"Group3D::add("<<vol->getName()<<") à "<<getName()<<std::endl;
    m_volumes.push_back(vol);
}
/*----------------------------------------------------------------------------*/
void Group3D::add(Topo::Block* bloc)
{
	if (find(bloc)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<bloc->getName();
        throw TkUtil::Exception(messErr);
	}
	if (bloc->getGeomAssociation()){
		Geom::Volume* vol = dynamic_cast<Geom::Volume*>(bloc->getGeomAssociation());
		if (vol && find(vol)){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Le groupe "<<getName()<<" possède déjà "<<vol->getName()<<" auquel est associé "<<bloc->getName();
			throw TkUtil::Exception(messErr);
		}
	}
	if (!m_volumes.empty()){
		TkUtil::UTF8String  warningText (TkUtil::Charset::UTF_8);
		warningText<<"ATTENTION, vous ajoutez le bloc "<<bloc->getName()
				<<" au groupe "<<getName()
				<<" qui possède déjà des volumes, il faudrait peut-être plutôt associé ce bloc à un volume, et mettre ce volume dans ce groupe si ce n'est pas déjà le cas";
		getStdContext()->getLogStream()->log(TkUtil::WarningLog (warningText));

	}
	if (!m_mesh_volumes.empty()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà un maillage importé";
        throw TkUtil::Exception(messErr);
	}

	//std::cout<<"Group3D::add("<<vol->getName()<<") à "<<getName()<<std::endl;
    m_blocks.push_back(bloc);
}
/*----------------------------------------------------------------------------*/
void Group3D::add(Mesh::Volume* vol)
{
	if (find(vol)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<vol->getName();
        throw TkUtil::Exception(messErr);
	}

	if (!m_volumes.empty() || !m_blocks.empty()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà une topologie, on ne peut y ajouter un maillage importé";
        throw TkUtil::Exception(messErr);
	}

	//std::cout<<"Group3D::add("<<vol->getName()<<") à "<<getName()<<std::endl;
    m_mesh_volumes.push_back(vol);
}
/*----------------------------------------------------------------------------*/
void Group3D::add(CoordinateSystem::SysCoord* rep)
{
	if (find(rep)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<rep->getName();
        throw TkUtil::Exception(messErr);
	}

	//std::cout<<"Group3D::add("<<rep->getName()<<") à "<<getName()<<std::endl;
	m_sys_coord.push_back(rep);
}
/*----------------------------------------------------------------------------*/
bool Group3D::find(Geom::Volume* vol)
{
    uint i = 0;
    for (; i<m_volumes.size() && vol != m_volumes[i]; ++i)
        ;

    return (i!=m_volumes.size());
}
/*----------------------------------------------------------------------------*/
bool Group3D::find(Topo::Block* bloc)
{
    uint i = 0;
    for (; i<m_blocks.size() && bloc != m_blocks[i]; ++i)
        ;

    return (i!=m_blocks.size());
}
/*----------------------------------------------------------------------------*/
bool Group3D::find(Mesh::Volume* vol)
{
    uint i = 0;
    for (; i<m_mesh_volumes.size() && vol != m_mesh_volumes[i]; ++i)
        ;

    return (i!=m_mesh_volumes.size());
}
/*----------------------------------------------------------------------------*/
bool Group3D::find(CoordinateSystem::SysCoord* rep)
{
    uint i = 0;
    for (; i<m_sys_coord.size() && rep != m_sys_coord[i]; ++i)
        ;

    return (i!=m_sys_coord.size());
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group3D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    if (!m_volumes.empty()){
        Utils::SerializedRepresentation volumes ("Volumes géométriques",
                TkUtil::NumericConversions::toStr(m_volumes.size()));
        for (std::vector<Geom::Volume*>::const_iterator its = m_volumes.begin( ); m_volumes.end( )!=its; its++)
            volumes.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (volumes);
    }

    if (!m_blocks.empty()){
        Utils::SerializedRepresentation blocks ("Blocs topologiques",
                TkUtil::NumericConversions::toStr(m_blocks.size()));
        for (std::vector<Topo::Block*>::const_iterator its = m_blocks.begin( ); m_blocks.end( )!=its; its++)
        	blocks.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (blocks);
    }

    // recherche du Mesh::Volume associé (suivant le nom)
    Mesh::Volume* mvol = getContext().getLocalMeshManager().getVolume(getName(), false);
    if (mvol){
        Utils::SerializedRepresentation volumes ("Volume maillage", std::string("1"));
        volumes.addProperty (
                Utils::SerializedRepresentation::Property (mvol->getName(), *mvol));

        description->addPropertiesSet (volumes);
    } else {
        description->addProperty (
                Utils::SerializedRepresentation::Property (
                        "Volume maillage", std::string("Aucun")));
    }

    if (!m_sys_coord.empty()){
        Utils::SerializedRepresentation reperes ("Repères",
                TkUtil::NumericConversions::toStr(m_sys_coord.size()));
        for (std::vector<CoordinateSystem::SysCoord*>::const_iterator its = m_sys_coord.begin( ); m_sys_coord.end( )!=its; its++)
        	reperes.addProperty (
                    Utils::SerializedRepresentation::Property (
                            (*its)->getName ( ), *(*its)));
        description->addPropertiesSet (reperes);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
