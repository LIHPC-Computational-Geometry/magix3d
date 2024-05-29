/**
 * \file		Resources.h
 * \author		Charles PIGNEROL
 * \date		12/02/2019
 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <PrefsCore/BoolNamedValue.h>
#include <PrefsCore/ColorNamedValue.h>
#include <PrefsCore/DoubleNamedValue.h>
#include <PrefsCore/LongNamedValue.h>
#include <PrefsCore/StringNamedValue.h>
#include <PrefsCore/UnsignedLongNamedValue.h>
#include <PrefsCore/Section.h>
#include <TkUtil/Log.h>


namespace Mgx3D
{
	
namespace Internal
{
		
/**
 * Les ressources de l'application à l'exécution, pour l'essentiel des "préférences
 * utilisateur" lues en configuration et éventuellement surchargées à la ligne de
 * commande.
 * 
 * A finaliser : mettre de beaux accesseurs, de belles fonctions load, save, ...
 */
class Resources
{
	public :
	
	static Resources& instance ( );
	
	/** L'application doit elle afficher des boites de dialogue d'erreur
	 * lors des exceptions remontées lors de l'actualisation d'informations
	 * sur des entités ?
	 * \see			displayUpdatesErrors
	 */
	bool _displayUpdatesErrors;

	/** URL du fichier de configuration par défaut. */
	std::string								_defaultConfigURL;

	/** URL du fichier de configuration de l'utilisateur. */
	std::string								_userConfigURL;

	/** URL du fichier des éventuels patchs à appliquer au fichier de
	 * configuration de l'utilisateur. */
	std::string									_userConfigPatchsURL;


	/** URL du fichier du numéro de la dernière version lancée par l'utilisateur. */
	std::string									_userLastVersionURL;

	/**
	 * Le nom de la section racine des préférences utilisateur.
	 * \TODO [CP]		A placer en dehors de l'IHM, mais où ?
	 */
	std::string									_mgx3DRootSectionName;
	
	/**
	 * La police de caractères à utiliser pour les affichages 2D dans la
	 * fenêtre graphique. */
	Preferences::StringNamedValue				_fontFamily;

	/**
	 * La taille de la police de caractères à utiliser pour les affichages 2D
	 * dans la fenêtre graphique. */	
	Preferences::UnsignedLongNamedValue			_fontSize;
	
	/**
	 * Les caractères gras/italiques de la police de caractères à utiliser pour
	 * les affichages 2D dans la fenêtre graphique. */
	Preferences::BoolNamedValue					_fontBold, _fontItalic;
	
	/**
	 * Action du déplacement vertical de la souris avec le bouton droit
	 * pressé. Par défaut, _mouseUpZoom vaut false, et le fait d'aller
	 * vers le bas provoque un zoom, alors que d'aller vers le haut diminue
	 * la vue. Actions inverses si _mouseUpZoom vaut true.
	 */
	 Preferences::BoolNamedValue				_mouseUpZoom;

	/**
	 * <I>True</I> si la molette de la souris joue sur le zoom, <I>false</I> 
	 * dans le cas contraire (déplacement caméra/point focal).
	 * Vaut <I>false</I> par défaut.
	 */
	Preferences::BoolNamedValue					_zoomOnWheel;

	/**
	 * La couleur utilisée pour représenter les entités sélectionnées.
	 */
	Preferences::ColorNamedValue				_selectionColor;

	/**
	 * La largeur de trait utilisée pour représenter les points/vertex sélectionnés.
	 */
	Preferences::DoubleNamedValue				_selectionPointSize;

	/**
	 * La largeur de trait utilisée pour représenter les entités sélectionnées.
	 */
	Preferences::DoubleNamedValue				_selectionLineWidth;

	/**
	 * <I>True</I> si le fait de sélectionner une entité provoque son affichage
	 * au premier plan, <I>false</I> si sa position dans la liste d'entités
	 * affichées est invariante.
	 */
	Preferences::BoolNamedValue					_selectionOnTop;

