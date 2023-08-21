/*----------------------------------------------------------------------------*/
/** \file CommandNewCylinder.cpp
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle
 *
 *  \date 15/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewCylinder.h"
#include "Geom/PropertyCylinder.h"
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
CommandNewCylinder::CommandNewCylinder(Internal::Context& c,
                                       const Utils::Math::Point& pcentre,
                                       const double& dr,
                                       const Utils::Math::Vector& dv,
                                       const double& angle,
                                       const std::string& groupName)
:CommandCreateGeom(c, "Création d'un cylindre", groupName), m_centre(pcentre), m_dr(dr), m_dv(dv),
 m_angle(angle)
{
    validate();
}
/*----------------------------------------------------------------------------*/
CommandNewCylinder::CommandNewCylinder(Internal::Context& c,
                                       const Utils::Math::Point& pcentre,
                                       const double& dr,
                                       const Utils::Math::Vector& dv,
                                       const Utils::Portion::Type& dp,
                                       const std::string& groupName)
:CommandCreateGeom(c, "Création d'un cylindre", groupName), m_centre(pcentre), m_dr(dr), m_dv(dv),
 m_angle(Utils::Portion::getAngleInDegree(dp))
{
    validate();
}
/*----------------------------------------------------------------------------*/
CommandNewCylinder::
~CommandNewCylinder()
{}
/*----------------------------------------------------------------------------*/
void CommandNewCylinder::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewCylinder::execute. Cylindre R = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
		    << Utils::Math::MgxNumeric::userRepresentation (m_dr)
	        << ", V (" << Utils::Math::MgxNumeric::userRepresentation (m_dv.getX ( ))
	        << ", " << Utils::Math::MgxNumeric::userRepresentation (m_dv.getY ( ))
		    << ", " << Utils::Math::MgxNumeric::userRepresentation (m_dv.getZ ( ))
	        << "), P (" << Utils::Math::MgxNumeric::userRepresentation (m_centre.getX ( ))
	        << ", " << Utils::Math::MgxNumeric::userRepresentation (m_centre.getY ( ))
	        << ", " << Utils::Math::MgxNumeric::userRepresentation (m_centre.getZ ( )) << ").";
	Volume* vol = EntityFactory(getContext()).newCylinder(
	               new PropertyCylinder(m_dr,m_dv.abs(),m_centre,m_dv, m_angle));
	m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le
	// manager géom
	split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du cylindre "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewCylinder::
validate()
{
    if (m_dr<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon du cylindre doit être strictement positif", TkUtil::Charset::UTF_8));

    if (Utils::Math::MgxNumeric::isNearlyZero(m_dv.abs2()))
        throw TkUtil::Exception(TkUtil::UTF8String ("La hauteur du cylindre doit être strictement positive", TkUtil::Charset::UTF_8));

    if (m_angle<0.0 || m_angle>360.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewCylinder::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
