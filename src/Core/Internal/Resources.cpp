/**
 * \file		Resources.cpp
 * \author		Charles PIGNEROL
 * \date		12/02/2019
 */

#include "Internal/Resources.h"
#include "Utils/Common.h"


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
_cancelClearEntities ("cancelClearEntities", false, "Les noms des entités des panneaux d'opérations doivent ils être effacés lorsque les panneaux sont masqués ? Oui si true, non si false."),
_autoUpdateUsesSelection ("autoUpdateUsesSelection", true, "Les panneaux d'opérations doivent ils être initialisés (autant que possible) avec la sélection en cours lors de leur affichage ? Oui si true, non si false."),
_showDialogOnCommandError ("showDialogOnCommandError", true, "Faut-il afficher une boite de dialogue d'erreur lorsqu'une commande échoue ? Oui si true, non si false."),
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
	Resources::_instance	= this;
}	// Resources::Resources


Resources::Resources (const Resources&)
: _displayUpdatesErrors (true),
_defaultConfigURL (""),
_userConfigURL (""),
_userConfigPatchsURL (""),
_userLastVersionURL (""),
_mgx3DRootSectionName ("Magix3D"),
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
