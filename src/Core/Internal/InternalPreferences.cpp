/*----------------------------------------------------------------------------*/
/*
 * \file InternalPreferences.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11 sept. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/InternalPreferences.h"
#include <PrefsCore/PreferencesHelper.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/


InternalPreferences*	InternalPreferences::_instance	= 0;

InternalPreferences::InternalPreferences ( )
	:
_displayFace("displayFace", false, "true si l'on souhaite voir les Faces en plus des CoFaces"),
_displayEdge("displayEdge", false, "true si l'on souhaite voir les Edges en plus des CoEdges"),
_propagateDownEntityShow ("propagateDownEntityShow", true, "Si true la demande d'affichage d'un groupe provoquera celui des entités du même groupe et de dimensions inférieures."),
_useRgbColorComparison ("useRgbColorComparison", true, "Si true la comparaison de couleurs se fait dans l'espace RGB. Si false elle se fait dans l'espace HSV."),
_backgroundRelDifference ("backgroundRelDifference", 0.05, "Ecart relatif accepté entre la couleur de fond et une couleur d'entité (ex : 0.2 pour 20 pourcents)."),
_topoColorWithoutProj  ("topoColorWithoutProj", 0., 1., 1., "Composantes R, G, B de la couleur des entités topologiques sans associations. Valeurs comprises entre 0 et 1."),
_topoColorWith0DProj ("topoColorWith0DProj", 0.8, 0.4, 0.4, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 0. Valeurs comprises entre 0 et 1."),
_topoColorWith1DProj ("topoColorWith1DProj", 0.0, 1.0, 0.0, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 1. Valeurs comprises entre 0 et 1."),
_topoColorWith2DProj ("topoColorWith2DProj", 1.0, 1.0, 1.0, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 2. Valeurs comprises entre 0 et 1."),
_topoColorWith3DProj ("topoColorWith3DProj", 1.0, 1.0, 1.0, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 3. Valeurs comprises entre 0 et 1.")
{
}


InternalPreferences::InternalPreferences (const InternalPreferences&)
	:
_displayFace("displayFace", false, "true si l'on souhaite voir les Faces en plus des CoFaces"),
_displayEdge("displayEdge", false, "true si l'on souhaite voir les Edges en plus des CoEdges"),
_propagateDownEntityShow ("propagateDownEntityShow", true, "Si true la demande d'affichage d'un groupe provoquera celui des entités du même groupe et de dimensions inférieures."),
_useRgbColorComparison ("useRgbColorComparison", true, "Si true la comparaison de couleurs se fait dans l'espace RGB. Si false elle se fait dans l'espace HSV."),
_backgroundRelDifference ("backgroundRelDifference", 0.05, "Ecart relatif accepté entre la couleur de fond et une couleur d'entité (ex : 0.2 pour 20 pourcents)."),
_topoColorWithoutProj  ("topoColorWithoutProj", 0., 1., 1., "Composantes R, G, B de la couleur des entités topologiques sans associations. Valeurs comprises entre 0 et 1."),
_topoColorWith0DProj ("topoColorWith0DProj", 0.8, 0.4, 0.4, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 0. Valeurs comprises entre 0 et 1."),
_topoColorWith1DProj ("topoColorWith1DProj", 0.0, 1.0, 0.0, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 1. Valeurs comprises entre 0 et 1."),
_topoColorWith2DProj ("topoColorWith2DProj", 1.0, 1.0, 1.0, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 2. Valeurs comprises entre 0 et 1."),
_topoColorWith3DProj ("topoColorWith3DProj", 1.0, 1.0, 1.0, "Composantes R, G, B de la couleur des entités topologiques avec une association de dimension 3. Valeurs comprises entre 0 et 1.")
{
	MGX_FORBIDDEN ("Constructeur de copie interdit");
}


InternalPreferences& InternalPreferences::instance ( )
{
	if (0 == _instance)
		_instance	= new InternalPreferences ( );
		
	return *_instance;
}


void InternalPreferences::loadPreferences(Preferences::Section& section)
{
	// Préférences affichage :
	try
	{
		Preferences::PreferencesHelper::getBoolean (section, instance ( )._useRgbColorComparison);
		Preferences::PreferencesHelper::getDouble (section, instance ( )._backgroundRelDifference);
		Preferences::PreferencesHelper::getBoolean (section, instance ( )._propagateDownEntityShow);
	}
	catch (...)
	{
	}

	// Préférences topologie :
	try
	{
		Preferences::Section&	topoSection	= section.getSection ("topoEntities");

		Preferences::PreferencesHelper::getBoolean (topoSection, instance ( )._displayFace);
		Preferences::PreferencesHelper::getBoolean (topoSection, instance ( )._displayEdge);

		// Code couleur des entités topologiques :
		Preferences::PreferencesHelper::getColor (topoSection, instance ( )._topoColorWithoutProj);
		Preferences::PreferencesHelper::getColor (topoSection, instance ( )._topoColorWith0DProj);
		Preferences::PreferencesHelper::getColor (topoSection, instance ( )._topoColorWith1DProj);
		Preferences::PreferencesHelper::getColor (topoSection, instance ( )._topoColorWith2DProj);
		Preferences::PreferencesHelper::getColor (topoSection, instance ( )._topoColorWith3DProj);
	}
	catch (...)
	{
	}
}

void InternalPreferences::savePreferences(Preferences::Section& section)
{
	// Préférences affichage :
	try
	{
		Preferences::PreferencesHelper::updateBoolean (section, instance ( )._useRgbColorComparison);
		Preferences::PreferencesHelper::updateDouble (section, instance ( )._backgroundRelDifference);
		Preferences::PreferencesHelper::updateBoolean (section, instance ( )._propagateDownEntityShow);
	}
	catch (...)
	{
	}

	// Préférences topologie :
	try
	{
		Preferences::Section&	topoSection	= section.getSection ("topoEntities");

		Preferences::PreferencesHelper::updateBoolean (topoSection, instance ( )._displayFace);
		Preferences::PreferencesHelper::updateBoolean (topoSection, instance ( )._displayEdge);

		// Code couleur des entités topologiques :
		Preferences::PreferencesHelper::updateColor (topoSection, instance ( )._topoColorWithoutProj);
		Preferences::PreferencesHelper::updateColor (topoSection, instance ( )._topoColorWith0DProj);
		Preferences::PreferencesHelper::updateColor (topoSection, instance ( )._topoColorWith1DProj);
		Preferences::PreferencesHelper::updateColor (topoSection, instance ( )._topoColorWith2DProj);
		Preferences::PreferencesHelper::updateColor (topoSection, instance ( )._topoColorWith3DProj);
	}
	catch (...)
	{
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
