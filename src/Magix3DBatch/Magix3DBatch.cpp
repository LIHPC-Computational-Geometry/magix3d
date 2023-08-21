/**
 * \file	Magix3DBatch.cpp
 * \author	Charles PIGNEROL
 * \date	31/01/2019
 * Point d'entrée dde l'application Magix 3D en mode batch (parallèle ou non).
 */
#include "QtVtkComponents/VTKMgx3DSelectionManager.h"
#include "Internal/Mgx3DArguments.h"
#include "Internal/ClientServerProperties.h"
#include "VtkComponents/vtkDMAMultiProcessStream.h"
#include <VtkComponents/vtkUnixServerSocket.h>
#include "Utils/CommandManager.h"
#include "Utils/Log.h"

#include <TkUtil/ArgumentsMap.h>
#include <TkUtil/Exception.h>
#include <TkUtil/File.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NetworkData.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/Process.h>
#include <TkUtil/ThreadManager.h>
#include <TkUtil/ThreadPool.h>


#include <assert.h>
#include <memory>
#include <vector>

#include <vtkParallelRenderManager.h>	// RENDER_RMI_TAG
#include <vtkGraphicsFactory.h>
#include <vtkOpenGLRenderWindow.h>

#include <stdlib.h>
#include <unistd.h>	// getopt, getpid


using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtVtkComponents;
using namespace TkUtil;
using namespace std;



static bool					standalone			= false;
static bool					isMaster			= true;	// standalone ou processus maître
static unsigned long		nbProcs				= 1;
static int					procNum				= 0;
// Les scripts python Magix 3D à exécuter :
static vector<string>		scripts;


//-----------------------------------------------------------------------------
int parseArgs (int argc, char *argv[])
{
	if (1 >= argc)
		return -1;
		
	// Recherche des scripts python à exécuter.
	// C'est le fichier py en fin de ligne de commande, ou, à défaut, ceux qui suivent l'option -script :
		File	last (argv [argc - 1]);
	if ((string ("--script") != argv [argc -2]) && (string ("-script") != argv [argc -2]) && (last.getExtension ( ) == "py"))
	{
		scripts.push_back (last.getFullFileName ( ));
	}	// if (last.getExtension ( ) == "py")
	else
	{
		for (size_t s = argc - 1; s > 0; s--)
		{
			const string	arg (argv [s]);
			File	file (arg);
			if (file.getExtension ( ) != "py")
				continue;

			scripts.insert (scripts.begin ( ), file.getFullFileName ( ));
		}	// for (size_t s = argc - 1; s > 0; s--)
	}	// else if (last.getExtension ( ) != "py")
	
	// Il faut au moins un script :
	if (0 == scripts.size ( ))
	{
		cerr << "Absence de script python à exécuter." << endl;
		return -1;
	}	// if (0 == scripts.size ( ))
	
	// On vérifie l'utilisabilité des scripts soumis :
	for (vector<string>::const_iterator its = scripts.begin ( ); scripts.end ( ) != its; its++)
	{
		const string	arg (*its);
		File	file (arg);

		if ((false == file.exists ( )) || (false == file.isReadable ( )))
			cerr << "ATTENTION : le fichier " << file.getFullFileName ( ) << " n'existe pas ou n'est pas accessible en lecture." << endl;
		if (false == file.isExecutable ( ))
			cerr << "ATTENTION : le fichier " << file.getFullFileName ( ) << " n'est pas exécutable." << endl;
	}	// for (vector<string>::const_iterator its = scripts.begin ( ); scripts.end ( ) != its; its++)
	
	return 0;
}	// parseArgs


int syntax (const string& pgm)
{
	cerr << "Syntaxe :" << endl
	     << pgm << "[-script s1.py][-script s2.py] script.py" << endl
	     << "OU" << endl
	     << pgm << "[--nbprocs nb] script.py" << endl
	     << "--script s.py .................. : script à exécuter (usage multiple possible). Peut ne pas être précédé de --script mais doit alors être le dernier argument de ligne de commande." << endl
	     << endl;

	return 1;
}	// syntax


