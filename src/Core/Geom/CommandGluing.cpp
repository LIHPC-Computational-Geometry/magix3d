/*----------------------------------------------------------------------------*/
/** \file CommandSewing.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 01/12/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandGluing.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomGluingImplementation.h"
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
CommandGluing::
CommandGluing(Internal::Context& c,std::vector<GeomEntity*>& es)
: CommandEditGeom(c, "Collage"), m_entities(es)
{
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Collage entre";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandGluing::
CommandGluing(Internal::Context& c,std::vector<CommandCreateGeom*>& ccg)
: CommandEditGeom(c, "Collage"), m_command_create_entities(ccg)
{
}
/*----------------------------------------------------------------------------*/
CommandGluing::~CommandGluing()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandGluing::validate()
{
    if(m_entities.size()<2)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le collage nécessite au moins 2 entités", TkUtil::Charset::UTF_8));


    int dim_ref = m_entities[0]->getDim();

    for(unsigned int i=1;i<m_entities.size();i++){
        if(m_entities[i]->getDim()!=dim_ref)
            throw TkUtil::Exception(TkUtil::UTF8String ("Toutes les entités à coller doivent avoir la même dimension", TkUtil::Charset::UTF_8));
    }

    if(dim_ref!=3 && dim_ref!=1)
        throw TkUtil::Exception(TkUtil::UTF8String ("Seul le collage entre volumes ou courbes est supporté", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandGluing::internalSpecificPreExecute()
{
    if (m_command_create_entities.size()){
        for (std::vector<CommandCreateGeom*>::iterator iter = m_command_create_entities.begin();
                iter != m_command_create_entities.end(); ++iter){
            std::vector<GeomEntity*> res;
            (*iter)->getResult(res);
            m_entities.insert(m_entities.begin(), res.begin(), res.end());
        }
    }

    validate();

    m_impl = new GeomGluingImplementation(getContext(), m_entities);

    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandGluing::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
