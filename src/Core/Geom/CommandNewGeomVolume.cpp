/*----------------------------------------------------------------------------*/
/*
 * CommandNewGeomVolume.cpp
 *
 *  Created on: 3 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewGeomVolume.h"
#include "Geom/GeomNewVolumeImplementation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
//class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandNewGeomVolume::
CommandNewGeomVolume(Internal::Context& c,std::vector<Surface*>& es, const std::string& groupName)
: CommandEditGeom(c, "Création d'un volume", groupName), m_surfaces(es)
{
}
/*----------------------------------------------------------------------------*/
CommandNewGeomVolume::~CommandNewGeomVolume()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandNewGeomVolume::validate()
{
}
/*----------------------------------------------------------------------------*/
void CommandNewGeomVolume::internalSpecificPreExecute()
{
    validate();

    m_impl = new GeomNewVolumeImplementation(getContext(), m_surfaces);

    m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandNewGeomVolume::internalSpecificExecute()
{
    m_impl->perform(m_createdEntities);

    // information un peu plus parlante
    if (m_createdEntities.size() == 1){
    	TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    	comments << "Création du volume "<<m_createdEntities[0]->getName();
    	setScriptComments(comments);
    	setName(comments);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
