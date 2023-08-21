/*----------------------------------------------------------------------------*/
/** \file CommandNewSegment.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 15/10/2010
 *
 *  Gestion avec memento par EB à partir du 29/8/2018
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewSegment.h"
#include "Geom/PropertyBox.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewSegment::CommandNewSegment(Internal::Context& c,
                             Geom::Vertex* v1,
                             Geom::Vertex* v2,
                             const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'un segment", groupName), m_v1(v1), m_v2(v2), m_cmd_v1(0), m_cmd_v2(0)
{}
/*----------------------------------------------------------------------------*/
CommandNewSegment::CommandNewSegment(  Internal::Context& c,
             Geom::CommandCreateGeom* cmd1,
             Geom::CommandCreateGeom* cmd2,
             const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'un segment", groupName), m_v1(0), m_v2(0), m_cmd_v1(cmd1), m_cmd_v2(cmd2)
{}
/*----------------------------------------------------------------------------*/
CommandNewSegment::
~CommandNewSegment()
{}
/*----------------------------------------------------------------------------*/
void CommandNewSegment::
internalExecute()
{
	if (m_v1 == 0 && m_v2 == 0){
		if (m_cmd_v1 == 0 || m_cmd_v2 == 0)
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, m_cmd_v1 ou m_cmd_v2 à 0", TkUtil::Charset::UTF_8));

		std::vector<GeomEntity*> res;
		m_cmd_v1->getResult(res);
		if (res.size() != 1){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Erreur interne, m_cmd_v1 n'a pas qu'une entité mais "<<(short)res.size();
			throw TkUtil::Exception(messErr);
		}
		m_v1 = dynamic_cast<Vertex*>(res[0]);

		m_cmd_v2->getResult(res);
		if (res.size() != 1){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Erreur interne, m_cmd_v2 n'a pas qu'une entité mais "<<(short)res.size();
			throw TkUtil::Exception(messErr);
		}
		m_v2 = dynamic_cast<Vertex*>(res[0]);
	}

	if (m_v1 == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, v1 n'a pas été correctement initialisé", TkUtil::Charset::UTF_8));
	if (m_v2 == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, v2 n'a pas été correctement initialisé", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewSegment::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( )
//		    << std::ios_base::fixed << TkUtil::setprecision (8)
		    << ". P1 (" << Utils::Math::MgxNumeric::userRepresentation (m_v1->getX ( )) << ", "
		    << Utils::Math::MgxNumeric::userRepresentation (m_v1->getY ( )) << ", "
	        << Utils::Math::MgxNumeric::userRepresentation (m_v1->getZ ( )) << "), P2 ("
		    << Utils::Math::MgxNumeric::userRepresentation (m_v2->getX ( )) << ", "
	        << Utils::Math::MgxNumeric::userRepresentation (m_v2->getY ( )) << ", "
		    << Utils::Math::MgxNumeric::userRepresentation (m_v2->getZ ( )) << ").";

    // sauvegarde des relations (connections topologiques) avant modification
    saveMemento(m_v1);
    saveMemento(m_v2);

    Curve* c = EntityFactory(getContext()).newSegment(m_v1,m_v2);
    m_createdEntities.push_back(c);

    // stockage dans le manager géom
    store(c);

    // ajoute la courbe au groupe s'il y en a un de spécifié
    addToGroup(c);

    //creation des connections topologiques
    c->add(m_v1);
    c->add(m_v2);
    m_v1->add(c);
    m_v2->add(c);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du segment "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
