/**
 * \file		src/Core/protected/Internal/ClientServerProperties.cpp
 * \author		Charles PIGNEROL
 * \date		03/04/2012
 *
 */


#include "Internal/ClientServerProperties.h"

#include <TkUtil/Exception.h>
#include <TkUtil/NetworkData.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/RemoteProcess.h>
#include <PrefsCore/PreferencesHelper.h>

#include <limits>	// unsigned short numeric_limits<unsigned short>::max
#include <string.h>
#include <ctype.h>


using namespace std;
using namespace Mgx3D;
using namespace TkUtil;
using namespace Preferences;


namespace Mgx3D
{

ClientServerProperties*	ClientServerProperties::_instance		= 0;
vector<std::string>		ClientServerProperties::_displays;


ClientServerProperties::ClientServerProperties ( )
	: _policy (ClientServerProperties::BUILTIN),
	  _remoteShell ("remoteShell", "ssh", "Shell pour accéder à une mhine distante (ex : ssh)."),
	  _launcher ("launcher", "tkutil_launcher", "Lanceur d'applications (ex : tkutil_launcher, TkUtilLauncher)."),
	  _serverPath ("path", "/opt/Magix3D/bin/CorbaMpiMagix3DServer", "Chemin d'accès complet à l'exécutable serveur."),
	  _serverHostName ("host", "localhost", "Nom ou adresse TCP/IP de la machine serveur."),
	  _mpiLauncher ("launcher", "mpirun", "Path du lanceur parallèle MPI (mpirun, mpiexec, ...)"),
	  _partitionName ("partitionName", "", "Partition de la machine serveur à utiliser."),
	  _port ("port", "/tmp/magix3d.commands.socket", "Port de communications utilisé par client et serveur pour les commandes Magix 3D."),
	  _renderingPort ("renderingPort", "/tmp/magix3d.rendering.socket", "Port de communications utilisé par client et serveur pour les opérations de rendu graphique."),
	  _userDataDirectory ("userDataDirectory", "", "Répertoire sur la machine serveur proposé par défaut pour charger les données utilisateurs.")
{
}	// ClientServerProperties::ClientServerProperties


ClientServerProperties::ClientServerProperties (const ClientServerProperties& csp)
	: _policy (csp._policy), _remoteShell (csp._remoteShell), _launcher (csp._launcher), _serverPath (csp._serverPath),
	  _serverHostName (csp._serverHostName), _mpiLauncher (csp._mpiLauncher), _partitionName (csp._partitionName),
	  _port (csp._port), _renderingPort (csp._renderingPort), _userDataDirectory (csp._userDataDirectory)
{
}	// ClientServerProperties::ClientServerProperties


ClientServerProperties& ClientServerProperties::operator = (const ClientServerProperties& csp)
{
	if (&csp != this)
	{
		_policy					= csp._policy;
        _remoteShell            = csp._remoteShell;
        _launcher               = csp._launcher;
        _serverPath             = csp._serverPath;
		_serverHostName			= csp._serverHostName;
		_mpiLauncher            = csp._mpiLauncher;
		_partitionName          = csp._partitionName;
		_port					= csp._port;
		_renderingPort			= csp._renderingPort;
		_userDataDirectory      = csp._userDataDirectory;
	}	// if (&csp != this)

	return *this;
}	// ClientServerProperties::operator =


ClientServerProperties::~ClientServerProperties ( )
{
}	// ClientServerProperties::~ClientServerProperties


ClientServerProperties& ClientServerProperties::getInstance ( )
{
	if (0 == _instance)
		throw Exception ("ClientServerProperties::getInstance : absence d'instance affectée.");

	return *_instance;
}	// ClientServerProperties::getInstance


void ClientServerProperties::setInstance (ClientServerProperties* csp)
{
	if (_instance == csp)
		return;

	delete _instance;
	_instance	= csp;
}	// ClientServerProperties::setInstance


ClientServerProperties::CS_POLICY ClientServerProperties::getPolicy ( ) const
{
	return _policy;
}	// ClientServerProperties::getPolicy


void ClientServerProperties::setPolicy (ClientServerProperties::CS_POLICY policy)
{
	_policy	= policy;
}	// ClientServerProperties::setPolicy


string ClientServerProperties::getRemoteShell ( )
{
    return _remoteShell.getValue ( );
}   // ClientServerProperties::getRemoteShell


void ClientServerProperties::setRemoteShell (const string& remoteShell)
{
    _remoteShell.setStrValue (remoteShell);
    RemoteProcess::remoteShell  = remoteShell;
}   // ClientServerProperties::getRemoteShell


string ClientServerProperties::getLauncher ( )
{
    return _launcher.getValue ( );
}   // ClientServerProperties::getLauncher


void ClientServerProperties::setLauncher (const string& launcher)
{
    _launcher.setStrValue (launcher);
    RemoteProcess::launcher = launcher;
}   // ClientServerProperties::getLauncher


string ClientServerProperties::getServerPath ( )
{
    return _serverPath.getValue ( );
}   // ClientServerProperties::getServerPath


void ClientServerProperties::setServerPath (const string& path)
{
    _serverPath.setStrValue (path);
}   // ClientServerProperties::setServerPath

    
string ClientServerProperties::getServerHost ( )
{
	return _serverHostName.getStrValue ( );
}	// ClientServerProperties::getServerHost


string ClientServerProperties::getUsableInetAdress ( )
{
	try
	{
		const string	serverDomain ("133");
		// NetworkData::getUsableInetAddress est très lent, on optimise :
		if (0 == strncmp (
			serverDomain.c_str( ), _serverHostName.getStrValue( ).c_str( ), 3))
			return _serverHostName.getStrValue( );
		return NetworkData::getUsableInetAddress (
							_serverHostName.getStrValue ( ), serverDomain);
	}
	catch (...)
	{	// Seul un port de la machine serveur peut être ouvert
		// => impossibilité d'avoir des infos sur cette machine
		// => En cas d'échec on renvoit la seule valeur que l'on a, à savoir
		// _serverHostName.
		cout << endl << endl
		     << "Impossibilité d'avoir des informations précises sur la "
		     << "machine " << _serverHostName.getStrValue ( )
		     << ", et notamment son adresse sur le réseau " << string ("133")
		     << ". Utilisation du nom " << _serverHostName.getStrValue ( )
		     << " pour se connecter au serveur ..." << endl << endl;
		return _serverHostName.getStrValue ( );
	}
}	// ClientServerProperties::getUsableInetAdress


string ClientServerProperties::getPartition ( )
{
	return _partitionName.getValue ( );
}   // ClientServerProperties::getPartition


void ClientServerProperties::setPartition (const string& partition)
{
	_partitionName.setStrValue (partition);
}   // ClientServerProperties::getPartition


string ClientServerProperties::getPort ( )
{
	return _port.getValue ( );
}	// ClientServerProperties::getPort


string ClientServerProperties::getRenderingPort ( )
{
	return _renderingPort.getValue ( );
}	// ClientServerProperties::getRenderingPort


void ClientServerProperties::setServerParameters (string host, string port, string renderingPort)
{
	// TODO [CP] vérifier l'existence de host si non vide
	_serverHostName.setStrValue (host);
	_port.setValue (port);
	_renderingPort.setValue (renderingPort);
}	// ClientServerProperties::setServerParameters


string ClientServerProperties::getMpiLauncher ( )
{
    return _mpiLauncher.getValue ( );
}	// ClientServerProperties::getMpiLauncher


void ClientServerProperties::setMpiLauncher (const std::string& launcher)
{
    _mpiLauncher.setStrValue (launcher);
}   // ClientServerProperties::setMpiLauncher


string ClientServerProperties::getUserDataDirectory ( )
{
	return _userDataDirectory.getValue ( );
}   // ClientServerProperties::getUserDataDirectory


void ClientServerProperties::setUserDataDirectory (const string& directory)
{
	_userDataDirectory.setStrValue (directory);
}   // ClientServerProperties::getUserDataDirectory


vector<string> ClientServerProperties::getDisplays ( )
{
	return _displays;
}	// ClientServerProperties::getDisplays


void ClientServerProperties::setDisplays (const vector<string>& displays)
{
	_displays	= displays;
//	cout << "DISPLAYS ARE : ";
//	for (vector<string>::const_iterator it = _displays.begin ( ); _displays.end ( ) != it; it++)
//	    cout << *it << ' ' ;
//	cout << endl;
}	// ClientServerProperties::setDisplays


#define LOADING_TRY_BLOCK                                                    \
	try                                                                      \
	{

#define LOADING_CATCH_BLOCK(r)                                               \
	}                                                                        \
	catch (const Exception& e)                                               \
	{                                                                        \
		cout << "Erreur lors de la lecture de la ressource "                 \
	         << r.getName ( ) << " : " << e.getFullMessage ( ) << endl;      \
	}                                                                        \
	catch (...)                                                              \
	{                                                                        \
		cout << "Erreur non documentée lors de la lecture de la ressource "  \
		     << r.getName ( ) << "." << endl;                                \
	}


#define SAVING_TRY_BLOCK                                                    \
	try                                                                      \
	{

#define SAVING_CATCH_BLOCK(r)                                               \
	}                                                                        \
	catch (const Exception& e)                                               \
	{                                                                        \
		cout << "Erreur lors de l'enregistrement de la ressource "                 \
	         << r.getName ( ) << " : " << e.getFullMessage ( ) << endl;      \
	}                                                                        \
	catch (...)                                                              \
	{                                                                        \
		cout << "Erreur non documentée lors de l'enregistrement de la ressource "  \
		     << r.getName ( ) << "." << endl;                                \
	}

void ClientServerProperties::load (Section& s)
{
    LOADING_TRY_BLOCK
        PreferencesHelper::getString (s, _serverPath);
    LOADING_CATCH_BLOCK (_serverPath)

	LOADING_TRY_BLOCK
		PreferencesHelper::getString (s, _serverHostName);
	LOADING_CATCH_BLOCK (_serverHostName)

	LOADING_TRY_BLOCK
		PreferencesHelper::getString (s, _partitionName);
	LOADING_CATCH_BLOCK (_partitionName)

	LOADING_TRY_BLOCK
		PreferencesHelper::getString (s, _port);
	LOADING_CATCH_BLOCK (_port)

	LOADING_TRY_BLOCK
		PreferencesHelper::getString (s, _renderingPort);
	LOADING_CATCH_BLOCK (_renderingPort)

	LOADING_TRY_BLOCK
		PreferencesHelper::getString (s, _userDataDirectory);
	LOADING_CATCH_BLOCK (_userDataDirectory)

    Section&    remoteProcessSection  = PreferencesHelper::getSection (s, "remoteProcess");
    LOADING_TRY_BLOCK
        PreferencesHelper::getString (remoteProcessSection, _remoteShell);
        RemoteProcess::remoteShell  = _remoteShell.getStrValue ( );
    LOADING_CATCH_BLOCK (_remoteShell)

    LOADING_TRY_BLOCK
        PreferencesHelper::getString (remoteProcessSection, _launcher);
        RemoteProcess::launcher = _launcher.getStrValue ( );
    LOADING_CATCH_BLOCK (_launcher)

	Section&    mpiSection  = PreferencesHelper::getSection (s, "mpi");
    LOADING_TRY_BLOCK
        PreferencesHelper::getString (mpiSection, _mpiLauncher);
    LOADING_CATCH_BLOCK (_userDataDirectory)
}	// ClientServerProperties::load


void ClientServerProperties::save (Section& s)
{
    Section&    remoteProcessSection  = PreferencesHelper::getSection (s, "remoteProcess");
    PreferencesHelper::updateString (remoteProcessSection, _remoteShell);
    PreferencesHelper::updateString (remoteProcessSection, _launcher);
    PreferencesHelper::updateString (s, _serverPath);
	PreferencesHelper::updateString (s, _serverHostName);
	PreferencesHelper::updateString (s, _partitionName);
	PreferencesHelper::updateString (s, _port);
	PreferencesHelper::updateString (s, _renderingPort);
	PreferencesHelper::updateString (s, _userDataDirectory);
    Section&    mpiSection  = PreferencesHelper::getSection (s, "mpi");
    PreferencesHelper::updateString(mpiSection, _mpiLauncher);
}	// ClientServerProperties::save

}	//namespace Mgx3D
