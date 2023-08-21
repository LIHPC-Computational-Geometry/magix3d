/*----------------------------------------------------------------------------*/
/*
 * CommandNewSpherePart.cpp
 *
 *  Created on: 21 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewSpherePart.h"
#include "Geom/PropertySpherePart.h"
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
CommandNewSpherePart::CommandNewSpherePart(Internal::Context& c,
        const double& dr,
        const double& angleY,
        const double& angleZ,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une aiguille pleine", groupName), m_dr(dr),
m_angleY(angleY), m_angleZ(angleZ)
{}
/*----------------------------------------------------------------------------*/
CommandNewSpherePart::~CommandNewSpherePart()
{}
/*----------------------------------------------------------------------------*/
void CommandNewSpherePart::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewSpherePart::execute. Sphere S = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
            <<Utils::Math::MgxNumeric::userRepresentation (m_dr)
	        <<", angle Y "<<Utils::Math::MgxNumeric::userRepresentation (m_angleY)
            <<", angle Z "<<Utils::Math::MgxNumeric::userRepresentation (m_angleZ);
    Volume* vol = EntityFactory(getContext()).newSpherePart(
                   new PropertySpherePart(m_dr,m_angleY,m_angleZ));

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
void CommandNewSpherePart::
validate()
{
    if (m_dr<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon de l'aiguille doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_angleY<=0.0 || m_angleY>=180.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 180", TkUtil::Charset::UTF_8));
    if (m_angleZ<=0.0 || m_angleZ>=180.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 180", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewSpherePart::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/

