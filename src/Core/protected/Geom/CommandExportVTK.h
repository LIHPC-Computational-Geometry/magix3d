#ifndef MGX3D_GEOM_COMMANDEXPORTVTK_H_
#define MGX3D_GEOM_COMMANDEXPORTVTK_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "Geom/ExportVTKImplementation.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandExportVTK
 *  \brief Commande permettant d'exporter l'ensemble des entités géométriques
 *         dans un fichier au format VTK
 */
/*----------------------------------------------------------------------------*/
class CommandExportVTK: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportVTK(Internal::Context& context, const std::string &fileName);

    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param geomEntities les entités à exporter
     *  \param fileName le nom du fichier de sortie
     */
    CommandExportVTK(Internal::Context& context,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string &fileName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExportVTK();

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
    ExportVTKImplementation m_impl;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_COMMANDEXPORTVTK_H_ */
/*----------------------------------------------------------------------------*/
