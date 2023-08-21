/*----------------------------------------------------------------------------*/
/** \file ContextIfc.h
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 31/01/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef CONTEXT_IFC_H_
#define CONTEXT_IFC_H_
/*----------------------------------------------------------------------------*/

#include "Group/GroupManagerIfc.h"
#include "Utils/SelectionManagerIfc.h"
#include "Utils/Landmark.h"
#include "Utils/Unit.h"
#include "Utils/SwigCompletion.h"
#include "Geom/GeomManagerIfc.h"
#include "Topo/TopoManagerIfc.h"
#include "Mesh/MeshManagerIfc.h"
#include "Structured/StructuredMeshManagerIfc.h"
#include "SysCoord/SysCoordManagerIfc.h"
#include "Internal/NameManager.h"
#include "Internal/ScriptingManager.h"
#include "Internal/M3DCommandResultIfc.h"
#include <PythonUtil/PythonSession.h>
#include <TkUtil/LogDispatcher.h>
#include <PrefsCore/BoolNamedValue.h>
#include <TkUtil/PaintAttributes.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Utils {
class CommandManagerIfc;
class Property;
class DisplayProperties;
}

/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Internal
 *
 * \brief Espace de nom des classes communes au projet de haut niveau
 *
 *
 */
namespace Internal {
class M3DCommandManager;
/*----------------------------------------------------------------------------*/
/** \class ContextIfc
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
 */
/*----------------------------------------------------------------------------*/
class ContextIfc{

public:

    /*------------------------------------------------------------------------*/
	/**
	 * Initialisation/finalisation de l'API.
	 */
	static void initialize ( );
	static void finalize ( );

	/**
	 * La gestion de la configuration.
	 */

	/**
	 * Applique la configuration reçue en argument.
	 * \param		Section racine de la configuration à appliquer.
	 */
	void load (const Preferences::Section& contextSection);

	/**
	 * Ajoute à la section tranmise en argument la configuration actuelle.
	 * \param		Section principale de la configuration de l'application.
	 * \warning		N'effectue aucune action persistante.
	 * \see			setPersistantConfiguration
	 */
	void saveConfiguration (Preferences::Section& contextSection);
	

    /*------------------------------------------------------------------------*/
	/** Suivant ce que l'on souhaite voir apparaitre dans le script de sauvegarde
	 */
	enum encodageScripts {
		WITHNAMES=0,
		WITHIDREF=1,
		WITHCOORD=2
	};


	/*------------------------------------------------------------------------*/
	/** Suivant ce que l'on souhaite comme gestionnaire pour noyau géométrique
	 */
	enum geomKernel {
		WITHOCC=0,
		WITHOCAF=1
	};

