/**
 * \file        src/Core/protected/Internal/ClientServerProperties.h
 * \author      Charles PIGNEROL
 * \date        03/04/2012
 *
 */

#ifndef CLIENT_SERVER_PROPERTIES_H
#define CLIENT_SERVER_PROPERTIES_H

#include <PrefsCore/Section.h>
#include <PrefsCore/BoolNamedValue.h>
#include <PrefsCore/StringNamedValue.h>
#include <PrefsCore/UnsignedLongNamedValue.h>

#include <string>
#include <vector>

namespace Mgx3D
{

/**
 * <P>Classe contenant les propriétés du mode client/serveur de Magix 3D.
 * </P>
 *
 * <P>Cette classe a vocation à avoir être utilisée via une instance unique, accessible via sa méthode
 * statique <I>getInstance ( )</I> et modifiable  via sa méthode statique <I>setInstance (ClientServerProperties*)</I>.
 * Ce dispositif non obligatoire permet de surcharger et/ou augmenter les services proposés.
 * </P>
 */
class ClientServerProperties
{
	public :

	/**
	 * Le mode de fonctionnement :<BR>
	 * - BUILTIN : fonctionnement mono-process (non C/S),<BR>
	 * - PARALLEL_CS : fonctionnement en mode C/S avec serveur sur partition parallèle (MPI),
	 */
	enum CS_POLICY { BUILTIN, PARALLEL_CS };

	/**
	 * <P>Constructeur par défaut.
	 * </P>
	 */
	ClientServerProperties ( );

	/**
	 * Constructeur de copie. RAS.
	 */
	ClientServerProperties (const ClientServerProperties&);

	/**
	 * Opérateur =. RAS.
	 */
	ClientServerProperties& operator = (const ClientServerProperties&);

	/**
	 * Destructeur. RAS.
	 */
	~ClientServerProperties ( );


	/**
	 * La gestion de l'éventuelle instance unique.
	 */
	//@{

	/**
	 * \return		L'éventuelle instance unique, affectée via <I>setInstance</I>.
	 * \exception	Une exception est levée en l'absence d'instance unique affectée.
	 * \see			setInstance
	 */
	static ClientServerProperties& getInstance ( );

	/**
	 * \param		L'éventuelle instance unique. Détruit l'existante s'il y en une.
	 * \see			getInstance
	 */
	static void setInstance (ClientServerProperties*);

	//@}
	
	/**
	 * Le mode de fonctionnement de l'application.
	 * /
	//@{

	/**
	 * \return	Le mode de fonctionnement de l'application.
	 */
	 virtual CS_POLICY getPolicy ( ) const;
	 
	 /**
	 * \param	Le mode de fonctionnement de l'application.
	 */
	 virtual void setPolicy (CS_POLICY policy);
	 
	//@}

	/**
	 * La gestion du serveur et des communications avec le serveur.
	 */
	//@{

	/**
	 * \return      Le shell de connection distante
	 * \see         setRemoteShell
	 */
	 virtual std::string getRemoteShell ( );

    /**
     * \param       Le shell de connection distante
     * \see         getRemoteShell
     */
    virtual void setRemoteShell (const std::string& shell);

    /**
     * \return      Le lanceur de processus
     * \see         setLauncher
     */
    virtual std::string getLauncher ( );

    /**
     * \param       Le lanceur de processus
     * \see         getLauncher
     */
    virtual void setLauncher (const std::string& launcher);

    /**
     * \return      Le chemin d'accès complet à l'exécutable du serveur.
     * \see         setServerPath
     */
     virtual std::string getServerPath ( );

    /**
     * \param      Le chemin d'accès complet à l'exécutable du serveur.
     * \see         getServerPath
     */
    virtual void setServerPath (const std::string& path);

	/**
	 * \return		Le nom ou adresse TCP/IP du serveur
	 * \see			setServerParameters
	 * \see			getListeningPort
	 * \see			getConnectionPort
	 * \see			getUsableInetAdress
	 */
	virtual std::string getServerHost ( );

	/**
	 * \return		L'adresse TCP/IP a priori utilisable du serveur
	 * \see			setServerParameters
	 * \see			getListeningPort
	 * \see			getConnectionPort
	 * \see			getServerHost
	 */
	virtual std::string getUsableInetAdress ( );

	/**
	 * \return		La partition de la machine serveur à utiliser.
	 */
	 virtual std::string getPartition ( );

	/**
	 * \return		La partition de la machine serveur à utiliser.
	 */
	virtual void setPartition (const std::string&);

