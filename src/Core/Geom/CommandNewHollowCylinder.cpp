/*----------------------------------------------------------------------------*/
/*
 * CommandNewHollowCylinder.cpp
 *
 *  Created on: 12 nov. 2014
 *      Author: ledouxf
 */

/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewHollowCylinder.h"
#include "Geom/PropertyHollowCylinder.h"
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
CommandNewHollowCylinder::CommandNewHollowCylinder(Internal::Context& c,
                                       const Utils::Math::Point& pcentre,
                                       const double& dr_int,
                                       const double& dr_ext,
                                       const Utils::Math::Vector& dv,
                                       const double& angle,
                                       const std::string& groupName)
:CommandCreateGeom(c, "Création d'un cylindre creux", groupName), m_centre(pcentre),
 m_dr_int(dr_int), m_dr_ext(dr_ext), m_dv(dv),
 m_angle(angle)
{
    validate();
}
/*----------------------------------------------------------------------------*/
CommandNewHollowCylinder::CommandNewHollowCylinder(Internal::Context& c,
                                       const Utils::Math::Point& pcentre,
                                       const double& dr_int,
                                       const double& dr_ext,
                                       const Utils::Math::Vector& dv,
                                       const Utils::Portion::Type& dp,
                                       const std::string& groupName)
:CommandCreateGeom(c, "Création d'un cylindre creux", groupName), m_centre(pcentre),
 m_dr_int(dr_int), m_dr_ext(dr_ext), m_dv(dv),
 m_angle(Utils::Portion::getAngleInDegree(dp))
{
    validate();
}
/*----------------------------------------------------------------------------*/
CommandNewHollowCylinder::
~CommandNewHollowCylinder()
{}
/*----------------------------------------------------------------------------*/
void CommandNewHollowCylinder::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewHollowCylinder::execute. HollowCylindre R = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
            << Utils::Math::MgxNumeric::userRepresentation (m_dr_int)
            <<", "<< Utils::Math::MgxNumeric::userRepresentation (m_dr_ext)
            << ", V (" << Utils::Math::MgxNumeric::userRepresentation (m_dv.getX ( ))
            << ", " << Utils::Math::MgxNumeric::userRepresentation (m_dv.getY ( ))
            << ", " << Utils::Math::MgxNumeric::userRepresentation (m_dv.getZ ( ))
            << "), P (" << Utils::Math::MgxNumeric::userRepresentation (m_centre.getX ( ))
            << ", " << Utils::Math::MgxNumeric::userRepresentation (m_centre.getY ( ))
            << ", " << Utils::Math::MgxNumeric::userRepresentation (m_centre.getZ ( )) << ").";
    Volume* vol = EntityFactory(getContext()).newHollowCylinder(
                   new PropertyHollowCylinder(m_dr_int, m_dr_ext,m_dv.abs(),m_centre,m_dv, m_angle));
    m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le
    // manager géom
    split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du cylindre creux "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewHollowCylinder::
validate()
{
    if (m_dr_int<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon interne du cylindre creux doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_dr_ext<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon externe du cylindre creux doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_dr_ext<=m_dr_int)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon externe du cylindre creux doit être strictement supérieur au rayon interne du cylindre creux", TkUtil::Charset::UTF_8));

    if (Utils::Math::MgxNumeric::isNearlyZero(m_dv.abs2()))
        throw TkUtil::Exception(TkUtil::UTF8String ("La hauteur du cylindre doit être strictement positive", TkUtil::Charset::UTF_8));

    if (m_angle<0.0 || m_angle>360.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewHollowCylinder::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
