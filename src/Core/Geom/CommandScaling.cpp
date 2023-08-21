/*
 * CommandScaling.cpp
 *
 *  Created on: 19 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandScaling.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomScaleImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandScaling::CommandScaling(Internal::Context& c,
		std::vector<GeomEntity*>& entities,
		const double factor,
		const Point& pcentre)
: CommandEditGeom(c, "Homothétie")
, m_factor(factor)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
{
	validate();
	m_impl = new GeomScaleImplementation(c, entities, m_factor, pcentre);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	comments << " par rapport à "<<pcentre<<" et de facteur "
			<<Utils::Math::MgxNumeric::userRepresentation(m_factor);
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaling::CommandScaling(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
		const double factor,
		const Point& pcentre)
: CommandEditGeom(c, "Homothétie")
, m_factor(factor)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
{
	validate();
	m_impl = new GeomScaleImplementation(c, cmd, m_factor, pcentre);
}
/*----------------------------------------------------------------------------*/
CommandScaling::CommandScaling(Internal::Context& c,
		const double factor,
        const Point& pcentre)
: CommandEditGeom(c, "Homothétie de tout")
, m_factor(factor)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
{
	validate();
	m_impl = new GeomScaleImplementation(c, m_factor, pcentre);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de tout par rapport à "<<pcentre<<" et de facteur "
			<<Utils::Math::MgxNumeric::userRepresentation(m_factor);
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaling::CommandScaling(Internal::Context& c,
		std::vector<GeomEntity*>& entities,
        const double factorX,
        const double factorY,
        const double factorZ)
: CommandEditGeom(c, "Homothétie")
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
{
	validate();
	m_impl = new GeomScaleImplementation(c, entities, m_factorX, m_factorY, m_factorZ);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de";
	for (uint i=0; i<entities.size() && i<5; i++)
		comments << " " << entities[i]->getName();
	if (entities.size()>5)
		comments << " ... ";
	comments << " de facteur "
			<<Utils::Math::MgxNumeric::userRepresentation(m_factorX)
	        <<", "<<Utils::Math::MgxNumeric::userRepresentation(m_factorY)
	        <<", "<<Utils::Math::MgxNumeric::userRepresentation(m_factorZ);
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaling::CommandScaling(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const double factorX,
        const double factorY,
        const double factorZ)
: CommandEditGeom(c, "Homothétie")
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
{
	validate();
	m_impl = new GeomScaleImplementation(c, cmd, m_factorX, m_factorY, m_factorZ);
}
/*----------------------------------------------------------------------------*/
CommandScaling::CommandScaling(Internal::Context& c,
        const double factorX,
        const double factorY,
        const double factorZ)
: CommandEditGeom(c, "Homothétie de tout")
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
{
	validate();
	m_impl = new GeomScaleImplementation(c, m_factorX, m_factorY, m_factorZ);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de tout de facteur "
			<<Utils::Math::MgxNumeric::userRepresentation(m_factorX)
		    <<", "<<Utils::Math::MgxNumeric::userRepresentation(m_factorY)
			<<", "<<Utils::Math::MgxNumeric::userRepresentation(m_factorZ);
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaling::~CommandScaling() {
	if (m_impl)
		delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandScaling::validate() {
	if (m_isHomogene){
		if (Utils::Math::MgxNumeric::isNearlyZero(m_factor))
			throw TkUtil::Exception(TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul", TkUtil::Charset::UTF_8));
	}
	else {
		if (Utils::Math::MgxNumeric::isNearlyZero(m_factorX))
			throw TkUtil::Exception(TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant X", TkUtil::Charset::UTF_8));
		if (Utils::Math::MgxNumeric::isNearlyZero(m_factorY))
			throw TkUtil::Exception(TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Y", TkUtil::Charset::UTF_8));
		if (Utils::Math::MgxNumeric::isNearlyZero(m_factorZ))
			throw TkUtil::Exception(TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Z", TkUtil::Charset::UTF_8));
	}

}
/*----------------------------------------------------------------------------*/
void CommandScaling::internalSpecificExecute() {
	m_impl->perform(m_createdEntities);

}
/*----------------------------------------------------------------------------*/
void CommandScaling::internalSpecificPreExecute() {
	m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandScaling::internalSpecificUndo()
{
    m_impl->performUndo();
}
/*----------------------------------------------------------------------------*/
void CommandScaling::internalSpecificRedo()
{
    m_impl->performRedo();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
