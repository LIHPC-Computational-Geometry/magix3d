#ifndef MGX3D_GEOM_OCCHELPER_H_
#define MGX3D_GEOM_OCCHELPER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class OCCHelper
 *
 * \brief Classe regroupant des fonctions statiques utilitaires OCC
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

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point en fonction du paramètre sur la courbe
     * \param edge l'arête représentant la courbe
     * \param p le paramètre curviligne
     * \param Pt le résultat
     */
    static void getPoint(TopoDS_Edge& edge, const double& p, Utils::Math::Point& Pt, const bool in01 = false);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur la shape, le résultat est le point P2.
     *  \return l'indice de la shape qui a donné la meilleure projection
     */
    static uint project(const std::vector<TopoDS_Shape>& shapes, const Utils::Math::Point& P1, Utils::Math::Point& P2);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur la shape. P est modifié
     *  \param P le point à projeter
     */
    static uint project(const std::vector<TopoDS_Shape>& shapes, Utils::Math::Point& P);

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la tangente à une courbe en un point
     *
     *  \param edge l'arête représentant la courbe
     *  \param P1 le point à projeter
     *  \param V2 la tangente recherchée suivant le projeté du point
     */
    static void tangent(const TopoDS_Edge& edge, const Utils::Math::Point& P1, Utils::Math::Vector& V2);

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point a l'intersection avec le plan
     *  \param edge l'arête représentant la courbe
     *  \param plan_cut le plan
     *  \param Pt le résultat
     */
    static void getIntersection(const TopoDS_Edge& edge, gp_Pln& plan_cut, Utils::Math::Point& Pt);

    /*------------------------------------------------------------------------*/
    /** \brief Donne le paramètre curviligne du point sur la courbe
     * \param Pt le point sur la courbe
     */
    static double getParameter(const TopoDS_Edge& edge, const Utils::Math::Point& Pt, double& p);
    static void getParameters(const TopoDS_Edge& edge, double& first, double& last);

    /// Teste le type de la courbe
    static bool isTypeOf(const TopoDS_Edge& edge, const Handle_Standard_Type& type);

    /// Teste le type de la surfave
    static bool isTypeOf(const TopoDS_Face& edge, const Handle_Standard_Type& type);

    /// Retourne la longueur de la courbe
    static double getLength(const TopoDS_Edge& edge);

    /// Retourne l'aire de la surface
    static double computeArea(const TopoDS_Face& face);

    /// Retourne l'aire du volume
    static double computeArea(const TopoDS_Shape& volume);

    /// Calcule la boite englobante
    static void computeBoundingBox(const TopoDS_Shape& shape, Utils::Math::Point& pmin, Utils::Math::Point& pmax, double tol=0.0);

    /*------------------------------------------------------------------------*/
     /** \brief  Return true si la shape sh contient la shape shOther a epsilon
      *          pres
      */
    static bool contains(const TopoDS_Shape& sh, const TopoDS_Shape& shOther);

    /*------------------------------------------------------------------------*/
    /** \brief  Effectue le nettoyage topologique de la shape occ sh passée en
     *          paramètre
     */
    static TopoDS_Shape cleanShape(TopoDS_Shape& sh);

private:
    /*------------------------------------------------------------------------*/
    /** \brief  Projette le point P sur la shape OCC
     */
    static void projectPointOn(const TopoDS_Shape& shape, Utils::Math::Point& P);

    /*------------------------------------------------------------------------*/
    /** \brief  construction de map pour le nettoyage
     */
    static void addShapeToLists(TopoDS_Shape& shape,
        TopTools_IndexedMapOfShape& fmap,
        TopTools_IndexedMapOfShape& emap,
        TopTools_IndexedMapOfShape& vmap,
        TopTools_IndexedMapOfShape& somap,
        TopTools_IndexedMapOfShape& shmap,
        TopTools_IndexedMapOfShape& wmap);
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_OCCHELPER_H_ */
/*----------------------------------------------------------------------------*/