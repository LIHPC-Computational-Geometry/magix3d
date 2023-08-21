/*----------------------------------------------------------------------------*/
/** \file CommandCommon.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCommon.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomCommonImplementation.h"
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
CommandCommon::
CommandCommon(Internal::Context& c,std::vector<GeomEntity*> e)
: CommandEditGeom(c, "Intersection Booléenne"), m_entities(e)
{
    validate();
    m_impl = new GeomCommonImplementation(c, m_entities);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Intersection Booléenne entre";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandCommon::~CommandCommon()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandCommon::validate()
{
    /* L'intersection booléenne ne peut se faire qu'entre entités de même dimension
     * et qui ne sont pas reliées à des entités de dimension supérieure */
    if(m_entities.size()<2)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'intersection booléenne nécessite au moins 2 entités", TkUtil::Charset::UTF_8));

    int dim_ref = m_entities[0]->getDim();
    if(dim_ref!=3)
        throw TkUtil::Exception("Les objets à intersecter doivent être des volumes");

    for(unsigned int i=1;i<m_entities.size();i++){
        int dimi = m_entities[i]->getDim();
        if(dimi!=dim_ref)
            throw TkUtil::Exception("Les objets à intersecter doivent avoir la même dimension");
    }


}
/*----------------------------------------------------------------------------*/
void CommandCommon::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);

}
/*----------------------------------------------------------------------------*/
void CommandCommon::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
