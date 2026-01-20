#ifndef CONTEXT_H_
#define CONTEXT_H_
/*----------------------------------------------------------------------------*/
#include "Internal/M3DCommandManager.h"
#include "Internal/NameManager.h"
#include "Internal/ScriptingManager.h"

#include "SelectionManager.h"
#include "Utils/Unit.h"
#include "Utils/Entity.h"
#include "Utils/Landmark.h"
#include "Utils/SwigCompletion.h"

#include "Geom/GeomManager.h"
#include "Topo/TopoManager.h"
#include "Mesh/MeshManager.h"
#include "SysCoord/SysCoordManager.h"
#include "Structured/StructuredMeshManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/ArgumentsMap.h>
#include <TkUtil/PaintAttributes.h>
#include <TkUtil/LogDispatcher.h>
#include <TkUtil/OstreamLogOutputStream.h>
#include <TkUtil/Mutex.h>
#include <PrefsCore/Section.h>
#include <PythonUtil/PythonSession.h>
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
namespace Internal{
class M3DCommandResult;
}
namespace Utils {
class CommandManager;
class Property;
class DisplayProperties;
}
namespace Mesh {
class CommandChangeMeshDim;
class MeshImplementation;
}
namespace Group {
	class GroupManager;
}
typedef void (*fSignalFunc)(int);

/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Internal
 *
 * \brief Espace de nom des classes communes au projet de haut niveau
 *
 *
 */
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class Context
 *  \brief Un contexte correspond à l'exécution d'une séance de travail où
 *         toutes les opérations de Magix 3D peuvent être effectuées et aussi
 *         défaites.
 *
 *         A un contexte est associé un certain de nombre de managers permettant
 *         la création d'entités propres à la session de travail. En particulier,
 *         si deux contextes de travail sont ouvertes simultanément, les
 *         opérations de undo/redo sont spécifiques à chaque contexte.
 * 
 *         Un contexte peut avoir des entités sélectionnées, sur lesquelles
 *         seront effectués des traitements. Chaque contexte est doté à cet
 *         effet d'un gestionnaire de sélection qui lui est spécifique.
 *
 *         Pour les pythoneries, on accède à un contexte via la fonction
 *         <I>Mgx3D.getContext</I> qui retourne un contexte existant en
 *         fonction du nom tranmis en argument, le créé si ce n'est pas le cas,
 *         ou via appel du constructeur
 *         par <I>Mgx3D.Context("nom_unique")</I>.
 *
 * \see		getContext
 */
/*----------------------------------------------------------------------------*/
class Context {

	friend class CommandChangeLengthUnit;
	friend class CommandChangeLandmark;
	friend class Mesh::CommandChangeMeshDim;
	friend class Mesh::MeshImplementation;

public:
	/*------------------------------------------------------------------------*/
	/** Suivant ce que l'on souhaite voir apparaitre dans le script de sauvegarde
	 */
	enum encodageScripts {
		WITHNAMES=0,
		WITHIDREF=1,
		WITHCOORD=2
	};

	/*------------------------------------------------------------------------*/
	/** Possibilité de spécifier que le maillage est 2D
	 */
	enum meshDim {
		MESH2D=2,
		MESH3D=3
	};

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur. S'enregistre auprès d'une liste de contextes
	 *			existants et accessibles via la fonction <I>getContext</I>.
	 * \param	name Nom unique de ce contexte
	 * \param   withStdOutputs Utilise les flux standards (à éviter depuis l'IHM, car il y a
	 * des conflits avec les sorties de commandes python)
	 * \see		getContext
     */
    Context(const std::string& name, bool withStdOutputs = true);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur. Se désenregistre auprès de la liste de contextes
	 *			existants et accessibles via la fonction <I>getContext</I>.
	 * \see		getContext
     */
    ~Context();
#endif

    /** \brief	Initialisation de l'API.
     *  \param	argv argc et argv du main. Utilisation des éventuels arguments :<BR>
     *		-outCharsetRef àéèùô : détection automatique du jeu de caractères de
     *		la sortie standard (API TkUtil::Locale)<BR>
     *		-outCharset suivi de UTF-8, UTF-16, ISO-8859 ou ASCII pour imposer le jeu
     *          de caractères de la sortie standard.
     * Détection de l'encodage de la sortie standard.
     */
    static void initialize (int argc, char* argv []);

