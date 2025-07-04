#ifndef COMMANDEXTRUDEDIRECTION_H_
#define COMMANDEXTRUDEDIRECTION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrusion.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomExtrudeImplementation;
/*----------------------------------------------------------------------------*/
class CommandExtrudeDirection: public CommandExtrusion {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param entities les entities dont on fait l'extrusion
     *  \param dp      le vecteur pour l'extrusion
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    CommandExtrudeDirection(Internal::Context& c,
            std::vector<GeomEntity*>& entities,
    		const Vector& dp, const bool keep);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExtrudeDirection();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

private:


protected:

    /* entités à unir */
    std::vector<GeomEntity*> m_entities;

    /* le vecteur de l'extrusion */
    Utils::Math::Vector m_vector;

    /* objet gérant l'opération d'extrusion */
    GeomExtrudeImplementation* m_impl;

    /* pour conserver les entités de départ ou non*/
    bool m_keep;
};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXTRUDEDIRECTION_H_ */
/*----------------------------------------------------------------------------*/
