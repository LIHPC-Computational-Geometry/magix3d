#ifndef COMMANDNEWVOLUMEBYBREPIMPORT_H_
#define COMMANDNEWVOLUMEBYBREPIMPORT_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewVolumeByBREPImport
 *  \brief Commande permettant de créer une volume à partir d'une shape OCC importée d'un fichier BREP
 */
/*----------------------------------------------------------------------------*/
class CommandNewVolumeByBREPImport: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param shape la shape OCC à importer
     */
    CommandNewVolumeByBREPImport(Internal::Context& c,
            const TopoDS_Shape& shape);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewVolumeByBREPImport();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

protected:
    /* parametres de l'operation */
    TopoDS_Shape m_shape;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWVERTEXBYBREPIMPORT_H_ */
/*----------------------------------------------------------------------------*/

