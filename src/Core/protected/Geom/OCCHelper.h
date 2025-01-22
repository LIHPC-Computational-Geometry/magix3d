#ifndef MGX3D_GEOM_OCCHELPER_H_
#define MGX3D_GEOM_OCCHELPER_H_
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Solid.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class OCCGeomRepresentation
 *
 * \brief Calcul des propriétés géométriques d'un objet géométrique lorsqu'il
 *        connait OCC
 */
class OCCHelper
{
public:    
    /*------------------------------------------------------------------------*/
    /** \brief  Compare topologiquement 2 shapes OCC de manière générale. A
     *          utiliser si possible via les méthodes areEquals uniquement.
     */
    static bool areSame(const TopoDS_Shape& sh1,const TopoDS_Shape& sh2);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 volumes OCC en comparant géométriquement leurs bords.
     */
    static bool areEquals(const TopoDS_Solid& s1, const TopoDS_Solid& s2);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 surfaces OCC en comparant géométriquement leurs bords.
     */
    static bool areEquals(const TopoDS_Face& f1, const TopoDS_Face& f2);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 courbes OCC en comparant géométriquement leurs bords.
     */
    static bool areEquals(const TopoDS_Edge& e1, const TopoDS_Edge& e2);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 wires OCC
     */
    static bool areEquals(const TopoDS_Wire& e1, const TopoDS_Wire& e2);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 sommet OCC.
     */
    static bool areEquals(const TopoDS_Vertex& v1, const TopoDS_Vertex& v2);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 courbes OCC pour voir si la seconde est incluse
     *          dans la premiere
     *
     * @param e1 courbe de référence
     * @param e2 courbe incluse dans e1?
     * @return
     */
    static bool isIn(const TopoDS_Edge& eComposite, const TopoDS_Edge& eComponent);

    /*------------------------------------------------------------------------*/
    /** \brief  De nombreuses opération nécessitent de créer le maillage OCC
     *          associé à une entité OCC. Cette méthode factorise ce
     *          traitement.
     */
    static double buildIncrementalBRepMesh(TopoDS_Shape& shape, const double& deflection);

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_OCCHELPER_H_ */
/*----------------------------------------------------------------------------*/