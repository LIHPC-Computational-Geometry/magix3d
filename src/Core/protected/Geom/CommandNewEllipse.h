#ifndef COMMANDNEWELLIPSE_H_
#define COMMANDNEWELLIPSE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewEllipse
 *  \brief Commande permettant de créer une ellipse
 */
/*----------------------------------------------------------------------------*/
class CommandNewEllipse: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur. L'ellipse centrée sur le point center, plan défini 
     *         par center/p1/p2, grand axe défini par center/p1, 
     *         grand rayon défini par la distance center-p1, 
     *         petit rayon défini par la distance p2-axe principal.
     *
     *  \param p1 premier point de l'ellipse
     *  \param p2 second point de l'ellipse
     *  \param center centre de l'ellipse
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewEllipse(Internal::Context& c,
            Geom::Vertex* p1,
            Geom::Vertex* p2,
            Geom::Vertex* center,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewEllipse();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();
    
    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);


protected:
    /* parametres de l'operation */
    Geom::Vertex* m_p1;
    Geom::Vertex* m_p2;
    Geom::Vertex* m_center;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWELLIPSE_H_ */
/*----------------------------------------------------------------------------*/

