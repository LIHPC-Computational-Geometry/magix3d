/*----------------------------------------------------------------------------*/
/** \file Context.h
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef CONTEXT_H_
#define CONTEXT_H_
/*----------------------------------------------------------------------------*/

#include "Internal/ContextIfc.h"
#include "Internal/M3DCommandManager.h"
#include "Internal/NameManager.h"
#include "Internal/ScriptingManager.h"

#include "Utils/SelectionManager.h"
#include "Utils/Unit.h"
#include "Utils/Entity.h"

#include "Geom/GeomManager.h"
#include "Topo/TopoManager.h"
#include "Mesh/MeshManager.h"
#include "Group/GroupManager.h"
#include "SysCoord/SysCoordManager.h"
#include "Structured/StructuredMeshManager.h"

#include <TkUtil/ArgumentsMap.h>
#include <TkUtil/PaintAttributes.h>
#include <TkUtil/LogDispatcher.h>
#include <TkUtil/OstreamLogOutputStream.h>
#include <PrefsCore/BoolNamedValue.h>
#include <PrefsCore/Section.h>

#include <TkUtil/Mutex.h>
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Utils {
class CommandManager;
class Property;
class DisplayProperties;
}
namespace Mesh {
class CommandChangeMeshDim;
class MeshImplementation;
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
class Context : public ContextIfc {

	friend class CommandChangeLengthUnit;
	friend class CommandChangeLandmark;
	friend class Mesh::CommandChangeMeshDim;
	friend class Mesh::MeshImplementation;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur. S'enregistre auprès d'une liste de contextes
	 *			existants et accessibles via la fonction <I>getContext</I>.
	 * \param	Nom unique de ce contexte
	 * \param   Utilise les flux standards (à éviter depuis l'IHM, car il y a
	 * des conflits avec les sorties de commandes python)
	 * \see		getContext
     */
    Context(const std::string& name, bool withStdOutputs = true);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur. Se désenregistre auprès de la liste de contextes
	 *			existants et accessibles via la fonction <I>getContext</I>.
	 * \see		getContext
     */
    virtual ~Context();

    /** \brief	Initialisation de l'API.
     *  \param	argc et argv du main. Utilisation des éventuels arguments :<BR>
     *		-outCharsetRef àéèùô : détection automatique du jeu de caractères de
     *		la sortie standard (API TkUtil::Locale)<BR>
     *		-outCharset suivi de UTF-8, UTF-16, ISO-8859 ou ASCII pour imposer le jeu
     *          de caractères de la sortie standard.
     *      -useOCAF
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
    virtual bool isFinished() {return m_is_finish;}
    virtual void setFinished(bool b) {m_is_finish = b;}

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur la session de scripting <I>Python</I>.
	 * \exception	Une exception est levée en l'absence de session associée.
     */
    virtual TkUtil::PythonSession& getPythonSession();
    virtual void setPythonSession (TkUtil::PythonSession*);

#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager géométrique
     */

    virtual Mgx3D::Geom::GeomManagerIfc& getGeomManager();
    virtual const Mgx3D::Geom::GeomManagerIfc& getGeomManager() const;
	virtual void setGeomManager (Mgx3D::Geom::GeomManagerIfc* mgr);
    virtual Mgx3D::Geom::GeomManager& getLocalGeomManager();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager topologique
     */
    virtual Mgx3D::Topo::TopoManagerIfc& getTopoManager();
    virtual const Mgx3D::Topo::TopoManagerIfc& getTopoManager() const;
	virtual void setTopoManager (Mgx3D::Topo::TopoManagerIfc* mgr);
    virtual Mgx3D::Topo::TopoManager& getLocalTopoManager();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de maillage
     */
    virtual Mgx3D::Mesh::MeshManagerIfc& getMeshManager();
    virtual const Mgx3D::Mesh::MeshManagerIfc& getMeshManager() const;
	virtual void setMeshManager (Mgx3D::Mesh::MeshManagerIfc* mgr);
    virtual Mgx3D::Mesh::MeshManager& getLocalMeshManager();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de groupes
     */
    virtual Mgx3D::Group::GroupManagerIfc& getGroupManager();
    virtual const Mgx3D::Group::GroupManagerIfc& getGroupManager() const;
    virtual void setGroupManager (Mgx3D::Group::GroupManagerIfc* mgr);
    virtual Mgx3D::Group::GroupManager& getLocalGroupManager();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de repères
     */
    virtual Mgx3D::CoordinateSystem::SysCoordManagerIfc& getSysCoordManager();
    virtual const Mgx3D::CoordinateSystem::SysCoordManagerIfc& getSysCoordManager() const;
    virtual void setSysCoordManager (Mgx3D::CoordinateSystem::SysCoordManagerIfc* mgr);
    virtual Mgx3D::CoordinateSystem::SysCoordManager& getLocalSysCoordManager();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de maillages structurés
     */
    virtual Mgx3D::Structured::StructuredMeshManagerIfc& getStructuredMeshManager();
    virtual const Mgx3D::Structured::StructuredMeshManagerIfc& getStructuredMeshManager() const;
    virtual void setStructuredMeshManager (Mgx3D::Structured::StructuredMeshManagerIfc* mgr);


    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de créations de niveau supérieur (compositions)
     */
#ifndef SWIG
    virtual Mgx3D::Internal::M3DCommandManager& getM3DCommandManager() {return m_m3d_command_manager;}
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de commandes
     */
#ifndef SWIG
    virtual Mgx3D::Utils::CommandManagerIfc& getCommandManager();
	virtual void setCommandManager (Mgx3D::Utils::CommandManagerIfc* mgr);
    virtual Mgx3D::Utils::CommandManager& getLocalCommandManager();
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de noms
     */
#ifndef SWIG
    virtual Mgx3D::Internal::NameManager& getNameManager() {return *m_name_manager;}
    virtual const Mgx3D::Internal::NameManager& getNameManager() const {return *m_name_manager;}
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le gestionnaire de sélection.
     */
#ifndef SWIG
    virtual Mgx3D::Utils::SelectionManagerIfc& getSelectionManager ( );
    virtual const Mgx3D::Utils::SelectionManagerIfc& getSelectionManager ( ) const;
    virtual void setSelectionManager (Mgx3D::Utils::SelectionManagerIfc*);
#endif

