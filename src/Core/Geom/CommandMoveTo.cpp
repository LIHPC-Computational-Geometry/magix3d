/*----------------------------------------------------------------------------*/
#include "Geom/CommandMoveTo.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomTranslateImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>

#include "Geom/CommandGeomCopy.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandMoveTo::
CommandMoveTo(Internal::Context& c,
        std::vector<GeomEntity*>& entities,
        const Utils::Math::Point& point)
: CommandEditGeom(c, "Déplacement"), m_point(point)
{
   Utils::Math::Vector vec = computeVector(entities);
    m_impl = new GeomTranslateImplementation(c,entities,vec);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Déplacement de";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	comments << " vers "<<point;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandMoveTo::
CommandMoveTo(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const Utils::Math::Point& point)
: CommandEditGeom(c, "Déplacement"), m_point(point)
{
	Utils::Math::Vector vec = computeVector(cmd->getEntities());
    m_impl = new GeomTranslateImplementation(c,cmd,vec);
}
/*----------------------------------------------------------------------------*/
CommandMoveTo::~CommandMoveTo()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandMoveTo::validate()
{
    //if (Utils::Math::MgxNumeric::isNearlyZero(m_point))
          //throw TkUtil::Exception(TkUtil::UTF8String ("La translation ne peut se faire avec un vecteur nul", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandMoveTo::
internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
void CommandMoveTo::internalSpecificPreExecute()
{
    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
Utils::Math::Vector CommandMoveTo::computeVector(const std::vector<GeomEntity *> &entities) {

	Utils::Math::Point p(0,0,0);


	for (auto ge : entities) {
		Utils::Math::Point p_ge = ge->getCenteredPosition();
		p.setXYZ(p.getX()+p_ge.getX(),p.getY()+p_ge.getY(),p.getZ()+p_ge.getZ());
	}

	double x = p.getX()/entities.size();
	double y = p.getY()/entities.size();
	double z = p.getZ()/entities.size();

	Utils::Math::Vector vec(m_point.getX()-x,m_point.getY()-y,m_point.getZ()-z);
	return vec;
}

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/