/*----------------------------------------------------------------------------*/
/*
 * \file CommandComposite.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/CommandComposite.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"

#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/ReferencedMutex.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Group {
class GroupEntity;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
CommandComposite::CommandComposite(Internal::Context& c, std::string name)
: Internal::CommandInternal (c, name)
{

}
/*----------------------------------------------------------------------------*/
CommandComposite::~CommandComposite()
{
	//std::cout<<"Destruction de CommandComposite ..."<<std::endl;
    for (std::vector<CommandInternal*>::iterator iter = m_commands.begin();
            iter != m_commands.end(); ++iter)
        delete *iter;
    m_commands.clear();
}
/*----------------------------------------------------------------------------*/
void CommandComposite::
internalExecute()
{
    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
    message << "CommandComposite::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << ", composée de "<<m_commands.size() << " commandes";

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    // exécution successive des différentes commandes
    for (std::vector<CommandInternal*>::iterator iter = m_commands.begin();
         iter != m_commands.end(); ++iter){
        (*iter)->execute();
        if ((*iter)->getStatus() != Command::DONE){
        	throw TkUtil::Exception((*iter)->getErrorMessage());
        }
    }

   // remplir le InfoCommand en fonction de ceux des différentes commandes, une fois ces dernières terminées
   Internal::InfoCommand& icmd = getInfoCommand();
   for (std::vector<CommandInternal*>::iterator iter = m_commands.begin();
        iter != m_commands.end(); ++iter) {
       Internal::InfoCommand& icmd_iter = (*iter)->getInfoCommand();

        for (uint i=0; i<icmd_iter.getNbGeomInfoEntity(); i++) {
            Geom::GeomEntity* ge = 0;
            Internal::InfoCommand::type t;
            icmd_iter.getGeomInfoEntity(i, ge, t);

            icmd.addGeomInfoEntity(ge, t);
        }

        std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd_iter.getTopoInfoEntity();
        for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
                iter_tei != topo_entities_info.end(); ++iter_tei){
            Topo::TopoEntity* te = iter_tei->first;
            Internal::InfoCommand::type t = iter_tei->second;

            icmd.addTopoInfoEntity(te, t);
        }

        for (uint i=0; i<icmd_iter.getNbMeshInfoEntity(); i++) {
            Mesh::MeshEntity* me = 0;
            Internal::InfoCommand::type t;
            icmd_iter.getMeshInfoEntity(i, me, t);

            icmd.addMeshInfoEntity(me, t);
        }

        std::map<Group::GroupEntity*, Internal::InfoCommand::type>& group_entities_info = icmd_iter.getGroupInfoEntity();
        for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::iterator iter_grp = group_entities_info.begin();
                iter_grp != group_entities_info.end(); ++iter_grp){
            Group::GroupEntity* grp = iter_grp->first;
            InfoCommand::type& t = iter_grp->second;

            icmd.addGroupInfoEntity(grp, t);
        } // end for ( ... iter ...)

        for (uint i=0; i<icmd_iter.getNbSysCoordInfoEntity(); i++) {
        	CoordinateSystem::SysCoord* rep = 0;
        	Internal::InfoCommand::type t;
        	icmd_iter.getSysCoordInfoEntity(i, rep, t);

        	icmd.addSysCoordInfoEntity(rep, t);
        }

        // on supprime de cette commande la liste des groupes pour s'assurer de ne faire le ménage (destruction) qu'une fois
        group_entities_info.clear();

        InfoCommand::type t_iter;
        InfoCommand::type t_loc;
        icmd_iter.getContextInfo(t_iter);
        icmd.getContextInfo(t_loc);
        if (t_loc < t_iter)
        	icmd.addContextInfo(t_iter);

    } // end for iter = m_commands.begin()

#ifdef _DEBUG2
   {
	   InfoCommand::type t_loc;
	   icmd.getContextInfo(t_loc);
	   std::cout<<"CommandComposite::internalExecute() => getContextInfo : "
			    <<InfoCommand::type2String(t_loc)<<std::endl;
   }
#endif
//   std::cout<<"Dans CommandComposite: "<<std::endl;
//   std::cout<< getInfoCommand() <<std::endl;

}
/*----------------------------------------------------------------------------*/
void CommandComposite::postExecute(bool hasError)
{
	if (hasError){
		// en cas d'erreur, il faut annuler les premières commandes qui se sont correctement terminées
		//std::cout<<"CommandComposite::postExecute, cas en erreur ..."<<std::endl;
		for (std::vector<CommandInternal*>::reverse_iterator iter = m_commands.rbegin();
				iter != m_commands.rend(); ++iter){
			//std::cout<<" Etat de la commande "<<(*iter)->getName()<<" : "<<CommandIfc::statusToString((*iter)->getStatus())<<std::endl;
			if ((*iter)->getStatus() == Command::DONE){
				(*iter)->undo();
				(*iter)->postExecute(true);
			}
		}
	}

}
/*----------------------------------------------------------------------------*/
void CommandComposite::internalUndo()
{
    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    // undo pour les différentes commandes
    for (std::vector<CommandInternal*>::reverse_iterator iter = m_commands.rbegin();
         iter != m_commands.rend(); ++iter)
        (*iter)->undo();

    // les entités créées sont dites détruites et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandComposite::internalRedo()
{
    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    startingOrcompletionLog (true);

    // redo pour les différentes commandes
    for (std::vector<CommandInternal*>::iterator iter = m_commands.begin();
         iter != m_commands.end(); ++iter)
        (*iter)->redo();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    startingOrcompletionLog (false);
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