	/*------------------------------------------------------------------------*/
    /**
     * Sauvegarde dans un fichier les commandes Python actuellement
     * dans le commandManager (les undo n'apparaissent pas)
     * \param fileName nom du fichier pour la sauvegarde
     * \param enc le type d'information dans le script (nom, référence / commandes, coordonnées)
     * \param enc le type d'information dans le script (nom, référence / commandes, coordonnées)
     */
    virtual void savePythonScript (std::string fileName, encodageScripts enc, TkUtil::Charset::CHARSET charset);

    /*------------------------------------------------------------------------*/
    /// active le décalage des id pour renommer les entités
    virtual void activateShiftingNameId();

    /// désactive le décalage des id pour renommer les entités
    virtual void unactivateShiftingNameId();

    /*------------------------------------------------------------------------*/

	/**
	 * La gestion de l'affichage des informations relatives au déroulement des
	 * sessions.
	 */
	//@{
    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le gestionnaire de flux.
     */
#ifndef SWIG
    virtual TkUtil::LogDispatcher& getLogDispatcher ( ) {return m_log_dispatcher;}
    virtual const TkUtil::LogDispatcher& getLogDispatcher ( ) const {return m_log_dispatcher;}

    virtual TkUtil::LogOutputStream* getLogStream ( ) {return &m_log_dispatcher; }
	/** \brief	Accesseur sur les flux de logs sur sortie standard et sortie
	 *			en erreur.
	 */
	virtual TkUtil::OstreamLogOutputStream& getStdLogStream ( );
	virtual TkUtil::OstreamLogOutputStream& getErrLogStream ( );

	/**
	 * \return		Le masque à appliquer par défaut aux flux sortants de logs.
	 */
	static TkUtil::Log::TYPE getLogsMask ( );

	/**
	 * \param		Nouveau masque à appliquer par défaut aux flux sortants de
	 *				logs.
	 */
	static void setLogsMask (TkUtil::Log::TYPE mask);
	//@}	// gestion des logs

    /*------------------------------------------------------------------------*/
    /**
    * \brief  Accesseur sur le gestionnaire d'écrivain de scripts
    */
    virtual Internal::ScriptingManager& getScriptingManager();

	/*------------------------------------------------------------------------*/
	/**
	 * \param	Nouveau gestionnaire d'écrivain de scripts (adoption).
	 */
	virtual void setScriptingManager (Internal::ScriptingManager* manager);


#endif
    /*------------------------------------------------------------------------*/
    /**
     * Attribution d'un nouveau id unique par entité, incrémentation automatique
     */
#ifndef SWIG
    unsigned long newUniqueId();
#endif
    /**
      * Attribution d'un nouveau id unique par entité, sans incrémentation
      */
#ifndef SWIG
    unsigned long nextUniqueId();
#endif

    /*------------------------------------------------------------------------*/
    /**
     * Attribution d'un nouveau id unique par explorateur
     */
#ifndef SWIG
    unsigned long newExplorerId();
#endif

    /*------------------------------------------------------------------------*/
    /**
    * Allocation d'une nouvelle propriété pour une entité (le nom)
    * \param t le type d'objet
    */
#ifndef SWIG
    Utils::Property* newProperty(const Utils::Entity::objectType& ot);
#endif

