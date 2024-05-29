/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewSysCoord.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 mai 2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Group/Group3D.h"
#include <SysCoord/CommandNewSysCoord.h>
#include <SysCoord/SysCoordManager.h>
#include <SysCoord/SysCoordDisplayRepresentation.h>

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
CommandNewSysCoord::CommandNewSysCoord(Internal::Context& c,
		const std::string& groupName)
: Internal::CommandInternal (c, "Création d'un repère centré")
, m_method(parDefaut)
, m_group_name(groupName)
{
}

CommandNewSysCoord::CommandNewSysCoord(Internal::Context& c,
		const Utils::Math::Point& p,
		const std::string& groupName)
: Internal::CommandInternal (c, "Création d'un repère suivant un centre")
, m_method(unCentre)
, m_center(p)
, m_group_name(groupName)
{
}

CommandNewSysCoord::CommandNewSysCoord(Internal::Context& c,
		const Utils::Math::Point& pCentre,
		const Utils::Math::Point& pX,
		const Utils::Math::Point& pY,
		const std::string& groupName)
: Internal::CommandInternal (c, "Création d'un repère suivant 3 points")
, m_method(unCentreEt2Points)
, m_center(pCentre)
, m_pX(pX)
, m_pY(pY)
, m_group_name(groupName)
{
}

/*----------------------------------------------------------------------------*/
CommandNewSysCoord::~CommandNewSysCoord()
{
    // Suppression par le manager des entités détruites
    Internal::InfoCommand& icmd = getInfoCommand();

    if (!getContext().isFinished())
        for (uint i=0; i<icmd.getNbSysCoordInfoEntity(); i++) {
            SysCoord* rep = 0;
            Internal::InfoCommand::type t;
            icmd.getSysCoordInfoEntity(i, rep, t);

            if (rep){
            	if (Internal::InfoCommand::DELETED == t) {
            		getContext().getLocalSysCoordManager().remove(rep);
            		// destruction des entités que l'on avait construites
            		delete rep;
            	}
            }
            else
            	std::cerr<<"Erreur interne dans ~CommandNewSysCoord, SysCoord nul"<<std::endl;
        } // end for i<icmd.getNbSysCoordInfoEntity()
}
/*----------------------------------------------------------------------------*/
void CommandNewSysCoord::internalExecute()
{
	SysCoord* rep = 0;

	if (m_method == parDefaut)
		rep = new SysCoord(getContext());
	else if (m_method == unCentre)
		rep = new SysCoord(getContext(), m_center);
	else if (unCentreEt2Points)
		rep = new SysCoord(getContext(), m_center, m_pX, m_pY);
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, méthode pour CommandNewSysCoord non prévue", TkUtil::Charset::UTF_8));

	addToGroup (rep, false);

	getContext().getLocalSysCoordManager().add(rep);

	getInfoCommand().addSysCoordInfoEntity(rep, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandNewSysCoord::internalUndo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandNewSysCoord::internalRedo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandNewSysCoord::addToGroup(SysCoord* rep, bool use_default_name)
{
    Group::Group3D* group = getContext().getLocalGroupManager().getNewGroup3D(use_default_name?"":m_group_name, &getInfoCommand());
    //rep->add(group);
    group->add(rep);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandNewSysCoord::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
    getContext().getNameManager().getInternalStats(m_name_manager_before);
	bool hasError = false;
	// effectue la commande
	const bool	graphicalPreviewMode = getContext ( ).isPreviewMode ( );
	getContext ( ).setPreviewMode (true);
	try {
		internalExecute();
		if (Command::CANCELED == getStatus ( ))
			hasError    = true;
	}
	catch (...)
	{
		hasError    = true;
	}
	getContext ( ).setPreviewMode (graphicalPreviewMode);

    // fait le ménage si nécessaire
    postExecute(hasError);
    if (hasError){
        // retour en arrière pour les noms
        getContext().getNameManager().setInternalStats(m_name_manager_before);
        throw TkUtil::Exception("Commande en erreur lors du preview");
    }

	SysCoord* rep = 0;
	Internal::InfoCommand::type t;
	getInfoCommand().getSysCoordInfoEntity(0, rep, t);
	rep->getRepresentation(dr, true);

	SysCoordDisplayRepresentation* scdr = dynamic_cast<SysCoordDisplayRepresentation*> (&dr);
	CHECK_NULL_PTR_ERROR(scdr);

	const Utils::Math::Point	center	= scdr->getTransform ( ).getCenter ( );
	const Utils::Math::Matrix33	transf	= scdr->getTransform ( ).getTransf ( );

    std::vector<Utils::Math::Point>& points = scdr->getPoints();
    std::vector<size_t>& indices = scdr->getCurveDiscretization();

    points.clear();
    indices.clear();

    points.push_back (transf*Utils::Math::Point (0,0,0)+center);
    points.push_back (transf*Utils::Math::Point (1,0,0)+center);
    points.push_back (transf*Utils::Math::Point (0,1,0)+center);
    points.push_back (transf*Utils::Math::Point (0,0,1)+center);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(3);

    // annulation de la commande
    internalUndo();

    // remet les id comme au départ de la commande
    getContext().getNameManager().setInternalStats(m_name_manager_before);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

