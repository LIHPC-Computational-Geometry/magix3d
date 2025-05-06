#ifndef GEOMSPLITIMPLEMENTATION_H_
#define GEOMSPLITIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Curve;
class Vertex;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
class GeomSplitImplementation
{
public:
    GeomSplitImplementation(Internal::Context& context);
    ~GeomSplitImplementation() = default;

    #ifndef SWIG
    /*------------------------------------------------------------------------*/
    /** \brief  Décompose la courbe en ses sous-entités.
     */
    void split(Curve* curve, std::vector<Vertex* >& vert);

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose la surface en ses sous-entités.
     */
    virtual void split(Surface* surf, std::vector<Curve* >& curv,
        std::vector<Vertex* >&  vert);

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité en ses sous-entités.
     */
    virtual void split(Volume* vol,
                       std::vector<Surface*>& surf,
                       std::vector<Curve*  >& curv,
                       std::vector<Vertex* >&  vert);
    #endif

private:
    Internal::Context& m_context;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMSPLITIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