    /** \brief	Finalisation de l'API.
     */
    static void finalize ( );

    /**
     * \return	Un accès aux arguments transmis à la ligne de commande.
     */
    static TkUtil::ArgumentsMap& getArguments ( );

    /** \return	Le jeu de caractères utilisé par la sortie standard.
      * \see	initialize
      */
    static const TkUtil::Charset& getOutCharset ( );

#ifndef SWIG
    /** Booléen qui permet de savoir si l'on est en train de quitter la session ou non */
    bool isFinished() {return m_is_finish;}
    void setFinished(bool b) {m_is_finish = b;}


	/*------------------------------------------------------------------------*/
	/** \param  name Le nouveau nom unique de l'instance.
	 */
	void setName (const std::string& name) {m_name=name;}

	/*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur la session de scripting <I>Python</I>.
	 * \exception	Une exception est levée en l'absence de session associée.
     */
    TkUtil::PythonSession& getPythonSession();
    void setPythonSession (TkUtil::PythonSession*);

	/*------------------------------------------------------------------------*/
	/** \brief  Accesseur sur le manager de créations de niveau supérieur (compositions)
	 */
	Mgx3D::Internal::M3DCommandManager& getM3DCommandManager() {return m_m3d_command_manager;}

	/*------------------------------------------------------------------------*/
	/** \brief  Accesseur sur le manager de commandes
	 */
	Mgx3D::Utils::CommandManager& getCommandManager();
	void setCommandManager (Mgx3D::Utils::CommandManager* mgr);
	Mgx3D::Utils::CommandManager& getLocalCommandManager();

	/*------------------------------------------------------------------------*/
	/** Le thread principal de l'application. */
	static pthread_t threadId;

	/*------------------------------------------------------------------------*/
	/** \brief  Accesseur sur le gestionnaire de sélection.
	 */
	Mgx3D::Internal::SelectionManager& getSelectionManager ( );
	const Mgx3D::Internal::SelectionManager& getSelectionManager ( ) const;
	void setSelectionManager (Mgx3D::Internal::SelectionManager*);
	/*------------------------------------------------------------------------*/

	/**
	 * La gestion de l'affichage des informations relatives au déroulement des
	 * sessions.
	 */
	//@{
	/*------------------------------------------------------------------------*/
	/** \brief  Accesseur sur le gestionnaire de flux.
	 */
	TkUtil::LogDispatcher& getLogDispatcher ( ) {return m_log_dispatcher;}
	const TkUtil::LogDispatcher& getLogDispatcher ( ) const {return m_log_dispatcher;}
	TkUtil::LogOutputStream* getLogStream ( ) {return &m_log_dispatcher; }

	/** \brief	Accesseur sur les flux de logs sur sortie standard et sortie
	 *			en erreur.
	 */
	TkUtil::OstreamLogOutputStream& getStdLogStream ( );
	TkUtil::OstreamLogOutputStream& getErrLogStream ( );

	/**
	 * \return		Le masque à appliquer par défaut aux flux sortants de logs.
	 */
	static TkUtil::Log::TYPE getLogsMask ( );

	/**
	 * \param		mask Nouveau masque à appliquer par défaut aux flux sortants de
	 *				logs.
	 */
	static void setLogsMask (TkUtil::Log::TYPE mask);
	//@}	// gestion des logs

	/*------------------------------------------------------------------------*/
	/**
	* \brief  Accesseur sur le gestionnaire d'écrivain de scripts
	*/
	Internal::ScriptingManager& getScriptingManager();

	/*------------------------------------------------------------------------*/
	/**
	 * \param	manager Nouveau gestionnaire d'écrivain de scripts (adoption).
	 */
	void setScriptingManager (Internal::ScriptingManager* manager);

	/*------------------------------------------------------------------------*/
	/**
	 * Choix d'un nom de fichier pour stockage des commandes Python
	 */
	std::string newScriptingFileName();

	/*------------------------------------------------------------------------*/
	/**
	 *  Retourne vrai si le contexte est graphique
	 *  c'est à dire s'il existe une représentation des entités
	 */
	bool isGraphical() const
	{return m_is_graphical;}

