/*----------------------------------------------------------------------------*/
/** \file CommandCut.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCut.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomCutImplementation.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
//class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandCut::
CommandCut(Internal::Context& c, GeomEntity* e,
           std::vector<GeomEntity*>& es)
: CommandEditGeom(c, "Différence Booléenne"),m_entity(e),
  m_cutting_entities(es)
{
    validate();
    m_impl = new GeomCutImplementation(c, m_entity, m_cutting_entities);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Différence Booléenne pour "<<m_entity->getName();
	comments << " auquel on retire";
	for (uint i=0; i<m_cutting_entities.size() && i<5; i++)
		comments << " " << m_cutting_entities[i]->getName();
	if (m_cutting_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandCut::~CommandCut()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandCut::validate()
{

    /* L'intersection booléenne ne peut se faire qu'entre entités de même dimension
     * et qui ne sont pas reliées à des entités de dimension supérieure */
    if(m_cutting_entities.size()<1)
        throw TkUtil::Exception(TkUtil::UTF8String ("La différence booléenne nécessite au moins 1 entité à retirer", TkUtil::Charset::UTF_8));


    int dim_ref = m_entity->getDim();
    if(dim_ref!=3)
        throw TkUtil::Exception(TkUtil::UTF8String ("Seule la différence entre volumes est supportée", TkUtil::Charset::UTF_8));
    dim_ref = m_cutting_entities[0]->getDim();
    if(dim_ref!=3)
        throw TkUtil::Exception(TkUtil::UTF8String ("Seule la différence entre volumes est supportée", TkUtil::Charset::UTF_8));

    for(unsigned int i=1;i<m_cutting_entities.size();i++){
        int dimi = m_cutting_entities[i]->getDim();
        if(dimi!=dim_ref)
            throw TkUtil::Exception(TkUtil::UTF8String ("Les objets sur lesquelles effectuer une différence booléenne doivent avoir la même dimension", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandCut::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}

/*----------------------------------------------------------------------------*/
void CommandCut::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
