/*----------------------------------------------------------------------------*/
/** \file CommandCreateGeom.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Geom/CommandCreateGeom.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Group/GroupManager.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
using namespace TkUtil;
using namespace Mgx3D::Utils;

/*----------------------------------------------------------------------------*/
#include <TDocStd_Document.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandCreateGeom::CommandCreateGeom(Internal::Context& c, std::string name,
                                     const std::string& groupName)
	: Internal::CommandInternal (c, name)
, m_group_name(groupName)
, m_dim_new_group(3)
, m_isPreview(false)
{}
/*----------------------------------------------------------------------------*/
CommandCreateGeom::~CommandCreateGeom()
{
    // Suppression par le manager des entités détruites
    Internal::InfoCommand& icmd = getInfoCommand();

    if (!getContext().isFinished()) {
        for (uint i = 0; i < icmd.getNbGeomInfoEntity(); i++) {
            GeomEntity *ge = 0;
            Internal::InfoCommand::type t;
            icmd.getGeomInfoEntity(i, ge, t);

            if (ge) {
                if (Internal::InfoCommand::DELETED == t) {
                    getGeomManager().removeEntity(ge);
                    // destruction des entités que l'on avait construites
                    delete ge;
                }
            } else
                std::cerr << "Erreur interne dans ~CommandCreateGeom, GeomEntity nulle" << std::endl;
        } // end for i<icmd.getNbGeomInfoEntity()
    }

}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::preExecute()
{
	if (!m_isPreview)
		EntityFactory(getContext()).beginOCAFCommand();
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::postExecute(bool hasError)
{
	if (hasError){
		std::cout<<"CommandCreateGeom::postExecute en erreur => destruction des entités"<<std::endl;
	    // Suppression par le manager des entités détruites
	    Internal::InfoCommand& icmd = getInfoCommand();
		for (uint i=0; i<icmd.getNbGeomInfoEntity(); i++) {
			GeomEntity* ge = 0;
			Internal::InfoCommand::type t;
			icmd.getGeomInfoEntity(i, ge, t);

			if (ge){
				if (Internal::InfoCommand::DELETED == t) {
					getGeomManager().removeEntity(ge);
					// destruction des entités que l'on avait construites
					delete ge;
				}
			}
			else
				std::cerr<<"Erreur interne dans CommandCreateGeom::postExecute, GeomEntity nulle"<<std::endl;
		} // end for i<icmd.getNbGeomInfoEntity()

		icmd.clear();

		if (!m_isPreview)
			EntityFactory(getContext()).abortOCAFCommand();
	}
	else {
		if (!m_isPreview)
			EntityFactory(getContext()).endOCAFCommand();
	}
}
/*----------------------------------------------------------------------------*/
GeomManager& CommandCreateGeom::getGeomManager()
{
    return getContext().getLocalGeomManager();
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::internalUndo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandCreateGeom::internalUndo pour la commande " << getName ( )
        << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex autoMutex (getMutex ( ));

    // gestion du undo via OCAF
    EntityFactory(getContext()).undoOCAFCommand();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::internalRedo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandCreateGeom::internalRedo pour la commande " << getName ( )
        << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex	autoMutex (getMutex ( ));

    startingOrcompletionLog (true);

    // gestion du undo via OCAF
    EntityFactory(getContext()).redoOCAFCommand();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

	startingOrcompletionLog (false);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
getResult(std::vector<GeomEntity*>& res)
{
    res =m_createdEntities;
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
split(Volume* v)
{
#ifdef _DEBUG2
	std::cout<<"split("<<v->getName()<<")"<<std::endl;
#endif
    std::vector<Surface*> surfaces;
    std::vector<Curve* >  curves;
    std::vector<Vertex* > vertices;
    
    /* l'entité v va créer les cellules de dimension inférieure. Il en a donc
     * la responsabilité. De plus les mises à jour entre connectivité vont se
     * faire au fil de l'eau des créations.
     *
     * On utilise l'InfoCommand pour stocker les différentes entités créées
     */
    v->split(surfaces,curves,vertices);

    store(v);

    for(unsigned int i=0;i<surfaces.size();i++){
        store(surfaces[i]);
        addToGroup(surfaces[i], true);
    }
    for(unsigned int i=0;i<curves.size();i++){
        store(curves[i]);
        addToGroup(curves[i], true);
    }
    for(unsigned int i=0;i<vertices.size();i++){
        store(vertices[i]);
        addToGroup(vertices[i], true);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
split(Surface* s)
{
    std::vector<Curve* >  curves;
    std::vector<Vertex* > vertices;

    /* l'entité s va créer les cellules de dimension inférieure. Il en a donc
     * la responsabilité. De plus les mises à jour entre connectivité vont se
     * faire au fil de l'eau des créations.
     *
     * On utilise l'InfoCommand pour stocker les différentes entités créées
     */
    s->split(curves,vertices);

    store(s);

    for(unsigned int i=0;i<curves.size();i++){
        store(curves[i]);
        addToGroup(curves[i], true);
    }
    for(unsigned int i=0;i<vertices.size();i++){
        store(vertices[i]);
        addToGroup(vertices[i], true);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
split(Curve* c)
{
    std::vector<Vertex* > vertices;


    /* l'entité c va créer les cellules de dimension inférieure. Il en a donc
     * la responsabilité. De plus les mises à jour entre connectivité vont se
     * faire au fil de l'eau des créations.
     *
     * On utilise l'InfoCommand pour stocker les différentes entités créées
     */
    c->split(vertices);

    store(c);

    for(unsigned int i=0;i<vertices.size();i++){
        store(vertices[i]);
        addToGroup(vertices[i], true);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
split(Vertex* v)
{
    /* On utilise l'InfoCommand pour stocker les différentes entités créées */
    store(v);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
store(GeomEntity* e)
{
#ifdef _DEBUG2
	std::cout<<"CommandCreateGeom::store("<<e->getName()<<")"<<std::endl;
#endif
    if(e->getDim()==0)
        store(dynamic_cast<Vertex*>(e));
    else if(e->getDim()==1)
        store(dynamic_cast<Curve*>(e));
    else if(e->getDim()==2)
        store(dynamic_cast<Surface*>(e));
    else
        store(dynamic_cast<Volume*>(e));

}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
store(Vertex* e)
{
    getGeomManager().add(e);
    getInfoCommand().addGeomInfoEntity(e, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
store(Curve* e)
{
    getGeomManager().add(e);
    getInfoCommand().addGeomInfoEntity(e, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
store(Surface* e)
{
    getGeomManager().add(e);
    getInfoCommand().addGeomInfoEntity(e, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
store(Volume* e)
{
    getGeomManager().add(e);
    getInfoCommand().addGeomInfoEntity(e, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
store(std::vector<GeomEntity*>& e){
    for(unsigned int i=0;i<e.size();i++){
        store(e[i]);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
addToGroup(GeomEntity* e, bool use_default_name)
{
#ifdef _DEBUG2
	std::cout<<"CommandCreateGeom::addToGroup("<<e->getName()<<", "<<use_default_name<<") avec m_group_name "<<m_group_name<<std::endl;
#endif
   if(e->getDim()==0)
        addToGroup(dynamic_cast<Vertex*>(e), use_default_name);
    else if(e->getDim()==1)
        addToGroup(dynamic_cast<Curve*>(e), use_default_name);
    else if(e->getDim()==2)
        addToGroup(dynamic_cast<Surface*>(e), use_default_name);
    else
        addToGroup(dynamic_cast<Volume*>(e), use_default_name);

}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
addToGroup(Volume* v, bool use_default_name)
{
    Group::Group3D* group = getContext().getLocalGroupManager().getNewGroup3D(use_default_name?"":m_group_name, &getInfoCommand());
    v->add(group);
    group->add(v);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
addToGroup(Surface* s, bool use_default_name)
{
    Group::Group2D* group = getContext().getLocalGroupManager().getNewGroup2D(use_default_name?"":m_group_name, &getInfoCommand());
    s->add(group);
    group->add(s);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
addToGroup(Curve* c, bool use_default_name)
{
    Group::Group1D* group = getContext().getLocalGroupManager().getNewGroup1D(use_default_name?"":m_group_name, &getInfoCommand());
    c->add(group);
    group->add(c);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
addToGroup(Vertex* v, bool use_default_name)
{
    Group::Group0D* group = getContext().getLocalGroupManager().getNewGroup0D(use_default_name?"":m_group_name, &getInfoCommand());
    v->add(group);
    group->add(v);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
updateDimensionGroup(std::vector<GeomEntity*>& entities)
{
	m_dim_new_group = 0;
	for (uint i=0; i<entities.size(); i++)
		if (entities[i]->getDim() > m_dim_new_group)
			m_dim_new_group = entities[i]->getDim();
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	// on ne fait rien par défaut
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_PREVIEW
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
getPreviewRepresentationNewVertices(Utils::DisplayRepresentation& dr)
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"getPreviewRepresentationNewVertices"<<std::endl;
#endif
    // cas où la commande doit être détuite, il faut en créer une autre ...
    if (m_isPreview){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "La commande " << getName ( )
    	        << "  ne peut faire qu'un seul preview";
    	throw TkUtil::Exception(message);
    }

    getContext().getNameManager().getInternalStats(m_name_manager_before);


    bool            hasError    = false;

    // effectue la commande
    try {
    	internalExecute();
        if (Command::CANCELED == getStatus ( ))
        	hasError    = true;
    }
    catch (...)
    {
    	hasError    = true;
    }

    // on s'assure de ne pas la relancer une seconde fois
    m_isPreview = true;

    // fait le ménage si nécessaire
    postExecute(hasError);
    if (hasError){
        // retour en arrière pour les noms
        getContext().getNameManager().setInternalStats(m_name_manager_before);
        throw TkUtil::Exception(TkUtil::UTF8String ("Commande en erreur lors du preview", TkUtil::Charset::UTF_8));
    }


    // parcours les sommets créés et ajoute leur représentation
    Internal::InfoCommand& icmd = getInfoCommand();
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    points.clear();

    std::vector<Internal::InfoCommand::GeomEntityInfo>& entities_info = icmd.getGeomInfoEntities();

    for (std::vector<Internal::InfoCommand::GeomEntityInfo>::iterator iter_ei = entities_info.begin();
    		iter_ei != entities_info.end(); ++iter_ei){
    	Geom::GeomEntity* ge = iter_ei->m_geom_entity;
    	Internal::InfoCommand::type t =  iter_ei->m_type;
    	if (t == Internal::InfoCommand::CREATED && ge->getType() == Utils::Entity::GeomVertex){
    		Geom::Vertex* vertex = dynamic_cast<Geom::Vertex*>(ge);
    		CHECK_NULL_PTR_ERROR(vertex);
#ifdef _DEBUG_PREVIEW
    		std::cout<<"  On ajoute le sommet "<<vertex->getName()<<std::endl;
#endif
    		points.push_back(vertex->getCoord());
    	}
    } // end for iter_ei

    // annulation de la commande
    CommandCreateGeom::internalUndo();

    // remet les id comme au départ de la commande
    getContext().getNameManager().setInternalStats(m_name_manager_before);
}
/*----------------------------------------------------------------------------*/
void CommandCreateGeom::
getPreviewRepresentationNewObjects(Utils::DisplayRepresentation& dr)
{
#ifdef _DEBUG_PREVIEW
	std::cout<<"getPreviewRepresentationNewObjects"<<std::endl;
#endif
    // cas où la commande doit être détruite, il faut en créer une autre ...
    if (m_isPreview){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "La commande " << getName ( )
    	        << "  ne peut faire qu'un seul preview";
    	throw TkUtil::Exception(message);
    }

    getContext().getNameManager().getInternalStats(m_name_manager_before);


    bool            hasError    = false;

    // effectue la commande
    const bool	graphicalPreviewMode	= getContext ( ).isPreviewMode ( );
    getContext ( ).setPreviewMode (true);
    try {
    	internalExecute();
        if (Command::CANCELED == getStatus ( ))
        	hasError    = true;
    }
    catch (...)
    {
    	hasError    = true;
    }
    getContext ( ).setPreviewMode (graphicalPreviewMode);

    // on s'assure de ne pas la relancer une seconde fois
    m_isPreview = true;

    // fait le ménage si nécessaire
    postExecute(hasError);
    if (hasError){
        // retour en arrière pour les noms
        getContext().getNameManager().setInternalStats(m_name_manager_before);
        throw TkUtil::Exception("Commande en erreur lors du preview");
    }

    // parcours les objets (1D, 2D ou 3D) créés et ajoute leur représentation
    Internal::InfoCommand& icmd = getInfoCommand();
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indices = dr.getCurveDiscretization();
    points.clear();
    indices.clear();

    std::vector<Internal::InfoCommand::GeomEntityInfo>& entities_info = icmd.getGeomInfoEntities();

    for (std::vector<Internal::InfoCommand::GeomEntityInfo>::iterator iter_ei = entities_info.begin();
    		iter_ei != entities_info.end(); ++iter_ei){
    	Geom::GeomEntity* ge = iter_ei->m_geom_entity;
    	Internal::InfoCommand::type t =  iter_ei->m_type;
    	if (t == Internal::InfoCommand::CREATED
    			&& ge->getType() >= Utils::Entity::GeomCurve
				&& ge->getType() <= Utils::Entity::GeomVolume){
#ifdef _DEBUG_PREVIEW
    		std::cout<<"  On ajoute l'objet "<<ge->getName()<<std::endl;
#endif
    		ge->getRepresentation(dr, true);
    	}
    } // end for iter_ei

    // annulation de la commande
    CommandCreateGeom::internalUndo();

    // remet les id comme au départ de la commande
    getContext().getNameManager().setInternalStats(m_name_manager_before);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