	/// modifie le contexte (avec affichage graphique ou non)
	void setGraphical(bool gr)
	{m_is_graphical = gr;}

	/**
	 * \return  La couleur de fond du système graphique.
	 */
	TkUtil::Color getBackground ( ) const;

	/**
	 * \param   bg La nouvelle couleur de fond du système graphique.
	 */
	void setBackground (const TkUtil::Color& bg);

	/**
	 * \return  Le ratio de dégradation pour l'affichage du maillage en mode dégradé
	 */
	int getRatioDegrad() {return m_ratio_degrad;}

	/**
	 * Modifie le ratio de dégradation pour l'affichage du maillage en mode dégradé
	 */
	void setRatioDegrad(int ratio) {m_ratio_degrad = ratio;}

	/*------------------------------------------------------------------------*/
	/** \brief  Adapte les Managers pour le cas d'une importation de script.
	 *
	 * Il sera fait un décalage sur les Id des noms des entités
	 */
	void beginImportScript();

	/** \brief  Termine l'adaptation des Managers pour le cas d'une importation de script.
	 */
	void endImportScript();
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager géométrique
     */

    Mgx3D::Geom::GeomManager& getGeomManager();
    const Mgx3D::Geom::GeomManager& getGeomManager() const;
#ifndef SWIG
	void setGeomManager (Mgx3D::Geom::GeomManager* mgr);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager topologique
     */
    Mgx3D::Topo::TopoManager& getTopoManager();
    const Mgx3D::Topo::TopoManager& getTopoManager() const;
#ifndef SWIG
	void setTopoManager (Mgx3D::Topo::TopoManager* mgr);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de maillage
     */
    Mgx3D::Mesh::MeshManager& getMeshManager();
    const Mgx3D::Mesh::MeshManager& getMeshManager() const;
#ifndef SWIG
	void setMeshManager (Mgx3D::Mesh::MeshManager* mgr);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de groupes
     */
    Mgx3D::Group::GroupManager& getGroupManager();
    const Mgx3D::Group::GroupManager& getGroupManager() const;
#ifndef SWIG
    void setGroupManager (Mgx3D::Group::GroupManager* mgr);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de repères
     */
    Mgx3D::CoordinateSystem::SysCoordManager& getSysCoordManager();
    const Mgx3D::CoordinateSystem::SysCoordManager& getSysCoordManager() const;
#ifndef SWIG
    void setSysCoordManager (Mgx3D::CoordinateSystem::SysCoordManager* mgr);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de maillages structurés
     */
    Mgx3D::Structured::StructuredMeshManager& getStructuredMeshManager();
    const Mgx3D::Structured::StructuredMeshManager& getStructuredMeshManager() const;
#ifndef SWIG
    void setStructuredMeshManager (Mgx3D::Structured::StructuredMeshManager* mgr);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de noms
     */
#ifndef SWIG
    Mgx3D::Internal::NameManager& getNameManager() {return *m_name_manager;}
    const Mgx3D::Internal::NameManager& getNameManager() const {return *m_name_manager;}
#endif


	/*------------------------------------------------------------------------*/
    /**
     * Sauvegarde dans un fichier les commandes Python actuellement
     * dans le commandManager (les undo n'apparaissent pas)
     * \param fileName nom du fichier pour la sauvegarde
     * \param enc le type d'information dans le script (nom, référence / commandes, coordonnées)
     */
    void savePythonScript (std::string fileName, encodageScripts enc, TkUtil::Charset::CHARSET charset);

    /*------------------------------------------------------------------------*/
    /// active le décalage des id pour renommer les entités
    void activateShiftingNameId();

    /// désactive le décalage des id pour renommer les entités
    void unactivateShiftingNameId();


#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /**
     * Attribution d'un nouveau id unique par entité, incrémentation automatique
     */
    unsigned long newUniqueId();

	/**
      * Attribution d'un nouveau id unique par entité, sans incrémentation
      */
    unsigned long nextUniqueId();

    /*------------------------------------------------------------------------*/
    /**
     * Attribution d'un nouveau id unique par explorateur
     */
    unsigned long newExplorerId();

