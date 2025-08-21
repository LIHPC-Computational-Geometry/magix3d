#ifndef COMMANDREVOLUTION_H_
#define COMMANDREVOLUTION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrusion.h"
#include "Geom/GeomRevolImplementation.h"
#include "Geom/GeomEntity.h"
#include "Utils/Point.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class CommandExtrudeRevolution: public CommandExtrusion {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param entities les entities dont on fait la révolution
     *  \param rot      la rotation a appliquer
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    CommandExtrudeRevolution(Internal::Context& c,
            std::vector<GeomEntity*>& entities,
            const Utils::Math::Rotation& rot, const bool keep);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExtrudeRevolution() = default;

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();


private:
    /** Identifie les courbes sur l'axe et leur ajoute le groupe AXE
     *
     *  Pour cela on identifie les sommets invariants géométriquement
     */
    void addGroupOnAxis();

    /* Pour caster l'implémentation... */
    GeomRevolImplementation* getImpl()
    { return dynamic_cast<GeomRevolImplementation*>(m_impl); }

    /* points définissant l'axe de révolution */
    Utils::Math::Point m_axis1;
    Utils::Math::Point m_axis2;

    /* angle de révolution */
    double m_angle;
};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* COMMANDREVOLUTION_H_ */
/*----------------------------------------------------------------------------*/