	/**
	 * \return		Le port d'écoute du serveur utilisé pour les commandes Magix 3D (numéro de port TCP/IP ou
	 *				nom complet de fichier pour les sockets de domaine UNIX).
	 * \see			setServerParameters
	 * \see			getServerHost
	 * \see			getRenderingPort
	 */
	virtual std::string getPort ( );
	
	/**
	 * \return		Le port utilisé par le client pour se connecter au serveur pour les opérations de rendu graphique
	 * 				(numéro de port TCP/IP ou nom complet de fichier pour les sockets de domaine UNIX).
	 * \see			setServerParameters
	 * \see			getServerHost
	 * \see			getListeningPort
	 */
	virtual std::string getRenderingPort ( );

	/**
	 * Les paramètres de connexion au serveur. 
	 * \param		Nom ou adresse TCP/IP du serveur
	 * \param		Port d'écoute du serveur pour les commandes Magix 3D
	 * \param		Port d'écoute du serveur pour les opérations de rendu graphique
	 * \see			getServerHost
	 * \see			getPort
	 * \see			getRenderingPort
	 */
	virtual void setServerParameters (std::string host, std::string port, std::string renderingPort);

	//@}	// La gestion du serveur et des communications avec le serveur.

	/**
	 * La gestion des processus parallèles
	 */
	 //@{

    /**
     * @return      Le lanceur de processus parallèles (mpirun, mpiexec, ...)
     * @see         setMpiLauncher
     */
    virtual std::string getMpiLauncher ( );

    /**
     * @param      Le lanceur de processus parallèles (mpirun, mpiexec, ...)
     * @see         getMpiLauncher
     */
    virtual void setMpiLauncher (const std::string& launcher);

    //@}   // La gestion des processus parallèles

	/**
	 * Les données utilisateur.
	 */
	//@{

	/**
	 * \return		Le répertoire sur la machine serveur où aller chercher les données utilisateur.
	 */
	virtual std::string getUserDataDirectory ( );

	/**
	 * \return		Le répertoire sur la machine serveur où aller chercher les données utilisateur.
	 */
	virtual void setUserDataDirectory (const std::string&);


	//@}    // Les données utilisateur

	/**
	 * Les aspects graphiques.
	 */
	//@{

		/**
		 * \return	Les afficheurs utilisés par le serveur parallèle.
		 * \see		setDisplays
		 */
		static std::vector<std::string> getDisplays ( );

		/**
		 * \param	Les afficheurs à utiliser par le serveur parallèle.
		 * \see		getDisplays
		 */
		static void setDisplays (const std::vector<std::string>& displays);

	//@}	// Les aspects graphiques.


	/**
	 * La gestion des paramètres en configuration (préférences utilisateur).
	 */
	//@{

	/**
	 * Actualise les paramètres depuis la section transmise en argument. Un
	 * message est écrit sur la sortie standard en cas d'erreur rencontrée.
	 * \see			save
	 */
	virtual void load (Preferences::Section& s);

	/**
	 * Enregistre les paramètres dans la section transmise en argument.
	 * \see			load
	 */
	virtual void save (Preferences::Section& s);

	//@}	// La gestion des paramètres en configuration.


	private :

	/** L'éventuelle instance unique de la classe. */
	static ClientServerProperties*			_instance;
	
	/** Le mode de fonctionnement de l'application (BUILTIN par défaut). */
	CS_POLICY								_policy;

	/** Le shell de connection distante. */
    Preferences::StringNamedValue			_remoteShell;

    /** Le lanceur de processus. */
    Preferences::StringNamedValue			_launcher;

    /** Le chemin d'accès complet à l'exécutable serveur. */
    Preferences::StringNamedValue			_serverPath;

	/** Le nom ou l'adresse TCP/IP de la machine hébergeant le serveur. */
	Preferences::StringNamedValue			_serverHostName;

    /** Le path complet du lanceur parallèle (mpirun, mpiexec, ...). */
    Preferences::StringNamedValue			_mpiLauncher;

	/** Le nom de la partition à utiliser (standard, hybride, large, ...). */
	Preferences::StringNamedValue			_partitionName;

	/** Le port utiliser par le client pour se connecter au serveur, et le port d'écoute du serveur. */
	Preferences::StringNamedValue			_port, _renderingPort;

	/** Le répertoire par défaut contenant les données utilisateur. */
	Preferences::StringNamedValue			_userDataDirectory;

	/** La liste des afficheurs utilisés. */
	static std::vector<std::string>			_displays;
};	// class ClientServerProperties


}	// namespace Mgx3D

#endif	// CLIENT_SERVER_PROPERTIES_H