    /*------------------------------------------------------------------------*/
    /**
    * Allocation d'une nouvelle propriété pour une entité (le nom)
    * \param ot le type d'objet
    */
    Utils::Property* newProperty(const Utils::Entity::objectType& ot);

    /*------------------------------------------------------------------------*/
    /**
    * Allocation d'une nouvelle propriété pour une entité avec un nom d'imposé
    * \param ot le type d'objet
    * \param name le nom de l'objet
    */
    Utils::Property* newProperty(const Utils::Entity::objectType& ot, const std::string& name);

    /*------------------------------------------------------------------------*/
    /**
    * \return	Les propriétés d'affichage globale pour une entité (la couleur)
    * \param	ot le type d'objet
    */
    Utils::DisplayProperties& globalDisplayProperties (
										Utils::Entity::objectType ot);
    const Utils::DisplayProperties& globalDisplayProperties (
										Utils::Entity::objectType ot) const;

    /*------------------------------------------------------------------------*/
    /**
    * \return	Le masque d'affichage global pour une entité (filaire, surfacique, ...)
    * \param	ot le type d'objet
    */
    unsigned long& globalMask (Utils::Entity::objectType ot);
    unsigned long globalMask (Utils::Entity::objectType ot) const;

    /*------------------------------------------------------------------------*/
    /**
    * \return	Le masque d'affichage global sauvegardé pour une entité (filaire, surfacique, ...)
    * \param	ot le type d'objet
    * \see		restoreGlobalMaskWireProperties
    * \see		restoreGlobalMaskSolidProperties
    */
    unsigned long& savedGlobalMask (Utils::Entity::objectType ot);
    unsigned long savedGlobalMask (Utils::Entity::objectType ot) const;

    /*------------------------------------------------------------------------*/
    /**
    * Restaure les propriétés filaires/solides au masque pour le type d'entité transmis en argument.
    * \param	ot le type d'objet
    */
    void restoreGlobalMaskWireProperties (Utils::Entity::objectType ot);
    void restoreGlobalMaskSolidProperties (Utils::Entity::objectType ot);

    /*------------------------------------------------------------------------*/
    /**
    * Allocation d'une nouvelle propriété d'affichage pour une entité (la couleur)
    * \param ot le type d'objet
    */
    Utils::DisplayProperties* newDisplayProperties(const Utils::Entity::objectType& ot);
	/*------------------------------------------------------------------------*/
	/**
	* Affectation d'une nouvelle représentation graphique pour une entité
	* SI l'entité est affichable
	* (<I>true == getDisplayProperties ( ).isDisplayable</I>.
	*/
	void newGraphicalRepresentation(Utils::Entity& entity);

	/**
	 * Recherche une couleur en configuration pour une entité. Une entité pouvant appartenir à plusieurs
	 * groupes c'est ici la liste des groupes qui est transmise en argument.
	 * \param	Groupes dont on veut récupérer une éventuelle couleur définie en configuration. 
	 *			La première couleur trouvée en configuration est retenue.
	 * \param	En retour, la première couleur trouvée en configuration, s'il y en a une.
	 * \return	true si une couleur est trouvée, false dans le cas contraire.
	 * \see		loadGroupsColors
	 */
	 bool getGroupColor (const std::vector<Group::GroupEntity*>& groups, TkUtil::Color& color);
#endif

    /** Passage en mode apperçu ou non. En mode apperçu les couleurs fournies
     * par newDisplayProperties n'interviennent pas dans l'algorithme d'attribution
     * des couleurs.
     */
     bool isPreviewMode ( ) const
     { return m_preview_mode; }
     void setPreviewMode (bool preview)
     { m_preview_mode = preview; }

    /*------------------------------------------------------------------------*/
    /**
     *  Déseffectue la dernière commande effectuée
     */
    void undo();
    /**
     *  Rejoue la dernière commande défaite
     */
    void redo();

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne un vecteur avec les identifiants des entités actuellement sélectionnées
     */
    std::vector<std::string> getSelectedEntities ( ) const;

    /**
     *  Ajoute à la sélection un ensemble d'entités suivant leurs noms
     */
    void addToSelection (std::vector<std::string>& names);

