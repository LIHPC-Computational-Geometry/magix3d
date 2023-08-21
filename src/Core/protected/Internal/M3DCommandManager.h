/*----------------------------------------------------------------------------*/
/*
 * \file M3DCommandManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#ifndef M3DCOMMANDMANAGER_H_
#define M3DCOMMANDMANAGER_H_

#include "Geom/GeomManager.h"
#include "Topo/TopoManager.h"
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class Volume;
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class M3DCommandManager
 *  \brief Manager de création de commandes de niveau supérieur aux commandes
 *  géométriques ou topologiques (les commandes composites essentiellement)
 */
/*----------------------------------------------------------------------------*/
class M3DCommandManager : public Internal::CommandCreator {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param	Nom à vocation unique de l'instance. Ce nom unique permet, dans
	 *			un contexte distribué, de retrouver la bonne instance sur un
	 *			serveur.
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    M3DCommandManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief création d'une commande longue, pour tester l'interuption
     *
     */
    Mgx3D::Internal::M3DCommandResultIfc* newLongCommand();

    /*------------------------------------------------------------------------*/
    /** \brief création d'une commande longue non inyterruptible (pour tests).
     *
     */
    Mgx3D::Internal::M3DCommandResultIfc* newOpenCascadeLongCommand();

private :
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* M3DCOMMANDMANAGER_H_ */
