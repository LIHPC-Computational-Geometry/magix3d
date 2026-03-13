#ifndef COMMANDNEWSURFACEBYBREPIMPORT_H_
#define COMMANDNEWSURFACEBYBREPIMPORT_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewSurfaceByBREPImport
 *  \brief Commande permettant de créer une surface
 */
/*----------------------------------------------------------------------------*/
class CommandNewSurfaceByBREPImport: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param shape la shape OCC à importer
     */
    CommandNewSurfaceByBREPImport(Internal::Context& c,
            const std::vector<TopoDS_Face>& shapes);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewSurfaceByBREPImport();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

protected:
    /* parametres de l'operation */
    std::vector<TopoDS_Face> m_faceShapes;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWSURFACEBYBREPIMPORT_H_ */
/*----------------------------------------------------------------------------*/

