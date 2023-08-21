/*----------------------------------------------------------------------------*/
/*
 * CommandNewHollowSphere.cpp
 *
 *  Created on: 23 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewHollowSphere.h"
#include "Geom/PropertyHollowSphere.h"
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
CommandNewHollowSphere::CommandNewHollowSphere(Internal::Context& c,
        const Utils::Math::Point& pcentre,
        const double& dr_int,
        const double& dr_ext,
        const double& angle,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une sphère creuse", groupName), m_center(pcentre),
 m_dr_int(dr_int),
 m_dr_ext(dr_ext),
 m_angle(angle),m_type(Utils::Portion::ANGLE_DEF)
{
	validate();
}
/*----------------------------------------------------------------------------*/
CommandNewHollowSphere::CommandNewHollowSphere(Internal::Context& c,
        const Utils::Math::Point& pcentre,
        const double& dr_int,
        const double& dr_ext,
        const Utils::Portion::Type& dp,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une sphère creuse", groupName), m_center(pcentre),
 m_dr_int(dr_int),
 m_dr_ext(dr_ext),
 m_angle(0), m_type(dp)
{
	if (dp == 0){
		MGX_FORBIDDEN("Sphère creuse avec portion indéfinie")
	}
	validate();
}
/*----------------------------------------------------------------------------*/
CommandNewHollowSphere::~CommandNewHollowSphere()
{}
/*----------------------------------------------------------------------------*/
void CommandNewHollowSphere::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewHollowSphere::execute. HollowSphere S = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
            <<"C (" << Utils::Math::MgxNumeric::userRepresentation (m_center.getX ( ))
            << Utils::Math::MgxNumeric::userRepresentation (m_center.getY ( )) << ", "
	        << Utils::Math::MgxNumeric::userRepresentation (m_center.getZ ( )) << "), R int "
            <<Utils::Math::MgxNumeric::userRepresentation (m_dr_int)<< "), R ext "
            <<Utils::Math::MgxNumeric::userRepresentation (m_dr_ext)
            <<" Type "<<Utils::Portion::getName(m_type)
	        <<", angle "<<Utils::Math::MgxNumeric::userRepresentation (m_angle);
    Volume* vol = EntityFactory(getContext()).newHollowSphere(
                   new PropertyHollowSphere(m_center, m_dr_int, m_dr_ext,m_type,m_angle));

    m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le
    // manager géom
    split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de la sphère creuse "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewHollowSphere::
validate()
{
    if (m_dr_int<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon interne de la sphère creuse doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_dr_ext<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon externe de la sphère creuse doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_dr_ext<=m_dr_int)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon externe de la sphère creuse doit être strictement supérieur au rayon interne", TkUtil::Charset::UTF_8));

    if (m_angle<0.0 || m_angle>360.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewHollowSphere::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/

