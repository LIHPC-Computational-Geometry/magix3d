/**
 * \file		Resources.cpp
 * \author		Charles PIGNEROL
 * \date		12/02/2019
 */

#include "Internal/Resources.h"
#include "Utils/Common.h"

using namespace TkUtil;


namespace Mgx3D
{
	
namespace Internal
{
		
Resources*	Resources::_instance	= 0;



Resources::Resources ( )
: _displayUpdatesErrors (true),
_defaultConfigURL (""),
_userConfigURL (""),
_userConfigPatchsURL (""),
_userLastVersionURL (""),
_mgx3DRootSectionName ("Magix3D"),
_confirmQuitAction ("confirmQuitAction", true, UTF8String ("true si une boite de dialogue de confirmation doit être affichée lorsque l'utilisateur met fin à Magix3D, false dans le cas contraire.")),
_allowThreadedCommandTasks ("allowThreadedCommandTasks", true, UTF8String ("true si une commande est autorisée à être décomposée en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_allowThreadedEdgePreMeshTasks ("allowThreadedEdgePreMeshTasks", true, UTF8String ("true si le prémaillage des arêtes peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_allowThreadedFacePreMeshTasks ("allowThreadedFacePreMeshTasks", true, UTF8String ("true si le prémaillage des faces peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_allowThreadedBlockPreMeshTasks ("allowThreadedBlockPreMeshTasks", true, UTF8String ("true si le prémaillage des blocs peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_displayScriptOutputs ("displayScriptOutputs", true, UTF8String ("true si le programme doit afficher les sorties des commandes script, false dans le cas contraire.")),
_memorizeEdgePreMesh ("memorizeEdgePreMesh", true, UTF8String ("true si le programme doit mémoriser le prémaillage des arêtes, false dans le cas contraire.")),
_fontFamily ("fontFamily", "Arial", UTF8String ("Police de caractères utilisée pour les affichages graphiques. Valeurs possibles : Arial, Times, Courier.", Charset::UTF_8)),
_fontSize ("fontSize", 12, UTF8String ("Taille de la police de caractères utilisée pour les affichages graphiques.", Charset::UTF_8)),
_fontBold ("bold", false, UTF8String ("Caractère gras de la police de caractères utilisée pour les affichages graphiques. Si true la police est grasse.", Charset::UTF_8)),
_fontItalic ("italic", false, UTF8String ("Caractère italiques de la police de caractères utilisée pour les affichages graphiques. Si true la police est italiques.", Charset::UTF_8)),
_mouseUpZoom ("mouseUpZoom", false, UTF8String ("Action du déplacement vertical de la souris avec le bouton droit pressé. Si false le fait d'aller vers le bas provoque un zoom, alors que d'aller vers le haut diminue la vue. Action inversée si true.", Charset::UTF_8)),
_zoomOnWheel ("zoomOnWheel", false, UTF8String ("Action de la molette de la souris : zoom (true) ou déplacement vers le point focal (false).", Charset::UTF_8)),
_selectionOnTop ("selectionOnTop", false, UTF8String ("True si le fait de sélectionner une entité provoque son affichage au premier plan, false si sa position dans la liste d'entités affichées est invariante.", Charset::UTF_8)),
_selectionColor ("selectionColor", 1., 0., 0., UTF8String ("Couleur utilisée pour représenter les entités sélectionnées.", Charset::UTF_8)),
_selectionPointSize ("selectionPointSize", 8., UTF8String ("Taille utilisée pour représenter les points/vertex sélectionnés.", Charset::UTF_8)),
_selectionLineWidth ("selectionLineWidth", 4., UTF8String ("Epaisseur de trait utilisée pour représenter les entités sélectionnées.", Charset::UTF_8)),
_nextSelectionKey ("nextSelectionKey", "tab", UTF8String ("Touche permettant de sélectionner à la souris l'entité suivante lorsque plusieurs entités se superposent à l'écran (tab pour tabulation, space pour espace). Un parcours en sens inverse est effectué si la touche shift (majuscule) est également pressée.", Charset::UTF_8)),
_pickOnLeftButtonDown ("pickOnLeftButtonDown", false, UTF8String ("True si, en mode sélection, le fait de presser le bouton gauche de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire.", Charset::UTF_8)),
_pickOnRightButtonDown ("pickOnRightButtonDown", false, UTF8String ("True si, en mode sélection, le fait de presser le bouton droit de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire.", Charset::UTF_8)),
_pickOnLeftButtonUp ("pickOnLeftButtonUp", true, UTF8String ("True si, en mode sélection, le fait de relâcher le bouton gauche de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire.", Charset::UTF_8)),
_pickOnRightButtonUp ("pickOnRightButtonUp", false, UTF8String ("True si, en mode sélection, le fait de relâcher le bouton droit de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire.", Charset::UTF_8)),
_marginBoundingBoxPercentage ("marginBoundingBoxPercentage", 0.005, UTF8String ("Pourcentage à appliquer aux échelles pour avoir les marges des boites englobantes, par exemple lors de la mise en évidence d'entités (highlighting). Une échelle est ici une puissance de 10 indiquant l'ordre de grandeur de taille d'une objet ou ensemble d'objets (ex : si xmin = 01. et xmax = 0.3, la taille est 0.2, l'échelle est 1, et la marge cette valeur * 1.", Charset::UTF_8)),
_highlightColor ("highlightColor", 1., 0.65, 0., UTF8String ("Couleur des boites englobantes utilisées pour la mise en évidence d'entités (highlighting).", Charset::UTF_8)),
_highLightWidth ("highLightWidth", 4, UTF8String ("Epaisseur du trait des boites englobantes utilisées pour la mise en évidence d'entités (highlighting).", Charset::UTF_8)),
_highLightBoundinBox ("highLightBoundinBox", false, UTF8String ("true si une boite englobante doit être représentée autour d'une entité mise en évidence (highlighting), false dans le cas contraire.", Charset::UTF_8)),
_useHighLightColor ("useHighLightColor", true, UTF8String ("true si la couleur d'une entité doit être remplaçée par la couleur de mise en évidence lors de la mise en évidence (highlighting), false dans le cas contraire.", Charset::UTF_8)),
_previewColor ("previewColor", 0.17, 0.65, 0.95, UTF8String ("Couleur utilisée pour afficher les éléments de prévisualisation (preview) : axes de rotation, boites englobantes, résultats d'opérations, ...", Charset::UTF_8)),
_previewWidth ("previewWidth", 4, UTF8String ("Epaisseur du trait utilisé pour afficher les éléments de prévisualisation (preview) : axes de rotation, boites englobantes, ...", Charset::UTF_8)),
_previewPointSize ("previewPointSize", 10, UTF8String ("Epaisseur des points utilisée pour afficher les éléments de prévisualisation (preview) : extrémités de segments, ...", Charset::UTF_8)),
_margin ("margin", 0, UTF8String ("Marges (haut/bas/gauche/droite) à l'intérieur des panneaux.\nElles valent 0 par défaut, l'objectif étant de gagner de la place.\nLes augmenter si du texte est illisible.\nNécessite de redémarrer l'application pour prendre effet.", Charset::UTF_8)),
_spacing ("spacing", 0, UTF8String ("Espace séparant 2 composants (bouton, zone de texte, ...) à l'intérieur des panneaux.\nIls valent 5 pixels par défaut, l'objectif étant de gagner de la place. Les augmenter si du texte est illisible.\nNécessite de redémarrer l'application pour prendre effet.", Charset::UTF_8)),
_IDSeizureBackgroundColor ("IDSeizureBackgroundColor", 220/255., 200/255., 240/255., UTF8String ("Composantes R, G, B de la couleur de fond des fenêtre de saisie des identifiants en mode de saisie interactive. Valeurs comprises entre 0 et 1.", Charset::UTF_8)),
_prevalidateSeizure ("prevalidateSeizure", false, UTF8String ("Faut-il procéder à une (pré-)validation des champs de texte de saisie des grandeurs numériques lorsque le panneau perd le focus ?", Charset::UTF_8)),
_cancelClearEntities ("cancelClearEntities", false, UTF8String ("Les noms des entités des panneaux d'opérations doivent ils être effacés lorsque les panneaux sont masqués ? Oui si true, non si false.", Charset::UTF_8)),
_autoUpdateUsesSelection ("autoUpdateUsesSelection", true, UTF8String ("Les panneaux d'opérations doivent ils être initialisés (autant que possible) avec la sélection en cours lors de leur affichage ? Oui si true (défaut), non si false (expérimental).", Charset::UTF_8)),
_showDialogOnCommandError ("showDialogOnCommandError", true, UTF8String ("Faut-il afficher une boite de dialogue d'erreur lorsqu'une commande échoue ? Oui si true, non si false.", Charset::UTF_8)),
_showAmodalDialogOnCommandError ("showAmodalDialogOnCommandError", false, UTF8String ("Faut-il afficher une boite de dialogue d'erreur non modale lorsqu'une commande échoue ? Non si true (risque de désorganiser le bureau courant), oui si false.", Charset::UTF_8)),
_commandNotificationDelay ("commandNotificationDelay", 30, UTF8String ("Durée d'exécution de commande en secondes à partir de laquelle une notification système est envoyée.", Charset::UTF_8)),
_maxIndividualProperties ("maxIndividualProperties", 50, UTF8String ("Nombre maximum d'entités sélectionnées dont on détaille les propriétés individuelles.", Charset::UTF_8)),
_maxCommonProperties ("maxCommonProperties", 50, UTF8String ("Nombre maximum d'entités sélectionnées dont on détaille les propriétés communes.", Charset::UTF_8)),
_logInformations ("informations", true),
_logWarnings ("warnings", true),
_logErrors ("errors", true),
_logTaskOutputs ("taskOutputs", true),
_logTraces_1 ("traces_1", false, UTF8String ("Traces liées aux commandes.", Charset::UTF_8)),
_logTraces_2 ("traces_2", false, UTF8String ("Traces liées aux scripts et pythoneries.", Charset::UTF_8)),
_logTraces_3 ("traces_3", false, UTF8String ("Traces liées aux commandes de type géométrique.", Charset::UTF_8)),
_logTraces_4 ("traces_4", false, UTF8String ("Traces liées aux commandes de type topologique.", Charset::UTF_8)),
_logTraces_5 ("traces_5", false, UTF8String ("Traces liées aux commandes de type maillage.", Charset::UTF_8)),
_logDate ("date", false, UTF8String ("Faut-il faire figurer la date dans les traces (true/false) ?", Charset::UTF_8)),
_logTime ("time", false, UTF8String ("Faut-il faire figurer l'heure dans les traces (true/false) ?", Charset::UTF_8)),
_logThreadID ("threadID", false, UTF8String ("Faut-il faire figurer l'identifiant du thread dans les traces (true/false) ?", Charset::UTF_8)),
_useDisplayList ("useDisplayList", true, UTF8String ("True si les display lists Open GL sont utilisées, false dans le cas contraire.", Charset::UTF_8)),
_xyzCancelRoll ("xyzCancelRoll", false, UTF8String ("Une opération de positionnement de la vue dans un plan xOy, xOz, yOz (touches z, y ou x) doit elle être suivie d'une annulation du roulis (true) ou non (false) ?", Charset::UTF_8)),
_updateRefreshRate ("updateRefreshRate", 1000, UTF8String ("Fréquence de rafraîchissement de la fenêtre graphique lors d'opérations ajouts/suppressions/modifications d'entités (1 rafraîchissement sur n opérations).", Charset::UTF_8)),
_stillFrameRate ("stillFrameRate", 0.0001, UTF8String ("Nombre d'images/seconde souhaité hors interactions.", Charset::UTF_8)),	// Défaut VTK 5.10
_desiredFrameRate ("desiredFrameRate", 15, UTF8String ("Nombre d'images/seconde durant les interactions.", Charset::UTF_8)),	// Défaut VTK 5.10		
_background ("background", 0., 0., 0., UTF8String ("Composantes R, G, B de la couleur de fond de la fenêtre graphique. Valeurs comprises entre 0 et 1.", Charset::UTF_8)),
_multipleLighting ("multipleLighting", true, UTF8String ("Faut-il utiliser (true) ou non (false) plusieurs sources de lumière ?", Charset::UTF_8)),
_displayTrihedron ("displayTrihedron", false, UTF8String ("Faut-il afficher (true) ou non (false) le trièdre dans la fenêtre graphique ?", Charset::UTF_8)),
_displayFocalPoint ("displayFocalPoint", false, UTF8String ("Faut-il afficher (true) ou non (false) la croix matérialisant le point focal dans la fenêtre graphique ?", Charset::UTF_8)),
_graphicalWindowWidth ("graphicalWindowWidth", 800, UTF8String ("Largeur souhaitée de la fenêtre graphique.", Charset::UTF_8)),
_graphicalWindowHeight ("graphicalWindowHeight", 600, UTF8String ("Hauteur souhaitée de la fenêtre graphique.", Charset::UTF_8)),
_graphicalWindowFixedSize ("graphicalWindowFixedSize", false, UTF8String ("True si la taille de la fenêtre graphique doit être fixe, false dans le cas contraire.", Charset::UTF_8)),
_automaticSort ("automaticSort", true, UTF8String ("True si les noms de groupes sont automatiquement triés, false dans le cas contraire.", Charset::UTF_8)),
_sortType ("sortType", "typeAscendingName", UTF8String ("typeDescendingName pour un ordre alphabétique descendant, typeAscendingName pour un ordre alphabétique ascendant.", Charset::UTF_8)),
_multiLevelGroupsView ("multiLevelGroupsView", false, UTF8String ("True si les groupes sont classés par niveau, false dans le cas contraire.", Charset::UTF_8)),
_catchStdOutputs ("catchStdOutputs", true, UTF8String ("True si les flux standards (stdout/stderr) des commandes et fichiers scripts doivent être récupérées pour être traitées comme des logs, false dans le cas contraire.", Charset::UTF_8)),
_recentScriptCapacity ("recentScriptCapacity", 10, UTF8String ("Nombre de scripts ouverts conservés en mémore et proposés dans le menu Session.Scripts Magix 3D récents ...", Charset::UTF_8)),
_scriptsCharset ("scriptCharset", "UTF-8", UTF8String ("Jeu de caractères utilisé par la génération de scripts (ISO8859, UTF8).\nNécessite de redémarrer l'application pour prendre effet.", Charset::UTF_8)),
_saveGuiState ("saveGuiState", true, UTF8String ("True si l'état de l'IHM (position, taille, organisation des panneaux, ...) doit être automatiquement enregistré en fin de session, false dans le cas contraire.", Charset::UTF_8)),
_loadGuiState ("loadGuiState", true, UTF8String ("True si l'état de l'IHM (position, taille, organisation des panneaux, ...) enregistré en configuration doit être appliqué au lancement de Magix 3D.", Charset::UTF_8)),
_geomDestroyOnHide ("destroyOnHide", false, UTF8String ("True si la représentation graphique des entités géométriques doit être détruite lorsque ces entités ne sont plus affichées, false dans le cas contraire.", Charset::UTF_8)),
_topoDestroyOnHide ("destroyOnHide", false, UTF8String ("True si la représentation graphique des entités topologiques doit être détruite lorsque ces entités ne sont plus affichées, false dans le cas contraire.", Charset::UTF_8)),
_meshDestroyOnHide ("destroyOnHide", true, UTF8String ("True si la représentation graphique des entités de maillage doit être détruite lorsque ces entités ne sont plus affichées, false dans le cas contraire.", Charset::UTF_8)),


_organisation ("CEA.DAM.DSSI"),
_softwareName ("Magix3D"),

_scripts ( )
	
{
	Resources::_instance	= this;
}	// Resources::Resources


Resources::Resources (const Resources&)
: _displayUpdatesErrors (true),
_defaultConfigURL (""),
_userConfigURL (""),
_userConfigPatchsURL (""),
_userLastVersionURL (""),
_mgx3DRootSectionName ("Magix3D"),
_confirmQuitAction ("confirmQuitAction", true, UTF8String ("true si une boite de dialogue de confirmation doit être affichée lorsque l'utilisateur met fin à Magix3D, false dans le cas contraire.")),
_allowThreadedCommandTasks ("allowThreadedCommandTasks", true, UTF8String ("true si une commande est autorisée à être décomposée en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_allowThreadedEdgePreMeshTasks ("allowThreadedEdgePreMeshTasks", true, UTF8String ("true si le prémaillage des arêtes peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_allowThreadedFacePreMeshTasks ("allowThreadedFacePreMeshTasks", true, UTF8String ("true si le prémaillage des faces peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_allowThreadedBlockPreMeshTasks ("allowThreadedBlockPreMeshTasks", true, UTF8String ("true si le prémaillage des blocs peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
_displayScriptOutputs ("displayScriptOutputs", true, UTF8String ("true si le programme doit afficher les sorties des commandes script, false dans le cas contraire.")),
_memorizeEdgePreMesh ("memorizeEdgePreMesh", true, UTF8String ("true si le programme doit mémoriser le prémaillage des arêtes, false dans le cas contraire.")),
_fontFamily ("fontFamily", "Arial", "Police de caractères utilisée pour les affichages graphiques. Valeurs possibles : Arial, Times, Courier."),
_fontSize ("fontSize", 12, "Taille de la police de caractères utilisée pour les affichages graphiques."),
_fontBold ("bold", false, "Caractère gras de la police de caractères utilisée pour les affichages graphiques. Si true la police est grasse."),
_fontItalic ("italic", false, "Caractère italiques de la police de caractères utilisée pour les affichages graphiques. Si true la police est italiques."),
_mouseUpZoom ("mouseUpZoom", false, "Action du déplacement vertical de la souris avec le bouton droit pressé. Si false le fait d'aller vers le bas provoque un zoom, alors que d'aller vers le haut diminue la vue. Action inversée si true."),
_zoomOnWheel ("zoomOnWheel", false, "Action de la molette de la souris : zoom (true) ou déplacement vers le point focal (false)."),
_selectionOnTop ("selectionOnTop", false, "True si le fait de sélectionner une entité provoque son affichage au premier plan, false si sa position dans la liste d'entités affichées est invariante."),
_selectionColor ("selectionColor", 1., 0., 0., "Couleur utilisée pour représenter les entités sélectionnées."),
_selectionPointSize ("selectionPointSize", 8., "Taille utilisée pour représenter les points/vertex sélectionnés."),
_selectionLineWidth ("selectionLineWidth", 4., "Epaisseur de trait utilisée pour représenter les entités sélectionnées."),
_nextSelectionKey ("nextSelectionKey", "tab", "Touche permettant de sélectionner à la souris l'entité suivante lorsque plusieurs entités se superposent à l'écran (tab pour tabulation, space pour espace). Un parcours en sens inverse est effectué si la touche shift (majuscule) est également pressée."),
_pickOnLeftButtonDown ("pickOnLeftButtonDown", false, "True si, en mode sélection, le fait de presser le bouton gauche de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire."),
_pickOnRightButtonDown ("pickOnRightButtonDown", false, "True si, en mode sélection, le fait de presser le bouton droit de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire."),
_pickOnLeftButtonUp ("pickOnLeftButtonUp", true, "True si, en mode sélection, le fait de relâcher le bouton gauche de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire."),
_pickOnRightButtonUp ("pickOnRightButtonUp", false, "True si, en mode sélection, le fait de relâcher le bouton droit de la souris doit déclencher une opération de pointage (picking), false dans le cas contraire."),
_marginBoundingBoxPercentage ("marginBoundingBoxPercentage", 0.005, "Pourcentage à appliquer aux échelles pour avoir les marges des boites englobantes, par exemple lors de la mise en évidence d'entités (highlighting). Une échelle est ici une puissance de 10 indiquant l'ordre de grandeur de taille d'une objet ou ensemble d'objets (ex : si xmin = 01. et xmax = 0.3, la taille est 0.2, l'échelle est 1, et la marge cette valeur * 1."),
_highlightColor ("highlightColor", 1., 0.65, 0., "Couleur des boites englobantes utilisées pour la mise en évidence d'entités (highlighting)."),
_highLightWidth ("highLightWidth", 4, "Epaisseur du trait des boites englobantes utilisées pour la mise en évidence d'entités (highlighting)."),
_highLightBoundinBox ("highLightBoundinBox", false, "true si une boite englobante doit être représentée autour d'une entité mise en évidence (highlighting), false dans le cas contraire."),
_useHighLightColor ("useHighLightColor", true, "true si la couleur d'une entité doit être remplaçée par la couleur de mise en évidence lors de la mise en évidence (highlighting), false dans le cas contraire."),
_previewColor ("previewColor", 0.17, 0.65, 0.95, "Couleur utilisée pour afficher les éléments de prévisualisation (preview) : axes de rotation, boites englobantes, résultats d'opérations, ..."),
_previewWidth ("previewWidth", 4, "Epaisseur du trait utilisé pour afficher les éléments de prévisualisation (preview) : axes de rotation, boites englobantes, ..."),
_previewPointSize ("previewPointSize", 10, "Epaisseur des points utilisée pour afficher les éléments de prévisualisation (preview) : extrémités de segments, ..."),
_margin ("margin", 0, "Marges (haut/bas/gauche/droite) à l'intérieur des panneaux.\nElles valent 0 par défaut, l'objectif étant de gagner de la place.\nLes augmenter si du texte est illisible.\nNécessite de redémarrer l'application pour prendre effet."),
_spacing ("spacing", 0, "Espace séparant 2 composants (bouton, zone de texte, ...) à l'intérieur des panneaux.\nIls valent 5 pixels par défaut, l'objectif étant de gagner de la place. Les augmenter si du texte est illisible.\nNécessite de redémarrer l'application pour prendre effet."),
_IDSeizureBackgroundColor ("IDSeizureBackgroundColor", 220/255., 200/255., 240/255., "Composantes R, G, B de la couleur de fond des fenêtre de saisie des identifiants en mode de saisie interactive. Valeurs comprises entre 0 et 1."),
_prevalidateSeizure ("prevalidateSeizure", false, "Faut-il procéder à une (pré-)validation des champs de texte de saisie des grandeurs numériques lorsque le panneau perd le focus ?"),
_cancelClearEntities ("cancelClearEntities", "false", "Les noms des entités des panneaux d'opérations doivent ils être effacés lorsque les panneaux sont masqués ? Oui si true, non si false."),
_autoUpdateUsesSelection ("autoUpdateUsesSelection", "true", "Les panneaux d'opérations doivent ils être initialisés (autant que possible) avec la sélection en cours lors de leur affichage ? Oui si true, non si false."),
_showDialogOnCommandError ("showDialogOnCommandError", "true", "Faut-il afficher une boite de dialogue d'erreur lorsqu'une commande échoue ? Oui si true, non si false."),
_showAmodalDialogOnCommandError ("showAmodalDialogOnCommandError", false, "Faut-il afficher une boite de dialogue d'erreur non modale lorsqu'une commande échoue ? Oui si true (risque de désorganiser le bureau courant), non si false."),
_commandNotificationDelay ("commandNotificationDelay", 20, "Durée d'exécution de commande en secondes à partir de laquelle une notification système est envoyée."),
_maxIndividualProperties ("maxIndividualProperties", 50, "Nombre maximum d'entités sélectionnées dont on détaille les propriétés individuelles."),
_maxCommonProperties ("maxCommonProperties", 50, "Nombre maximum d'entités sélectionnées dont on détaille les propriétés communes."),
_logInformations ("informations", true),
_logWarnings ("warnings", true),
_logErrors ("errors", true),
_logTaskOutputs ("taskOutputs", true),
_logTraces_1 ("traces_1", false, "Traces liées aux commandes."),
_logTraces_2 ("traces_2", false, "Traces liées aux scripts et pythoneries."),
_logTraces_3 ("traces_3", false, "Traces liées aux commandes de type géométrique."),
_logTraces_4 ("traces_4", false, "Traces liées aux commandes de type topologique."),
_logTraces_5 ("traces_5", false, "Traces liées aux commandes de type maillage."),
_logDate ("date", false, "Faut-il faire figurer la date dans les traces (true/false) ?"),
_logTime ("time", false, "Faut-il faire figurer l'heure dans les traces (true/false) ?"),
_logThreadID ("threadID", false, "Faut-il faire figurer l'identifiant du thread dans les traces (true/false) ?"),
_useDisplayList ("useDisplayList", true, "True si les display lists Open GL sont utilisées, false dans le cas contraire."),
_xyzCancelRoll ("xyzCancelRoll", false, "Une opération de positionnement de la vue dans un plan xOy, xOz, yOz (touches z, y ou x) doit elle être suivie d'une annulation du roulis (true) ou non (false) ?"),
_updateRefreshRate ("updateRefreshRate", 100, "Fréquence de rafraîchissement de la fenêtre graphique lors d'opérations ajouts/suppressions/modifications d'entités (1 rafraîchissement sur n opérations)."),
_stillFrameRate ("stillFrameRate", 0.0001, "Nombre d'images/seconde souhaité hors interactions."),	// Défaut VTK 5.10
_desiredFrameRate ("desiredFrameRate", 15, "Nombre d'images/seconde durant les interactions."),	// Défaut VTK 5.10		
_background ("background", 0., 0., 0., "Composantes R, G, B de la couleur de fond de la fenêtre graphique. Valeurs comprises entre 0 et 1."),
_multipleLighting ("multipleLighting", true, "Faut-il utiliser (true) ou non (false) plusieurs sources de lumière ?"),
_displayTrihedron ("displayTrihedron", false, "Faut-il afficher (true) ou non (false) le trièdre dans la fenêtre graphique ?"),
_displayFocalPoint ("displayFocalPoint", false, "Faut-il afficher (true) ou non (false) la croix matérialisant le point focal dans la fenêtre graphique ?"),
_graphicalWindowWidth ("graphicalWindowWidth", 800, "Largeur souhaitée de la fenêtre graphique."),
_graphicalWindowHeight ("graphicalWindowHeight", 600, "Hauteur souhaitée de la fenêtre graphique."),
_graphicalWindowFixedSize ("graphicalWindowFixedSize", false, "True si la taille de la fenêtre graphique doit être fixe, false dans le cas contraire."),
_automaticSort ("automaticSort", true, "True si les noms de groupes sont automatiquement triés, false dans le cas contraire."),
_sortType ("sortType", "typeAscendingName", "typeDescendingName pour un ordre alphabétique descendant, typeAscendingName pour un ordre alphabétique ascendant."),
_multiLevelGroupsView ("multiLevelGroupsView", false, "True si les groupes sont classés par niveau, false dans le cas contraire."),
_catchStdOutputs ("catchStdOutputs", true, "True si les flux standards (stdout/stderr) des commandes et fichiers scripts doivent être récupérées pour être traitées comme des logs, false dans le cas contraire."),
_recentScriptCapacity ("recentScriptCapacity", 10, "Nombre de scripts ouverts conservés en mémore et proposés dans le menu Session.Scripts Magix 3D récents ..."),
_scriptsCharset ("scriptCharset", "UTF-8", "Jeu de caractères utilisé par la génération de scripts (ISO8859, UTF8).\nNécessite de redémarrer l'application pour prendre effet."),
_saveGuiState ("saveGuiState", true, "True si l'état de l'IHM (position, taille, organisation des panneaux, ...) doit être automatiquement enregistré en fin de session, false dans le cas contraire."),
_loadGuiState ("loadGuiState", true, "True si l'état de l'IHM (position, taille, organisation des panneaux, ...) enregistré en configuration doit être appliqué au lancement de Magix 3D."),
_geomDestroyOnHide ("destroyOnHide", false, "True si la représentation graphique des entités géométriques doit être détruite lorsque ces entités ne sont plus affichées, false dans le cas contraire."),
_topoDestroyOnHide ("destroyOnHide", false, "True si la représentation graphique des entités topologiques doit être détruite lorsque ces entités ne sont plus affichées, false dans le cas contraire."),
_meshDestroyOnHide ("destroyOnHide", true, "True si la représentation graphique des entités de maillage doit être détruite lorsque ces entités ne sont plus affichées, false dans le cas contraire."),


_organisation ("CEA.DAM.DSSI"),
_softwareName ("Magix3D"),

_scripts ( )
{
	MGX_FORBIDDEN ("Resources copy constructor is not allowed.");
}	// Resources::Resources (const Resources&)


Resources& Resources::operator = (const Resources&)
{
	MGX_FORBIDDEN ("Resources assignment operator is not allowed.");
	return *this;
}	// Resources::Resources (const Resources&)


Resources::~Resources ( )
{
	_instance	= 0;
}	// Resources::~Resources


Resources& Resources::instance ( )
{
	if (0 == _instance)
		_instance	= new Resources ( );
		
	return *_instance;
}	// Resources::instance


}	// namespace Internal

}	// namespace Mgx3D
