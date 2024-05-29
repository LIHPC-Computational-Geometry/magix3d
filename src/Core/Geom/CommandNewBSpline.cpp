/*----------------------------------------------------------------------------*//*
 * CommandNewBSpline.cpp
 *
 *  Created on: 2 avr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandNewBSpline.h"
#include "Geom/PropertyBox.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewBSpline::
CommandNewBSpline(Internal::Context& c, std::vector<Geom::Vertex*>& points,
		int degMin,
		int degMax,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une bspline", groupName)
, m_control_points(points)
, m_vtx1(0), m_vtx2(0)
, m_deg_min(degMin)
, m_deg_max(degMax)
{}
/*----------------------------------------------------------------------------*/
CommandNewBSpline::
CommandNewBSpline(Internal::Context& c,
		const std::vector<Geom::CommandCreateGeom*>& cmds,
		int degMin,
		int degMax,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une bspline", groupName)
, m_cmds_vertices(cmds)
, m_vtx1(0), m_vtx2(0)
, m_deg_min(degMin)
, m_deg_max(degMax)
{}
/*----------------------------------------------------------------------------*/
CommandNewBSpline::
CommandNewBSpline(Internal::Context& c,
		Vertex* vtx1,
		std::vector<Point>& vp,
		Vertex* vtx2,
		int degMin,
		int degMax,
		const std::string& groupName)
:CommandCreateGeom(c, "Création d'une bspline", groupName)
, m_vtx1(vtx1)
, m_vp(vp)
, m_vtx2(vtx2)
, m_deg_min(degMin)
, m_deg_max(degMax)
{}
/*----------------------------------------------------------------------------*/
CommandNewBSpline::~CommandNewBSpline()
{}
/*----------------------------------------------------------------------------*/
void CommandNewBSpline::
internalExecute()
{
	if (m_control_points.empty() && m_vtx1 == 0){
		std::vector<GeomEntity*> res;
		for (uint i=0; i<m_cmds_vertices.size(); i++){
			m_cmds_vertices[i]->getResult(res);
			if (res.size() != 1)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne CommandNewBSpline, la commande n'a pas qu'une entité", TkUtil::Charset::UTF_8));
			Vertex* vertex = dynamic_cast<Vertex*>(res[0]);
			if (vertex == 0)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne CommandNewBSpline, la commande n'a pas créée un sommet", TkUtil::Charset::UTF_8));
			m_control_points.push_back(vertex);
		}
	}

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewBSpline::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
//            << std::ios_base::fixed << TkUtil::setprecision (8)
            << "(";

    std::vector<Utils::Math::Point> points;

    if (m_control_points.empty()){
    	// cas avec sommets aux extrémités
    	points.push_back(m_vtx1->getPoint());
    	points.insert(points.end(), m_vp.begin(), m_vp.end());
    	points.push_back(m_vtx2->getPoint());
    }
    else {
    	for(unsigned int i=0;i<m_control_points.size();i++)
    	{
    		Geom::Vertex* vi = m_control_points[i];
    		points.push_back(vi->getPoint());
    		if(i!=0)
    			message<<",";
    		message <<" "<<Utils::Math::MgxNumeric::userRepresentation (vi->getX())
    		<<", "<<Utils::Math::MgxNumeric::userRepresentation (vi->getY())
    		<<", "<<Utils::Math::MgxNumeric::userRepresentation (vi->getZ());
    	}
    	message <<").";
    }
    Curve* c = EntityFactory(getContext()).newBSpline(points, m_deg_min, m_deg_max);
    CHECK_NULL_PTR_ERROR(c);
    m_createdEntities.push_back(c);

    // stockage dans le manager géom
    store(c);

    // ajoute la courbe au groupe s'il y en a un de spécifié
    addToGroup(c);

    //creation des connections topologiques avec les sommets extremites uniquement
    if (m_control_points.empty()){
    	c->add(m_vtx1);
    	c->add(m_vtx2);
    	m_vtx1->add(c);
    	m_vtx2->add(c);
    }
    else {
    	Geom::Vertex* vi = m_control_points[0];
    	c->add(vi);
    	vi->add(c);
        if(m_control_points[m_control_points.size()-1]!=m_control_points[0])
        {
            Geom::Vertex* last_point = m_control_points[m_control_points.size()-1];
            c->add(last_point);
            last_point->add(c);
        }
    }

    // information un peu plus parlante
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
    comments << "Création de la bspline "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
