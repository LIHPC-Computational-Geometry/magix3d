/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include "Internal/Context.h"
#include "Mesh/MeshModificationItf.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
GroupEntity::GroupEntity(Internal::Context& ctx,
                       Utils::Property* prop,
                       Utils::DisplayProperties* disp,
					   bool isDefaultGroup,
					   uint level)
: Internal::InternalEntity (ctx, prop, disp)
, m_isDefaultGroup(isDefaultGroup)
, m_level(level)
{
#ifdef _DEBUG2
    std::cout<<"GroupEntity::GroupEntity de nom "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
GroupEntity::~GroupEntity()
{
#ifdef _DEBUG2
    std::cout<<"GroupEntity::~GroupEntity() de "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void GroupEntity::setDestroyed(bool b)
{
    // cas sans changement
    if (isDestroyed() == b)
        return;

#ifdef _DEBUG2
    std::cout<<"GroupEntity::setDestroyed("<<b<<") de "<<getName()<<std::endl;;
#endif

    Entity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
void GroupEntity::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    MGX_FORBIDDEN("Pas de représentation des entités Group...");
    throw TkUtil::Exception (TkUtil::UTF8String ("Il n'est pas prévu de représenter les entités de type Groupe", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* GroupEntity::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Mgx3D::Utils::SerializedRepresentation>   description (
    		InternalEntity::getDescription (alsoComputed));

    description->addProperty(Utils::SerializedRepresentation::Property ("Niveau", (long)getLevel()));

    if (!m_meshModif.empty()){
    	Utils::SerializedRepresentation  modification (
    			            "Modification du maillage", "");
    	for (std::vector<Mesh::MeshModificationItf*>::const_iterator iter = m_meshModif.begin();
    			iter != m_meshModif.end(); ++iter)

    		(*iter)->addToDescription(&modification);

    	description->addPropertiesSet(modification);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
void GroupEntity::remove(Utils::Entity* e, const bool exceptionIfNotFound)
{
    if (!find(e)) {
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" ne contient pas "<<e->getName();
        throw TkUtil::Exception(messErr);
    }

    m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e), m_entities.end());
}
/*----------------------------------------------------------------------------*/
void GroupEntity::add(Utils::Entity* e)
{
	if (find(e)) {
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Le groupe "<<getName()<<" possède déjà "<<e->getName();
        throw TkUtil::Exception(messErr);
	}

    // Si e est une entité de maillage, m_entities ne doit 
    // comporter que des entités de maillages. A l'inverse
    // aucune entité autre que MeshEntity ne peut être ajoutée
    // au groupe s'il contient déjà des entités de maillage
    bool eIsMeshEntity = (dynamic_cast<Mesh::MeshEntity*>(e) != nullptr);
    if (eIsMeshEntity) {
        if (m_entities.size() != getFilteredEntities<Mesh::MeshEntity>().size()) {
            TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            messErr << "Le groupe "<<getName()<<" possède déjà des entités et ne peut y ajouter un maillage importé";
            throw TkUtil::Exception(messErr);
        }
    } else {
        if (!getFilteredEntities<Mesh::MeshEntity>().empty()) {
            TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            messErr << "Le groupe "<<getName()<<" possède déjà un maillage importé et ne peut y ajouter une autre entité";
            throw TkUtil::Exception(messErr);
        }
    }

    // si e est une entité topologique associée à une entité géométrique ge
    // alors ge ne doit pas appartenir à ce groupe
    Topo::TopoEntity* topo = dynamic_cast<Topo::TopoEntity*>(e);
    if (topo && topo->getGeomAssociation()) {
        Geom::GeomEntity* ge = topo->getGeomAssociation();
        if (ge && find(ge)) {
		    TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Le groupe "<<getName()<<" possède déjà "<<ge->getName()<<" à laquelle est associée "<<e->getName();
			throw TkUtil::Exception(messErr);
		}
    }

    // Dans tous les autres cas on peut ajouter
    m_entities.push_back(e);
}
/*----------------------------------------------------------------------------*/
bool GroupEntity::find(Utils::Entity* e)
{
    return (std::find(m_entities.begin(), m_entities.end(), e) != m_entities.end());
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const GroupEntity & g)
{
    o << g.getName()
      << " (uniqueId "<<g.getUniqueId()<<")"
      << (g.isDestroyed()?" (DETRUIT)":"");
    o << (g.isVisible()?" visible":" caché");
    if (g.empty())
        o << " vide.";
    o << "\n";

    Topo::TopoManager& tm = g.getContext().getTopoManager();
    for (Utils::Entity* e : g.m_entities) {
        o<<"  "<<e->getName();
        // si e est une GeomEntity, on montre l'association topologique
        if (Geom::GeomEntity* ge = dynamic_cast<Geom::GeomEntity*>(e)) {
            const std::vector<Topo::TopoEntity*>& topos = tm.getRefTopos(ge);
            if (topos.size() > 0) {
                o<<" ->";
                for (Topo::TopoEntity* te : topos)
                    o<<" "<<te->getName();
            }
        }
        if (Topo::TopoEntity* te = dynamic_cast<Topo::TopoEntity*>(e)) {
            if (te->getGeomAssociation())
                o<<" ->"<<te->getGeomAssociation()->getName();
        }
        o << "\n";
    }

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const GroupEntity & g)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << g;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
std::string GroupEntity::getInfos()
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << *this;
    return us.iso();
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