    /*------------------------------------------------------------------------*/
    /**
    * Allocation d'une nouvelle propriété pour une entité avec un nom d'imposé
    * \param t le type d'objet
    * \param name le nom de l'objet
    */
#ifndef SWIG
    Utils::Property* newProperty(const Utils::Entity::objectType& ot, const std::string& name);
#endif

    /*------------------------------------------------------------------------*/
    /**
    * \return	Les propriétés d'affichage globale pour une entité (la couleur)
    * \param	t le type d'objet
    */
#ifndef SWIG
    Utils::DisplayProperties& globalDisplayProperties (
										Utils::Entity::objectType ot);
    const Utils::DisplayProperties& globalDisplayProperties (
										Utils::Entity::objectType ot) const;
#endif

    /*------------------------------------------------------------------------*/
    /**
    * \return	Le masque d'affichage global pour une entité (filaire, surfacique, ...)
    * \param	t le type d'objet
    */
#ifndef SWIG
    unsigned long& globalMask (Utils::Entity::objectType ot);
    unsigned long globalMask (Utils::Entity::objectType ot) const;
#endif

    /*------------------------------------------------------------------------*/
    /**
    * Allocation d'une nouvelle propriété d'affichage pour une entité (la couleur)
    * \param t le type d'objet
    */
#ifndef SWIG
    Utils::DisplayProperties* newDisplayProperties(const Utils::Entity::objectType& ot);
#endif

    /** Passage en mode apperçu ou non. En mode apperçu les couleurs fournies
     * par newDisplayProperties n'interviennent pas dans l'algorithme d'attribution
     * des couleurs.
     */
     virtual bool isPreviewMode ( ) const
     { return m_preview_mode; }
     virtual void setPreviewMode (bool preview)
     { m_preview_mode = preview; }

    /*------------------------------------------------------------------------*/
    /**
    * Affectation d'une nouvelle représentation graphique pour une entité
	* SI l'entité est affichable
	* (<I>true == getDisplayProperties ( ).isDisplayable</I>.
    */
#ifndef SWIG
    virtual void newGraphicalRepresentation(Utils::Entity& entity);
#endif

    /*------------------------------------------------------------------------*/
    /**
     *  Déseffectue la dernière commande effectuée
     */
    virtual void undo();
    /**
     *  Rejoue la dernière commande défaite
     */
    virtual void redo();

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne un vecteur avec les identifiants des entités actuellement sélectionnées
     */
    virtual std::vector<std::string> getSelectedEntities ( ) const;

    /**
     *  Ajoute à la sélection un ensemble d'entités suivant leurs noms
     */
    virtual void addToSelection (std::vector<std::string>& names);

    /**
     *  Retire de la sélection un ensemble d'entités suivant leurs noms
     */
    virtual void removeFromSelection (std::vector<std::string>& names);

    /*------------------------------------------------------------------------*/
    /**
     * Choix d'un nom de fichier pour stockage des commandes Python
     */
#ifndef SWIG
    virtual std::string newScriptingFileName();
#endif

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne vrai si le contexte est graphique
     *  c'est à dire s'il existe une représentation des entités
     */
    virtual bool isGraphical() const
    {return m_is_graphical;}

    /// modifie le contexte (avec affichage graphique ou non)
    virtual void setGraphical(bool gr)
    {m_is_graphical = gr;}

#ifndef SWIG

	/**
	 * \return  La couleur de fond du système graphique.
	 */
	virtual TkUtil::Color getBackground ( ) const;

    /**
	 * \param   La nouvelle couleur de fond du système graphique.
	 */
	virtual void setBackground (const TkUtil::Color& bg);

#endif	// SWIG

	/**
	 * \return  Le ratio de dégradation pour l'affichage du maillage en mode dégradé
	 */
	virtual int getRatioDegrad() {return m_ratio_degrad;}

	/**
	 * Modifie le ratio de dégradation pour l'affichage du maillage en mode dégradé
	 */
	virtual void setRatioDegrad(int ratio) {m_ratio_degrad = ratio;}

	/*------------------------------------------------------------------------*/
    /** \brief  Adapte les Managers pour le cas d'une importation de script.
     *
     * Il sera fait un décalage sur les Id des noms des entités
     */
#ifndef SWIG
    virtual void beginImportScript();
#endif
    /** \brief  Termine l'adaptation des Managers pour le cas d'une importation de script.
     */
#ifndef SWIG
    virtual void endImportScript();
#endif

    /*------------------------------------------------------------------------*/
    /// retourne le repère dans lequel on travaille.
    virtual Utils::Landmark::kind getLandmark() const {return m_landmark;}

    /// commande, change le repère dans lequel on travaille.
    virtual Internal::M3DCommandResultIfc* setLandmark(Utils::Landmark::kind l);

