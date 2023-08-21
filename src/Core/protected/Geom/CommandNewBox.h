/*----------------------------------------------------------------------------*/
/** \file CommandNewBox.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWBOX_H_
#define COMMANDNEWBOX_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewBox
 *  \brief Commande permettant de créer une boite
 */
/*----------------------------------------------------------------------------*/
class CommandNewBox: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param pmin le point min de la boite
     *  \param pmax le point max de la boite
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewBox(Internal::Context& c,
            const Utils::Math::Point& pmin,
            const Utils::Math::Point& pmax,
            const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewBox();

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
    Utils::Math::Point m_pmin;
    Utils::Math::Point m_pmax;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWBOX_H_ */
/*----------------------------------------------------------------------------*/

