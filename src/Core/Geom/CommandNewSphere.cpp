/*----------------------------------------------------------------------------*/
/*
 * CommandNewSphere.cpp
 *
 *  Created on: 23 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewSphere.h"
#include "Geom/PropertySphere.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <cmath>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewSphere::CommandNewSphere(Internal::Context& c,
        const Utils::Math::Point& pcentre,
        const double& dr,
        const double& angle,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une sphère", groupName), m_center(pcentre), m_dr(dr),
m_angle(angle),m_type(Utils::Portion::ANGLE_DEF)
{
	validate();
}
/*----------------------------------------------------------------------------*/
CommandNewSphere::CommandNewSphere(Internal::Context& c,
        const Utils::Math::Point& pcentre,
        const double& dr,
        const Utils::Portion::Type& dp,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une sphère", groupName), m_center(pcentre), m_dr(dr),
 m_angle(0), m_type(dp)
{
	if (dp == 0){
		MGX_FORBIDDEN("Sphère pleine avec portion indéfinie")
	}
	validate();
}
/*----------------------------------------------------------------------------*/
CommandNewSphere::~CommandNewSphere()
{}
/*----------------------------------------------------------------------------*/
void CommandNewSphere::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewSphere::execute. Sphere S = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
            <<"C (" << Utils::Math::MgxNumeric::userRepresentation (m_center.getX ( ))
            << Utils::Math::MgxNumeric::userRepresentation (m_center.getY ( )) << ", "
	        << Utils::Math::MgxNumeric::userRepresentation (m_center.getZ ( )) << "), R "
            <<Utils::Math::MgxNumeric::userRepresentation (m_dr)
            <<" Type "<<Utils::Portion::getName(m_type)
	        <<", angle "<<Utils::Math::MgxNumeric::userRepresentation (m_angle);
    Volume* vol = EntityFactory(getContext()).newSphere(
                   new PropertySphere(m_center, m_dr,m_type,m_angle));

    m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le
    // manager géom
    split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du volume "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewSphere::
validate()
{
    if (m_dr<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon de la sphère doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_angle<0.0 || m_angle>360.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewSphere::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/

