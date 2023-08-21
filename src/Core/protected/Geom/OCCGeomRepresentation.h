/*----------------------------------------------------------------------------*/
/** \file OCCGeomRepresentation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 19/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_OCCGEOMREPRESENTATION_H_
#define MGX3D_GEOM_OCCGEOMREPRESENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomRepresentation.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TDF_Label.hxx>
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
class OCCGeomRepresentation: public GeomRepresentation{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    OCCGeomRepresentation(Internal::Context& c, const TopoDS_Shape& shape);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur par copie
     */
    OCCGeomRepresentation(const OCCGeomRepresentation& rep);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~OCCGeomRepresentation();

    /*------------------------------------------------------------------------*/
    /** Cas d'utilisation de OCAF
     */
    bool useOCAF() const;

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
    /** \brief  Compare 2 courbes OCC pour voir si la seconde est incluse
     *          dans la premiere
     *
     * @param e1 courbe de référence
     * @param e2 courbe incluse dans e1?
     * @return
     */
    static bool isIn(const TopoDS_Edge& eComposite, const TopoDS_Edge& eComponent);

    /*------------------------------------------------------------------------*/
    /** \brief  Compare 2 sommet OCC.
     */
    static bool areEquals(const TopoDS_Vertex& v1, const TopoDS_Vertex& v2);

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    double computeVolumeArea();
    double computeSurfaceArea();
    double computeCurveArea();

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    void computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax,
                            double tol=0.0) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité volumique en ses sous-entités.
     */
    void split(std::vector<Surface* >& surf,
               std::vector<Curve*>& curv,
               std::vector<Vertex* >&  vert,
               Volume* owner);

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité surfacique en ses sous-entités.
     */
    void split(std::vector<Curve*>& curv,
               std::vector<Vertex* >&  vert,
               Surface* owner);

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité linéaire en ses sous-entités.
     */
    void split( std::vector<Vertex* >&  vert,
                Curve* owner);

    /*------------------------------------------------------------------------*/
    /** \brief  Effectue l'ensemble des connectivités topologiques entre l'objet
     *          volumique M3D volM3D et ses sous-entités contenues dans
     *          verticesM3D, curvesM3D, surfacesM3D, à l'aide du modèle OCC
     *          contenus dans volOCC, verticesOCC, curvesOCC, surfacesOCC. Pour
     *          cela, il faut qu'il existe une correspondance entre les indices
     *          des entités M3D et OCC. Par exemple, l'entité M3D surfacesM3D[i]
     *          doit avoir pour représentation OCC celle contenue dans
     *          surfacesOCC[i].
     */
    static void connectTopology( Volume* volM3D, TopoDS_Shape& volOCC,
                std::vector<Vertex*>&  verticesM3D,
                std::vector<TopoDS_Shape>& verticesOCC,
                std::vector<Curve*>&   curvesM3D  ,
                std::vector<TopoDS_Shape>& curvesOCC,
                std::vector<Surface*>& surfacesM3D,
                std::vector<TopoDS_Shape>& surfacesOCC);

    static void connectTopology( Volume* volM3D,
                std::vector<Vertex*>&  verticesM3D,
                std::vector<Curve*>&   curvesM3D  ,
                std::vector<Surface*>& surfacesM3D);

    /*------------------------------------------------------------------------*/
    /** \brief  Effectue l'ensemble des connectivités topologiques entre l'objet
     *          surfacique M3D surfM3D et ses sous-entités contenues dans
     *          verticesM3D, curvesM3D, à l'aide du modèle OCC
     *          contenus dans surfOCC, verticesOCC, curvesOCC. Pour
     *          cela, il faut qu'il existe une correspondance entre les indices
     *          des entités M3D et OCC. Par exemple, l'entité M3D curvesM3D[i]
     *          doit avoir pour représentation OCC celle contenue dans
     *          curvesOCC[i].
     */
    static void connectTopology( Surface* surfM3D, TopoDS_Shape& surfOCC,
                std::vector<Vertex*>&  verticesM3D,
                std::vector<TopoDS_Shape>& verticesOCC,
                std::vector<Curve*>&   curvesM3D  ,
                std::vector<TopoDS_Shape>& curvesOCC);

    /*------------------------------------------------------------------------*/
    /** \brief  Effectue l'ensemble des connectivités topologiques entre l'objet
     *          linéarie M3D curveM3D et ses sous-entités contenues dans
     *          verticesM3D à l'aide du modèle OCC
     *          contenus dans verticesOCC. Pour
     *          cela, il faut qu'il existe une correspondance entre les indices
     *          des entités M3D et OCC. Par exemple, l'entité M3D verticesM3D[i]
     *          doit avoir pour représentation OCC celle contenue dans
     *          verticesOCC[i].
     */
    static void connectTopology( Curve* curveOCC, TopoDS_Shape& curve0CC,
                std::vector<Vertex*>&  verticesM3D,
                std::vector<TopoDS_Shape>& verticesOCC);
    /*------------------------------------------------------------------------*/
     /** \brief  Return true si la shape sh contient la shape shOther a epsilon
      *          pres
      */
    static bool contains(const TopoDS_Shape& sh,const TopoDS_Shape& shOther);

    /*------------------------------------------------------------------------*/
    /** \brief  Effectue le nettoyage topologique de la shape occ sh passée en
     *          paramètre
     */
    static TopoDS_Shape cleanShape(TopoDS_Shape& sh);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur l'entité géométrique associée.
     *
     *  \param P le point à projeter qui sera modifié
     *  \param C la courbe sur laquelle on projette
     */
    void project(Utils::Math::Point& P,const Curve* C);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur l'entité géométrique associée.
     *
     *  \param P le point à projeter qui sera modifié
     *  \param S la surface sur laquelle on projette
     */
    void project(Utils::Math::Point& P, const Surface* S);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur l'entité géométrique associée.
     *         Le résultat est le point P2.
     *
     *  \param P1 le point à projeter
     *  \param P2 le résultat de la projection
     *  \param S la surface sur laquelle on projete
     */
    void project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Surface* S);

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur l'entité géométrique associée.
     *         Le résultat est le point P2.
     *
     *  \param P1 le point à projeter
     *  \param P2 le résultat de la projection
     *  \param C la courbe sur laquelle on projete
     */
    void project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Curve* C);

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la normale à une surface en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la normale recherchée suivant le projeté du point
     *  \param S la surface sur laquelle on projete
     */
    void normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S);

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la tangente à une courbe en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la tangente recherchée suivant le projeté du point
     */
    void tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2);

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point en fonction du paramètre sur la courbe
     * \param p le paramètre curviligne
     * \param Pt le résultat
     */
    void getPoint(const double& p, Utils::Math::Point& Pt,
                    const bool inO1 = false);

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point a l'intersection avec le plan
     * \param plan_cut le plan
     * \param Pt le résultat
     */
    void getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt);

    /*------------------------------------------------------------------------*/
    /** \brief Donne le paramètre curviligne du point sur la courbe
     * \param Pt le point sur la courbe
     */
    void getParameter(const Utils::Math::Point& Pt, double& p, const Curve* C);

    /*------------------------------------------------------------------------*/
    /** \brief Donne les paramètres curviligne aux extrémités de la courbe
     */
    void getParameters(double& first, double& last);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la translation de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param V le vecteur de translation à appliquer
     */
    void translate(const Utils::Math::Vector& V);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue l'homothétié de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param F le facteur d'homothétie
     *  \param center le centre d'homothétie
     */
    void scale(const double F, const Point& center);
    void scale(const double factorX,
                const double factorY,
                const double factorZ);
    /*------------------------------------------------------------------------*/
    /** \brief Effectue la rotation de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param P1    premier point definissant l'axe de rotation
     *  \param P2    second point definissant l'axe de rotation
     *  \param Angle    angle de rotation en degré (compris entre ]0,360])
     */
    void rotate(const Utils::Math::Point& P1,
            const Utils::Math::Point& P2, double Angle);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la symétrie de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param plane le plan de symétrie
     */
    virtual void mirror(const Utils::Math::Plane& plane);