    /**
     *  Retire de la sélection un ensemble d'entités suivant leurs noms
     */
    void removeFromSelection (std::vector<std::string>& names);

	/*------------------------------------------------------------------------*/
	/** Accesseur sur le nom de l'instance du context
	 * \return  Le nom unique de l'instance.
	 */
	const std::string& getName() const {return m_name;}

    /*------------------------------------------------------------------------*/
    /// retourne le repère dans lequel on travaille.
    Utils::Landmark::kind getLandmark() const {return m_landmark;}

    /// commande, change le repère dans lequel on travaille.
    Internal::M3DCommandResult* setLandmark(Utils::Landmark::kind l);
	SET_SWIG_COMPLETABLE_METHOD(setLandmark)

    /*------------------------------------------------------------------------*/
    /// retourne l'unité de longueur
    Utils::Unit::lengthUnit getLengthUnit() const {return m_length_unit;}

    /// commande, change l'unité de longueur
    Internal::M3DCommandResult* setLengthUnit(const Utils::Unit::lengthUnit& lu);
	SET_SWIG_COMPLETABLE_METHOD(setLengthUnit)

    /*------------------------------------------------------------------------*/
	/// retourne la dimension du maillage en sortie
	meshDim getMeshDim() const {return m_mesh_dim;}
	SET_SWIG_COMPLETABLE_METHOD(getMeshDim)

	/// change la dimension pour le maillage en sortie, 3D par défaut
	Internal::M3DCommandResult* setMesh2D();
	SET_SWIG_COMPLETABLE_METHOD(setMesh2D)

    /*------------------------------------------------------------------------*/
    /** Réinitialisation de la session
     */
    void clearSession();

    /** Retourne toutes les entités visibles, y compris celle détruites mais visibles !
     */
    const std::vector<Utils::Entity*> getAllVisibleEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute un couple unique id et entité.
     */
#ifndef SWIG
    void add(unsigned long id,  Utils::Entity* entity);

    /** \brief  Retire un couple référencé par un unique id.
     */
    void remove(unsigned long id);

	/** \brief Retourne les entités dont les ids sont transmis en argument.
	 *         Une exception est levée si une entité n'est pas dans le contexte
	 *         et si le second argument vaut true.
	 */
	std::vector<Utils::Entity*> get(const std::vector<unsigned long>& ids, bool raisesIfNotFound) const;

	/** \brief Retourne les entités dont les noms sont transmis en argument.
	 *         Une exception est levée si une entité n'est pas dans le contexte
	 *         et si le second argument vaut true.
	 */
	std::vector<Utils::Entity*> get(const std::vector<std::string>& names, bool raisesIfNotFound) const;

    /**  Vide la table de correspondance entre un unique id et l'objet
     *
     *   Utile pour terminer plus rapidement une session.
     *
     * */
	void clearIdToEntity();

	/** Affiche dans le flux la liste des entités stockées dans la map uid vers Entity
	 */
	void showAllEntities(std::ostream & o);

    /** Affiche dans le flux des stats sur la liste des entités stockées dans la map
     */
    void showEntitiesStats(std::ostream & o);

    /** Retourne vrai si la table de correspondance est vide
     */
	bool isIdToEntityEmpty();
#endif

	/*------------------------------------------------------------------------*/
	/** \return		Une référence sur l'entité dont l'identifiant unique est
	 *				transmis en argument.
	 * \exception	Une exception est levée s'il n'y a pas d'entité ayant cet
	 *				identifiant unique.
	 */
	Utils::Entity& uniqueIdToEntity (unsigned long uid) const;
	SET_SWIG_COMPLETABLE_METHOD(uniqueIdToEntity)

	/*------------------------------------------------------------------------*/
	/** \return		Une référence sur l'entité dont le nom est
	 *				transmis en argument.
	 * \exception	Une exception est levée s'il n'y a pas d'entité ayant ce
	 *				nom.
	 */
	Utils::Entity& nameToEntity (const std::string& name) const;
	SET_SWIG_COMPLETABLE_METHOD(nameToEntity)

	/// accesseur sur ne nom de l'exe
	std::string getExeName() const {return m_exeName;}

	/// modificateur sur nom de l'exe
	void setExeName(std::string name) {m_exeName = name;}
	SET_SWIG_COMPLETABLE_METHOD(setExeName)

