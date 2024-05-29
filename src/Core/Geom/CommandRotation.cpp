/*----------------------------------------------------------------------------*/
/** CommandRotation.cpp
 *
 *  Created on: 27 sept. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Rotation.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandRotation.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomRotationImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandRotation::
CommandRotation(Internal::Context& c,
        std::vector<GeomEntity*>& entities,
        const Utils::Math::Rotation& rot)
: CommandEditGeom(c, "Rotation"),
  m_rotation(rot)
{
	validate();
	m_impl = new GeomRotationImplementation(c,entities,rot.getAxis1(),rot.getAxis2(),rot.getAngle());

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Rotation de";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	comments << " suivant "<<m_rotation;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandRotation::
CommandRotation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const Utils::Math::Rotation& rot)
: CommandEditGeom(c, "Rotation"),
  m_rotation(rot)
{
	validate();
	m_impl = new GeomRotationImplementation(c,cmd,rot.getAxis1(),rot.getAxis2(),rot.getAngle());
}
/*----------------------------------------------------------------------------*/
CommandRotation::
CommandRotation(Internal::Context& c,
        const Utils::Math::Rotation& rot)
: CommandEditGeom(c, "Rotation"),
  m_rotation(rot)
{
	validate();
	m_impl = new GeomRotationImplementation(c,rot.getAxis1(),rot.getAxis2(),rot.getAngle());

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Rotation de tout suivant "<<m_rotation;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandRotation::~CommandRotation()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandRotation::validate()
{
	/* les deux points passes en paramètres ne peuvent être égaux */
	if(m_rotation.getAxis1()==m_rotation.getAxis2())
		throw TkUtil::Exception(TkUtil::UTF8String ("Mauvaise définition de l'axe de révolution", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandRotation::
internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
void CommandRotation::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandRotation::internalSpecificUndo()
{
    m_impl->performUndo();
}
/*----------------------------------------------------------------------------*/
void CommandRotation::internalSpecificRedo()
{
    m_impl->performRedo();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