	/**
	 * Touche permettant de passer à l'entité suivante dans une liste d'entités
	 * pointées au même emplacement de la souris.
	 */
	Preferences::StringNamedValue				_nextSelectionKey;

	/**
	 * <I>True</I> si, en mode sélection, le fait de presser le bouton gauche de
	 * la souris doit déclencher une opération de pointage (<I>picking</I>),
	 * <I>false</I> dans le cas contraire.
	 */
	Preferences::BoolNamedValue					_pickOnLeftButtonDown;

	/**
	 * <I>True</I> si, en mode sélection, le fait de presser le bouton droit de
	 * la souris doit déclencher une opération de pointage (<I>picking</I>),
	 * <I>false</I> dans le cas contraire.
	 */
	Preferences::BoolNamedValue					_pickOnRightButtonDown;

	/**
	 * <I>True</I> si, en mode sélection, le fait de relâcher le bouton gauche
	 * de la souris doit déclencher une opération de pointage (<I>picking</I>),
	 * <I>false</I> dans le cas contraire.
	 */
	Preferences::BoolNamedValue					_pickOnLeftButtonUp;

	/**
	 * <I>True</I> si, en mode sélection, le fait de relâcher le bouton droit de
	 * la souris doit déclencher une opération de pointage (<I>picking</I>),
	 * <I>false</I> dans le cas contraire.
	 */
	Preferences::BoolNamedValue					_pickOnRightButtonUp;

	/**
	 * Le pourcentage à appliquer aux échelles pour avoir les marges des boites
	 * englobantes. Une échelle est ici une puissance de 10 indiquant l'ordre de
	 * grandeur de taille d'une objet ou ensemble d'objets.
	 */
	Preferences::DoubleNamedValue				_marginBoundingBoxPercentage;

	/**
	 * La couleur des tracés pour les mise en évidence (<I>highligthing</I>).
	 */
	Preferences::ColorNamedValue				_highlightColor;

	/**
	 * L'épaisseur des tracés pour les mise en évidence (<I>highligthing</I>).
	 */
	Preferences::DoubleNamedValue				_highLightWidth;

	/**
	 * Faut-il dessiner une boite englobante lors de la mise en évidence ?
	 */
	Preferences::BoolNamedValue					_highLightBoundinBox;

	/**
	 * Faut-il modifier la couleur de l'entité lors de la mise en évidence ?
	 */
	Preferences::BoolNamedValue					_useHighLightColor;

	/**
	 * La couleur des tracés pour les prévisualisations (<I>preview</I>).
	 */
	Preferences::ColorNamedValue				_previewColor;

	/**
	 * L'épaisseur des tracés pour les prévisualisations (<I>preview</I>).
	 */
	Preferences::DoubleNamedValue				_previewWidth;

	/**
	 * L'épaisseur des points pour les prévisualisations (<I>preview</I>).
	 */
	Preferences::DoubleNamedValue				_previewPointSize;

	/**
	 * Les marges à l'intérieur des conteneurs dans les panneaux.
	 */
	Preferences::LongNamedValue					_margin;

	/**
	 * Les espaces entre 2 widgets à l'intérieur des conteneurs dans les
	 * panneaux.
	 */
	Preferences::LongNamedValue					_spacing;

	/**
	 * La couleur des champs de textes lors de la saisie interactive d'une
	 * entité.
	 */
	Preferences::ColorNamedValue				_IDSeizureBackgroundColor;

	/**
	 * Faut-il procéder à une (pré-)validation des champs de texte de saisie
	 * des grandeurs numériques lorsque le panneau perd le focus ?
	 */
	Preferences::BoolNamedValue					_prevalidateSeizure;

	/**
	 * Les noms des entités des panneaux d'opérations doivent ils être 
	 * effacés lorsque les panneaux sont masqués (méthode <I>cancel</I>) ?
	 */
	Preferences::BoolNamedValue					_cancelClearEntities;

	/**
	 * Les panneaux d'opérations doivent ils être initialisés autant que
	 * possible avec la sélection en cours lors de leur activation (méthode
	 * <I>autoUpdate</I>) ?
	 */
	Preferences::BoolNamedValue					_autoUpdateUsesSelection;

