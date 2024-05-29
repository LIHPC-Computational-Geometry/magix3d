/*----------------------------------------------------------------------------*/
/*
 * CommandImportIGES.h
 *
 *  Created on: 21 juin 2013
 *      Author: ledouxf
 */
 /*----------------------------------------------------------------------------*/

#ifndef COMMANDIMPORTIGES_H_
#define COMMANDIMPORTIGES_H_

/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Unit.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
class ImportIGESImplementation;
/*----------------------------------------------------------------------------*/
/** \class CommandImportIGES
 *  \brief Commande permettant d'importer le contenu d'un fichier au format
 *         IGES
 */
/*----------------------------------------------------------------------------*/
class CommandImportIGES: public CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param splitCompoundCurves décompose les courbes composites en de multiples courbes
     */
    CommandImportIGES(Internal::Context& c, const std::string& n, const bool splitCompoundCurves);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandImportIGES();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();


protected:

    /* fichier à importer*/
    std::string  m_filename;

    /* objet gérant l'opération de fusion*/
    ImportIGESImplementation* m_impl;
};


/**
 * Assistant de récupération du contenu de l'entête d'un fichier IGES.
 * Specs récupérés dans iges5-3_fordownload.pdf @USPRO 1996
 */
struct IGESHeader
{
	/**
	 * Constructeurs, destructeur. RAS.
	 */
	IGESHeader ( );
	IGESHeader (const IGESHeader&);
	IGESHeader& operator = (const IGESHeader&);
	~IGESHeader ( )
	{}

	/**
	 * Lit l'entête du fichier dont le path est donné en argument. Si <I>verbose</I> vaut <I>true</I>
	 * ajoute alors à messages les informations relatives à l'avancée de la lecture.
	 * @exception	Une exception est levée en cas d'erreur ou non conformité aux spécifications.
	 */
	 void read (const std::string& filepath, TkUtil::UTF8String& messages, bool verbose = false);

	 /**
	  * @return		L'unité de longueur <I>Magix3D<I> correspondante.
	  * @exception	Une exception est levée s'il n'y a pas d'unité de longueur <I>Magix3D</I> correspondante et si
	  * 			<I>couldRaises</I> vaut <I>true</I>.
	  */
	  Mgx3D::Utils::Unit::lengthUnit getMgxUnit (bool couldRaises = true) const;

	 /**
	  * Les paramètres de l'entête, dans l'ordre de lecture
	  */
	std::string     parameterDelimiter, recordDelimiter, productIdentification, fileName, nativeSystemId, preprocessorVersion;
	long            intBitNum, floatPowerMax, floatTenPowerMax, doublePowerMax, doubleTenPowerMax;
	std::string     receivingSystemID;
	double          modelSpaceScale;
	long            unitsFlag;
	std::string     unitsName;
	long            weightGradationsMaxLines;
	double          weightMaxWidth;
	std::string     fileDateCreation;
	double          resolutionMin, maxCoordinateValue;
	std::string     authorName, authorOrganization;
	long            specsVersion, draftVersion;
	std::string     fileDateModification, applicationData;
};  // struct IGESHeader
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDIMPORTIGES_H_ */
/*----------------------------------------------------------------------------*/