    /** Les couleurs initiales des représentations des entités. */
    static TkUtil::Color	m_initial_geom_displayColor;
    static TkUtil::Color	m_initial_topo_displayColor;
    static TkUtil::Color	m_initial_mesh_displayColor;
    static TkUtil::Color	m_initial_group_displayColor;

	/**
	 * Créé un nom supposé unique d'instance en utilisant son propre nom comme
	 * suffixe.
	 * Ex : si cette instance s'appelle <I>"Session_1"</I> et <I>base</I>
	 * <I>"GeomManager"</I>, retourne un nom type
	 * <I>"GeomManager_Session_1"</I>.
	 * \param	base Suffixe du nom créé
	 */
	std::string createName (const std::string& base) const;

private:
  
    /**
     * Le chargement optionnel des fichiers table groupe -> couleur.
     */
    static void loadGroupsColors (const Preferences::Section& section);

	/** Nom unique de l'instance. */
	std::string m_name;

	/*------------------------------------------------------------------------*/
    /**
     * Redirige les signaux en vue d'intervenir avec un debuggeur
     * Pourrait effectuer une procedure d'urgence avant de quitter.
     */
    void _redirect_signals(fSignalFunc sig_func);

	/**
 	 * Constructeurs de copie et opérateurs = : interdits.
	 */
	Context (const Context&);
	Context& operator = (const Context&);

    /// Manager pour les commandes
    Mgx3D::Utils::CommandManager*    m_command_manager; // on utilise un pointeur pour éviter d'inclure le .h car cela pose pb / swig
	/// Session Python :
	TkUtil::PythonSession*				m_python_session;
    /// Manager pour les commandes géométriques
    Mgx3D::Geom::GeomManager*		m_geom_manager;
    /// Manager pour les commandes topologiques
    Mgx3D::Topo::TopoManager*		m_topo_manager;
    /// Manager pour le maillage
    Mgx3D::Mesh::MeshManager*        m_mesh_manager;
    /// Groupes pour les différents types d'entités
    Mgx3D::Group::GroupManager*      m_group_manager;
    /// Repères
    Mgx3D::CoordinateSystem::SysCoordManager* m_sys_coord_manager;
    /// Maillages structurés :
    Mgx3D::Structured::StructuredMeshManager*	m_structured_mesh_manager;
    /// Manager pour les commandes de niveau supérieur
    Mgx3D::Internal::M3DCommandManager  m_m3d_command_manager;
    /// Manager de sélection
    Mgx3D::Internal::SelectionManager*	m_selection_manager;
    /// Manager pour la gestion des noms
    Mgx3D::Internal::NameManager       *m_name_manager;

    /** Le gestionnaire de flux d'affichage des informations relatives au
     * déroulement de la session. */
    TkUtil::LogDispatcher               m_log_dispatcher;

	/** Les sorties des logs sur sortie standard et sortie erreurs. */
	TkUtil::OstreamLogOutputStream*		m_stdout_log_stream;
	TkUtil::OstreamLogOutputStream*		m_stderr_log_stream;

    /// Ecrivain pour les commandes au format Python
    Mgx3D::Internal::ScriptingManager*  m_scripting_manager;

    /** La couleur de représentation des entités géométriques. Evolue à chaque
        création d'entité géométrique. */
    TkUtil::Color						m_geom_displayColor;
    /** La couleur de représentation des entités topologiques.  */
    TkUtil::Color						m_topo_displayColor;
    /** La couleur de représentation des entités de maillage. Evolue à chaque
        création d'entité maillage. */
    TkUtil::Color						m_mesh_displayColor;
    /** La couleur de représentation des groupes. Evolue à chaque
        création de groupe.*/
    TkUtil::Color						m_group_displayColor;
	/** La couleur de fond : l'algorithme de détermination des couleurs essaie
	 * de s'en démarquer. */
	TkUtil::Color						m_backgroundColor;

	/** Le masque à appliquer par défaut aux flux sortants de logs. */
	static TkUtil::Log::TYPE	m_logMask;

    /** Les arguments transmis `la ligne de commande. */
    static TkUtil::ArgumentsMap		m_argumentsMap;

