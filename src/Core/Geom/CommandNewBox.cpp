/*----------------------------------------------------------------------------*/
/** \file CommandNewBox.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 15/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandNewBox.h"
#include "Geom/PropertyBox.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomManager.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewBox::CommandNewBox(Internal::Context& c,
                             const Utils::Math::Point& pmin,
                             const Utils::Math::Point& pmax,
                             const std::string& groupName)
:CommandCreateGeom(c, "Création d'une boite", groupName), m_pmin(pmin), m_pmax(pmax)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(pmin.getX()-pmax.getX())
       || Utils::Math::MgxNumeric::isNearlyZero(pmin.getY()-pmax.getY())
       || Utils::Math::MgxNumeric::isNearlyZero(pmin.getZ()-pmax.getZ()))
        throw TkUtil::Exception(TkUtil::UTF8String ("La boite doit se faire avec des sommets ayants toutes les coordonnées de distinctes", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandNewBox::
~CommandNewBox()
{}
/*----------------------------------------------------------------------------*/
void CommandNewBox::
internalExecute()
{
    double dx = m_pmax.getX() - m_pmin.getX();
    double dy = m_pmax.getY() - m_pmin.getY();
    double dz = m_pmax.getZ() - m_pmin.getZ();
//    TkUtil::UTF_8   message (Charset::UTF_8);
//	message << "CommandNewBox::execute pour la commande " << getName ( )
//		    << " de nom unique " << getUniqueName ( )
////		    << std::ios_base::fixed << TkUtil::setprecision (8)
//		    << ". P1 (" << Utils::Math::MgxNumeric::userRepresentation (m_pmin.getX ( )) << ", "
//		    << Utils::Math::MgxNumeric::userRepresentation (m_pmin.getY ( ))
//	        << ", " << Utils::Math::MgxNumeric::userRepresentation (m_pmin.getZ ( )) << "), P2 ("
//		    << Utils::Math::MgxNumeric::userRepresentation (m_pmax.getX ( ))
//	        << ", " << Utils::Math::MgxNumeric::userRepresentation (m_pmax.getY ( )) << ", "
//		    << Utils::Math::MgxNumeric::userRepresentation (m_pmax.getZ ( )) << ").";

    Volume* vol = EntityFactory(getContext()).newBox(new PropertyBox(m_pmin,dx,dy,dz));
    m_createdEntities.push_back(vol);

    // découpage du volume en entité de dim inférieure et stockage dans le manager géom
    split(vol);

    // ajoute le volume au groupe s'il y en a un de spécifié
    addToGroup(vol);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de la boite "<<vol->getName();
    setScriptComments(comments);
    setName(comments);

//	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewBox::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
