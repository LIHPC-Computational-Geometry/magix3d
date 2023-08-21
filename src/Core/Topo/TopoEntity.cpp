/*----------------------------------------------------------------------------*/
/*
 * \file TopoEntity.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 nov. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Topo/TopoEntity.h"
#include "Topo/CommandEditTopo.h"
#include "Geom/GeomEntity.h"
#include "Mesh/CommandCreateMesh.h"
#include "Internal/InternalPreferences.h"
#include "Utils/Bounds.h"
#include "Utils/SerializedRepresentation.h"
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
TopoEntity::TopoEntity(Internal::Context& ctx,
                       Utils::Property* prop,
                       Utils::DisplayProperties* disp)
: Internal::InternalEntity (ctx, prop, disp)
, m_topo_property(new TopoProperty())
, m_save_topo_property(0)
{
#ifdef _DEBUG_MEMORY
    std::cout<<"TopoEntity::TopoEntity() de nom "<<getName()<<std::endl;
    //std::cout<<"  m_topo_property en "<<m_topo_property<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
TopoEntity::~TopoEntity()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"TopoEntity::~TopoEntity() de "<<getName()<<std::endl;
    //std::cout<<"  m_topo_property en "<<m_topo_property<<std::endl;
#endif
    if (m_topo_property)
        delete m_topo_property;

    if (m_save_topo_property)
        std::cerr<<"La sauvegarde du TopoProperty a été oublié pour "<<getName()<<std::endl;
}
/*----------------------------------------------------------------------------*/
void TopoEntity::getBounds (double bounds[6]) const
{
    // tous les sommets
    std::vector<Vertex* > vertices;
    getAllVertices(vertices);

    if (vertices.empty()){
        Entity::getBounds(bounds);
        return;
    }

    Utils::Bounds bnds(vertices[0]->getCoord());
    for (uint i=1; i<vertices.size(); i++){
        bnds.add(vertices[i]->getCoord());
    }

    bnds.get(bounds);
}
/*----------------------------------------------------------------------------*/
void TopoEntity::getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, TopoEntity::getAllVertices doit être implémenté.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void TopoEntity::setDestroyed(bool b)
{
    // cas sans changement
    if (isDestroyed() == b)
        return;

#ifdef _DEBUG2
    std::cout<<"TopoEntity::setDestroyed("<<b<<") de "<<getName();//<<std::endl;
#endif

    // on met à jour la géométrie s'il y a une relation
    Geom::GeomEntity* ge = getGeomAssociation();
    if (ge){
#ifdef _DEBUG2
    std::cout<<"  en relation avec "<<ge->getName()<<std::endl;
#endif
        if (b) // cas de la destruction
            ge->removeRefTopo(this);
        else // on remet la relation
            ge->addRefTopo(this);
    }
#ifdef _DEBUG2
    else
        std::cout<<"  sans relation géométrique "<<std::endl;
#endif


    Entity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
void TopoEntity::
setGeomAssociation(Geom::GeomEntity* ge)
{
    // cas sans changement
    if (ge == m_topo_property->getGeomAssociation())
        return;

#ifdef _DEBUG2
    std::cout << "TopoEntity::setGeomAssociation(ge) pour "
              << getName()<<" est associé à "<<(ge?ge->getName():"0")
              << " en remplacement de "
              <<(m_topo_property->getGeomAssociation()?m_topo_property->getGeomAssociation()->getName():"0")
              <<std::endl;
#endif

    // on met à jour la relation réciproque (de Geom vers Topo)
    if (m_topo_property->getGeomAssociation())
        m_topo_property->getGeomAssociation()->removeRefTopo(this);
    if (ge)
        ge->addRefTopo(this);

    // mise à jour de la couleur si changement de dimension pour la projection
    Geom::GeomEntity* oldGe = m_topo_property->getGeomAssociation();
    bool need_update_color = (0==ge || 0==oldGe || ge->getDim() != oldGe->getDim());

    // la relation Topo vers Geom
    m_topo_property->setGeomAssociation(ge);

    if (need_update_color)
    	updateDisplayPropertiesColor();
}
/*----------------------------------------------------------------------------*/
void TopoEntity::updateDisplayPropertiesColor()
{
#ifdef _DEBUG2
    std::cout << "TopoEntity::updateDisplayPropertiesColor() pour "<<getName()<<std::endl;
#endif
	Geom::GeomEntity* ge = m_topo_property->getGeomAssociation();

	// Actualisation code couleur pour affichage :
	TkUtil::Color	color (
			255*Internal::InternalPreferences::instance ( )._topoColorWithoutProj.getRed(),
			255*Internal::InternalPreferences::instance ( )._topoColorWithoutProj.getGreen(),
			255*Internal::InternalPreferences::instance ( )._topoColorWithoutProj.getBlue());
	if (0 != ge)
	{
		if (0 == ge->getDim ( ))
		{
			color.setRGB (
			255*Internal::InternalPreferences::instance ( )._topoColorWith0DProj.getRed( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith0DProj.getGreen( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith0DProj.getBlue( ));
		}	// else if (0 == ge->getDim ( ))
		else if (1 == ge->getDim ( ))
		{
			color.setRGB (
			255*Internal::InternalPreferences::instance ( )._topoColorWith1DProj.getRed( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith1DProj.getGreen( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith1DProj.getBlue( ));
		}	// else if (1 == ge->getDim ( ))
		else if (2 == ge->getDim ( ))
		{
			color.setRGB (
			255*Internal::InternalPreferences::instance ( )._topoColorWith2DProj.getRed( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith2DProj.getGreen( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith2DProj.getBlue( ));
		}	// else if (2 == ge->getDim ( ))
		else if (3 == ge->getDim ( ))
		{
			color.setRGB (
			255*Internal::InternalPreferences::instance ( )._topoColorWith3DProj.getRed( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith3DProj.getGreen( ),
			255*Internal::InternalPreferences::instance ( )._topoColorWith3DProj.getBlue( ));
		}	// else if (3 == ge->getDim ( ))
	}	// if (0 != ge)

	getDisplayProperties ( ).setCloudColor (color);
	getDisplayProperties ( ).setWireColor (color);
	getDisplayProperties ( ).setSurfacicColor (color);
	getDisplayProperties ( ).setVolumicColor (color);
	getDisplayProperties ( ).setFontColor (color);
}
/*----------------------------------------------------------------------------*/
void TopoEntity::saveTopoProperty()
{
	/* Explication sur le fait que l'on ne passe pas Internal::InfoCommand* comme pour
	 * save... des entités de base:
	 * Les appels à CoEdge::saveCoEdgeMeshingData par exemple font un test change = icmd->addTopoInfoEntity ...
	 * or si l'on fait icmd->addTopoInfoEntity ici, alors change sera faux et la sauvegarde non faite
	 *
	 * Peut-être que change n'est pas assez riche en info, et qu'il faudrait 3 états de sortie...
	 */


    if (m_save_topo_property == 0){
#ifdef _DEBUG2
        std::cout<< "TopoEntity::saveTopoProperty() pour "<<getName()<<std::endl;
#endif
        m_save_topo_property = m_topo_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
TopoProperty* TopoEntity::setProperty(TopoProperty* new_tp)
{
#ifdef _DEBUG2
    std::cout<< "TopoEntity::setProperty(...) pour "<<getName()<<std::endl;
    std::cout<< " ancienne topo property : "<<*m_topo_property<<std::endl;
    std::cout<< " nouvelle topo property : "<<*new_tp<<std::endl;
#endif

    // on met à jour la géométrie s'il y a une relation
    Geom::GeomEntity* oldge = m_topo_property->getGeomAssociation();
    Geom::GeomEntity* newge = new_tp->getGeomAssociation();
    if (oldge)
        oldge->removeRefTopo(this);
    if (newge)
        newge->addRefTopo(this);

    TopoProperty* old_tp = m_topo_property;
    m_topo_property = new_tp;

    if (0==newge || 0==oldge || newge->getDim() != oldge->getDim())
    	updateDisplayPropertiesColor();

    return old_tp;
}
/*----------------------------------------------------------------------------*/
void TopoEntity::
saveInternals(CommandEditTopo* cet)
{
#ifdef _DEBUG2
    std::cout<<"TopoEntity::saveInternals pour "<<getName()<<std::endl;
#endif
    if (m_save_topo_property) {
        cet->addTopoInfoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
bool TopoEntity::
isFinished()
{
    return getContext().isFinished();
}
/*----------------------------------------------------------------------------*/
bool TopoEntity::
isEdited() const
{
    return m_save_topo_property != 0;
}
/*----------------------------------------------------------------------------*/
void TopoEntity::
getGroupsName (std::vector<std::string>& gn, bool byGeom, bool byTopo) const
{
    // on filtre les groupes suivant la dimension
    if (getGeomAssociation() && getGeomAssociation()->getDim() == getDim())
        getGeomAssociation()->getGroupsName(gn);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::SerializedRepresentation* TopoEntity::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Mgx3D::Utils::SerializedRepresentation>   description (
    		InternalEntity::getDescription (alsoComputed));

    if (getGeomAssociation()){
        Mgx3D::Utils::SerializedRepresentation  geomDescription (
                "Relation vers la géométrie", getGeomAssociation()->getName());
        geomDescription.addProperty (
                Utils::SerializedRepresentation::Property (getGeomAssociation()->getName(),
                        *getGeomAssociation()));
        description->addPropertiesSet (geomDescription);
    }
    else {
        description->addProperty (
                Utils::SerializedRepresentation::Property (
                        "Relation vers la géométrie", std::string("Aucune")));
    }

/*    std::vector<Topo::Vertex* > vertices;
    getVertices(vertices);
    if (!vertices.empty()){
		UTF8String	summary (TkUtil::Charset::UTF_8);
        for (std::vector<Topo::Vertex*>::iterator itv = vertices.begin( ); vertices.end( )!=itv; itv++)
            summary << (*itv)->getName ( ) <<" ";
        description->setSummary(summary.ascii());
    }*/
	description->setSummary (getSummary ( ));

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
std::string TopoEntity::getSummary ( ) const
{
    std::vector<Topo::Vertex* > vertices;
    getVertices(vertices);
	TkUtil::UTF8String	summary (TkUtil::Charset::UTF_8);
    for (std::vector<Topo::Vertex*>::iterator itv = vertices.begin( ); vertices.end( )!=itv; itv++)
            summary << (*itv)->getName ( ) <<" ";

	return summary.ascii ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