	/*------------------------------------------------------------------------*/
	/** Possibilité de spécifier que le maillage est 2D
	 */
	enum meshDim {
		MESH2D=2,
		MESH3D=3
	};



    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** \brief  Constructeur. S'enregistre auprès d'une liste de contextes
	 *			existants et accessibles via la fonction <I>getContextIfc</I>.
	 * \param	Nom unique de ce contexte
	 * \see		getContextIfc
     */
    ContextIfc(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur. Se désenregistre auprès de la liste de contextes
	 *			existants et accessibles via la fonction <I>getContextIfc</I>.
	 * \see		getContextIfc
     */
    virtual ~ContextIfc();
#endif

	/**
	 * Créé un nom supposé unique d'instance en utilisant son propre nom comme
	 * suffixe.
	 * Ex : si cette instance s'appelle <I>"Session_1"</I> et <I>base</I>
	 * <I>"GeomManager"</I>, retourne un nom type
	 * <I>"GeomManager_Session_1"</I>.
	 * \param		Suffixe du nom créé
	 */
	virtual std::string createName (const std::string& base) const;
	
    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager géométrique
	 * \exception	Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Geom::GeomManagerIfc& getGeomManager();
    virtual const Mgx3D::Geom::GeomManagerIfc& getGeomManager() const;
#ifndef SWIG
    virtual void setGeomManager (Mgx3D::Geom::GeomManagerIfc*);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager topologique
	 * \exception	Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Topo::TopoManagerIfc& getTopoManager();
    virtual const Mgx3D::Topo::TopoManagerIfc& getTopoManager() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de maillage
	 * \exception	Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Mesh::MeshManagerIfc& getMeshManager();
    virtual const Mgx3D::Mesh::MeshManagerIfc& getMeshManager() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de groupes
     * \exception   Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Group::GroupManagerIfc& getGroupManager();
    virtual const Mgx3D::Group::GroupManagerIfc& getGroupManager() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de repères
     * \exception   Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::CoordinateSystem::SysCoordManagerIfc& getSysCoordManager();
    virtual const Mgx3D::CoordinateSystem::SysCoordManagerIfc& getSysCoordManager() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de maillages structurés
     * \exception   Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Structured::StructuredMeshManagerIfc& getStructuredMeshManager();
    virtual const Mgx3D::Structured::StructuredMeshManagerIfc& getStructuredMeshManager() const;

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
    /** Accesseur sur le nom de l'instance du context
     * \return  Le nom unique de l'instance.
     */
    const std::string& getName ( ) const;

	/*------------------------------------------------------------------------*/
	/**
	 * Sauvegarde dans un fichier les commandes Python actuellement
	 * dans le commandManager (les undo n'apparaissent pas)
	 * \param fileName nom du fichier pour la sauvegarde
	 * \param enc le type d'information dans le script (nom, référence / commandes, coordonnées)
	 * \param jeu de caractères utilisé pour encoder le fichier script
	 */
	virtual void savePythonScript (std::string fileName, encodageScripts enc, TkUtil::Charset::CHARSET charset);

	/*------------------------------------------------------------------------*/
	/// active le décalage des id pour renommer les entités
	virtual void activateShiftingNameId();

	/// désactive le décalage des id pour renommer les entités
	virtual void unactivateShiftingNameId();

	/*------------------------------------------------------------------------*/


#ifndef SWIG

    /*------------------------------------------------------------------------*/
	/**  Booléen qui permet de savoir si l'on est en train de quitter la session ou non */
    virtual bool isFinished();
    virtual void setFinished(bool b);

    /*------------------------------------------------------------------------*/
    /** \param  Le nouveau nom unique de l'instance.
     */
    virtual void setName (const std::string& name);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur la session de scripting <I>Python</I>.
	 * \exception	Une exception est levée en l'absence de session associée.
     */
    virtual TkUtil::PythonSession& getPythonSession();
    virtual void setPythonSession (TkUtil::PythonSession*);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de créations de niveau supérieur (compositions)
	 * \exception	Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Internal::M3DCommandManager& getM3DCommandManager();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le manager de commandes
	 * \exception	Une exception est levée en l'absence de manager associé.
     */
    virtual Mgx3D::Utils::CommandManagerIfc& getCommandManager();

	/*------------------------------------------------------------------------*/
	/** \brief	Autorise-t-on l'usage de tâches parallèles lors de l'exécution
	 *			de commandes (une tâche étant un élément de commande) ?
	 */
	Preferences::BoolNamedValue			allowThreadedCommandTasks;

	/*------------------------------------------------------------------------*/
	/** \brief	Autorise-t-on l'usage de tâches parallèles lors du prémaillage
	 *			des arêtes ?
	 */
	Preferences::BoolNamedValue			allowThreadedEdgePreMeshTasks;

	/*------------------------------------------------------------------------*/
	/** \brief	Autorise-t-on l'usage de tâches parallèles lors du prémaillage
	 *			des faces ?
	 */
	Preferences::BoolNamedValue			allowThreadedFacePreMeshTasks;

	/*------------------------------------------------------------------------*/
	/** \brief	Autorise-t-on l'usage de tâches parallèles lors du prémaillage
	 *			des blocs ?
	 */
	Preferences::BoolNamedValue			allowThreadedBlockPreMeshTasks;

	/*------------------------------------------------------------------------*/
	/** Le thread principal de l'application. */
	static pthread_t							threadId;

	/*------------------------------------------------------------------------*/
	/** \brief	Autorise-t-on la mémorisation du prémaillage des arêtes ?
	 */
	Preferences::BoolNamedValue			memorizeEdgePreMesh;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le gestionnaire de sélection.
	 * \exception	Une exception est levée en l'absence de gestionnaire associé.
     */
    virtual Mgx3D::Utils::SelectionManagerIfc& getSelectionManager ( );
    virtual const Mgx3D::Utils::SelectionManagerIfc& getSelectionManager ( ) const;
    virtual void setSelectionManager (Mgx3D::Utils::SelectionManagerIfc*);

	/**
	 * La gestion de l'affichage des informations relatives au déroulement des
	 * sessions.
	 */
	//@{

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le gestionnaire de flux.
     */
    virtual TkUtil::LogDispatcher& getLogDispatcher ( );
    virtual const TkUtil::LogDispatcher& getLogDispatcher ( ) const;
    virtual TkUtil::LogOutputStream* getLogStream ( );

	/** <I>true</I> s'il faut afficher les sorties des commandes scripts
	 * exécutées, <I>false</I> dans le cas contraire.
	 * \warning		Certaines versions de <I>MPI</I> seraient incompatible avec
	 *				<I>fork</I>, et ce service repose sur les classes
	 *				<I>PythonSession</I> et <I>DuplicatedStream</I> (au 26/07/12).
	 *				La classe <I>DuplicatedStream</I> repose sur <I>fork</I>, et
	 *				il arrive que le serveur, lancé sur station et avec mpich 1.2.6,
	 *				reste en attente sur un <I>waitpid</I> du processus fils chargé
	 *				de rediriger les sorties du script.
	 *				Donc, au moins pour les serveurs, ne pas avoir peur d'affecter
	 *				<I>false</I> à cette variable.
	 */
	Preferences::BoolNamedValue			displayScriptOutputs;

	//@}	// gestion des logs

	/*------------------------------------------------------------------------*/
	/**
	 * \brief  Accesseur sur le gestionnaire d'écrivain de scripts
	 */
	virtual Internal::ScriptingManager& getScriptingManager();

    /*------------------------------------------------------------------------*/
    /**
     * Choix d'un nom de fichier pour stockage des commandes Python
     */
    virtual std::string newScriptingFileName();

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne vrai si le contexte est graphique
     *  c'est à dire s'il existe une représentation des entités
     */
    virtual bool isGraphical() const;

    /// modifie le contexte
    virtual void setGraphical(bool gr);

	/**
	 * \return	La couleur de fond du système graphique.
	 */
	virtual TkUtil::Color getBackground ( ) const;

	/**
	 * \param	La nouvelle couleur de fond du système graphique.
	 */
	virtual void setBackground (const TkUtil::Color& bg);


	/**
	 * \return  Le ratio de dégradation pour l'affichage du maillage en mode dégradé
	 */
	virtual int getRatioDegrad();

	/**
	 * Modifie le ratio de dégradation pour l'affichage du maillage en mode dégradé
	 */
	virtual void setRatioDegrad(int ratio);

    /*------------------------------------------------------------------------*/
    /** \brief  Adapte les Managers pour le cas d'une importation de script.
     *
     * Il sera fait un décalage sur les Id des noms des entités
     */
    virtual void beginImportScript();
    /** \brief  Termine l'adaptation des Managers pour le cas d'une importation de script.
     */
    virtual void endImportScript();
#endif

	/// retourne le repère dans lequel on travaille.
	virtual Utils::Landmark::kind getLandmark ( ) const;

	/// modifie le repère dans lequel on travaille.
	virtual Internal::M3DCommandResultIfc* setLandmark (Utils::Landmark::kind);
	SET_SWIG_COMPLETABLE_METHOD(setLandmark)

    /// retourne l'unité de longueur
    virtual Utils::Unit::lengthUnit getLengthUnit() const;

    /// change l'unité de longueur
    virtual Internal::M3DCommandResultIfc* setLengthUnit(const Utils::Unit::lengthUnit& lu);
	SET_SWIG_COMPLETABLE_METHOD(setLengthUnit)

    /// type de gestionnaire géométrique
    virtual geomKernel getGeomKernel() const;
	SET_SWIG_COMPLETABLE_METHOD(getGeomKernel)

	/// retourne la dimension du maillage en sortie
	virtual meshDim getMeshDim() const;
	SET_SWIG_COMPLETABLE_METHOD(getMeshDim)

	/// change la dimension pour le maillage en sortie, 3D par défaut
	virtual Internal::M3DCommandResultIfc* setMesh2D();
	SET_SWIG_COMPLETABLE_METHOD(setMesh2D)

    /** Réinitialisation de la session
     */
    virtual void clearSession();

    /** Retourne toutes les entités visibles, y compris celle détruites mais visibles !
     */
    virtual const std::vector<Utils::Entity*> getAllVisibleEntities();


#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute un couple unique id et entité.
     */
    virtual void add(unsigned long id,  Utils::Entity* entity);

    /** \brief  Retire un couple référencé par un unique id.
     */
    virtual void remove(unsigned long id);

	/** \brief Retourne les entités dont les ids sont transmis en argument.
	 *         Une exception est levée si une entité n'est pas dans le contexte
	 *         et si le second argument vaut true.
	 */
	virtual std::vector<Utils::Entity*> get(const std::vector<unsigned long>& ids, bool raisesIfNotFound) const;
#endif

	/*------------------------------------------------------------------------*/
	/** \return		Une référence sur l'entité dont l'identifiant unique est
	 *				transmis en argument.
	 * \exception	Une exception est levée s'il n'y a pas d'entité ayant cet
	 *				identifiant unique.
	 */
	virtual Utils::Entity& uniqueIdToEntity (unsigned long uid) const;
	SET_SWIG_COMPLETABLE_METHOD(uniqueIdToEntity)

	/*------------------------------------------------------------------------*/
	/** \return		Une référence sur l'entité dont le nom est
	 *				transmis en argument.
	 * \exception	Une exception est levée s'il n'y a pas d'entité ayant ce
	 *				nom.
	 */
	virtual Utils::Entity& nameToEntity (const std::string& name) const;
	SET_SWIG_COMPLETABLE_METHOD(nameToEntity)

	/// accesseur sur ne nom de l'exe
	virtual std::string getExeName() const;

	/// modificateur sur nom de l'exe
	virtual void setExeName(std::string name);
	SET_SWIG_COMPLETABLE_METHOD(setExeName)




private:

	/**
 	 * Constructeurs de copie et opérateurs = : interdits.
	 */
	ContextIfc (const ContextIfc&);
	ContextIfc& operator = (const ContextIfc&);

	/** Nom unique de l'instance. */
	std::string							m_name;


};	// class ContextIfc

/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/


/**
 * Pour les pythoneries.
 * \return      Le premier contexte trouvé
 */
Mgx3D::Internal::ContextIfc* getFirstContextIfc ();

/**
 * Pour les pythoneries.
 * \return      Le contexte dont le nom est transmis en argument.
 */
Mgx3D::Internal::ContextIfc* getContextIfc (const char* name);

} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/
#endif /* CONTEXT_IFC_H_ */
/*----------------------------------------------------------------------------*/
