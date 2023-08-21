/*----------------------------------------------------------------------------*/
/** \file CommandNewFacetedSurface.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 26/02/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef M3D_COMMANDNEWFACETEDSURFACES_H_
#define M3D_COMMANDNEWFACETEDSURFACES_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewFacetedSurfaces
 *  \brief Commande permettant de créer une ou plusieurs surfaces facétisées à partir
 *  d'un fichier de maillage
 */
/*----------------------------------------------------------------------------*/
class CommandNewFacetedSurfaces: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param nom du fichier lu
     */
    CommandNewFacetedSurfaces(  Internal::Context& c,
    		std::string& nom);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewFacetedSurfaces();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /// nom du fichier pour la lecture
    std::string m_file_name;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* M3D_COMMANDNEWFACETEDSURFACES_H_ */
/*----------------------------------------------------------------------------*/

