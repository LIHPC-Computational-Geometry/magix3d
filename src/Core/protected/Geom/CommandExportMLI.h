/*----------------------------------------------------------------------------*/
/*
 * CommandExportMLI.h
 *
 *  Created on: 30 sep. 2016
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_COMMANDEXPORTMLI_H_
#define MGX3D_GEOM_COMMANDEXPORTMLI_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "Geom/ExportMLIImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportMLI
 *  \brief Commande permettant d'exporter l'ensemble des entités géométriques
 *         dans un fichier au format Mli
 */
/*----------------------------------------------------------------------------*/
class CommandExportMLI: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportMLI(Internal::Context& context, const std::string &fileName);

    /** \brief  Constructeur
      *
      *  \param context  le contexte
      *  \param geomEntities les entités à exporter
      *  \param fileName le nom du fichier de sortie
      */
     CommandExportMLI(Internal::Context& context,
    		 const std::vector<GeomEntity*>& geomEntities,
    		 const std::string &fileName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportMLI();

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
    ExportMLIImplementation m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_COMMANDEXPORTMLI_H_ */
/*----------------------------------------------------------------------------*/
