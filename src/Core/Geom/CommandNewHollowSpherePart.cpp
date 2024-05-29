/*----------------------------------------------------------------------------*/
/*
 * CommandNewHollowSpherePart.cpp
 *
 *  Created on: 29 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewHollowSpherePart.h"
#include "Geom/PropertyHollowSpherePart.h"
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
CommandNewHollowSpherePart::CommandNewHollowSpherePart(Internal::Context& c,
        const double& dr_int,
        const double& dr_ext,
        const double& angleY,
        const double& angleZ,
        const std::string& groupName)
:CommandCreateGeom(c, "Création d'une aiguille creuse", groupName)
, m_dr_int(dr_int)
, m_dr_ext(dr_ext)
, m_angleY(angleY)
, m_angleZ(angleZ)
{}
/*----------------------------------------------------------------------------*/
CommandNewHollowSpherePart::~CommandNewHollowSpherePart()
{}
/*----------------------------------------------------------------------------*/
void CommandNewHollowSpherePart::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewHollowSpherePart::execute. HollowSpherePart S = "
//            << std::ios_base::fixed << TkUtil::setprecision (8)
            <<" R int "<<Utils::Math::MgxNumeric::userRepresentation (m_dr_int)
    		<<", R ext "<<Utils::Math::MgxNumeric::userRepresentation (m_dr_ext)
	        <<", angle Y "<<Utils::Math::MgxNumeric::userRepresentation (m_angleY)
            <<", angle Z "<<Utils::Math::MgxNumeric::userRepresentation (m_angleZ);
    Volume* vol = EntityFactory(getContext()).newHollowSpherePart(
                   new PropertyHollowSpherePart(m_dr_int, m_dr_ext, m_angleY, m_angleZ));

    m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le
    // manager géom
    split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de l'aiguille creuse "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewHollowSpherePart::
validate()
{
    if (m_dr_int<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon interne de l'aiguille creuse doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_dr_ext<=0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon externe de l'aiguille creuse doit être strictement positif", TkUtil::Charset::UTF_8));

    if (m_dr_ext<=m_dr_int)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le rayon externe de l'aiguille creuse doit être strictement supérieur au rayon interne", TkUtil::Charset::UTF_8));


    if (m_angleY<=0.0 || m_angleY>=180.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 180", TkUtil::Charset::UTF_8));
    if (m_angleZ<=0.0 || m_angleZ>=180.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 180", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandNewHollowSpherePart::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/

