/*----------------------------------------------------------------------------*/
#ifndef GEOMCONTAINSIMPLEMENTATION_H_
#define GEOMCONTAINSIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
class GeomContainsImplementation
{
public:
    GeomContainsImplementation() = default;
    ~GeomContainsImplementation() = default;

    /*------------------------------------------------------------------------*/
    /** \brief Retourne true si la courbe c1 contient le volume c2
     */
    bool contains(Curve* c1, Curve* c2) const;

    /*------------------------------------------------------------------------*/
    /** \brief Retourne true si le surface s1 contient la surface s2
     */
    bool contains(Surface* s1, Surface* s2) const;

    /*------------------------------------------------------------------------*/
    /** \brief Retourne true si le volume v1 contient le volume v2
     */
    bool contains(Volume* v1, Volume* v2) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Return true si la shape sh contient la shape shOther 
     */
     bool contains(const TopoDS_Shape& sh, const TopoDS_Shape& shOther) const;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMCONTAINSIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