	/**
	 * Faut-il afficher une boite de dialogue d'erreur lorsqu'une commande
	 * échoue ?
	 */
	Preferences::BoolNamedValue					_showDialogOnCommandError;

	/**
	 * A partir de quel nombre d'entités sélectionnées ne faut il plus afficher
	 * les propriétés individuelles de la sélection ? */
	Preferences::UnsignedLongNamedValue			_maxIndividualProperties;

	/**
	 * A partir de quel nombre d'entités sélectionnées ne faut il plus afficher
	 * les propriétés communes de la sélection ? */
	Preferences::UnsignedLongNamedValue			_maxCommonProperties;

	/**
	 * Faut-il afficher les logs de type information ? Par défaut oui.
	 */
	Preferences::BoolNamedValue					_logInformations;

	/**
	 * Faut-il afficher les logs de type erreur ? Par défaut oui.
	 */
	Preferences::BoolNamedValue					_logErrors;

	/**
	 * Faut-il afficher les logs de type avertissement ? Par défaut oui.
	 */
	Preferences::BoolNamedValue					_logWarnings;

	/**
	 * Faut-il afficher les logs de type erreur ? Par défaut oui.
	 */
	Preferences::BoolNamedValue					_logTaskOutputs;

	/**
	 * Faut-il afficher les logs de type TRACE_1 (<I>Commandes</I>) ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logTraces_1;

	/**
	 * Faut-il afficher les logs de type TRACE_2 (<I>Scripting</I>) ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logTraces_2;

	/**
	 * Faut-il afficher les logs de type TRACE_3 (<I>Géométrie</I>) ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logTraces_3;

	/**
	 * Faut-il afficher les logs de type TRACE_4 (<I>Topologie</I>) ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logTraces_4;

	/**
	 * Faut-il afficher les logs de type TRACE_5 (<I>Maillage</I>) ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logTraces_5;

	/**
	 * Faut-il afficher la date des logs ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logDate;

	/**
	 * Faut-il afficher l'heure des logs ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logTime;

	/**
	 * Faut-il afficher le thread des logs ?
	 * Par défaut non.
	 */
	Preferences::BoolNamedValue					_logThreadID;

	/**
	 * Utilisation (<I>true</I> ou non <I>false</I>) par défaut des
	 * <I>display lists Open GL</I>.
	 */
	Preferences::BoolNamedValue					_useDisplayList;

	/**
	 * Si <I>true</I> les évènements claviers de repositionnement dans un plan
	 * xOy, xOz, yOz provoquent également une annulation du roulis.
	 */
	Preferences::BoolNamedValue					_xyzCancelRoll;

	/**
	 * Fréquence de rafraîchissement de la vue 3D lors
	 * d'ajouts/suppressions/modifications d'entités.
	 */
	Preferences::UnsignedLongNamedValue			_updateRefreshRate;

	/**
	 * <I>Frame rate</I> demandé hors interactions, en images par seconde.
	 */
	Preferences::DoubleNamedValue				_stillFrameRate;

	/**
	 * <I>Frame rate</I> demandé lors des interactions, en images par seconde.
	 */
	Preferences::DoubleNamedValue				_desiredFrameRate;

	/**
	 * Couleur de fond de la fenêtre graphique (blanc par défaut).
	 */
	Preferences::ColorNamedValue				_background;

	/**
	 * Faut il utiliser plusieurs sources de lumière ?
	 */
	Preferences::BoolNamedValue					_multipleLighting;

	/**
	 * Faut-il afficher le trièdre ? *
	 */
	Preferences::BoolNamedValue					_displayTrihedron;

	/**
	 * Faut-il afficher ou non le point focal (matérialisé par une croix,
	 * équivalent de l'option <I>show center</I>) de <I>ParaView</I>.
	 */
	Preferences::BoolNamedValue					_displayFocalPoint;

