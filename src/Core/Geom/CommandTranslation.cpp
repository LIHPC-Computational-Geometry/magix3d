/*----------------------------------------------------------------------------*/
/** \file CommandTranslation.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandTranslation.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomTranslateImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandTranslation::
CommandTranslation(Internal::Context& c,
        std::vector<GeomEntity*>& entities,
        const Utils::Math::Vector& dv)
: CommandEditGeom(c, "Translation"), m_dv(dv)
{
   validate();
    m_impl = new GeomTranslateImplementation(c,entities,m_dv);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Translation de";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	comments << " suivant "<<dv;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandTranslation::
CommandTranslation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const Utils::Math::Vector& dv)
: CommandEditGeom(c, "Translation"), m_dv(dv)
{
   validate();
    m_impl = new GeomTranslateImplementation(c,cmd,m_dv);
}
/*----------------------------------------------------------------------------*/
CommandTranslation::
CommandTranslation(Internal::Context& c,
        const Utils::Math::Vector& dv)
: CommandEditGeom(c, "Translation"), m_dv(dv)
{
   validate();
    m_impl = new GeomTranslateImplementation(c,m_dv);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Translation de tout suivant "<<dv;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandTranslation::~CommandTranslation()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandTranslation::validate()
{
    if (Utils::Math::MgxNumeric::isNearlyZero(m_dv.abs2()))
          throw TkUtil::Exception(TkUtil::UTF8String ("La translation ne peut se faire avec un vecteur nul", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandTranslation::
internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
void CommandTranslation::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandTranslation::internalSpecificUndo()
{
    m_impl->performUndo();
}
/*----------------------------------------------------------------------------*/
void CommandTranslation::internalSpecificRedo()
{
    m_impl->performRedo();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
