/*----------------------------------------------------------------------------*/
/** \file CommandNewBox.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWVERTEX_H_
#define COMMANDNEWVERTEX_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewVertex
 *  \brief Commande permettant de créer une sommet
 */
/*----------------------------------------------------------------------------*/
class CommandNewVertex: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param p le point géométrique
     *  \param pmax le point max de la boite
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewVertex(Internal::Context& c,
            const Utils::Math::Point& p,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewVertex();

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
    Utils::Math::Point m_p;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWVERTEX_H_ */
/*----------------------------------------------------------------------------*/