	/**
	 * Taille souhaitée de la fenêtre graphique (800x600 par défaut).
	 */
	Preferences::UnsignedLongNamedValue			_graphicalWindowWidth, _graphicalWindowHeight;

	/**
	 * La taille de la fenêtre graphique est-elle fixe ? (<I>false</I> par
	 * défaut).
	 */
	Preferences::BoolNamedValue					_graphicalWindowFixedSize;

	/**
	 * La liste des plugins de l'application.
	 */
	 IN_STD vector<IN_PREFS Section*>			_pluginDefinitions;

	/** Le masque à appliquer dans le flux "debug" de logs (Log::NONE par
	 *  défaut. */
	IN_UTIL Log::TYPE							_debugLogsMask;

	/** Boite de dialogue pour afficher la progression des tâches. */

	/**
	 * Le tri dans le panneau Groupes. Automatique ? Oui par défaut.
	 */
	Preferences::BoolNamedValue					_automaticSort;

	/**
	 * Le type de tri dans le panneau Groupes. Ascendant/Descendant ?
	 * Ascendant par défaut.
	 */
	Preferences::StringNamedValue				_sortType;

	/**
	 * L'affichage multiniveaux des groupes dans leur panneau. Non par défaut.
	 */
	Preferences::BoolNamedValue					_multiLevelGroupsView;

	/**
	 * <P>Flux standards (<I>stdout</I> et <I>stderr</I>) récupérés
	 * (<I>true</I>) ou non (<I>false</I>) par la session <I>python</I> lors de
	 * l'exécution de commandes ou fichiers scripts.<P>
	 * <P>L'intérêt de le faire est que les sorties figurent dans les
	 * <I>logs</I>. L'intérêt de ne pas le faire est en cas de plantage où ces
	 * traces, éventuellement utiles à la mise au point, sont perdues.
	 * </P>
	 */
	Preferences::BoolNamedValue					_catchStdOutputs;

	/**
	 * Le nombre de fichiers scripts conservés en mémoire.
	 */
	Preferences::UnsignedLongNamedValue			_recentScriptCapacity;

	/**
	 * Le jeu de caractères d'encodage des scripts.
	 */
	Preferences::StringNamedValue				_scriptsCharset;
	
		/**
	 * L'état de l'IHM. Repose sur la classe <I>QSettings</I>.
	 */
	//@{

	/**
	 * Faut-il sauvegarder/réutiliser l'état de l'IHM (position, taille, ...)
	 * d'une session à l'autre ?
	 * \see			_organisation
	 * \see			_softwareName
	 * \see			QtMgx3DMainWindow::writeSettings
	 * \see			QtMgx3DMainWindow::readSettings
	 */
	Preferences::BoolNamedValue					_saveGuiState, _loadGuiState;

	/**
	 * L'organisation auteur de ce logiciel.
	 */
	const std::string							_organisation;

	/**
	 * Le nom de ce logiciel.
	 */
	std::string									_softwareName;

	//@}	// L'état de l'IHM.

	/**
 	 * Les représentations graphiques des entités.
 	 */
	//@{

	/** Faut-il détruire la représentation graphique des entités géométriques
	 * lorsqu'elles ne sont plus affichées ?
	 */
	Preferences::BoolNamedValue					_geomDestroyOnHide;

	/** Faut-il détruire la représentation graphique des entités topologiques
	 * lorsqu'elles ne sont plus affichées ?
	 */
	Preferences::BoolNamedValue					_topoDestroyOnHide;

	/** Faut-il détruire la représentation graphique des entités de maillage
	 * lorsqu'elles ne sont plus affichées ?
	 */
	Preferences::BoolNamedValue					_meshDestroyOnHide;

	/** Les scripts à exécuter au lancement de l'application (arguments de
	 * ligne de commande suivant -scripts. */
	std::vector<std::string>					_scripts;

	//@}

	
	private :
	
	Resources ( );
	Resources (const Resources&);
	Resources& operator = (const Resources&);
	~Resources ( );
	static Resources*	_instance;
};	// class Resources

}	// namespace Internal

}	// namespace Mgx3D

#endif	// RESOURCES_H
