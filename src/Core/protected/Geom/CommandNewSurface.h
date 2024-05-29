/*----------------------------------------------------------------------------*/
/** \file CommandNewSurface.h
 *
 *  \author Franck Ledoux
 *
 *  \date 31/01/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef M3D_COMMANDNEWSURFACE_H_
#define M3D_COMMANDNEWSURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandCreateGeom;
/*----------------------------------------------------------------------------*/
/** \class CommandNewSurface
 *  \brief Commande permettant de créer une surface à partir d'un ensemble de
 *         segments définissant le bord de la surface
 */
/*----------------------------------------------------------------------------*/
class CommandNewSurface: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param curves les courbes définissant le bord de la surface
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewSurface(  Internal::Context& c,
                        const std::vector<Geom::Curve*>& curves,
                        const std::string& groupName);

    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmds les commandes de création des courbes définissant le bord de la surface
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewSurface(  Internal::Context& c,
                        const std::vector<Geom::CommandCreateGeom*>& cmds,
                        const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewSurface();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /* parametres de l'operation */
    std::vector<Geom::Curve*> m_curves;

    /* commandes de création des objets qui servent à l'opération*/
    std::vector<Geom::CommandCreateGeom*> m_cmds_curves;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWSURFACE_H_ */
/*----------------------------------------------------------------------------*/