//-----------------------------------------------------------------------------
int main (int argc, char *argv[], char* envp[])
{
	try
	{
		
	ClientServerProperties::setInstance (new ClientServerProperties ( ));	// Requis
	Process::initialize (argc, argv, envp);
	
    // logues version ~ Magix
    Utils::maillage_log(argc, argv, "Magix3DBatch");

	try
	{
		ThreadManager::instance ( );	// Déjà initialisé par le code de projection
	}
	catch (...)
	{
		ThreadManager::initialize (0);
		ThreadPool::initialize (0);
	}
	
	const char*	jobid	= getenv ("SLURM_JOB_ID");
	if (0 != jobid)
	{
		cout << "Context d'exécution parallèle détecté (SLURM_JOB_ID = " << jobid << ")" << endl;
		ClientServerProperties::getInstance ( ).setPolicy (ClientServerProperties::PARALLEL_CS);
	}	// if (0 != jobid)
	
	vector<string>	allowedArgs	= mgx3dAllowedArgs ( );
	Context::getArguments ( ).allowsArgs (allowedArgs);
	Context::initialize (argc, argv);	// Récupère les arguments de ligne de commande
	if (0 != parseArgs (argc, argv))	// Transfert les arguments spécifiques au serveur aux variables intéressées
		return syntax (argv [0]);

	// Durée à partir de laquelle il est décidé d'exécuter une commande dans un thread :
	CommandManager::sequentialDuration	= 5;
	
	const string	sessionName ("session_1");
	Context*		context	= 0;
	if (ClientServerProperties::BUILTIN == ClientServerProperties::getInstance ( ).getPolicy ( ))
	{
		VTKRenderingManager::initialize (false);	// !compositing
		context	= new Internal::Context (sessionName, true);
		context->setGraphical (false);
		context->setExeName (argv[0]);
	}	// if (ClientServerProperties::BUILTIN == ClientServerProperties::getInstance ( ).getPolicy ( ))
	CHECK_NULL_PTR_ERROR (context)
	
	// fork, et ce service repose sur les classes
	// PythonSession et DuplicatedStream (au 26/07/12).
	// La classe DuplicatedStream repose sur fork, et il arrive que le serveur,
	// lancé sur station et avec mpich 1.2.6, reste en attente sur un waitpid
	// du processus fils chargé de rediriger les sorties du script.
	// Donc, au moins pour les serveurs, on affecte false à cette variable.
	context->displayScriptOutputs.setValue (false);
	
	vtkGraphicsFactory::SetOffScreenOnlyMode (1);	// Pas de main loop
	
	// Exécution des scripts transmis en arguments (pour processus maîtres uniquement) :
	if (true == isMaster)
	{
		for (vector<string>::const_iterator its = scripts.begin ( ); scripts.end ( ) != its; its++)
		{
cout << "PROCESSING SCRIPT " << *its << " on processor " << procNum << " ..." << endl;
                context->getPythonSession ( ).execFile (*its, false);
cout << "SCRIPT " << *its << " PROCESSED on processor " << procNum << "." << endl;
		}	// for (vector<string>::const_iterator its = scripts.begin ( ); scripts.end ( ) != its; its++)
	}	// if (true == isMaster)

	}	// try
	catch (const Exception& exc)
	{
		cerr << "Erreur fatale durant l'exécution de Magix3DBatch : " <<endl
		     << exc.getFullMessage ( ) << endl;
		return -1;

	}
	catch (const exception& e)
	{
		cerr << "Erreur fatale durant l'exécution de Magix3DBatch : " <<endl
		     << e.what ( ) << endl;
		return -1;
	}
	catch (...)
	{
		cerr << "Erreur fatale durant l'exécution de Magix3DBatch : " <<endl
		     << "Erreur non documentée." << endl;
		return -1;
	}

	cout << "Magix3DBatch completly completed on processor " << procNum << "." << endl;
	return 0;
}	// main


