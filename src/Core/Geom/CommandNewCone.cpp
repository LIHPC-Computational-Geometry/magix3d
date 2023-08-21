/*----------------------------------------------------------------------------*/
/** \file CommandNewCone.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/5/2014
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewCone.h"
#include "Geom/PropertyCone.h"
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
CommandNewCone::CommandNewCone(Internal::Context& c,
		const double& dr1,
		const double& dr2,
		const Utils::Math::Vector& dv,
		const double& angle,
		const std::string& groupName)
:CommandCreateGeom(c, "Création d'un cône", groupName), m_dr1(dr1), m_dr2(dr2), m_dv(dv),
 m_angle(angle)
{
    validate();
}
/*----------------------------------------------------------------------------*/
CommandNewCone::CommandNewCone(Internal::Context& c,
		const double& dr1,
		const double& dr2,
		const Utils::Math::Vector& dv,
		const Utils::Portion::Type& dp,
		const std::string& groupName)
:CommandCreateGeom(c, "Création d'un cône", groupName), m_dr1(dr1), m_dr2(dr2), m_dv(dv),
 m_angle(Utils::Portion::getAngleInDegree(dp))
{
    validate();
}
/*----------------------------------------------------------------------------*/
CommandNewCone::
~CommandNewCone()
{}
/*----------------------------------------------------------------------------*/
void CommandNewCone::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewCone::execute. Cone R1 = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
    << Utils::Math::MgxNumeric::userRepresentation (m_dr1)
    << ", R2 " << Utils::Math::MgxNumeric::userRepresentation (m_dr2)
    << ", V (" << Utils::Math::MgxNumeric::userRepresentation (m_dv.getX ( ))
    << ", " << Utils::Math::MgxNumeric::userRepresentation (m_dv.getY ( ))
    << ", " << Utils::Math::MgxNumeric::userRepresentation (m_dv.getZ ( ))
    << ").";
	Volume* vol = EntityFactory(getContext()).newCone(
	               new PropertyCone(m_dr1, m_dr2, m_dv.abs(), m_dv, m_angle));
	m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le
	// manager géom
	split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du cône "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewCone::
validate()
{
    if (m_dr1<0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le 1er rayon du cône doit être positif", TkUtil::Charset::UTF_8));
    if (m_dr2<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le 2ème rayon du cône doit être strictement positif", TkUtil::Charset::UTF_8));

    if (Utils::Math::MgxNumeric::isNearlyZero(m_dv.abs2()))
        throw TkUtil::Exception(TkUtil::UTF8String ("Le vecteur de direction du cône doit être de taille strictement positive", TkUtil::Charset::UTF_8));

    if (m_angle<0.0 || m_angle>360.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewCone::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