    /** Le jeu de caractères utilisé par la sortie standard. */
    static TkUtil::Charset		m_outCharset;

	/** La table groupe -> couleur.  */
	static std::map<std::string, TkUtil::Color>	m_groups_colors;

	/** booléen positionné à vrai seulement lorsque la sessione est terminée
	 * (destruction du context)
	 */
	bool m_is_finish;

	/**  Vrai si le contexte est graphique */
	bool m_is_graphical;

	/** Est on en mode "apperçu" ? */
	bool m_preview_mode;

	/** Ratio de dégradation pour l'affichage du maillage en mode dégradé
	 *
	 * On affiche un maillage avec un noeud sur ratio_degrad
	 */
	int m_ratio_degrad;

	/**
	 * Repère de travail.
	 */
	Utils::Landmark::kind	m_landmark;

	/** Unité de longueur
	 *
	 *  \see Unit
	 */
	Utils::Unit::lengthUnit m_length_unit;

	/** Dimesion de sortie pour le maillage
	 *
	 */
	meshDim m_mesh_dim;

	/** La liste des entités, avec pour clé les identifiants uniques.
	 * \see		uniqueIdToEntity
	 */
	std::map<unsigned long, Utils::Entity*>		_entities;

	/** Mutex pour protéger les variables de classe (<I>Context</I>). */
	TkUtil::Mutex		   						_entitiesMutex;

	/** nom de l'exécutable avec le path */
	std::string m_exeName;

	/** Les propriétés d'affichage globales. */
	Utils::DisplayProperties	m_geomVertexDisplayProperties,
								m_geomCurveDisplayProperties,
								m_geomSurfaceDisplayProperties,
								m_geomVolumeDisplayProperties,
								m_topoVertexDisplayProperties,
								m_topoEdgeDisplayProperties,
								m_topoCoEdgeDisplayProperties,
								m_topoFaceDisplayProperties,
								m_topoCoFaceDisplayProperties,
								m_topoBlockDisplayProperties,
								m_meshCloudDisplayProperties,
								m_meshLineDisplayProperties,
								m_meshSurfaceDisplayProperties,
								m_meshSubSurfaceDisplayProperties,
								m_meshVolumeDisplayProperties,
								m_meshSubVolumeDisplayProperties,
								m_structuredMeshDisplayProperties,
								m_group0DisplayProperties,
								m_group1DisplayProperties,
								m_group2DisplayProperties,
								m_group3DisplayProperties,
								m_sysCoordDisplayProperties;

	/** Les masques globaux d'affichage. */
	unsigned long				m_geomVertexMask,
								m_geomCurveMask,
								m_geomSurfaceMask,
								m_geomVolumeMask,
								m_topoVertexMask,
								m_topoEdgeMask,
								m_topoCoEdgeMask,
								m_topoFaceMask,
								m_topoCoFaceMask,
								m_topoBlockMask,
								m_meshCloudMask,
								m_meshLineMask,
								m_meshSurfaceMask,
								m_meshSubSurfaceMask,
								m_meshVolumeMask,
								m_meshSubVolumeMask,
								m_structuredMeshMask,
								m_sysCoordMask;

	/** Les sauvegardes des masques globaux pour actions d'affichage/masquage mapides. 
	 * \see		restoreGlobalMaskWireProperties
	 * \see		restoreGlobalMaskSolidProperties
	 */
	unsigned long				m_geomSavedSurfaceMask,
								m_geomSavedVolumeMask,
								m_topoSavedCoEdgeMask,
								m_topoSavedCoFaceMask,
								m_topoSavedBlockMask;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/

/**
 * Pour les pythoneries.
 * \return		Le contexte dont le nom est transmis en argument, le créé s'il
 *              n'existe pas
 */
Mgx3D::Internal::Context* getContext (const char* name);

/**
 * Pour les pythoneries.
 * \return      Le premier contexte trouvé, en créé un de nom "python" s'il
 *              n'en existe pas
 */
Mgx3D::Internal::Context* getStdContext ();

/** Version de Magix3D pour les pythoneries */
std::string getVersion();

} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/
#endif /* CONTEXT_H_ */
/*----------------------------------------------------------------------------*/
