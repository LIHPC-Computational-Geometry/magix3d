#ifndef COMMANDNEWCURVEBYBREPIMPORT_H_
#define COMMANDNEWCURVEBYBREPIMPORT_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewCurveByBREPImport
 *  \brief Commande permettant de créer une sommet
 */
/*----------------------------------------------------------------------------*/
class CommandNewCurveByBREPImport: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param shape la shape OCC à importer
     */
    CommandNewCurveByBREPImport(Internal::Context& c,
            const std::vector<TopoDS_Edge>& shapes,
            const Utils::Math::Point& extremaFirst, const Utils::Math::Point& extremaLast);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewCurveByBREPImport();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

protected:
    /* parametres de l'operation */
    std::vector<TopoDS_Edge> m_edgeShapes;
    Utils::Math::Point m_extremaFirst;
    Utils::Math::Point m_extremaLast;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWVERTEXBYBREPIMPORT_H_ */
/*----------------------------------------------------------------------------*/

