/*----------------------------------------------------------------------------*/
/*
 * CommandExportIGES.h
 *
 *  Created on: 14 févr. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXPORTIGES_H_
#define COMMANDEXPORTIGES_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "Geom/ExportIGESImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportIGES
 *  \brief Commande permettant d'exporter l'ensemble des entités géométriques
 *         dans un fichier au format IGES
 */
/*----------------------------------------------------------------------------*/
class CommandExportIGES: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportIGES(Internal::Context& context, const std::string &fileName);

    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param geomEntities les entités à exporter
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportIGES(Internal::Context& context,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string &fileName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportIGES();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo(){}

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo(){}


protected:

    /* objet gérant l'opération d'exportation */
    ExportIGESImplementation m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXPORTIGES_H_ */
/*----------------------------------------------------------------------------*/

