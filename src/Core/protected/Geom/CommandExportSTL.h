/*----------------------------------------------------------------------------*/
/*
 * CommandExportSTL.h
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXPORTSTL_H_
#define COMMANDEXPORTSTL_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "Geom/ExportSTLImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportSTL
 *  \brief Commande permettant d'exporter une entité géométrique
 *         dans un fichier au format STL
 */
/*----------------------------------------------------------------------------*/
class CommandExportSTL: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
      *
      *  \param context  le contexte
      *  \param geomEntity l'entité à exporter
      *  \param fileName le nom du fichier de sortie
      */
     CommandExportSTL(Internal::Context& context,
    		 GeomEntity* geomEntity,
    		 const std::string &fileName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportSTL();

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
    ExportSTLImplementation m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXPORTSTL_H_ */
/*----------------------------------------------------------------------------*/
