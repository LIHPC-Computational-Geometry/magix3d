/*----------------------------------------------------------------------------*/
/** \file CommandNewSurface.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 31/01/2011
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewSurface.h"
#include "Geom/CommandCreateGeom.h"
#include "Geom/PropertyBox.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"

#include "Geom/OCCGeomRepresentation.h"

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewSurface::CommandNewSurface(Internal::Context& c,
                const std::vector<Geom::Curve*>& curves,
                const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'une surface", groupName), m_curves(curves)
{}
/*----------------------------------------------------------------------------*/
CommandNewSurface::CommandNewSurface(Internal::Context& c,
                const std::vector<Geom::CommandCreateGeom*>& cmds,
                const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'une surface", groupName), m_cmds_curves(cmds)
{}
/*----------------------------------------------------------------------------*/
CommandNewSurface::
~CommandNewSurface()
{}
/*----------------------------------------------------------------------------*/
void CommandNewSurface::
internalExecute()
{
	if (m_curves.empty()){
		std::vector<GeomEntity*> res;
		for (uint i=0; i<m_cmds_curves.size(); i++){
			m_cmds_curves[i]->getResult(res);
			if (res.size() != 1)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne CommandNewSurface, la commande n'a pas qu'une entité", TkUtil::Charset::UTF_8));
			Curve* curve = dynamic_cast<Curve*>(res[0]);
			if (curve == 0)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne CommandNewSurface, la commande n'a pas créée une courbe", TkUtil::Charset::UTF_8));
			m_curves.push_back(curve);
		}
	}

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewSurface::execute pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );
/*                << std::ios_base::fixed << TkUtil::setprecision (8)
                << ". P1 (" << m_v1.getX ( ) << ", "
                << m_v1.getY ( ) << ", " << m_v1.getZ ( ) << "), P2 ("
                << m_v2.getX ( ) << ", " << m_v2.getY ( ) << ", "
                << m_v2.getZ ( ) << ").";
*/
    // sauvegarde des relations (connections topologiques) avant modification
    for (uint i=0; i<m_curves.size(); i++)
    	saveMemento(m_curves[i]);

    Surface* s = EntityFactory(getContext()).newSurface(m_curves);
    m_createdEntities.push_back(s);

    // stockage dans le manager géom
    store(s);

    // ajoute la surface au groupe s'il y en a un de spécifié
    addToGroup(s);

    //creation des connections topologiques
    for(unsigned int i=0;i<m_curves.size();i++)
    {
        s->add(m_curves[i]);
        m_curves[i]->add(s);

    }

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de la surface "<<s->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
