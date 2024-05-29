/*----------------------------------------------------------------------------*/
/*
 * \file CommandInternal.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/InformationLog.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/Mutex.h>
#include <TkUtil/UTF8String.h>

#include <stdio.h>	// stderr, ...
#include <ostream>

// OCC
#include <Standard_Failure.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
/*----------------------------------------------------------------------------*/
CommandInternal::CommandInternal(Internal::Context& c, const std::string& name)
: Utils::Command(name) ,
  m_context(c),
  m_info_command(), m_warning_to_pop_up (TkUtil::Charset::UTF_8),
  m_name_manager_before ( ), m_name_manager_after ( )
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandInternal::CommandInternal() pour la commande "<<name<<" uniqueName: "<<getUniqueName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
CommandInternal::CommandInternal(const CommandInternal& cmd)
: Utils::Command("Bidon"),
  m_context(cmd.m_context), m_warning_to_pop_up (TkUtil::Charset::UTF_8),
  m_name_manager_before ( ), m_name_manager_after ( )
{
    MGX_FORBIDDEN("CommandInternal::CommandInternal is not allowed.");
}
/*----------------------------------------------------------------------------*/
CommandInternal::~CommandInternal()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandInternal::~CommandInternal() pour la commande "<<getScriptComments()<<" uniqueName: "<<getUniqueName()<<std::endl;
#endif

	std::map<Group::GroupEntity*, Internal::InfoCommand::type>& gei = getInfoCommand().getGroupInfoEntity();

	for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::const_iterator iter = gei.begin();
			iter != gei.end(); ++iter) {
		if (iter->second == InfoCommand::DELETED){
#ifdef _DEBUG_MEMORY
			std::cout<<"groupe détruit car créé avec cette commande: "<< iter->first->getName()<<", id "<<iter->first->getUniqueId()<<std::endl;
#endif
			getContext().getLocalGroupManager().deleteEntity(iter->first);
		}
#ifdef _DEBUG_MEMORY
		else {
			std::cout<<"groupe non pris en compte : "<< iter->first->getName()<<", id "<<iter->first->getUniqueId()<<" car du type "<<Internal::InfoCommand::type2String(iter->second)<<std::endl;
		}
#endif
	}
	gei.clear();

	notifyObserversForDestruction ( );
	unregisterReferences ( );

	// remet les id comme avant la commande
	if (!m_name_manager_before.empty())
		getContext().getNameManager().setInternalStats(m_name_manager_before);

}
/*----------------------------------------------------------------------------*/
CommandInternal& CommandInternal::operator = (const CommandInternal&)
{
    MGX_FORBIDDEN("CommandInternal::operator = is not allowed.");
    return *this;
}
/*----------------------------------------------------------------------------*/
Utils::CommandIfc::status CommandInternal::execute()
{
    startingOrcompletionLog (true); // log de début de commande

    // suppression du contenu pour le cas d'un redo par exemple
    m_info_command.clear();

    Command::execute ( );   // => status = PROCESSING et suppression des commandes inutiles (=> modif getNameManager)

    getContext().getNameManager().getInternalStats(m_name_manager_before);

    //COMMAND_BEGIN_TRY_CATCH_BLOCK [explicité, EB]
    bool            hasError    = false;
    /* On alloue errorString avant un éventuel pthread_cancel ... */
    TkUtil::UTF8String     errorString (
						"Erreur non documentée.", TkUtil::Charset::UTF_8);
    try {

    	// on fait le ménage pour le cas d'une commande qui serait rejouée (maillage par ex)
    	m_warning_to_pop_up.clear();

    	// opérations particulières type OCAF...
    	preExecute();

        // ce qui est propre à la commande
        internalExecute();

        if (Command::CANCELED != getStatus ( )){
            // met à jour l'état de la visibilité des entités
            if (getContext().isGraphical())
                getContext().getLocalGroupManager().updateDisplayProperties(&getInfoCommand());

            // recherche les groupes vides, les détruits et ajoute l'info dans InfoCommand
            // et réciproquement pour les groupes détruits mais non vides
            getContext().getLocalGroupManager().updateDeletedGroups(&getInfoCommand());

			// Avant de changer le status, ce qui va informer tous les
			// observateurs, y compris la visu 3D, qui peut être tentée de faire
			// des affichages assez long, on informe (l'utilisateur) que la
			// commande est finie avec succès.
			TkUtil::UTF8String	forwardAnnonce (TkUtil::Charset::UTF_8);
			forwardAnnonce << "La commande " << getName ( )
			               << " s'est terminée avec succès. Modification de "
			               << "son status et actualisation de l'IHM dont la "
			               << "visu (potentiellement un peu long) en cours.";
			//log (TkUtil::InformationLog (forwardAnnonce));
			// [EB] on limite l'affichage de l'info au cas où les traces pour les commandes sont activées
			// car la fenêtre est poluée par trop de messages
			log (TkUtil::TraceLog (forwardAnnonce, TkUtil::Log::TRACE_1));

            // change le status et fait les callbacks
            setStatus (Command::DONE);
        }
        else
            hasError    = true;

    //COMMAND_COMPLETE_TRY_CATCH_BLOCK
    }
    catch (const TkUtil::Exception& exc)
    {
        errorString = exc.getFullMessage ( );
        hasError    = true;
        setErrorMessage (errorString);
        setStatus (Command::FAIL);
    }
    catch (const std::exception& stdExc)
    {
        errorString = stdExc.what ( );
        std::cout << "Erreur : " << errorString << std::endl;
        hasError    = true;
        setErrorMessage (errorString);
        setStatus (Command::FAIL);
    }
    catch (const Standard_Failure& exc)
    {
        std::cerr << "Erreur OCC : ";
        exc.Print(std::cerr);
        //std::cerr<<exc.GetMessageString();
        std::cerr<<std::endl;
        hasError    = true;
        setErrorMessage ("Erreur OCC");
        setStatus (Command::FAIL);
    }
    catch (...)
    {
		// On est ici car on ne maitrise pas le code à l'origine de l'erreur,
		// on sort anormalement d'une boite noire. La situation est peut être
		// critique (plus de mémoire par exemple).
        /* Ne pas toucher à errorString, éviter les appels libc++ */
        /* => Pas de setErrorMessage ici.                               */
        /* De même, pas de setStatus qui génère des logs, utilise des   */
        /* strings, des flux, ... Si pthread_cancel est appelé alors    */
        /* cancelledCleanup doit être appelé et lui positionne le       */
        /* à FAIL.                                                      */
        fprintf (stderr, "Erreur interne non documentée.\n");
        fprintf (stderr, "Thread peut être détruit par appel de ");
        fprintf (stderr, "pthread_cancel. En cas de plantage merci de ");
        fprintf (stderr, "remonter ce dysfonctionnement à l'équipe de ");
        fprintf (stderr, "développement et de préciser dans quelles ");
        fprintf (stderr, "conditions il s'est produit, notamment si ");
        fprintf (stderr, "vous avez tué une commande.");
        hasError    = true;
// CP : cancelledCleanup remis en service car l'expérience montre que ce
// catch peut servir sans être passé par pthread_cancel (dont l'usage un jour
// est très improbable). Cette méthode effectue (en consommant le moins possible
// de ressources système) :
// - Positionnement du status à FAIL
// - Invocation de la méthode emergencyCleanup qui peut être surchargée pour
// faire du netoyage.
		cancelledCleanup ( );
        throw;  /* Assure la compatibilité avec pthread_cancel. */
    }

//  std::cout << "Erreur : " << errorString << std::endl; [EB: supprimé de catch(TkUtil::Exception)]

    // fait le ménage si nécessaire
    postExecute(hasError);

    if (hasError){
        // retour en arrière pour les noms
        getContext().getNameManager().setInternalStats(m_name_manager_before);
    }
    else
        getContext().getNameManager().getInternalStats(m_name_manager_after);

    startingOrcompletionLog (false); // log de fin de commande

    return getStatus ( );
}
/*----------------------------------------------------------------------------*/
Utils::Command::status CommandInternal::undo ( )
{
    TkUtil::AutoMutex	autoMutex (getCommandMutex ( ));

    TkUtil::UTF8String   message1 (TkUtil::Charset::UTF_8), message2 (TkUtil::Charset::UTF_8);
    message1 << "Annulation de la commande " << getName ( ) << " de nom unique "
             << getUniqueName ( );
    message2    = message1;
    message1 << " en cours.";

    if (DONE != getStatus ( ))
    {
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message << "Réversion de la commande " << getName ( )
                << " impossible. Etat courant : "
                << statusToString (getStatus( )) << ".";
        throw TkUtil::Exception (message);
    }   // if (DONE != getStatus ( ))
    else
        log (TkUtil::TraceLog(message1, TkUtil::Log::TRACE_3));


    getTimer ( ).reset ( );

    // ce qui est propre à la commande
    internalUndo();

    // met à jour l'état de la visibilité des entités
    if (getContext().isGraphical())
        getContext().getLocalGroupManager().updateDisplayProperties(&getInfoCommand());

    // change le status et fait les callbacks
    setStatus (Command::INITED);

    message2 << " effectué.";
    log (TkUtil::TraceLog(message2, TkUtil::Log::TRACE_3));

    return getStatus ( );
}   // Command::undo
/*----------------------------------------------------------------------------*/
Utils::Command::status CommandInternal::redo ( )
{
    TkUtil::UTF8String   message1 (TkUtil::Charset::UTF_8), message2 (TkUtil::Charset::UTF_8);
    message1 << "Rejeu de la commande " << getName ( ) << " de nom unique "
            << getUniqueName ( );
    message2    = message1;
    message1 << " en cours.";
	getTimer ( ).start ( );

    if (Utils::Command::INITED != getStatus ( ))
        throw TkUtil::Exception (TkUtil::UTF8String ("Command::redo impossible, status différent du status initial.", TkUtil::Charset::UTF_8));
    else
        log (TkUtil::TraceLog(message1, TkUtil::Log::TRACE_3));

    // ce qui est propre à la commande
    internalRedo();

    if (Command::CANCELED != getStatus ( )){

        // met à jour l'état de la visibilité des entités
        if (getContext().isGraphical())
            getContext().getLocalGroupManager().updateDisplayProperties(&getInfoCommand());

        // change le status et fait les callbacks
        setStatus (Command::DONE);
    }

    message2 << " effectué.";
    log (TkUtil::TraceLog(message2, TkUtil::Log::TRACE_3));

    return getStatus ( );
} // Command::redo

/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
