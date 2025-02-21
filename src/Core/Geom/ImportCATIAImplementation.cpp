/*----------------------------------------------------------------------------*/
/*
 * ImportCATIAImplementation.cpp
 *
 *  Created on: 15 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <map>
#include <set>
#include <algorithm>
/*----------------------------------------------------------------------------*/
#include "Geom/ImportCATIAImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"

#include "Group/GroupManager.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"

#include "Internal/InfoCommand.h"

#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <mgx_config.h>
/*----------------------------------------------------------------------------*/
#ifdef USE_DKOC
// fichiers d'en-tête de DataKit pour le lecteur Catia
#include <DKOC/OCCTopoDSRead.h>
#endif
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportCATIAImplementation::
ImportCATIAImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
		const std::string& n)
: GeomImport(c,icmd,n,true)
{
}
/*----------------------------------------------------------------------------*/
ImportCATIAImplementation::~ImportCATIAImplementation()
{}
/*----------------------------------------------------------------------------*/
void ImportCATIAImplementation::readFile()
{
#ifdef USE_DKOC

    //recuperation de la licence du lecteur CATIA
    OCCTopoDSRead_set_ID_client(DKOC_LICENCE);

    std::cout<<"Importation du fichier Catia : "<<m_filename<<std::endl;
    int CatalogItem = 0;
    m_importedShapes.resize(1);
    int stat = OCCTopoDS_Read ((char*)(m_filename.c_str()), m_importedShapes[0], CatalogItem);


    // gestion du retour d'erreur DKOC
    switch (stat){
    case 0:
    	// no error
    	break;
    case 1:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: arret utilisateur", TkUtil::Charset::UTF_8));
    	break;
    case -1:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: allocation", TkUtil::Charset::UTF_8));
    	break;
    case -2:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: ouverture de fichier", TkUtil::Charset::UTF_8));
    	break;
    case -4:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: licence", TkUtil::Charset::UTF_8));
    	break;
    case -5:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: fichier en entrée", TkUtil::Charset::UTF_8));
    	break;
    case -6:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: fichier en sortie", TkUtil::Charset::UTF_8));
    	break;
    case -13:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: fichier de configuration", TkUtil::Charset::UTF_8));
    	break;
    case -15:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: shéma non trouvé", TkUtil::Charset::UTF_8));
    	break;
    case -22:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: ouverture du fichier en entrée", TkUtil::Charset::UTF_8));
    	break;
    case -23:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: ouverture du fichier en sortie", TkUtil::Charset::UTF_8));
    	break;
    case -24:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: version de la bibliothèque", TkUtil::Charset::UTF_8));
    	break;
    case -25:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: API déjà démarrée", TkUtil::Charset::UTF_8));
    	break;
    case -26:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: API non démarrée", TkUtil::Charset::UTF_8));
    	break;
    case -27:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: version non supportée", TkUtil::Charset::UTF_8));
    	break;
    case -28:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: le fichier n'existe pas", TkUtil::Charset::UTF_8));
    	break;
    case -29:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: lecteur non disponible", TkUtil::Charset::UTF_8));
    	break;
    case -30:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: type de fichier inconnu", TkUtil::Charset::UTF_8));
    	break;
    case -31:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: fichier corrompu", TkUtil::Charset::UTF_8));
    	break;
    case -32:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Avertissement DKOC: pas d'entitée écrite", TkUtil::Charset::UTF_8));
    	break;
    case -33:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: entité corrompu", TkUtil::Charset::UTF_8));
    	break;
    case -34:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: fichier vide", TkUtil::Charset::UTF_8));
    	break;
    case -10:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC/OCC: interruption par une exception", TkUtil::Charset::UTF_8));
    	break;
    case -51:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC/OCC: entitée null", TkUtil::Charset::UTF_8));
    	break;
    case -52:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC/OCC: entitée vide", TkUtil::Charset::UTF_8));
    	break;
    case -1000:
     	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur DKOC: type non supporté", TkUtil::Charset::UTF_8));
     	break;
    default:
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur inconnue lors de l'appel à DKOC", TkUtil::Charset::UTF_8));
    }

#else
    throw TkUtil::Exception (TkUtil::UTF8String ("ImportCATIAImplementation non actif, USE_DKOC à activer", TkUtil::Charset::UTF_8));
#endif
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
