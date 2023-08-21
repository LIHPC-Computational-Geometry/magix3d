/**
 * \file		VTKConfiguration.h
 * \author		Charles PIGNEROL
 * \date		19/10/2012
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKConfiguration.h"

#include <Utils/Common.h>

#include <TkUtil/Exception.h>
#include <PrefsCore/PreferencesHelper.h>


using namespace TkUtil;
using namespace Preferences;
using namespace Mgx3D;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace QtVtkComponents {

/*----------------------------------------------------------------------------*/


// =========================================================================
//                           VTKConfiguration
// =========================================================================

VTKConfiguration*	VTKConfiguration::_instance	= 0;



VTKConfiguration::VTKConfiguration ( )
:
_pickingComment ("Section contenant les paramètres de sélection à la souris. La tolérance appliquée, lors des opérations de sélection, est définie comme étant la fraction de la diagonale de la fenêtre de rendu 3D."),
/*
 * Expérience LEM : tolérance de 1E-3 pour les noeuds et arêtes, en utilisant
 * les boites englobantes, et 1E-4 pour les polygones et polyèdres.
 *
 * Expérience Magix 3D : pour les noeuds 1E-2 semble mieux.
 */
_0DPickerTolerance ("0DPickerTolerance", 0.01, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 0D par boite englobante."),
_1DPickerTolerance ("1DPickerTolerance", 0.001, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 1D par boite englobante."),
_2DPickerTolerance ("2DPickerTolerance", 0.0001, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 2D par boite englobante."),
_3DPickerTolerance ("3DPickerTolerance", 0.0001, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 3D par boite englobante."),
_cellPickerTolerance ("cellPickerTolerance", 0.005, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités par proximité directe.")

{
}	// VTKConfiguration::VTKConfiguration


VTKConfiguration::VTKConfiguration (const VTKConfiguration&)
:
_pickingComment ("Section contenant les paramètres de sélection à la souris. La tolérance appliquée, lors des opérations de sélection, est définie comme étant la fraction de la diagonale de la fenêtre de rendu 3D."),
_0DPickerTolerance ("0DPickerTolerance", 0.01, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 0D par boite englobante."),
_1DPickerTolerance ("1DPickerTolerance", 0.001, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 1D par boite englobante."),
_2DPickerTolerance ("2DPickerTolerance", 0.0001, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 2D par boite englobante."),
_3DPickerTolerance ("3DPickerTolerance", 0.0001, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités 3D par boite englobante."),
_cellPickerTolerance ("cellPickerTolerance", 0.005, "Tolérance appliquée lors de la sélection par pointage à la souris d'entités par proximité directe.")

{
    MGX_FORBIDDEN("VTKConfiguration::VTKConfiguration (const VTKConfiguration&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


VTKConfiguration& VTKConfiguration::operator = (const VTKConfiguration&)
{
    MGX_FORBIDDEN("VTKConfiguration::operator = (const VTKConfiguration&) forbidden.");
	return *this;
}	// VTKConfiguration::operator = (const VTKConfiguration&)


VTKConfiguration::~VTKConfiguration ( )
{
    MGX_FORBIDDEN("VTKConfiguration::~VTKConfiguration ( ) forbidden.");
}	// VTKConfiguration::~VTKConfiguration


VTKConfiguration& VTKConfiguration::instance ( )
{
	if (0 == _instance)
		_instance	= new VTKConfiguration ( );
		
	return *_instance;
}	// VTKConfiguration::instance


void VTKConfiguration::applyConfiguration (const Section& mainSection)
{
	try
	{
		Section&	pickingSection	= mainSection.getSection ("picking");
		if (0 != pickingSection.getComment ( ).length ( ))
			VTKConfiguration::instance ( )._pickingComment	= pickingSection.getComment ( ).iso ( );
		PreferencesHelper::getDouble (pickingSection, VTKConfiguration::instance ( )._0DPickerTolerance);
		PreferencesHelper::getDouble (pickingSection, VTKConfiguration::instance ( )._1DPickerTolerance);
		PreferencesHelper::getDouble (pickingSection, VTKConfiguration::instance ( )._2DPickerTolerance);
		PreferencesHelper::getDouble (pickingSection, VTKConfiguration::instance ( )._3DPickerTolerance);
		PreferencesHelper::getDouble (pickingSection, VTKConfiguration::instance ( )._cellPickerTolerance);
	}
	catch (...)
	{
	}
}	// VTKConfiguration::applyConfiguration


void VTKConfiguration::saveConfiguration (Preferences::Section& mainSection)
{
	Section&    pickingSection  = PreferencesHelper::getSection (mainSection, "picking");
	pickingSection.setComment (VTKConfiguration::instance ( )._pickingComment);
	PreferencesHelper::updateDouble (pickingSection, VTKConfiguration::instance ( )._0DPickerTolerance);
	PreferencesHelper::updateDouble (pickingSection, VTKConfiguration::instance ( )._1DPickerTolerance);
	PreferencesHelper::updateDouble (pickingSection, VTKConfiguration::instance ( )._2DPickerTolerance);
	PreferencesHelper::updateDouble (pickingSection, VTKConfiguration::instance ( )._3DPickerTolerance);
	PreferencesHelper::updateDouble (pickingSection, VTKConfiguration::instance ( )._cellPickerTolerance);
}	// VTKConfiguration::saveConfiguration


/*----------------------------------------------------------------------------*/
} // end namespace QtVtkComponents

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/

