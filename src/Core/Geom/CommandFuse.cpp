/*----------------------------------------------------------------------------*/
/** \file CommandFuse.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandFuse.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomFuseImplementation.h"
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
CommandFuse::
CommandFuse(Internal::Context& c,std::vector<GeomEntity*> e)
: CommandEditGeom(c, "Fusion Booléenne"), m_entities(e)
{
    validate();
    m_impl = new GeomFuseImplementation(c, m_entities);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Fusion Booléenne de ";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandFuse::~CommandFuse()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandFuse::validate()
{
    /* La fusion booléenne ne peut se faire qu'entre entités de même dimension
     * et qui ne sont pas reliées à des entités de dimension supérieure */
    if(m_entities.size()<2)
        throw TkUtil::Exception(TkUtil::UTF8String ("La fusion booléenne nécessite au moins 2 entités", TkUtil::Charset::UTF_8));

    int dim_ref = m_entities[0]->getDim();
    if(dim_ref== 0 )
        throw TkUtil::Exception(TkUtil::UTF8String ("Les objets à fusionner ne doivent pas être des sommets", TkUtil::Charset::UTF_8));

    for(unsigned int i=1;i<m_entities.size();i++){
        int dimi = m_entities[i]->getDim();
        if(dimi!=dim_ref)
            throw TkUtil::Exception(TkUtil::UTF8String ("Les objets à fusionner doivent avoir la même dimension (que des volumes ou que des courbes)", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandFuse::
internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandFuse::
internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
