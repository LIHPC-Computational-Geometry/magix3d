/*----------------------------------------------------------------------------*/
/*
 * \file InternalPreferences.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11 sept. 2014
 *
 *  Stockage des préférences dites internes, car accessibles depuis le Context
 *  ou depuis une entité de bas niveau tel que Block, CoFace ou CoEdge
 */
/*----------------------------------------------------------------------------*/
#ifndef INTERNALPREFERENCES_H_
#define INTERNALPREFERENCES_H_
/*----------------------------------------------------------------------------*/
#include <PrefsCore/BoolNamedValue.h>
#include <PrefsCore/ColorNamedValue.h>
#include <PrefsCore/DoubleNamedValue.h>
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
class InternalPreferences{
public:
/** Faut-il représenter les Faces topologiques ?
 *
 *  Faux par défaut, on ne représente habituellement que les CoFaces
 */
Preferences::BoolNamedValue			_displayFace;

/** Faut-il représenter les Edges topologiques ?
 *
 *  Faux par défaut, on ne représente habituellement que les CoEdges
 */
Preferences::BoolNamedValue			_displayEdge;

/** La demande d'affichage d'un groupe provoquera-t-il celui des entités du même groupe et de dimensions inférieures ?
 *
 *  Vrai par défaut
 */
Preferences::BoolNamedValue	        _propagateDownEntityShow;

/**
 * L'écart entre une couleur et la couleur de fond est il évalué dans l'espace
 * de couleur RGB ? Si <I>true</I> l'évalutation se fait selon les composantes
 * RGB, si <I>false</I> selon les composantes HSV.
 */
Preferences::BoolNamedValue			_useRgbColorComparison;

/**
 * Ecart relatif accepté entre une couleur et la couleur de fond. Pourcentage,
 * exprimé entre 0 et 1.
 */
Preferences::DoubleNamedValue		_backgroundRelDifference;

/**
 * Couleur des entités topologiques sans associations, bleu par défaut
 */
Preferences::ColorNamedValue			_topoColorWithoutProj;

/**
 * Couleur des entités topologiques avec une association vers une entité de dimension 0, marron par défaut
 */
Preferences::ColorNamedValue			_topoColorWith0DProj;

/**
 * Couleur des entités topologiques avec une association vers une entité de dimension 1, vert par défaut
 */
Preferences::ColorNamedValue			_topoColorWith1DProj;

/**
 * Couleur des entités topologiques avec une association vers une entité de dimension 2, blanc par défaut
 */
Preferences::ColorNamedValue			_topoColorWith2DProj;

/**
 * Couleur des entités topologiques avec une association vers une entité de dimension 3, blanc par défaut
 */
Preferences::ColorNamedValue			_topoColorWith3DProj;

/** chargement des paramètres 
 */
static void loadPreferences(Preferences::Section&);

/** sauvegarde des paramètres 
 */
static void savePreferences(Preferences::Section&);


static InternalPreferences& instance ( );


private:

static InternalPreferences*	_instance;

InternalPreferences ( );

~InternalPreferences()
{MGX_FORBIDDEN ("Destructeur interdit")}

InternalPreferences(const InternalPreferences&);

InternalPreferences& operator =(const InternalPreferences&)
{MGX_FORBIDDEN ("Copie interdite");return *this;}



};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INTERNALPREFERENCES_H_ */
