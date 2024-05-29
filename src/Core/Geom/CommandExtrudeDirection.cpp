/*----------------------------------------------------------------------------*/
/** \file CommandExtrudeDirection.cpp
 *
 *  Created on: 8/11/2019
 *      Author: Eric B
 *      à partir de CommandRevolution, renommé depuis en CommandExtrudeRevolution
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités extrudées et correction du bug de perte de relations
 *      géométriques avec les entités créées
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrudeDirection.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomExtrudeImplementation.h"
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
CommandExtrudeDirection::
CommandExtrudeDirection(Internal::Context& c,
        std::vector<GeomEntity*>& entities,
		const Vector& dp, const bool keep)
:   CommandExtrusion(c, "Direction"),
    m_entities(entities),
    m_vector(dp),
    m_keep(keep)
{
    if (m_vector.abs2()==0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le vecteur pour l'extrusion ne doit pas être nul", TkUtil::Charset::UTF_8));

    m_impl = new GeomExtrudeImplementation(c,m_entities,m_vector,m_keep);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Extrusion de";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandExtrudeDirection::~CommandExtrudeDirection()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeDirection::
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

    // Si on ne garde pas, alors on transmet les relations géométriques des entités supprimées aux entités copiées pour
    // l'extrusion
    if(!m_keep) {

        //On reconstruit les relations points <-> courbes
        for (auto v2v : m_v2v) {
            Vertex *v_ref = v2v.first;
            Vertex *v_created = v2v.second;
            std::vector<Curve *> curves;
            v_ref->get(curves);
            for (int i = 0; i < curves.size(); i++) {
                Curve *c = curves[i];
                v_created->add(c);
                c->add(v_created);
            }
        }

        //On reconstruit les relations courbes <-> surfaces
        for (auto c2c : m_c2c) {
            Curve *c_ref = c2c.first;
            Curve *c_created = c2c.second;
            std::vector<Surface *> surfaces;
            c_ref->get(surfaces);
            for (int i = 0; i < surfaces.size(); i++) {
                Surface *s = surfaces[i];
                c_created->add(s);
                s->add(c_created);
            }
        }

        //On reconstruit les relations surfaces <-> volumes
        for (auto s2s : m_s2s) {
            Surface *s_ref = s2s.first;
            Surface *s_created = s2s.second;
            std::vector<Volume *> volumes;
            s_ref->get(volumes);
            for (int i = 0; i < volumes.size(); i++) {
                Volume *vol = volumes[i];
                s_created->add(vol);
                vol->add(s_created);
            }
        }

        //On a besoin de gérer que les relations de dimension n <-> n+1, le reste est créé implicitement par ces relations
    }

    for(int i=0;i<rem_entities.size();i++){
        getInfoCommand ( ).addGeomInfoEntity (rem_entities[i], Internal::InfoCommand::DELETED);
    }

    getInfoCommand().setDestroyAndUpdateConnectivity(rem_entities);

    // transmet les groupes du 2D vers le 3D
    groups2DTo3D();

    for(int i=0;i<new_entities.size();i++){
    	GeomEntity* ge = new_entities[i];
    	if (ge->getNbGroups() == 0)
    		// ajoute à un groupe par défaut
    		addToGroup(ge);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


















