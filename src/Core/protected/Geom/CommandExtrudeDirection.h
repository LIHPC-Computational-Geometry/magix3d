#ifndef COMMANDEXTRUDEDIRECTION_H_
#define COMMANDEXTRUDEDIRECTION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrusion.h"
#include "Geom/GeomExtrudeImplementation.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
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
    virtual ~CommandExtrudeDirection() = default;

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

private:
    /* Pour caster l'implémentation... */
    GeomExtrudeImplementation* getImpl()
    { return dynamic_cast<GeomExtrudeImplementation*>(m_impl); }

    /* le vecteur de l'extrusion */
    Utils::Math::Vector m_vector;
};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXTRUDEDIRECTION_H_ */
/*----------------------------------------------------------------------------*/
