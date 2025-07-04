#ifndef COMMANDCOMMON_H_
#define COMMANDCOMMON_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
class GeomEntity;
/*----------------------------------------------------------------------------*/
/** \class CommandCommon
 *  \brief Commande permettant de faire l'intersection Booléenne de 2 géométries 3D
 */
/*----------------------------------------------------------------------------*/
class CommandCommon: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités que l'on intersecte
     */
    CommandCommon(Internal::Context& c,std::vector<GeomEntity*> e);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandCommon();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();



protected:

    void validate();

    /* entités à unir */
    std::vector<GeomEntity*> m_entities;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCOMMON_H_ */
/*----------------------------------------------------------------------------*/



