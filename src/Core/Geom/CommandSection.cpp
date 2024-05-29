/*
 * CommandSection.cpp
 *
 *  Created on: 22 janv. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandSection.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomSectionImplementation.h"
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
CommandSection::
CommandSection(Internal::Context& c,std::vector<GeomEntity*>& entities,
        GeomEntity* t)
: CommandEditGeom(c, "Section"), m_entities(entities), m_tool(t)
{
    validate();
    m_impl = new GeomSectionImplementation(c, entities, t);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Section suivant "<<t->getName();
	comments << " des entités";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSection::~CommandSection()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandSection::validate()
{
    for(unsigned int i=0;i<m_entities.size();i++){
        if(m_entities[i]->getDim()<2)
        throw TkUtil::Exception(TkUtil::UTF8String ("Seuls des volumes géométriques peuvent être découpés", TkUtil::Charset::UTF_8));
    }

    int dim_tool = m_tool->getDim();
    if(dim_tool!=2)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'outil de découpe doit être une surface", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
void CommandSection::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandSection::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