    /*------------------------------------------------------------------------*/
    /// retourne l'unité de longueur
    virtual Utils::Unit::lengthUnit getLengthUnit() const {return m_length_unit;}

    /// commande, change l'unité de longueur
    virtual Internal::M3DCommandResultIfc* setLengthUnit(const Utils::Unit::lengthUnit& lu);

    /*------------------------------------------------------------------------*/
	/// retourne la dimension du maillage en sortie
	virtual meshDim getMeshDim() const {return m_mesh_dim;}

	/// change la dimension pour le maillage en sortie, 3D par défaut
	virtual Internal::M3DCommandResultIfc* setMesh2D();

   /*------------------------------------------------------------------------*/
    /// type de gestionnaire géométrique
    virtual geomKernel getGeomKernel() const {return m_geom_kernel;}

    /*------------------------------------------------------------------------*/
    /** Réinitialisation de la session
     */
    virtual void clearSession();

    /** Retourne toutes les entités visibles, y compris celle détruites mais visibles !
     */
    virtual const std::vector<Utils::Entity*> getAllVisibleEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute un couple unique id et entité.
     */
#ifndef SWIG
    virtual void add(unsigned long id,  Utils::Entity* entity);

    /** \brief  Retire un couple référencé par un unique id.
     */
    virtual void remove(unsigned long id);

	/** \brief Retourne les entités dont les ids sont transmis en argument.
	 *         Une exception est levée si une entité n'est pas dans le contexte
	 *         et si le second argument vaut true.
	 */
	virtual std::vector<Utils::Entity*> get(const std::vector<unsigned long>& ids, bool raisesIfNotFound) const;


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
	virtual Utils::Entity& uniqueIdToEntity (unsigned long uid) const;

	/*------------------------------------------------------------------------*/
	/** \return		Une référence sur l'entité dont le nom est
	 *				transmis en argument.
	 * \exception	Une exception est levée s'il n'y a pas d'entité ayant ce
	 *				nom.
	 */
	virtual Utils::Entity& nameToEntity (const std::string& name) const;


	/// accesseur sur ne nom de l'exe
	virtual std::string getExeName() const {return m_exeName;}

	/// modificateur sur nom de l'exe
	virtual void setExeName(std::string name) {m_exeName = name;}

    /** Les couleurs initiales des représentations des entités. */
    static TkUtil::Color	m_initial_geom_displayColor;
    static TkUtil::Color	m_initial_topo_displayColor;
    static TkUtil::Color	m_initial_mesh_displayColor;
    static TkUtil::Color	m_initial_group_displayColor;


protected:
    /*------------------------------------------------------------------------*/
    /**
     * Redirige les signaux en vue d'intervenir avec un debuggeur
     * Pourrait effectuer une procedure d'urgence avant de quitter.
     */
    void _redirect_signals(fSignalFunc sig_func);

public :
	/**
	 * Créé un nom supposé unique d'instance en utilisant son propre nom comme
	 * suffixe.
	 * Ex : si cette instance s'appelle <I>"Session_1"</I> et <I>base</I>
	 * <I>"GeomManager"</I>, retourne un nom type
	 * <I>"GeomManager_Session_1"</I>.
	 * \param		Suffixe du nom créé
	 */
	virtual std::string createName (const std::string& base) const;

	//avant private, passe en protected par F. Ledoux car accès par des classes
	//filles utile en parallele
protected:

	/**
 	 * Constructeurs de copie et opérateurs = : interdits.
	 */
	Context (const Context&);
	Context& operator = (const Context&);

    /// Manager pour les commandes
    Mgx3D::Utils::CommandManagerIfc*    m_command_manager; // on utilise un pointeur pour éviter d'inclure le .h car cela pose pb / swig
	/// Session Python :
	TkUtil::PythonSession*				m_python_session;
    /// Manager pour les commandes géométriques
    Mgx3D::Geom::GeomManagerIfc*		m_geom_manager;
    /// Manager pour les commandes topologiques
    Mgx3D::Topo::TopoManagerIfc*		m_topo_manager;
    /// Manager pour le maillage
    Mgx3D::Mesh::MeshManagerIfc*        m_mesh_manager;
    /// Groupes pour les différents types d'entités
    Mgx3D::Group::GroupManagerIfc*      m_group_manager;
    /// Repères
    Mgx3D::CoordinateSystem::SysCoordManagerIfc* m_sys_coord_manager;
    /// Maillages structurés :
    Mgx3D::Structured::StructuredMeshManagerIfc*	m_structured_mesh_manager;
    /// Manager pour les commandes de niveau supérieur
    Mgx3D::Internal::M3DCommandManager  m_m3d_command_manager;
    /// Manager de sélection
    Mgx3D::Utils::SelectionManagerIfc*	m_selection_manager;
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

	/** gestionnaire noyau géométrique */
    static geomKernel m_geom_kernel;

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
