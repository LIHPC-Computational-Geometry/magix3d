/*----------------------------------------------------------------------------*/
/** \file GeomManager.cpp
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle
 *
 *  \date 14/10/2010
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités géométriques
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/GeomInfo.h"
#include "Geom/CommandRemove.h"
#include "Geom/CommandNewHollowCylinder.h"
#include "Geom/CommandNewHollowSphere.h"
#include "Geom/CommandNewVertex.h"
#include "Geom/CommandNewSegment.h"
#include "Geom/CommandNewSurface.h"
#include "Geom/CommandNewSurfaceByOffset.h"
#include "Geom/CommandNewFacetedSurfaces.h"
#include "Geom/CommandNewSphere.h"
#include "Geom/CommandNewSpherePart.h"
#include "Geom/CommandNewHollowSpherePart.h"
#include "Geom/CommandNewBox.h"
#include "Geom/CommandNewCylinder.h"
#include "Geom/CommandNewCone.h"
#include "Geom/CommandNewPrism.h"
#include "Geom/CommandGeomCopy.h"
#include "Geom/CommandNewCircle.h"
#include "Geom/CommandNewEllipse.h"
#include "Geom/CommandNewCurveByCurveProjectionOnSurface.h"
#include "Geom/CommandNewArcCircle.h"
#include "Geom/CommandNewArcCircleWithAngles.h"
#include "Geom/CommandNewArcEllipse.h"
#include "Geom/CommandNewBSpline.h"
#include "Geom/CommandTranslation.h"
#include "Geom/CommandImportBREP.h"
#include "Geom/CommandImportIGES.h"
#include "Geom/CommandImportSTL.h"
#include "Geom/CommandImportSTEP.h"
#include "Geom/CommandImportCATIA.h"
#include "Geom/CommandImportMDL.h"
#include "Geom/CommandExportMDL.h"
#include "Geom/CommandExportVTK.h"
#include "Geom/CommandExportMLI.h"
#include "Geom/CommandExportBREP.h"
#include "Geom/CommandExportSTEP.h"
#include "Geom/CommandExportSTL.h"
#include "Geom/CommandExportIGES.h"
#include "Geom/CommandFuse.h"
#include "Geom/CommandCut.h"
#include "Geom/CommandCommon.h"
#include "Geom/CommandCommon2D.h"
#include "Geom/CommandGluing.h"
#include "Geom/CommandSection.h"
#include "Geom/CommandSectionByPlane.h"
#include <Geom/CommandExtrudeRevolution.h>
#include "Geom/CommandExtrudeDirection.h"
#include "Geom/CommandRotation.h"
#include "Geom/CommandScaling.h"
#include "Geom/CommandMirroring.h"
#include "Geom/GeomRepresentation.h"
#include "Geom/CommandNewVertexByProjection.h"
#include "Geom/CommandNewVertexByCurveParameterization.h"
#include "Geom/CommandJoinCurves.h"
#include "Geom/CommandJoinSurfaces.h"
#include "Geom/CommandNewGeomVolume.h"
#include "Geom/CommandSplitCurve.h"
#include "Internal/ImportMDLImplementation.h"
#include "Internal/ImportMDL2Commandes.h"
#include "Internal/ExportMDLImplementation.h"
#include "Internal/Context.h"
#include "Internal/NameManager.h"
#include "Internal/CommandComposite.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandChangeLengthUnit.h"

#include "Utils/Entity.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Plane.h"
#include "Utils/Vector.h"
#include "Utils/Rotation.h"
#include "Utils/CommandManager.h"
#include "Utils/TypeDedicatedNameManager.h"
#include "Utils/MgxException.h"

#include "Group/GroupManager.h"
#include "Group/Group3D.h"

#include "Mesh/CommandAddRemoveGroupName.h"
#include "Mesh/CommandTranslateMesh.h"
#include "Mesh/CommandRotateMesh.h"
#include "Mesh/CommandScaleMesh.h"

#include "Topo/CommandModificationTopo.h"
#include "Topo/CommandTranslateTopo.h"
#include "Topo/CommandRotateTopo.h"
#include "Topo/CommandScaleTopo.h"
#include "Topo/CommandMirrorTopo.h"
#include "Topo/CommandDuplicateTopo.h"
#include "Topo/CommandSetGeomAssociation.h"
#include "Topo/CommandDestroyTopo.h"
#include "Topo/CommandExtrudeTopo.h"
#include "Topo/TopoHelper.h"

#include <SysCoord/SysCoord.h>

/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>

#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomManager::GeomManager(const std::string& name, Internal::ContextIfc* c)
:Geom::GeomManagerIfc(name, c)
{
}
/*----------------------------------------------------------------------------*/
GeomManager::~GeomManager()
{
	clear();
}
/*----------------------------------------------------------------------------*/
void GeomManager::clear()
{
    // destruction des entités référencées par le manager
    for (std::vector<Volume*>::const_iterator iter = m_volumes.begin();
            iter != m_volumes.end(); ++iter)
        delete *iter;
    m_volumes.clear();

    for (std::vector<Surface*>::const_iterator iter = m_surfaces.begin();
            iter != m_surfaces.end(); ++iter)
        delete *iter;
    m_surfaces.clear();

    for (std::vector<Curve*>::const_iterator iter = m_curves.begin();
            iter != m_curves.end(); ++iter)
        delete *iter;
    m_curves.clear();

    for (std::vector<Vertex*>::const_iterator iter = m_vertices.begin();
            iter != m_vertices.end(); ++iter)
        delete *iter;
    m_vertices.clear();
}
/*----------------------------------------------------------------------------*/
Geom::GeomInfo GeomManager::getInfos(std::string name, int dim)
{
    GeomEntity* e = 0;
    switch(dim){
    case(0):{
        e =getVertex(name);
    }
    break;
    case(1):{
        e =getCurve(name);
    }
    break;
    case(2):{
        e =getSurface(name);
    }
    break;
    case(3):{
        e =getVolume(name);
    }
    break;
    default:{
        throw TkUtil::Exception (TkUtil::UTF8String ("Dimension erronée", TkUtil::Charset::UTF_8));
    }
    break;
    }
    return getInfos(e);
}
/*----------------------------------------------------------------------------*/
Geom::GeomInfo GeomManager::getInfos(const GeomEntity* e)
{
    Geom::GeomInfo infos;
    infos.name = e->getName();
    infos.dimension = e->getDim();
    infos.area = e->computeArea();

    e->get(infos.incident_vertices);
    e->get(infos.incident_curves);
    e->get(infos.incident_surfaces);
    e->get(infos.incident_volumes);
    e->getRefTopo(infos.topo_entities);
    e->getGroupsName(infos.groups_name);
    return infos;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point GeomManager::getCoord(const std::string& name) const
{
	Vertex* vtx = GeomManager::getVertex (name);
	return vtx->getCoord();
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copy(std::vector<std::string>& e, bool withTopo, std::string groupName)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<e.size(); i++)
        vge.push_back(getEntity(e[i]));

    return copy(vge, withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copy(std::vector<GeomEntity*>& e, bool withTopo, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::copy (";
    for(unsigned int i=0;i<e.size();i++){
        if(i!=0)
            message<<", ";
        message << e[i]->getName();
    }
    message<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (withTopo && Internal::EntitiesHelper::hasTopoRef(e)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Copie de géométries avec topologies");

        CommandGeomCopy *commandGeom =
                new CommandGeomCopy(getLocalContext(),e, groupName);
        commandCompo->addCommand(commandGeom);

        Topo::CommandDuplicateTopo* commandTopo =
                new Topo::CommandDuplicateTopo(getLocalContext(), commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new CommandGeomCopy(getLocalContext(),e,groupName);
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copy([";
    for(unsigned int i=0;i<e.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< e[i]->getName()<<"\"";
    }
    if(withTopo)
        cmd <<"], True,\""<<groupName<<"\")";
    else
        cmd <<"], False,\""<<groupName<<"\")";

    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;

}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVolume(std::vector<std::string>& e, std::string groupName)
{
    std::vector<Surface*> vge;
    for (uint i=0; i<e.size(); i++)
        vge.push_back(getSurface(e[i],true));

    return newVolume(vge, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVolume(std::vector<Surface*>& e, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVolume (";
    for(unsigned int i=0;i<e.size();i++){
        if(i!=0)
            message<<", ";
        message << e[i]->getName();
    }
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewGeomVolume* command =
            new CommandNewGeomVolume(getLocalContext(),e, groupName);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newVolume([";
    for(unsigned int i=0;i<e.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< e[i]->getName()<<"\"";
    }

    cmd <<"],\""<<groupName<<"\")";

    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;

}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(std::string vertexName,std::string curveName, std::string groupName){
    Vertex *v = getVertex(vertexName);
    Curve  *c = getCurve(curveName, false);
    Surface *s = getSurface(curveName, false);

    if (c)
    	return newVertex(v,c, groupName);
    else if (s)
    	return newVertex(v,s, groupName);
    else {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message <<"newVertex impossible, entité \""<<curveName<<"\" n'a pas été trouvée dans le GeomManager (ni une courbe ni une surface)";
    	throw TkUtil::Exception(message);
    }
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(const Vertex* ref,  Curve* curve, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVertex("<<ref->getName()<<", "<<curve->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewVertexByProjection *command =
            new CommandNewVertexByProjection(getLocalContext(),ref,curve, groupName);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd <<getContextAlias()<< "." << "getGeomManager().newVertex (\""
            <<ref->getName()<<"\", \""<<curve->getName();
    if (!groupName.empty())
        cmd<<"\", \""<<groupName;
    cmd<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(const Vertex* ref,  Surface* surface, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVertex("<<ref->getName()<<", "<<surface->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewVertexByProjection *command =
            new CommandNewVertexByProjection(getLocalContext(),ref,surface, groupName);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd <<getContextAlias()<< "." << "getGeomManager().newVertex (\""
            <<ref->getName()<<"\", \""<<surface->getName();
    if (!groupName.empty())
        cmd<<"\", \""<<groupName;
    cmd<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(std::string curveName, const double& param, std::string groupName)
{
    Curve  *c = getCurve(curveName);
    return newVertex(c,param, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(Curve* curve, const double& param, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVertex("<<curve->getName()<<", "<<param<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewVertexByCurveParameterization *command =
            new CommandNewVertexByCurveParameterization(getLocalContext(),curve, param, groupName);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd <<getContextAlias()<< "." << "getGeomManager().newVertex (\""
            <<curve->getName()<<"\", "<<param;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(std::string vertex1Name, std::string vertex2Name, const double& param, std::string groupName)
{
    Vertex* v1 = getVertex(vertex1Name);
    Vertex* v2 = getVertex(vertex2Name);
    return newVertex(v1, v2 ,param, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(const Vertex* ref1, const Vertex* ref2, const double& param, std::string groupName)
{
    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVertex("<<ref1->getName()<<", "<<ref2->getName()<<", "<<param<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Point p1 = ref1->getPoint();
    Point p2 = ref2->getPoint();

    if (p1 == p2){
        TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"newVertex impossible, entités "<<ref1->getName()<<" et "<<ref2->getName()<<" trop proches";
        throw TkUtil::Exception(message);
    }

    Point p = p1 + (p2-p1)*param;

    //creation de la commande de création
    CommandNewVertex *command =
            new CommandNewVertex(getLocalContext(),p, groupName);

    // trace dans le script
    TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd <<getContextAlias()<< "." << "getGeomManager().newVertex (\""
        <<ref1->getName()<<"\", \""<<ref2->getName()<<"\", "<<param;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(const double& x, const double& y, const double& z, std::string groupName)
{
    return newVertex(Point(x,y,z), groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertex(const Point& p, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVertex ("<<p<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewVertex *command = new CommandNewVertex(getLocalContext(),p, groupName);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newVertex ("<<p.getScriptCommand();
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newVertexFromTopo(std::string vertexName, bool asso, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "GeomManager::newVertexFromTopo(" << vertexName << ", " << (true == asso ? "True" : "False") << ", " << groupName << ")";
	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));
	Topo::Vertex*	vertex	= getLocalContext ().getTopoManager ( ).getVertex (vertexName, true);
	CHECK_NULL_PTR_ERROR (vertex)
	Internal::CommandComposite* commandCompo = new Internal::CommandComposite(getLocalContext(), "Création d'un sommet géométrique à partir d'un sommet topologique.");
	CHECK_NULL_PTR_ERROR (commandCompo)
	CommandNewVertex*	commandVertex = new CommandNewVertex(getLocalContext(),vertex->getCoord ( ), groupName);
	CHECK_NULL_PTR_ERROR (commandVertex)
	commandCompo->addCommand (commandVertex);
	std::vector<Topo::TopoEntity*>	verticies;
	verticies.push_back (vertex);
	if (true == asso)
	{
		Topo::CommandSetGeomAssociation* commandAsso = new Topo::CommandSetGeomAssociation(getLocalContext(), verticies, commandVertex);
		CHECK_NULL_PTR_ERROR (commandAsso)
		commandCompo->addCommand (commandAsso);
	}	// if (true == asso)

	// Trace dans le script :
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newVertexFromTopo(\"" << vertexName << "\", " << (true == asso ? "True" : "False") << ",\"" << groupName << "\")";
    commandCompo->setScriptCommand(cmd);

    getCommandManager().addCommand(commandCompo, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   = new Internal::M3DCommandResult (*commandCompo);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newSegment(std::string n1,std::string n2, std::string groupName)
{
    Vertex* v1 = getVertex(n1);
    Vertex* v2 = getVertex(n2);
    return newSegment(v1,v2, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newSegment( Geom::Vertex* v1,  Geom::Vertex* v2, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newSegment ("<<v1->getCoord()<<","<<v2->getCoord()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewSegment*command = new CommandNewSegment(getLocalContext(),v1,v2, groupName);
    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newSegment(\""
            <<v1->getName()<<"\", \""<<v2->getName();
    if (!groupName.empty())
        cmd<<"\", \""<<groupName;
    cmd<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newPlanarSurface(std::vector<std::string>& curve_names, std::string groupName)
{
    const int nb_curves = curve_names.size();
    std::vector<Geom::Curve* > curves;
    curves.reserve(nb_curves);
    for(int i=0;i<nb_curves;i++)
        curves.push_back(getCurve(curve_names[i]));
    return newPlanarSurface(curves, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newPlanarSurface(const std::vector<Geom::Curve* >& curves, std::string groupName )
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newPlanarSurface from curves";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewSurface* command = new CommandNewSurface(getLocalContext(),curves, groupName);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newPlanarSurface( [\"";
    for(unsigned int i=0;i<curves.size();i++)
    {
        if (i!=0)
            cmd <<"\", \"";
        cmd << curves[i]->getName();
    }
    cmd <<"\"] , \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newSurfaceByOffset(std::string name, const double& offset, std::string groupName)
{
	return newSurfaceByOffset(getSurface(name), offset, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newSurfaceByOffset(Surface* base, const double& offset, std::string groupName)
{
	TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
	message << "GeomManager::newSurfaceByOffset("<<base->getName()<<", "<<offset;
	if (!groupName.empty())
		message<<", "<<groupName;
	message<<")";
	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

	Internal::CommandInternal* command = new CommandNewSurfaceByOffset(getLocalContext(), base, offset, groupName);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << "." << "getGeomManager().newSurfaceByOffset (\""<<base->getName()<<"\", "
			<<offset;
	if (!groupName.empty())
		cmd<<", \""<<groupName<<"\"";
	cmd<<")";
	command->setScriptCommand(cmd);

	getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* GeomManager::
newVerticesCurvesAndPlanarSurface(std::vector<Point>& points, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newVerticesCurvesAndPlanarSurface from points";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* commandCompo =
    		new Internal::CommandComposite(getLocalContext(), "Création d'une surface avec vecteur de points");

    uint nbPts = points.size();

    //std::cout<<"newVerticesCurvesAndPlanarSurface avec "<<nbPts<<" points"<<std::endl;
    enum typeCrv {undef, segment, bspline};
    std::vector<Geom::CommandCreateGeom*> cmdsVtx;
    // création du premier sommet
    CommandNewVertex *commandVtx = new CommandNewVertex(getLocalContext(),points[0], groupName);
    cmdsVtx.push_back(commandVtx);
    commandCompo->addCommand(commandVtx);

    // on le met de côté pour la fin
    CommandNewVertex *commandVtx_first = commandVtx;

    Vector v1 = points[1] - points[0];
    v1 = v1/v1.norme();
    typeCrv type_crv = undef;

    // paramètre de déviation:
    // nearlyZero si colinéaires,
    // max_dev pour BSpline,
    // nouvelle entité si plus
    double max_dev = 0.05;
    double nearlyZero = 1e-4;

    // stockage des commandes des contours pour la surface
    std::vector<Geom::CommandCreateGeom*> cmdsCrv;

    for (uint i=1; i<nbPts; i++){
    	Vector v2 = points[(i+1)%nbPts] - points[i];
    	v2 = v2/v2.norme();
    	double dev = (v1*v2).norme();

    	//std::cout<<" v["<<i-1<<"]*v["<<i<<"] = "<<dev<<std::endl;

    	if (type_crv == undef){
    		if (Mgx3D::Utils::Math::MgxNumeric::isNearlyZero(dev, nearlyZero)){
    			type_crv = segment;
    			// on ne fait rien de particulier, on passe les points internes
    		} else if (dev < max_dev){
    			type_crv = bspline;
    			// construction du point et stokage
    			CommandNewVertex *commandVtx = new CommandNewVertex(getLocalContext(),points[i], groupName);
    			cmdsVtx.push_back(commandVtx);
    			commandCompo->addCommand(commandVtx);
    		} else {
    			// construction du point
    			CommandNewVertex *commandVtx = new CommandNewVertex(getLocalContext(),points[i], groupName);
    			cmdsVtx.push_back(commandVtx);
    			commandCompo->addCommand(commandVtx);

    			// construction du segment
    	    	CommandNewSegment *commandSgt = new CommandNewSegment(getLocalContext(),cmdsVtx[0], cmdsVtx[1], groupName);
    	    	cmdsCrv.push_back(commandSgt);
    	    	commandCompo->addCommand(commandSgt);
    	    	//std::cout<<" construction d'un segment"<<std::endl;

    	    	// réinitialise le conteneur de sommets pour le prochain contour avec uniquement ce dernier sommet
    	    	CommandCreateGeom* commandLast = cmdsVtx.back();
    	    	cmdsVtx.clear();
    	    	cmdsVtx.push_back(commandLast);
    		}

    	} else if (type_crv == segment){
       		if (Mgx3D::Utils::Math::MgxNumeric::isNearlyZero(dev, nearlyZero)){
        			// on ne fait rien de particulier, on passe les points internes
        		} else {
        			// construction du point
        			CommandNewVertex *commandVtx = new CommandNewVertex(getLocalContext(),points[i], groupName);
        			cmdsVtx.push_back(commandVtx);
        			commandCompo->addCommand(commandVtx);

        			// construction du segment
        	    	CommandNewSegment *commandSgt = new CommandNewSegment(getLocalContext(),cmdsVtx[0], cmdsVtx[1], groupName);
        	    	cmdsCrv.push_back(commandSgt);
        	    	commandCompo->addCommand(commandSgt);
        	    	//std::cout<<" construction d'un segment"<<std::endl;

        	    	// réinitialise le conteneur de sommets pour le prochain contour avec uniquement ce dernier sommet
        	    	CommandCreateGeom* commandLast = cmdsVtx.back();
        	    	cmdsVtx.clear();
        	    	cmdsVtx.push_back(commandLast);

        	    	type_crv = undef;
        		}

    	} else if (type_crv == bspline) {

			// construction du point
			CommandNewVertex *commandVtx = new CommandNewVertex(getLocalContext(),points[i], groupName);
			cmdsVtx.push_back(commandVtx);
			commandCompo->addCommand(commandVtx);

			if (dev < max_dev){
    			// rien à faire de plus
    		} else {

    			// construction de la bspline
    			CommandNewBSpline *commandCrv = new CommandNewBSpline(getLocalContext(),cmdsVtx, 1, 2, groupName);
    			cmdsCrv.push_back(commandCrv);
    			commandCompo->addCommand(commandCrv);
    	    	//std::cout<<" construction d'une b-spline"<<std::endl;

    			// réinitialise le conteneur de sommets pour le prochain contour avec uniquement ce dernier sommet
    	    	CommandCreateGeom* commandLast = cmdsVtx.back();
    	    	cmdsVtx.clear();
    	    	cmdsVtx.push_back(commandLast);

    			type_crv = undef;
    		}

    	} else
        	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, type_crv non prévu", TkUtil::Charset::UTF_8));

    	v1 = v2;
    } // end for i

    // on termine le dernier morceau pour boucler avec le premier sommet
    cmdsVtx.push_back(commandVtx_first);
    if (type_crv == undef || type_crv == segment){
		// construction du segment
    	CommandNewSegment *commandSgt = new CommandNewSegment(getLocalContext(),cmdsVtx[0], cmdsVtx[1], groupName);
    	cmdsCrv.push_back(commandSgt);
    	commandCompo->addCommand(commandSgt);
    	//std::cout<<" construction d'un segment"<<std::endl;
    }
    else if (type_crv == bspline) {
    	// construction de la bspline
    	CommandNewBSpline *commandCrv = new CommandNewBSpline(getLocalContext(),cmdsVtx, 1, 2, groupName);
    	cmdsCrv.push_back(commandCrv);
    	commandCompo->addCommand(commandCrv);
    	//std::cout<<" construction d'une b-spline"<<std::endl;
    }


    //creation de la commande de création de la surface
    CommandNewSurface* commandSrf = new CommandNewSurface(getLocalContext(),cmdsCrv, groupName);
    
    commandCompo->addCommand(commandSrf);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newVerticesCurvesAndPlanarSurface( [";
    for(unsigned int i=0;i<points.size();i++)
    {
        if (i!=0)
            cmd <<", ";
        cmd << points[i].getScriptCommand();
    }
    cmd <<"] , \""<<groupName<<"\")";
    commandCompo->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(commandCompo, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*commandCompo);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc*
GeomManager::newFacetedSurface(std::string nom)
{
	CommandNewFacetedSurfaces* command = new CommandNewFacetedSurfaces(getLocalContext(), nom);

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newFacetedSurface(\""<<nom<<"\")";
    command->setScriptCommand(cmd);
	getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*  cmdResult = new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc*
GeomManager::newCurveByCurveProjectionOnSurface(const std::string& curveName, const std::string& surfaceName, std::string groupName)
{
	return newCurveByCurveProjectionOnSurface(getCurve(curveName), getSurface(surfaceName), groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc*
GeomManager::newCurveByCurveProjectionOnSurface(Geom::Curve* curve, Geom::Surface* surface, std::string groupName)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newCurveByCurveProjectionOnSurface("<<curve->getName()<<", "<<surface->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewCurveByCurveProjectionOnSurface *command =
            new CommandNewCurveByCurveProjectionOnSurface(getLocalContext(), curve, surface, groupName);
    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newCurveByCurveProjectionOnSurface(\""
            <<curve->getName()<<"\", \""
            <<surface->getName()<<"\"";
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
destroy(std::vector<std::string>& e_names, bool propagagetDown)
{
    const int nb_entities= e_names.size();
    std::vector<Geom::GeomEntity* > es;
    es.reserve(nb_entities);
    for(int i=0;i<nb_entities;i++)
        es.push_back(getEntity(e_names[i]));

    return destroy(es, propagagetDown);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
destroy(std::vector<Geom::GeomEntity*>& entities, bool propagagetDown)
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::destroy(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    if(propagagetDown)
        message <<", true)";
    else
        message <<", false)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Suppression d'entités géométriques et suppression des liens topologiques");

        Geom::CommandEditGeom *commandGeom = new CommandRemove(getLocalContext(), entities, propagagetDown);
        commandCompo->addCommand(commandGeom);

        Topo::CommandSetGeomAssociation* commandTopo = new Topo::CommandSetGeomAssociation(getLocalContext(),
        		commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	command = new CommandRemove(getLocalContext(), entities, propagagetDown);
    }

    // trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().destroy([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    if(propagagetDown)
        cmd <<"\"], True)";
    else
        cmd <<"\"], False)";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
destroyWithTopo(std::vector<std::string>& e_names, bool propagagetDown)
{
    const int nb_entities= e_names.size();
    std::vector<Geom::GeomEntity* > es;
    es.reserve(nb_entities);
    for(int i=0;i<nb_entities;i++)
        es.push_back(getEntity(e_names[i]));

    return destroyWithTopo(es, propagagetDown);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
destroyWithTopo(std::vector<Geom::GeomEntity*>& entities, bool propagagetDown)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::destroyWithTopo(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    if(propagagetDown)
        message <<", true)";
    else
        message <<", false)";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Suppression d'entités géométriques et suppression des entités topologiques dépendantes");

        Geom::CommandEditGeom *commandGeom = new CommandRemove(getLocalContext(), entities, propagagetDown);
        commandCompo->addCommand(commandGeom);

        Topo::CommandDestroyTopo* commandTopo = new Topo::CommandDestroyTopo(getLocalContext(),
        		commandGeom, propagagetDown);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	command = new CommandRemove(getLocalContext(), entities, propagagetDown);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().destroyWithTopo([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    if(propagagetDown)
        cmd <<"\"], True)";
    else
        cmd <<"\"], False)";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scale(std::vector<std::string>& geo, const double factor)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return scale(vge,factor, Point(0,0,0));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scale(std::vector<std::string>& geo, const double factor, const Point& pcentre)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return scale(vge,factor, pcentre);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scale(std::vector<Geom::GeomEntity*>& entities, const double factor, const Point& pcentre)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour l'homothétie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::scale(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<factor<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Homothétie d'une géométrie avec sa topologie");

        commandGeom = new CommandScaling(getLocalContext(), entities, factor, pcentre);
        commandCompo->addCommand(commandGeom);

        std::vector<Topo::TopoEntity*> topoEntities = Topo::TopoHelper::getTopoEntities(entities);

        Topo::CommandScaleTopo* commandTopo =
        		new Topo::CommandScaleTopo(getLocalContext(), topoEntities, factor, pcentre);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	commandGeom = new CommandScaling(getLocalContext(), entities, factor, pcentre);
    	command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().scale([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<factor;
    if (!(pcentre == Point(0,0,0)))
    	cmd <<", "<<pcentre.getScriptCommand();
    cmd<<")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command, commandGeom);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scaleAll(const double factor, const Point& pcentre)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::scaleAll("<<factor<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* commandCompo =
    		new Internal::CommandComposite(getLocalContext(), "Homothétie de tout");

    Internal::CommandInternal *commandGeom = new CommandScaling(getLocalContext(), factor, pcentre);
    commandCompo->addCommand(commandGeom);

    Internal::CommandInternal* commandTopo = new Topo::CommandScaleTopo(getLocalContext(), factor, pcentre);
    commandCompo->addCommand(commandTopo);

    Internal::CommandInternal* commandMesh = new Mesh::CommandScaleMesh(getLocalContext(), factor, pcentre);
    commandCompo->addCommand(commandMesh);


    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().scaleAll("<<factor;
    if (!(pcentre == Point(0,0,0)))
    	cmd <<", "<<pcentre.getScriptCommand();
    cmd<<")";
    commandCompo->setScriptCommand(cmd);

    getCommandManager().addCommand(commandCompo, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*commandCompo,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scale(std::vector<std::string>& geo,
		const double factorX,
		const double factorY,
		const double factorZ)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return scale(vge, factorX, factorY, factorZ, Point(0,0,0));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scale(std::vector<std::string>& geo,
		const double factorX,
		const double factorY,
		const double factorZ,
		const Point& pcentre)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return scale(vge, factorX, factorY, factorZ, pcentre);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scale(std::vector<Geom::GeomEntity*>& entities,
		const double factorX,
		const double factorY,
		const double factorZ,
		const Point& pcentre)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour l'homothétie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::scale(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        message <<", "<<pcentre.getScriptCommand();
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Homothétie d'une géométrie avec sa topologie");

        commandGeom = new CommandScaling(getLocalContext(), entities, factorX, factorY, factorZ, pcentre);
        commandCompo->addCommand(commandGeom);

        std::vector<Topo::TopoEntity*> topoEntities = Topo::TopoHelper::getTopoEntities(entities);

        Topo::CommandScaleTopo* commandTopo =
             new Topo::CommandScaleTopo(getLocalContext(), topoEntities, factorX, factorY, factorZ, pcentre);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        commandGeom = new CommandScaling(getLocalContext(), entities, factorX, factorY, factorZ, pcentre);
        command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().scale([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        cmd <<", "<<pcentre.getScriptCommand();
    cmd <<")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command, commandGeom);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
scaleAll(const double factorX,
		const double factorY,
		const double factorZ,
		const Point& pcentre)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::scaleAll("<<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        message <<", "<<pcentre.getScriptCommand();
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* commandCompo =
    		new Internal::CommandComposite(getLocalContext(), "Homothétie de tout");

    Internal::CommandInternal *commandGeom = new CommandScaling(getLocalContext(), factorX, factorY, factorZ, pcentre);
    commandCompo->addCommand(commandGeom);

    Internal::CommandInternal* commandTopo = new Topo::CommandScaleTopo(getLocalContext(), factorX, factorY, factorZ, pcentre);
    commandCompo->addCommand(commandTopo);

    Internal::CommandInternal* commandMesh = new Mesh::CommandScaleMesh(getLocalContext(), factorX, factorY, factorZ, pcentre);
    commandCompo->addCommand(commandMesh);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().scaleAll("<<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        cmd <<", "<<pcentre.getScriptCommand();
    cmd <<")";
    commandCompo->setScriptCommand(cmd);

    getCommandManager().addCommand(commandCompo, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*commandCompo,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScale(std::vector<std::string>& geo, const double factor, bool withTopo, std::string groupName)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return copyAndScale(vge,factor, Point(0,0,0), withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScale(std::vector<std::string>& geo, const double factor, const Point& pcentre, bool withTopo, std::string groupName)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return copyAndScale(vge,factor, pcentre, withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScale(std::vector<Geom::GeomEntity*>& entities, const double factor, const Point& pcentre, bool withTopo, std::string groupName)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour l'homothétie avec copie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndScale(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<factor<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (withTopo && Internal::EntitiesHelper::hasTopoRef(entities)){

    	command =
    			new Internal::CommandComposite(getLocalContext(), "Homothétie d'une copie d'une géométrie avec sa topologie");

    	CommandGeomCopy *commandGeomCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandGeomCopy);

        Topo::CommandDuplicateTopo* commandTopoCopy =
                new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
        command->addCommand(commandTopoCopy);

        CommandScaling* commandGeomScale =
    			new CommandScaling(getLocalContext(), commandGeomCopy, factor, pcentre);
    	command->addCommand(commandGeomScale);

    	Topo::CommandScaleTopo* commandTopoScale =
    			new Topo::CommandScaleTopo(getLocalContext(), commandTopoCopy, factor, pcentre, false);
    	command->addCommand(commandTopoScale);

    }
    else {
    	command =
             new Internal::CommandComposite(getLocalContext(), "Homothétie d'une copie d'une géométrie");

    	CommandGeomCopy *commandCopy =
    	                new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandCopy);

    	CommandScaling* commandGeom = new CommandScaling(getLocalContext(), commandCopy, factor, pcentre);
    	command->addCommand(commandGeom);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndScale([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<factor;
    cmd <<", "<<pcentre.getScriptCommand();
    if(withTopo)
        cmd <<", True,\""<<groupName<<"\")";
    else
        cmd <<", False,\""<<groupName<<"\")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScaleAll(const double factor, const Point& pcentre, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndScaleAll("<<factor<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command =
			new Internal::CommandComposite(getLocalContext(), "Homothétie d'une copie de tout");

	CommandGeomCopy *commandGeomCopy =
			new CommandGeomCopy(getLocalContext(), groupName);
	command->addCommand(commandGeomCopy);

    Topo::CommandDuplicateTopo* commandTopoCopy =
            new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
    command->addCommand(commandTopoCopy);

    CommandScaling* commandGeomScale =
			new CommandScaling(getLocalContext(), commandGeomCopy, factor, pcentre);
	command->addCommand(commandGeomScale);

	Topo::CommandScaleTopo* commandTopoScale =
			new Topo::CommandScaleTopo(getLocalContext(), commandTopoCopy, factor, pcentre, true);
	command->addCommand(commandTopoScale);

	// on ne fait rien du maillage...
//    Internal::CommandInternal* commandMesh = new Mesh::CommandScaleMesh(getLocalContext(), factor, pcentre);
//    commandCompo->addCommand(commandMesh);


    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndScaleAll("<<factor;
    cmd <<", "<<pcentre.getScriptCommand();
    cmd<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScale(std::vector<std::string>& geo,
		const double factorX,
		const double factorY,
		const double factorZ,
		bool withTopo,
		std::string groupName)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return copyAndScale(vge, factorX, factorY, factorZ, Point(0,0,0), withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScale(std::vector<std::string>& geo,
		const double factorX,
		const double factorY,
		const double factorZ,
        const Point& pcentre,
		bool withTopo,
		std::string groupName)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<geo.size(); i++)
        vge.push_back(getEntity(geo[i]));

    return copyAndScale(vge, factorX, factorY, factorZ, pcentre, withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScale(std::vector<Geom::GeomEntity*>& entities,
		const double factorX,
		const double factorY,
		const double factorZ,
        const Point& pcentre,
		bool withTopo,
		std::string groupName)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour l'homothétie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndScale(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        message <<", "<<pcentre.getScriptCommand();
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (withTopo && Internal::EntitiesHelper::hasTopoRef(entities)){
    	command =
    			new Internal::CommandComposite(getLocalContext(), "Homothétie d'une copie d'une géométrie avec sa topologie");

    	CommandGeomCopy *commandGeomCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandGeomCopy);

        Topo::CommandDuplicateTopo* commandTopoCopy =
                new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
        command->addCommand(commandTopoCopy);

        CommandScaling* commandGeomScale =
                new CommandScaling(getLocalContext(), commandGeomCopy, factorX, factorY, factorZ, pcentre);
    	command->addCommand(commandGeomScale);

    	Topo::CommandScaleTopo* commandTopoScale =
                new Topo::CommandScaleTopo(getLocalContext(), commandTopoCopy, factorX, factorY, factorZ, pcentre, false);
    	command->addCommand(commandTopoScale);
    }
    else {
    	command =
    			new Internal::CommandComposite(getLocalContext(), "Homothétie d'une copie d'une géométrie");

    	CommandGeomCopy *commandCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandCopy);

        CommandScaling* commandGeom = new CommandScaling(getLocalContext(), commandCopy, factorX, factorY, factorZ, pcentre);
    	command->addCommand(commandGeom);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndScale([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        cmd <<", "<<pcentre.getScriptCommand();
    if(withTopo)
        cmd <<", True,\""<<groupName<<"\")";
    else
        cmd <<", False,\""<<groupName<<"\")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndScaleAll(const double factorX,
		const double factorY,
		const double factorZ,
        const Point& pcentre,
		std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndScaleAll("<<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        message <<", "<<pcentre.getScriptCommand();
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command =
			new Internal::CommandComposite(getLocalContext(), "Homothétie d'une copie de tout");

	CommandGeomCopy *commandGeomCopy =
			new CommandGeomCopy(getLocalContext(), groupName);
	command->addCommand(commandGeomCopy);

    Topo::CommandDuplicateTopo* commandTopoCopy =
            new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
    command->addCommand(commandTopoCopy);

    CommandScaling* commandGeomScale =
			new CommandScaling(getLocalContext(), commandGeomCopy, factorX, factorY, factorZ, pcentre);
	command->addCommand(commandGeomScale);

	Topo::CommandScaleTopo* commandTopoScale =
			new Topo::CommandScaleTopo(getLocalContext(), commandTopoCopy, factorX, factorY, factorZ, pcentre, true);
	command->addCommand(commandTopoScale);

	// on ne fait rien du maillage...
//    Internal::CommandInternal* commandMesh = new Mesh::CommandScaleMesh(getLocalContext(), factorX, factorY, factorZ);
//    commandCompo->addCommand(commandMesh);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndScaleAll("
        <<factorX<<", "<<factorY<<", "<<factorZ;
    if (!(pcentre == Point(0,0,0)))
        cmd <<", "<<pcentre.getScriptCommand();
    cmd <<", \""<<groupName<<"\")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
mirror(std::vector<std::string>& geo, Utils::Math::Plane* plane)
{
	std::vector<GeomEntity*> vge;
	for (uint i=0; i<geo.size(); i++)
		vge.push_back(getEntity(geo[i]));

	return mirror(vge, plane);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
mirror(std::vector<Geom::GeomEntity*>& entities, Utils::Math::Plane* plane)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour la symétrie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::mirror(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<*plane<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Symétrie d'une géométrie avec sa topologie");

        commandGeom = new CommandMirroring(getLocalContext(), entities, plane);
        commandCompo->addCommand(commandGeom);

        std::vector<Topo::TopoEntity*> topoEntities = Topo::TopoHelper::getTopoEntities(entities);

        Topo::CommandMirrorTopo* commandTopo =
        		new Topo::CommandMirrorTopo(getLocalContext(), topoEntities, plane);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	commandGeom = new CommandMirroring(getLocalContext(), entities, plane);
    	command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().mirror([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<plane->getScriptCommand();

    cmd<<")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
				new Internal::M3DCommandResult (*command,0);

	return cmdResult;
}


/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndMirror(std::vector<std::string>& geo, Utils::Math::Plane* plane, bool withTopo, std::string groupName)
{
	std::vector<GeomEntity*> vge;
	for (uint i=0; i<geo.size(); i++)
		vge.push_back(getEntity(geo[i]));

	return copyAndMirror(vge, plane, withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndMirror(std::vector<Geom::GeomEntity*>& entities, Utils::Math::Plane* plane, bool withTopo, std::string groupName)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour la symétrie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndMirror(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<*plane<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (withTopo && Internal::EntitiesHelper::hasTopoRef(entities)){

    	command =
                new Internal::CommandComposite(getLocalContext(), "Symétrie d'une copie d'une géométrie avec sa topologie");

    	CommandGeomCopy *commandGeomCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandGeomCopy);

        Topo::CommandDuplicateTopo* commandTopoCopy =
                new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
        command->addCommand(commandTopoCopy);

    	CommandMirroring* commandGeomMirror =
    			new CommandMirroring(getLocalContext(), commandGeomCopy, plane);
    	command->addCommand(commandGeomMirror);

    	Topo::CommandMirrorTopo* commandTopoMirror =
    			new Topo::CommandMirrorTopo(getLocalContext(), commandTopoCopy, plane, false);
    	command->addCommand(commandTopoMirror);

    }
    else {
    	command =
             new Internal::CommandComposite(getLocalContext(), "Symétrie d'une copie d'une géométrie");

    	CommandGeomCopy *commandCopy =
    	                new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandCopy);

    	CommandMirroring* commandGeom = new CommandMirroring(getLocalContext(), commandCopy, plane);
    	command->addCommand(commandGeom);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndMirror([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<plane->getScriptCommand();
    if(withTopo)
        cmd <<", True,\""<<groupName<<"\")";
    else
        cmd <<", False,\""<<groupName<<"\")";

    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
				new Internal::M3DCommandResult (*command);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newBox(const Utils::Math::Point& pmin, const Utils::Math::Point& pmax, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newBox ("<<pmin<<", "<<pmax;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandNewBox *command = new CommandNewBox(getLocalContext(),pmin,pmax, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newBox ("
        <<pmin.getScriptCommand()<<", "<<pmax.getScriptCommand();
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);
    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newCylinder(const Utils::Math::Point& pcentre, const double& dr,
            const Utils::Math::Vector& dv, const double& da, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newCylinder("<<pcentre<<", "<<dr<<", "<<dv<<", "<<da;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewCylinder *command = new CommandNewCylinder(getLocalContext(), pcentre, dr, dv,da, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newCylinder ("
                             <<pcentre.getScriptCommand()<<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
                             <<dv.getScriptCommand()<<", "
                             <<da;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newCylinder(const Utils::Math::Point& pcentre, const double& dr,
            const Utils::Math::Vector& dv, const Utils::Portion::Type& dt, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newCylinder("<<pcentre<<", "<<dr<<", "<<dv<<", "<<Utils::Portion::getName(dt);
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewCylinder *command = new CommandNewCylinder(getLocalContext(), pcentre, dr, dv,dt, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newCylinder ("
                             <<pcentre.getScriptCommand()<<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
                             <<dv.getScriptCommand()<<", "
                             <<Utils::Portion::getName(dt);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newHollowCylinder(
        const Utils::Math::Point& pcentre,
        const double& dr_int,
        const double& dr_ext,
        const Utils::Math::Vector& dv,
        const double& da,
        std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newHollowCylinder("<<pcentre<<", "
            <<dr_int<<", "
            <<dr_ext<<", "
            <<dv<<", "<<da;

    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewHollowCylinder *command =
            new CommandNewHollowCylinder(getLocalContext(), pcentre, dr_int, dr_ext,
                    dv,da, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newHollowCylinder ("
                             <<pcentre.getScriptCommand()<<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
                             <<dv.getScriptCommand()<<", "
                             <<da;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newHollowCylinder(
        const Utils::Math::Point& pcentre,
        const double& dr_int,
        const double& dr_ext,
        const Utils::Math::Vector& dv,
        const Utils::Portion::Type& dt,
        std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newHollowCylinder("<<pcentre<<", "
            <<dr_int<<", "
            <<dr_ext<<", "
            <<dv<<", "<<Utils::Portion::getName(dt);
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewHollowCylinder *command =
            new CommandNewHollowCylinder(getLocalContext(), pcentre, dr_int, dr_ext,
                    dv,dt, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newHollowCylinder ("
                             <<pcentre.getScriptCommand()<<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
                             <<dv.getScriptCommand()<<", "
                             <<Utils::Portion::getName(dt);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}

/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newSphere(const Utils::Math::Point& pcentre, const double& dr,
            const double& da, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newSphere("<<pcentre<<", "<<dr<<", "<<da;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewSphere *command = new CommandNewSphere(getLocalContext(), pcentre, dr,da, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newSphere ("
                             <<pcentre.getScriptCommand()<<", "<< Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
                             <<Utils::Math::MgxNumeric::userRepresentation (da);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newSphere(const Utils::Math::Point& pcentre, const double& dr,
          const Utils::Portion::Type& dt, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newSphere("<<pcentre<<", "<<dr<<", "<<Utils::Portion::getName(dt);
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewSphere *command = new CommandNewSphere(getLocalContext(), pcentre, dr,dt, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newSphere ("
                             <<pcentre.getScriptCommand()<<", "<< Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
                             <<Utils::Portion::getName(dt);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc *GeomManager::
newSpherePart(const double& dr,
		const double& angleY,
		const double& angleZ,
		std::string groupName)
{
    CommandNewSpherePart *command = new CommandNewSpherePart(getLocalContext(), dr, angleY, angleZ, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newSpherePart ("
                             << Utils::Math::MgxNumeric::userRepresentation (dr) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (angleY) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (angleZ);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
	 	 newHollowSpherePart(const double& dr_int,
	 			 const double& dr_ext,
				 const double& angleY,
				 const double& angleZ,
				 std::string groupName)
{
	CommandNewHollowSpherePart *command = new CommandNewHollowSpherePart(getLocalContext(), dr_int, dr_ext, angleY, angleZ, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newHollowSpherePart ("
                             << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (angleY) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (angleZ);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newHollowSphere(
        const Utils::Math::Point& pcentre,
        const double& dr_int,
        const double& dr_ext,
        const double& da,
        std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newHollowSphere("<<pcentre<<", "<<dr_int<<", "<<dr_ext<<", "<<da;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewHollowSphere *command =
            new CommandNewHollowSphere(getLocalContext(), pcentre, dr_int, dr_ext,da, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newHollowSphere ("
                             <<pcentre.getScriptCommand()<<", "
                             <<Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
                             <<Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
                             <<Utils::Math::MgxNumeric::userRepresentation (da);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newHollowSphere(
        const Utils::Math::Point& pcentre,
        const double& dr_int,
        const double& dr_ext,
        const Utils::Portion::Type& dt,
        std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newHollowSphere("<<pcentre<<", "
            <<dr_int<<", "
            <<dr_ext<<", "
            <<Utils::Portion::getName(dt);
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewHollowSphere *command =
            new CommandNewHollowSphere(getLocalContext(), pcentre, dr_int,dr_ext, dt, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newHollowSphere ("
                             <<pcentre.getScriptCommand()<<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_int) <<", "
                             << Utils::Math::MgxNumeric::userRepresentation (dr_ext) <<", "
                             <<Utils::Portion::getName(dt);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManager::newCone(const double& dr1, const double& dr2,
   		const Vector& dv, const double& da, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newCone("<<dr1<<", "<<dr2<<", "<<dv<<", "<<da;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewCone *command = new CommandNewCone(getLocalContext(), dr1, dr2, dv, da, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newCone ("
    		<< Utils::Math::MgxNumeric::userRepresentation (dr1) <<", "
    		<< Utils::Math::MgxNumeric::userRepresentation (dr2) <<", "
    		<<dv.getScriptCommand()<<", "
    		<<da;
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManager::newCone(const double& dr1, const double& dr2,
   		const Vector& dv, const  Utils::Portion::Type& dt, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newCone("<<dr1<<", "<<dr2<<", "<<dv<<", "<<Utils::Portion::getName(dt);
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewCone *command = new CommandNewCone(getLocalContext(), dr1, dr2, dv, dt, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newCone ("
    		<< Utils::Math::MgxNumeric::userRepresentation (dr1) <<", "
            << Utils::Math::MgxNumeric::userRepresentation (dr2) <<", "
            <<dv.getScriptCommand()<<", "
            <<Utils::Portion::getName(dt);
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcCircle(std::string pc,std::string pd, std::string pe, const bool direct, const Vector& normal, std::string groupName)
{
    Vertex* vc = getVertex(pc);
    Vertex* vd = getVertex(pd);
    Vertex* ve = getVertex(pe);
    return newArcCircle(vc,vd,ve,direct, normal, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcCircle(std::string pc,std::string pd, std::string pe, const bool direct, std::string groupName)
{
    Vertex* vc = getVertex(pc);
    Vertex* vd = getVertex(pd);
    Vertex* ve = getVertex(pe);
    return newArcCircle(vc,vd,ve,direct, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcCircle(const double& angleDep,
             const double& angleFin,
             const double& rayon,
             std::string sysCoordName,
             std::string groupName)
{
    CoordinateSystem::SysCoord* rep = 0;
    if (!sysCoordName.empty())
        rep = getLocalContext().getSysCoordManager().getSysCoord(sysCoordName, true);

    return newArcCircle(angleDep, angleFin, rayon, rep, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcCircle(Vertex* pc, Vertex* pd, Vertex* pe, const bool direct, std::string groupName )
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newArcCircle("<<*pc<<", "<<*pd<<", "<<*pe
            <<", "<<(short)direct<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));
    Vector normal(0,0,1);

    CommandNewArcCircle *command =
            new CommandNewArcCircle(getLocalContext(), pc,pd,pe,direct, normal, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newArcCircle(\""
            <<pc->getName()<<"\", \""
            <<pd->getName()<<"\", \""
            <<pe->getName()<<"\", "
            <<(direct?"True":"False");
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcCircle(Vertex* pc, Vertex* pd, Vertex* pe, const bool direct, const Vector& normal, std::string groupName )
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newArcCircle("<<*pc<<", "<<*pd<<", "<<*pe
            <<", "<<(short)direct<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewArcCircle *command =
            new CommandNewArcCircle(getLocalContext(), pc,pd,pe,direct, normal, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newArcCircle(\""
            <<pc->getName()<<"\", \""
            <<pd->getName()<<"\", \""
            <<pe->getName()<<"\", "
            <<(direct?"True":"False")<<", "
			<<normal.getScriptCommand();
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcCircle(const double& angleDep,
             const double& angleFin,
             const double& rayon,
             CoordinateSystem::SysCoord* sysCoord,
             std::string groupName)
{
    CommandNewArcCircleWithAngles *command =
            new CommandNewArcCircleWithAngles(getLocalContext(), angleDep, angleFin, rayon, sysCoord, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newArcCircle(\""
            <<angleDep<<"\", \""
            <<angleFin<<"\", \""
            <<rayon<<"\", ";
    if (sysCoord)
        cmd << "\"" <<sysCoord->getName()<< "\"";
    else
        cmd << "\"\"";
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newCircle(std::string p1,std::string p2, std::string p3, std::string groupName)
{
    Vertex* v1 = getVertex(p1);
    Vertex* v2 = getVertex(p2);
    Vertex* v3 = getVertex(p3);
    return newCircle(v1,v2,v3, groupName);
}

/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newCircle(Vertex* p1, Vertex* p2, Vertex* p3, std::string groupName )
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newCircle("<<*p1<<", "<<*p2<<", "<<*p3<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewCircle *command =
            new CommandNewCircle(getLocalContext(), p1,p2,p3, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newCircle(\""
            <<p1->getName()<<"\", \""
            <<p2->getName()<<"\", \""
            <<p3->getName()<<"\"";
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newEllipse(std::string p1,std::string p2, std::string center, std::string groupName)
{
    Vertex* v1 = getVertex(p1);
    Vertex* v2 = getVertex(p2);
    Vertex* vcenter = getVertex(center);
    return newEllipse(v1,v2,vcenter, groupName);
}

/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newEllipse(Vertex* p1, Vertex* p2, Vertex* center, std::string groupName )
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newEllipse("<<*p1<<", "<<*p2<<", "<<*center<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewEllipse *command =
            new CommandNewEllipse(getLocalContext(),p1,p2,center,groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newEllipse(\""
            <<p1->getName()<<"\", \""
            <<p2->getName()<<"\", \""
            <<center->getName()<<"\"";
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcEllipse(std::string pc,std::string pd, std::string pe, const bool direct, std::string groupName)
{
    Vertex* vc = getVertex(pc);
    Vertex* vd = getVertex(pd);
    Vertex* ve = getVertex(pe);
    return newArcEllipse(vc,vd,ve,direct, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newArcEllipse(  Vertex* pc, Vertex* pd, Vertex* pe, const bool direct, std::string groupName )
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newArcEllipse("<<*pc<<", "<<*pd<<", "<<*pe
            <<", "<<(short)direct<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewArcEllipse *command =
            new CommandNewArcEllipse(getLocalContext(),pc,pd,pe,direct, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newArcEllipse(\""
            <<pc->getName()<<"\", \""
            <<pd->getName()<<"\", \""
            <<pe->getName()<<"\", "
            <<(direct?"True":"False");
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::newBSpline(std::string n1,
		std::vector<Point>& vp,
		std::string n2,
		int deg_min,
		int deg_max,
		std::string groupName){

    Vertex* v1 = getVertex(n1);
    Vertex* v2 = getVertex(n2);

    return newBSpline(v1, vp, v2, deg_min, deg_max, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newBSpline(Vertex* vtx1,
		std::vector<Point>& vp,
		Vertex* vtx2,
		int deg_min,
		int deg_max,
		std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newBSpline("
    		<<vtx1->getName()<<","
			<< vp.size()<<" Points, "
			<<vtx2->getName()<<")";

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandNewBSpline *command =
            new CommandNewBSpline(getLocalContext(), vtx1, vp, vtx2, deg_min, deg_max, groupName);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newBSpline(\""
    		<<vtx1->getName()<<"\", [";
    for(unsigned int i=0;i<vp.size();i++)
    {
        if (i!=0)
            cmd <<", ";
        cmd << vp[i].getScriptCommand();
    }
    cmd <<"] , \""<<vtx2->getName()<<"\","<<(short)deg_min<<", "<<(short)deg_max
    		<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newPrism(std::string name, const Utils::Math::Vector& dv, std::string groupName)
{
    return newPrism(getEntity(name),dv, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newPrismWithTopo(std::string name, const Utils::Math::Vector& dv, std::string groupName)
{
    return newPrismWithTopo(getEntity(name),dv, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newPrism(GeomEntity* base, const Utils::Math::Vector& dv, std::string groupName)
{
    TkUtil::UTF8String   warning (TkUtil::Charset::UTF_8);
    warning <<"La fonction newPrism est obsolete, il est préférable d'utiliser makeExtrude";
    log (TkUtil::TraceLog (warning, TkUtil::Log::WARNING));

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::newPrism("<<base->getName()<<", "<<dv;
    if (!groupName.empty())
        message<<", "<<groupName;
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = new CommandNewPrism(getLocalContext(), base, dv,groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newPrism (\""<<base->getName()<<"\", "
                             <<dv.getScriptCommand();
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
newPrismWithTopo(GeomEntity* base, const Utils::Math::Vector& dv, std::string groupName)
{
    TkUtil::UTF8String   warning (TkUtil::Charset::UTF_8);
    warning <<"La fonction newPrismWithTopo est obsolete, il est faut utiliser makeBlocksByExtrude";
    log (TkUtil::TraceLog (warning, TkUtil::Log::ERROR));

    Internal::CommandComposite* command =
                 new Internal::CommandComposite(getLocalContext(), "[OBSOLETE] Création d'un prisme avec topologie [OBSOLETE]");

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().newPrismWithTopo (\""<<base->getName()<<"\", "
                             <<dv.getScriptCommand();
    if (!groupName.empty())
        cmd<<", \""<<groupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
translate(std::vector<std::string>& entities, const Utils::Math::Vector& dp){

    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return translate(vge, dp);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
translate(std::vector<GeomEntity*>& entities, const Vector& dp)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour la translation", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::translate(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<dp<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Translation d'une géométrie avec sa topologie");

        commandGeom = new CommandTranslation(getLocalContext(), entities, dp);
        commandCompo->addCommand(commandGeom);

        // recherche des entités topologiques parmis les entités géométriques sélectionnées
        std::vector<Topo::TopoEntity*> topoEntities = Topo::TopoHelper::getTopoEntities(entities);

        Topo::CommandTranslateTopo* commandTopo = new Topo::CommandTranslateTopo(getLocalContext(), topoEntities, dp);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	commandGeom = new CommandTranslation(getLocalContext(), entities, dp);
    	command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().translate([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<dp.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command,  commandGeom);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
translateAll(const Vector& dp)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::translateAll("<<dp<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* commandCompo =
    		new Internal::CommandComposite(getLocalContext(), "Translation de tout");

    Internal::CommandInternal *commandGeom = new CommandTranslation(getLocalContext(), dp);
    commandCompo->addCommand(commandGeom);

    Internal::CommandInternal* commandTopo = new Topo::CommandTranslateTopo(getLocalContext(), dp);
    commandCompo->addCommand(commandTopo);

    Internal::CommandInternal* commandMesh = new Mesh::CommandTranslateMesh(getLocalContext(), dp);
    commandCompo->addCommand(commandMesh);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().translateAll("<<dp.getScriptCommand()<<")";
    commandCompo->setScriptCommand(cmd);

    getCommandManager().addCommand(commandCompo, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*commandCompo,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndTranslate(std::vector<std::string>& entities, const Utils::Math::Vector& dp, bool withTopo, std::string groupName){

    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return copyAndTranslate(vge, dp, withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndTranslate(std::vector<GeomEntity*>& entities, const Vector& dp, bool withTopo, std::string groupName)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour la translation avec copie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndTranslate(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<dp<<", \""<<groupName<<"\")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (withTopo && Internal::EntitiesHelper::hasTopoRef(entities)){

    	command =
    			new Internal::CommandComposite(getLocalContext(), "Translation d'une copie d'une géométrie avec sa topologie");

    	CommandGeomCopy *commandGeomCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandGeomCopy);

        Topo::CommandDuplicateTopo* commandTopoCopy =
                new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
        command->addCommand(commandTopoCopy);

        CommandTranslation* commandGeomTranslate =
    			new CommandTranslation(getLocalContext(), commandGeomCopy, dp);
    	command->addCommand(commandGeomTranslate);

    	Topo::CommandTranslateTopo* commandTopoTranslate =
    			new Topo::CommandTranslateTopo(getLocalContext(), commandTopoCopy, dp, false);
    	command->addCommand(commandTopoTranslate);

    }
    else {
    	command =
             new Internal::CommandComposite(getLocalContext(), "Translation d'une copie d'une géométrie");

    	CommandGeomCopy *commandCopy =
    	                new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandCopy);

    	CommandTranslation* commandGeom = new CommandTranslation(getLocalContext(), commandCopy, dp);
    	command->addCommand(commandGeom);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndTranslate([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<dp.getScriptCommand();
    if(withTopo)
        cmd <<", True,\""<<groupName<<"\")";
    else
        cmd <<", False,\""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndTranslateAll(const Vector& dp, std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndTranslateAll("<<dp<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite*   command =
			new Internal::CommandComposite(getLocalContext(), "Translation d'une copie de tout");

	CommandGeomCopy *commandGeomCopy =
			new CommandGeomCopy(getLocalContext(), groupName);
	command->addCommand(commandGeomCopy);

    Topo::CommandDuplicateTopo* commandTopoCopy =
            new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
    command->addCommand(commandTopoCopy);

    CommandTranslation* commandGeomTranslate =
			new CommandTranslation(getLocalContext(), commandGeomCopy, dp);
	command->addCommand(commandGeomTranslate);

	Topo::CommandTranslateTopo* commandTopoTranslate =
			new Topo::CommandTranslateTopo(getLocalContext(), commandTopoCopy, dp, false);
	command->addCommand(commandTopoTranslate);

	// on ne fait rien au maillage...
//    Internal::CommandInternal* commandMesh = new Mesh::CommandTranslateMesh(getLocalContext(), dp);
//    commandCompo->addCommand(commandMesh);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndTranslateAll("
    		<<dp.getScriptCommand()<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
joinCurves(std::vector<std::string>& entities){

    std::vector<GeomEntity*> vge;
    for (uint i=0; i<entities.size(); i++)
        vge.push_back(getEntity(entities[i]));

    return joinCurves(vge);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
joinCurves(std::vector<GeomEntity*>& entities)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::joinCurves(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Fusion de courbes avec maj de la topologie");

        Geom::CommandCreateGeom *commandGeom = new CommandJoinCurves(getLocalContext(), entities);
        commandCompo->addCommand(commandGeom);

        // recherche des entités topologiques parmis les entités géométriques sélectionnées
        // recherche des entités topologiques parmis les entités géométriques sélectionnées
        std::vector<Topo::TopoEntity*> allTopoEntities = Topo::TopoHelper::getTopoEntities(entities);
        std::vector<Topo::TopoEntity*> selectedTopoEntities;
        for (std::vector<Topo::TopoEntity*>::iterator iter=allTopoEntities.begin(); iter!=allTopoEntities.end(); ++iter)
            if ((*iter)->getDim()<=1)
            	selectedTopoEntities.push_back(*iter);

        // projection sur la nouvelle courbe pour les arêtes et sommets détruits
        if (selectedTopoEntities.size()){
        	Topo::CommandSetGeomAssociation* commandTopo = new Topo::CommandSetGeomAssociation(getLocalContext(), selectedTopoEntities, commandGeom);
        	commandCompo->addCommand(commandTopo);
        }
        command = commandCompo;
    }
    else {
        command = new CommandJoinCurves(getLocalContext(), entities);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().joinCurves([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);


    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
joinSurfaces(std::vector<std::string>& entities){

    std::vector<GeomEntity*> vge;
    for (uint i=0; i<entities.size(); i++)
        vge.push_back(getEntity(entities[i]));

    return joinSurfaces(vge);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
joinSurfaces(std::vector<GeomEntity*>& entities)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::joinSurfaces(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<",";
    message <<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities))
    {
        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Fusion de surfaces avec maj de la topologie");

        Geom::CommandCreateGeom *commandGeom = new CommandJoinSurfaces(getLocalContext(), entities);
        commandCompo->addCommand(commandGeom);

        // recherche des entités topologiques parmis les entités géométriques sélectionnées
        std::vector<Topo::TopoEntity*> allTopoEntities = Topo::TopoHelper::getTopoEntities(entities);
        std::vector<Topo::TopoEntity*> selectedTopoEntities;
        for (std::vector<Topo::TopoEntity*>::iterator iter=allTopoEntities.begin(); iter!=allTopoEntities.end(); ++iter)
            if ((*iter)->getDim()<=2)
            	selectedTopoEntities.push_back(*iter);

//        std::cout<<"allTopoEntities.size() = "<<allTopoEntities.size()<<std::endl;
//        std::cout<<"selectedTopoEntities.size() = "<<selectedTopoEntities.size()<<std::endl;

        // projection sur la nouvelle surface pour les cofaces, arêtes et sommets détruits
        if (selectedTopoEntities.size()){
        	Topo::CommandSetGeomAssociation* commandTopo = new Topo::CommandSetGeomAssociation(getLocalContext(), selectedTopoEntities, commandGeom);
        	commandCompo->addCommand(commandTopo);
        }
        command = commandCompo;
    }
    else
    {
        command = new CommandJoinSurfaces(getLocalContext(), entities);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().joinSurfaces([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"])";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);


    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
rotate( std::vector<std::string>& entities, const Utils::Math::Rotation& rot)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return rotate(vge, rot);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
rotate( std::vector<GeomEntity*>& entities, const Utils::Math::Rotation& rot)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour la rotation", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::rotate(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<", "<<rot<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Rotation d'une géométrie avec sa topologie");

//        Geom::CommandEditGeom *commandGeom = new CommandRotation(getLocalContext(), entities,rot);
        Internal::CommandInternal *commandGeom = new CommandRotation(getLocalContext(), entities,rot);
        commandCompo->addCommand(commandGeom);

//        geom_modif = commandGeom->getGeomModificationBaseClass();

        // recherche des entités topologiques parmis les entités géométriques sélectionnées
        std::vector<Topo::TopoEntity*> topoEntities = Topo::TopoHelper::getTopoEntities(entities);

        Topo::CommandRotateTopo* commandTopo =
                       new Topo::CommandRotateTopo(getLocalContext(), topoEntities, rot);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new CommandRotation(getLocalContext(), entities, rot);
        //geom_modif = commandGeom->getGeomModificationBaseClass();
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().rotate([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<rot.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
rotateAll(const Utils::Math::Rotation& rot)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::rotateAll("<<rot<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* commandCompo =
    		new Internal::CommandComposite(getLocalContext(), "Rotation de tout");

    Internal::CommandInternal *commandGeom = new CommandRotation(getLocalContext(), rot);
    commandCompo->addCommand(commandGeom);

    Internal::CommandInternal* commandTopo = new Topo::CommandRotateTopo(getLocalContext(), rot);
    commandCompo->addCommand(commandTopo);

    Internal::CommandInternal* commandMesh = new Mesh::CommandRotateMesh(getLocalContext(), rot);
    commandCompo->addCommand(commandMesh);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().rotateAll("<<rot.getScriptCommand()<<")";
    commandCompo->setScriptCommand(cmd);

    getCommandManager().addCommand(commandCompo, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*commandCompo,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndRotate( std::vector<std::string>& entities,
		const Utils::Math::Rotation& rot,
		bool withTopo,
		std::string groupName)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return copyAndRotate(vge, rot, withTopo, groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndRotate( std::vector<GeomEntity*>& entities,
		const Utils::Math::Rotation& rot,
		bool withTopo,
		std::string groupName)
{
    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour la rotation avec copie", TkUtil::Charset::UTF_8));
    }

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndRotate(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<", "<<rot<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (withTopo && Internal::EntitiesHelper::hasTopoRef(entities)){

    	command =
    			new Internal::CommandComposite(getLocalContext(), "Rotation d'une copie d'une géométrie avec sa topologie");

    	CommandGeomCopy *commandGeomCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandGeomCopy);

        Topo::CommandDuplicateTopo* commandTopoCopy =
                new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
        command->addCommand(commandTopoCopy);

        CommandRotation* commandGeomRotate =
    			new CommandRotation(getLocalContext(), commandGeomCopy, rot);
    	command->addCommand(commandGeomRotate);

    	Topo::CommandRotateTopo* commandTopoRotate =
    			new Topo::CommandRotateTopo(getLocalContext(), commandTopoCopy, rot, false);
    	command->addCommand(commandTopoRotate);

    }
    else {
    	command =
             new Internal::CommandComposite(getLocalContext(), "Rotation d'une copie d'une géométrie");

    	CommandGeomCopy *commandCopy =
    			new CommandGeomCopy(getLocalContext(), entities, groupName);
    	command->addCommand(commandCopy);

    	CommandRotation* commandGeom =
    			new CommandRotation(getLocalContext(), commandCopy, rot);
    	command->addCommand(commandGeom);

    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndRotate([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<rot.getScriptCommand();
    if(withTopo)
        cmd <<", True,\""<<groupName<<"\")";
    else
        cmd <<", False,\""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
copyAndRotateAll(const Utils::Math::Rotation& rot,
		std::string groupName)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::copyAndRotateAll("<<rot<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandComposite* command =
			new Internal::CommandComposite(getLocalContext(), "Rotation d'une copie de tout");

	CommandGeomCopy *commandGeomCopy =
			new CommandGeomCopy(getLocalContext(), groupName);
	command->addCommand(commandGeomCopy);

    Topo::CommandDuplicateTopo* commandTopoCopy =
            new Topo::CommandDuplicateTopo(getLocalContext(), commandGeomCopy);
    command->addCommand(commandTopoCopy);

    CommandRotation* commandGeomRotate =
			new CommandRotation(getLocalContext(), commandGeomCopy, rot);
	command->addCommand(commandGeomRotate);

	Topo::CommandRotateTopo* commandTopoRotate =
			new Topo::CommandRotateTopo(getLocalContext(), commandTopoCopy, rot, true);
	command->addCommand(commandTopoRotate);

	// on ne fait rien pour le maillage ....
//    Internal::CommandInternal* commandMesh = new Mesh::CommandRotateMesh(getLocalContext(), rot);
//    commandCompo->addCommand(commandMesh);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().copyAndRotateAll("
    		<<rot.getScriptCommand()<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command,0);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
makeRevol( std::vector<std::string>& entities,
        const Utils::Math::Rotation& rot,
        const bool keep)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return makeRevol(vge, rot,keep);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
makeRevol( std::vector<GeomEntity*>& entities,
        const Utils::Math::Rotation& rot,
        const bool keep)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::makeRevol(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<", "<<rot<<(keep?"True":"False")<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandExtrudeRevolution *command =
            new CommandExtrudeRevolution(getLocalContext(), entities,rot,keep);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().makeRevol([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<rot.getScriptCommand()<<", "<<(keep?"True":"False")<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
makeExtrude(std::vector<std::string>& entities,
					const Vector& dp, const bool keep)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return makeExtrude(vge, dp, keep);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
makeExtrude( std::vector<GeomEntity*>& entities,
		const Vector& dp, const bool keep)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::makeExtrude(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<", "<<dp<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    CommandExtrudeDirection *command =
            new CommandExtrudeDirection(getLocalContext(), entities, dp, keep);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().makeExtrude([\"";
    for(unsigned int i=0;i<entities.size();i++){
           if(i!=0)
               cmd << "\", \"";
           cmd<< entities[i]->getName();
       }
    cmd <<"\"], "<<dp.getScriptCommand()<<", "<<(keep?"True":"False")<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
makeBlocksByExtrude(std::vector<std::string>& entities,
					const Vector& dp, const bool keep)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

    return makeBlocksByExtrude(vge, dp, keep);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
makeBlocksByExtrude(std::vector<GeomEntity*>& entities, const Utils::Math::Vector& dv, const bool keep)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message <<"GeomManager::makeBlocksByExtrude(";
    for (uint i=0; i<entities.size(); i++)
        message <<entities[i]->getName()<<", "<<dv<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities))
    {
        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Création d'une topologie par extrusion");

        Geom::CommandExtrudeDirection *commandGeom = new CommandExtrudeDirection(getLocalContext(), entities, dv, keep);
        commandCompo->addCommand(commandGeom);

        // recherche des entités topologiques parmis les entités géométriques sélectionnées
        std::vector<Topo::TopoEntity*> allTopoEntities = Topo::TopoHelper::getTopoEntities(entities);
        std::vector<Topo::TopoEntity*> selectedCoFaces;
        for (std::vector<Topo::TopoEntity*>::iterator iter=allTopoEntities.begin(); iter!=allTopoEntities.end(); ++iter)
            if ((*iter)->getDim()==2){
            	selectedCoFaces.push_back(*iter);
            }
//        std::cout<<"allTopoEntities.size() = "<<allTopoEntities.size()<<std::endl;
//        std::cout<<"selectedCoFaces.size() = "<<selectedCoFaces.size()<<std::endl;

        // extrusion de la topologie suivant une direction
        if (selectedCoFaces.size()){
        	Topo::CommandExtrudeTopo* commandTopo = new Topo::CommandExtrudeTopo(getLocalContext(), commandGeom, selectedCoFaces, dv);
        	commandCompo->addCommand(commandTopo);
        }
        command = commandCompo;
    }
    else
    {
        command = new CommandExtrudeDirection(getLocalContext(), entities, dv, keep);
    }


    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().makeBlocksByExtrude ([\"";
    for(unsigned int i=0;i<entities.size();i++){
    	if(i!=0)
    		cmd << "\", \"";
    	cmd<< entities[i]->getName();
    }
    cmd <<"\"], "<<dv.getScriptCommand()<<", "<<(keep?"True":"False")<<")";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
fuse(std::vector<std::string>& entities)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

   return fuse(vge);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
fuse(std::vector<GeomEntity*>& entities)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::fuse (";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            message<<", ";
        message << entities[i]->getName();
    }
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Fusion booléenne entre géométries avec topologies");

        commandGeom = new CommandFuse(getLocalContext(),entities);
        commandCompo->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	commandGeom = new CommandFuse(getLocalContext(),entities);
    	command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().fuse([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< entities[i]->getName()<<"\"";
    }
    cmd <<"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command, commandGeom);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
common(std::vector<std::string>& entities)
{
    std::vector<GeomEntity*> vge;
    convert(entities, vge);

   return common(vge);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
common(std::vector<Geom::GeomEntity*>& entities)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::common (";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            message<<", ";
        message << entities[i]->getName();
    }
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Intersection booléenne entre géométries avec topologies");

        commandGeom = new CommandCommon(getLocalContext(),entities);
        commandCompo->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	commandGeom = new CommandCommon(getLocalContext(), entities);
    	command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().common([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< entities[i]->getName()<<"\"";
    }
    cmd <<"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command, commandGeom);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
common2D(std::string entity1, std::string entity2, std::string groupName)
{
   return common2D(getEntity(entity1), getEntity(entity2), groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
common2D(Geom::GeomEntity* entity1, Geom::GeomEntity* entity2, std::string groupName)
{

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    std::vector<Geom::GeomEntity*> entities;
    entities.push_back(entity1);
    entities.push_back(entity2);
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Intersection booléenne entre 2 géométries 1D ou 2D avec topologies");

        commandGeom = new CommandCommon2D(getLocalContext(), entity1, entity2, groupName);
        commandCompo->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
    	commandGeom = new CommandCommon2D(getLocalContext(), entity1, entity2, groupName);
    	command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().common2D(\""
        << entity1->getName()<<"\", \""
        << entity2->getName()<<"\", \""
		<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command, commandGeom);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
common2DOnCopy(std::string entity1, std::string entity2, std::string groupName)
{
   return common2DOnCopy(getEntity(entity1), getEntity(entity2), groupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
common2DOnCopy(Geom::GeomEntity* entity1, Geom::GeomEntity* entity2, std::string groupName)
{

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;
    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    std::vector<Geom::GeomEntity*> entities;
    entities.push_back(entity1);
    entities.push_back(entity2);

    Internal::CommandComposite* commandCompo =
    		new Internal::CommandComposite(getLocalContext(), "Intersection booléenne entre 2 géométries 1D ou 2D, sans destruction");
    CommandGeomCopy *commandGeomCopy =
    		new CommandGeomCopy(getLocalContext(), entities, groupName);
    commandCompo->addCommand(commandGeomCopy);

    commandGeom = new CommandCommon2D(getLocalContext(), commandGeomCopy, groupName);
    commandCompo->addCommand(commandGeom);

    command = commandCompo;

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().common2DOnCopy(\""
        << entity1->getName()<<"\", \""
        << entity2->getName()<<"\", \""
		<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
			new Internal::M3DCommandResult (*command, commandGeom);

	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importBREP(std::string n, const bool testVolumicProperties, const bool splitCompoundCurves)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importBREP ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandImportBREP *command = new CommandImportBREP(getLocalContext(), n, testVolumicProperties, splitCompoundCurves);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importBREP(\""<<n<<"\"";
    if (!testVolumicProperties || splitCompoundCurves)
        cmd << (testVolumicProperties?", True":", False") << (splitCompoundCurves?", True":", False");
    cmd << ")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importSTEP(std::string n, const bool testVolumicProperties, const bool splitCompoundCurves)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importSTEP ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandImportSTEP *command = new CommandImportSTEP(getLocalContext(), n, testVolumicProperties, splitCompoundCurves);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importSTEP(\""<<n<<"\"";
    if (!testVolumicProperties || splitCompoundCurves)
        cmd << (testVolumicProperties?", True":", False") << (splitCompoundCurves?", True":", False");
    cmd << ")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importSTL(std::string n)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importSTL ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandImportSTL *command = new CommandImportSTL(getLocalContext(),n);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importSTL(\""<<n<< "\"";
    cmd << ")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importIGES(std::string n, const bool splitCompoundCurves)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importIGES ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandImportIGES *command = new CommandImportIGES(getLocalContext(),n, splitCompoundCurves);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importIGES(\""<<n<<"\"";
    if (splitCompoundCurves)
    	cmd << (splitCompoundCurves?", True":", False");
    cmd << ")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importCATIA(std::string n, const bool testVolumicProperties, const bool splitCompoundCurves)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importCATIA ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    //creation de la commande de création
    CommandImportCATIA *command = new CommandImportCATIA(getLocalContext(), n, testVolumicProperties, splitCompoundCurves);
    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importCATIA(\""<<n<<"\"";
    if (!testVolumicProperties || splitCompoundCurves)
    	cmd << (testVolumicProperties?", True":", False") << (splitCompoundCurves?", True":", False");
    cmd << ")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importMDL(std::string n, const bool all, const bool useAreaName,
		std::string prefixName, int deg_min, int deg_max)
{
#ifdef USE_MDLPARSER    
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importMDL ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    // création de l'importateur de modélisation/topo MDL
    Internal::ImportMDLImplementation* impl =
	  new Internal::ImportMDLImplementation(getLocalContext(), n, all, useAreaName, deg_min, deg_max);
    if (!prefixName.empty())
    	impl->setPrefix(prefixName);

    Utils::Unit::lengthUnit luMdl = impl->getLengthUnit();
    Utils::Unit::lengthUnit luCtx = getContext().getLengthUnit();

    Internal::CommandComposite* commandCompo = 0;
    Internal::CommandInternal* command = 0;

    // gestion du changement d'unité de longueur
    Internal::CommandChangeLengthUnit* commandCU = 0;
    if (luCtx == Utils::Unit::undefined){
    	// si l'unité n'est pas définie, on se met dans celle du mdl
    	commandCU = new Internal::CommandChangeLengthUnit(getLocalContext(), luMdl);

    	commandCompo =
    			new Internal::CommandComposite(getLocalContext(), "Import Mdl (géométrie) avec changement d'unité de longueur");

    	commandCompo->addCommand(commandCU);
    }
    else if (luMdl != luCtx) {
    	// cas où l'unité est définie, mais différente dans le MDL
    	double factor = Utils::Unit::computeFactor(luMdl, luCtx);
    	impl->setScale(factor);
    }

    //creation de la commande d'importation
    CommandImportMDL *commandImport = new CommandImportMDL(getLocalContext(), impl);

    if (commandCompo){
    	commandCompo->addCommand(commandImport);
    	command = commandCompo;
    }
    else
    	command = commandImport;

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importMDL(\""<<n<<"\","
            <<(all?"True":"False")
			<<", "
			<<(useAreaName?"True":"False")
			<<", "
			<<"\""<<prefixName<<"\""
			<<", "
			<<(short)deg_min
			<<", "
			<<(short)deg_max
			<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    // on peut détruire l'importateur de MDL
    delete impl;

	Internal::M3DCommandResultIfc*	cmdResult	=
	        new Internal::M3DCommandResult (*command);
	return cmdResult;
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("ImportMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER    
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importMDL(std::string n, std::string groupe)
{
#ifdef USE_MDLPARSER    
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importMDL ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    // création de l'importateur de modélisation/topo MDL
    Internal::ImportMDLImplementation* impl = new Internal::ImportMDLImplementation(getLocalContext(), n, groupe);

    Utils::Unit::lengthUnit luMdl = impl->getLengthUnit();
    Utils::Unit::lengthUnit luCtx = getContext().getLengthUnit();

    Internal::CommandComposite* commandCompo = 0;
    Internal::CommandInternal* command = 0;

    // gestion du changement d'unité de longueur
    Internal::CommandChangeLengthUnit* commandCU = 0;
    if (luCtx == Utils::Unit::undefined){
    	// si l'unité n'est pas définie, on se met en mètre
    	commandCU = new Internal::CommandChangeLengthUnit(getLocalContext(), Utils::Unit::meter);

    	commandCompo =
    			new Internal::CommandComposite(getLocalContext(), "Import Mdl (géométrie) avec changement d'unité de longueur");

    	commandCompo->addCommand(commandCU);

    	double factor = Utils::Unit::computeMeterFactor(luMdl);
    	impl->setScale(factor);
    }
    else if (luMdl != luCtx) {
    	// cas où l'unité est définie, mais différente dans le MDL
    	double factor = Utils::Unit::computeFactor(luMdl, luCtx);
    	impl->setScale(factor);
    }

    //creation de la commande d'importation
    CommandImportMDL *commandImport = new CommandImportMDL(getLocalContext(), impl);

    if (commandCompo){
    	commandCompo->addCommand(commandImport);
    	command = commandCompo;
    }
    else
    	command = commandImport;

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importMDL(\""<<n<<"\",\""
            <<groupe<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    // on peut détruire l'importateur de MDL
    delete impl;

	Internal::M3DCommandResultIfc*	cmdResult	=
	        new Internal::M3DCommandResult (*command);
	return cmdResult;
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("ImportMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER    
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::importMDL(std::string n, std::vector<std::string>& zones)
{
#ifdef USE_MDLPARSER    
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::importMDL ("<<n<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    // création de l'importateur de modélisation/topo MDL
    Internal::ImportMDLImplementation* impl = new Internal::ImportMDLImplementation(getLocalContext(), n, zones);

    Utils::Unit::lengthUnit luMdl = impl->getLengthUnit();
    Utils::Unit::lengthUnit luCtx = getContext().getLengthUnit();

    Internal::CommandComposite* commandCompo = 0;
    Internal::CommandInternal* command = 0;

    // gestion du changement d'unité de longueur
    Internal::CommandChangeLengthUnit* commandCU = 0;
    if (luCtx == Utils::Unit::undefined){
    	// si l'unité n'est pas définie, on se met en mètre
    	commandCU = new Internal::CommandChangeLengthUnit(getLocalContext(), Utils::Unit::meter);

    	commandCompo =
    			new Internal::CommandComposite(getLocalContext(), "Import Mdl (géométrie) avec changement d'unité de longueur");

    	commandCompo->addCommand(commandCU);

    	double factor = Utils::Unit::computeMeterFactor(luMdl);
    	impl->setScale(factor);
    }
    else if (luMdl != luCtx) {
    	// cas où l'unité est définie, mais différente dans le MDL
    	double factor = Utils::Unit::computeFactor(luMdl, luCtx);
    	impl->setScale(factor);
    }

    //creation de la commande d'importation
    CommandImportMDL *commandImport = new CommandImportMDL(getLocalContext(), impl);

    if (commandCompo){
    	commandCompo->addCommand(commandImport);
    	command = commandCompo;
    }
    else
    	command = commandImport;

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().importMDL(\""<<n<<"\",[";
    for(unsigned int i=0;i<zones.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< zones[i]<<"\"";
    }
    cmd << "])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    // on peut détruire l'importateur de MDL
    delete impl;

	Internal::M3DCommandResultIfc*	cmdResult	=
	        new Internal::M3DCommandResult (*command);
	return cmdResult;
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("ImportMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER
}
/*----------------------------------------------------------------------------*/
void GeomManager::mdl2CommandesMagix3D(std::string n, const bool withTopo)
{
#ifdef USE_MDLPARSER
    // création de l'importateur de modélisation/topo MDL
    Internal::ImportMDL2Commandes* impl = new Internal::ImportMDL2Commandes(getLocalContext(), n, withTopo);

    Utils::Unit::lengthUnit luMdl = impl->getLengthUnit();
    Utils::Unit::lengthUnit luCtx = getContext().getLengthUnit();

    // gestion du changement d'unité de longueur
    if (luCtx == Utils::Unit::undefined){
    	// si l'unité n'est pas définie, on se met dans celle du mdl
    	getContext().setLengthUnit(luMdl);
    }
    else if (luMdl != luCtx) {
    	// cas où l'unité est définie, mais différente dans le MDL
    	double factor = Utils::Unit::computeFactor(luMdl, luCtx);
    	impl->setScale(factor);
    }

    impl->perform();

    // on peut détruire l'importateur de MDL
    delete impl;
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("mdl2CommandesMagix3D non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER    
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportMDL(std::vector<std::string>& ge,
        const std::string& n)
{
#ifdef USE_MDLPARSER
#ifdef _DEBUG2
    std::cout<<"exportMDL avec "<<ge.size()<<" noms entités"<<std::endl;
#endif

    std::vector<Geom::GeomEntity*> geomEntities;
    for (uint i=0; i<ge.size(); i++)
        geomEntities.push_back(getEntity(ge[i]));

    return exportMDL(geomEntities, n);
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("exportMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER    
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportMDL(std::vector<Geom::GeomEntity*>& geomEntities,
        const std::string& n)
{
#ifdef USE_MDLPARSER
#ifdef _DEBUG2
    std::cout<<"exportMDL avec "<<geomEntities.size()<<" entités"<<std::endl;
#endif

    // création de l'exportateur de modélisation/topo MDL
    Internal::ExportMDLImplementation* impl =
            new Internal::ExportMDLImplementation(getLocalContext(), geomEntities, n);

    //creation de la commande d'exportation
    CommandExportMDL *command = new CommandExportMDL(getLocalContext(), impl);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportMDL([";
    for(unsigned int i=0;i<geomEntities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< geomEntities[i]->getName()<<"\"";
    }
    cmd <<"], \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    // on peut détruire l'exportateur de MDL
    delete impl;

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("exportMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER    
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportVTK(const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportVTK"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportVTK *command = new CommandExportVTK(getLocalContext(), n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportVTK(";
    cmd <<"\""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportVTK(std::vector<std::string>& ge,
		const std::string& n)
{
    std::vector<Geom::GeomEntity*> geomEntities;
    for (uint i=0; i<ge.size(); i++)
        geomEntities.push_back(getEntity(ge[i]));

    return exportVTK(geomEntities, n);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportVTK(std::vector<Geom::GeomEntity*>& geomEntities,
        const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportVTK avec "<<geomEntities.size()<<" entités"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportVTK *command = new CommandExportVTK(getLocalContext(), geomEntities, n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportVTK([";
    for(unsigned int i=0;i<geomEntities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< geomEntities[i]->getName()<<"\"";
    }
    cmd <<"], \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportSTL(const std::string& ge,
		const std::string& n)
{
    return exportSTL(getEntity(ge), n);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportSTL(Geom::GeomEntity* geomEntity,
        const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportSTL pour "<<geomEntity->getName()<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportSTL *command = new CommandExportSTL(getLocalContext(), geomEntity, n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportSTL(";
    cmd << "\""<< geomEntity->getName()<<"\"";
    cmd <<", \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportMLI(const std::string& n)
{
	std::cout<<"exportMLI poyop"<<std::endl;
#ifdef _DEBUG2
    std::cout<<"exportMLI"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportMLI *command = new CommandExportMLI(getLocalContext(), n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportMLI(";
    cmd <<"\""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportMLI(std::vector<std::string>& ge,
		const std::string& n)
{
    std::vector<Geom::GeomEntity*> geomEntities;
    for (uint i=0; i<ge.size(); i++)
        geomEntities.push_back(getEntity(ge[i]));

    return exportMLI(geomEntities, n);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportMLI(std::vector<Geom::GeomEntity*>& geomEntities,
        const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportMLI avec "<<geomEntities.size()<<" entités"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportMLI *command = new CommandExportMLI(getLocalContext(), geomEntities, n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportMLI([";
    for(unsigned int i=0;i<geomEntities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< geomEntities[i]->getName()<<"\"";
    }
    cmd <<"], \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportBREP(const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportBREP"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportBREP *command = new CommandExportBREP(getLocalContext(), n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportBREP(";
    cmd <<"\""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportBREP(std::vector<std::string>& ge,
		const std::string& n)
{
    std::vector<Geom::GeomEntity*> geomEntities;
    for (uint i=0; i<ge.size(); i++)
        geomEntities.push_back(getEntity(ge[i]));

    return exportBREP(geomEntities, n);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportBREP(std::vector<Geom::GeomEntity*>& geomEntities,
        const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportBREP avec "<<geomEntities.size()<<" entités"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportBREP *command = new CommandExportBREP(getLocalContext(), geomEntities, n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportBREP([";
    for(unsigned int i=0;i<geomEntities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< geomEntities[i]->getName()<<"\"";
    }
    cmd <<"], \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportSTEP(const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportSTEP"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportSTEP *command = new CommandExportSTEP(getLocalContext(), n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportSTEP(";
    cmd <<"\""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportSTEP(std::vector<std::string>& ge,
		const std::string& n)
{
    std::vector<Geom::GeomEntity*> geomEntities;
    for (uint i=0; i<ge.size(); i++)
        geomEntities.push_back(getEntity(ge[i]));

    return exportSTEP(geomEntities, n);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportSTEP(std::vector<Geom::GeomEntity*>& geomEntities,
        const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportSTEP avec "<<geomEntities.size()<<" entités"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportSTEP *command = new CommandExportSTEP(getLocalContext(), geomEntities, n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportSTEP([";
    for(unsigned int i=0;i<geomEntities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< geomEntities[i]->getName()<<"\"";
    }
    cmd <<"], \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportIGES(const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportIGES"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportIGES *command = new CommandExportIGES(getLocalContext(), n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportIGES(";
    cmd <<"\""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportIGES(std::vector<std::string>& ge,
		const std::string& n)
{
    std::vector<Geom::GeomEntity*> geomEntities;
    for (uint i=0; i<ge.size(); i++)
        geomEntities.push_back(getEntity(ge[i]));

    return exportIGES(geomEntities, n);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::exportIGES(std::vector<Geom::GeomEntity*>& geomEntities,
        const std::string& n)
{
#ifdef _DEBUG2
    std::cout<<"exportIGES avec "<<geomEntities.size()<<" entités"<<std::endl;
#endif

    //creation de la commande d'exportation
    CommandExportIGES *command = new CommandExportIGES(getLocalContext(), geomEntities, n);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().exportIGES([";
    for(unsigned int i=0;i<geomEntities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< geomEntities[i]->getName()<<"\"";
    }
    cmd <<"], \""<<n<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
cut(std::string tokeep,std::vector<std::string>& tocut)
{
    std::vector<GeomEntity*> vge;
    convert(tocut, vge);

   return cut(getEntity(tokeep),vge);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
cut(Geom::GeomEntity* tokeep, std::vector<Geom::GeomEntity*>& tocut)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::cut (";
    for(unsigned int i=0;i<tocut.size();i++){
        if(i!=0)
            message<<", ";
        message << tocut[i]->getName();
    }
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    std::vector<Geom::GeomEntity*> tokeeps;
    tokeeps.push_back(tokeep);
    if (Internal::EntitiesHelper::hasTopoRef(tokeeps)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Différences entre géométries avec topologies");

        commandGeom = new CommandCut(getLocalContext(),tokeep, tocut);
        commandCompo->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        commandGeom = new CommandCut(getLocalContext(),tokeep, tocut);
        command = commandGeom;
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().cut(\""<<tokeep->getName()<<"\",[";
    for(unsigned int i=0;i<tocut.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< tocut[i]->getName()<<"\"";
    }
    cmd <<"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command, commandGeom);

    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
cut(std::vector<std::string>& tokeep,std::vector<std::string>& tocut)
{
    std::vector<GeomEntity*> vge_keep, vge_cut;

    for (uint i=0; i<tokeep.size(); i++)
        vge_keep.push_back(getEntity(tokeep[i]));

    for (uint i=0; i<tocut.size(); i++)
        vge_cut.push_back(getEntity(tocut[i]));

   return cut(vge_keep, vge_cut);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
cut(std::vector<Geom::GeomEntity*>& tokeep,
        std::vector<Geom::GeomEntity*>& tocut)
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::cut ([";
    for(unsigned int i=0;i<tokeep.size();i++){
        if(i!=0)
            message<<", ";
        message << tokeep[i]->getName();
    }
    message<<"], [";
    for(unsigned int i=0;i<tocut.size();i++){
        if(i!=0)
            message<<", ";
        message << tocut[i]->getName();
    }
    message<<"])";

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;
    Geom::CommandEditGeom *commandGeom = 0;


    Internal::CommandComposite* commandCompo =
         new Internal::CommandComposite(getLocalContext(), "Différences entre géométries avec plusieurs entités à couper");

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(tokeep)){
    	for(unsigned int i=0;i<tokeep.size();i++)
    	{
    		commandGeom = new CommandCut(getLocalContext(),tokeep[i], tocut);
    		commandCompo->addCommand(commandGeom);

    		Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
    				commandGeom);
    		commandCompo->addCommand(commandTopo);
    	}
    }
    else {
    	for(unsigned int i=0;i<tokeep.size();i++)
    	{
    		commandGeom = new CommandCut(getLocalContext(),tokeep[i], tocut);
    		commandCompo->addCommand(commandGeom);
    	}
    }

    command = commandCompo;

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().cut([";
    for(unsigned int i=0;i<tokeep.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< tokeep[i]->getName()<<"\"";
    }
    cmd <<"], [";
    for(unsigned int i=0;i<tocut.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< tocut[i]->getName()<<"\"";
    }
    cmd <<"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command, commandGeom);

    return cmdResult;
}

/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
glue(std::vector<std::string>& entities)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<entities.size(); i++)
        vge.push_back(getEntity(entities[i]));

    return glue(vge);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::glue( std::vector<Geom::GeomEntity*>& entities)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::glue (";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            message<<", ";
        message << entities[i]->getName();
    }
    message<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
    if (Internal::EntitiesHelper::hasTopoRef(entities)){

        Internal::CommandComposite* commandCompo =
             new Internal::CommandComposite(getLocalContext(), "Collage entre géométries avec topologies");

        Geom::CommandEditGeom *commandGeom = new CommandGluing(getLocalContext(),entities);
        commandCompo->addCommand(commandGeom);

        Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                commandGeom);
        commandCompo->addCommand(commandTopo);

        command = commandCompo;
    }
    else {
        command = new CommandGluing(getLocalContext(),entities);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().glue([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< entities[i]->getName()<<"\"";
    }
    cmd <<"])";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	Internal::M3DCommandResultIfc*	cmdResult	=
									new Internal::M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
section(std::vector<std::string>& entities, std::string tool)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<entities.size(); i++)
        vge.push_back(getEntity(entities[i]));

    GeomEntity* ge_tool = getEntity(tool);
    return section(vge,ge_tool);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::section( std::vector<Geom::GeomEntity*>& entities,
    GeomEntity* tool)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::section ([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            message<<", ";
        message << entities[i]->getName();
    }
    message<<"], "<<tool->getName()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;


    if(entities.size()==1){
        // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
        if (Internal::EntitiesHelper::hasTopoRef(entities)){

            Internal::CommandComposite* commandCompo =
                    new Internal::CommandComposite(getLocalContext(), "Section par un plan entre géométries avec topologies");

            Geom::CommandEditGeom *commandGeom =
                    new CommandSection(getLocalContext(), entities, tool);

            commandCompo->addCommand(commandGeom);

            Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                    commandGeom);
            commandCompo->addCommand(commandTopo);

            command = commandCompo;
        }
        else {
            command = new CommandSection(getLocalContext(),entities,tool);
        }
    }
    else{
        // est-ce qu'il y a une topologie parmis les entités géométriques sélectionnées ?
        if (Internal::EntitiesHelper::hasTopoRef(entities)){

            Internal::CommandComposite* commandCompo=
                 new Internal::CommandComposite(getLocalContext(), "Section entre géométries avec topologies");

            Geom::CommandEditGeom *commandGeom =
                    new CommandSection(getLocalContext(),entities, tool);

            commandCompo->addCommand(commandGeom);
            Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
                    commandGeom);
            commandCompo->addCommand(commandTopo);

//            for(unsigned int i_entities=0;i_entities<entities.size();i_entities++){
//                std::vector<GeomEntity*> singleEntity;
//                singleEntity.push_back( entities[i_entities]);
//                Geom::CommandEditGeom *commandGeom =
//                        new CommandSection(getLocalContext(),singleEntity, tool);
//
//                commandCompo->addCommand(commandGeom);
//
//                Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
//                        commandGeom);
//                commandCompo->addCommand(commandTopo);
//
//            }

            command = commandCompo;
        }
        else {

            Internal::CommandComposite* commandCompo =
                 new Internal::CommandComposite(getLocalContext(), "Section entre géométries");

            Geom::CommandEditGeom *commandGeom =
                    new CommandSection(getLocalContext(),entities, tool);

            commandCompo->addCommand(commandGeom);
//            for(unsigned int i_entities=0;i_entities<entities.size();i_entities++){
//                std::vector<GeomEntity*> singleEntity;
//                singleEntity.push_back( entities[i_entities]);
//                Geom::CommandEditGeom *commandGeom =
//                        new CommandSection(getLocalContext(),singleEntity, tool);
//
//                commandCompo->addCommand(commandGeom);
//            }

            command = commandCompo;
        }
    }


    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().section([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< entities[i]->getName()<<"\"";
    }
    cmd<<"], \""<<tool->getName()<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
sectionByPlane(std::vector<std::string>& entities,
        Utils::Math::Plane* tool, std::string planeGroupName)
{
    std::vector<GeomEntity*> vge;
    for (uint i=0; i<entities.size(); i++)
        vge.push_back(getEntity(entities[i]));


    return sectionByPlane(vge,tool,planeGroupName);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::
sectionByPlane( std::vector<Geom::GeomEntity*>& entities,
                Utils::Math::Plane* tool, std::string planeGroupName)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::sectionByPlane ([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            message<<", ";
        message << entities[i]->getName();
    }
    message<<"], "<<tool->getScriptCommand()<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    if (entities.empty()){
    	throw TkUtil::Exception(TkUtil::UTF8String ("Aucune entité sélectionnée pour sectionByPlane", TkUtil::Charset::UTF_8));
    }

    Internal::CommandInternal* command = 0;

    if (Internal::EntitiesHelper::hasTopoRef(entities)){

    	Internal::CommandComposite* commandCompo =
    			new Internal::CommandComposite(getLocalContext(), "Section par un plan entre géométries avec topologies");

    	Geom::CommandEditGeom *commandGeom =
    			new CommandSectionByPlane(getLocalContext(), entities, tool, planeGroupName);

    	commandCompo->addCommand(commandGeom);

    	Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
    			commandGeom);
    	commandCompo->addCommand(commandTopo);

    	command = commandCompo;
    }
    else {
    	command = new CommandSectionByPlane(getLocalContext(),entities,tool,planeGroupName);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().sectionByPlane([";
    for(unsigned int i=0;i<entities.size();i++){
        if(i!=0)
            cmd << ", ";
        cmd << "\""<< entities[i]->getName()<<"\"";
    }
    cmd<<"], "<<tool->getScriptCommand();
    cmd<<", \""<<planeGroupName<<"\"";
    cmd<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}

/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::splitCurve(std::string entity, const Point& pt)
{
	Geom::Curve* crv = getCurve(entity, true);
	return splitCurve(crv, pt);
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::splitCurve(Geom::Curve* crv, const Point& pt)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::splitCurve ("<<crv->getName()<<", "<<pt<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));

    Internal::CommandInternal* command = 0;

    if (!crv->getRefTopo().empty()){

    	Internal::CommandComposite* commandCompo =
    			new Internal::CommandComposite(getLocalContext(), "Découpage d'une courbe avec topologie");

    	Geom::CommandEditGeom *commandGeom =
    			new CommandSplitCurve(getLocalContext(),crv,pt);

    	commandCompo->addCommand(commandGeom);

    	Topo::CommandModificationTopo* commandTopo = new Topo::CommandModificationTopo(getLocalContext(),
    			commandGeom);
    	commandCompo->addCommand(commandTopo);

    	command = commandCompo;
    }
    else {
    	command = new CommandSplitCurve(getLocalContext(),crv,pt);
    }

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().splitCurve("<<crv->getName()<<", "<<pt<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
std::vector<Volume*> GeomManager::getVolumesObj() const
{
    std::vector<Volume*> v_vol;
    for (std::vector<Volume*>::const_iterator iter = m_volumes.begin();
    		iter != m_volumes.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_vol.push_back(*iter);

    return v_vol;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManager::getVolumes() const
{
    std::vector<std::string> v_vol;
    for (std::vector<Volume*>::const_iterator iter = m_volumes.begin();
    		iter != m_volumes.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_vol.push_back((*iter)->getName());

    return v_vol;
}
/*----------------------------------------------------------------------------*/
std::vector<Surface*> GeomManager::getSurfacesObj() const
{
    std::vector<Surface*> v_surf;
    for (std::vector<Surface*>::const_iterator iter = m_surfaces.begin();
    		iter != m_surfaces.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_surf.push_back(*iter);

    return v_surf;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManager::getSurfaces() const
{
    std::vector<std::string> v_surf;
    for (std::vector<Surface*>::const_iterator iter = m_surfaces.begin();
    		iter != m_surfaces.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_surf.push_back((*iter)->getName());

    return v_surf;
}
/*----------------------------------------------------------------------------*/
std::vector<Curve*> GeomManager::getCurvesObj() const
{
    std::vector<Curve*> v_cur;
    for (std::vector<Curve*>::const_iterator iter = m_curves.begin();
    		iter != m_curves.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_cur.push_back(*iter);

    return v_cur;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManager::getCurves() const
{
    std::vector<std::string> v_cur;
    for (std::vector<Curve*>::const_iterator iter = m_curves.begin();
    		iter != m_curves.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_cur.push_back((*iter)->getName());

    return v_cur;
}
/*----------------------------------------------------------------------------*/
std::vector<Vertex*> GeomManager::getVerticesObj() const
{
    std::vector<Vertex*> v_ver;
    for (std::vector<Vertex*>::const_iterator iter = m_vertices.begin();
    		iter != m_vertices.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_ver.push_back(*iter);

    return v_ver;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManager::getVertices() const
{
    std::vector<std::string> v_ver;
    for (std::vector<Vertex*>::const_iterator iter = m_vertices.begin();
    		iter != m_vertices.end(); ++iter)
    	if (!(*iter)->isDestroyed())
    		v_ver.push_back((*iter)->getName());

    return v_ver;
}
/*----------------------------------------------------------------------------*/
int GeomManager::getNbVertices() const
{
    return getVertices().size();
}
/*----------------------------------------------------------------------------*/
int GeomManager::getNbCurves() const
{
    return getCurves().size();
}
/*----------------------------------------------------------------------------*/
int GeomManager::getNbSurfaces() const
{
    return getSurfaces().size();
}
/*----------------------------------------------------------------------------*/
int GeomManager::getNbVolumes() const
{
    return getVolumes().size();
}
/*----------------------------------------------------------------------------*/
Volume* GeomManager::getVolume(const std::string& name, const bool exceptionIfNotFound) const
{
    Volume* vol = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated()){
    	if (Volume::isA(name))
    		new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::GeomVolume)->renameWithShiftingId(name);
    	else
    		return vol;
    }
    else
        new_name = name;

    for (std::vector<Volume*>::const_iterator iter = m_volumes.begin();
            iter != m_volumes.end(); ++iter)
        if (new_name == (*iter)->getName())
            vol = (*iter);

    if (exceptionIfNotFound && vol == 0){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message <<"getVolume impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le GeomManager";
        throw TkUtil::Exception(message);
    }

    if (vol && vol->isDestroyed()){
    	TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    	message <<"getVolume impossible, entité \""<<new_name<<"\" est détruite";
    	throw Utils::IsDestroyedException(message);
    }

    return vol;
}
/*----------------------------------------------------------------------------*/
Surface* GeomManager::getSurface(const std::string& name, const bool exceptionIfNotFound) const
{
    Surface* surf = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated()){
    	if (Surface::isA(name))
    		new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::GeomSurface)->renameWithShiftingId(name);
    	else
    		return surf;
    }
    else
        new_name = name;

    for (std::vector<Surface*>::const_iterator iter = m_surfaces.begin();
            iter != m_surfaces.end(); ++iter)
        if (new_name == (*iter)->getName())
            surf = (*iter);

    if (exceptionIfNotFound && surf == 0){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message <<"getSurface impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le GeomManager";
        throw TkUtil::Exception(message);
    }

    if (surf && surf->isDestroyed()){
    	TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    	message <<"getSurface impossible, entité \""<<new_name<<"\" est détruite";
    	throw Utils::IsDestroyedException(message);
    }

    return surf;
}
/*----------------------------------------------------------------------------*/
Curve* GeomManager::getCurve(const std::string& name, const bool exceptionIfNotFound) const
{
    Curve* curve = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated()){
    	if (Curve::isA(name))
    		new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::GeomCurve)->renameWithShiftingId(name);
    	else
    		return curve;
    }
    else
        new_name = name;

    for (std::vector<Curve*>::const_iterator iter = m_curves.begin();
            iter != m_curves.end(); ++iter)
        if (new_name == (*iter)->getName())
            curve = (*iter);

    if (exceptionIfNotFound && curve == 0){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message <<"getCurve impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le GeomManager";
        throw TkUtil::Exception(message);
    }

    if (curve && curve->isDestroyed()){
    	TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    	message <<"getCurve impossible, entité \""<<new_name<<"\" est détruite";
    	throw Utils::IsDestroyedException(message);
    }

    return curve;
}
/*----------------------------------------------------------------------------*/
Vertex* GeomManager::getVertex(const std::string& name, const bool exceptionIfNotFound) const
{
    Vertex* vertex = 0;

    std::string new_name;
    if (getLocalContext().getNameManager().isShiftingIdActivated()){
    	if (Vertex::isA(name))
    		new_name = getLocalContext().getNameManager().getTypeDedicatedNameManager(Utils::Entity::GeomVertex)->renameWithShiftingId(name);
    	else
    		return vertex;
    }
    else
        new_name = name;

    for (std::vector<Vertex*>::const_iterator iter = m_vertices.begin();
            iter != m_vertices.end(); ++iter)
        if (new_name == (*iter)->getName())
            vertex = (*iter);

    if (exceptionIfNotFound && vertex == 0){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message <<"getVertex impossible, entité \""<<new_name<<"\" n'a pas été trouvée dans le GeomManager";
        throw TkUtil::Exception(message);
    }

    if (vertex && vertex->isDestroyed()){
     	TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
     	message <<"getVertex impossible, entité \""<<new_name<<"\" est détruite";
     	throw Utils::IsDestroyedException(message);
     }

    return vertex;
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getVertexAt(const Point& pt1) const
{
	// il pourrait y en avoir aucun ou plusieurs, on n'en veut qu'un
	std::vector<Vertex*> vertices;

	for (std::vector<Vertex*>::const_iterator iter = m_vertices.begin();
			iter != m_vertices.end(); ++iter)
		if ((*iter)->getCoord() == pt1)
			vertices.push_back(*iter);

	if (vertices.size() == 1)
		return vertices[0]->getName();
	else {
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getVertexAt impossible, on trouve "<<vertices.size()<<" sommets à la position "<<pt1;
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getCurveAt(const Point& pt1, const Point& pt2, const Point& pt3) const
{
	// il pourrait y en avoir aucune ou plusieurs, on n'en veut qu'une
	std::vector<Curve*> curves;

	for (std::vector<Curve*>::const_iterator iter = m_curves.begin();
	            iter != m_curves.end(); ++iter){
		Utils::Math::Point pt4, pt5, pt6;
		(*iter)->getPoint(0.0, pt4, true);
		(*iter)->getPoint(0.5, pt5, true);
		(*iter)->getPoint(1.0, pt6, true);

		if (pt4 == pt1 && pt5 == pt2 && pt6 == pt3)
			curves.push_back(*iter);
	}

	if (curves.size() == 1)
		return curves[0]->getName();
	else {
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getCurveAt impossible, on trouve "<<curves.size()<<" courbes aux positions "
				<<pt1<<", "<<pt2<<", "<<pt3;
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getSurfaceAt(std::vector<Utils::Math::Point>& pts) const
{
#ifdef _DEBUG2
	std::cout<<"getSurfaceAt avec les pts";
	for (uint i=0; i<pts.size(); i++)
		std::cout<<" "<<pts[i];
	std::cout<<std::endl;
#endif

	// il pourrait y en avoir aucune ou plusieurs, on n'en veut qu'une
	std::vector<Surface*> surfaces;

	for (std::vector<Surface*>::const_iterator iter = m_surfaces.begin();
	            iter != m_surfaces.end(); ++iter){
		std::vector<Geom::Vertex*> vertices;
		(*iter)->get(vertices);
		uint i;
		if (vertices.size() != pts.size())
			continue;
		for (i=0; i<vertices.size() && vertices[i]->getCoord() == pts[i]; i++){
		}
#ifdef _DEBUG2
		std::cout<<"Pour "<<(*iter)->getName()<<" vertices.size() = "<<vertices.size()<<" et i = "<<i<<std::endl;
#endif

		if (i == vertices.size())
			surfaces.push_back(*iter);
	} // end for iter = m_surfaces.begin()

	if (surfaces.size() == 1)
		return surfaces[0]->getName();
	else {
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getCurveAt impossible, on trouve "<<surfaces.size()
				<<" surfaces avec ces "
				<<pts.size()<<" positions";
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getVolumeAt(std::vector<Utils::Math::Point>& pts) const
{
#ifdef _DEBUG2
	std::cout<<"getVolumeAt avec les pts";
	for (uint i=0; i<pts.size(); i++)
		std::cout<<" "<<pts[i];
	std::cout<<std::endl;
#endif

	// il pourrait y en avoir aucune ou plusieurs, on n'en veut qu'une
	std::vector<Volume*> volumes;

	for (std::vector<Volume*>::const_iterator iter = m_volumes.begin();
	            iter != m_volumes.end(); ++iter){
		std::vector<Geom::Vertex*> vertices;
		(*iter)->get(vertices);
		uint i;
		if (vertices.size() != pts.size())
			continue;
		for (i=0; i<vertices.size() && vertices[i]->getCoord() == pts[i]; i++){
		}
#ifdef _DEBUG2
		std::cout<<"Pour "<<(*iter)->getName()<<" vertices.size() = "<<vertices.size()<<" et i = "<<i<<std::endl;
#endif

		if (i == vertices.size())
			volumes.push_back(*iter);
	} // end for iter = m_volumes.begin()

	if (volumes.size() == 1)
		return volumes[0]->getName();
	else {
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getCurveAt impossible, on trouve "<<volumes.size()
				<<" volumes avec ces "
				<<pts.size()<<" positions";
		throw TkUtil::Exception(message);
	}
}
/*----------------------------------------------------------------------------*/
GeomEntity* GeomManager::getEntity(const std::string& name, const bool exceptionIfNotFound) const
{
	//std::cout<<"GeomManager::getEntity pour "<<name<<std::endl;
    GeomEntity* ge = 0;

    if (!ge)
        ge = getVolume(name, false);

    if (!ge)
        ge = getSurface(name, false);

    if (!ge)
        ge = getCurve(name, false);

    if (!ge)
        ge = getVertex(name, false);

    if (exceptionIfNotFound && ge == 0){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message <<"getEntity impossible, entité \""<<name<<"\" n'a pas été trouvée dans le GeomManager";
        throw TkUtil::Exception(message);
    }

    return ge;
}
/*----------------------------------------------------------------------------*/
int GeomManager::getIndexOf(Vertex* v) const
{
	int index = -1;
	for (unsigned int i=0; i<m_vertices.size(); i++){
		if(v->getUniqueId()==m_vertices[i]->getUniqueId())
			index = i;
	}
	if(index==-1){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getIndexOf impossible pour un sommet";
		throw TkUtil::Exception(message);
	}
	return index;
}
/*----------------------------------------------------------------------------*/
int GeomManager::getIndexOf(Curve* c) const
{
	int index = -1;
	for (unsigned int i=0; i<m_curves.size(); i++){
		if(c->getUniqueId()==m_curves[i]->getUniqueId())
			index = i;
	}
	if(index==-1){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getIndexOf impossible pour une courbe";
		throw TkUtil::Exception(message);
	}
	return index;
}
/*----------------------------------------------------------------------------*/
int GeomManager::getIndexOf(Surface* s) const
{
	int index = -1;

	for (unsigned int i=0; i<m_surfaces.size(); i++){
		if(s->getUniqueId()==m_surfaces[i]->getUniqueId())
			index = i;
	}
	if(index==-1){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getIndexOf impossible pour une surface";
		throw TkUtil::Exception(message);
	}
	return index;
}
/*----------------------------------------------------------------------------*/
int GeomManager::getIndexOf(Volume* v) const
{
	int index = -1;

	for (unsigned int i=0; i<m_volumes.size(); i++){
		if(v->getUniqueId()==m_volumes[i]->getUniqueId())
			index = i;
	}
	if(index==-1){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message <<"getIndexOf impossible pour un volume";
		throw TkUtil::Exception(message);
	}
	return index;
}

/*----------------------------------------------------------------------------*/
void GeomManager::addEntity (GeomEntity* ge)
{
    if(ge==0)
        return;

  switch (ge->getDim()){
    case 0: add((Geom::Vertex*)ge); break;
    case 1: add((Geom::Curve*)ge); break;
    case 2: add((Geom::Surface*)ge); break;
    case 3: add((Geom::Volume*)ge); break;
    default:
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, GeomManager::addEntity avec entité de dimension non prévue", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void GeomManager::removeEntity (GeomEntity* ge)
{
    if(ge==0)
        return;

    switch (ge->getDim()){
    case 3 : remove((Volume*)ge); break;
    case 2 : remove((Surface*)ge); break;
    case 1 : remove((Curve*)ge); break;
    case 0 : remove((Vertex*)ge); break;
    default:
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, GeomManager::removeEntity avec entité de dimension non prévue", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void GeomManager::remove (Volume* v)
{
    std::vector<Volume*>::iterator iter;
    iter = find(m_volumes.begin(), m_volumes.end(), v);
    if (iter != m_volumes.end())
        m_volumes.erase(iter);
    else
        throw TkUtil::Exception("Erreur interne, volume absent du GeomManager pour removeVolume");
}
/*----------------------------------------------------------------------------*/
void GeomManager::remove (Surface* s)
{
    std::vector<Surface*>::iterator iter;
    iter = find(m_surfaces.begin(), m_surfaces.end(), s);
    if (iter != m_surfaces.end())
        m_surfaces.erase(iter);
    else
        throw TkUtil::Exception("Erreur interne, surface absente du GeomManager pour removeSurface");
}
/*----------------------------------------------------------------------------*/
void GeomManager::remove (Curve* c)
{
    std::vector<Curve*>::iterator iter;
    iter = find(m_curves.begin(), m_curves.end(), c);
    if (iter != m_curves.end())
        m_curves.erase(iter);
    else
        throw TkUtil::Exception("Erreur interne, courbe absente du GeomManager pour removeCurve");
}
/*----------------------------------------------------------------------------*/
void GeomManager::remove (Vertex* v)
{
    std::vector<Vertex*>::iterator iter;
    iter = find(m_vertices.begin(), m_vertices.end(), v);
    if (iter != m_vertices.end())
        m_vertices.erase(iter);
    else
        throw TkUtil::Exception("Erreur interne, sommet absent du GeomManager pour removeVertex");
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getLastVolume() const
{
    std::string nom("");
    if (m_volumes.empty())
        return nom;
    // recherche du dernier non détruit, en partant de la fin
    int i;
    for (i=m_volumes.size()-1; i>=0 && m_volumes[i]->isDestroyed(); i--)
    {}
    if (i>=0)
        nom = m_volumes[i]->getName();
    return nom;
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getLastSurface() const
{
    std::string nom("");
    if (m_surfaces.empty())
        return nom;
    // recherche du dernier non détruit, en partant de la fin
    int i;
    for (i=m_surfaces.size()-1; i>=0 && m_surfaces[i]->isDestroyed(); i--)
    {}
    if (i>=0)
        nom = m_surfaces[i]->getName();
    return nom;
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getLastCurve() const
{
    std::string nom("");
    if (m_curves.empty())
        return nom;
    // recherche du dernier non détruit, en partant de la fin
    int i;
    for (i=m_curves.size()-1; i>=0 && m_curves[i]->isDestroyed(); i--)
    {}
    if (i>=0)
        nom = m_curves[i]->getName();
    return nom;
}
/*----------------------------------------------------------------------------*/
std::string GeomManager::getLastVertex() const
{
    std::string nom("");
    if (m_vertices.empty())
        return nom;
    // recherche du dernier non détruit, en partant de la fin
    int i;
    for (i=m_vertices.size()-1; i>=0 && m_vertices[i]->isDestroyed(); i--)
    {}
    if (i>=0)
        nom = m_vertices[i]->getName();
    return nom;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::addToGroup ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i];
    }
    message << "], "<<(short)dim<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));


    Mesh::CommandAddRemoveGroupName* command = 0;

    switch(dim){
    case(0):{
        // reconstitue le vecteur de sommets
        std::vector<Vertex*> vertices;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            vertices.push_back(getVertex(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), vertices, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    case(1):{
        // reconstitue le vecteur de courbes
        std::vector<Curve*> curves;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            curves.push_back(getCurve(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), curves, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    case(2):{
        // reconstitue le vecteur de surfaces
        std::vector<Surface*> surfaces;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            surfaces.push_back(getSurface(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), surfaces, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    case(3):{
        // reconstitue le vecteur de volumes
        std::vector<Volume*> volumes;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            volumes.push_back(getVolume(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), volumes, groupName, Mesh::CommandAddRemoveGroupName::add);
    }
    break;
    default:{
        TkUtil::Exception ("Dimension erronée");
    }
    break;
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().addToGroup ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i];
    }
    cmd << "\"], "<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::removeFromGroup ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i];
    }
    message << "], "<<(short)dim<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));


    Mesh::CommandAddRemoveGroupName* command = 0;

    switch(dim){
    case(0):{
        // reconstitue le vecteur de sommets
        std::vector<Vertex*> vertices;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            vertices.push_back(getVertex(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), vertices, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    case(1):{
        // reconstitue le vecteur de courbes
        std::vector<Curve*> curves;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            curves.push_back(getCurve(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), curves, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    case(2):{
        // reconstitue le vecteur de surfaces
        std::vector<Surface*> surfaces;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            surfaces.push_back(getSurface(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), surfaces, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    case(3):{
        // reconstitue le vecteur de volumes
        std::vector<Volume*> volumes;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            volumes.push_back(getVolume(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), volumes, groupName, Mesh::CommandAddRemoveGroupName::remove);
    }
    break;
    default:{
        TkUtil::Exception ("Dimension erronée");
    }
    break;
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().removeFromGroup ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i];
    }
    cmd << "\"], "<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManager::setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{

    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "GeomManager::setGroup ([";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            message <<", ";
        message << ve[i];
    }
    message << "], "<<(short)dim<<", "<<groupName<<")";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));


    Mesh::CommandAddRemoveGroupName* command = 0;

    switch(dim){
    case(0):{
        // reconstitue le vecteur de sommets
        std::vector<Vertex*> vertices;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            vertices.push_back(getVertex(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), vertices, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    case(1):{
        // reconstitue le vecteur de courbes
        std::vector<Curve*> curves;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            curves.push_back(getCurve(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), curves, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    case(2):{
        // reconstitue le vecteur de surfaces
        std::vector<Surface*> surfaces;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            surfaces.push_back(getSurface(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), surfaces, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    case(3):{
        // reconstitue le vecteur de volumes
        std::vector<Volume*> volumes;
        for (std::vector<std::string>::const_iterator iter = ve.begin();
                iter != ve.end(); ++iter)
            volumes.push_back(getVolume(*iter, true));

        command = new Mesh::CommandAddRemoveGroupName(getLocalContext(), volumes, groupName, Mesh::CommandAddRemoveGroupName::set);
    }
    break;
    default:{
        TkUtil::Exception ("Dimension erronée");
    }
    break;
    }

    CHECK_NULL_PTR_ERROR(command);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGeomManager().setGroup ([\"";
    for (uint i=0; i<ve.size(); i++){
        if (i)
            cmd <<"\", \"";
        cmd << ve[i];
    }
    cmd << "\"], "<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
                                    new Internal::M3DCommandResult (*command);
    return cmdResult;
}

/*----------------------------------------------------------------------------*/
void GeomManager::
convert(std::vector<std::string>& names, std::vector<GeomEntity*>& entities)
{
	for (uint i=0; i<names.size(); i++)
		entities.push_back(getEntity(names[i], true));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
