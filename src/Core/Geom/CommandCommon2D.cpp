/*----------------------------------------------------------------------------*/
/** \file CommandCommon2D.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCommon2D.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomCommon2DImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

/*----------------------------------------------------------------------------*/
CommandCommon2D::
CommandCommon2D(Internal::Context& c,GeomEntity* e1, GeomEntity* e2, std::string groupName)
: CommandEditGeom(c, "Intersection Booléenne", groupName)
{
    m_impl = new GeomCommon2DImplementation(c, e1, e2);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Intersection Booléenne entre "<<e1->getName()<<" et "<<e2->getName();
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandCommon2D::
CommandCommon2D(Internal::Context& c, Geom::CommandGeomCopy* geom_cmd, std::string groupName)
: CommandEditGeom(c, "Intersection Booléenne", groupName)
{
    m_impl = new GeomCommon2DImplementation(c, geom_cmd);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Intersection Booléenne entre 2 entités conservées";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandCommon2D::~CommandCommon2D()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandCommon2D::validate(GeomEntity* entity1, GeomEntity* entity2)
{
    /* L'intersection booléenne ne peut se faire qu'entre entités de même dimension */
    if (entity1->getDim() != entity2->getDim())
    	throw TkUtil::Exception("Les objets à intersecter doivent avoir la même dimension");

    if (entity1->getDim() != 1 && entity1->getDim() != 2)
    	throw TkUtil::Exception("Les objets à intersecter doivent être des courbes ou des surfaces");
    if (entity2->getDim() != 1 && entity2->getDim() != 2)
    	throw TkUtil::Exception("Les objets à intersecter doivent être des courbes ou des surfaces");

    // sélection du groupe pour les nouvelles entités en fonction de la dimension des entités intersectées
    if (entity1->getDim() == 1)
    	setDimensionGroup(0);
    else if (entity1->getDim() == 2)
    	setDimensionGroup(1);

}
/*----------------------------------------------------------------------------*/
void CommandCommon2D::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);

}
/*----------------------------------------------------------------------------*/
void CommandCommon2D::internalSpecificPreExecute()
{
    m_impl->prePerform();
    GeomCommon2DImplementation* impl = dynamic_cast<GeomCommon2DImplementation*>(m_impl);
    validate(impl->getEntity1(), impl->getEntity2());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
