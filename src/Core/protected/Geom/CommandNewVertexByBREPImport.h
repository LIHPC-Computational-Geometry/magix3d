#ifndef COMMANDNEWVERTEXBYBREPIMPORT_H_
#define COMMANDNEWVERTEXBYBREPIMPORT_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewVertexByBREPImport
 *  \brief Commande permettant de créer une sommet
 */
/*----------------------------------------------------------------------------*/
class CommandNewVertexByBREPImport: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param shape la shape OCC à importer
     */
    CommandNewVertexByBREPImport(Internal::Context& c,
            const TopoDS_Vertex& shape);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewVertexByBREPImport();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

protected:
    /* parametres de l'operation */
    TopoDS_Vertex m_vertexShape;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWVERTEXBYBREPIMPORT_H_ */
/*----------------------------------------------------------------------------*/