//    /*------------------------------------------------------------------------*/
//    /** \brief Met à jour la représentation géométrique. Pour cela on doit avoir
//     *         accès aux entités géométriques M3D impliquées, d'où le paramètre
//     *         de type GeomEntity
//     *
//     *  \param entity l'entité géométrique que l'on met à jour
//     */
//    void updateShape(GeomEntity* e);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation affichable de l'entité en se basant
     *          sur la représentation passée en paramètre
     *
     *  \param dr la représentation que l'on demande à afficher
     *  \param caller un pointeur sur la géométrie appelant cette méthode. Il
     *         est nécessaire à cause de la discrétisation de courbes qui
     *         peuvent nécessiter d'accéder à des entités de dimension
     *         supérieure.
     */
    void buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
                                    const GeomEntity* caller) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation facétisée (point, segments, polygones) de l'entité
     *
     */
    virtual void getFacetedRepresentation(
            std::vector<gmds::math::Triangle >& AVec,
            const GeomEntity* caller) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Renvoie si la surface AEntityOrientation est bien orientée
     * 			par rapport au volume ACaller.
     *
     */
    virtual void facetedRepresentationForwardOrient(
    		const GeomEntity* ACaller,
    		const GeomEntity* AEntityOrientation,
    		std::vector<gmds::math::Triangle >*) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit un clone de l'instance de représentation géométrique.
     *          Pour cela, un appel à "new" est effectué dans cette commande.
     *
     *  \return un pointeur vers une représentation géométrique identique
     */
    GeomRepresentation* clone() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accès à la shape OCC sous-jacente.
     */
  //  TopoDS_Shape const& getShape() const;
    TopoDS_Shape getShape();

    /** \brief Accès au label OCAF
     */
    TDF_Label getLabel() const;

    /// changement de label racine
    static void setRootLabel(TDF_Label label) {OCCGeomRepresentation::m_rootLabel = label;}

    /*------------------------------------------------------------------------*/
    /** \brief  Accès à la shape OCC sous-jacente via un pointeur.
     */
    TopoDS_Shape const* getShapePtr() const {return &m_shape;}


    /*------------------------------------------------------------------------*/
    /** \brief  Accès à la shape OCC sous-jacente via un pointeur.
     */
    void setShape(const TopoDS_Shape& sh)  {m_shape=sh;}


    /*------------------------------------------------------------------------*/
    /** \brief  De nombreuses opération nécessitent de créer le maillage OCC
     *          associé à une entité OCC. Cette méthode factorise ce
     *          traitement.
     */
    static double buildIncrementalBRepMesh(TopoDS_Shape& shape, const double& deflection);

    static double getParameterOnTopoDSEdge(const TopoDS_Edge& edge,
            const Utils::Math::Point& Pt, double& p);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit la précision
     *
     *  \return un double (en général de l'ordre de e-5)
     */
   virtual double getPrecision();


protected:
    static void connectV2C(TopoDS_Shape& shape,
            std::vector<Vertex*>&  verticesM3D,  std::vector<TopoDS_Shape>& verticesOCC,
            std::vector<Curve*>&  curvesM3D  ,  std::vector<TopoDS_Shape>& curvesOCC);

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

    /*------------------------------------------------------------------------*/
    /** \brief Crée une surface à partir de la face OCC passée en argument
     *
     *  \param f une face OCC
     */
    Surface* newOCCSurface(TopoDS_Face& f);

    /*------------------------------------------------------------------------*/
    /** \brief Crée une courbe à partir de l'arête OCC passée en argument
     *
     *  \param e une arête OCC
     */
    Curve* newOCCCurve(TopoDS_Edge& f);

    /*------------------------------------------------------------------------*/
    /** \brief Crée un sommet à partir du sommet OCC passée en argument
     *
     *  \param occ_v un sommet occ
     */
    Vertex* newOCCVertex(TopoDS_Vertex& occ_v);

//    void updateFace(GeomEntity* e);
//    void updateCurve(GeomEntity* e);
//    void updateVolume(GeomEntity* e);

    void projectPointOn( Utils::Math::Point& P);



protected:


    Internal::Context & m_context;

    TopoDS_Shape m_shape;

    /// label pour OCAF
    TDF_Label m_label;

    /// le label pour ajouter les construction suivantes
    static TDF_Label m_rootLabel;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_OCCGEOMREPRESENTATION_H_ */
/*----------------------------------------------------------------------------*/




